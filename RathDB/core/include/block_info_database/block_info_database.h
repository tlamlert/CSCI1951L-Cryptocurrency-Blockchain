/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_BLOCK_INFO_DATABASE_H
#define RATHDB_BLOCK_INFO_DATABASE_H

#include <db.h>
#include <block_record.h>

/// Interface for Database. Contains information about where blocks and undo blocks on disk are stored.
class BlockInfoDatabase {
private:
    /// (hashmap storage) of block hash to block record.
    std::unique_ptr<Database> _database;
public:
    /// Constructor.
    BlockInfoDatabase();

    /// Writes a key (hash) value (record) pair to the database.
    void store_block_record(uint32_t hash, const BlockRecord& record);
    /// Gets a value (block record) based on a key (block hash).
    std::unique_ptr<BlockRecord> get_block_record(uint32_t block_hash);

    // Copy constructor and copy assignment operator deleted.
    BlockInfoDatabase(BlockInfoDatabase&& other) = delete;
    BlockInfoDatabase& operator=(const BlockInfoDatabase& other) = delete;

    // Move constructor and move assignment operator deleted.
    BlockInfoDatabase(const BlockInfoDatabase& other) = delete;
    BlockInfoDatabase& operator=(BlockInfoDatabase&& other) = delete;
};

#endif //RATHDB_BLOCK_INFO_DATABASE_H
