#include "testApp.h"


#define D_ENABLE_MOTION_CAPTURE

void getQuadSubImage(   unsigned char * inputData, unsigned char * outputData,
					 int inW, int inH, int outW, int outH,
					 int x1, int y1, int x2, int y2,
					 int x3, int y3, int x4, int y4, int bpp) {
    for(int x=0;x<outW;x++) {
        for(int y=0;y<outH;y++) {
            float xlrp = x/(float)outW;
            float ylrp = y/(float)outH;
            int xinput = (x1*(1-xlrp)+x2*xlrp)*(1-ylrp) + (x4*(1-xlrp)+x3*xlrp)*ylrp;
            int yinput = (y1*(1-ylrp)+y4*ylrp)*(1-xlrp) + (y2*(1-ylrp)+y3*ylrp)*xlrp;
            int inIndex = (xinput + yinput*inW)*bpp;
            int outIndex = (x+y*outW)*bpp;
            memcpy((void*)(outputData+outIndex),(void*)(inputData+inIndex),sizeof(unsigned char)*bpp);
        }
    }
}

void testApp::setup(){
        // this number describes how many bins are used
        // on my machine, 2 is the ideal number (2^2 = 4x4 pixel bins)
        // if this number is too high, binning is not effective
        // because the screen is not subdivided enough. if
        // it's too low, the bins take up so much memory as to
        // become inefficient.
	int binPower =5;
	
	particleSystem.setup(ofGetWidth(), ofGetHeight(), binPower, 0, 0, 0);
	
	kParticles =ofGetWidth()/128;
	float padding = 10;
	float maxVelocity = 0.05;
	for(int i = 0; i < kParticles * 1024; i++) {
		
		float x = ofRandom(padding, ofGetWidth() - padding);
		float y = ofRandom(padding, ofGetHeight() - padding);
		float xv = ofRandom(-maxVelocity, maxVelocity);
		float yv = ofRandom(-maxVelocity, maxVelocity);
		int cBr = ofRandom(255);
		int cBg = ofRandom(255);
		int cBb = ofRandom(255);
		Particle particle(x, y, xv, yv, cBr, cBg, cBb);
		particleSystem.add(particle);
	}
	
	timeStep = 0.25;
	lineOpacity = 40;
	pointOpacity = 200; //was 255
	isMousePressed = false;
	slowMotion = false;
	particleNeighborhood = 14;
	particleRepulsion = 1.25;
	centerAttraction = .015; //was 0.1
	
	for( int i=0;i<5;i++) {
		mouse[i] = false;
	}
	for(int i=0;i<256;i++){
		keys[i]=false;
	}
	/*
#ifdef D_ENABLE_MOTION_CAPTURE
    captureWidth = 320;
    captureHeight = 240;
	camWidth = 160;
	camHeight = 60;
	numCamPixels = camWidth * camHeight;
	numCapturePixels = captureWidth * captureHeight * 3;
	
		//	videoImage.allocate(camWidth, camHeight);
		//	grayImage.allocate(camWidth, camHeight);
		//	diffImage.allocate(camWidth, camHeight);
		//	bgImage.allocate(camWidth, camHeight);
	diffThreshold = 80;
	videoFrame = new unsigned char[camWidth*camHeight*3];
	videoBackground = new unsigned char[camWidth*camHeight*3];
	videoDiff = new unsigned char[camWidth*camHeight];
	
	videoTexture.allocate(camWidth,camHeight, GL_RGB);
	captureTexture.allocate(captureWidth, captureHeight, GL_RGB);
	
		//    printf("initializing video grabber...");
	vidGrabber.setDeviceID(4);
	vidGrabber.setUseTexture(false);
	vidGrabber.initGrabber(captureWidth,captureHeight,false);
	sleep(1);
	vidGrabber.grabFrame();
		//    videoImage.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight);
		//    grayImage = videoImage;
		//    bgImage = grayImage;
#endif
	*/
	
	
		// start of kinect stuff
	
	
    captureWidth = kinect.width;
    captureHeight = kinect.height;
	camWidth = kinect.width/2;
	camHeight = kinect.height/2;
	numCamPixels = camWidth * camHeight;
	numCapturePixels = captureWidth * captureHeight * 3;
	totalKinect = kinect.width*kinect.height;
	
	diffThreshold = 80;
	videoFrame = new unsigned char[kinect.width*kinect.height];
	videoBackground = new unsigned char[kinect.width*kinect.height];
	videoDiff = new unsigned char[kinect.width*kinect.height];
	
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();
	
	colorImage.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThresh.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 150;
	farThreshold  = 0;
	
	ofSetFrameRate(150);
	
		// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	videoSnapshot = new unsigned char[totalKinect];
	videoFrame = new unsigned char[totalKinect];
	
	vidImageBW.allocate(captureWidth, captureHeight,OF_IMAGE_GRAYSCALE);
	videoTexture.allocate(640,480,GL_LUMINANCE);
	captureTexture.allocate(640,480,GL_RGB);
	
	
	topLeft.x=0;
	topLeft.y=0;
	topRight.x=captureWidth;
	topRight.y=0;
	bottomRight.x=captureWidth;
	bottomRight.y=captureHeight;
	bottomLeft.x=0;
	bottomLeft.y=captureHeight;
	
	
	ofSetBackgroundAuto(false);
    ofBackground(0,0,0);
    ofEnableAlphaBlending();
}

void testApp::update(){
	
	
	
		//	ofBackground(0,0,0);
	
	kinect.update();
	if(kinect.isFrameNew())	// there is a new frame and we are connected
		{
		
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		
		colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);

			//  grayImage.mirror(false, true); // horiz / vert
        
			//      grayImage.blur(5);       // so its always odd
			//we do two thresholds - one for the far plane and one for the near plane
			//we then do a cvAnd to get the pixels which are a union of the two thresholds.	
		
			grayThreshFar = grayImage;
			grayThresh = grayImage;
			grayThresh.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThresh.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		
		
			//	grayImage.threshold(20, false);
			//	grayImage.contrastStretch();
		videoSnapshot = grayImage.getPixels();
			//	videoFrame = videoSnapshot;
			//	videoDiff = videoSnapshot;
		getQuadSubImage(videoSnapshot,videoFrame,captureWidth, captureHeight,camWidth, camHeight,
						topLeft.x, topLeft.y,
						topRight.x, topRight.y,
						bottomRight.x, bottomRight.y,
						bottomLeft.x, bottomLeft.y,1); 		
		
		
		if (KeyIsDown('b'))  {
			memcpy((void*)videoBackground,(void*)videoFrame,sizeof(unsigned char)*numCamPixels);
        }
		
		
		for(int i=0;i<numCamPixels;i++){
            int j = i;
            int d = videoFrame[i]-videoBackground[i];
					videoDiff[i] = ABS(d);
				// if (ABS(d)>0) {videoDiff[i] = 255;}
        }
		
		
		
	}
		grayImage.flagImageChanged();
	
}

void testApp::draw(){
	
	
	
	glEnable(GL_BLEND);
	
	ofSetColor(0, 0, 0,75);
	ofFill();
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
	ofNoFill();
	
#ifdef D_ENABLE_MOTION_CAPTURE
	
		//Video keystone setup
	if (KeyIsDown('v') || mouse[1]) {
        ofShowCursor();
		glColor4f(1,1,1,1);
		glDisable(GL_BLEND);
			//diffImage.draw(0,0,ofGetWidth(),ofGetHeight());
		if(mouse[0])
			{
		    captureTexture.loadData(colorImage.getPixels(),captureWidth,captureHeight, GL_RGB);
            captureTexture.draw(0,0,ofGetWidth(),ofGetHeight());
			}
        else {
            videoTexture.loadData(videoFrame,camWidth,camHeight, GL_LUMINANCE);
            videoTexture.draw(0,0,ofGetWidth(),ofGetHeight());
        }
        ofNoFill();
        ofSetColor(255,0,0);
		
        ofCircle(topLeft.x*ofGetWidth()/captureWidth,topLeft.y*ofGetHeight()/captureHeight,25);
        ofCircle(topRight.x*ofGetWidth()/captureWidth,topRight.y*ofGetHeight()/captureHeight,25);
        ofCircle(bottomRight.x*ofGetWidth()/captureWidth,bottomRight.y*ofGetHeight()/captureHeight,25);
        ofCircle(bottomLeft.x*ofGetWidth()/captureWidth,bottomLeft.y*ofGetHeight()/captureHeight,25);
		
		if(mouse[0]) {
            ofSetColor(255,255,255);
            ofCircle(0,0,1);
            ofCircle(0,ofGetHeight(),2);
            ofCircle(ofGetWidth(),ofGetHeight(),5);
            ofCircle(ofGetWidth(),0,5);
            ofPoint mousePointer = ofPoint(mouseX*captureWidth/ofGetWidth(), mouseY*captureHeight/ofGetHeight());
			
            if(mousePointer.distance(topLeft) < 25) topLeft = mousePointer;
            if(mousePointer.distance(topRight) < 25) topRight = mousePointer;
            if(mousePointer.distance(bottomRight) < 25) bottomRight = mousePointer;
            if(mousePointer.distance(bottomLeft) < 25) bottomLeft = mousePointer;
			
            ofSetColor(0,255,0);
            ofCircle(topLeft.x*ofGetWidth()/captureWidth,topLeft.y*ofGetHeight()/captureHeight,2);
            ofCircle(topRight.x*ofGetWidth()/captureWidth,topRight.y*ofGetHeight()/captureHeight,2);
            ofCircle(bottomRight.x*ofGetWidth()/captureWidth,bottomRight.y*ofGetHeight()/captureHeight,2);
            ofCircle(bottomLeft.x*ofGetWidth()/captureWidth,bottomLeft.y*ofGetHeight()/captureHeight,2);
        }
        return;
	}
#endif
	
	
		//	ofHideCursor();
	glEnable(GL_BLEND);
		glLineWidth(3);
	particleSystem.setTimeStep(timeStep);
	
	ofEnableAlphaBlending();
	
	particleSystem.setupForces();
        // apply per-particle forces
	glBegin(GL_LINES);
	for(int i = 0; i < particleSystem.size(); i++) {
		Particle& cur = particleSystem[i];
			// global force on other particles
		particleSystem.addRepulsionForce(cur, particleNeighborhood, particleRepulsion);
			// forces on this particle
		cur.bounceOffWalls(0, 0, ofGetWidth(), ofGetHeight());
		cur.addDampingForce();
	}
	glEnd();
        // single global forces
		//	particleSystem.addAttractionForce(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth(), centerAttraction);
	if(isMousePressed)
		particleSystem.addRepulsionForce(mouseX, mouseY, 100, 50);
		//	particleSystem.update();
		    //repel from silouette
		//#ifdef D_ENABLE_MOTION_CAPTURE
	
	
	float widthRatio = ofGetWidth()/camWidth;
	float heightRatio = ofGetHeight()/camHeight;
	
	for (int i = 0; i < camWidth; i++){  
		for (int j = 0; j < camHeight; j++){ 
			int ii = i;
			int jj = j;
			if (videoDiff[jj*camWidth+ii]>diffThreshold){
					//	ofCircle(i*widthRatio, j*heightRatio, 3);
						particleSystem.addRepulsionForce(ii*widthRatio*1.07, jj*heightRatio*1.55, 40, 10);
			} // end if white check
		}  
	}
	particleSystem.update();
		//#endif
	ofSetColor(255, 255, 255);
	glLineWidth(2.0);
	ofDisableAlphaBlending();
			ofDrawBitmapString(ofToString(kParticles) + "k particles", 32, 32);
			ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 32, 52);
	/*
	
	vidImageBW.setFromPixels(videoFrame,camWidth,camHeight,OF_IMAGE_GRAYSCALE);
	vidImageBW.draw(0,0,camWidth,camHeight);
	
	vidImageBW.setFromPixels(videoBackground,camWidth,camHeight,OF_IMAGE_GRAYSCALE);
	vidImageBW.draw(0,camHeight+10,camWidth,camHeight);
	
	vidImageBW.setFromPixels(videoDiff,camWidth,camHeight,OF_IMAGE_GRAYSCALE);
	vidImageBW.draw(0,camHeight*2+20,camWidth,camHeight);
	 */
}



	//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key<256 && key>0) keys[key] = true;
	
	switch(key) {
		case '-':
				//	pDist = MAX( pDist-1, 1);
			break;
		case '=':
				//	pDist++;
			break;
		case 't':
			diffThreshold = MAX(diffThreshold - 1, 1);
			break;
		case 'T':
			diffThreshold++;
			break;
		case 'f':
				//	isFullScreen = !isFullScreen;
				//	ofSetFullscreen(isFullScreen);
			break;
			
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
		case '<':		
		case ',':		
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
		default:
			break;
	}
}

	//--------------------------------------------------------------
void testApp::keyReleased  (int key){
	if (key<256 && key>0) keys[key] = false;
	switch(key) {
		case ' ':
				//	for(int i=0;i<numPars;i++)
				//		cloud[i].vel *=0;
			break;
	}
}

bool testApp::KeyIsDown(const int key) {
	if (key<256 && key>0) return keys[key];
	return false;
}
	//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	mouseX = x;
	mouseY = y;
}

	//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	mouseX = x;
	mouseY = y;
}

	//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	mouseX = x;
	mouseY = y;
	mousePressedX = x;
	mousePressedY = y;
	mouse[button] = true;
	isMousePressed = true;
}


	//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	mouseX = x;
	mouseY = y;
	mousePressedX = x;
	mousePressedY = y;
	mouse[button] = false;
	isMousePressed = false;
}

	//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	int i = 0;
	
}

	//The IplImage to OpenGl texture function
int loadTexture_Ipl(IplImage *image, GLuint *text) {
	
    if (image==NULL) return -1;
	
    glGenTextures(1, text);
	
	glBindTexture( GL_TEXTURE_2D, *text ); //bind the texture to it's array
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height,0, GL_BGR, GL_UNSIGNED_BYTE, image->imageData);
    return 0;
	
} 

/*
void testApp::keyPressed(int key){
	if(key == 'p')
		ofSaveScreen(ofToString(ofGetMinutes()) + "_" + ofToString(ofGetFrameNum()) + ".png");
	if(key == 's') {
		slowMotion = !slowMotion;
		if(slowMotion)
			timeStep = .05;
		else
			timeStep = 1;
	}
}

void testApp::mousePressed(int x, int y, int button){
	isMousePressed = true;
}

void testApp::mouseReleased(int x, int y, int button){
	isMousePressed = false;
}


bool testApp::KeyIsDown(const int key) {
	if (key<256 && key>0) return keys[key];
	return false;
}
	//--------------------------------------------------------------
void testApp::keyReleased  (int key){
	if (key<256 && key>0) keys[key] = false;
	switch(key) {
		case ' ':
				//	for(int i=0;i<numPars;i++)
				//		cloud[i].vel *=0;
			break;
	}
}


	//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	int i = 0;
	
}*/