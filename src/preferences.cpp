#include <helpers/foobar2000+atl.h>
#include <helpers/atl-misc.h>
#include <helpers/DarkMode.h>

#include <atlctrlx.h>

#include <Uxtheme.h>

#include "preferences.h"
#include "NowPlaying.h"

#include "TabNowPlaying.h"
#include "TabNextUp.h"


const std::vector<encoding_info> encodings{encoding_info{encoding::UTF8, L"UTF-8", {0xef, 0xbb, 0xbf}},
                                           encoding_info{encoding::UTF16LE, L"UTF-16 LE", {0xff, 0xfe}}};


namespace now
{
    cfg_string playback_format(guid_playback_format, default_playback_format);
    cfg_string file_path(guid_file_path, default_file_path);
    cfg_uint file_encoding(guid_file_encoding, default_file_encoding);
    cfg_bool with_bom(guid_with_bom, default_with_bom);
    cfg_bool file_append(guid_file_append, default_file_append);
    cfg_uint max_lines(guid_max_lines, default_max_lines);
} // namespace now

namespace next
{
    cfg_string playback_format(guid_playback_format, default_playback_format);
    cfg_bool use_now(guid_use_now, default_use_now);
} // namespace next


class Preferences : public CDialogImpl<Preferences>, public preferences_page_instance
{
public:
    // Constructor - invoked by preferences_page_impl helpers - don't do Create() in here, preferences_page_impl does this for us.
    Preferences(preferences_page_callback::ptr callback) :
        tab_now_(callback), tab_next_(callback)
    {
    }

    // Note that we don't bother doing anything regarding destruction of our class.
    // The host ensures that our dialog is destroyed first, then the last reference to our preferences_page_instance object is released, causing our object to be deleted.


    // Dialog resource ID.
    enum
    {
        IDD = IDD_PREFERENCES
    };


    t_uint32 get_state() override
    {
        return preferences_state::resettable | preferences_state::dark_mode_supported | changed_flag();
    }

    void apply() override
    {
        // Apply changes.
        now::file_path = tab_now_.Path();
        now::playback_format = tab_now_.Format();
        now::file_encoding = tab_now_.FileEncoding();
        now::with_bom = tab_now_.WithBom();
        now::file_append = tab_now_.FileAppned();
        now::max_lines = tab_now_.MaxLines();

        next::use_now = tab_next_.UseSameAsNow();
        if (!next::use_now)
        {
            next::playback_format = tab_next_.Format();
        }

        g_nowplaying2.get_static_instance().refresh_settings(true);
    }

    void reset() override
    {
        // Reset to defaults.
        tab_now_.Reset();
        tab_next_.Reset();
    }

    // WTL message map
    BEGIN_MSG_MAP_EX(Preferences)
        MSG_WM_INITDIALOG(OnInitDialog)
        NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnTabChanged)
    END_MSG_MAP()

private:
    TabNowPlaying tab_now_;
    TabNextUp tab_next_;

    // Dark mode hooks object, must be a member of dialog class.
    fb2k::CDarkModeHooks dark_mode_;

    CTabCtrl tabs_;

    // WTL handlers.
    BOOL OnInitDialog(CWindow, LPARAM lParam);
    LRESULT OnTabChanged(int, LPNMHDR, BOOL&);

    t_uint32 changed_flag() const
    {
        return
            tab_now_.Format() != now::playback_format || tab_now_.Path() != now::file_path || tab_now_.FileEncoding() != now::file_encoding ||
            tab_now_.WithBom() != now::with_bom || tab_now_.FileAppned() != now::file_append || tab_now_.MaxLines() != now::max_lines ||
            tab_next_.Format() != next::playback_format || tab_next_.UseSameAsNow() != next::use_now
        ? preferences_state::changed
        : 0;
    }
};

BOOL Preferences::OnInitDialog(CWindow, LPARAM lParam)
{
    // Enable dark mode.
    // One call does it all, applies all relevant hacks automatically.
    dark_mode_.AddDialogWithControls(*this);

    tabs_.Attach(GetDlgItem(IDC_TAB));
    tabs_.AddItem(L"Now Playing");
    tabs_.AddItem(L"Next Up");
    tabs_.SetCurSel(0);

    tab_now_.Create(*this, lParam);
    tab_now_.ShowWindow(SW_SHOW);

    tab_next_.Create(*this, lParam);
    tab_next_.ShowWindow(SW_HIDE);

    CTabView view;
    view.m_tab.Attach(tabs_);
    const int tabHeight = view.CalcTabHeight();
    CRect rc;
    tabs_.GetWindowRect(&rc);
    ScreenToClient(&rc);
    rc.top += tabHeight;
    rc.DeflateRect(5, 0);
    rc.bottom -= 5;
    tab_now_.MoveWindow(&rc);
    tab_next_.MoveWindow(&rc);

    // Don't set keyboard focus to the dialog.
    return FALSE;
}

LRESULT Preferences::OnTabChanged(int, LPNMHDR, BOOL&)
{
    const int index = tabs_.GetCurSel();
    switch (index)
    {
    case 0:
        tab_now_.ShowWindow(SW_SHOW);
        tab_next_.ShowWindow(SW_HIDE);
        break;
    case 1:
        tab_now_.ShowWindow(SW_HIDE);
        tab_next_.ShowWindow(SW_SHOW);
        break;
    }
    return 0;   
}


// preferences_page_impl<> helper deals with instantiation of our dialog; inherits from preferences_page_v3.
class preferences_page_nowplaying2impl : public preferences_page_impl<Preferences>
{
public:
    const char *get_name() override { return "Now Playing 2"; }

    // {EB2F1D5B-B5A2-4D64-9CC3-C7CB82B82A7F}
    GUID get_guid() override
    {
        static constexpr GUID guid =
        { 0xeb2f1d5b, 0xb5a2, 0x4d64, { 0x9c, 0xc3, 0xc7, 0xcb, 0x82, 0xb8, 0x2a, 0x7f } };

        return guid;
    }

    GUID get_parent_guid() override { return guid_tools; }
};

static preferences_page_factory_t<preferences_page_nowplaying2impl> g_preferences_page_nowplaying2impl_factory;
