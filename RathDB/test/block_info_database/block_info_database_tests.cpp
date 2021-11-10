//
// Created by Tanadol Lamlertprasertkul on 7/13/21.
//

#include <gtest/gtest.h>
#include <block_info_database.h>

TEST(BlockInfoDatabase, StoreAndGet) {
    BlockInfoDatabase database = BlockInfoDatabase();

    FileInfo file_info1 = FileInfo("a", 1, 2);
    BlockRecord bl_rec1 = BlockRecord(std::make_unique<BlockHeader>(), 40, 1, file_info1, file_info1);
    database.store_block_record(1, bl_rec1);

    FileInfo file_info2 = FileInfo("a", 2, 3);
    BlockRecord bl_rec2 = BlockRecord(std::make_unique<BlockHeader>(), 20, 1, file_info2, file_info2);
    database.store_block_record(2, bl_rec2);

    EXPECT_EQ(database.get_block_record(1)->num_transactions, 40);
    EXPECT_EQ(database.get_block_record(2)->num_transactions, 20);
    EXPECT_EQ(database.get_block_record(3)->num_transactions, 0);
    EXPECT_EQ(database.get_block_record(1)->height, 1);
    EXPECT_EQ(database.get_block_record(1)->block_header->previous_block_hash, 0);
}