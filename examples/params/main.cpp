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
        params.write(PARAM_NAME, i);

        std::cout << "Wrote param: " << i << std::endl;
        std::this_thread::sleep_for(1000ms);
    }

    bool terminate = true;
    params.write(TERMINATE, terminate);

    std::cout << "Write finished" << std::endl;
}

int main()
{
    auto current_time = std::chrono::system_clock::now().time_since_epoch();
    double duration = std::chrono::duration<double>(current_time).count() * 1000.0f;
    emessgee::Params params;

    params.write(TERMINATE, false);
    params.write(PARAM_NAME, 0);

    auto [param_addr, param_length] = params.read_raw(PARAM_NAME);
    int* param_raw = reinterpret_cast<int*>(param_addr);

    std::thread w_thread = std::thread(write_thread, 5);

    while(true)
    {
        emessgee::Param<bool> term = params.read<bool>(TERMINATE);
        emessgee::Param<int> int_param = params.read<int>(PARAM_NAME);

        if(term.value)
        {
            break;
        }

        std::cout << "Reading: " << int_param.value << " Raw Ptr: " << *param_raw << " Valid: " << int_param.valid << std::endl;
        std::this_thread::sleep_for(200ms);
    }

    w_thread.join();
    std::cout << "Done" << std::endl;

    return 0;
}
