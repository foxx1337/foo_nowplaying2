#include "formatter.h"

#include <pfc/filetimetools.h>

bool formatter::process_field(titleformat_text_out* p_out, const char* p_name, t_size p_name_length, bool& p_found_flag)
{
    if (stricmp_utf8(p_name, "datetime") == 0)
    {
        auto ts = pfc::fileTimeNow();
        pfc::string8 ts_text{pfc::format_filetimestamp(ts)};
        p_out->write(titleformat_inputtypes::unknown, ts_text);
        p_found_flag = true;
    }
    else
    {
        p_found_flag = false;
    }
    return p_found_flag;
}

bool formatter::process_function(titleformat_text_out* p_out, const char* p_name, t_size p_name_length,
                                 titleformat_hook_function_params* p_params, bool& p_found_flag)
{
    p_found_flag = false;
    return p_found_flag;
}

formatter formatter::m_instance;
