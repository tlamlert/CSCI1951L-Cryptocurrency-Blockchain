/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_STENCIL_CHAIN_WRITER_H
#define RATHDB_STENCIL_CHAIN_WRITER_H

#include <block_record.h>
#include <stdio.h>
#include <mutex>
#include <file_info.h>
#include <undo_block.h>
#include "../primitives/block.h"

/// File I/O for block data and "undo" block data.
class ChainWriter {
private:
    // The first section of private members holds information
    // about where the block data is being written to.

    /// Default file extension: used for both Undo and Block files
    static const std::string _file_extension;
    static const std::string _data_directory;

    /// Default filename that block data is stored to.
    static const std::string _block_filename;
    /// Current file number appended to filename to get real path.
    uint16_t _current_block_file_number;
    /// Current offset into the current file to write data to.
    uint16_t _current_block_offset;
    /// Defaul maximum size (in bytes) a file containing block data can be.
    static const uint16_t _max_block_file_size;

    // The second section of private members holds information
    // about where the "undo" block data is being written to.

    /// Default filename that "undo" block data is stored to.
    static const std::string _undo_filename;
    /// Current file number appended to filename to get real path.
    uint16_t _current_undo_file_number;
    /// Current offset into the current file to write "undo" data to.
    uint16_t _current_undo_offset;
    /// Default aximum size a file containing "undo" block data can be.
    static const uint16_t _max_undo_file_size;
public:
    ChainWriter();
    
    static std::string get_data_directory() {return _data_directory;}
    static std::string get_file_extension() {return _file_extension;}
    static std::string get_block_filename() {return _block_filename;}
    static std::string get_undo_filename() {return _undo_filename;}
    static uint16_t get_max_block_file_size() {return _max_block_file_size;}
    static uint16_t get_max_undo_file_size() {return _max_undo_file_size;}

    /// stores a block's info and its undo block info
    std::unique_ptr<BlockRecord> store_block(const Block& block, const UndoBlock& undo_block, uint32_t height);
    /// writes a serialized block to disk.
    std::unique_ptr<FileInfo> write_block(std::string serialized_block);
    /// writes a serialized undo block to disk.
    std::unique_ptr<FileInfo> write_undo_block(std::string serialized_block);
    /// reads a serialized block from disk.
    std::string read_block(const FileInfo& block_location);
    /// reads a serialized undo block from disk.
    std::string read_undo_block(const FileInfo& undo_block_location);

    // Copy constructor and copy assignment operator deleted.
    ChainWriter(ChainWriter&& other) = delete;
    ChainWriter& operator=(const ChainWriter& other) = delete;
};

#endif //RATHDB_STENCIL_CHAIN_WRITER_H
