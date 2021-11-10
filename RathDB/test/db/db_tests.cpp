/*
 * This file belongs to Brown University's computer
 * science department as part of csci1951L - Blockchains
 * and cryptocurrencies.
 *
 * This software was developed by Colby Anderson (HTA)
 * and Parker Ljung (TA) of csci1951L in Summer of 2021.
 */

#include <gtest/gtest.h>
#include <db.h>

TEST(Database, GetPutBasic) {
    Database db = Database();
    db.put_safely("keyy", "valuee");
    EXPECT_EQ("valuee", db.get_safely("keyy"));
    db.put_safely("keyy", "tim");
    EXPECT_EQ("tim", db.get_safely("keyy"));
    db.put_safely("hi", "bye");
    EXPECT_EQ("bye", db.get("hi"));
    EXPECT_EQ("", db.get("hello"));
}

TEST(Database, DeleteBasic) {
    Database db = Database();
    db.put_safely("keyy", "valuee");
    EXPECT_EQ("valuee", db.get_safely("keyy"));
    db.delete_safely("keyy");
    EXPECT_EQ("", db.get_safely("keyy"));
}

TEST(Database, BatchGetPutBasic) {
    Database db = Database();
    Batch batch = Batch();
    batch.put("key1", "value1.0");
    batch.put("key2", "value2.0");
    batch.put("key3", "value3.0");
    db.batch_write_safely(batch);
    EXPECT_EQ("value1.0", db.get("key1"));
    EXPECT_EQ("value2.0", db.get("key2"));
    EXPECT_EQ("value3.0", db.get("key3"));
    std::vector<std::string> keys = {"key1", "key2", "key3"};
    std::vector<std::string> values = db.batch_read(keys);
    EXPECT_EQ("value1.0", values.at(0));
    EXPECT_EQ("value2.0", values.at(1));
    EXPECT_EQ("value3.0", values.at(2));
    values = db.batch_read_safely(keys);
    EXPECT_EQ("value1.0", values.at(0));
    EXPECT_EQ("value2.0", values.at(1));
    EXPECT_EQ("value3.0", values.at(2));
}

TEST(Database, BatchGetPutWithDelete) {
    Database db = Database();
    Batch batch = Batch();
    batch.put("key1", "value1.0");
    batch.put("key2", "value2.0");
    batch.delete_("key2");
    db.batch_write_safely(batch);
    EXPECT_EQ("value1.0", db.get("key1"));
    EXPECT_EQ("", db.get("key2"));
}

