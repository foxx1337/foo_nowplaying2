#pragma once

#include <SDK/foobar2000.h>
#include <string>

#include "queue.h"

class NowPlaying : public play_callback_static, private QueueCallback
{
public:
    void queue_register();
    void queue_unregister();
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
    void on_playback_stop(play_control::t_stop_reason p_reason) override
    {
        update(nullptr, p_reason == playback_control::stop_reason_user,
               p_reason == playback_control::stop_reason_eof, p_reason == playback_control::stop_reason_starting_another);
    }
    void on_playback_seek(double p_time) override {}
    void on_playback_pause(bool p_state) override { update(); }
    void on_playback_time(double p_time) override {}
    void on_playback_edited(metadb_handle_ptr p_track) override {}
    void on_playback_dynamic_info(const file_info& p_info) override {}
    void on_playback_dynamic_info_track(const file_info& p_info) override {}
    void on_volume_change(float p_new_val) override {}

    void on_queue() override { update(); }

    void update(metadb_handle_ptr track = nullptr, bool stopped = false, bool exhausted = false, bool another = false);

    void write_file(const pfc::string8& payload, const std::wstring& file_name, t_uint id_encoding, bool with_bom, bool with_append, t_uint max_lines);

    pfc::string8 playback_string_;

    std::wstring file_now_;
    t_uint file_encoding_now_;
    bool with_bom_now_;
    bool file_append_now_;
    t_uint max_lines_now_;

    titleformat_object::ptr script_now_;

    pfc::string8 playback_string_next_;

    std::wstring file_next_;
    t_uint file_encoding_next_;
    bool with_bom_next_;
    bool file_append_next_;
    t_uint max_lines_next_;

    titleformat_object::ptr script_next_;

    metadb_handle_ptr current_track_;

    pfc::mutex file_lock_;
};

metadb_handle_ptr get_next_handle(metadb_handle_ptr track, bool stopped, bool exhausted);

extern service_factory_single_t<NowPlaying> g_nowplaying2;
