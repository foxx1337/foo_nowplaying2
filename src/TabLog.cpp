#include "TabLog.h"

#include <helpers/DarkMode.h>
#include <helpers/atl-misc.h>
#include <chrono>
#include <format>

#include "nowplaying.h"

void TabLog::Reset()
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

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

BOOL TabLog::OnInitDialog(CWindow, LPARAM)
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

    play_callback_manager::get()->register_callback(this, NowPlaying::playback_flags, true);

    return TRUE;
}

void TabLog::OnDestroyDialog()
{
    play_callback_manager::get()->unregister_callback(this);
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
    uGetDlgItemText(*this, IDC_FORMAT, format_);

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
    CCheckBox same_as_now{GetDlgItem(IDC_USE_NOWPLAYING)};
    same_as_now_ = same_as_now.GetCheck() == BST_CHECKED;

    format_ = same_as_now_ ? now::playback_format.get() : play_log::playback_format.get();

    uSetDlgItemText(*this, IDC_FORMAT, format_);
    CEdit format{GetDlgItem(IDC_FORMAT)};
    format.SetReadOnly(same_as_now_);

    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabLog::OnFileEncodingChange(UINT, int, CWindow)
{
    CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
    file_encoding_ = file_encoding.GetCurSel();
    CCheckBox with_bom{GetDlgItem(IDC_WITH_BOM)};
    with_bom_ = with_bom.GetCheck() == BST_CHECKED;

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void TabLog::update_preview() const
{
    pfc::string8 preview;
    playback_control::get()->playback_format_title(nullptr, preview, script_, nullptr,
                                                   playback_control::display_level_all);

    if (!preview.empty())
    {
        auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
        preview = pfc::string8(std::format("{:%Y-%m-%d %X} ", time).c_str()) + preview;
    }
    uSetDlgItemText(*this, IDC_PREVIEW, preview);
}
