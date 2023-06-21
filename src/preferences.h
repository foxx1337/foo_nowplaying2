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

extern cfg_string playback_format;

extern cfg_string file_path;

extern cfg_uint file_encoding;

extern cfg_bool with_bom;

extern cfg_bool file_append;
