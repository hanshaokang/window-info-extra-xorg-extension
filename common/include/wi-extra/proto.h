//
// Created by hsk on 22-12-17.
//

#ifndef WINDOW_INFO_EXTRA_XORG_EXTENSION_PROTO_H
#define WINDOW_INFO_EXTRA_XORG_EXTENSION_PROTO_H

#include <X11/Xmd.h>
#define WI_EXT_NAME "WI-EXTRA"

#define WI_MAJOR_VERSION 1
#define WI_MINOR_VERSION 0
#define WI_PATCH_VERSION 0

#define WI_NUMBER_EVENTS 0
#define WI_NUMBER_ERRORS 0

enum X_REQUEST_NUM {
    X_WIVersion,  // 0
    X_WIWindowPid,
};

typedef struct {
    CARD8 reqType;
    CARD8 xWIReqType;
    CARD16 length B16;
} xWIReq;

typedef struct {
    CARD8 reqType;
    CARD8 xWIReqType;
    CARD16 length B16;
    CARD32 majorVersion B32;
    CARD32 minorVersion B32;
} xWIVersionReq;

#define sz_xWIVersionReq   12

typedef struct {
    CARD8 reqType;
    CARD8 xWIReqType;
    CARD16 length B16;
    CARD32 windowId B32;
} xWIWindowPidReq;

#define sz_xWIWindowPidReq   8


typedef struct {
    BYTE type;   /* X_Reply */
    BYTE sub;
    CARD16 sequenceNumber B16;
    CARD32 length B32;
    union {
        struct {
            CARD32 pad1 B32;
            CARD32 pad2 B32;
            CARD32 pad3 B32;
            CARD32 pad4 B32;
            CARD32 pad5 B32;
            CARD32 pad6 B32;
        };
        struct {
            CARD32 majorVersion B32;
            CARD32 minorVersion B32;
        } version;
        struct {
            CARD32 error B32;
            CARD32 pid B32;
            CARD32 cmdLen B32;
            CARD32 argsLen B32;
        } windowPid;
    } data;
} xWIReply;

#endif //WINDOW_INFO_EXTRA_XORG_EXTENSION_PROTO_H
