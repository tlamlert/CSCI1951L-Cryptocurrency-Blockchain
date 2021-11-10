/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_BLOCK_RECORD_H
#define RATHDB_BLOCK_RECORD_H

#include <file_info.h>
#include <block_header.h>
#include <rath.pb.h>
#include <string>

/// Contains metadata about a block and where the block and "undo" block are stored on disk.
class BlockRecord {
public:
    /// The Block's block header
    const std::unique_ptr<BlockHeader> block_header;
    /// The Block's height (depth) relative to its own chain.
    const uint32_t height;
    /// The Block's number of transactions
    const uint32_t num_transactions;

    // The second section of public members holds locating information
    // for the raw Block on disk.

    /// The filename storing the raw Block
    const std::string block_file_stored;
    /// The raw Block's starting offset in the file
    uint16_t block_offset_start;
    /// The raw Block's ending offset in the file
    uint16_t block_offset_end;

    // The third section of public members holds locating information
    // for the Block's "undo" protocol

    /// The filename storing the "undo" protocol
    const std::string undo_file_stored;
    /// The Block's "undo" protocol starting offset in the file
    uint16_t undo_offset_start;
    /// The Block's "undo" protocol ending offset in the file
    uint16_t undo_offset_end;

    /// Constructor based on a Block, file information for that Block, and file information
    /// to "undo" the Block
    BlockRecord(std::unique_ptr<BlockHeader> block_header_,
                uint32_t num_transactions_, uint32_t height_,
                const FileInfo& block_info_, const FileInfo& undo_info_);

    /// serialize
    static std::string serialize(const BlockRecord& block_record);
    /// deserialize.
    static std::unique_ptr<BlockRecord> deserialize(
            const std::string& serialized_block_record);
};

#endif //RATHDB_BLOCK_RECORD_H
