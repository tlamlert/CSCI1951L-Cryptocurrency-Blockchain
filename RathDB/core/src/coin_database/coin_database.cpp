/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#include <coin_database.h>
#include <rathcrypto.h>

CoinLocator::CoinLocator(
        uint32_t transaction_hash_, uint8_t output_index_)
        : transaction_hash(transaction_hash_),
          output_index(output_index_) {}

std::string CoinLocator::serialize(const CoinLocator& coin_locator) {
    return std::to_string(coin_locator.transaction_hash) + "-" +
           std::to_string(coin_locator.output_index);
}

std::unique_ptr<CoinLocator> CoinLocator::deserialize(const std::string& serialized_coin_locator) {
    std::string transaction_hash = serialized_coin_locator.substr(0, serialized_coin_locator.find("-"));
    std::string output_index = serialized_coin_locator.substr(serialized_coin_locator.find("-") + 1, serialized_coin_locator.size());
    return std::make_unique<CoinLocator>(std::stoul (transaction_hash,nullptr,0), std::stoul (output_index,nullptr,0));
}

std::string CoinLocator::serialize_from_construct(uint32_t transaction_hash, uint8_t output_index) {
    return std::to_string(transaction_hash) + "-" +
           std::to_string(output_index);
}

CoinDatabase::CoinDatabase()
    : _main_cache_capacity(30),
      _mempool_capacity(50) {
    _database = std::make_unique<Database>();
    _main_cache_size = 0;
    _mempool_size = 0;
};

bool CoinDatabase::validate_block(const std::vector<std::unique_ptr<Transaction>>& transactions){
    for (const std::unique_ptr<Transaction> & tx : transactions) {
        if (!validate_transaction(*tx)) { return false; }
    }
    return true;
}

bool CoinDatabase::validate_transaction(const Transaction& transaction) {
    // check duplicates
    int sz = transaction.transaction_inputs.size();
    for (int i = 0; i < sz; i++) {
        for (int j = i+1; j < sz; j++) {
            if (transaction.transaction_inputs[i] == transaction.transaction_inputs[j]) {
                return false;
            }
        }
    }

    // check utxo validity
    for (const std::unique_ptr<TransactionInput>& txinput : transaction.transaction_inputs) {
        uint32_t tx_hash = txinput->reference_transaction_hash;
        std::string locator = CoinLocator::serialize_from_construct(tx_hash, txinput->utxo_index);
        if(_main_cache.contains(locator)){
            if(_main_cache[locator]->is_spent) {
                return false;
            }
        } else if (_database->get_safely(std::to_string(tx_hash)) != "") {
            std::unique_ptr<CoinRecord> coin_record = CoinRecord::deserialize(_database->get_safely(std::to_string(tx_hash)));
            bool contain;
            for (uint32_t utxo : coin_record->utxo) {
                if (utxo == txinput->utxo_index) {
                    contain = true;
                }
            }
            if (!contain) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

void CoinDatabase::store_block(std::vector<std::unique_ptr<Transaction>> transactions){
    remove_transactions_from_mempool(transactions);
    store_transactions_to_main_cache(std::move(transactions));
}

void CoinDatabase::store_transaction(std::unique_ptr<Transaction> transaction){
    store_transaction_in_mempool(std::move(transaction));
}

bool CoinDatabase::validate_and_store_block(std::vector<std::unique_ptr<Transaction>> transactions){
    if (validate_block(transactions)) {
        store_block(std::move(transactions));
        return true;
    }
    return false;
}

bool CoinDatabase::validate_and_store_transaction(std::unique_ptr<Transaction> transaction){
    if (validate_transaction(*transaction)) {
        store_transaction(std::move(transaction));
        return true;
    }
    return false;
}

void CoinDatabase::remove_transactions_from_mempool(const std::vector<std::unique_ptr<Transaction>>& transactions) {
    for (const std::unique_ptr<Transaction>& tx : transactions) {
        uint32_t tx_hash = RathCrypto::hash(Transaction::serialize(*tx.get()));
        _mempool_cache.erase(tx_hash);
        _mempool_size--;
    }
}

void CoinDatabase::store_transactions_to_main_cache(std::vector<std::unique_ptr<Transaction>> transactions) {
    for (const std::unique_ptr<Transaction>& tx : transactions) {
        uint32_t tx_hash = RathCrypto::hash(Transaction::serialize(*tx));

        // construct and store coin record to database
        std::vector<uint32_t> utxo, amounts, public_keys;
        for (int i = 0; i < tx->transaction_outputs.size(); i++) {
            utxo.push_back(i);
            amounts.push_back(tx->transaction_outputs[i]->amount);
            public_keys.push_back(tx->transaction_outputs[i]->public_key);
        }
        CoinRecord coin_record = CoinRecord(tx->version, utxo, amounts, public_keys);
        _database->put_safely(std::to_string(tx_hash), CoinRecord::serialize(coin_record));

        // add coins to _main_cache
        int szi = tx->transaction_inputs.size();
        for (int i = 0; i < szi; i++) {
            std::string locator = CoinLocator::serialize_from_construct(tx->transaction_inputs[i]->reference_transaction_hash,
                                                                        tx->transaction_inputs[i]->utxo_index);
            if(_main_cache.contains(locator)){
                _main_cache[locator]->is_spent = true;
            } else {
                _database->delete_safely(std::to_string(tx->transaction_inputs[i]->reference_transaction_hash));
            }
        }
        int szo = tx->transaction_outputs.size();
        for (int i = 0; i < szo; i++) {
            if (_main_cache_size >= _main_cache_capacity) {
                flush_main_cache();
            }
            std::string locator = CoinLocator::serialize_from_construct(tx_hash, i);
            std::unique_ptr<Coin> coin = std::make_unique<Coin>(std::move(tx->transaction_outputs[i]), false);
            _main_cache.insert(std::make_pair(locator, std::move(coin)));
        }
    }
}

void CoinDatabase::store_transaction_in_mempool(std::unique_ptr<Transaction> transaction) {
    if (_mempool_size < _mempool_capacity) {
        uint32_t tx_hash = RathCrypto::hash(Transaction::serialize(*transaction));
        _mempool_cache.insert(std::make_pair(tx_hash, std::move(transaction)));
        _mempool_size++;
    }
}

void CoinDatabase::remove_coins(std::vector<std::unique_ptr<Block>> blocks) {
    for (const std::unique_ptr<Block>& bl : blocks) {
        for (const std::unique_ptr<Transaction>& tx : bl->transactions){
            int szi = tx->transaction_inputs.size();
            for (int i = 0; i < szi; i++) {
                _database->delete_safely(std::to_string(tx->transaction_inputs[i]->reference_transaction_hash));
            }
        }
    }
}

void CoinDatabase::undo_coins(std::vector<std::unique_ptr<UndoBlock>> undo_blocks) {
    for (const std::unique_ptr<UndoBlock>& undo_bl : undo_blocks) {
        for (int i = 0; i < undo_bl->transaction_hashes.size(); i++) {
            uint32_t tx_hash = undo_bl->transaction_hashes[i];
            for (int j = 0; j < undo_bl->undo_coin_records[i]->utxo.size(); j++) {
                uint32_t utxo = undo_bl->undo_coin_records[i]->utxo[j];
                uint32_t amount = undo_bl->undo_coin_records[i]->amounts[j];
                uint32_t pubkey = undo_bl->undo_coin_records[i]->public_keys[j];
                std::string loc = CoinLocator::serialize_from_construct(tx_hash, utxo);
                if (_main_cache.contains(loc)) {
                    _main_cache[loc]->is_spent = false;
                } else if (_database->get_safely(loc) != "") {
                    std::unique_ptr<CoinRecord> rec = CoinRecord::deserialize(_database->get_safely(loc));
                    rec->utxo.push_back(utxo);
                    rec->amounts.push_back(amount);
                    rec->public_keys.push_back(pubkey);
                    _database->put_safely(loc, CoinRecord::serialize(*rec));
                } else {
                    _database->put_safely(loc, CoinRecord::serialize(
                            CoinRecord(undo_bl->undo_coin_records[i]->version,
                                       std::vector<uint32_t> {utxo},
                                       std::vector<uint32_t> {amount},
                                       std::vector<uint32_t> {pubkey})));
                }
            }
        }
    }

}

void CoinDatabase::flush_main_cache() {
    for (const auto& [key, coin] : _main_cache) {
        if (!coin->is_spent) {
            continue;
        }
        std::unique_ptr<CoinLocator> locator = CoinLocator::deserialize(key);
        uint32_t tx_hash = locator->transaction_hash;
        uint32_t output_index = locator->output_index;
        std::unique_ptr<CoinRecord> coin_record = CoinRecord::deserialize(_database->get_safely(std::to_string(tx_hash)));
        auto it = std::find(coin_record->utxo.begin(), coin_record->utxo.end(), output_index);
        int index = it - coin_record->utxo.begin();
        coin_record->utxo.erase(coin_record->utxo.begin() + index);
        coin_record->amounts.erase(coin_record->amounts.begin() + index);
        coin_record->public_keys.erase(coin_record->public_keys.begin() + index);
    }
    _main_cache_size = 0;
    _main_cache.clear();
}

std::unique_ptr<UndoBlock> CoinDatabase::make_undo_block(Block& block) {
    std::vector<uint32_t> tx_hashes = std::vector<uint32_t>();
    std::vector<std::unique_ptr<UndoCoinRecord>> undo_rec = std::vector<std::unique_ptr<UndoCoinRecord>>();

    for (const std::unique_ptr<Transaction>& tx : block.transactions) {
        uint32_t tx_hash = RathCrypto::hash(Transaction::serialize(*tx));
        tx_hashes.push_back(tx_hash);
        std::vector<uint32_t> utxo, amounts, pubkeys;
        for (const std::unique_ptr<TransactionInput>& txi : tx->transaction_inputs) {
            std::string locator = CoinLocator::serialize_from_construct(txi->reference_transaction_hash, txi->utxo_index);
            if (_main_cache.contains(locator)) {
                utxo.push_back(txi->utxo_index);
                amounts.push_back(_main_cache[locator]->transaction_output->amount);
                pubkeys.push_back(_main_cache[locator]->transaction_output->public_key);
            } else if (_database->get_safely(std::to_string(tx_hash)) != "") {
                std::unique_ptr<CoinRecord> coin_record = CoinRecord::deserialize(_database->get_safely(std::to_string(tx_hash)));
                auto it = std::find(coin_record->utxo.begin(), coin_record->utxo.end(), txi->utxo_index);
                int index = it - coin_record->utxo.begin();
                utxo.push_back(coin_record->utxo[index]);
                amounts.push_back(coin_record->amounts[index]);
                pubkeys.push_back(coin_record->public_keys[index]);
            } else {
                return nullptr;
            }
        }
        std::unique_ptr<UndoCoinRecord> undo_coin = std::make_unique<UndoCoinRecord>(tx->version, utxo, amounts, pubkeys);
        undo_rec.push_back(std::move(undo_coin));
    }
    return std::make_unique<UndoBlock>(tx_hashes, std::move(undo_rec));
}