#pragma once

#include <helpers/foobar2000+atl.h>
#include <helpers/DarkMode.h>

#include "preferences.h"
#include "queue.h"
#include "nowplaying.h"
#include "resource.h"

#include "TabNowPlaying.h"
#include "TabNextUp.h"
#include "TabLog.h"

class TabRun : public CDialogImpl<TabRun>, private play_callback_impl_base, private QueueCallback
{
public:
    TabRun(preferences_page_callback::ptr callback, const CFont& mono_font,
            const TabNowPlaying& tab_now, const TabNextUp& tab_next, const TabLog& tab_log) :
        callback_(callback), font_(mono_font), tab_now_(tab_now), tab_next_(tab_next), tab_log_(tab_log),
        commandline_(run::commandline.get()),
        trigger_on_new_(run::trigger_on_new), trigger_on_pause_(run::trigger_on_pause),
        trigger_on_stop_(run::trigger_on_stop), trigger_on_time_(run::trigger_on_time),
        hide_(run::hide)
    {
    }

    static constexpr int IDD = IDD_RUN;

    BEGIN_MSG_MAP(TabRun)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SHOWWINDOW(OnShowWindow)
        MSG_WM_DESTROY(OnDestroyDialog)
        COMMAND_HANDLER_EX(IDC_COMMAND, EN_CHANGE, OnCommandlineChange)
        COMMAND_HANDLER_EX(IDC_ON_NEW, BN_CLICKED, OnTriggerChange)
        COMMAND_HANDLER_EX(IDC_ON_PAUSE, BN_CLICKED, OnTriggerChange)
        COMMAND_HANDLER_EX(IDC_ON_STOP, BN_CLICKED, OnTriggerChange)
        COMMAND_HANDLER_EX(IDC_ON_TIME, BN_CLICKED, OnTriggerChange)
        COMMAND_HANDLER_EX(IDC_HIDE, BN_CLICKED, OnHideChange)
    END_MSG_MAP()

    // Getters.
    const pfc::string8& Commandline() const { return commandline_; }
    bool TriggerOnNew() const { return trigger_on_new_; }
    bool TriggerOnPause() const { return trigger_on_pause_; }
    bool TriggerOnStop() const { return trigger_on_stop_; }
    bool TriggerOnTime() const { return trigger_on_time_; }

    bool Hide() const { return hide_; }

    // Resets everything to default and updates the view.
    void Reset();

private:
    const preferences_page_callback::ptr callback_;

    // Dark mode hooks object, must be a member of dialog class.
    fb2k::CDarkModeHooks dark_mode_;

    const CFont& font_;

    const TabNowPlaying& tab_now_;
    const TabNextUp& tab_next_;
    const TabLog& tab_log_;

    pfc::string8 commandline_;
    bool trigger_on_new_;
    bool trigger_on_pause_;
    bool trigger_on_stop_;
    bool trigger_on_time_;
    bool hide_;

    // WTL handlers.
    BOOL OnInitDialog(CWindow, LPARAM);
    void OnShowWindow(BOOL, int);
    void OnDestroyDialog();
    void OnCommandlineChange(UINT, int, CWindow);
    void OnTriggerChange(UINT, int, CWindow);
    void OnHideChange(UINT, int, CWindow);

    // Playback callback handlers.
    void on_playback_starting(play_control::t_track_command p_command, bool p_paused) override {}
    void on_playback_new_track(metadb_handle_ptr p_track) override
    {
        if (trigger_on_new_)
        {
            update_preview(NowPlaying::action::new_track, p_track);
        }
    }
    void on_playback_stop(play_control::t_stop_reason p_reason) override
    {
        if (trigger_on_stop_)
        {
            update_preview(NowPlaying::action::stop, nullptr, p_reason == playback_control::stop_reason_user,
                           p_reason == playback_control::stop_reason_eof);
        }
    }
    void on_playback_seek(double p_time) override {}
    void on_playback_pause(bool p_state) override
    {
        if (trigger_on_pause_)
        {
            update_preview(NowPlaying::action::pause);
        }
    }
    void on_playback_edited(metadb_handle_ptr p_track) override {}
    void on_playback_dynamic_info(const file_info& p_info) override {}
    void on_playback_dynamic_info_track(const file_info& p_info) override { update_preview(NowPlaying::action::info); }
    void on_playback_time(double p_time) override
    {
        if (trigger_on_time_)
        {
            update_preview(NowPlaying::action::time);
        }
    }
    void on_volume_change(float p_new_val) override {}

    void on_queue() override { update_preview(NowPlaying::action::queue); }

    void update_preview(NowPlaying::action action = NowPlaying::action::any, metadb_handle_ptr p_track = nullptr,
                        bool stopped = false,
                        bool exhausted = false) const;
};
