//
// Created by hsk on 22-12-18.
//

#include <wi-extra/wi-extra.h>
#include <wi-extra/proto.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>

#include "init.h"

_X_EXPORT int WIQueryWindowPid(
        IN Display *dpy,
        IN Window windowId,
        OUT pid_t *windowPid,
        OUT char **cmd,
        OUT char **args) {
    if (!dpy) {
        return BadRequest;
    }

    WIExtDisplayInfo *info = wi_find_display(dpy);
    WICheckExtension(info, BadRequest)

    xWIWindowPidReq *req;
    xWIReply rep;

    Status ret = BadRequest;

    LockDisplay(dpy);
    do {
        GetReq(WIWindowPid, req);
        req->reqType = info->codes->major_opcode;
        req->xWIReqType = X_WIWindowPid;
        req->windowId = windowId;

        if (!_XReply(dpy, (xReply *) &rep, 0, xFalse)) {
            break;
        }

        if (rep.data.windowPid.error != Success) {
            ret = (int) rep.data.windowPid.error;
            break;
        }

        CARD32 extLength = rep.length;
        do {
            if (rep.data.windowPid.error != Success) {
                ret = (int) rep.data.windowPid.error;
                break;
            }

            if (windowPid) {
                *windowPid = (pid_t) rep.data.windowPid.pid;
            }

            CARD32 cmdLen = rep.data.windowPid.cmdLen;
            CARD32 argsLen = rep.data.windowPid.argsLen;
            CARD32 total = cmdLen + argsLen;
            if (cmdLen > 10240 || argsLen > 10240 || total > 10240) {
                ret = BadAlloc;
                break;
            }

            char *data = (char *) calloc(1, rep.length << 2);
            if (!data) {
                break;
            }
            _XReadPad(dpy, data, rep.length << 2);
            extLength = 0;

            if (cmd != NULL) {
                *cmd = calloc(1, cmdLen);
                if (*cmd) {
                    memcpy(*cmd, data, cmdLen - 1);
                }
            }

            if (args != NULL) {
                *args = calloc(1, argsLen);
                if (*args) {
                    memcpy(*args, data + cmdLen, argsLen - 1);
                }
            }

            free(data);
            ret = Success;
        } while (xFalse);

        _XEatDataWords(dpy, extLength);
    } while (xFalse);
    UnlockDisplay(dpy);
    SyncHandle();
    return ret;
}