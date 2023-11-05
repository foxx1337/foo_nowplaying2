#include "TabLog.h"

#include <helpers/DarkMode.h>
#include <helpers/atl-misc.h>

#include "nowplaying.h"

void TabLog::Reset()
{
    
}

BOOL TabLog::OnInitDialog(CWindow, LPARAM)
{
    EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);

    // Enable dark mode.
    // One call does it all, applies all relevant hacks automatically.
    dark_mode_.AddDialogWithControls(*this);

    play_callback_manager::get()->register_callback(this, NowPlaying::playback_flags, true);

    return TRUE;
}

void TabLog::OnDestroyDialog()
{
    play_callback_manager::get()->unregister_callback(this);
}

void TabLog::update_preview() const
{
    
}
