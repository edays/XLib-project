#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include<vector>
#include <string>
#include "RectMap.h"

using namespace std;


// X11 structures
Display* display;
Window window;

// fixed frames per second animation
int FPS = 60;
int ballSpeed = 3;

// get current time
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

// entry point
int main( int argc, char *argv[] ) {
	if (argc == 1) {
		// use default parameter
	}
	else if (argc != 3 ||
	     atoi(argv[1]) > 60 ||
		 atoi(argv[1]) < 30 ||
		 atoi(argv[2]) > 10 ||
		 atoi(argv[2]) < 1) {
		cerr << "The format of input is: "
		     << "frame-rate [30, 60]\n"
			 << "ball speed [1, 10]" << endl;
		exit(0);
	}
	else {
		FPS = atoi(argv[1]);
		ballSpeed = atoi(argv[2]);
	}

	// pass in the command line argument
	// initial the number of rows and lines of the rectangles
	// initial the size of each rectangle
	// initial the points of the player
	int rows = 5, lines = 10, width = 100, height = 35;

	// initial the size of the bar
	int barWidth = 400;
	int barHeight = 10;

	// initial the bar movement speed
	int barSpeed = 100;

	// create window
	display = XOpenDisplay("");
	if (display == NULL) exit (-1);
	int screennum = DefaultScreen(display);
	long background = WhitePixel(display, screennum);
	long foreground = BlackPixel(display, screennum);
	window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                            10, 10, 1280, 800, 2, foreground, background);

	// set events to monitor and display window
	XSelectInput(display, window, ButtonPressMask | KeyPressMask);
	XMapRaised(display, window);
	XFlush(display);

	// create colored gc for coloring the rectangles	
	GC gcKhaki = XCreateGC(display, window, 0, 0); // #F0E68C
	XSetForeground(display, gcKhaki, 0xF0E68C);
	GC gcLightGreen = XCreateGC(display, window, 0, 0); //#90EE90
	XSetForeground(display, gcLightGreen, 0x90EE90);
	GC gcHotPink = XCreateGC(display, window, 0, 0);// #FF69B4
	XSetForeground(display, gcHotPink, 0xFF69B4);
	GC gcMidnightBlue = XCreateGC(display, window, 0, 0); //#191970
	XSetForeground(display, gcMidnightBlue, 0x191970);
	GC gcYellow = XCreateGC(display, window, 0, 0); //#FFFF00
	XSetForeground(display, gcYellow, 0xFFFF00);
	GC gcColored[5] = {gcKhaki, gcLightGreen, gcHotPink, gcMidnightBlue, gcYellow};	

	// ball postition, size, and velocity
	XPoint ballPos;
	ballPos.x = 400;
	ballPos.y = 400;
	int ballSize = 10;

	XPoint ballDir;
	ballDir.x = (60 * ballSpeed) / FPS; // the speed should be MAX FPS * speed/real fps
	ballDir.y = (60 * ballSpeed) / FPS; 

	// block position, size
	XPoint rectPos;
	rectPos.x = 300;
	rectPos.y = 750;

	// create gc for drawing
	GC gc = XCreateGC(display, window, 0, 0);
	XWindowAttributes w;
	XGetWindowAttributes(display, window, &w);

	// save time of last window paint
	unsigned long lastRepaint = 0;

	// save the last position of the ball
	XPoint lastBallPos;

	// event handle for current event
	XEvent event;

	// setup the rectangles
	RectMap rectanglesMap(lines, rows, 0, width, height);

	// bools to determine game stage
	bool welcome = true, gameOver = false, nextGame = false, quit = false;

	// bool if window has been cleared
	bool windowCleared = false;

	// event loop
	while ( true ) {
			
		// process if we have any events
		if (XPending(display) > 0) { 
			XNextEvent( display, &event ); 

			switch ( event.type ) {

				// mouse button press
				case ButtonPress:
					cout << "CLICK" << endl;
					break;

				case KeyPress: // any keypress
					KeySym key;
					char text[10];
					int i = XLookupString( (XKeyEvent*)&event, text, 10, &key, 0 );

					// if user is in the welcome page,
					// press any key other than q to start game
					if ( welcome ) { // in the welcome page
						if ( i == 1 ) {
							welcome = false;
						}
					}

					// user has pressed quit
					// press any key to close the window
					if (quit) {
						if ( i == 1 ) {
							XCloseDisplay(display);
							exit(0);
						}
					}

					// user loose
					// press r to restart a game
					// press other keys to close the window
					if ( gameOver ) {
						if ( i == 1 && text[0] == 'r' ) {
							// reset the bools and rectangles
							gameOver = false;
							windowCleared = false;
							RectMap newMap(lines, rows, 0, width, height);
							rectanglesMap = newMap;

							// reset ball
							ballPos.x = 400;
							ballPos.y = 400;

							ballDir.x = (60 * ballSpeed) / FPS;
							ballDir.y = (60 * ballSpeed) / FPS;

							// reset bar
							rectPos.x = 300;
							rectPos.y = 750;
						}
						if ( i == 1 && text[0] == 'q' ) {
							XCloseDisplay(display);
							exit(0);
						}
					}

					// user win
					// press r to restart a game
					// press q to quit
					if ( nextGame ) {
						if ( i == 1 && text[0] == 'r' ) {
							// reset the bools and rectangles
							nextGame = false;
							windowCleared = false;
							RectMap newMap(lines, rows, rectanglesMap.getPoints(), width, height);
							rectanglesMap = newMap;

							// reset ball
							ballPos.x = 400;
							ballPos.y = 400;

							ballDir.x = (60 * ballSpeed) / FPS;
							ballDir.y = (60 * ballSpeed) / FPS;

							// reset bar
							rectPos.x = 300;
							rectPos.y = 750;
						}
						if ( i == 1 && text[0] == 'q' ) {
							XCloseDisplay(display);
							exit(0);
						}
					}

					// move right
					if ( i == 1 && text[0] == 'd' ) {
						if (rectPos.x + barWidth + barSpeed >= w.width) {
							rectPos.x = w.width - barWidth;
						} else {
							rectPos.x += barSpeed;
						}
					}

					// move left
					if ( i == 1 && text[0] == 'a' ) {
						if (rectPos.x - barSpeed < 0) {
							rectPos.x = 0;
						} else {
							rectPos.x -= barSpeed;
						}
					}

					// quit game
					if ( i == 1 && text[0] == 'q' ) {
						quit = true;
					}
					break;

				}
		}

		// display welcome messages
		if (welcome) {
        	vector<string> welcomeString;
			string s = "Welcome to this super interesting game (Maybe..)";
			welcomeString.push_back(s);
			s = "About the developer: Jiayi Liu  j457liu";
			welcomeString.push_back(s);
			s = "How to play?"; 
			welcomeString.push_back(s);
			s = "Press a to move the paddle to left"; 
			welcomeString.push_back(s);
			s = "Press d to move the paddle to right"; 
			welcomeString.push_back(s);
			s = "Press q to quit. After checking your score again, press any key to close"; 
			welcomeString.push_back(s);
			s = "If the ball hits the bottom, then GG WELL PLAY! Then press r to restart";
			welcomeString.push_back(s);
			s = "Now, you are well prepared!";
			welcomeString.push_back(s);
			s = "Good Luck! Now, press any key to start or q to quit";
			welcomeString.push_back(s);

        	int xStringPosition = 400;
			int yStringPosition = 100;
        	for(int i = 0; i < welcomeString.size(); ++i) {
	    		XDrawString(display, window, gc, xStringPosition, yStringPosition, 
				            welcomeString[i].c_str(), welcomeString[i].size());
				yStringPosition += 50;
			}

			// keep while loop until user press the key to proceed
			continue;
		}

        // dislay instruction to quit the game
		if ( quit ) {
			if (!windowCleared) {
				XClearWindow(display, window);
				windowCleared = true;
			}
			string s = "Quit. Thanks for playing. Your scores is : "
			          + to_string(rectanglesMap.getPoints());
			XDrawString(display, window, gc, 400, 300, s.c_str(), s.size());
			s = "Press any key to close the window";
			XDrawString(display, window, gc, 400, 400, s.c_str(), s.size());
			continue;
		}

        // display instructions after game over
		if ( gameOver ) {
			if (!windowCleared) {
				XClearWindow(display, window);
				windowCleared = true;
			}
			string s = "You loose. Your score is : " +
			             to_string(rectanglesMap.getPoints());
			XDrawString(display, window, gc, 400, 300, s.c_str(), s.size());
			s = "Press r to start another game. Press q to quit.";
			XDrawString(display, window, gc, 400, 400, s.c_str(), s.size());
			continue;
		}

		if ( nextGame ) {
			if (!windowCleared) {
				XClearWindow(display, window);
				windowCleared = true;
			}
			string s = "Good job! Your total score is : " +
			              to_string(rectanglesMap.getPoints());
			XDrawString(display, window, gc, 400, 300, s.c_str(), s.size());
			s = "Press r to the next round! Press q to quit.";
			XDrawString(display, window, gc, 400, 400, s.c_str(), s.size());
			continue;
		}


		unsigned long end = now();	// get current time in microsecond

		if (end - lastRepaint > 1000000 / FPS) { 

			// clear background
			XClearWindow(display, window);

			// draw rectangle
			XDrawRectangle(display, window, gc, rectPos.x, rectPos.y, barWidth, barHeight);
			GC gcBlack = XCreateGC(display, window, 0, 0);
			XSetForeground(display, gcBlack, 0x000000);
			XFillRectangle(display, window, gcBlack, rectPos.x, rectPos.y, barWidth, barHeight);
			
			// draw Rectangles
			rectanglesMap.drawRectangels(display, window, gc, gcColored);

			string fpsString = "FPS: " + to_string(FPS);
			XDrawString(display, window, gc, 20, 20,fpsString.c_str(), fpsString.size());

			string speedString = "Ball Speed: " + to_string(ballSpeed);
			XDrawString(display, window, gc, 20, 60, speedString.c_str(), speedString.size());
	
			string pointsString = "Score: " + to_string(rectanglesMap.getPoints());
			XDrawString(display, window, gc, 20, 100, pointsString.c_str(), pointsString.size());

			// draw ball from centre
			XFillArc(display, window, gc, 
				ballPos.x - ballSize/2, 
				ballPos.y - ballSize/2, 
				ballSize, ballSize,
				0, 360*64);

			// update ball position
			ballPos.x += ballDir.x;
			ballPos.y += ballDir.y;

			// bounce ball with the edge of the bottom bar
			if (ballPos.y >= rectPos.y) {// && ballPos.y <= rectPos.y + barHeight) {
				if (ballPos.x >= rectPos.x && ballPos.x <= rectPos.x + barWidth) {
					if (lastBallPos.y <= rectPos.y) { // hits the top edge
						ballDir.y = -ballDir.y;
					}
					else if (lastBallPos.x >= rectPos.x + barWidth) { // hits the right edge
						ballDir.x = -ballDir.x;
					}
					else if (lastBallPos.x <= rectPos.x) { // hits the left edge
						ballDir.x = -ballDir.x;
					}
					else { // hits the bottom edge
					       // should never go into this case
						   // designed for test purpose only
						//ballDir.y = -ballDir.y;
					}
				}
			}

			// bounce the ball with the screen edge
			if (ballPos.x + ballSize/2 > w.width ||
				ballPos.x - ballSize/2 < 0) 
				ballDir.x = -ballDir.x;
			if (ballPos.y - ballSize/2 < 0)
				ballDir.y = -ballDir.y;
			
			if (ballPos.y + ballSize/2 > w.height) {
				// game over
				gameOver = true;
			}

		    bool cleared = rectanglesMap.destroyRectangle(lastBallPos, ballPos, &ballDir, ballSize);
			if(cleared) {  // all rectangles are destroyed
					       // the player will start another round of game
				nextGame = true;
			}

			XFlush(display);
			
			lastRepaint = now(); // remember when the paint happened

			lastBallPos = ballPos; // remember the last position of the ball
		}

		// IMPORTANT: sleep for a bit to let other processes work
		if (XPending(display) == 0) {
			usleep(1000000 / FPS - (now() - lastRepaint));
		}
	}

	XCloseDisplay(display);
}
