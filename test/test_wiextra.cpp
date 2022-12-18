//
// Created by hsk on 22-12-18.
//
#include <gtest/gtest.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <wi-extra/wi-extra.h>

class XorgTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 远程调试
//        setenv("DISPLAY", "localhost:10.0", 1);
        // 本地调试
        setenv("DISPLAY", ":0", false);
        dpy = XOpenDisplay(nullptr);

        if (dpy == nullptr) {
            std::cout << "can not open display." << std::endl;
            exit(1);
        }
    }

    void TearDown() override {
        XCloseDisplay(dpy);
    }

    Window CreateWindow() {
        int screen = DefaultScreen(dpy);
        Window rootWindow = DefaultRootWindow(dpy);

        XVisualInfo visualInfo;
        XMatchVisualInfo(dpy, screen, 32, TrueColor, &visualInfo);

        XSetWindowAttributes attributes;
        attributes.colormap = XCreateColormap(dpy, rootWindow, visualInfo.visual, AllocNone);
        attributes.border_pixel = BlackPixel(dpy, screen);
        attributes.background_pixel = BlackPixel(dpy, screen);
        attributes.override_redirect = true;

        unsigned long attrMask = CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect;
        Window window = XCreateWindow(dpy, rootWindow, 0, 0, 10, 10, 0, visualInfo.depth, InputOutput,
                                      visualInfo.visual, attrMask, &attributes);
        XMapWindow(dpy, window);
        return window;
    }

protected:
    Display *dpy = nullptr;
};

TEST_F(XorgTest, TestExtensionLoad) {
    int event_base_return, error_base_return;
    ASSERT_EQ(WIQueryExtension(dpy, &event_base_return, &error_base_return), Success);
}

TEST_F(XorgTest, TestWindowPid) {
    int event_base_return, error_base_return;
    ASSERT_EQ(WIQueryExtension(dpy, &event_base_return, &error_base_return), Success);
    auto window = CreateWindow();

    pid_t pid;
    char *cmd = nullptr;
    char *args = nullptr;
    ASSERT_EQ(WIQueryWindowPid(dpy, window, &pid, &cmd, &args), Success);
    printf("%d: %s && %s\n", pid, cmd, args);
    free(cmd);
    free(args);
    ASSERT_EQ(WIQueryWindowPid(dpy, window, &pid, &cmd, &args), Success);
    printf("%d: %s && %s\n", pid, cmd, args);
    free(cmd);
    free(args);
    ASSERT_EQ(pid, getpid());
}