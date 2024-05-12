#include <iostream>
#include <vector>
#include "Streamfs.hpp"
#include "HasMethod.hpp"

SeekMethod(data);



int main()
{

    FILE* file = fopen( "filename", "rb+");
    //fwrite(g, 15, 1, file);
    fclose(file);
    file = fopen( "filename", "rb+");
    streamfs::File<streamfs::Mode::free_stream> test("test", streamfs::classic);
    return 0;
}
