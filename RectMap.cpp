#include "RectMap.h"

using namespace std;

// ctor
RectMap::RectMap(int lines, int rows, int points, unsigned short width, unsigned short height): 
	        lines(lines), rows(rows), points(points), width(width), height(height) {
	num = rows * lines;
	addRect();
}

// destructor
RectMap::~RectMap() {
	// intentially empty
}
    
// initialize the rectangles
void RectMap::addRect() {
	// initial position of the right uppermost rectangle 
	short xPosition = 140;
	short yPosition = 100;

	for(int i = 0; i < rows; ++i) {
		// vector to store rectangles in raw i;
		vector<XRectangle> ithRow;

		for(int j = 0; j < lines; ++j) {
			XRectangle singleRectangle = {xPosition, yPosition, width, height};
			ithRow.push_back(singleRectangle);
			// update the xPosition for the next rectangle
			// Note that the height of rectangle in the same row is unchanged
			xPosition += width;
		}

		rects.push_back(ithRow);

		// reset the xPosition and increase the yPosition for the next raw
		xPosition = 140;
		yPosition += height;
	}
}

// method to get the overall points the player has got
int RectMap::getPoints() {
	return points;
}

// method to get a rectangle array for the given row
XRectangle * RectMap::getRectArray(int row) {
	XRectangle * rectArray = &(rects[row][0]);
	return rectArray;
}

// method to get the number of rectangles in the given row
int RectMap::getNum(int row) {
	return rects[row].size();
}

// method to pop out a rectangle which the ball hits on
bool RectMap::destroyRectangle(XPoint lastBallPos, XPoint ballPos, XPoint * ballDir, int ballSize) {
	for (int row = 0; row < rects.size(); ++row) {
		short topEdge = 100 + row * height;
		short bottomEdge = 100 + row * height + height;
		bool destroyed = false;

		// ball is in the range of the row
		if (ballPos.y + ballSize/2 >= topEdge && ballPos.y - ballSize/2 <= bottomEdge) {
			for(int line = 0; line < rects[row].size(); ++line) {
				short leftEdge = rects[row][line].x;
				short rightEdge = rects[row][line].x + width;
				// ball is in the range of a rectangle
				if (ballPos.x + ballSize/2 >= leftEdge &&
					ballPos.x - ballSize/2 <= rightEdge) {
					if (lastBallPos.y <= topEdge) { // ball hits top edge
						ballDir->y = -ballDir->y;
					}
					else if (lastBallPos.y >= bottomEdge) { // ball hits bottom edge
						ballDir->y = -ballDir->y;
					}
					else if (lastBallPos.x <=  leftEdge) { // ball hits left edge
						ballDir->x = -ballDir->x;
					}
					else if (lastBallPos.x >= rightEdge) { // ball hits right edge
						ballDir->x = -ballDir->x;
					}
					else {
						// should never be in this stage
						// for test purpose only
					}

					--num;
					++points;
					rects[row].erase(rects[row].begin() + line);
					//if (rects[row].size() == 0) {
					//	rects.erase(rects.begin() + row);
					//}
					destroyed = true;
					break;
				}
			}
		}
		if(destroyed) break;
	}
	return(num == 0);
}

// method to draw the rectangles
void RectMap::drawRectangels(Display * display, Window window, GC gc, GC * gcColored) {
	for(int i = 0; i < rects.size(); ++i) {
		XFillRectangles(display, window, gcColored[i], getRectArray(i), getNum(i));
		XDrawRectangles(display, window, gc, getRectArray(i), getNum(i));
	}
	return;
}
