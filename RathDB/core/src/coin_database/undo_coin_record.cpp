/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#include <undo_coin_record.h>

UndoCoinRecord::UndoCoinRecord(
        uint8_t version_,
        std::vector<uint32_t> utxo_, std::vector<uint32_t> amounts_,
        std::vector<uint32_t> public_keys_) : version(version_),
                                              utxo(std::move(utxo_)), amounts(std::move(amounts_)),
                                              public_keys(std::move(public_keys_)) {}

