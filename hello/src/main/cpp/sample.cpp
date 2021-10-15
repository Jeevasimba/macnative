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
void initAuxKeyDriver()
{
    mach_port_t masterPort, service, iter;

    if(IOMasterPort( bootstrap_port, &masterPort ) != KERN_SUCCESS)
        return;

    if(IOServiceGetMatchingServices(masterPort, IOServiceMatching( kIOHIDSystemClass), &iter ) != KERN_SUCCESS)
        return;

    service = IOIteratorNext( iter );
    if(service == 0) {
        IOObjectRelease(iter);
        return;
    }

    if(IOServiceOpen(service, mach_task_self(), kIOHIDParamConnectType, &auxKeyDriverRef ) == KERN_SUCCESS)
        driverLoaded = true;

    IOObjectRelease(service);
    IOObjectRelease(iter);
}

void tapAuxKey(const UInt8 auxKeyCode)
{
    if(!driverLoaded)
        return;

    NXEventData event;
    IOGPoint loc = { 0, 0 };

    // down
    UInt32 evtInfo = auxKeyCode << 16 | NX_KEYDOWN << 8;
    bzero(&event, sizeof(NXEventData));
    event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
    event.compound.misc.L[0] = evtInfo;
    if(IOHIDPostEvent( auxKeyDriverRef, NX_SYSDEFINED, loc, &event, kNXEventDataVersion, 0, FALSE ) != KERN_SUCCESS)
        return;

    // up
    evtInfo = auxKeyCode << 16 | NX_KEYUP << 8;
    bzero(&event, sizeof(NXEventData));
    event.compound.subType = NX_SUBTYPE_AUX_CONTROL_BUTTONS;
    event.compound.misc.L[0] = evtInfo;
    if(IOHIDPostEvent( auxKeyDriverRef, NX_SYSDEFINED, loc, &event, kNXEventDataVersion, 0, FALSE ) != KERN_SUCCESS)
        return;
}
