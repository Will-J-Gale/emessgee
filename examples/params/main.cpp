#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#include "emessgee/emessgee.h"

using namespace std::chrono_literals;

std::string PARAM_NAME = "test_param";
std::string TERMINATE = "terminate";

void write_thread(uint num_loops=10)
{
    emessgee::Params params;

    for(int i = 0; i < num_loops; i++)
    {
        params.write_int(PARAM_NAME, i);

        std::cout << "Wrote param: " << i << std::endl;
        std::this_thread::sleep_for(1000ms);
    }

    bool terminate = true;
    params.write_bool(TERMINATE, terminate);

    std::cout << "Write finished" << std::endl;
}

int main()
{
    auto current_time = std::chrono::system_clock::now().time_since_epoch();
    double duration = std::chrono::duration<double>(current_time).count() * 1000.0f;
    emessgee::Params params;

    params.write_bool(TERMINATE, false);
    params.write_int(PARAM_NAME, 0);

    auto [param_addr, param_length] = params.read_addr(PARAM_NAME);
    int* param_raw = reinterpret_cast<int*>(param_addr);

    std::thread w_thread = std::thread(write_thread, 5);

    while(true)
    {
        bool term = params.read_bool(TERMINATE);
        int int_param = params.read_int(PARAM_NAME);

        if(term)
        {
            break;
        }

        std::cout << "Reading: " << int_param << " Raw Ptr: " << *param_raw << std::endl;
        std::this_thread::sleep_for(200ms);
    }

    w_thread.join();
    std::cout << "Done" << std::endl;

    return 0;
}
