#include <iostream>
#include <vector>
#include "Streamfs.hpp"
#include "HasMethod.hpp"

SeekMethod(data);

struct test {
    template<typename Container, std::enable_if_t<Has_dataMethod<Container>::value, bool> = true>
    Container& operator<<(Container&& a_container)
    {
        std::cout << "free data: "<< *a_container.data()<< " size " <<std::endl;
    }
    template<typename Container, std::enable_if_t<!Has_dataMethod<Container>::value, bool> = true>
    Container& operator<<(Container&& a_container)
    {
        std::cout << "NO FREE data: ";
    }

    //template<typename Container, std::enable_if_t<!Has_dataMethod<Container>::value, bool> = true>
    //Container& operator<<(Container&& a_container)
    //{
      //  std::cout << "non fre data: ";
    //}

    //template<typename Container>
    //Container& operator<<(Container&& a_container)<Container std::void_t<decltype(std::declval<Container>.data())>> : true_type
    //{
      //  std::cout << "free data: "<< *a_container.data();
    //}
   /* template<typename Container>
    Container& operator<<(Container&& a_container) //check has .data() call another
    {
        auto begin_it = std::begin(a_container), end_it = std::end(a_container);
        for(;begin_it != end_it; ++begin_it){
            std::cout<<*begin_it<<std::endl;
        }
        return std::forward<Container>(a_container);
    }
*/
};


class ga{};



int main()
{
    decltype(std::declval<std::vector<std::string>>().data()) a;
    test t;
    int ab;
    std::vector<std::string> vec {"one", "two", "three"};
    t<<vec;
    t<<ab;
    //std::cout << "free data: "<< *vec.data();
    //test<std::vector<std::string>> a;
    //std::declval<ga>();
    //streamfs::File<> file("test_name1", streamfs::table_encrypted);
    const char * g = "text read some";
    char b[20];
    FILE* file = fopen( "filename", "rb+");
    //fwrite(g, 15, 1, file);
    fclose(file);
    file = fopen( "filename", "rb+");
    fread(b, 20, 1, file);

    std::cout<< b << std::endl;
    return 0;
}
