//
// Created by User on 03/11/2024.
//

#include <gtest/gtest.h>

TEST(ReaderTests, test1) {
    int a = 10 + 15;

    EXPECT_EQ(a, 25);
}

TEST(ReaderTests, test2) {
    int a = 10 + 13;

    EXPECT_EQ(a, 27);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
