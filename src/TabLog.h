#pragma once

#include <helpers/foobar2000+atl.h>
#include <helpers/DarkMode.h>

#include "preferences.h"
#include "resource.h"

class TabLog : public CDialogImpl<TabLog>, private play_callback_impl_base
{
public:
    TabLog(preferences_page_callback::ptr callback) :
        callback_(callback)
    {}

    enum
    {
        IDD = IDD_LOG
    };

    BEGIN_MSG_MAP(TabLog)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroyDialog)
    END_MSG_MAP()

    // Getters.

    // Resets everything to default and updates the view.
    void Reset();

private:
    const preferences_page_callback::ptr callback_;

    // Dark mode hooks object, must be a member of dialog class.
    fb2k::CDarkModeHooks dark_mode_;

    // WTL handlers.
    BOOL OnInitDialog(CWindow, LPARAM);
    void OnDestroyDialog();

    // Playback callback methods.
    void on_playback_starting(play_control::t_track_command p_command, bool p_paused) override {}
    void on_playback_new_track(metadb_handle_ptr p_track) override { update_preview(); }
    void on_playback_stop(play_control::t_stop_reason p_reason) override { update_preview(); }
    void on_playback_seek(double p_time) override {}
    void on_playback_pause(bool p_state) override { update_preview(); }
    void on_playback_edited(metadb_handle_ptr p_track) override {}
    void on_playback_dynamic_info(const file_info& p_info) override {}
    void on_playback_dynamic_info_track(const file_info& p_info) override {}
    void on_playback_time(double p_time) override {}
    void on_volume_change(float p_new_val) override {}

    void update_preview() const;
};
