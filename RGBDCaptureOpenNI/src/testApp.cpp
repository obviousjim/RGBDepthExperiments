#include "testApp.h"
#include "ofxXmlSettings.h"
#include "ofxDepthImageProviderFreenect.h"

//--------------------------------------------------------------

void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetFrameRate(60);
	ofToggleFullscreen();
	
	//ofBackground(255*.2);
	ofBackground(255*0);
	
//	cameraFound =  recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
//	cameraFound &= recordDepth.setup(&recordContext);
//	cameraFound &= recordImage.setup(&recordContext);
	if(!cameraFound){
		ofLogError() << "RECORDER --- Camera not found";
	}
	
	fullscreenPoints = false;
//	inCaptureMode = false;
//	clicks = 0;
//	lastClickTime = 0;
	
	currentTab = TabCalibrate;

	downColor = ofColor(255, 120, 0);
	idleColor = ofColor(220, 200, 200);
	hoverColor = ofColor(255*.2, 255*.2, 30*.2);
	
	//setup buttons
	framewidth = 640;
	frameheight = 480;
	thirdWidth = framewidth/3;
	btnheight = 30;
	
	btnSetDirectory = new ofxMSAInteractiveObjectWithDelegate();
	btnSetDirectory->setPosAndSize(0, 0, framewidth, btnheight);
	btnSetDirectory->setLabel("Load Directory");
	btnSetDirectory->setIdleColor(idleColor);
	btnSetDirectory->setDownColor(downColor);
	btnSetDirectory->setHoverColor(hoverColor);
	btnSetDirectory->setDelegate(this);
	
	btnCalibrateTab = new ofxMSAInteractiveObjectWithDelegate();
	btnCalibrateTab->setPosAndSize(0, btnheight, thirdWidth, btnheight);
	btnCalibrateTab->setLabel("Calibrate");
	btnCalibrateTab->setIdleColor(idleColor);
	btnCalibrateTab->setDownColor(downColor);
	btnCalibrateTab->setHoverColor(hoverColor);
	btnCalibrateTab->setDelegate(this);
	
	btnRecordTab = new ofxMSAInteractiveObjectWithDelegate();
	btnRecordTab->setPosAndSize(thirdWidth, btnheight, thirdWidth, btnheight);
	btnRecordTab->setLabel("Record");
	btnRecordTab->setIdleColor(idleColor);
	btnRecordTab->setDownColor(downColor);
	btnRecordTab->setHoverColor(hoverColor);
	btnRecordTab->setDelegate(this);
	
	btnPlaybackTab = new ofxMSAInteractiveObjectWithDelegate();
	btnPlaybackTab->setPosAndSize(thirdWidth*2, btnheight, thirdWidth, btnheight);
	btnPlaybackTab->setLabel("Playback");
	btnPlaybackTab->setIdleColor(idleColor);
	btnPlaybackTab->setDownColor(downColor);
	btnPlaybackTab->setHoverColor(hoverColor);
	btnPlaybackTab->setDelegate(this);
	
	btnRenderBW = new ofxMSAInteractiveObjectWithDelegate();
	btnRenderBW->setPosAndSize(0, btnheight*2+frameheight, thirdWidth, btnheight);
	btnRenderBW->setLabel("Blaick&White");
	btnRenderBW->setIdleColor(idleColor);
	btnRenderBW->setDownColor(downColor);
	btnRenderBW->setHoverColor(hoverColor);
	btnRenderBW->setDelegate(this);
	
	btnRenderRainbow = new ofxMSAInteractiveObjectWithDelegate();
	btnRenderRainbow->setPosAndSize(thirdWidth, btnheight*2+frameheight, thirdWidth, btnheight);
	btnRenderRainbow->setLabel("Rainbow");
	btnRenderRainbow->setIdleColor(idleColor);
	btnRenderRainbow->setDownColor(downColor);
	btnRenderRainbow->setHoverColor(hoverColor);
	btnRenderRainbow->setDelegate(this);
	
	btnRenderPointCloud = new ofxMSAInteractiveObjectWithDelegate();
	btnRenderPointCloud->setPosAndSize(thirdWidth*2, btnheight*2+frameheight, thirdWidth, btnheight);
	btnRenderPointCloud->setLabel("Pointcloud");
	btnRenderPointCloud->setIdleColor(idleColor);
	btnRenderPointCloud->setDownColor(downColor);
	btnRenderPointCloud->setHoverColor(hoverColor);
	btnRenderPointCloud->setDelegate(this);
	
	btnRecordBtn = new ofxMSAInteractiveObjectWithDelegate();
	btnRecordBtn->setPosAndSize(0, btnheight*3+frameheight, framewidth, btnheight);
	btnRecordBtn->setLabel("Record");
	btnRecordBtn->setIdleColor(idleColor);
	btnRecordBtn->setDownColor(downColor);
	btnRecordBtn->setHoverColor(hoverColor);
	btnRecordBtn->setDelegate(this);

	/*
	btnCaptureMode = new ofxMSAInteractiveObjectWithDelegate();
	btnCaptureMode->setPosAndSize(framewidth+thirdWidth*3, 0, framewidth, btnheight*2);
	btnCaptureMode->setLabel("!! CAPTURE MODE !!");
	btnCaptureMode->setIdleColor(idleColor);
	btnCaptureMode->setDownColor(downColor);
	btnCaptureMode->setHoverColor(hoverColor);
	btnCaptureMode->setDelegate(this);
	*/
	
	calibrationPreview.setup(10, 7, 2.5);
	alignment.setup(10, 7, 2.5);
	alignment.setupGui(0, btnheight*4+frameheight, ofGetWidth());
	
	timeline.setup();
	timeline.setOffset(ofVec2f(0,btnRecordBtn->y+btnRecordBtn->height));
	timeline.addElement("depth sequence", &depthSequence);
	timeline.setWidth(ofGetWidth());
	timeline.setLoopType(OF_LOOP_NORMAL);
	
	depthSequence.setup();
	
	ofxXmlSettings defaults;
	if(defaults.loadFile("defaults.xml")){
		loadDirectory(defaults.getValue("currentDir", ""));
	}
	else{
		loadDirectory("depthframes");
	}
	
	updateTakeButtons();
	
	cam.loadCameraPosition();
	
	cam.speed = 25;
	cam.setFarClip(50000);
	recorder.setup();	
		
	flip = false;
	
	//depthImageProvider = new ofxDepthImageProviderOpenNI();
	depthImageProvider = new ofxDepthImageProviderFreenect();
	depthImageProvider->setup();
}


//--------------------------------------------------------------

void testApp::update(){
	if(!cameraFound){
		return;
	}
	
	//JG conv -- recordContext.update();
	depthImageProvider->update();
	if(depthImageProvider->isFrameNew()){
		
		if(currentTab == TabCalibrate){
			//JG conv -- recordImage.update();
			//JG conv -- calibrationImage.setFromPixels(recordImage.getIRPixels(), 640, 480, OF_IMAGE_GRAYSCALE);
			calibrationPreview.setTestImage( depthImageProvider->getRawIRImage() );
		}
		
		if(recorder.isRecording()){
			//JG conv -- recorder.addImage( (unsigned short*)recordDepth.getRawDepthPixels());
			recorder.addImage(depthImageProvider->getRawDepth());
		}
	}
}

void testApp::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y){}
void testApp::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y){}
void testApp::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button){}
void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){}
void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){
	
	if(object == btnSetDirectory){
		loadDirectory();
	}
	else if(object == btnCalibrateTab){
		currentTab = TabCalibrate; 
	}
	else if(object == btnRecordTab){
		currentTab = TabRecord;
	}
	else if(object == btnPlaybackTab){
		currentTab = TabPlayback;
	}
	else if(object == btnRecordBtn){
		if(currentTab == TabRecord){
			toggleRecord();
		}
		else if(currentTab == TabCalibrate){
			captureCalibrationImage();
		}
		else if(currentTab == TabPlayback){
			//no need to do anything atm
		}
	}
	else if(object == btnRenderBW){
		currentRenderMode = RenderBW;
	}
	else if(object == btnRenderRainbow){
		currentRenderMode = RenderRainbow;
	}
	else if(object == btnRenderPointCloud){
		currentRenderMode = RenderPointCloud;
	}
//	else if(object == btnCaptureMode){
//		inCaptureMode = true;
//		timeline.disable();
//	}
	else {
		for(int i = 0; i < btnTakes.size(); i++){
			if(object == btnTakes[i]){
				loadSequenceForPlayback( i );
			}
		}
	}
}


void testApp::loadDirectory(){
	ofFileDialogResult r = ofSystemLoadDialog("Select Record Directory", true);
	if(r.bSuccess){
		loadDirectory(r.getPath());
	}
}

void testApp::loadDirectory(string path){
	workingDirectory = path;
	recorder.setRecordLocation(path+"/takes", "frame");
	ofDirectory dir(workingDirectory+"/calibration");
	if(!dir.exists()){
		dir.create(true);
	}
	alignment.loadState(workingDirectory+"/calibration/alignmentsave.xml");
	
	btnSetDirectory->setLabel(path);
	updateTakeButtons();
	ofxXmlSettings defaults;
	defaults.loadFile("defaults.xml");
	defaults.setValue("currentDir", path);
	defaults.saveFile("defaults.xml");
}

void testApp::loadSequenceForPlayback( int index ){
	vector<string> paths = recorder.getTakePaths();
	//TODO add timeline stuff in here;
	depthSequence.loadSequence(paths[index]);
	timeline.setDurationInFrames(depthSequence.videoThumbs.size());
}

void testApp::toggleRecord(){
	recorder.toggleRecord();
	updateTakeButtons();
}

//--------------------------------------------------------------
void testApp::captureCalibrationImage(){
//	if(recordImage.getIRPixels() != NULL){
//	calibrationImage.setFromPixels(recordImage.getIRPixels(), 640, 480, OF_IMAGE_GRAYSCALE);
	char filename[1024];
	sprintf(filename, "%s/calibration/calibration_image_%02d_%02d_%02d_%02d_%02d.png", workingDirectory.c_str(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds());
	//jg conv ofSaveImage( calibrationImage, filename);
	ofSaveImage( depthImageProvider->getRawIRImage(), filename);
	alignment.addDepthCalibrationImage(filename);
	alignment.generateAlignment();
	alignment.saveState();
	
//	}
}

//--------------------------------------------------------------
void testApp::updateTakeButtons(){
	vector<string> files = recorder.getTakePaths();
	
	for(int i = 0; i < btnTakes.size(); i++){
		delete btnTakes[i];
	}
	btnTakes.clear();
	
	for(int i = 0; i < files.size(); i++){
		ofxMSAInteractiveObjectWithDelegate* btnTake = new ofxMSAInteractiveObjectWithDelegate();		
		float x = framewidth;
		float y = btnheight/2*i;
//		cout << "y is " << btnheight/2*i << " limit is " << btnheight*3+frameheight << endl;
		while(y >= btnheight*3+frameheight){
			y -= btnheight*3+frameheight;
			x += thirdWidth;
		}
		
		btnTake->setPosAndSize(x, y, thirdWidth, btnheight/2);
		btnTake->setLabel( ofFilePath::getFileName(files[i]) );
		btnTake->setIdleColor(idleColor);
		btnTake->setDownColor(downColor);
		btnTake->setHoverColor(hoverColor);
		btnTake->setDelegate(this);
		btnTake->enableAllEvents();
		
		btnTakes.push_back( btnTake );
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	if(fullscreenPoints && currentTab == TabPlayback){
		drawPointcloud(true);
		return;
	}

	if(currentTab == TabCalibrate){
		//recordImage.draw(0, btnheight*2, 640, 480);
		depthImageProvider->getRawIRImage().draw(0, btnheight*2, 640, 480);
		calibrationPreview.draw(0, btnheight*2);
		alignment.drawDepthImages();
	}
	else if(currentTab == TabRecord){
		//TODO render modes			
		ofPushStyle();
		ofSetColor(255, 255, 255, 60);
		ofLine(320, btnheight*2, 320, btnheight*2+480);
		ofLine(0, btnheight*2+240, 640, btnheight*2+240);
		ofPopStyle();
		depthImageProvider->getDepthImage().draw(0, btnheight*2, 640, 480);
	}
	else {
		if(currentRenderMode == RenderBW || currentRenderMode == RenderRainbow){
			depthSequence.currentDepthImage.draw(0, btnheight*2, 640, 480);
		}
		else {
			if(depthSequence.currentDepthRaw != NULL){
				drawPointcloud(false);
			}
		}
		//draw timeline
		timeline.draw();
	}

	//draw save meter if buffer is getting full
	if(recorder.isRecording()){
		ofPushStyle();
		ofSetColor(255, 0, 0);
		ofNoFill();
		ofSetLineWidth(5);
		
		ofRect(0, btnheight*2, 640, 480);
		ofPopStyle();
	}
	
	if(recorder.numFramesWaitingSave() > 0){
		ofPushStyle();
		float width = recorder.numFramesWaitingSave()/2000.0 * btnRecordBtn->width;
		ofFill();
		ofSetColor(255,0, 0);
		ofRect(btnRecordBtn->x,btnRecordBtn->y,width,btnRecordBtn->height);
		
		if(ofGetFrameNum() % 30 < 15){
			ofSetColor(255, 0, 0, 40);
			ofRect(*btnRecordBtn);
		}
		ofPopStyle();
	}
//	
//	if(inCaptureMode){
//		ofSetColor(255, 0, 0, 15);
//		ofRect(0, 0, ofGetWidth(), ofGetHeight());
//	}		
}

void testApp::drawPointcloud(bool fullscreen){
	glEnable(GL_DEPTH_TEST);
	ofRectangle rect = fullscreen ? ofRectangle(0,0, ofGetWidth(), ofGetHeight()) : ofRectangle(0, btnheight*2, 640, 480);
	cam.begin(rect);
	glBegin(GL_POINTS);	
	glPointSize(4);
	for(int y = 0; y < 480; y++){
		for(int x = 0; x < 640; x++){
			//0.104200 ref dist 120.000000
			double ref_pix_size = 0.104200;
			double ref_distance = 120.000000;
			double wz = depthSequence.currentDepthRaw[y*640+x];
			double factor = 2 * ref_pix_size * wz / ref_distance;
			double wx = (double)(x - 640/2) * factor;
			double wy = (double)(y - 480/2) * factor;
			glColor3f(1.0,1.0,1.0);
			glVertex3f(wx,-wy,-wz);
		}
	}
	glEnd();
	cam.end();
	glDisable(GL_DEPTH_TEST);	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if(key == ' '){
		if(currentTab == TabCalibrate){
			captureCalibrationImage();
		}
		else if(currentTab == TabRecord){
			toggleRecord();
		}
		else if(currentTab == TabPlayback){
			timeline.togglePlay();
		}
	}
    	
	if(key == OF_KEY_DEL && currentTab == TabCalibrate){
		alignment.discardCurrentPair();
		alignment.saveState();
	}

	if(key == 'f'){
		ofToggleFullscreen();
	}
	
	if(key == 'p'){
		fullscreenPoints = !fullscreenPoints;
		if(fullscreenPoints){
			btnSetDirectory->disableAllEvents();
		
			btnCalibrateTab->disableAllEvents();
			btnRecordTab->disableAllEvents();
			btnPlaybackTab->disableAllEvents();
			
			btnRecordBtn->disableAllEvents();
			
			btnRenderBW->disableAllEvents();
			btnRenderRainbow->disableAllEvents();
			btnRenderPointCloud->disableAllEvents();
			
//			btnCaptureMode->disableAllEvents();
		
			for(int i = 0; i < btnTakes.size(); i++) btnTakes[i]->disableAllEvents();
			ofHideCursor();
		}
		else{
			btnSetDirectory->enableAllEvents();
			
			btnCalibrateTab->enableAllEvents();
			btnRecordTab->enableAllEvents();
			btnPlaybackTab->enableAllEvents();
			
			btnRecordBtn->enableAllEvents();
			
			btnRenderBW->enableAllEvents();
			btnRenderRainbow->enableAllEvents();
			btnRenderPointCloud->enableAllEvents();
			
//			btnCaptureMode->enableAllEvents();
			
			for(int i = 0; i < btnTakes.size(); i++) btnTakes[i]->enableAllEvents();
			ofShowCursor();
			
		}
	}
}

void testApp::exit() {
	depthImageProvider->close();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	timeline.setWidth(w);
	alignment.setMaxDrawWidth(w);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}