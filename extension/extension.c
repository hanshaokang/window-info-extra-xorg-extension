//
// Created by hsk on 22-12-17.
//
#include "extension.h"

#include <xorg/extnsionst.h>
#include <wi-extra/define.h>
#include <wi-extra/proto.h>

#define REPLY(type_name, sub_type)          \
    type_name rep = {                       \
        .type           = X_Reply,          \
        .sub            = sub_type,         \
        .sequenceNumber = client->sequence, \
        .length         = 0};


static int proc_wi_version(ClientPtr client) {
    REQUEST(xWIVersionReq);
    REQUEST_SIZE_MATCH(xWIVersionReq);

    REPLY(xWIReply, X_WIVersion)
    rep.data.version.majorVersion = WI_MAJOR_VERSION;
    rep.data.version.minorVersion = WI_MINOR_VERSION;

    // by default, server can support old client.
    // todo check if client of this version can support.

    if (version_compare(stuff->majorVersion, stuff->minorVersion,
                        rep.data.version.majorVersion, rep.data.version.minorVersion) < 0) {
        rep.data.version.majorVersion = stuff->majorVersion;
        rep.data.version.minorVersion = stuff->minorVersion;
    }

    // todo save client version info into clientPtr for future use.


    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swapl(&rep.data.version.majorVersion);
        swapl(&rep.data.version.minorVersion);
    }

    WriteToClient(client, sizeof(xWIReply), &rep);
    return Success;
}

static int s_proc_wi_version(ClientPtr client) {
    REQUEST(xWIVersionReq);
    REQUEST_SIZE_MATCH(xWIVersionReq);

    swaps(&stuff->length);
    swapl(&stuff->majorVersion);
    swapl(&stuff->minorVersion);
    return proc_wi_version(client);
}

static int proc_wi_window_pid(ClientPtr client) {
    REQUEST(xWIWindowPidReq);
    REQUEST_SIZE_MATCH(xWIWindowPidReq);

    REPLY(xWIReply, X_WIWindowPid)
    rep.data.windowPid.error = BadRequest;
    rep.data.windowPid.pid = 0;
    rep.data.windowPid.cmdLen = 0;
    rep.data.windowPid.argsLen = 0;

    const char *cmd = NULL;
    const char *args = NULL;
    do {
        int clientId = CLIENT_ID(stuff->windowId);
        if (clientId < 0 || clientId >= currentMaxClients) {
            rep.data.windowPid.error = BadWindow;
            break;
        }

        ClientPtr targetClient = clients[clientId];
        if (!targetClient) {
            rep.data.windowPid.error = BadWindow;
            break;
        }

        if (targetClient->clientState != ClientStateRunning) {
            rep.data.windowPid.error = BadWindow;
            break;
        }

        // if a remote client, should not get client pid
        if (!ClientIsLocal(targetClient)) {
            rep.data.windowPid.error = BadWindow;
            break;
        }
        rep.data.windowPid.pid = GetClientPid(targetClient);
        cmd = GetClientCmdName(targetClient);
        args = GetClientCmdArgs(targetClient);
        rep.data.windowPid.error = Success;
    } while (FALSE);

    CARD32 cmdLen = 0;
    if (cmd) {
        cmdLen = strlen(cmd) + 1;
        rep.data.windowPid.cmdLen = cmdLen;
    }
    CARD32 argsLen = 0;
    if (args) {
        argsLen = strlen(args) + 1;
        rep.data.windowPid.argsLen = argsLen;
    }

    rep.length = bytes_to_int32((int) (cmdLen + argsLen));

    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swapl(&rep.data.windowPid.error);
        swapl(&rep.data.windowPid.pid);
        swapl(&rep.data.windowPid.cmdLen);
        swapl(&rep.data.windowPid.argsLen);
    }

    char *data = (char *) calloc(1, rep.length << 2);

    if (!data) {
        return BadAlloc;
    }

    if (cmd) {
        memcpy(data, cmd, cmdLen);
    }

    if (args) {
        memcpy(data + cmdLen, args, argsLen);
    }

    WriteToClient(client, sizeof(xWIReply), &rep);
    WriteToClient(client, (int) rep.length << 2, data);
    free(data);

    return Success;
}

static int s_proc_wi_window_pid(ClientPtr client) {
    REQUEST(xWIWindowPidReq);
    REQUEST_SIZE_MATCH(xWIWindowPidReq);

    swaps(&stuff->length);
    swapl(&stuff->windowId);
    return proc_wi_window_pid(client);
}

static int proc_wi_dispatch(ClientPtr client) {
    REQUEST(xWIReq);
    switch (stuff->xWIReqType) {
        case X_WIVersion:
            return proc_wi_version(client);
        case X_WIWindowPid:
            return proc_wi_window_pid(client);
        default:
            return BadRequest;
    }
}

static int _X_COLD
s_proc_wi_dispatch(ClientPtr
                   client) {
    REQUEST(xWIReq);
    switch (stuff->xWIReqType) {
        case X_WIVersion:
            return
                    s_proc_wi_version(client);
        case X_WIWindowPid:
            return
                    s_proc_wi_window_pid(client);
        default:
            return
                    BadRequest;
    }
}

static void proc_wi_close_down(UNUSED ExtensionEntry *extEntry) {
    // todo if alloc memory in client, free it.
}

void wi_ext_init() {
    AddExtension(WI_EXT_NAME,
                 WI_NUMBER_EVENTS,
                 WI_NUMBER_ERRORS,
                 proc_wi_dispatch,
                 s_proc_wi_dispatch,
                 proc_wi_close_down,
                 StandardMinorOpcode
    );
}