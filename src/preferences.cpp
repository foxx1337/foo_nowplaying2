#include <helpers/foobar2000+atl.h>
#include <helpers/atl-misc.h>
#include <helpers/DarkMode.h>

#include "preferences.h"
#include "NowPlaying.h"
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


// {75B61323-4ABB-4692-9483-800B00A30E6B}
static constexpr GUID guid_file_encoding
{ 0x75b61323, 0x4abb, 0x4692, { 0x94, 0x83, 0x80, 0xb, 0x0, 0xa3, 0xe, 0x6b } };

const std::vector<encoding_info> encodings
{
    encoding_info{encoding::UTF8, L"UTF-8", {0xef, 0xbb, 0xbf}},
    encoding_info{encoding::UTF16LE, L"UTF-16 LE", {0xff, 0xfe}}
};

static constexpr t_uint32 default_file_encoding = static_cast<t_uint32>(encoding::UTF8);

cfg_uint file_encoding(guid_file_encoding, default_file_encoding);


// {25E80CCB-B77E-428C-AC0C-89DC08531C9B}
static constexpr GUID guid_with_bom
{ 0x25e80ccb, 0xb77e, 0x428c, { 0xac, 0xc, 0x89, 0xdc, 0x8, 0x53, 0x1c, 0x9b } };

static constexpr bool default_with_bom = false;

cfg_bool with_bom(guid_with_bom, default_with_bom);


// {22F1D723-9487-4791-851B-1D2C74E0F4A0}
static const GUID guid_file_append
{ 0x22f1d723, 0x9487, 0x4791, { 0x85, 0x1b, 0x1d, 0x2c, 0x74, 0xe0, 0xf4, 0xa0 } };

static constexpr bool default_file_append = false;

cfg_bool file_append(guid_file_append, default_file_append);


// {26D45612-B6EC-4B8D-BF27-251639114FA4}
static constexpr GUID guid_max_lines
{ 0x26d45612, 0xb6ec, 0x4b8d, { 0xbf, 0x27, 0x25, 0x16, 0x39, 0x11, 0x4f, 0xa4 } };

static constexpr t_uint32 default_max_lines = 0;

cfg_uint max_lines(guid_max_lines, default_max_lines);


class Preferences : public CDialogImpl<Preferences>, public preferences_page_instance, private play_callback_impl_base
{
public:
    // Constructor - invoked by preferences_page_impl helpers - don't do Create() in here, preferences_page_impl does this for us.
    Preferences(preferences_page_callback::ptr callback) :
        callback_(callback), path_(file_path.get()), format_(playback_format.get()), file_encoding_(static_cast<t_uint>(file_encoding)),
        with_bom_(with_bom), file_append_(file_append), max_lines_(static_cast<t_uint>(max_lines))
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
        file_path = path_;
        playback_format = format_;
        file_encoding = file_encoding_;
        with_bom = with_bom_;
        file_append = file_append_;
        max_lines = max_lines_;
        g_nowplaying2.get_static_instance().refresh_settings(true);
    }

    void reset() override
    {
        // Reset to default.
        path_ = default_file_path;
        uSetDlgItemText(*this, IDC_PATH, path_);

        format_ = default_playback_format;
        uSetDlgItemText(*this, IDC_FORMAT, format_);

        file_encoding_ = default_file_encoding;
        CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
        file_encoding.SetCurSel(file_encoding_);

        with_bom_ = default_with_bom;
        CCheckBox with_bom{GetDlgItem(IDC_WITH_BOM)};
        with_bom.SetCheck(with_bom_ ? BST_CHECKED : BST_UNCHECKED);

        file_append_ = default_file_append;
        CCheckBox file_append{GetDlgItem(IDC_FILE_APPEND)};
        file_append.SetCheck(file_append_ ? BST_CHECKED : BST_UNCHECKED);

        max_lines_ = default_max_lines;
        CEdit max_lines{GetDlgItem(IDC_MAX_LINES)};
        max_lines.SetWindowText(L"");

        update_preview();
    }


    // WTL message map
    BEGIN_MSG_MAP_EX(Preferences)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroyDialog)
        COMMAND_HANDLER_EX(IDC_PATH, EN_CHANGE, OnPathChange)
        COMMAND_HANDLER_EX(IDC_FORMAT, EN_CHANGE, OnFormatChange)
        COMMAND_HANDLER_EX(IDC_BUTTON_BROWSE, BN_CLICKED, OnBrowse)
        COMMAND_HANDLER_EX(IDC_FILE_ENCODING, CBN_SELCHANGE, OnFileEncodingChange)
        COMMAND_HANDLER_EX(IDC_WITH_BOM, BN_CLICKED, OnFileEncodingChange)
        COMMAND_HANDLER_EX(IDC_FILE_APPEND, BN_CLICKED, OnFileAppendChange)
        COMMAND_HANDLER_EX(IDC_MAX_LINES, EN_CHANGE, OnMaxLinesChange)
    END_MSG_MAP()

private:
    const preferences_page_callback::ptr callback_;

    // Dark mode hooks object, must be a member of dialog class.
    fb2k::CDarkModeHooks dark_mode_;

    pfc::string8 path_;

    pfc::string8 format_;

    titleformat_object::ptr script_;

    t_uint file_encoding_;

    bool with_bom_;

    bool file_append_;

    t_uint max_lines_;

    // WTL handlers.
    BOOL OnInitDialog(CWindow, LPARAM);
    void OnDestroyDialog();
    void OnPathChange(UINT, int, CWindow);
    void OnFormatChange(UINT, int, CWindow);
    void OnBrowse(UINT, int, CWindow);
    void OnFileEncodingChange(UINT, int, CWindow);
    void OnFileAppendChange(UINT, int, CWindow);
    void OnMaxLinesChange(UINT, int, CWindow);

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
        if (format_ != playback_format || path_ != file_path || file_encoding_ != file_encoding ||
            with_bom_ != with_bom || file_append_ != file_append || max_lines_ != max_lines)
        {
            return preferences_state::changed;
        }
        else
        {
            return 0;
        }
    }
};

BOOL Preferences::OnInitDialog(CWindow, LPARAM)
{
    // Enable dark mode.
    // One call does it all, applies all relevant hacks automatically.
    dark_mode_.AddDialogWithControls(*this);

    uSetDlgItemText(*this, IDC_PATH, path_);
    uSetDlgItemText(*this, IDC_FORMAT, format_);
    titleformat_compiler::get()->compile_safe_ex(script_, playback_format.get(), nullptr);

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

void Preferences::OnBrowse(UINT, int, CWindow)
{
    pfc::string8 new_path;
    if (uGetOpenFileName(*this, "All files|*.*", 0, nullptr, "Save file...", nullptr, new_path, TRUE))
    {
        path_ = new_path;
        uSetDlgItemText(*this, IDC_PATH, path_);
    }
}

void Preferences::OnFileEncodingChange(UINT, int, CWindow)
{
    CComboBox file_encoding{GetDlgItem(IDC_FILE_ENCODING)};
    file_encoding_ = file_encoding.GetCurSel();
    CCheckBox with_bom{GetDlgItem(IDC_WITH_BOM)};
    with_bom_ = with_bom.GetCheck() == BST_CHECKED;
    callback_->on_state_changed();
}

void Preferences::OnFileAppendChange(UINT, int, CWindow)
{
    CCheckBox file_append{GetDlgItem(IDC_FILE_APPEND)};
    file_append_ = file_append.GetCheck() == BST_CHECKED;
    CEdit max_lines{GetDlgItem(IDC_MAX_LINES)};
    max_lines.EnableWindow(file_append_ ? TRUE : FALSE);
    callback_->on_state_changed();
}

void Preferences::OnMaxLinesChange(UINT, int, CWindow)
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
    callback_->on_state_changed();
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
