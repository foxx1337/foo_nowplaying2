#include <helpers/foobar2000+atl.h>
#include <helpers/atl-misc.h>
#include <helpers/DarkMode.h>

#include "nowplaying.h"
#include "resource.h"

// {CA730BF5-9B42-403D-BB57-27C430D9086E}
static constexpr GUID guid_playback_format
{ 0xca730bf5, 0x9b42, 0x403d, { 0xbb, 0x57, 0x27, 0xc4, 0x30, 0xd9, 0x8, 0x6e } };

static constexpr char default_playback_format[]
{ "%artist% - %title%" };

cfg_string playback_format(guid_playback_format, default_playback_format);

// {E8EB0BA5-877E-4F43-A99A-C23F145578F2}
static constexpr GUID guid_file_path
{ 0xe8eb0ba5, 0x877e, 0x4f43, { 0xa9, 0x9a, 0xc2, 0x3f, 0x14, 0x55, 0x78, 0xf2 } };

static constexpr char default_file_path[]
{""};

cfg_string file_path(guid_file_path, default_file_path);


class Preferences : public CDialogImpl<Preferences>, public preferences_page_instance, private play_callback_impl_base
{
public:
    // Constructor - invoked by preferences_page_impl helpers - don't do Create() in here, preferences_page_impl does this for us.
    Preferences(preferences_page_callback::ptr callback) :
        callback_(callback), format_(playback_format.get()), path_(file_path) {}

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
        file_path = path_;
        playback_format = format_;
        g_nowplaying2.get_static_instance().SetScript(format_);
    }

    void reset() override
    {
        // Reset to default.
        path_ = default_file_path;
        uSetDlgItemText(*this, IDC_PATH, path_);
        format_ = default_playback_format;
        uSetDlgItemText(*this, IDC_FORMAT, format_);
        update_preview();
    }


    // WTL message map
    BEGIN_MSG_MAP_EX(Preferences)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroyDialog)
        COMMAND_HANDLER_EX(IDC_PATH, EN_CHANGE, OnPathChange)
        COMMAND_HANDLER_EX(IDC_FORMAT, EN_CHANGE, OnFormatChange)
        COMMAND_HANDLER_EX(IDC_BUTTON_BROWSE, BN_CLICKED, OnBrowse)
    END_MSG_MAP()

private:
    const preferences_page_callback::ptr callback_;

    // Dark mode hooks object, must be a member of dialog class.
    fb2k::CDarkModeHooks dark_mode_;

    pfc::string8 path_;

    pfc::string8 format_;

    titleformat_object::ptr script_;

    // WTL handlers.
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDestroyDialog();
    void OnPathChange(UINT, int, CWindow);
    void OnFormatChange(UINT, int, CWindow);
    void OnBrowse(UINT, int, HWND);

    // Playback callback methods.
    void on_playback_starting(play_control::t_track_command p_command, bool p_paused) {}
    void on_playback_new_track(metadb_handle_ptr p_track) { update_preview(); }
    void on_playback_stop(play_control::t_stop_reason p_reason) { update_preview(); }
    void on_playback_seek(double p_time) {}
    void on_playback_pause(bool p_state) { update_preview(); }
    void on_playback_edited(metadb_handle_ptr p_track) {}
    void on_playback_dynamic_info(const file_info& p_info) {}
    void on_playback_dynamic_info_track(const file_info& p_info) {}
    void on_playback_time(double p_time) {}
    void on_volume_change(float p_new_val) {}

    void update_preview();

    t_uint32 changed_flag()
    {
        if (format_ != playback_format || path_ != file_path)
        {
            return preferences_state::changed;
        }
        else
        {
            return 0;
        }
    }
};

BOOL Preferences::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    // Enable dark mode.
    // One call does it all, applies all relevant hacks automatically.
    dark_mode_.AddDialogWithControls(*this);

    uSetDlgItemText(*this, IDC_PATH, path_);
    uSetDlgItemText(*this, IDC_FORMAT, format_);
    titleformat_compiler::get()->compile_safe_ex(script_, playback_format.c_str(), nullptr);

    play_callback_manager::get()->register_callback(
        this,
        flag_on_playback_new_track | flag_on_playback_pause |flag_on_playback_stop,
        true
        );

    // Don't set keyboard focus to the dialog.
    return FALSE;
}

void Preferences::OnDestroyDialog()
{
    play_callback_manager::get()->unregister_callback(this);
}

void Preferences::OnPathChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_PATH, path_);

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void Preferences::OnFormatChange(UINT, int, CWindow)
{
    // Get the text from the edit control.
    uGetDlgItemText(*this, IDC_FORMAT, format_);

    // Save the text to the config
    titleformat_compiler::get()->compile_safe_ex(script_, format_, nullptr);

    update_preview();

    // Notify the host that the preferences have changed.
    callback_->on_state_changed();
}

void Preferences::OnBrowse(UINT, int, HWND)
{
    pfc::string8 new_path;
    if (uGetOpenFileName(*this, "All files|*.*", 0, nullptr, "Save file...", nullptr, new_path, TRUE))
    {
        path_ = new_path;
        uSetDlgItemText(*this, IDC_PATH, path_);
    }
}

void Preferences::update_preview()
{
    pfc::string8 preview;
    playback_control::get()->playback_format_title(nullptr, preview, script_, nullptr,
                                                   playback_control::display_level_all);
    uSetDlgItemText(*this, IDC_PREVIEW, preview);
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
