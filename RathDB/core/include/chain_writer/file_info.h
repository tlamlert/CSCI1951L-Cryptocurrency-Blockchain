/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */


#ifndef RATHDB_STENCIL_FILE_INFO_H
#define RATHDB_STENCIL_FILE_INFO_H


#include <string>

/// FileInfo is returned by ChainWriter after writing to disk.
class FileInfo {
public:
    /// name of the file
    std::string file_name;
    /// starting offset of data
    const uint16_t start;
    /// ending offset of data
    const uint16_t end;

    FileInfo(std::string file_name_, uint16_t start_, uint16_t end_);
};

#endif //RATHDB_STENCIL_FILE_INFO_H
