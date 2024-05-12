//
// Created by vuniverse on 3/16/24.
//

#ifndef STREAMFS_HEADER
#define STREAMFS_HEADER

#include <cstdint>
#include <string_view>
#include <filesystem>
#include <cstdio>
#include <cassert>
#include <cstring>


/* Use document
 * construct main styles are classic and table, also can be added
 * any encryption or/and compression, for this use styles like:
 * table + encrypted // encryption and table.
 * table + compressed // compression and table.
 * compressed + encrypted // compressed_encrypted.
 * table + compressed + encrypted == table_compressed_encrypted
 * classic style is using by default
 */

namespace sfs_prvt
{
#include "HasMethod.hpp"
    SeekMethod(data);
}

namespace streamfs
{
    constexpr char no_extension[] = ".";
    enum struct Mode{ error, write, read, free_stream };
    enum Style{
        classic, table, encrypted, table_encrypted, compressed,
        table_compressed, compressed_encrypted, table_compressed_encrypted, error
    };

    static_assert(table + encrypted == table_encrypted);
    static_assert(table + compressed == table_compressed);
    static_assert(table + compressed_encrypted == table_compressed_encrypted);
    static_assert(compressed + encrypted == compressed_encrypted);
    static_assert(table + compressed + encrypted == table_compressed_encrypted);

    template<Mode t_Mode = Mode::free_stream>
    class File
    {
        enum struct State{error, ok, process, none,
                          encrypt_failed, compress_failed,
                          decrypt_failed, decompress_failed,
                          cannot_open_file, freestream_without_style};
        typedef std::int8_t m_return_code_type;
        typedef std::int64_t m_size_type;
        typedef std::FILE* m_file_type;
        typedef char m_data_type;
        typedef m_data_type m_buffer_type [];
        typedef m_data_type *m_buffer_pointer;
        typedef m_data_type const *m_const_buffer_pointer;
        typedef m_data_type m_info_buffer_type [32];
        typedef std::unique_ptr<m_data_type[]>(* m_callback_type )(m_size_type, m_const_buffer_pointer);


        /* 1 byte Style
         * 8 bytes check code
         * 23 for future using
         */

        m_file_type m_file = nullptr;
        m_callback_type m_encrypt = nullptr;
        m_callback_type m_compress = nullptr;
        m_info_buffer_type m_info {"0vuniverse"};
        Style m_style{error};
        std::vector<State> m_state{State::ok};

    public:
        explicit File(const std::string& a_name, Style a_style = classic)
        :m_style {a_style}
        {
            if(m_open(a_name) != 0)
                m_state[0] = State::error;
        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        explicit File(const std::string& a_name) : File(a_name, Style::error){}

        ~File(){ std::fclose(m_file); }

        void setEncrypt(m_callback_type callback) { m_encrypt = callback; }

        void setCompress(m_callback_type callback) { m_compress = callback; }

        m_return_code_type run() //call when encryption or/and compression callback/s setted
        {
            //check code try decrypt
        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::write, bool> = true>
        m_size_type write(m_size_type a_size, m_buffer_pointer a_p) //returns how many bytes has writen
        {
            switch (m_style) {
                classic:
                    fwrite(&a_size, sizeof(a_size), 1, m_file);
                    break;
                table:

                    break;
                encrypted:
                    break;
                table_encrypted:
                    break;
                compressed:
                    break;
                table_compressed:
                    break;
                compressed_encrypted:
                    break;
                table_compressed_encrypted:
                    break;
            }
            fwrite(a_p, a_size, 1, m_file);

            if(!m_file || !a_p || a_size == 0)
                return 0;
            auto buffer = std::make_unique<std::remove_reference_t<decltype(*a_p)>[]>(a_size);
            fwrite(a_p, a_size, 1, m_file);
        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::write, bool> = true>
        m_size_type write_stream(m_size_type a_size, m_buffer_pointer a_p) //returns how many bytes has writen
        {
            if(m_style)
                fwrite(a_p, a_size, 1, m_file);

            if(!m_file || !a_p || a_size == 0)
                return 0;
            auto buffer = std::make_unique<std::remove_reference_t<decltype(*a_p)>[]>(a_size);
            fwrite(a_p, a_size, 1, m_file);
        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::write, bool> = true>
        m_size_type write_end(m_size_type a_size, m_buffer_pointer a_p) //returns how many bytes has writen
        {}

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        m_size_type get_read_size(m_buffer_pointer a_p)
        {

        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        m_size_type read(m_buffer_pointer a_p) //returns how many bytes has read
        {
            switch (m_style) {
                classic:
                    break;
                table:
                    break;
                encrypted:
                    break;
                table_encrypted:
                    break;
                compressed:
                    break;
                table_compressed:
                    break;
                compressed_encrypted:
                    break;
                table_compressed_encrypted:
                    break;
            }

            //if(!m_file || !a_p || a_size == 0)
                return 0;
            //fread(a_p, a_size, 1, m_file);
        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        std::unique_ptr<m_data_type*> get_read_size(m_buffer_pointer a_p)
        {

        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        m_size_type plain_read(m_size_type a_size, m_buffer_pointer a_p) //returns how many bytes has read
        {
            switch (m_style) {
                classic:
                    break;
                table:
                    break;
                encrypted:
                    break;
                table_encrypted:
                    break;
                compressed:
                    break;
                table_compressed:
                    break;
                compressed_encrypted:
                    break;
                table_compressed_encrypted:
                    break;
            }

            if(!m_file || !a_p || a_size == 0)
                return 0;
            fread(a_p, a_size, 1, m_file);
        }

        template<typename Container,  std::enable_if_t<!sfs_prvt::Has_dataMethod<Container>::value, bool> = true>
        Container& operator<<(Container&& a_container) //have no data method
        {
            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for(;begin_it != end_it; ++begin_it){

            }
            return std::forward<Container>(a_container);
        }

        template<typename Container,  std::enable_if_t<!sfs_prvt::Has_dataMethod<Container>::value, bool> = true>
        Container& operator>>(Container&& a_container) //have no data method
        {
            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for(;begin_it != end_it; ++begin_it){

            }
            return std::forward<Container>(a_container);
        }

        template<typename Container, std::enable_if_t<sfs_prvt::Has_dataMethod<Container>::value, bool> = true>
        Container& operator<<(Container&& a_container)
        {
            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for(;begin_it != end_it; ++begin_it){

            }
            return std::forward<Container>(a_container);
        }

        template<typename Container, std::enable_if_t<sfs_prvt::Has_dataMethod<Container>::value, bool> = true>
        Container& operator>>(Container&& a_container)
        {
            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for(;begin_it != end_it; ++begin_it){

            }
            return std::forward<Container>(a_container);
        }

        std::string get_errors()
        {
            std::string error_message{"\n"};

            if(m_state == State::cannot_open_file)
                error_message += "Cannot open file!\n";
            else if(m_state == State::freestream_without_style)
                error_message += "\n";

            if(m_style == encrypted
            || m_style == compressed_encrypted
            || m_style == table_encrypted
            || m_style == table_compressed_encrypted
            && m_encrypt == nullptr){
                error_message += "Style contains encrypt but callback has not setted!\n";
            }else if(m_state == State::encrypt_failed){
                error_message += "Encrypt failed\n";
            }else if(m_state == State::decrypt_failed){
                error_message += "Decrypt failed\n";
            }

            if(m_style == compressed
            || m_style == compressed_encrypted
            || m_style == table_compressed
            || m_style == table_compressed_encrypted
            && m_compress == nullptr){
                error_message += "Style contains compress but callback has not setted!\n";
            }else if(m_state == State::compress_failed){
                error_message += "Compress failed\n";
            }else if(m_state == State::decompress_failed){
                error_message += "Decompress failed\n";
            }

            return std::move(error_message);
        }

    private:

        /*static m_return_code_type p_check(m_file_type a_file)
        {
            m_info_buffer_type temp;
            std::fread(&temp, sizeof(temp), sizeof(*temp), a_file);
            if(*temp){return 0;}
            else{return 1;}
        }*/
        m_return_code_type m_check() //check modes and structures
        {
            switch (m_style) {

            }
                m_info;
            return 0;
        }

        m_return_code_type m_open(const std::string& a_name)
        {

            auto is_exist = [&]() -> bool {
                m_file_type file = fopen(a_name.c_str(), "rb");
                bool exist = ( file != nullptr );
                if(exist) fclose(file);
                return exist;
            };

            switch (t_Mode) {
                case Mode::write :
                    m_file = std::fopen(a_name.c_str(), "wb+");
                    //!!! write(std::size(m_info) * sizeof(*m_info), m_info);
                    break;
                case Mode::read :
                    if(is_exist()){
                        m_file = std::fopen(a_name.c_str(), "rb+");
                        //!!! read(std::size(m_info) * sizeof(*m_info), m_info);
                        m_check();
                        //if(p_check(m_file)) { m_mode = Mode::error; }
                    }else{
                        m_state.insert(State::cannot_open_file);
                        return 1;
                    }
                    break;
                case Mode::free_stream :
                    if(is_exist()){
                        m_file = std::fopen(a_name.c_str(), "rb+");
                        //!!! read(std::size(m_info) * sizeof(*m_info), m_info);
                        m_check();
                    }else if(m_style != Style::error){
                        m_file = std::fopen(a_name.c_str(), "wb+");
                        //!!! write(std::size(m_info) * sizeof(*m_info), m_info);
                    }else{
                        m_state.insert(State::freestream_without_style);
                        return 1;
                    }
                    break;
                default:
                    break;
            }
            if(m_file) return 0;
            m_state.insert(State::cannot_open_file);
            return 1;
        }
    };
}

#endif //STREAMFS_HEADER