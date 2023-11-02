#pragma once

#include <SDK/foobar2000.h>
#include <string>

class NowPlaying : public play_callback_static
{
public:
    static constexpr unsigned playback_flags = flag_on_playback_new_track | flag_on_playback_pause | flag_on_playback_stop;

    void refresh_settings(bool force_update = false);

    metadb_handle_ptr get_current_track() const { return current_track_; }

private:
    // Playback callback methods.
    unsigned get_flags() override { return playback_flags; }

    void on_playback_starting(play_control::t_track_command p_command, bool p_paused) override {}
    void on_playback_new_track(metadb_handle_ptr p_track) override
    {
        current_track_ = p_track;
        update(p_track);
    }
    void on_playback_stop(play_control::t_stop_reason p_reason) override { update(); }
    void on_playback_seek(double p_time) override {}
    void on_playback_pause(bool p_state) override { update(); }
    void on_playback_time(double p_time) override {}
    void on_playback_edited(metadb_handle_ptr p_track) override {}
    void on_playback_dynamic_info(const file_info& p_info) override {}
    void on_playback_dynamic_info_track(const file_info& p_info) override {}
    void on_volume_change(float p_new_val) override {}

    void update(metadb_handle_ptr track = nullptr);

    void write_file(const pfc::string8& payload, const std::wstring& file_name, t_uint id_encoding, bool with_bom, bool with_append, t_uint max_lines);

    pfc::string8 playback_string_;

    std::wstring file_now_;
    t_uint file_encoding_now_;
    bool with_bom_now_;
    bool file_append_now_;
    t_uint max_lines_now_;
    metadb_handle_ptr current_track_;

    titleformat_object::ptr script_now_;
    pfc::mutex file_lock_;
};

metadb_handle_ptr get_next_handle(metadb_handle_ptr track, bool stopped, bool exhausted);

extern service_factory_single_t<NowPlaying> g_nowplaying2;
