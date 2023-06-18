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

std::vector<unsigned char> to_encoding(const pfc::string8& message, encoding encoding, bool with_bom)
{
    std::vector<unsigned char> result(with_bom ? encodings[static_cast<size_t>(encoding)].bom : std::vector<unsigned char>());
    switch (encoding)
    {
    case encoding::UTF8:
        result.insert(result.end(), message.get_ptr(), message.get_ptr() + message.get_length());
        break;
    case encoding::UTF16LE:
        const pfc::stringcvt::string_wide_from_utf8_t utf16_string(message);
        result.insert(result.end(), reinterpret_cast<const unsigned char*>(utf16_string.get_ptr()),
                      reinterpret_cast<const unsigned char*>(utf16_string.get_ptr() + utf16_string.length()));
        break;
    }
    return result;
}

void NowPlaying::update()
{
    if (playback_format.get_length() == 0 || file_path.get_length() == 0)
    {
        return;
    }

    playback_control::get()->playback_format_title(nullptr, playback_string_, script_, nullptr,
                                                   playback_control::display_level_all);

    write_file();
}

void NowPlaying::write_file()
{
    // Make sure no empty line is appended to the file on stop.
    if (file_append && playback_string_.is_empty())
    {
        return;
    }
    // Only do one write and let all others fail.
    if (file_lock_.tryEnter())
    {
        HANDLE file = CreateFile(file_.c_str(),
                        GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        nullptr,
                        file_append ? OPEN_ALWAYS : CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        nullptr);
        if (file != INVALID_HANDLE_VALUE)
        {
            if (file_append)
            {
                SetFilePointer(file, 0, nullptr, FILE_END);
            }
            const std::vector<unsigned char> message =
                to_encoding(playback_string_ + (file_append ? "\r\n" : ""),
                            static_cast<encoding>(file_encoding.get_value()), with_bom && !file_append);
            size_t pos = 0;
            while (pos < message.size())
            {
                DWORD written = 0;
                if (!WriteFile(file, message.data() + pos,
                               static_cast<DWORD>((message.size() - pos) * sizeof(unsigned char)), &written, nullptr) &&
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

class InitQuit : public initquit
{
public:
    void on_init() override { g_nowplaying2.get_static_instance().refresh_settings(); }
    void on_quit() override {}
};

service_factory_single_t<NowPlaying> g_nowplaying2;
static initquit_factory_t<InitQuit> g_initquit;
