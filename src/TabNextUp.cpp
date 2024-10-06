#include "TabNextUp.h"

#include "nowplaying.h"
#include "formatter.h"

#include <helpers/atl-misc.h>

void TabNextUp::Reset()
{
    path_ = next::default_file_path;
    uSetDlgItemText(*this, IDC_PATH, path_);

    format_ = next::default_playback_format;
    uSetDlgItemText(*this, IDC_FORMAT, format_);

    same_as_now_ = next::default_use_now;
    CCheckBox same_as_now{GetDlgItem(IDC_USE_NOWPLAYING)};
    same_as_now.SetCheck(same_as_now_ ? BST_CHECKED : BST_UNCHECKED);

    file_encoding_ = next::default_file_encoding;
    CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
    file_encoding.SetCurSel(file_encoding_);

    with_bom_ = next::default_with_bom;
    CCheckBox with_bom{GetDlgItem(IDC_WITH_BOM)};
    with_bom.SetCheck(with_bom_ ? BST_CHECKED : BST_UNCHECKED);

    file_append_ = next::default_file_append;
    CCheckBox file_append{GetDlgItem(IDC_FILE_APPEND)};
    file_append.SetCheck(file_append_ ? BST_CHECKED : BST_UNCHECKED);

    max_lines_ = next::default_max_lines;
    CEdit max_lines{GetDlgItem(IDC_MAX_LINES)};
    max_lines.SetWindowText(L"");

    exit_message_ = next::default_exit_message;
    uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);

    use_exit_now_ = next::default_use_exit_now;
    CCheckBox exit_now{GetDlgItem(IDC_USE_EXIT_NOW)};
    exit_now.SetCheck(use_exit_now_ ? BST_CHECKED : BST_UNCHECKED);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

BOOL TabNextUp::OnInitDialog(CWindow, LPARAM)
{
    EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);

    // Enable dark mode.
    // One call does it all, applies all relevant hacks automatically.
    dark_mode_.AddDialogWithControls(*this);

    uSetDlgItemText(*this, IDC_PATH, path_);
    uSetDlgItemText(*this, IDC_FORMAT, format_);
    CEdit format{GetDlgItem(IDC_FORMAT)};
    format.SetReadOnly(same_as_now_);

    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    if (!font_.IsNull())
    {
        format.SetFont(font_);
    }

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

    CCheckBox file_append{GetDlgItem(IDC_FILE_APPEND)};
    file_append.SetCheck(file_append_ ? BST_CHECKED : BST_UNCHECKED);

    CEdit max_lines{GetDlgItem(IDC_MAX_LINES)};
    if (max_lines_ > 0)
    {
        max_lines.SetWindowText(std::to_wstring(max_lines_).c_str());
    }

    if (!file_append_)
    {
        max_lines.EnableWindow(FALSE);
    }

    uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);
    CEdit exit_message{GetDlgItem(IDC_EXIT_MESSAGE)};
    exit_message.SetReadOnly(use_exit_now_);

    CCheckBox exit_now{GetDlgItem(IDC_USE_EXIT_NOW)};
    exit_now.SetCheck(use_exit_now_ ? BST_CHECKED : BST_UNCHECKED);

    play_callback_manager::get()->register_callback(this, NowPlaying::playback_flags, true);
    g_queue.get_static_instance().register_callback(this);

    return TRUE;
}

void TabNextUp::OnShowWindow(BOOL, int)
{
    if (same_as_now_)
    {
        format_ = tab_now_.Format();
        uSetDlgItemText(*this, IDC_FORMAT, format_);
        update_preview();
    }
    if (use_exit_now_)
    {
        exit_message_ = tab_now_.ExitMessage();
        uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);
    }
}

void TabNextUp::OnDestroyDialog()
{
    play_callback_manager::get()->unregister_callback(this);
    g_queue.get_static_instance().unregister_callback(this);
}

void TabNextUp::OnPathChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_PATH, path_);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNextUp::OnFormatChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_FORMAT, format_);

    // Save the text to the config
    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNextUp::OnBrowse(UINT, int, CWindow)
{
    pfc::string8 new_path;
    if (uGetOpenFileName(*this, "All files|*.*", 0, nullptr, "Save file...", nullptr, new_path, TRUE))
    {
        path_ = new_path;
        uSetDlgItemText(*this, IDC_PATH, path_);
    }
}

void TabNextUp::OnSameAsNow(UINT, int, CWindow)
{
    same_as_now_ = IsDlgButtonChecked(IDC_USE_NOWPLAYING) == BST_CHECKED;

    if (same_as_now_)
    {
        original_format_ = format_;
        format_ = tab_now_.Format();
        uSetDlgItemText(*this, IDC_FORMAT, format_);
    }
    else
    {
        format_ = original_format_;
        uSetDlgItemText(*this, IDC_FORMAT, format_);
    }

    CEdit format{GetDlgItem(IDC_FORMAT)};
    format.SetReadOnly(same_as_now_);

    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNextUp::OnFileEncodingChange(UINT, int, CWindow)
{
    CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
    file_encoding_ = file_encoding.GetCurSel();
    with_bom_ = IsDlgButtonChecked(IDC_WITH_BOM) == BST_CHECKED;

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNextUp::OnFileAppendChange(UINT, int, CWindow)
{
    file_append_ = IsDlgButtonChecked(IDC_FILE_APPEND) == BST_CHECKED;
    CEdit max_lines{GetDlgItem(IDC_MAX_LINES)};
    max_lines.EnableWindow(file_append_ ? TRUE : FALSE);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNextUp::OnMaxLinesChange(UINT, int, CWindow)
{
    pfc::string8 lines;
    uGetDlgItemText(*this, IDC_MAX_LINES, lines);
    try
    {
        max_lines_ = std::stoul(lines.get_ptr());
    }
    catch (const std::invalid_argument&)
    {
        max_lines_ = 0;
    }

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNextUp::OnExitMessageChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNextUp::OnUseExitNow(UINT, int, CWindow)
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

void TabNextUp::update_preview(metadb_handle_ptr p_track /* = nullptr */, bool stopped /* = false */,
                               bool exhausted /* = false */) const
{
    pfc::string8 preview;
    const metadb_handle_ptr next = get_next_handle(p_track, stopped, exhausted);
    if (next.is_valid())
    {
        next->format_title(formatter::get(), preview, script_, nullptr);
    }
    uSetDlgItemText(*this, IDC_PREVIEW, preview);
}
