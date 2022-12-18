//
// Created by hsk on 22-12-18.
//

#ifndef WINDOW_INFO_EXTRA_XORG_EXTENSION_WI_EXTRA_H
#define WINDOW_INFO_EXTRA_XORG_EXTENSION_WI_EXTRA_H

#include <wi-extra/define.h>
#include <X11/Xlib.h>

_XFUNCPROTOBEGIN
_X_EXPORT int WIQueryExtension(
        IN Display *dpy,
        OUT int *event_base_return,
        OUT int *error_base_return);

_X_EXPORT int WIQueryVersion(
        IN Display *dpy,
        OUT int *major_version_return,
        OUT int *minor_version_return);

_X_EXPORT int WIQueryWindowPid(
        IN Display *dpy,
        IN Window windowId,
        OUT pid_t *windowPid,
        OUT char **cmd,
        OUT char **args);

_XFUNCPROTOEND
#endif //WINDOW_INFO_EXTRA_XORG_EXTENSION_WI_EXTRA_H
