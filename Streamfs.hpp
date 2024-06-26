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

namespace sfs_prvt {

#include "HasMethod.hpp"

    SeekMethod(data);
}

namespace streamfs {

    enum struct Mode {
        error, write, read, free_stream
    };

    enum Style {
        classic, table, encrypted, table_encrypted, compressed,
        table_compressed, compressed_encrypted, table_compressed_encrypted, style_error, get_style
    };

    static_assert(table + encrypted == table_encrypted);
    static_assert(table + compressed == table_compressed);
    static_assert(table + compressed_encrypted == table_compressed_encrypted);
    static_assert(compressed + encrypted == compressed_encrypted);
    static_assert(table + compressed + encrypted == table_compressed_encrypted);

    //class IFile {};

    template<Mode t_Mode = Mode::free_stream, typename H = uint8_t>
    class File {
        enum struct State {
            error, ok, process, undefined,
            encrypt_failed, compress_failed,
            decrypt_failed, decompress_failed,
            cannot_open_file, freestream_without_style, wrong_constructor,
            write_error, read_error, read_stopped, read_no_size, read_size_error,
            data_state_error, end_of_file, size
        };

        constexpr static std::array<std::pair<State, std::string_view>, static_cast<std::size_t>(State::size)>
                error_messages{{
                                       {State::error, "Error\n"},
                                       {State::ok, "No errors occurred\n"},
                                       {State::process, "File in superposition, need to finish last procedure\n"},
                                       {State::undefined, "Undefined error"},
                                       {State::encrypt_failed, "Encrypt failed\n"},
                                       {State::compress_failed, "Compress failed\n"},
                                       {State::decrypt_failed, "Decrypt failed\n"},
                                       {State::decompress_failed, "Decompress failed\n"},
                                       {State::cannot_open_file, "Cannot open file!\n"},
                                       {State::freestream_without_style, "File and style are empty, can't create\n"},
                                       {State::wrong_constructor, "Wrong callbacks or style\n"},
                                       {State::write_error, "Write error, check buffer and permissions\n"},
                                       {State::read_error, "Read error, check buffer and permissions\n"},
                                       {State::read_stopped, "Read operation stopped, written new data\n"},
                                       {State::read_no_size, "You must request size at first, to avoid segfault\n"},
                                       {State::read_size_error, "Buffer size is smaller than needed\n"},
                                       {State::data_state_error, "Wrong state of data, data corrupted\n"},
                                       {State::end_of_file, "End of file\n"}
                               }};

        enum struct DataState {
            error, ok, deleted, in_process, write_did, read_did
        } dataState = DataState::in_process;

        typedef std::int8_t m_return_code_type;
        typedef std::int64_t m_size_type;
        typedef std::FILE *m_file_type;
        typedef char m_data_type;
        typedef m_data_type m_buffer_type[];
        typedef m_data_type *m_buffer_pointer;
        typedef m_data_type const *m_const_buffer_pointer;
        typedef m_data_type m_info_buffer_type[32];

        typedef std::unique_ptr<m_data_type[]>(*m_callback_type )(m_size_type, m_const_buffer_pointer);

        template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
        using m_hash_type = T (*)(m_size_type, m_const_buffer_pointer);


        /* 1 byte Style
         * 8 bytes check code
         * 23 for future using
         */

        m_file_type m_file = nullptr;
        m_callback_type m_encrypt = nullptr;
        m_callback_type m_compress = nullptr;
        m_hash_type<H> m_hash = nullptr;
        m_info_buffer_type m_info{"0vunivers"};
        Style m_style;
        std::vector<State> m_state{State::ok};

    public:
        File(const std::string &a_name, Style a_style)
                : m_style{a_style} {

            if(a_style == get_style){

            }

            if (a_style != classic && a_style != table) {
                m_state[0] = State::error;
                m_state.emplace_back(State::wrong_constructor);
                return;
            }

            if (m_open(a_name) != 0)
                m_state[0] = State::error;
        }

        File(const std::string &a_name, Style a_style, m_callback_type callback)
                : m_style{a_style} {
            if (a_style == compressed)
                m_compress = callback;
            else if (a_style == encrypted)
                m_encrypt = callback;
            else {
                m_state[0] = State::error;
                m_state.emplace_back(State::wrong_constructor);
                return;
            }
            if (m_open(a_name) != 0)
                m_state[0] = State::error;
        }

        File(const std::string &a_name, Style a_style,
             m_callback_type callback_encrypt, m_callback_type callback_compress) //encrypt and compress
                : m_style{a_style}, m_encrypt{callback_encrypt}, m_compress{callback_compress} {
            if (m_open(a_name) != 0)
                m_state[0] = State::error;
        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        explicit File(const std::string &a_name) : File(a_name, Style::get_style) {}

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        explicit File(const std::string &a_name,
                      m_callback_type callback_encrypt, m_callback_type callback_compress)
                      : File(a_name, Style::get_style) {}

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        explicit File(const std::string &a_name, m_callback_type callback) : File(a_name, Style::get_style) {}

        ~File() { std::fclose(m_file); }

        Style style(){
            return m_style;
        }

        /* classic
         * size
         *
         *
         */

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::write, bool> = true>
        m_size_type write(m_size_type a_size, m_buffer_pointer a_p) //returns how many bytes has writen
        {
            if (a_p == nullptr || a_size == 0 || m_state[0] == State::error) {
                m_state.emplace_back(State::write_error);
                return 0;
            }

            static_assert(sizeof(m_size_type) == 8, "SIZE OF SIZE VARIABLE ERROR");
            static_assert(sizeof(DataState) == 4, "SIZE OF DATA STATE VARIABLE ERROR");

            auto dataStatePos = std::ftell(m_file);
            dataStatePos = -1;

            if(dataState != DataState::write_did){
                m_state.emplace_back(State::read_stopped);
                std::fseek(m_file, 0, SEEK_END);
                std::cout<<"RESET";
            }

            dataState = DataState::in_process;
            m_size_type writen_size = 0;
            switch (m_style) {
                case classic:
                    writen_size += std::fwrite(&a_size, 1, sizeof(a_size), m_file);
                    dataStatePos = std::ftell(m_file);

                    writen_size += std::fwrite(&dataState, 1, sizeof(dataState), m_file);
                    writen_size += std::fwrite(a_p, 1, a_size, m_file);
                    break;
                case table:
                    break;
                case encrypted:
                    break;
                case table_encrypted:
                    break;
                case compressed:
                    break;
                case table_compressed:
                    break;
                case compressed_encrypted:
                    break;
                case table_compressed_encrypted:
                    break;
            }
            //fwrite(a_p, a_size, 1, m_file);

            if(writen_size != sizeof(a_size) + sizeof(dataState) + a_size){
                dataState = DataState::error;
                m_state.emplace_back(State::write_error);
                m_state[0] = State::error;

                return writen_size;
            }

            std::fseek(m_file, dataStatePos, SEEK_SET);
            dataState = DataState::ok;
            writen_size = std::fwrite(&dataState, 1, sizeof(dataState), m_file);
            std::fseek(m_file, 0, SEEK_END);

            if(writen_size != sizeof(dataState)){
                dataState = DataState::error;
                m_state.emplace_back(State::write_error);
                m_state[0] = State::error;
                return writen_size;
            }
            std::cout<< "ALL OK;"<<std::endl;
            dataState = DataState::write_did;
            return writen_size;

            //auto buffer = std::make_unique<std::remove_reference_t<decltype(*a_p)>[]>(a_size);
        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::write, bool> = true>
        m_size_type write_stream(m_size_type a_size, m_buffer_pointer a_p) //returns how many bytes has writen
        {
            if (m_style)
                fwrite(a_p, 1, a_size, m_file);

            if (!m_file || !a_p || a_size == 0)
                return 0;
            auto buffer = std::make_unique<std::remove_reference_t<decltype(*a_p)>[]>(a_size);
            fwrite(a_p, 1, a_size, m_file);
        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::write, bool> = true>
        m_size_type write_end(m_size_type a_size, m_buffer_pointer a_p) //returns how many bytes has writen
        {}

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        m_size_type get_read_size() {
            if(dataState == DataState::write_did)
                std::fseek(m_file, 0, SEEK_SET);

            m_size_type a_size, read_size;
            read_size = fread(&a_size, 1, sizeof(a_size), m_file);
            std::cout << "Read size:: " << a_size << std::endl;
            if(read_size != sizeof(a_size)){
                m_state.emplace_back(State::end_of_file);
                m_state[0] = State::error;
                return 0;
            }

            std::fseek(m_file, 0l-static_cast<std::int64_t>(sizeof(m_size_type) ), SEEK_CUR);
            dataState = DataState::read_did;
            return a_size;
            //add checks here for data state
        }


        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        m_size_type read(m_buffer_pointer a_p) //returns how many bytes has read
        {
            if(dataState == DataState::write_did){
                m_state.emplace_back(State::read_no_size);
                return 0;
            }
            m_size_type a_size, read_size = 0;

            switch (m_style) {
                case classic:

                    read_size = fread(&a_size, 1, sizeof(a_size), m_file);
                    if(a_size > sizeof(a_p) || read_size != sizeof(m_size_type)) {
                        m_state.emplace_back(State::read_size_error);
                        m_state[0] = State::error;
                        return 0;
                    }

                    read_size = fread(&dataState, 1, sizeof(DataState), m_file);
                    if(dataState != DataState::ok || read_size != sizeof(DataState)){
                        m_state.emplace_back(State::data_state_error);
                        m_state.emplace_back(State::end_of_file);
                        m_state[0] = State::error;
                        return 0;
                    }

                    read_size = fread(a_p, 1, a_size, m_file);
                    break;
                case table:
                    break;
                case encrypted:
                    break;
                case table_encrypted:
                    break;
                case compressed:
                    break;
                case table_compressed:
                    break;
                case compressed_encrypted:
                    break;
                case table_compressed_encrypted:
                    break;
            }

            //if(!m_file || !a_p || a_size == 0)
            dataState = DataState::read_did;
            return read_size;
        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        std::unique_ptr<m_data_type *> undefided_get_release(m_buffer_pointer a_p) {
            assert(false);

        }

        template<std::enable_if_t<t_Mode == Mode::free_stream || t_Mode == Mode::read, bool> = true>
        m_size_type plain_read(m_size_type a_size, m_buffer_pointer a_p) //returns how many bytes has read
        {
            switch (m_style) {
                case classic:
                    break;
                case table:
                    break;
                case encrypted:
                    break;
                case table_encrypted:
                    break;
                case compressed:
                    break;
                case table_compressed:
                    break;
                case compressed_encrypted:
                    break;
                case table_compressed_encrypted:
                    break;
            }

            if (!m_file || !a_p || a_size == 0)
                return 0;
            fread(a_p, 1, a_size, m_file);
        }

        template<typename Container, std::enable_if_t<!sfs_prvt::Has_dataMethod<Container>::value, bool> = true>
        Container &operator<<(Container &&a_container) //have no data method
        {
            std::cout<< "no container with data method" << std::endl;
            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for (; begin_it != end_it; ++begin_it) {

            }
            return std::forward<Container>(a_container);
        }

        template<typename Container, std::enable_if_t<!sfs_prvt::Has_dataMethod<Container>::value, bool> = true>
        Container &operator>>(Container &&a_container) //have no data method
        {
            std::cout<< "no container with data method" << std::endl;

            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for (; begin_it != end_it; ++begin_it) {

            }
            return std::forward<Container>(a_container);
        }

        template<typename Container, std::enable_if_t<sfs_prvt::Has_dataMethod<Container>::value, bool> = true>
        Container &operator<<(Container &&a_container) {
            std::cout<< "container with data method" << std::endl;

            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for (; begin_it != end_it; ++begin_it) {

            }
            return std::forward<Container>(a_container);
        }

        template<typename Container, std::enable_if_t<sfs_prvt::Has_dataMethod<Container>::value, bool> = true>
        Container &operator>>(Container &&a_container) {
            std::cout<< "container with data method" << std::endl;

            auto begin_it = std::begin(a_container), end_it = std::end(a_container);
            for (; begin_it != end_it; ++begin_it) {

            }
            return std::forward<Container>(a_container);
        }

        std::string get_errors() {
            std::string error_message{"\n"};

            for (auto &state: m_state) {
                assert(error_messages[static_cast<std::size_t>(state)].first == state);
                error_message += error_messages[static_cast<std::size_t>(state)].second;
            }

            if (m_style == encrypted
                || m_style == compressed_encrypted
                || m_style == table_encrypted
                || m_style == table_compressed_encrypted
                   && m_encrypt == nullptr) {
                error_message += "Style contains encrypt but callback has not setted!\n";
            }

            if (m_style == compressed
                || m_style == compressed_encrypted
                || m_style == table_compressed
                || m_style == table_compressed_encrypted
                   && m_compress == nullptr) {
                error_message += "Style contains compress but callback has not setted!\n";
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
            m_info_buffer_type info;
            fread(m_info, 1, sizeof(*m_info) * std::size(m_info), m_file);
            switch (m_style) {
                case streamfs::classic:
                    break;
                case streamfs::table:
                    break;
                case streamfs::encrypted:
                    break;
                case streamfs::table_encrypted:
                    break;
                case streamfs::compressed:
                    break;
                case streamfs::table_compressed:
                    break;
                case streamfs::compressed_encrypted:
                    break;
                case streamfs::table_compressed_encrypted:
                    break;
                case streamfs::style_error:
                    break;
                case streamfs::get_style:
                    break;
                default:
                    //error
                    break;
            }
            return 0;
        }

        m_return_code_type m_open(const std::string &a_name) {

            auto is_exist = [&]() -> bool {
                m_file_type file = fopen(a_name.c_str(), "rb");
                bool exist = (file != nullptr);
                if (exist) fclose(file);
                return exist;
            };

            switch (t_Mode) {
                case Mode::write :
                    m_file = std::fopen(a_name.c_str(), "wb+");
                    //!!! write(std::size(m_info) * sizeof(*m_info), m_info);
                    break;
                case Mode::read :
                    if (is_exist()) {
                        m_file = std::fopen(a_name.c_str(), "rb+");
                        //!!! read(std::size(m_info) * sizeof(*m_info), m_info);
                        m_check();
                        //if(p_check(m_file)) { m_mode = Mode::style_error; }
                    } else {
                        m_state.emplace_back(State::cannot_open_file);
                        return 1;
                    }
                    break;
                case Mode::free_stream :
                    if (is_exist()) {
                        m_file = std::fopen(a_name.c_str(), "rb+");
                        //!!! read(std::size(m_info) * sizeof(*m_info), m_info);
                        m_check();
                    } else if (m_style != Style::style_error) {
                        m_file = std::fopen(a_name.c_str(), "wb+");
                        //!!! write(std::size(m_info) * sizeof(*m_info), m_info);
                    } else {
                        m_state.emplace_back(State::freestream_without_style);
                        return 1;
                    }
                    break;
                default:
                    break;
            }
            if (m_file) return 0;
            m_state.emplace_back(State::cannot_open_file);
            return 1;
        }
    };
}

std::ostream& operator << ( std::ostream& stream, streamfs::Style style)
{
    switch (style) {
        case streamfs::classic:
            stream << "classic";
            break;
        case streamfs::table:
            stream << "table";
            break;
        case streamfs::encrypted:
            stream << "encrypted";
            break;
        case streamfs::table_encrypted:
            stream << "table_encrypted";
            break;
        case streamfs::compressed:
            stream << "compressed";
            break;
        case streamfs::table_compressed:
            stream << "table_compressed";
            break;
        case streamfs::compressed_encrypted:
            stream << "compressed_encrypted";
            break;
        case streamfs::table_compressed_encrypted:
            stream << "table_compressed_encrypted";
            break;
        case streamfs::style_error:
            stream << "style_error";
            break;
        case streamfs::get_style:
            stream << "get_style";
            break;
        default:
            stream <<"OUTBOUNDS";
            break;
    }
    return stream;
}

#endif //STREAMFS_HEADER