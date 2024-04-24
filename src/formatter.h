#pragma once

//#include <SDK/titleformat.h>
//#include <pfc/int_types.h>
#include <SDK/foobar2000.h>

// Used to understand %datetime% (to signify "now") in the titleformat string.
class formatter : public titleformat_hook
{
public:
    bool process_field(titleformat_text_out* p_out, const char* p_name, t_size p_name_length, bool& p_found_flag) override;
    bool process_function(titleformat_text_out* p_out, const char* p_name, t_size p_name_length,
                          titleformat_hook_function_params* p_params, bool& p_found_flag) override;

    static titleformat_hook* get() { return &m_instance; }

private:
    static formatter m_instance;
};
