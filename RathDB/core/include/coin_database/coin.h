/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_STENCIL_COIN_H
#define RATHDB_STENCIL_COIN_H

#include <transaction_output.h>

class Coin {
public:
    /// Unspent TransactionOutput
    std::unique_ptr<TransactionOutput> transaction_output;
    bool is_spent;

    Coin(std::unique_ptr<TransactionOutput> transaction_output_,
         bool is_spent_);
};

#endif //RATHDB_STENCIL_COIN_H
