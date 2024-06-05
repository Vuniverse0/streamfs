#include <iostream>
#include <vector>
#include "Streamfs.hpp"
#include "HasMethod.hpp"

SeekMethod(data);

int main()
{

    FILE* file = fopen( "filename", "rb+");
    streamfs::File<streamfs::Mode::free_stream> file1("File1", streamfs::classic);
    //file1.write(1, nullptr);
    auto errors = file1.get_errors();
    std::cout << errors << std::endl;
    // streamfs::File<streamfs::Mode::free_stream> test("test", streamfs::classic);
    return 0;
}
