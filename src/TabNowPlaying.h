#pragma once

#include <helpers/foobar2000+atl.h>
#include <helpers/DarkMode.h>

#include "preferences.h"
#include "resource.h"

class TabNowPlaying : public CDialogImpl<TabNowPlaying>, private play_callback_impl_base
{
public:
    TabNowPlaying(preferences_page_callback::ptr callback, const CFont& mono_font) :
        callback_(callback), font_(mono_font), path_(now::file_path.get()), format_(now::playback_format.get()),
        file_encoding_(static_cast<t_uint>(now::file_encoding)), with_bom_(now::with_bom),
        file_append_(now::file_append), max_lines_(static_cast<t_uint>(now::max_lines)),
        trigger_on_new_(now::trigger_on_new), trigger_on_pause_(now::trigger_on_pause),
        trigger_on_stop_(now::trigger_on_stop), trigger_on_time_(now::trigger_on_time)
    {
    }

    enum
    {
        IDD = IDD_NOWPLAYING
    };

    BEGIN_MSG_MAP(TabNowPlaying)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroyDialog)
        COMMAND_HANDLER_EX(IDC_PATH, EN_CHANGE, OnPathChange)
        COMMAND_HANDLER_EX(IDC_FORMAT, EN_CHANGE, OnFormatChange)
        COMMAND_HANDLER_EX(IDC_BUTTON_BROWSE, BN_CLICKED, OnBrowse)
        COMMAND_HANDLER_EX(IDC_FILE_ENCODING, CBN_SELCHANGE, OnFileEncodingChange)
        COMMAND_HANDLER_EX(IDC_WITH_BOM, BN_CLICKED, OnFileEncodingChange)
        COMMAND_HANDLER_EX(IDC_FILE_APPEND, BN_CLICKED, OnFileAppendChange)
        COMMAND_HANDLER_EX(IDC_MAX_LINES, EN_CHANGE, OnMaxLinesChange)
        COMMAND_HANDLER_EX(IDC_ON_NEW, BN_CLICKED, OnTriggerChange)
        COMMAND_HANDLER_EX(IDC_ON_PAUSE, BN_CLICKED, OnTriggerChange)
        COMMAND_HANDLER_EX(IDC_ON_STOP, BN_CLICKED, OnTriggerChange)
        COMMAND_HANDLER_EX(IDC_ON_TIME, BN_CLICKED, OnTriggerChange)
    END_MSG_MAP()

    // Getters.
    const pfc::string8& Path() const { return path_; }
    const pfc::string8& Format() const { return format_; }
    t_uint FileEncoding() const { return file_encoding_; }
    bool WithBom() const { return with_bom_; }
    bool FileAppend() const { return file_append_; }
    t_uint MaxLines() const { return max_lines_; }
    bool TriggerOnNew() const { return trigger_on_new_; }
    bool TriggerOnPause() const { return trigger_on_pause_; }
    bool TriggerOnStop() const { return trigger_on_stop_; }
    bool TriggerOnTime() const { return trigger_on_time_; }

    // Resets everything to default and updates the view.
    void Reset();

private:
    const preferences_page_callback::ptr callback_;

    // Dark mode hooks object, must be a member of dialog class.
    fb2k::CDarkModeHooks dark_mode_;

    const CFont& font_;

    pfc::string8 path_;
    pfc::string8 format_;
    t_uint file_encoding_;
    bool with_bom_;
    bool file_append_;
    t_uint max_lines_;
    bool trigger_on_new_;
    bool trigger_on_pause_;
    bool trigger_on_stop_;
    bool trigger_on_time_;

    titleformat_object::ptr script_;

    // WTL handlers.
    BOOL OnInitDialog(CWindow, LPARAM);
    void OnDestroyDialog();
    void OnPathChange(UINT, int, CWindow);
    void OnFormatChange(UINT, int, CWindow);
    void OnBrowse(UINT, int, CWindow);
    void OnFileEncodingChange(UINT, int, CWindow);
    void OnFileAppendChange(UINT, int, CWindow);
    void OnMaxLinesChange(UINT, int, CWindow);
    void OnTriggerChange(UINT, int, CWindow);

    // Playback callback methods.
    void on_playback_starting(play_control::t_track_command p_command, bool p_paused) override {}
    void on_playback_new_track(metadb_handle_ptr p_track) override
    {
        if (trigger_on_new_)
        {
            update_preview();
        }
    }
    void on_playback_stop(play_control::t_stop_reason p_reason) override
    {
        if (trigger_on_stop_)
        {
            update_preview();
        }
    }
    void on_playback_seek(double p_time) override {}
    void on_playback_pause(bool p_state) override
    {
        if (trigger_on_pause_)
        {
            update_preview();
        }
    }
    void on_playback_edited(metadb_handle_ptr p_track) override {}
    void on_playback_dynamic_info(const file_info& p_info) override {}
    void on_playback_dynamic_info_track(const file_info& p_info) override { update_preview(); }
    void on_playback_time(double p_time) override
    {
        if (trigger_on_time_)
        {
            update_preview();
        }
    }
    void on_volume_change(float p_new_val) override {}

    void update_preview() const;
};
