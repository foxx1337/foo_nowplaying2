#include "NowPlaying.h"
#include "preferences.h"
#include "formatter.h"

#include <vector>
#include <optional>

void NowPlaying::queue_register()
{
    g_queue.get_static_instance().register_callback(this);
}

void NowPlaying::queue_unregister()
{
    g_queue.get_static_instance().unregister_callback(this);
    update_exit();
}

void NowPlaying::refresh_settings(bool force_update)
{
    // TODO lock the settings
    if (now::is_used())
    {
        titleformat_compiler::get()->compile_safe_ex(script_now_, now::playback_format, nullptr);

        // This is how one gets a wchar_t string.
        const pfc::stringcvt::string_wide_from_utf8_t file(now::file_path);
        file_now_ = file;
        file_encoding_now_ = now::file_encoding.get_value();
        with_bom_now_ = now::with_bom;
        file_append_now_ = now::file_append;
        max_lines_now_ = static_cast<t_uint>(now::max_lines);
    }
    if (next::is_used())
    {
        titleformat_compiler::get()->compile_safe_ex(
            script_next_, next::use_now ? now::playback_format : next::playback_format, nullptr);

        const pfc::stringcvt::string_wide_from_utf8_t file_next(next::file_path);
        file_next_ = file_next;
        file_encoding_next_ = next::file_encoding.get_value();
        with_bom_next_ = next::with_bom;
        file_append_next_ = next::file_append;
        max_lines_next_ = static_cast<t_uint>(next::max_lines);
    }
    if (play_log::is_used())
    {
        titleformat_compiler::get()->compile_safe_ex(
            script_log_, play_log::use_now ? now::playback_format : play_log::playback_format,nullptr);

        const pfc::stringcvt::string_wide_from_utf8_t file_log(play_log::file_path);
        file_log_ = file_log;
        file_encoding_log_ = play_log::file_encoding.get_value();
        with_bom_log_ = play_log::with_bom;
    }

    if (force_update)
    {
        update(action::any);
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

std::vector<unsigned char> read_file(HANDLE file)
{
    std::vector<unsigned char> data(GetFileSize(file, nullptr));
    SetFilePointer(file, 0, nullptr, FILE_BEGIN);
    DWORD offset = 0;
    DWORD read = 0;
    while (offset < data.size())
    {
        if (!ReadFile(file, data.data() + offset, static_cast<DWORD>(data.size() - offset), &read, nullptr))
        {
            return {};
        }
        offset += read;
    }
    return data;
}

bool starts_with(const std::vector<unsigned char>& vec, const std::vector<unsigned char>& start)
{
    if (vec.size() < start.size())
    {
        return false;
    }
    for (size_t i = 0; i < start.size(); i++)
    {
        if (vec[i] != start[i])
        {
            return false;
        }
    }
    return true;
}

size_t get_last_lines_offset(const std::vector<unsigned char>& vec, size_t start, size_t n, encoding enc)
{
    if (vec.empty())
    {
        return 0;
    }
    size_t pos = vec.size() - 1;
    while (pos > start)
    {
        if (vec[pos] == '\n')
        {
            n--;
            if (n == 0)
            {
                return pos + (enc == encoding::UTF16LE ? 2 : 1);
            }
        }
        pos--;
    }
    return start;
}

std::optional<encoding_info> get_encoding(const std::vector<unsigned char>& vec)
{
    for (const auto& e : encodings)
    {
        if (starts_with(vec, e.bom))
        {
            return e;
        }
    }
    return std::nullopt;
}

size_t write_all(HANDLE file, const std::vector<unsigned char>& data, size_t offset = 0)
{
    size_t pos = offset;
    while (pos < data.size())
    {
        DWORD written = 0;
        if (!WriteFile(file, data.data() + pos,
                       static_cast<DWORD>((data.size() - pos) * sizeof(unsigned char)), &written, nullptr) &&
            written == 0)
        {
            break;
        }
        pos += written;
    }
    return pos - offset;
}

bool truncate_file(HANDLE file, t_uint lines, encoding encoding, bool with_bom)
{
    const std::vector<unsigned char> data{read_file(file)};
    const std::optional<encoding_info> e{get_encoding(data)};
    const size_t start = e.has_value() ? e->bom.size() : 0;
    const size_t last_n =
        get_last_lines_offset(data, start, lines, encoding);

    // Empty the whole file, BOM or not.
    SetFilePointer(file, 0, nullptr, FILE_BEGIN);
    SetEndOfFile(file);

    size_t written = 0;
    if (with_bom)
    {
        written = write_all(file, encodings[static_cast<int>(encoding)].bom);
        if (written != encodings[static_cast<int>(encoding)].bom.size())
        {
            return false;
        }
    }
    if (data.size() > last_n)
    {
        written = write_all(file, data, last_n);
        if (written != data.size() - last_n)
        {
            return false;
        }
    }
    return true;
}

//! 
//! @returns handle to the first song in the queue, or the song after the current one in the playlist if no queue.
metadb_handle_ptr get_next_handle(metadb_handle_ptr track, bool stopped, bool exhausted)
{
    if (exhausted)
    {
        t_size playlist = playlist_manager::get()->get_playing_playlist();
        metadb_handle_ptr result;
        bool have_song = strcmp(playlist_manager::get()->playback_order_get_name(
                playlist_manager::get()->playback_order_get_active()),
                "Default") == 0
            && playlist_manager::get()->activeplaylist_get_item_handle(result, 0);
        return have_song ? result : nullptr;
    }
    if (stopped)
    {
        return g_nowplaying2.get_static_instance().get_current_track();
    }
    pfc::list_t<t_playback_queue_item> queue;
    playlist_manager::get()->queue_get_contents(queue);
    if (queue.size() >= 1 && queue.begin()->m_handle != track)
    {
        return queue.begin()->m_handle;
    }

    if (strcmp(playlist_manager::get()->playback_order_get_name(playlist_manager::get()->playback_order_get_active()),
               "Default") == 0)
    {
        t_size playlist;
        t_size index;
        bool have_song = true;
        if (!playlist_manager::get()->get_playing_item_location(&playlist, &index) || queue.size() == 1 && queue.begin()->m_handle == track)
        {
            playlist = playlist_manager::get()->get_playing_playlist();
            have_song = playlist_manager::get()->playlist_find_item(playlist, track, index);
        }
        if (have_song && index + 1 < playlist_manager::get()->playlist_get_item_count(playlist))
        {
            return playlist_manager::get()->playlist_get_item_handle(playlist, index + 1);
        }

    }
    return nullptr;
}

void NowPlaying::update(action action, metadb_handle_ptr track, bool stopped /* = false */, bool exhausted /* = false */, bool another /* = false */)
{
    if (now::is_used() && is_action_enabled(action))
    {
        playback_control::get()->playback_format_title(nullptr, playback_string_, script_now_, nullptr,
                                                       playback_control::display_level_all);

        // TODO lock the settings
        write_file(playback_string_, file_now_, file_encoding_now_, with_bom_now_, file_append_now_, max_lines_now_);
    }
    if (next::is_used() && !another)
    {
        const metadb_handle_ptr next_handle = get_next_handle(track, stopped, exhausted);
        if (next_handle.is_valid())
        {
            next_handle->format_title(formatter::get(), playback_string_next_, script_next_, nullptr);
            write_file(playback_string_next_, file_next_, file_encoding_next_, with_bom_next_, file_append_next_,
                       max_lines_next_);
        }
    }
    if (play_log::is_used() && (track.is_valid() || action == action::any))
    {
        pfc::string8 playback_string_log;
        bool do_log = false;
        if (track.is_valid())
        {
            track->format_title(formatter::get(), playback_string_log, script_log_, nullptr);
        }
        else
        {
            playback_control::get()->playback_format_title(formatter::get(), playback_string_log, script_log_, nullptr,
                                                           playback_control::display_level_all);
        }
        if (playback_string_log != playback_string_log_)
        {
            playback_string_log_ = playback_string_log;
            do_log = true;
        }
        if (!playback_string_log.empty() && do_log)
        {
            write_file(playback_string_log, file_log_, file_encoding_log_, with_bom_log_, true, 0);
        }
    }
}

void NowPlaying::write_file(const pfc::string8& payload, const std::wstring& file_name, t_uint id_encoding,
                            bool with_bom, bool with_append, t_uint max_lines)
{
    // Make sure no empty line is appended to the file on stop.
    if (with_append && payload.is_empty())
    {
        return;
    }
    // Only do one write and let all others fail.
    if (file_lock_.tryEnter())
    {
        const auto file_unlock = [](pfc::mutex* lock) { lock->leave(); };
        std::unique_ptr<pfc::mutex, decltype(file_unlock)> file_lock_scope(&file_lock_, file_unlock);

        HANDLE h_file = CreateFile(file_name.c_str(), GENERIC_WRITE | (max_lines > 0 ? GENERIC_READ : 0),
                                   FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                   with_append ? OPEN_ALWAYS : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h_file != INVALID_HANDLE_VALUE)
        {
            const std::unique_ptr<void, decltype(&CloseHandle)> file(h_file, &CloseHandle);
            if (max_lines > 0)
            {
                if (!truncate_file(file.get(), max_lines, static_cast<encoding>(id_encoding), with_bom))
                {
                    const pfc::stringcvt::string_utf8_from_wide_t file_name_utf(file_name.c_str());
                    console::printf("nowplaying2 couldn't truncate \"%s\" to %d lines.", file_name_utf.get_ptr(),
                                    max_lines);
                    return;
                }
            }
            bool need_bom = with_bom;
            if (with_append)
            {
                LARGE_INTEGER pos{0};
                pos.LowPart = SetFilePointer(file.get(), 0, &pos.HighPart, FILE_END);
                if (pos.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
                {
                    const pfc::stringcvt::string_utf8_from_wide_t file_name_utf(file_name.c_str());
                    console::printf("nowplaying2 failed to seek into \"%s\" for writing.", file_name_utf.get_ptr());
                    return;
                }
                need_bom = need_bom && pos.QuadPart == 0;
            }
            const std::vector<unsigned char> message = to_encoding(
                payload + (with_append ? "\n" : ""), static_cast<encoding>(id_encoding), need_bom);

            if (const size_t written = write_all(file.get(), message); written != message.size())
            {
                const pfc::stringcvt::string_utf8_from_wide_t file_name_utf(file_name.c_str());
                console::printf("nowplaying2 only managed to write %lld bytes into \"%s\".", written,
                                file_name_utf.get_ptr());
            }
        }
        else
        {
            const pfc::stringcvt::string_utf8_from_wide_t file_name_utf(file_name.c_str());
            console::printf("nowplaying2 failed to open \"%s\" for writing.", file_name_utf.get_ptr());
        }
    }
}

void NowPlaying::update_exit()
{
    if (now::is_used() && now::exit_message.length() != 0)
    {
        write_file(now::exit_message, file_now_, file_encoding_now_, with_bom_now_, file_append_now_, max_lines_now_);
    }
    if (next::is_used())
    {
        // Make sure empty is also written on quit.
        write_file(next::exit_message, file_next_, file_encoding_next_, with_bom_next_, file_append_next_, max_lines_next_);
    }
    if (play_log::is_used() && play_log::exit_message.length() != 0)
    {
        write_file(play_log::exit_message, file_log_, file_encoding_log_, with_bom_log_, true, 0);
    }
}

bool NowPlaying::is_action_enabled(action action)
{
    switch (action)
    {
    case action::new_track:
        return now::trigger_on_new;
    case action::pause:
        return now::trigger_on_pause;
    case action::stop:
        return now::trigger_on_stop;
    case action::time:
        return now::trigger_on_time;
    case action::any:
        return true;
    case action::queue:
        return false;
    default:
        return false;
    }
}


class InitQuit : public initquit
{
public:
    void on_init() override
    {
        g_nowplaying2.get_static_instance().refresh_settings();
        g_nowplaying2.get_static_instance().queue_register();
    }
    void on_quit() override
    {
        g_nowplaying2.get_static_instance().queue_unregister();
    }
};

service_factory_single_t<NowPlaying> g_nowplaying2;
static initquit_factory_t<InitQuit> g_initquit;
