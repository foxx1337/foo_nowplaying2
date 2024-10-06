﻿#pragma once

#include <helpers/foobar2000+atl.h>
#include <helpers/DarkMode.h>

#include "preferences.h"
#include "queue.h"
#include "resource.h"

#include "TabNowPlaying.h"

class TabNextUp : public CDialogImpl<TabNextUp>, private play_callback_impl_base, private QueueCallback
{
public:
    TabNextUp(preferences_page_callback::ptr callback, const CFont& mono_font, const TabNowPlaying& tab_now) :
        callback_(callback), font_(mono_font), tab_now_(tab_now), path_(next::file_path.get()), same_as_now_(next::use_now),
        file_encoding_(static_cast<t_uint>(next::file_encoding)), with_bom_(next::with_bom),
        file_append_(next::file_append), max_lines_(static_cast<t_uint>(next::max_lines)),
        use_exit_now_(next::use_exit_now)
    {
        format_ = same_as_now_ ? now::playback_format.get() : next::playback_format.get();
        original_format_ = format_;
        exit_message_ = use_exit_now_ ? now::exit_message.get() : next::exit_message.get();
        original_exit_message_ = exit_message_;
    }

    static constexpr int IDD = IDD_NEXTUP;

    BEGIN_MSG_MAP(TabNextUp)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SHOWWINDOW(OnShowWindow)
        MSG_WM_DESTROY(OnDestroyDialog)
        COMMAND_HANDLER_EX(IDC_PATH, EN_CHANGE, OnPathChange)
        COMMAND_HANDLER_EX(IDC_FORMAT, EN_CHANGE, OnFormatChange)
        COMMAND_HANDLER_EX(IDC_BUTTON_BROWSE, BN_CLICKED, OnBrowse)
        COMMAND_HANDLER_EX(IDC_USE_NOWPLAYING, BN_CLICKED, OnSameAsNow)
        COMMAND_HANDLER_EX(IDC_FILE_ENCODING, CBN_SELCHANGE, OnFileEncodingChange)
        COMMAND_HANDLER_EX(IDC_WITH_BOM, BN_CLICKED, OnFileEncodingChange)
        COMMAND_HANDLER_EX(IDC_FILE_APPEND, BN_CLICKED, OnFileAppendChange)
        COMMAND_HANDLER_EX(IDC_MAX_LINES, EN_CHANGE, OnMaxLinesChange)
        COMMAND_HANDLER_EX(IDC_EXIT_MESSAGE, EN_CHANGE, OnExitMessageChange)
        COMMAND_HANDLER_EX(IDC_USE_EXIT_NOW, BN_CLICKED, OnUseExitNow)
    END_MSG_MAP()

    // Getters.
    const pfc::string8& Path() const { return path_; }
    const pfc::string8& Format() const { return format_; }
    bool UseSameAsNow() const { return same_as_now_; }
    t_uint FileEncoding() const { return file_encoding_; }
    bool WithBom() const { return with_bom_; }
    bool FileAppend() const { return file_append_; }
    t_uint MaxLines() const { return max_lines_; }
    const pfc::string8& ExitMessage() const { return exit_message_; }
    bool UseExitNow() const { return use_exit_now_; }
    titleformat_object::ptr Script() const { return script_; }

    // Resets everything to default and updates the view.
    void Reset();

private:
    const preferences_page_callback::ptr callback_;

    // Dark mode hooks object, must be a member of dialog class.
    fb2k::CDarkModeHooks dark_mode_;

    const CFont& font_;

    const TabNowPlaying& tab_now_;

    pfc::string8 path_;
    pfc::string8 format_;
    pfc::string8 original_format_;
    bool same_as_now_;
    t_uint file_encoding_;
    bool with_bom_;
    bool file_append_;
    t_uint max_lines_;
    pfc::string8 exit_message_;
    pfc::string8 original_exit_message_;
    bool use_exit_now_;

    titleformat_object::ptr script_;

    // WTL handlers.
    BOOL OnInitDialog(CWindow, LPARAM);
    void OnShowWindow(BOOL, int);
    void OnDestroyDialog();
    void OnPathChange(UINT, int, CWindow);
    void OnFormatChange(UINT, int, CWindow);
    void OnBrowse(UINT, int, CWindow);
    void OnSameAsNow(UINT, int, CWindow);
    void OnFileEncodingChange(UINT, int, CWindow);
    void OnFileAppendChange(UINT, int, CWindow);
    void OnMaxLinesChange(UINT, int, CWindow);
    void OnExitMessageChange(UINT, int, CWindow);
    void OnUseExitNow(UINT, int, CWindow);

    // Playback callback methods.
    void on_playback_starting(play_control::t_track_command p_command, bool p_paused) override {}
    void on_playback_new_track(metadb_handle_ptr p_track) override { update_preview(p_track); }
    void on_playback_stop(play_control::t_stop_reason p_reason) override
    {
        update_preview(nullptr, p_reason == playback_control::stop_reason_user,
                       p_reason == playback_control::stop_reason_eof);
    }
    void on_playback_seek(double p_time) override {}
    void on_playback_pause(bool p_state) override { update_preview(); }
    void on_playback_edited(metadb_handle_ptr p_track) override {}
    void on_playback_dynamic_info(const file_info& p_info) override {}
    void on_playback_dynamic_info_track(const file_info& p_info) override {}
    void on_playback_time(double p_time) override {}
    void on_volume_change(float p_new_val) override {}

    void on_queue() override { update_preview(); }

    void update_preview(metadb_handle_ptr p_track = nullptr, bool stopped = false, bool exhausted = false) const;
};
