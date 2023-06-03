#include "nowplaying.h"
#include "preferences.h"

void NowPlaying::SetScript(const pfc::string8& script)
{
    titleformat_compiler::get()->compile(script_, script);
}

void NowPlaying::Update(const char* event)
{
    SetScript(playback_format.get());

    pfc::mutexScope writer(thread_lock_);
    playback_control::get()->playback_format_title(nullptr, playback_string_, script_, nullptr,
                                                   playback_control::display_level_all);
    // console::printf("nowplaying2 %s - %s", event, playback_string_.c_str());

    // This is how one gets a wchar_t string.
    //const pfc::stringcvt::string_wide_from_utf8_t buffer(playback_string_);
}


service_factory_single_t<NowPlaying> g_nowplaying2;
