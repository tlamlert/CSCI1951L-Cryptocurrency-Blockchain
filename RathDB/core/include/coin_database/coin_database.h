/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_STENCIL_COIN_DATABASE_H
#define RATHDB_STENCIL_COIN_DATABASE_H


#include <db.h>
#include <coin.h>
#include <coin_record.h>
#include <undo_block.h>
#include "../primitives/block.h"

/// Locator/Identifier for UTXO.
class CoinLocator {
public:
    const uint32_t transaction_hash;
    const uint8_t output_index;

    CoinLocator(uint32_t transaction_hash, uint8_t output_index);

    static std::string serialize_from_construct(uint32_t transaction_hash, uint8_t output_index);
    static std::string serialize(const CoinLocator& coin_locator);
    static std::unique_ptr<CoinLocator> deserialize(const std::string& serialized_coin_locator);
};

/// Database that manages transactions/UTXO (storing, validating, querying).
class CoinDatabase {
private:

    /// db (hashmap storage) of transaction hash to coin record.
    std::unique_ptr<Database> _database;

    /// Main cache variables
    std::unordered_map<std::string, std::unique_ptr<Coin>> _main_cache;
    /// maximum amount of entries in the cache
    const uint16_t _main_cache_capacity;
    /// current number of entries in the cache
    uint16_t _main_cache_size;

    /// maps transaction hash to transaction
    std::unordered_map<uint32_t, std::unique_ptr<Transaction>> _mempool_cache;
    /// maximum amount of entries in mempool
    const uint16_t _mempool_capacity;
    /// current amount of entires in mempool
    uint16_t _mempool_size;

public:
    CoinDatabase();

    bool validate_block(const std::vector<std::unique_ptr<Transaction>>& transactions);
    bool validate_transaction(const Transaction& transaction);
    void store_block(std::vector<std::unique_ptr<Transaction>> transactions);
    void store_transaction(std::unique_ptr<Transaction> transaction);
    bool validate_and_store_block(std::vector<std::unique_ptr<Transaction>> transactions);
    bool validate_and_store_transaction(std::unique_ptr<Transaction> transaction);

    void remove_transactions_from_mempool(const std::vector<std::unique_ptr<Transaction>>& transactions);
    void store_transactions_to_main_cache(std::vector<std::unique_ptr<Transaction>> transactions);
    void store_transaction_in_mempool(std::unique_ptr<Transaction> transaction);

    void remove_coins(std::vector<std::unique_ptr<Block>> blocks);
    void undo_coins(std::vector<std::unique_ptr<UndoBlock>> undo_blocks);
    std::vector<std::pair<uint32_t, uint8_t>> get_all_utxo(uint32_t public_key);
    void flush_main_cache();

    std::unique_ptr<UndoBlock> make_undo_block(Block& block);
};

#endif //RATHDB_STENCIL_COIN_DATABASE_H
