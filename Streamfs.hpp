//
// Created by vuniverse on 3/16/24.
//

#pragma once

#include <cstdint>
#include <string_view>
#include <filesystem>
#include <cstdio>
#include <cassert>

/* Use document
 * construct main styles are classic and table, also can be added
 * any encryption or/and compression, for this use styles like:
 * table + encrypted // encryption and table.
 * table + compressed // compression and table.
 * compressed + encrypted // compressed_encrypted.
 * table + compressed + encrypted == table_compressed_encrypted
 * classic style is using by default
 */

namespace streamfs
{
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
                          cannot_open_file};
        typedef std::int8_t m_return_code_type;
        typedef std::int64_t m_size_type;
        typedef std::FILE* m_file_type;
        typedef void(* m_callback_type )(char *, char *);
        typedef char m_buffer_type [];
        typedef char *m_buffer_pointer;
        typedef char m_info_buffer_type [32];

        /* 1 byte Style
         * 4 bytes check code
         * 27 for future using
         */

        m_file_type m_file = nullptr;
        m_callback_type m_encrypt = nullptr;
        m_callback_type m_compress = nullptr;
        Mode m_mode = t_Mode;
        Style m_style;
        State m_state = State::error;
        std::string_view m_extension;

    public:
        explicit File(const std::string& a_name, Style a_style = classic,
                      std::string_view a_extension = ".streamfs")
        :m_style {a_style}
        ,m_extension{a_extension}
        {
            if(p_open(a_name+m_extension.data()) != 0){
                m_mode = Mode::error;
                m_state = State::cannot_open_file;
                return;
            }
        }

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

        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        m_size_type read(m_size_type a_size, m_buffer_pointer a_p) //returns how many bytes has read
        {

        }

        template<typename Container>
        Container& operator<<(Container&& a_container)
        {
            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for(;begin_it != end_it; ++begin_it){

            }
            return std::forward<Container>(a_container);
        }

        template<typename Container>
        Container& operator>>(Container&& a_container)
        {
            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for(;begin_it != end_it; ++begin_it){

            }
            return std::forward<Container>(a_container);
        }

        std::string getErrors()
        {
            std::string error_message{"\n"};

            if(m_state == State::cannot_open_file)
                error_message += "Cannot open file!\n";

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

        m_return_code_type p_open(const std::string& a_name)
        {

            bool is_exist = [&]{
                m_file_type file = fopen(a_name.c_str(), "r");
                bool exist = ( file != nullptr );
                fclose(file);
                return exist;
            }();
            switch (m_mode) {
                case Mode::write :
                    m_file = std::fopen(a_name.c_str(), "wb+");
                    break;
                case Mode::read :
                    if ( is_exist ) {
                        m_file = std::fopen(a_name.c_str(), "rb+");
                        //if(p_check(m_file)) { m_mode = Mode::error; }
                    } else { m_mode = Mode::error; }
                    break;
                case Mode::free_stream :
                    m_file = std::fopen(a_name.c_str(), "ab+");
                    break;
                default:
                    break;
            }
            if ( m_file ) return 0;
            return 1;
        }
    };
}
