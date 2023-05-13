#include <helpers/foobar2000+atl.h>
#include <helpers/atl-misc.h>
#include <helpers/DarkMode.h>

#include "resource.h"

// {CA730BF5-9B42-403D-BB57-27C430D9086E}
static constexpr GUID guid_playback_format
{ 0xca730bf5, 0x9b42, 0x403d, { 0xbb, 0x57, 0x27, 0xc4, 0x30, 0xd9, 0x8, 0x6e } };

static constexpr char default_playback_format[]
{ "%artist% - %title%" };

cfg_string playback_format(guid_playback_format, default_playback_format);

class Preferences : public CDialogImpl<Preferences>, public preferences_page_instance
{
public:
    // Constructor - invoked by preferences_page_impl helpers - don't do Create() in here, preferences_page_impl does this for us
    Preferences(preferences_page_callback::ptr callback) : callback(callback) {}

    // Note that we don't bother doing anything regarding destruction of our class.
    // The host ensures that our dialog is destroyed first, then the last reference to our preferences_page_instance object is released, causing our object to be deleted.


    // dialog resource ID
    enum
    {
        IDD = IDD_PREFERENCES
    };


    t_uint32 get_state() override
    {
        // return preferences_state::resettable;
        return preferences_state::resettable | preferences_state::dark_mode_supported;
    }

    void apply() override
    {
        // apply changes
    }

    void reset() override
    {
        // reset to default
    }


    // WTL message map
    BEGIN_MSG_MAP_EX(Preferences)
        MSG_WM_INITDIALOG(OnInitDialog)
    END_MSG_MAP()

private:
    const preferences_page_callback::ptr callback;
    // Dark mode hooks object, must be a member of dialog class.
    fb2k::CDarkModeHooks dark_mode;


    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
    {
        // Enable dark mode
        // One call does it all, applies all relevant hacks automatically
        dark_mode.AddDialogWithControls(*this);

        // Don't set keyboard focus to the dialog
        return FALSE;
    }
};

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
