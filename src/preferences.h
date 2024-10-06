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

    // {0A489F4B-1CA3-4247-9810-777CA9AA92CF}
    constexpr GUID guid_trigger_on_new{0xa489f4b, 0x1ca3, 0x4247, {0x98, 0x10, 0x77, 0x7c, 0xa9, 0xaa, 0x92, 0xcf}};
    constexpr bool default_trigger_on_new{true};
    extern cfg_bool trigger_on_new;

    // {BFFE2DCF-6DFA-499D-93E7-36F449314686}
    constexpr GUID guid_trigger_on_pause{0xbffe2dcf, 0x6dfa, 0x499d, {0x93, 0xe7, 0x36, 0xf4, 0x49, 0x31, 0x46, 0x86}};
    constexpr bool default_trigger_on_pause{true};
    extern cfg_bool trigger_on_pause;

    // {FE8D8F0B-823A-48FC-B307-E479982DADF0}
    constexpr GUID guid_trigger_on_stop{0xfe8d8f0b, 0x823a, 0x48fc, {0xb3, 0x7, 0xe4, 0x79, 0x98, 0x2d, 0xad, 0xf0}};
    constexpr bool default_trigger_on_stop{true};
    extern cfg_bool trigger_on_stop;

    // {06273EDE-4741-43D1-A22C-D77075CB2AA3}
    constexpr GUID guid_trigger_on_time{0x6273ede, 0x4741, 0x43d1, {0xa2, 0x2c, 0xd7, 0x70, 0x75, 0xcb, 0x2a, 0xa3}};
    constexpr bool default_trigger_on_time{false};
    extern cfg_bool trigger_on_time;

    // {8265504D-79DB-4DBC-AD78-D0AB5F4704B8}
    constexpr GUID guid_exit_message{0x8265504d, 0x79db, 0x4dbc, {0xad, 0x78, 0xd0, 0xab, 0x5f, 0x47, 0x4, 0xb8}};
    constexpr char default_exit_message[]{""};
    extern cfg_string exit_message;

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

    // {DD7895AD-B501-40D0-82D9-8018410B3BDE}
    constexpr GUID guid_exit_message{0xdd7895ad, 0xb501, 0x40d0, {0x82, 0xd9, 0x80, 0x18, 0x41, 0xb, 0x3b, 0xde}};
    constexpr char default_exit_message[]{""};
    extern cfg_string exit_message;

    // {3987FE77-8F3F-43CB-B4C5-3FF957D649C2}
    constexpr GUID guid_use_exit_now{0x3987fe77, 0x8f3f, 0x43cb, {0xb4, 0xc5, 0x3f, 0xf9, 0x57, 0xd6, 0x49, 0xc2}};
    constexpr bool default_use_exit_now{false};
    extern cfg_bool use_exit_now;

    bool is_used();
} // namespace next

namespace play_log
{
    // {B9A1E24F-0385-4816-BEFB-9A1C77682FC3}
    constexpr GUID guid_playback_format{0xb9a1e24f, 0x385, 0x4816, {0xbe, 0xfb, 0x9a, 0x1c, 0x77, 0x68, 0x2f, 0xc3}};
    constexpr char default_playback_format[]{"%datetime% %artist% - %title%"};
    extern cfg_string playback_format;

    // {43D9E0EF-EF4A-4BB4-968B-C38F9B501D11}
    constexpr GUID guid_file_path{0x43d9e0ef, 0xef4a, 0x4bb4, {0x96, 0x8b, 0xc3, 0x8f, 0x9b, 0x50, 0x1d, 0x11}};
    constexpr char default_file_path[]{""};
    extern cfg_string file_path;

    // {6955D815-6166-468A-9460-5E60E704FA04}
    constexpr GUID guid_use_now{0x6955d815, 0x6166, 0x468a, {0x94, 0x60, 0x5e, 0x60, 0xe7, 0x4, 0xfa, 0x4}};
    constexpr bool default_use_now{false};
    extern cfg_bool use_now;

    // {ABB8E2C1-74E6-410E-9949-9CAE05A281D5}
    constexpr GUID guid_file_encoding{0xabb8e2c1, 0x74e6, 0x410e, {0x99, 0x49, 0x9c, 0xae, 0x5, 0xa2, 0x81, 0xd5}};
    constexpr t_uint32 default_file_encoding{static_cast<t_uint32>(encoding::UTF8)};
    extern cfg_uint file_encoding;

    // {CF8BD801-89B9-48F5-8D74-435F5A14C528}
    constexpr GUID guid_with_bom{0xcf8bd801, 0x89b9, 0x48f5, {0x8d, 0x74, 0x43, 0x5f, 0x5a, 0x14, 0xc5, 0x28}};
    constexpr bool default_with_bom{false};
    extern cfg_bool with_bom;

    // {B7605109-DFF3-46EC-96E9-8C62D471186D}
    constexpr GUID guid_exit_message{0xb7605109, 0xdff3, 0x46ec, {0x96, 0xe9, 0x8c, 0x62, 0xd4, 0x71, 0x18, 0x6d}};
    constexpr char default_exit_message[]{""};
    extern cfg_string exit_message;

    // {82EDB5A5-99FA-4CCE-9DC2-9E6D566A2F70}
    constexpr GUID guid_use_exit_now{0x82edb5a5, 0x99fa, 0x4cce, {0x9d, 0xc2, 0x9e, 0x6d, 0x56, 0x6a, 0x2f, 0x70}};
    constexpr bool default_use_exit_now{false};
    extern cfg_bool use_exit_now;

    bool is_used();
} // namespace play_log

namespace run
{
    // {3D78EE3E-E815-45B2-BD2B-DA175DCC38F6}
    constexpr GUID guid_commandline{0x3d78ee3e, 0xe815, 0x45b2, {0xbd, 0x2b, 0xda, 0x17, 0x5d, 0xcc, 0x38, 0xf6}};
    constexpr char default_commandline[]{""};
    extern cfg_string commandline;

    // {B1B65C02-781F-4283-AD4B-7EB3B4F3A38F}
    constexpr GUID guid_trigger_on_new{0xb1b65c02, 0x781f, 0x4283, {0xad, 0x4b, 0x7e, 0xb3, 0xb4, 0xf3, 0xa3, 0x8f}};
    constexpr bool default_trigger_on_new{true};
    extern cfg_bool trigger_on_new;

    // {66723C71-8C5B-46C2-B934-0C787EF8B4A7}
    constexpr GUID guid_trigger_on_pause{0x66723c71, 0x8c5b, 0x46c2, {0xb9, 0x34, 0xc, 0x78, 0x7e, 0xf8, 0xb4, 0xa7}};
    constexpr bool default_trigger_on_pause{false};
    extern cfg_bool trigger_on_pause;

    // {D2A1C5EB-0FBD-434E-8EC9-4CBDC47471E8}
    constexpr GUID guid_trigger_on_stop{0xd2a1c5eb, 0xfbd, 0x434e, {0x8e, 0xc9, 0x4c, 0xbd, 0xc4, 0x74, 0x71, 0xe8}};
    constexpr bool default_trigger_on_stop{false};
    extern cfg_bool trigger_on_stop;

    // {393A042D-8AB5-4778-993F-88688C3F075E}
    constexpr GUID guid_trigger_on_time{0x393a042d, 0x8ab5, 0x4778, {0x99, 0x3f, 0x88, 0x68, 0x8c, 0x3f, 0x7, 0x5e}};
    constexpr bool default_trigger_on_time{false};
    extern cfg_bool trigger_on_time;

    // {15583F8D-9534-42DB-B2E8-FFD72093C8C1}
    constexpr GUID guid_hide{0x15583f8d, 0x9534, 0x42db, {0xb2, 0xe8, 0xff, 0xd7, 0x20, 0x93, 0xc8, 0xc1}};
    constexpr bool default_hide{true};
    extern cfg_bool hide;

    bool is_used();

    constexpr wchar_t var_now[]{L"$np2_now"};
    constexpr wchar_t var_next[]{L"$np2_next"};
    constexpr wchar_t var_log[]{L"$np2_log"};
    constexpr wchar_t var_event[]{L"$np2_event"};
} // namespace run
