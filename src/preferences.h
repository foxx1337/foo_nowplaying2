#pragma once

#include <SDK/cfg_var.h>

#include <vector>


enum class encoding
{
    UTF8,
    UTF16LE
};

struct encoding_info
{
    const encoding encoding;
    const wchar_t* name;
    const std::vector<unsigned char> bom;
};

extern const std::vector<encoding_info> encodings;

namespace now
{
    // {CA730BF5-9B42-403D-BB57-27C430D9086E}
    constexpr GUID guid_playback_format{0xca730bf5, 0x9b42, 0x403d, {0xbb, 0x57, 0x27, 0xc4, 0x30, 0xd9, 0x8, 0x6e}};
    constexpr char default_playback_format[]{"%artist% - %title%"};
    extern cfg_string playback_format;

    // {E8EB0BA5-877E-4F43-A99A-C23F145578F2}
    constexpr GUID guid_file_path{0xe8eb0ba5, 0x877e, 0x4f43, {0xa9, 0x9a, 0xc2, 0x3f, 0x14, 0x55, 0x78, 0xf2}};
    constexpr char default_file_path[]{""};
    extern cfg_string file_path;

    // {75B61323-4ABB-4692-9483-800B00A30E6B}
    constexpr GUID guid_file_encoding{0x75b61323, 0x4abb, 0x4692, {0x94, 0x83, 0x80, 0xb, 0x0, 0xa3, 0xe, 0x6b}};
    constexpr t_uint32 default_file_encoding{static_cast<t_uint32>(encoding::UTF8)};
    extern cfg_uint file_encoding;

    // {25E80CCB-B77E-428C-AC0C-89DC08531C9B}
    constexpr GUID guid_with_bom{0x25e80ccb, 0xb77e, 0x428c, {0xac, 0xc, 0x89, 0xdc, 0x8, 0x53, 0x1c, 0x9b}};
    constexpr bool default_with_bom{false};
    extern cfg_bool with_bom;

    // {22F1D723-9487-4791-851B-1D2C74E0F4A0}
    constexpr GUID guid_file_append{0x22f1d723, 0x9487, 0x4791, {0x85, 0x1b, 0x1d, 0x2c, 0x74, 0xe0, 0xf4, 0xa0}};
    constexpr bool default_file_append{false};
    extern cfg_bool file_append;

    // {26D45612-B6EC-4B8D-BF27-251639114FA4}
    constexpr GUID guid_max_lines{0x26d45612, 0xb6ec, 0x4b8d, {0xbf, 0x27, 0x25, 0x16, 0x39, 0x11, 0x4f, 0xa4}};
    constexpr t_uint32 default_max_lines{0};
    extern cfg_uint max_lines;

    bool is_used();
} // namespace now

namespace next
{
    // {D027ED67-7D61-476B-B05B-A5FBD0C612BE}
    constexpr GUID guid_playback_format{0xd027ed67, 0x7d61, 0x476b, {0xb0, 0x5b, 0xa5, 0xfb, 0xd0, 0xc6, 0x12, 0xbe}};
    constexpr char default_playback_format[]{"%artist% - %title%"};
    extern cfg_string playback_format;

    // {EBE61D1F-BB8E-46EB-A24B-AC161323406B}
    constexpr GUID guid_file_path{0xebe61d1f, 0xbb8e, 0x46eb, {0xa2, 0x4b, 0xac, 0x16, 0x13, 0x23, 0x40, 0x6b}};
    constexpr char default_file_path[]{""};
    extern cfg_string file_path;

    // {809CDEA8-E575-4D9F-B1E9-FB49965C80C9}
    constexpr GUID guid_use_now{0x809cdea8, 0xe575, 0x4d9f, {0xb1, 0xe9, 0xfb, 0x49, 0x96, 0x5c, 0x80, 0xc9}};
    constexpr bool default_use_now{false};
    extern cfg_bool use_now;

    // {31DF97AB-9BF7-47E0-8FF8-18AC0A1B3EAF}
    constexpr GUID guid_file_encoding{0x31df97ab, 0x9bf7, 0x47e0, {0x8f, 0xf8, 0x18, 0xac, 0xa, 0x1b, 0x3e, 0xaf}};
    constexpr t_uint32 default_file_encoding{static_cast<t_uint32>(encoding::UTF8)};
    extern cfg_uint file_encoding;

    // {0E5AF903-7D03-40FF-8D01-8AEFED6177F8}
    constexpr GUID guid_with_bom{0xe5af903, 0x7d03, 0x40ff, {0x8d, 0x1, 0x8a, 0xef, 0xed, 0x61, 0x77, 0xf8}};
    constexpr bool default_with_bom{false};
    extern cfg_bool with_bom;

    // {59B23536-037D-4F64-803B-B130F22B7C8A}
    constexpr GUID guid_file_append{0x59b23536, 0x37d, 0x4f64, {0x80, 0x3b, 0xb1, 0x30, 0xf2, 0x2b, 0x7c, 0x8a}};
    constexpr bool default_file_append{false};
    extern cfg_bool file_append;

    // {EF11227D-5CF7-4396-B956-CCD8747B016D}
    constexpr GUID guid_max_lines{0xef11227d, 0x5cf7, 0x4396, {0xb9, 0x56, 0xcc, 0xd8, 0x74, 0x7b, 0x1, 0x6d}};
    constexpr t_uint32 default_max_lines{0};
    extern cfg_uint max_lines;

    bool is_used();
} // namespace next
