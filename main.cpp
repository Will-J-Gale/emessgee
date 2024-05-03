#include <string>
#include <filesystem>
#include <unistd.h>
#include <sys/mman.h>
#include <cassert>

#include <typedefs.h>

#include <write_memory_block.h>
#include <read_memory_block.h>

int main()
{
    WriteMemoryBlock writer("test", 1000);
    writer.write(0, 244);
    writer.write(1, 65);
    writer.write(2, 22);

    ReadMemoryBlock reader("test");
    std::cout << "Read: " << *reader.read(0) << std::endl;
    std::cout << "Read: " << *reader.read(1) << std::endl;
    std::cout << "Read: " << *reader.read(2) << std::endl;

    return 0;
}