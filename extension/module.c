//
// Created by hsk on 22-12-17.
//

#include <xorg/xf86Module.h>
#include <xorg/xorg-server.h>
#include <xorg/extension.h>

#include <wi-extra/define.h>
#include <wi-extra/proto.h>

#include "extension.h"

static XF86ModuleVersionInfo wi_ver = {
        WI_EXT_NAME,
        "get extra info for x window.",
        MODINFOSTRING1,
        MODINFOSTRING2,
        XORG_VERSION_CURRENT,
        WI_MAJOR_VERSION,
        WI_MINOR_VERSION,
        WI_PATCH_VERSION,
        ABI_CLASS_EXTENSION,
        ABI_EXTENSION_VERSION,
        MOD_CLASS_EXTENSION,
        {0, 0, 0, 0},
};

static ExtensionModule wi_ext = {
        &wi_ext_init,
        WI_EXT_NAME,
        NULL
};

static MODULESETUPPROTO(wi_setup);

_X_EXPORT XF86ModuleData wi_extraModuleData = {
        &wi_ver,
        wi_setup,
        NULL,
};

static void *wi_setup(UNUSED void *module, UNUSED void *opts, int *err_maj,
                      UNUSED int *err_min) {

    static Bool setup_done = FALSE;
    if (setup_done) {
        if (err_maj) {
            *err_maj = LDR_ONCEONLY;
        }
        return NULL;
    }

    LoadExtensionList(&wi_ext, 1, FALSE);

    return (void *) 1;
}