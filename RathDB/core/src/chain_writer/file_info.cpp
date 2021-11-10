/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#include <file_info.h>

FileInfo::FileInfo(std::string file_name_, uint16_t start_,
                   uint16_t end_) : file_name(file_name_),
                                    start(start_), end(end_) {}