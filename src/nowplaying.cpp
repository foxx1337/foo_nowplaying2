#include "NowPlaying.h"
#include "preferences.h"

void NowPlaying::refresh_settings(bool force_update)
{
    titleformat_compiler::get()->compile_safe_ex(script_, playback_format.get(), nullptr);

    // This is how one gets a wchar_t string.
    const pfc::stringcvt::string_wide_from_utf8_t file(file_path.get());
    file_ = file;

    if (force_update)
    {
        update();
    }
}

void NowPlaying::update()
{
    if (playback_format.get_length() == 0 || file_path.get_length() == 0)
    {
        return;
    }
    if (script_.is_empty())
    {
        refresh_settings();
    }

    playback_control::get()->playback_format_title(nullptr, playback_string_, script_, nullptr,
                                                   playback_control::display_level_all);

    // Only do one write and let all others fail.
    if (file_lock_.tryEnter())
    {
        HANDLE file = CreateFile(file_.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE)
        {
            //const pfc::stringcvt::string_wide_from_utf8_t nowplaying(playback_string_);
            const size_t length = playback_string_.length();
            size_t pos = 0;
            while (pos < length)
            {
                DWORD written = 0;
                if (!WriteFile(file, playback_string_.get_ptr() + pos, static_cast<DWORD>((length - pos) * sizeof(char)), &written, nullptr) &&
                    written == 0)
                {
                    break;
                }
                pos += written;
            }
            
            CloseHandle(file);
        }
        else
        {
            const pfc::stringcvt::string_utf8_from_wide_t file_name(file_.c_str());
            console::printf("nowplaying2 failed to open \"%s\" for writing.", file_name.get_ptr());
        }
        file_lock_.leave();
    }
}


service_factory_single_t<NowPlaying> g_nowplaying2;
