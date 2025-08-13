#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <X11/Xlib.h>

#include <iostream>
#include <map>
#include <string>

class Xwindow {
    Display *d;
    Window w;
    int s;
    GC gc;
    Pixmap buffer;
    
    int width, height;
    
    std::map<int, unsigned long> colorMap;
    
    public:
    Xwindow(int width=500, int height=500);  // Constructor; displays the window.
    ~Xwindow();                              // Destructor; destroys the window.
    Xwindow(const Xwindow&) = delete;
    Xwindow &operator=(const Xwindow&) = delete;
    
        enum {White=0, Black, Red, Green, Blue}; // Available colours.
        unsigned long colours[10];

        // Draws a rectangle
        void fillRectangle(int x, int y, int width, int height, unsigned long colour);

        // Draws a string
        void drawString(int x, int y, std::string msg);

        unsigned long getColor(int r, int g, int b);

        void flush();

};

#endif
