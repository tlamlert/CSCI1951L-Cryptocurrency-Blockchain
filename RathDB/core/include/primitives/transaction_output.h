/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATH_TRANSACTION_OUTPUT_H
#define RATH_TRANSACTION_OUTPUT_H

#include <string>

class TransactionOutput {
public:
    uint32_t amount;
    uint32_t public_key;

    TransactionOutput(uint32_t amount_, uint32_t public_key_);
    static std::string serialize(const TransactionOutput& transaction_input);
    static std::unique_ptr<TransactionOutput> deserialize(const std::string& serialized_transaction_input);
};

#endif //RATH_TRANSACTION_OUTPUT_H
