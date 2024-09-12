#include <iostream>

#include <gtest/gtest.h>
#include <emessgee.h>

int main(int argc, char *argv[]) 
{
    emessgee::utils::create_tmp_folder();
    emessgee::utils::clean_temp_folder();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}