//
// Created by vuniverse on 3/16/24.
//

#pragma once

#include <cstdint>
#include <string_view>
#include <filesystem>
#include <cstdio>
#include <cassert>


namespace streamfs
{
    enum struct Mode{ write, read, free_stream, error};
    template<Mode t_Mode = Mode::free_stream, char* t_Extension = "streamfs">
    class File
    {
        typedef std::int8_t m_return_code_type;
        typedef std::int64_t m_size_type;
        typedef std::FILE* m_file_type;
        typedef void(* m_callback_type )(char *, char *);
        typedef char m_buffer_type [];
        typedef char m_info_buffer_type [10];

        m_file_type m_file = nullptr;
        m_callback_type m_encrypt = nullptr;
        m_callback_type m_compress = nullptr;
        Mode m_mode = Mode::error;

    public:
        explicit File(const std::string& a_name) :
        m_mode { t_Mode }
        { assert(p_open(a_name)); }

        ~File(){ std::fclose(m_file); }


        void setEncrypt(m_callback_type callback) { m_encrypt = callback; }
        void setCompress(m_callback_type callback) { m_compress = callback; }
        m_size_type write(){}

    private:
        static m_return_code_type p_check(m_file_type a_file)
        {
            m_info_buffer_type temp;
            std::fread(&temp, sizeof(temp), sizeof(*temp), a_file);
            if(*temp){return 0;}
            else{return 1;}
        }

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
                    m_file = std::fopen((a_name + "." + t_Extension).c_str(), "wb+");
                    break;
                case Mode::read :
                    if ( is_exist ) {
                        m_file = std::fopen((a_name + "." + t_Extension).c_str(), "rb+");
                        if(p_check(m_file)) { m_mode = Mode::error; }
                    } else { m_mode = Mode::error; }
                    break;
                case Mode::free_stream :
                    m_file = std::fopen((a_name + "." + t_Extension).c_str(), "ab+");
                    break;
                default:
                    break;
            }
            if ( m_file ) return 1;
        }
    };
}
