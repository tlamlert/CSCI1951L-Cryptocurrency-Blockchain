/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#include <gtest/gtest.h>
#include <coin_record.h>
#include <coin_database.h>
#include <rathcrypto.h>

TEST(CoinLocator, Serialize) {
    CoinLocator coin_locator = CoinLocator(40, 33);
    std::string serialized_coin_locator = CoinLocator::serialize(coin_locator);
    std::unique_ptr<CoinLocator> deserialized_coin_locator = CoinLocator::deserialize(serialized_coin_locator);
    EXPECT_EQ(coin_locator.output_index, deserialized_coin_locator->output_index);
    EXPECT_EQ(deserialized_coin_locator->output_index, 33);

    std::string serialized_coin_locator2 = CoinLocator::serialize_from_construct(22, 19);
    std::unique_ptr<CoinLocator> deserialized_coin_locator2 = CoinLocator::deserialize(serialized_coin_locator2);
    EXPECT_EQ(deserialized_coin_locator2->output_index, 19);
}

TEST(CoinRecord, Serialize) {
    std::vector<uint32_t> utxo = {99};
    std::vector<uint32_t> amounts = {12};
    std::vector<uint32_t> public_keys = {14};
    CoinRecord coin_record = CoinRecord(99, std::move(utxo), std::move(amounts),
                                        std::move(public_keys));
    std::string serialized_coin_record = CoinRecord::serialize(coin_record);
    std::unique_ptr<CoinRecord> deserialized_coin_record = CoinRecord::deserialize(serialized_coin_record);
    EXPECT_EQ(coin_record.version, deserialized_coin_record->version);
    EXPECT_EQ(deserialized_coin_record->version, 99);
}

TEST(CoinDatabase, store_transactions_to_main_cache) {
    CoinDatabase coin_database = CoinDatabase();
    std::vector<std::unique_ptr<TransactionInput>> txi;
    std::vector<std::unique_ptr<TransactionOutput>> txo;
    txo.push_back(std::make_unique<TransactionOutput> (100, 12345));
    txo.push_back(std::make_unique<TransactionOutput> (200, 67891));
    txo.push_back(std::make_unique<TransactionOutput> (300, 23456));
    std::unique_ptr<Transaction> transaction = std::make_unique<Transaction>(std::move(txi), std::move(txo));
    uint32_t tx_hash = RathCrypto::hash(Transaction::serialize(*transaction));
    std::vector<std::unique_ptr<Transaction>> transactions;
    transactions.push_back(std::move(transaction));
    coin_database.store_transactions_to_main_cache(std::move(transactions));


    std::vector<std::unique_ptr<TransactionInput>> txi2;
    std::vector<std::unique_ptr<TransactionOutput>> txo2;
    txi2.push_back(std::make_unique<TransactionInput>(tx_hash, 1, 555));
    txo2.push_back(std::make_unique<TransactionOutput> (100, 55555));
    std::vector<std::unique_ptr<Transaction>> transactions2;
    transactions2.push_back(std::make_unique<Transaction>(std::move(txi2), std::move(txo2)));

    std::vector<std::unique_ptr<TransactionInput>> txi3;
    std::vector<std::unique_ptr<TransactionOutput>> txo3;
    txi3.push_back(std::make_unique<TransactionInput>(0, 1, 55555));
    txo3.push_back(std::make_unique<TransactionOutput> (100, 55555));
    std::vector<std::unique_ptr<Transaction>> transactions3;
    transactions3.push_back(std::make_unique<Transaction>(std::move(txi3), std::move(txo3)));

    EXPECT_EQ(coin_database.validate_block(transactions2), true);
    EXPECT_EQ(coin_database.validate_block(transactions3), false);
    coin_database.store_transactions_to_main_cache(std::move(transactions2));

    std::vector<std::unique_ptr<TransactionInput>> txi4;
    std::vector<std::unique_ptr<TransactionOutput>> txo4;
    txi4.push_back(std::make_unique<TransactionInput>(tx_hash, 1, 555));
    txo4.push_back(std::make_unique<TransactionOutput> (100, 55555));
    std::vector<std::unique_ptr<Transaction>> transactions4;
    transactions4.push_back(std::make_unique<Transaction>(std::move(txi4), std::move(txo4)));
    EXPECT_EQ(coin_database.validate_block(transactions4), false);
}
