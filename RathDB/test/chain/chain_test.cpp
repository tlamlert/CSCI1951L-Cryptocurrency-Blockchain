#include <gtest/gtest.h>
#include <chain.h>
#include <rathcrypto.h>
#include <filesystem>
#include <memory>

TEST(Chain, GetGenesisBlockHash0) {
    // test setup - remove past data
    std::filesystem::remove_all(ChainWriter::get_data_directory());

    Chain chain = Chain();

    std::unique_ptr<Block> genesis_block = chain.get_last_block();
    // check that genesis block has at least 1 transaction
    std::cout << genesis_block->transactions.size() << std::endl;
    EXPECT_TRUE(genesis_block->transactions.size() >= 1);

    const std::vector<std::unique_ptr<TransactionOutput>> &transaction_outputs = genesis_block->transactions.at(0)->transaction_outputs;
    // check that the first transaction has at least 3 outputs
    EXPECT_TRUE(transaction_outputs.size() >= 3);
    EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
    EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
    EXPECT_EQ(transaction_outputs.at(2)->amount, 300);

    uint32_t hash0 = RathCrypto::hash(Block::serialize(*genesis_block));
    EXPECT_EQ(hash0, chain.get_last_block_hash());

    std::filesystem::remove_all(ChainWriter::get_data_directory());
}

std::unique_ptr<Block> make_blockd(std::unique_ptr<Block> block, int tx_index, int out_index) {
      std::vector<std::unique_ptr<Transaction>> transactions;
      std::vector<std::unique_ptr<TransactionInput>> transaction_inputs;
      transaction_inputs.push_back(std::make_unique<TransactionInput>(
          RathCrypto::hash(Transaction::serialize(*block->transactions.at(tx_index))),
          out_index,
          99
      ));
      std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs;
      transaction_outputs.push_back(std::make_unique<TransactionOutput>(
          block->transactions.at(tx_index)->transaction_outputs.at(out_index)->amount,
          block->transactions.at(tx_index)->transaction_outputs.at(out_index)->public_key
      ));
      transactions.push_back(std::make_unique<Transaction>(std::move(transaction_inputs), std::move(transaction_outputs)));
      std::unique_ptr<BlockHeader> block_header = std::make_unique<BlockHeader>(1, RathCrypto::hash(Block::serialize(*block)), 3, 4, 5, 6);
      std::unique_ptr<Block> b = std::make_unique<Block>( std::move(block_header), std::move(transactions));
      return std::move(b);
}

TEST(Chain, HandleValidBlock23) {
      std::filesystem::remove_all(ChainWriter::get_data_directory());

      Chain chain = Chain();

      // 1 = add valid to genesis
      std::unique_ptr<Block> genesis_block = chain.get_last_block();
      // check that genesis block has at least 1 transaction
      EXPECT_TRUE(genesis_block->transactions.size() >= 1);
      const std::vector<std::unique_ptr<TransactionOutput>> &transaction_outputs = genesis_block->transactions.at(0)->transaction_outputs;
      // check that the first transaction has at least 3 outputs
      EXPECT_TRUE(transaction_outputs.size() >= 3);
      EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
      EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
      EXPECT_EQ(transaction_outputs.at(2)->amount, 300);
      std::unique_ptr<Block> block = make_blockd(std::move(genesis_block), 0, 0);
      uint32_t block_hash = RathCrypto::hash(Block::serialize(*block));
      chain.handle_block(std::move(block));

      EXPECT_EQ(block_hash, chain.get_last_block_hash());
      std::unique_ptr<Block> ret_block = chain.get_last_block();
      EXPECT_EQ(ret_block->transactions.size(), 1);
      EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.size(), 1);
      EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.at(0)->amount, 100);
}

TEST(Chain, get_active_chain_hashes) {
    Chain chain = Chain();

    EXPECT_EQ(chain.get_active_chain_hashes(0,1)[0], chain.get_last_block_hash());
}

TEST(Chain, get_active_chain) {
    Chain chain = Chain();

    EXPECT_EQ(Block::serialize(*chain.get_active_chain(0,1)[0]), Block::serialize(*chain.get_last_block()));
}

TEST(Chain, get_block) {
    Chain chain = Chain();

    EXPECT_EQ(Block::serialize(*chain.get_block(chain.get_last_block_hash())), Block::serialize(*chain.get_last_block()));
}

std::vector<std::unique_ptr<Block>> construct_blocks(Block& previous_block, uint32_t num) {
    std::vector<std::unique_ptr<Block>> output;
    uint32_t previous_block_hash = RathCrypto::hash(Block::serialize(previous_block));
    for (int i = 0; i < num; i++) {
        std::unique_ptr<BlockHeader> block_header = std::make_unique<BlockHeader>(std::rand(), previous_block_hash, 0, 0, 0, 0);
        std::vector<std::unique_ptr<TransactionInput>> txi;
        std::vector<std::unique_ptr<TransactionOutput>> txo;
        std::vector<std::unique_ptr<Transaction>> transactions;
        transactions.push_back(std::make_unique<Transaction>(std::move(txi), std::move(txo)));
        std::unique_ptr<Block> block = std::make_unique<Block>(std::move(block_header), std::move(transactions));

        previous_block_hash = RathCrypto::hash(Block::serialize(*block));
        output.push_back(std::move(block));
    }
    return std::move(output);
}

TEST(Chain, handle_block) {
    Chain chain = Chain();
    std::vector<std::unique_ptr<Block>> blocks = construct_blocks(*chain.get_last_block(), 5);
    for (int i = 0; i < blocks.size(); i++) {
        chain.handle_block(std::move(blocks[i]));
    }
    uint32_t last_block_hash = chain.get_last_block_hash();
    std::vector<std::unique_ptr<Block>> active_chain = chain.get_active_chain(0, chain.get_active_chain_length());

    EXPECT_EQ(Block::serialize(*chain.get_block(last_block_hash)), Block::serialize(*active_chain[5]));
}

TEST(Chain, handle_forked_chain) {
    Chain chain = Chain();
    std::unique_ptr<Block> gen = chain.get_last_block();
    std::vector<std::unique_ptr<Block>> blocks_1 = construct_blocks(*chain.get_last_block(), 3);
    for (int i = 0; i < blocks_1.size(); i++) {
        chain.handle_block(std::move(blocks_1[i]));
    }
    std::vector<std::unique_ptr<Block>> blocks_2 = construct_blocks(*gen, 5);
    for (int i = 0; i < blocks_2.size(); i++) {
        chain.handle_block(std::move(blocks_2[i]));
    }

    uint32_t last_block_hash = chain.get_last_block_hash();
    std::vector<std::unique_ptr<Block>> active_chain = chain.get_active_chain(0, chain.get_active_chain_length());

    EXPECT_EQ(Block::serialize(*chain.get_block(last_block_hash)), Block::serialize(*active_chain[5]));
}