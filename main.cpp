#include "Streamfs.hpp"

int main()
{
    streamfs::File<> file("test_name1", streamfs::table_encrypted);
    return 0;
}
