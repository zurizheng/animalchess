#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <unistd.h>

#include "window.h"

using namespace std;

Xwindow::Xwindow(int width, int height) : width{width}, height{height} {
    d = XOpenDisplay(NULL);
    if (d == NULL) {
        cerr << "Cannot open display" << endl;
        exit(1);
    }
    s = DefaultScreen(d);
    w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, 1,
                            BlackPixel(d, s), WhitePixel(d, s));
    XSelectInput(d, w, ExposureMask | KeyPressMask);
    XMapRaised(d, w);

    // Pixmap pix = XCreatePixmap(d, w, width, height, DefaultDepth(d, DefaultScreen(d)));
    buffer = XCreatePixmap(d, w, width, height, DefaultDepth(d, s));
    gc = XCreateGC(d, buffer, 0, (XGCValues *)0);

    XFlush(d);
    XFlush(d);

    // Set up colours.
    XColor xcolour;
    Colormap cmap;
    char color_vals[7][10]={"white", "black", "red", "green", "blue"};

    cmap=DefaultColormap(d,DefaultScreen(d));
    for(int i=0; i < 5; ++i) {
        XParseColor(d,cmap,color_vals[i],&xcolour);
        XAllocColor(d,cmap,&xcolour);
        colours[i]=xcolour.pixel;
    }

    XSetForeground(d,gc,colours[Black]);

    // Make window non-resizeable.
    XSizeHints hints;
    hints.flags = (USPosition | PSize | PMinSize | PMaxSize );
    hints.height = hints.base_height = hints.min_height = hints.max_height = height;
    hints.width = hints.base_width = hints.min_width = hints.max_width = width;
    XSetNormalHints(d, w, &hints);

    // XSynchronize(d, True);

    XSetForeground(d, gc, colours[White]);
    XFillRectangle(d, buffer, gc, 0, 0, width, height);
    XSetForeground(d, gc, colours[Black]);

    usleep(1000);
}

Xwindow::~Xwindow() {
    XFreePixmap(d, buffer);
    XFreeGC(d, gc);
    XCloseDisplay(d);
}

unsigned long Xwindow::getColor(int r, int g, int b) {
    // Combine RGB into a single key (works since values are 0-255)
    int key = (r << 16) | (g << 8) | b;

    auto it = colorMap.find(key);
    if (it != colorMap.end()) {
        return it->second;
    }

    XColor color;
    Colormap cmap = DefaultColormap(d, DefaultScreen(d));
    
    // X11 uses 16-bit color values (0-65535), so scale from 8-bit (0-255)
    color.red = r * 257;    // 257 = 65535/255
    color.green = g * 257;
    color.blue = b * 257;
    
    unsigned long pixel;
    if (XAllocColor(d, cmap, &color)) {
        pixel = color.pixel;
    } else {
        std::cout << "Failed to allocate color: (" << r << ", " << g << ", " << b << ")" << std::endl;
        pixel = colours[Black];
    }

    // cache result
    colorMap[key] = pixel;
    return pixel;
}

void Xwindow::fillRectangle(int x, int y, int width, int height, unsigned long colour) {
    XSetForeground(d, gc, colour);
    XFillRectangle(d, buffer, gc, x, y, width, height);
    XSetForeground(d, gc, colours[Black]);
}

void Xwindow::drawString(int x, int y, string msg) {
    XDrawString(d, buffer, DefaultGC(d, s), x, y, msg.c_str(), msg.length());
}

void Xwindow::flush() {
    XCopyArea(d, buffer, w, gc, 0, 0, width, height, 0, 0);
    XFlush(d);
}

