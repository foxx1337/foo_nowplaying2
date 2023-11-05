#include "TabNextUp.h"

#include <helpers/DarkMode.h>
#include <helpers/atl-misc.h>

#include "nowplaying.h"

void TabNextUp::Reset()
{
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

    play_callback_manager::get()->register_callback(this, NowPlaying::playback_flags, true);
    g_queue.get_static_instance().register_callback(this);

    return TRUE;
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
    CCheckBox same_as_now{GetDlgItem(IDC_USE_NOWPLAYING)};
    same_as_now_ = same_as_now.GetCheck() == BST_CHECKED;

    format_ = same_as_now_ ? now::playback_format.get() : next::playback_format.get();

    uSetDlgItemText(*this, IDC_FORMAT, format_);
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
    CCheckBox with_bom{GetDlgItem(IDC_WITH_BOM)};
    with_bom_ = with_bom.GetCheck() == BST_CHECKED;

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNextUp::OnFileAppendChange(UINT, int, CWindow)
{
    CCheckBox file_append{GetDlgItem(IDC_FILE_APPEND)};
    file_append_ = file_append.GetCheck() == BST_CHECKED;
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

void TabNextUp::update_preview(metadb_handle_ptr p_track /* = nullptr */, bool stopped /* = false */,
                               bool exhausted /* = false */) const
{
    pfc::string8 preview;
    const metadb_handle_ptr next = get_next_handle(p_track, stopped, exhausted);
    if (next.is_valid())
    {
        next->format_title(nullptr, preview, script_, nullptr);
    }
    uSetDlgItemText(*this, IDC_PREVIEW, preview);
}
