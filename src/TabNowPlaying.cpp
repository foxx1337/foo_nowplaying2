#include "TabNowPlaying.h"

#include <helpers/atl-misc.h>

#include "nowplaying.h"

void TabNowPlaying::Reset()
{
    path_ = now::default_file_path;
    uSetDlgItemText(*this, IDC_PATH, path_);

    format_ = now::default_playback_format;
    uSetDlgItemText(*this, IDC_FORMAT, format_);

    file_encoding_ = now::default_file_encoding;
    CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
    file_encoding.SetCurSel(file_encoding_);

    with_bom_ = now::default_with_bom;
    CCheckBox with_bom{GetDlgItem(IDC_WITH_BOM)};
    with_bom.SetCheck(with_bom_ ? BST_CHECKED : BST_UNCHECKED);

    file_append_ = now::default_file_append;
    CCheckBox file_append{GetDlgItem(IDC_FILE_APPEND)};
    file_append.SetCheck(file_append_ ? BST_CHECKED : BST_UNCHECKED);

    max_lines_ = now::default_max_lines;
    CEdit max_lines{GetDlgItem(IDC_MAX_LINES)};
    max_lines.SetWindowText(L"");

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

BOOL TabNowPlaying::OnInitDialog(CWindow, LPARAM)
{
    EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);

    // Enable dark mode.
    // One call does it all, applies all relevant hacks automatically.
    dark_mode_.AddDialogWithControls(*this);

    uSetDlgItemText(*this, IDC_PATH, path_);
    uSetDlgItemText(*this, IDC_FORMAT, format_);
    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    CEdit format{GetDlgItem(IDC_FORMAT)};
    if (!font_.IsNull())
    {
        format.SetFont(font_);
    }

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

    play_callback_manager::get()->register_callback(this, NowPlaying::playback_flags, true);

    return TRUE;
}

void TabNowPlaying::OnDestroyDialog() { play_callback_manager::get()->unregister_callback(this); }

void TabNowPlaying::OnPathChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_PATH, path_);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNowPlaying::OnFormatChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_FORMAT, format_);

    // Save the text to the config
    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNowPlaying::OnBrowse(UINT, int, CWindow)
{
    pfc::string8 new_path;
    if (uGetOpenFileName(*this, "All files|*.*", 0, nullptr, "Save file...", nullptr, new_path, TRUE))
    {
        path_ = new_path;
        uSetDlgItemText(*this, IDC_PATH, path_);
    }
}

void TabNowPlaying::OnFileAppendChange(UINT, int, CWindow)
{
    CCheckBox file_append{GetDlgItem(IDC_FILE_APPEND)};
    file_append_ = file_append.GetCheck() == BST_CHECKED;
    CEdit max_lines{GetDlgItem(IDC_MAX_LINES)};
    max_lines.EnableWindow(file_append_ ? TRUE : FALSE);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNowPlaying::OnMaxLinesChange(UINT, int, CWindow)
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

void TabNowPlaying::OnFileEncodingChange(UINT, int, CWindow)
{
    CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
    file_encoding_ = file_encoding.GetCurSel();
    CCheckBox with_bom{GetDlgItem(IDC_WITH_BOM)};
    with_bom_ = with_bom.GetCheck() == BST_CHECKED;

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNowPlaying::update_preview() const
{
    pfc::string8 preview;
    playback_control::get()->playback_format_title(nullptr, preview, script_, nullptr,
                                                   playback_control::display_level_all);
    uSetDlgItemText(*this, IDC_PREVIEW, preview);
}
