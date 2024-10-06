#include "TabRun.h"

#include <helpers/atl-misc.h>
#include <string>

void TabRun::Reset()
{
    commandline_ = run::default_commandline;
    uSetDlgItemText(*this, IDC_COMMAND, commandline_);

    trigger_on_new_ = run::default_trigger_on_new;
    CCheckBox on_new{GetDlgItem(IDC_ON_NEW)};
    on_new.SetCheck(trigger_on_new_ ? BST_CHECKED : BST_UNCHECKED);

    trigger_on_pause_ = run::default_trigger_on_pause;
    CCheckBox on_pause{GetDlgItem(IDC_ON_PAUSE)};
    on_pause.SetCheck(trigger_on_pause_ ? BST_CHECKED : BST_UNCHECKED);

    trigger_on_stop_ = run::default_trigger_on_stop;
    CCheckBox on_stop{GetDlgItem(IDC_ON_STOP)};
    on_stop.SetCheck(trigger_on_stop_ ? BST_CHECKED : BST_UNCHECKED);

    trigger_on_time_ = run::default_trigger_on_time;
    CCheckBox on_time{GetDlgItem(IDC_ON_TIME)};
    on_time.SetCheck(trigger_on_time_ ? BST_CHECKED : BST_UNCHECKED);

    hide_ = run::default_hide;
    CCheckBox hide{GetDlgItem(IDC_HIDE)};
    hide.SetCheck(hide_ ? BST_CHECKED : BST_UNCHECKED);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

BOOL TabRun::OnInitDialog(CWindow, LPARAM)
{
    EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);

    // Enable dark mode.
    // One call does it all, applies all relevant hacks automatically.
    dark_mode_.AddDialogWithControls(*this);

    uSetDlgItemText(*this, IDC_COMMAND, commandline_);

    CCheckBox on_new{GetDlgItem(IDC_ON_NEW)};
    on_new.SetCheck(trigger_on_new_ ? BST_CHECKED : BST_UNCHECKED);

    CCheckBox on_pause{GetDlgItem(IDC_ON_PAUSE)};
    on_pause.SetCheck(trigger_on_pause_ ? BST_CHECKED : BST_UNCHECKED);

    CCheckBox on_stop{GetDlgItem(IDC_ON_STOP)};
    on_stop.SetCheck(trigger_on_stop_ ? BST_CHECKED : BST_UNCHECKED);

    CCheckBox on_time{GetDlgItem(IDC_ON_TIME)};
    on_time.SetCheck(trigger_on_time_ ? BST_CHECKED : BST_UNCHECKED);

    CCheckBox hide{GetDlgItem(IDC_HIDE)};
    hide.SetCheck(hide_ ? BST_CHECKED : BST_UNCHECKED);

    CEdit command{GetDlgItem(IDC_COMMAND)};
    CEdit preview{GetDlgItem(IDC_PREVIEW)};
    if (!font_.IsNull())
    {
        command.SetFont(font_);
        preview.SetFont(font_);
    }

    play_callback_manager::get()->register_callback(this, NowPlaying::playback_flags, true);
    g_queue.get_static_instance().register_callback(this);

    return TRUE;
}

void TabRun::OnShowWindow(BOOL, int)
{
    update_preview();
}

void TabRun::OnDestroyDialog()
{
    play_callback_manager::get()->unregister_callback(this);
    g_queue.get_static_instance().unregister_callback(this);
}

void TabRun::OnCommandlineChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_COMMAND, commandline_);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabRun::OnTriggerChange(UINT, int, CWindow)
{
    trigger_on_new_ = IsDlgButtonChecked(IDC_ON_NEW) == BST_CHECKED;
    trigger_on_pause_ = IsDlgButtonChecked(IDC_ON_PAUSE) == BST_CHECKED;
    trigger_on_stop_ = IsDlgButtonChecked(IDC_ON_STOP) == BST_CHECKED;
    trigger_on_time_ = IsDlgButtonChecked(IDC_ON_TIME) == BST_CHECKED;

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabRun::OnHideChange(UINT, int, CWindow)
{
    hide_ = IsDlgButtonChecked(IDC_HIDE) == BST_CHECKED;

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabRun::update_preview(NowPlaying::action action /* = NowPlaying::action::any */,
                            metadb_handle_ptr p_track /* = nullptr */,
                            bool stopped /* = false */, bool exhausted /* = false */) const
{
    const pfc::stringcvt::string_wide_from_utf8_t utf16_string(commandline_);
    std::wstring commandline{utf16_string};
    const metadb_handle_ptr next = get_next_handle(p_track, stopped, exhausted);
    CEdit edit_preview{GetDlgItem(IDC_PREVIEW)};
    edit_preview.SetWindowText(
        NowPlaying::expand_commandline(NowPlaying::action::any, next, commandline, tab_now_.Script(),
                                                    tab_next_.Script(), tab_log_.Script())
        .c_str());
}
