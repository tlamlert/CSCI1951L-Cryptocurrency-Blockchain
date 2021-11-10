/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_STENCIL_CHAIN_H
#define RATHDB_STENCIL_CHAIN_H

#include "../primitives/block.h"
#include <block_info_database.h>
#include <chain_writer.h>
#include <coin_database.h>

/// Manges blockchain data on disk/RAM. Supports querying.
class Chain {
private:
    // The first section of private members keep track of
    // information regarding the active chain.

    /// The length (in blocks) of the active chain.
    uint32_t _active_chain_length;
    /// The last block of the active chain.
    std::unique_ptr<Block> _active_chain_last_block;

    // The second section of private members keeps track of
    // the managing of block data.

    /// Interface for LevelDB of block metadata.
    std::unique_ptr<BlockInfoDatabase> _block_info_database;
    /// Interface for writing/retrieving blocks to/from disk.
    std::unique_ptr<ChainWriter> _chain_writer;

    // The third section of private members keeps track of
    // the managing of transaction data.

    /// Interface for storing/querying transactions.
    std::unique_ptr<CoinDatabase> _coin_database;

    /// Makes the genesis block.
    static std::unique_ptr<Block> construct_genesis_block();

    /// gets forked blocks on a chain in a stack-like order
    std::vector<std::shared_ptr<Block>> get_forked_blocks_stack(uint32_t starting_hash);

    /// gets all undo blocks on main chain to a specific height
    std::vector<std::unique_ptr<UndoBlock>> get_undo_blocks_queue(uint32_t branching_height);

public:
    // The first section of public functions constructs a Chain.

    /// Constructs a Chain.
    Chain();

    // The second section of public methods is used to handle
    // incoming data from either the network or locally.

    /// Handles validation and storing (if validated) a block.
    void handle_block(std::unique_ptr<Block> block);

    /// Handles validation and storing (if validated) a transaction.
    void handle_transaction(std::unique_ptr<Transaction> transaction);

    // The third section of public methods is getters for data
    // on the chain or metadata about a particular chain that
    // will most likely require traversal.

    /// Gets the chain length up until block with inputted hash.
    uint32_t get_chain_length(uint32_t block_hash);

    /// Gets the block object with the inputted block hash.
    std::unique_ptr<Block> get_block(uint32_t block_hash);

    /// Gets some blocks on the active chain in order.
    std::vector<std::unique_ptr<Block>> get_active_chain(uint32_t start, uint32_t end);

    /// Gets some block hashes on the active chain in order.
    std::vector<uint32_t> get_active_chain_hashes(uint32_t start, uint32_t end);

    // The fourth section of public methods is getters for
    // the active chain that shouldn't require any traversal.

    /// Gets the last block of the active chain.
    std::unique_ptr<Block> get_last_block();

    /// Gets the hash of the last block on the active chain.
    uint32_t get_last_block_hash();

    /// Gets the length of the active chain.
    uint32_t get_active_chain_length() const;

    // The fifth section of public methods is for querying
    // transaction related data.

    /// Gets all utxo for a particular public key.
    std::vector<std::pair<uint32_t, uint8_t>> get_all_utxo(uint32_t public_key);

    // Copy constructor and copy assignment operator deleted.
    Chain(Chain &&other) = delete;

    Chain &operator=(const Chain &other) = delete;
};

#endif //RATHDB_STENCIL_CHAIN_H
