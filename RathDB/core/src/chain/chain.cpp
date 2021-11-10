#include <chain.h>
#include <rathcrypto.h>
#include <algorithm>
#include <vector>

Chain::Chain() {
    _active_chain_length = 1;
    _active_chain_last_block = construct_genesis_block();
    _block_info_database = std::make_unique<BlockInfoDatabase>();
    _chain_writer = std::make_unique<ChainWriter>();
    _coin_database = std::make_unique<CoinDatabase>();

    std::unique_ptr<UndoBlock> undo_gen = _coin_database->make_undo_block(*_active_chain_last_block);

    std::unique_ptr<BlockRecord> block_record = _chain_writer->store_block(*_active_chain_last_block, *undo_gen, 1);
    uint32_t bl_hash = RathCrypto::hash(Block::serialize(*_active_chain_last_block));
    _block_info_database->store_block_record(bl_hash, *block_record);

    std::vector<std::unique_ptr<Transaction>> transactions;
    for (const std::unique_ptr<Transaction>& tx : _active_chain_last_block->transactions){
        std::vector<std::unique_ptr<TransactionInput>> txinpt;
        std::vector<std::unique_ptr<TransactionOutput>> txoutpt;
        for (const std::unique_ptr<TransactionInput>& txi : tx->transaction_inputs) {
            txinpt.push_back(std::make_unique<TransactionInput>(txi->reference_transaction_hash,txi->utxo_index,txi->signature));
        }
        for (const std::unique_ptr<TransactionOutput>& txo : tx->transaction_outputs) {
            txoutpt.push_back(std::make_unique<TransactionOutput>(txo->amount,txo->public_key));
        }
        transactions.push_back(std::make_unique<Transaction>(std::move(txinpt), std::move(txoutpt), tx->version, tx->lock_time));
    }

    _coin_database->store_block(std::move(transactions));
}

void Chain::handle_block(std::unique_ptr<Block> block) {
    uint32_t last_block_hash = RathCrypto::hash(Block::serialize(*_active_chain_last_block));
    uint32_t bl_hash = RathCrypto::hash(Block::serialize(*block));
    if (block->block_header->previous_block_hash == last_block_hash) {
        // append to active chain
        // copy transactions
        std::vector<std::unique_ptr<Transaction>> transactions = block->get_transactions();
        // validate and store in coin database
        if (_coin_database->validate_and_store_block(std::move(transactions))) {
            // write to disk
            uint32_t height = _active_chain_length + 1;
            std::unique_ptr<UndoBlock> undo_block = _coin_database->make_undo_block(*block);
            std::unique_ptr<BlockRecord> bl_record = _chain_writer->store_block(*block, *undo_block, height);
            _block_info_database->store_block_record(bl_hash, *bl_record);
            // update fields
            _active_chain_length++;
            _active_chain_last_block = std::move(block);
        }
    } else {
        // append to forked chain
        // write to disk
        uint32_t previous_block_hash = block->block_header->previous_block_hash;
        std::unique_ptr<BlockRecord> prev_bl_record = _block_info_database->get_block_record(previous_block_hash);
        uint32_t height = prev_bl_record->height + 1;
        std::unique_ptr<UndoBlock> undo_block = _coin_database->make_undo_block(*block);
        std::unique_ptr<BlockRecord> bl_record = _chain_writer->store_block(*block, *undo_block, height);
        _block_info_database->store_block_record(bl_hash, *bl_record);

        // switch to new branch if necessary
        if (height > _active_chain_length) {
            uint32_t bl_hash = RathCrypto::hash(Block::serialize(*block));
            std::vector<std::shared_ptr<Block>> forked_blocks = get_forked_blocks_stack(bl_hash);
            uint32_t ancestor_hash = RathCrypto::hash((Block::serialize(*forked_blocks.back())));
            std::vector<std::unique_ptr<UndoBlock>> undo_blocks = get_undo_blocks_queue(ancestor_hash);
            // "undo" blocks
            _coin_database->undo_coins(std::move(undo_blocks));
            // "do" blocks
            for(int i = forked_blocks.size() - 1; i >= 0; i--) {
                _coin_database->store_block(std::move(forked_blocks[0]->transactions));
            }
            // update fields
            _active_chain_length = height;
            _active_chain_last_block = std::move(block);
        }
    }
}

void Chain::handle_transaction(std::unique_ptr<Transaction> transaction) {
    _coin_database->validate_and_store_transaction(std::move(transaction));
}

uint32_t Chain::get_chain_length(uint32_t block_hash) {
    if(_block_info_database->get_block_record(block_hash)->num_transactions != 0) {
        return _block_info_database->get_block_record(block_hash)->height;
    }
    return 0;
}

std::unique_ptr<Block> Chain::get_block(uint32_t block_hash) {
    std::unique_ptr<BlockRecord> bl_record = _block_info_database->get_block_record(block_hash);
    if (bl_record->block_file_stored == ""){
        return nullptr;
    }
    FileInfo file_info = FileInfo(bl_record->block_file_stored,
                                 bl_record->block_offset_start,
                                 bl_record->block_offset_end);
    return Block::deserialize(_chain_writer->read_block(file_info));
}

std::vector<std::unique_ptr<Block>> Chain::get_active_chain(uint32_t start, uint32_t end) {
    if(start > end) {
        return std::vector<std::unique_ptr<Block>>();
    }
    if(start == 0) {
        start = 1;
    }
    if(end > _active_chain_length) {
        end = _active_chain_length;
    }

    int current = _active_chain_length;
    uint32_t traversing_block_hash = RathCrypto::hash(Block::serialize(*_active_chain_last_block));
    std::vector<std::unique_ptr<Block>> output;

    while (current >= start) {
        if (current <= end) {
            /// why push back unique ptr
            output.push_back(get_block(traversing_block_hash));
        }
        traversing_block_hash = _block_info_database->get_block_record(traversing_block_hash)->block_header->previous_block_hash;
        current -= 1;
    }
    std::reverse(output.begin(), output.end());

    return std::move(output);
}

std::vector<uint32_t> Chain::get_active_chain_hashes(uint32_t start, uint32_t end) {
    if(start > end) {
        return std::vector<uint32_t>();
    }
    if(start == 0) {
        start = 1;
    }
    if(end > _active_chain_length) {
        end = _active_chain_length;
    }

    int current = _active_chain_length;
    uint32_t traversing_block_hash = RathCrypto::hash(Block::serialize(*_active_chain_last_block));
    std::vector<uint32_t> output;

    while (current >= start) {
        if (current <= end) {
            output.push_back(traversing_block_hash);
        }
        traversing_block_hash = _block_info_database->get_block_record(traversing_block_hash)->block_header->previous_block_hash;
        current -= 1;
    }
    std::reverse(output.begin(), output.end());
    return output;
}

std::unique_ptr<Block> Chain::get_last_block() {
    // copy transactions
    std::vector<std::unique_ptr<Transaction>> transactions = _active_chain_last_block->get_transactions();
    return std::make_unique<Block>(std::make_unique<BlockHeader>(*_active_chain_last_block->block_header),
            std::move(transactions));
}

uint32_t Chain::get_last_block_hash() {
    return RathCrypto::hash(Block::serialize(*_active_chain_last_block)); // hash
}

uint32_t Chain::get_active_chain_length() const {
    return _active_chain_length;
}

std::unique_ptr<Block> Chain::construct_genesis_block(){
    std::unique_ptr<BlockHeader> header = std::make_unique<BlockHeader>(1, 0, 0, 0, 0, 0);

    std::vector<std::unique_ptr<TransactionInput>> txi;
    std::vector<std::unique_ptr<TransactionOutput>> txo;
    txo.push_back(std::make_unique<TransactionOutput> (100, 12345));
    txo.push_back(std::make_unique<TransactionOutput> (200, 67891));
    txo.push_back(std::make_unique<TransactionOutput> (300, 23456));
    std::vector<std::unique_ptr<Transaction>> transactions;
    transactions.push_back(std::make_unique<Transaction>(std::move(txi), std::move(txo)));

    return std::make_unique<Block>(std::move(header), std::move(transactions));
}

std::vector<std::unique_ptr<UndoBlock>> Chain::get_undo_blocks_queue(uint32_t branching_height) {
    if (branching_height == 0 || branching_height > _active_chain_length) {
        return std::vector<std::unique_ptr<UndoBlock>>();
    }

    int current = _active_chain_length;
    uint32_t traversing_block_hash = RathCrypto::hash(Block::serialize(*_active_chain_last_block));
    std::vector<std::unique_ptr<UndoBlock>> output;
    while (current >= branching_height) {
        std::unique_ptr<BlockRecord> bl_record = _block_info_database->get_block_record(traversing_block_hash);
        FileInfo fileInfo = FileInfo(bl_record->undo_file_stored,
                                     bl_record->undo_offset_start, bl_record->undo_offset_end);
        output.push_back(UndoBlock::deserialize(_chain_writer->read_undo_block(fileInfo)));
        traversing_block_hash = bl_record->block_header->previous_block_hash;
    }
    return output;
}

std::vector<std::shared_ptr<Block>> Chain::get_forked_blocks_stack(uint32_t starting_hash) {
    uint32_t forked_traversing_hash = starting_hash;
    uint32_t forked_chain_height = _block_info_database->get_block_record(starting_hash)->height;
    uint32_t active_traversing_hash = get_last_block_hash();
    uint32_t active_chain_height = _active_chain_length;
    std::vector<std::shared_ptr<Block>> output;
    while (forked_chain_height > active_chain_height) {
        std::unique_ptr<BlockRecord> bl_record = _block_info_database->get_block_record(forked_traversing_hash);
        std::unique_ptr<Block> block = Block::deserialize(
                _chain_writer->read_block(FileInfo(bl_record->block_file_stored,
                                                   bl_record->block_offset_start,
                                                   bl_record->block_offset_end)));
        output.push_back(std::move(block));
        forked_traversing_hash = bl_record->block_header->previous_block_hash;
        forked_chain_height -= 1;
    }
    while (forked_chain_height < active_chain_height) {
        active_traversing_hash = _block_info_database->get_block_record(active_traversing_hash)->block_header->previous_block_hash;
        active_chain_height -= 1;
    }
    while (forked_traversing_hash != active_traversing_hash) {
        std::unique_ptr<BlockRecord> bl_record = _block_info_database->get_block_record(forked_traversing_hash);
        std::unique_ptr<Block> block = Block::deserialize(
                _chain_writer->read_block(FileInfo(bl_record->block_file_stored,
                                                   bl_record->block_offset_start,
                                                   bl_record->block_offset_end)));
        output.push_back(std::move(block));
        forked_traversing_hash = _block_info_database->get_block_record(forked_traversing_hash)->block_header->previous_block_hash;
        forked_chain_height -= 1;
        active_traversing_hash = _block_info_database->get_block_record(active_traversing_hash)->block_header->previous_block_hash;
        active_chain_height -= 1;
    }
    return output;
}

//std::vector<std::unique_ptr<Transaction>> Chain::copy_transactions(std::vector<std::unique_ptr<Transaction>> from) {
//    std::vector<std::unique_ptr<Transaction>> transactions;
//    for (const std::unique_ptr<Transaction>& tx : _active_chain_last_block->transactions){
//        std::vector<std::unique_ptr<TransactionInput>> txinpt;
//        std::vector<std::unique_ptr<TransactionOutput>> txoutpt;
//        for (const std::unique_ptr<TransactionInput>& txi : tx->transaction_inputs) {
//            txinpt.push_back(std::make_unique<TransactionInput>(txi->reference_transaction_hash,txi->utxo_index,txi->signature));
//        }
//        for (const std::unique_ptr<TransactionOutput>& txo : tx->transaction_outputs) {
//            txoutpt.push_back(std::make_unique<TransactionOutput>(txo->amount,txo->public_key));
//        }
//        transactions.push_back(std::make_unique<Transaction>(std::move(txinpt), std::move(txoutpt), tx->version, tx->lock_time));
//    }
//}