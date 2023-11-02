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

    update_preview();
}

BOOL TabNextUp::OnInitDialog(CWindow, LPARAM)
{
    EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);

    // Enable dark mode.
    // One call does it all, applies all relevant hacks automatically.
    dark_mode_.AddDialogWithControls(*this);

    uSetDlgItemText(*this, IDC_FORMAT, format_);
    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    CCheckBox same_as_now{GetDlgItem(IDC_USE_NOWPLAYING)};
    same_as_now.SetCheck(same_as_now_ ? BST_CHECKED : BST_UNCHECKED);

    play_callback_manager::get()->register_callback(this, NowPlaying::playback_flags, true);
    g_queue.get_static_instance().register_callback(this);

    return TRUE;
}

void TabNextUp::OnDestroyDialog()
{
    play_callback_manager::get()->unregister_callback(this);
    g_queue.get_static_instance().unregister_callback();
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

void TabNextUp::OnSameAsNow(UINT, int, CWindow)
{
    CCheckBox same_as_now{GetDlgItem(IDC_USE_NOWPLAYING)};
    same_as_now_ = same_as_now.GetCheck() == BST_CHECKED;

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
