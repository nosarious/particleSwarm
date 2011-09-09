#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
			ofSetupOpenGL(&window, 4080, 768, OF_FULLSCREEN);
		//ofSetupOpenGL(&window, 1280, 800, OF_FULLSCREEN);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glBlendEquation(GL_FUNC_SUBTRACT_EXT);
	glEnable (GL_LINE_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
		// this kicks off the running of my app
		// can be OF_WINDOW or OF_FULLSCREEN
		// pass in width and height too:
	
	ofRunApp(new testApp());
}