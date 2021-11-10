/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATHDB_RATHCRYPTO_H
#define RATHDB_RATHCRYPTO_H

#include <string>

namespace RathCrypto {
    uint32_t hash(std::string message);
}

#endif //RATHDB_RATHCRYPTO_H
