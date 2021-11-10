/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATH_TRANSACTION_INPUT_H
#define RATH_TRANSACTION_INPUT_H

#include <string>

class TransactionInput {
public:
    uint32_t reference_transaction_hash;
    uint8_t utxo_index;
    uint32_t signature;

    TransactionInput(
            uint32_t reference_transaction_hash_, uint8_t utxo_index_,
            uint32_t signature_);
    static std::string serialize(const TransactionInput& transaction_input);
    static std::unique_ptr<TransactionInput> deserialize(const std::string& serialized_transaction_input);
};


#endif //RATH_TRANSACTION_INPUT_H
