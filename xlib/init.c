//
// Created by hsk on 22-12-18.
//
#include "init.h"

#include <X11/Xlibint.h>
#include <wi-extra/proto.h>
#include <wi-extra/wi-extra.h>

#define foreach(index, container) \
    for ((index) = (container)->head; (index); (index) = (index)->next)

WIExtInfo wiExtInfo;
char wiExtName[] = WI_EXT_NAME;

static WIExtDisplayInfo *wi_ext_find_display(WIExtInfo *extInfo, Display *dpy) {

    if (extInfo->cur && extInfo->cur->display == dpy) {
        return extInfo->cur;
    }

    WIExtDisplayInfo *info = NULL;

    {
        _XLockMutex(_Xglobal_lock);
        foreach(info, extInfo) {
            if (info->display == dpy) {
                extInfo->cur = info;
                break;
            }
        }
        _XUnlockMutex(_Xglobal_lock);
    }
    return info;
}

static int wi_ext_remove_display(WIExtInfo *extInfo, Display *dpy) {
    WIExtDisplayInfo *info, *prev;

    {
        _XLockMutex(_Xglobal_lock);
        prev = NULL;
        foreach(info, extInfo) {
            if (info->display == dpy) {
                break;
            }
            prev = info;
        }

        if (info) {
            if (prev) {
                prev->next = info->next;
            } else {
                extInfo->head = info->next;
            }
            extInfo->ndisplays--;

            if (info == extInfo->cur) {
                extInfo->cur = NULL;
            }
        }
        _XUnlockMutex(_Xglobal_lock);
    }

    if (info) {
        XFree(info);
        return True;
    } else {
        return False;
    }
}

static int wi_close_display(Display *dpy, __attribute__((unused)) XExtCodes *codes) {
    return wi_ext_remove_display(&wiExtInfo, dpy);
}

static WIExtDisplayInfo *wi_ext_add_display(WIExtInfo *extInfo,
                                            Display *dpy,
                                            char *extName) {
    WIExtDisplayInfo *info;

    info = (WIExtDisplayInfo *) Xmalloc(sizeof(WIExtDisplayInfo));
    if (!info) {
        return NULL;
    }

    info->display = dpy;
    info->codes = XInitExtension(dpy, extName);

    if (info->codes) {
        xWIVersionReq *req;
        xWIReply rep;

        XESetCloseDisplay(dpy, info->codes->extension, wi_close_display);

        LockDisplay(dpy);
        GetReq(WIVersion, req);
        req->reqType = info->codes->major_opcode;
        req->xWIReqType = X_WIVersion;
        req->majorVersion = WI_MAJOR_VERSION;
        req->minorVersion = WI_MINOR_VERSION;

        if (!_XReply(dpy, (xReply *) &rep, 0, xTrue)) {
            UnlockDisplay(dpy);
            SyncHandle();
            Xfree(info);
            return NULL;
        }
        info->major_version = (int) rep.data.version.majorVersion;
        info->minor_version = (int) rep.data.version.minorVersion;
        UnlockDisplay(dpy);
        SyncHandle();
    } else {
        /* The server doesn't have this extension.
         * Use a private Xlib-internal extension to hang the close_display
         * hook on so that the "cache" (extinfo->cur) is properly cleaned.
         * (XBUG 7955)
         */
        XExtCodes *codes = XAddExtension(dpy);
        if (!codes) {
            Xfree(info);
            return NULL;
        }
        XESetCloseDisplay(dpy, codes->extension, wi_close_display);
    }

    {
        _XLockMutex(_Xglobal_lock);
        info->next = extInfo->head;
        extInfo->head = info;
        extInfo->cur = info;
        extInfo->ndisplays++;
        _XUnlockMutex(_Xglobal_lock);
    }

    return info;
}

WIExtDisplayInfo *wi_find_display(Display *dpy) {
    WIExtDisplayInfo *info;

    info = wi_ext_find_display(&wiExtInfo, dpy);
    if (!info) {
        info = wi_ext_add_display(&wiExtInfo, dpy, wiExtName);
    }
    return info;
}

int WIQueryExtension(
        Display *dpy,
        int *event_base_return,
        int *error_base_return) {
    WIExtDisplayInfo *info = wi_find_display(dpy);

    if (!WIHasExtension(info)) {
        return BadRequest;
    }

    *event_base_return = info->codes->first_event;
    *error_base_return = info->codes->first_error;
    return Success;
}

int WIQueryVersion(
        Display *dpy,
        int *major_version_return,
        int *minor_version_return) {

    WIExtDisplayInfo *info = wi_find_display(dpy);

    if (info == NULL) {
        return BadRequest;
    }

    WICheckExtension(info, BadRequest)
    *major_version_return = info->major_version;
    *minor_version_return = info->minor_version;

    return Success;
}