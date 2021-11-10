#include <chain_writer.h>
#include <rathcrypto.h>
#include <fstream>
#include <stdio.h>
#include <filesystem>

const std::string ChainWriter::_file_extension = "data";
const std::string ChainWriter::_block_filename = "blocks";
const uint16_t ChainWriter::_max_block_file_size = 1000;
const std::string ChainWriter::_undo_filename = "undo_blocks";
const uint16_t ChainWriter::_max_undo_file_size = 1000;
const std::string ChainWriter::_data_directory = "data";

ChainWriter::ChainWriter() {
    _current_block_file_number = 0;
    _current_block_offset = 0;
    _current_undo_file_number = 0;
    _current_undo_offset = 0;
    std::filesystem::create_directory(_data_directory);
}

std::unique_ptr<BlockRecord> ChainWriter::store_block(const Block& block, const UndoBlock& undo_block, uint32_t height) {
    // write block
    std::unique_ptr<FileInfo> block_info = write_block(Block::serialize(block));

    // construct undo block
//    std::vector<uint32_t> tx_hashes;
//    std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records;
//    for (const std::unique_ptr<Transaction>& tx : block.transactions) {
//        tx_hashes.push_back(RathCrypto::hash(Transaction::serialize(*tx)));
//        std::vector<uint32_t> utxo, amounts, public_keys;
//        for (std::unique_ptr<TransactionInput>& txinpt : tx->transaction_inputs) {
//            utxo.push_back(txinpt->utxo_index);
//            amounts.push_back(0);
//            public_keys.push_back(txinpt->signature);
//        }
//        undo_coin_records.push_back(std::make_unique<UndoCoinRecord>(tx->version, utxo, amounts, public_keys));
//    }
//    UndoBlock undo_block = UndoBlock(tx_hashes, std::move(undo_coin_records));

    // write undo block
    std::unique_ptr<FileInfo> undo_info = write_undo_block(UndoBlock::serialize(undo_block));

    return std::move(std::make_unique<BlockRecord>(std::make_unique<BlockHeader>(*block.block_header),
                                         uint32_t(block.transactions.size()),
                                         height, *block_info, *undo_info));
}

std::unique_ptr<FileInfo> ChainWriter::write_block(std::string serialized_block) {
    uint16_t len = serialized_block.length();
    if (_current_block_offset > _max_block_file_size) {
        _current_block_file_number++;
        _current_block_offset = 0;
    }
    std::string file_name =
            _data_directory + "/" + _block_filename + "_" + std::to_string(_current_block_file_number) + "." +
            _file_extension;
    FILE *fw = std::fopen(file_name.c_str(), "ab");
    fputs(serialized_block.c_str(), fw);
    fclose(fw);
    _current_block_offset += len;
    return std::make_unique<FileInfo>(file_name, _current_block_offset - len, _current_block_offset);
}

std::unique_ptr<FileInfo> ChainWriter::write_undo_block(std::string serialized_block) {
    uint16_t len = serialized_block.length();
    if (_current_undo_offset + len > _max_undo_file_size) {
        _current_undo_file_number++;
        _current_undo_offset = 0;
    }
    std::string file_name =
            _data_directory + "/" + _undo_filename + "_" + std::to_string(_current_undo_file_number) + "."
            + _file_extension;

    FILE *fw = std::fopen(file_name.c_str(), "ab");
    fputs(serialized_block.c_str(), fw);
    fclose(fw);
    _current_undo_offset += len;
    return std::make_unique<FileInfo>(file_name, _current_block_offset - len, _current_block_offset);
}

std::string ChainWriter::read_block(const FileInfo& block_location) {
    std::string file_name = block_location.file_name;
    uint16_t len = block_location.end - block_location.start;
    char * buffer = (char*) malloc(len);
    FILE *fr = std::fopen(file_name.c_str(), "rb");
    fseek (fr, block_location.start, SEEK_SET);
    fread(buffer, sizeof(char), len, fr);
    fclose(fr);
    return std::string(buffer);
}

std::string ChainWriter::read_undo_block(const FileInfo& undo_block_location) {
    std::string file_name = undo_block_location.file_name;
    uint16_t len = undo_block_location.end - undo_block_location.start;
    char * buffer = (char*) malloc(len);
    FILE *fr = std::fopen(file_name.c_str(), "rb");
    fseek (fr, undo_block_location.start, SEEK_SET);
    fread(buffer, sizeof(char), len, fr);
    fclose(fr);
    return std::string(buffer);
}
