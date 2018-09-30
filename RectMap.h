#ifndef RECTMAP_H_
#define RECTMAP_H_

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include<vector>
#include <string>

class RectMap {
public:

	// constructor
	RectMap(int lines, int rows, int points, unsigned short width, unsigned short height);

	// destructor
	~RectMap();
    
	// initialize the rectangles
	void addRect();

	// method to get the overall points the player has got
	int getPoints();

    // method to get a rectangle array for the given row
	XRectangle * getRectArray(int row);

	// method to get the number of rectangles in the given row
	int getNum(int row);

    // method to pop out a rectangle which the ball hits on
	bool destroyRectangle(XPoint lastBallPos, XPoint ballPos, XPoint * ballDir, int ballSize);

	// method to draw the rectangles
    void drawRectangels(Display * display, Window window, GC gc, GC * gcColored);

private: 
	int lines, rows, num, points;
	unsigned short width;
	unsigned short height;
	std::vector< std::vector<XRectangle> > rects; 
};

#endif
