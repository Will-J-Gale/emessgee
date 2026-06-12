#include "gtest/gtest.h"

#include "emessgee/utils.h"

TEST(StringPad, successfully_pads_string)
{
    //Assemble
    std::string string = "sub_test";
    std::string expected = "sub_test_";

    //Act
    std::string result = emessgee::utils::pad_string(string, string.size() + 1);

    //Assert
    EXPECT_EQ(result, expected);
}

TEST(StringPad, string_bigger_than_length_returns_same_string)
{
    //Assemble
    std::string string = "sub_test";
    std::string expected = "sub_test";

    //Act
    std::string result = emessgee::utils::pad_string(string, 1);

    //Assert
    EXPECT_EQ(result, expected);
}

TEST(StringConcat, successfully_concatenates_strings)
{
    //Assemble
    std::string string_1 = "hello";
    std::string string_2 = "there";
    std::string string_3 = "!!!!!";
    std::string expected = string_1 + string_2 + string_3;

    //Act
    std::string result = emessgee::utils::string_concat({string_1, string_2, string_3});

    //Assert
    EXPECT_EQ(result, expected);
}