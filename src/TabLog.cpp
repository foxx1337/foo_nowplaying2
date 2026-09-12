#include "TabLog.h"

#include "nowplaying.h"
#include "formatter.h"

#include <helpers/atl-misc.h>

void TabLog::Reset()
{
    path_ = play_log::default_file_path;
    uSetDlgItemText(*this, IDC_PATH, path_);

    format_ = play_log::default_playback_format;
    same_as_now_ = play_log::default_use_now;
    original_format_ = format_;
    if (same_as_now_) format_ = tab_now_.Format();
    format_editor_.SetText(format_.get_ptr());
    format_editor_.SetReadOnly(same_as_now_);
    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);
    CCheckBox same_as_now{GetDlgItem(IDC_USE_NOWPLAYING)};
    same_as_now.SetCheck(same_as_now_ ? BST_CHECKED : BST_UNCHECKED);

    file_encoding_ = play_log::default_file_encoding;
    CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
    file_encoding.SetCurSel(file_encoding_);

    with_bom_ = play_log::default_with_bom;
    CCheckBox with_bom{GetDlgItem(IDC_WITH_BOM)};
    with_bom.SetCheck(with_bom_ ? BST_CHECKED : BST_UNCHECKED);

    exit_message_ = play_log::default_exit_message;
    uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);

    use_exit_now_ = play_log::default_use_exit_now;
    CCheckBox exit_now{GetDlgItem(IDC_USE_EXIT_NOW)};
    exit_now.SetCheck(use_exit_now_ ? BST_CHECKED : BST_UNCHECKED);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

BOOL TabLog::OnInitDialog(CWindow, LPARAM)
{
    EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);

    if (!format_editor_.Create(core_api::get_my_instance(), m_hWnd, IDC_FORMAT, format_.get_ptr(),
                               DarkMode::msgSetDarkMode(), fb2k::isDarkMode(), same_as_now_))
    {
        return FALSE;
    }

    // Enable dark mode.
    // One call does it all, applies all relevant hacks automatically.
    dark_mode_.AddDialogWithControls(*this);

    uSetDlgItemText(*this, IDC_PATH, path_);

    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    CCheckBox same_as_now{GetDlgItem(IDC_USE_NOWPLAYING)};
    same_as_now.SetCheck(same_as_now_ ? BST_CHECKED : BST_UNCHECKED);

    CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
    for (const auto& encoding : encodings)
    {
        file_encoding.AddString(encoding.name);
    }
    file_encoding.SetCurSel(file_encoding_);

    CCheckBox with_bom{GetDlgItem(IDC_WITH_BOM)};
    with_bom.SetCheck(with_bom_ ? BST_CHECKED : BST_UNCHECKED);

    uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);
    CEdit exit_message{GetDlgItem(IDC_EXIT_MESSAGE)};
    exit_message.SetReadOnly(use_exit_now_);

    CCheckBox exit_now{GetDlgItem(IDC_USE_EXIT_NOW)};
    exit_now.SetCheck(use_exit_now_ ? BST_CHECKED : BST_UNCHECKED);

    play_callback_manager::get()->register_callback(this, NowPlaying::playback_flags, true);
    initialized_ = true;

    return TRUE;
}

void TabLog::OnShowWindow(BOOL, int)
{
    if (!initialized_) return;
    if (same_as_now_)
    {
        format_ = tab_now_.Format();
        format_editor_.SetText(format_.get_ptr());
        titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);
        update_preview();
    }
    if (use_exit_now_)
    {
        exit_message_ = tab_now_.ExitMessage();
        uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);
    }
}

void TabLog::OnDestroyDialog()
{
    if (initialized_) play_callback_manager::get()->unregister_callback(this);
    initialized_ = false;
    dark_mode_.clear();
    format_editor_.Destroy();
}

void TabLog::OnPathChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_PATH, path_);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabLog::OnFormatChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    format_ = format_editor_.GetText().c_str();

    // Save the text to the config
    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabLog::OnBrowse(UINT, int, CWindow)
{
    pfc::string8 new_path;
    if (uGetOpenFileName(*this, "All files|*.*", 0, nullptr, "Save file...", nullptr, new_path, TRUE))
    {
        path_ = new_path;
        uSetDlgItemText(*this, IDC_PATH, path_);
    }
}

void TabLog::OnSameAsNow(UINT, int, CWindow)
{
    const auto current_format = Format();
    same_as_now_ = IsDlgButtonChecked(IDC_USE_NOWPLAYING) == BST_CHECKED;

    if (same_as_now_)
    {
        original_format_ = current_format;
        format_ = tab_now_.Format();
        format_editor_.SetText(format_.get_ptr());
    }
    else
    {
        format_ = original_format_;
        format_editor_.SetText(format_.get_ptr());
    }

    format_editor_.SetReadOnly(same_as_now_);

    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabLog::OnFileEncodingChange(UINT, int, CWindow)
{
    CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
    file_encoding_ = file_encoding.GetCurSel();
    with_bom_ = IsDlgButtonChecked(IDC_WITH_BOM) == BST_CHECKED;

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabLog::OnExitMessageChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabLog::OnUseExitNow(UINT, int, CWindow)
{
    use_exit_now_ = IsDlgButtonChecked(IDC_USE_EXIT_NOW) == BST_CHECKED;

    if (use_exit_now_)
    {
        original_exit_message_ = exit_message_;
        exit_message_ = tab_now_.ExitMessage();
        uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);
    }
    else
    {
        exit_message_ = original_exit_message_;
        uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);
    }

    CEdit exit_message{GetDlgItem(IDC_EXIT_MESSAGE)};
    exit_message.SetReadOnly(use_exit_now_);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabLog::update_preview() const
{
    pfc::string8 preview;
    playback_control::get()->playback_format_title(formatter::get(), preview, script_, nullptr,
                                                   playback_control::display_level_all);
    uSetDlgItemText(*this, IDC_PREVIEW, preview);
}
