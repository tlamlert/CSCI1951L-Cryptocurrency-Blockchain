/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATH_BLOCK_H
#define RATH_BLOCK_H

#include <transaction.h>
#include <block_header.h>
#include <vector>
#include <rath.pb.h>
#include <string>

class Block {
public:
    std::unique_ptr<BlockHeader> block_header;
    std::vector<std::unique_ptr<Transaction>> transactions;

    std::vector<std::unique_ptr<Transaction>> get_transactions();

    void set_block_header(std::unique_ptr<BlockHeader> block_header_);
    void set_transactions(std::vector<std::unique_ptr<Transaction>> transactions_);

    Block(std::unique_ptr<BlockHeader> block_header_, std::vector<std::unique_ptr<Transaction>> transactions_);

    static std::string serialize(const Block& block);
    static std::unique_ptr<Block> deserialize(const std::string& serialized_block);
};

#endif //RATH_BLOCK_H
