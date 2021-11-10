/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_STENCIL_COIN_RECORD_H
#define RATHDB_STENCIL_COIN_RECORD_H

#include <transaction.h>
#include <rath.pb.h>
#include <string>

class CoinRecord {

public:
    // The first section of public members holds the
    // Transaction's information.

    /// The Transaction's version number
    const uint8_t version;

    // The second section of public members holds the
    // Transaction's UTXO information.

    /// The up-to-date indexes of the Transaction's UTXO
    std::vector<uint32_t> utxo;
    /// the corresponding amounts of the utxo
    std::vector<uint32_t> amounts;
    /// the corresponding public keys of the utxo
    std::vector<uint32_t> public_keys;

    ///Constructor for when we need to recreate a CoinRecord
    CoinRecord(uint8_t version_,
               std::vector<uint32_t> utxo_, std::vector<uint32_t> amounts_,
               std::vector<uint32_t> public_keys_);

    /// Serializes a CoinRecord. Necessary to write CoinRecords to the
    /// Coin Database's db
    static std::string serialize(const CoinRecord& coin_record);
    /// Serializes a CoinRecord. Necessary to read CoinRecords from the
    /// Coin Database's db
    static std::unique_ptr<CoinRecord> deserialize(const std::string& serialized_coin_record);
};

#endif //RATHDB_STENCIL_COIN_RECORD_H
