#pragma once

#include "ParticleSystem.h"
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	int loadTexture_Ipl;
	
		//properties
		// mouse & keyboard
	int		mouseX, mouseY, mousePressedX, mousePressedY;
	bool	mouse[5];
	bool	keys[256];
	bool	KeyIsDown(const int key);
	
		//particle system stuff
	float timeStep;
	int lineOpacity, pointOpacity;
	float particleNeighborhood, particleRepulsion;
	float centerAttraction;
	
	int kParticles;
	ParticleSystem particleSystem;
	bool isMousePressed, slowMotion;
	
		//camera & video
	ofVideoGrabber      vidGrabber;
	int 				camWidth, captureWidth;
	int 				camHeight, captureHeight;
	int                 numCamPixels, numCapturePixels;
	int                 numThreads;
		//	ofxCvColorImage     videoImage;
		//	ofxCvGrayscaleImage bgImage;
		//	ofxCvGrayscaleImage grayImage;
		//	ofxCvGrayscaleImage diffImage;
		//	ofxCvContourFinder  contours;
    ofPoint             topLeft, topRight, bottomRight, bottomLeft;
	unsigned char *		videoFrame;
	unsigned char * 	videoBackground;
	unsigned char *		videoDiff;
	unsigned char *		videoSnapshot;
	
	unsigned char * 	grayTextureLUM_C;
	unsigned char *		grayTextureRGB_C;
	
	ofTexture		 	grayTextureLUM;
	ofTexture			grayTextureRGB;
	
	int					diffThreshold;
	ofTexture			videoTexture;
	ofTexture           captureTexture;
	
	ofImage				vidImageRGB;
	ofImage				vidImageBW;
	ofxKinect kinect;
	int 				totalKinect;
	
	ofxCvColorImage		colorImage;
	
	ofxCvGrayscaleImage 	grayImage;
	ofxCvGrayscaleImage 	grayThresh;
	ofxCvGrayscaleImage 	grayThreshFar;
	
	
	ofxCvGrayscaleImage 	grayBackground;
	ofxCvGrayscaleImage 	grayDifference;
	
	int 				nearThreshold;
	int					farThreshold;
	
	int					angle;
	
	int 				pointCloudRotationY;
	
	ofxCvContourFinder 	contourFinder;
	
	
	bool				bThreshWithOpenCV;
	bool				drawPC;


	
	float widthRatio;
	float heightRatio;
    float   boredom;
};