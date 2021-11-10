/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#ifndef RATH_BLOCK_HEADER_H
#define RATH_BLOCK_HEADER_H

#include <rath.pb.h>

class BlockHeader {
public:
    uint8_t version;
    uint32_t previous_block_hash;
    uint32_t merkle_root;
    uint32_t difficulty_target;
    uint32_t nonce;
    uint32_t timestamp;

    inline void set_version(uint8_t version_);
    inline void set_previous_block_hash(uint32_t previous_block_hash_);
    inline void set_merkle_root(uint32_t merkle_root_);
    inline void set_difficulty_target(uint32_t difficulty_target_);
    inline void set_nonce(uint32_t nonce_);
    inline void set_timestamp(uint32_t timestamp_);

    BlockHeader(uint8_t version_=0,
                uint32_t previous_block_hash_=0,
                uint32_t merkle_root_=0,
                uint32_t difficulty_target_=0,
                uint32_t nonce_=0,
                uint32_t timestamp_=0);

    static std::string serialize(const BlockHeader& block_header);
    static std::unique_ptr<BlockHeader> deserialize(const std::string& serialized_block_header);
};


#endif //RATH_BLOCK_HEADER_H
