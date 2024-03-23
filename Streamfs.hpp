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
    template<char* Extension = "streamfs">
    class File
    {
        typedef std::int8_t m_return_code_type;
        typedef std::int64_t m_size_type;
        typedef std::FILE *m_file_type;
        typedef void(*m_callback_type)(char *, char *);

        m_file_type m_file = nullptr;
        m_callback_type m_encrypt = nullptr;
        m_callback_type m_compress = nullptr;
        Mode m_mode = Mode::error;

    public:
        explicit File(const std::string& name, Mode mode = Mode::free_stream) :
        m_mode { mode }
        { assert(open(name)); }

        ~File(){ std::fclose(m_file); }


        void setEncrypt(m_callback_type callback) { m_encrypt = callback; }
        void setCompress(m_callback_type callback) { m_compress = callback; }
        m_size_type write(){}

    private:
        m_return_code_type open(const std::string& name)
        {

            bool is_exist = [&]{
                m_file_type file = fopen(name.c_str(), "r");
                bool exist = ( file != nullptr );
                fclose(file);
                return exist;
            }();
            switch (m_mode) {
                case Mode::write :
                    m_file = std::fopen((name + "." + Extension).c_str(), "wb+");
                    break;
                case Mode::read :
                    if ( is_exist ) { m_file = std::fopen((name + "." + Extension).c_str(), "rb+"); }
                    else { m_mode = Mode::error; }
                    break;
                case Mode::free_stream :
                    m_file = std::fopen((name + "." + Extension).c_str(), "ab+");
                    break;
                default:
                    break;
            }
            if ( m_file ) return 1;
        }

    };

}
