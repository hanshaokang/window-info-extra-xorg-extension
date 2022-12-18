//
// Created by hsk on 2022/3/10.
//

#ifndef WINDOW_INFO_EXTRA_XORG_EXTENSION_INIT_H
#define WINDOW_INFO_EXTRA_XORG_EXTENSION_INIT_H

#include <X11/Xlib.h>
/**
 * 扩展信息链表，用来存储与不同display关联的插件信息
 */
typedef struct WIExtDisplayInfo {
    struct WIExtDisplayInfo *next; // 链表指针
    Display *display;
    XExtCodes *codes;   // 插件信息
    int major_version;  // 主版本
    int minor_version;  // 子版本
} WIExtDisplayInfo;

/**
 * 用来存储所有display的插件信息的列表
 */
typedef struct WIExtInfo {
    WIExtDisplayInfo *head; // 链表的头部指针
    WIExtDisplayInfo *cur;  // 最近一次使用的信息， 用来加速查找。
    int ndisplays;  // 链表大小
} WIExtInfo;


WIExtDisplayInfo *wi_find_display(Display *dpy);

#define WIHasExtension(i) ((i) && ((i)->codes))

#define WICheckExtension(i, val) \
    if (!WIHasExtension(i)) { return val; }

#endif //WINDOW_INFO_EXTRA_XORG_EXTENSION_INIT_H
