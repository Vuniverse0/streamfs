#include <iostream>
#include <vector>
#include "Streamfs.hpp"
#include "HasMethod.hpp"

SeekMethod(data);

int UnitTest1(){
    streamfs::File<streamfs::Mode::free_stream> file1("TestFile4", streamfs::classic);

    char TestBuffer[] = "test one just test rea";


    file1.write(std::size(TestBuffer), TestBuffer);

    auto size = file1.get_read_size();
    std::cout<<"SIZE: "<<size<<std::endl;
    assert(size == std::size(TestBuffer));
    memset(TestBuffer, 0, std::size(TestBuffer));
    file1.read(TestBuffer);
    std::cout<< TestBuffer<<std::endl;

    char TestBuffer2[] = "test two just test rea";

    std::cout<<"\n\n\n\n\n"<<std::endl;

    file1.write(std::size(TestBuffer2), TestBuffer2);

    size = file1.get_read_size();
    std::cout<<"TestBuffer1 SIZE: "<<size<<std::endl;
    assert(size == std::size(TestBuffer));
    memset(TestBuffer, 0, std::size(TestBuffer));
    file1.read(TestBuffer);
    std::cout<< TestBuffer<<std::endl;

     size = file1.get_read_size();
    std::cout<<"SIZE: "<<size<<std::endl;
    assert(size == std::size(TestBuffer2));
    memset(TestBuffer2, 0, std::size(TestBuffer2));
    file1.read(TestBuffer2);
    std::cout<< TestBuffer2<<std::endl;

    std::cout<<"\n\n\n\n\n"<<std::endl;
    char TestBuffer3[] = "test three just test rea";

    file1.write(std::size(TestBuffer3), TestBuffer3);

    size = file1.get_read_size();
    std::cout<<"SIZE: "<<size<<std::endl;
    assert(size == std::size(TestBuffer));
    memset(TestBuffer, 0, std::size(TestBuffer));
    file1.read(TestBuffer);
    std::cout<< TestBuffer<<std::endl;

    size = file1.get_read_size();
    std::cout<<"SIZE: "<<size<<std::endl;
    assert(size == std::size(TestBuffer2));
    memset(TestBuffer2, 0, std::size(TestBuffer2));
    file1.read(TestBuffer2);
    std::cout<< TestBuffer2<<std::endl;

     size = file1.get_read_size();
    std::cout<<"SIZE: "<<size<<std::endl;
    assert(size == std::size(TestBuffer3));
    memset(TestBuffer3, 0, std::size(TestBuffer3));
    file1.read(TestBuffer3);
    std::cout<< TestBuffer3<<std::endl;



}

int main()
{
    UnitTest1();

    return 0;
}
