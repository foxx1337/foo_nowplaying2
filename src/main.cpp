#include <SDK/foobar2000.h>
#include "TitleFormatEditor.h"

DECLARE_COMPONENT_VERSION("Now Playing 2", "5.0", "Now Playing by foxx1337\n\nhttps://github.com/foxx1337/foo_nowplaying2");

// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or
// loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME("foo_nowplaying2.dll");

namespace
{
    class EditorRuntimeShutdown : public initquit
    {
        void on_quit() override
        {
            if (!TitleFormatEditor::ShutdownRuntime())
            {
                console::printf("Now Playing 2: could not release Footilla resources (Windows error %lu).",
                                GetLastError());
            }
        }
    };

    initquit_factory_t<EditorRuntimeShutdown> editor_runtime_shutdown;
}
