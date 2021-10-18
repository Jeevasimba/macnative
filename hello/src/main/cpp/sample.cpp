#include <ApplicationServices/ApplicationServices.h>
#include <IOKit/hidsystem/ev_keymap.h>
#include <IOKit/hidsystem/IOHIDLib.h>
#include <Carbon/Carbon.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
#include <unistd.h>


#include "stdlib.h"


using namespace std;

io_connect_t auxKeyDriverRef;
bool driverLoaded = false;




void doKeyboard(CGKeyCode key, bool down) {
    CGEventRef event = CGEventCreateKeyboardEvent(NULL, key, down);
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
}

void typeUniChar(ushort c, bool down = true, bool up = true) {
    CGEventSourceRef src = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    CGEventRef down_evt = CGEventCreateKeyboardEvent(src, (CGKeyCode) 0, true);
    CGEventRef up_evt = CGEventCreateKeyboardEvent(src, (CGKeyCode) 0, false);

    UniChar str[] = {(UniChar)c, '\0'};
    CGEventKeyboardSetUnicodeString(down_evt, 1, str);
    CGEventKeyboardSetUnicodeString(up_evt, 1, str);

    if(down)
        CGEventPost (kCGHIDEventTap, down_evt);
    if(up)
        CGEventPost (kCGHIDEventTap, up_evt);

    CFRelease (down_evt);
    CFRelease (up_evt);
    CFRelease (src);
}
