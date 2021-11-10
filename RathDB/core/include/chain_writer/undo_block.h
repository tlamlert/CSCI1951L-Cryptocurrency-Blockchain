/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_UNDO_BLOCK_H
#define RATHDB_UNDO_BLOCK_H

#include <undo_coin_record.h>
#include <rath.pb.h>
#include <string>

/**
 * The UndoBlock contains information necessary to "undo" a block. To "undo" a
 * Block, we must properly reset the CoinRecords for the Transactions from the
 * offending Block. We do this using the CoinLocators and UndoCoinRecords.
 *
 */
class UndoBlock {
public:
    /// Both of the below vectors shoudl have same size and have corresponding entries.
    /// The CoinLocators for the CoinRecords we want to change
    const std::vector<uint32_t> transaction_hashes;
    /// The UndoCoinRecords to roll back the offending Block's changes
    const std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records;

    UndoBlock(std::vector<uint32_t> transaction_hashes_,
              std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records_);

    /// Serializes an UndoBlock. Necessary to write UndoBlocks to an
    /// "undo" file
    static std::string serialize(const UndoBlock& undo_block);
    /// Deserializes an UndoBlock. Necessary to read UndoBlocks
    /// from an "undo" file
    static std::unique_ptr<UndoBlock> deserialize(const std::string& serialized_undo_block);
};

#endif //RATHDB_UNDO_BLOCK_H
