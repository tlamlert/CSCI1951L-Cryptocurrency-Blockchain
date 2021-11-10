/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_STENCIL_UNDO_COIN_RECORD_H
#define RATHDB_STENCIL_UNDO_COIN_RECORD_H


#include <vector>
#include <transaction.h>
/**
 * The UndoCoinRecord is used to roll back changes to a CoinRecord when we "undo"
 * a Block. The Transaction metadta is the same as for a CoinRecord, but the UTXO
 * information is not. Rather, it contains information for the UTXO used by a
 * just validated Block. That way, if we have to "undo" the Block, we can add
 * that UTXO information back to the Transaction's CoinRecord
 */
class UndoCoinRecord {
public:
    // The first section of public members holds the
    // Transaction's information.

    /// The Transaction's version number
    const uint8_t version;

    // The second section of public members holds the
    // Transaction's UTXO information.

    /// The UTXO used by the validated Block
    std::vector<uint32_t> utxo = {};
    /// the corresponding amounts of the utxo
    std::vector<uint32_t> amounts = {};
    /// the corresponding public keys of the utxo
    std::vector<uint32_t> public_keys = {};

    ////Constructor for when we need to recreate a CoinRecord
    UndoCoinRecord(uint8_t version_,
                   std::vector<uint32_t> utxo_, std::vector<uint32_t> amounts_,
                   std::vector<uint32_t> public_keys_);
};


#endif //RATHDB_STENCIL_UNDO_COIN_RECORD_H
