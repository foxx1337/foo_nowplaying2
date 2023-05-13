#include <SDK/foobar2000.h>

DECLARE_COMPONENT_VERSION("Now Playing 2", "0.1", "Now Playing by foxx1337");

// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or
// loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME("foo_nowplaying2.dll");
