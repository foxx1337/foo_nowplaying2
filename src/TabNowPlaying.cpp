#include "TabNowPlaying.h"

#include "nowplaying.h"

#include <helpers/atl-misc.h>

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

    trigger_on_new_ = now::default_trigger_on_new;
    CCheckBox on_new{GetDlgItem(IDC_ON_NEW)};
    on_new.SetCheck(trigger_on_new_ ? BST_CHECKED : BST_UNCHECKED);

    trigger_on_pause_ = now::default_trigger_on_pause;
    CCheckBox on_pause{GetDlgItem(IDC_ON_PAUSE)};
    on_pause.SetCheck(trigger_on_pause_ ? BST_CHECKED : BST_UNCHECKED);

    trigger_on_stop_ = now::default_trigger_on_stop;
    CCheckBox on_stop{GetDlgItem(IDC_ON_STOP)};
    on_stop.SetCheck(trigger_on_stop_ ? BST_CHECKED : BST_UNCHECKED);

    trigger_on_time_ = now::default_trigger_on_time;
    CCheckBox on_time{GetDlgItem(IDC_ON_TIME)};
    on_time.SetCheck(trigger_on_time_ ? BST_CHECKED : BST_UNCHECKED);

    exit_message_ = now::default_exit_message;
    uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);

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

    CCheckBox on_new{GetDlgItem(IDC_ON_NEW)};
    on_new.SetCheck(trigger_on_new_ ? BST_CHECKED : BST_UNCHECKED);

    CCheckBox on_pause{GetDlgItem(IDC_ON_PAUSE)};
    on_pause.SetCheck(trigger_on_pause_ ? BST_CHECKED : BST_UNCHECKED);

    CCheckBox on_stop{GetDlgItem(IDC_ON_STOP)};
    on_stop.SetCheck(trigger_on_stop_ ? BST_CHECKED : BST_UNCHECKED);

    CCheckBox on_time{GetDlgItem(IDC_ON_TIME)};
    on_time.SetCheck(trigger_on_time_ ? BST_CHECKED : BST_UNCHECKED);

    uSetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);

    tooltip_.Create(m_hWnd, rcDefault, nullptr);
    tooltip_.AddTool(CToolInfo(TTF_IDISHWND | TTF_SUBCLASS, m_hWnd,
                               reinterpret_cast<UINT_PTR>(GetDlgItem(IDC_FORMAT).m_hWnd), nullptr,
                               const_cast<LPWSTR>(L"Title formatting syntax, %datetime% inserts \"now\".")));
    tooltip_.AddTool(CToolInfo(TTF_IDISHWND | TTF_SUBCLASS, m_hWnd,
                               reinterpret_cast<UINT_PTR>(GetDlgItem(IDC_EXIT_MESSAGE).m_hWnd), nullptr,
                               const_cast<LPWSTR>(L"No title formatting syntax.")));

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
    file_append_ = IsDlgButtonChecked(IDC_FILE_APPEND) == BST_CHECKED;
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
    with_bom_ = IsDlgButtonChecked(IDC_WITH_BOM) == BST_CHECKED;

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNowPlaying::OnTriggerChange(UINT, int, CWindow)
{
    trigger_on_new_ = IsDlgButtonChecked(IDC_ON_NEW) == BST_CHECKED;
    trigger_on_pause_ = IsDlgButtonChecked(IDC_ON_PAUSE) == BST_CHECKED;
    trigger_on_stop_ = IsDlgButtonChecked(IDC_ON_STOP) == BST_CHECKED;
    trigger_on_time_ = IsDlgButtonChecked(IDC_ON_TIME) == BST_CHECKED;

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNowPlaying::OnExitMessageChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_EXIT_MESSAGE, exit_message_);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabNowPlaying::update_preview() const
{
    pfc::string8 preview;
    NowPlaying::format(preview, script_);
    uSetDlgItemText(*this, IDC_PREVIEW, preview);
}
