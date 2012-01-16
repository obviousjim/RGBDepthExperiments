#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	

	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofBackground(0);

	doUndistort = false;
	saveCurrentFrame = false;
	onRenderMode = false;
	
	savingImage.setUseTexture(false);
	savingImage.allocate(1920,1080, OF_IMAGE_COLOR);
	

	
	fbo.allocate(1920, 1080, GL_RGB, 4);
	
	if(projectsettings.loadFile("projectsettings.xml")){
		if(!loadAlignmentMatrices(projectsettings.getValue("alignmentDir", ""))){
			loadNewProject();
		}
		
		if(!loadVideoFile(projectsettings.getValue("videoFile", ""))){
			loadNewProject();
		}
		
		if(!loadDepthSequence(projectsettings.getValue("depthSequence", ""))){
			loadNewProject();
		}
		allLoaded = true;
	}
	else {
		loadNewProject();
	}
	
	//renderer.setRGBTexture(player);
	renderer.setRGBTexture(undistortedImage);
	
	cam.speed = 40;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.loadCameraPosition();
	cam.setFarClip(30000);
	
	depthPixelDecodeBuffer = depthSequence.currentDepthRaw;
	//depthPixelDecodeBuffer = new unsigned short[640*480];
	
	string videoThumbsPath = ofFilePath::removeExt(videoPath);
	if(!ofDirectory(videoThumbsPath).exists()){
		ofDirectory(videoThumbsPath).create(true);
	}

	timeline.setup();
	videoTimelineElement.setup();
	
	timeline.setDurationInFrames(player.getTotalNumFrames());
	timeline.addElement("Video", &videoTimelineElement);
	
	videoTimelineElement.toggleThumbs();
	videoTimelineElement.setVideoPlayer(player, videoThumbsPath);
	
	gui.addSlider("X Linear Shift", renderer.xshift, -20, 20);
	gui.addSlider("Y Linear Shift", renderer.yshift, -150, 15);
	gui.addToggle("Use Distorted bits", renderer.useDistorted);
	gui.addToggle("Start Render", onRenderMode);
	
	gui.loadFromXML();
	gui.toggleDraw();
	
	cam.setScale(1, -1, 1);
	

	
//	player.play();

}

bool testApp::loadNewProject(){
	allLoaded = false;

	//Alignment matrices
	ofFileDialogResult r;
	ofSystemAlertDialog("Select Alignment Directory");
	r = ofSystemLoadDialog("Alignment Directory", true);
	if(!r.bSuccess || !loadAlignmentMatrices(r.getPath())){
		return false;
	}
	
	//Video file
	ofSystemAlertDialog("Select Video File");
	r = ofSystemLoadDialog("Video File", false);
	if(!r.bSuccess || !loadVideoFile(r.getPath())){
		return false;
	}
	
	//depth sequence
	ofSystemAlertDialog("Select Depth Directory");
	r = ofSystemLoadDialog("Depth Directory", true);
	if(!r.bSuccess || !loadDepthSequence(r.getPath())){
		return false;
	}
	
	allLoaded = true;
}

bool testApp::loadDepthSequence(string path){
	projectsettings.setValue("depthSequence", path);
	projectsettings.saveFile();
	
	depthSequence.setup();
	depthSequence.disable();
	return depthSequence.loadSequence(path);
}

bool testApp::loadVideoFile(string path){
	projectsettings.setValue("videoFile", path);
	projectsettings.saveFile();

	videoPath = path;
	//qtRenderer.loadMovie(videoPath, OFXQTVIDEOPLAYER_MODE_TEXTURE_ONLY);
	player.loadMovie(videoPath);
	saveFolder = "saveout_" + ofFilePath::getBaseName(path);
	//seed();
	uniqueRand = ofGetMonth()+ofGetDay()+ofGetMinutes()+ofGetSeconds();
	ofDirectory savdir(saveFolder);
	if(!savdir.exists()){
		savdir.create(true);
	}
	//qtRenderer.loadMovie(videoPath);
	return sequencer.loadPairingFile(ofFilePath::removeExt(path) + "_pairings.xml");
}

bool testApp::loadAlignmentMatrices(string path){
	projectsettings.setValue("alignmentDir", path);
	projectsettings.saveFile();
	
	return renderer.setup(path);
}

//--------------------------------------------------------------
void testApp::update(){
	if(!allLoaded) return;
	
	if(onRenderMode){
		if(player.isPlaying())
			player.stop();
		
		//player.setFrame(player.getCurrentFrame()+1);
		videoTimelineElement.selectFrame(videoTimelineElement.getSelectedFrame()+1);

		cout << "current frame is " << player.getCurrentFrame() << endl;
	}
	
	player.update();
	
	if(player.isFrameNew() || onRenderMode){
		saveCurrentFrame = onRenderMode;
		
		//cout << "new frame?" << endl;
	//qtRenderer.update();
	//if(qtRenderer.isFrameNew()){
//		if(doUndistort){
//			renderer.getDepthCalibration().undistort(toCv(player), toCv(undistortedImage));
//			undistortedImage.update();
//		}
//		else{
//			undistortedImage.setFromPixels(player.getPixelsRef());
//		}
		
//		cout << "current frame number " << player.getCurrentFrame() << endl;
		long depthFrame;
		if(sequencer.isSequenceTimebased()){
			long movieMillis = player.getPosition() * player.getDuration()*1000;
			depthFrame = sequencer.getDepthFrameForVideoFrame(movieMillis);
		}
		else{
			depthFrame = sequencer.getDepthFrameForVideoFrame(player.getCurrentFrame());
		}
		depthSequence.selectTime(depthFrame);
		
		processDepthFrame();
		
		renderer.setDepthImage(depthPixelDecodeBuffer);
		renderer.update();
	}
}

void testApp::processDepthFrame(){
	
	for(int y = 0; y <	480; y++){
		for(int x = 0; x < 640; x++){
			int index = y*640+x;
//			if(depthPixelDecodeBuffer[index] == 0){
//				depthPixelDecodeBuffer[index] = 5000;
//			}
			//depthPixelDecodeBuffer[index] *= 1.0 - .0*(sin(y/10.0 + ofGetFrameNum()/10.0)*.5+.5); 
		}
	}
}
//--------------------------------------------------------------
void testApp::draw(){
	if(!allLoaded) return;
	
	
	//ofBackground(255*.2);
	ofBackground(255*.2);
	
	fbo.begin();
	ofClear(0, 0, 0);
	
	cam.begin(ofRectangle(0, 0, fbo.getWidth(), fbo.getHeight()));
	glEnable(GL_DEPTH_TEST);
	glPushMatrix();
//	ofScale(1, -1, 1);
	if(renderer.applyShader){
		renderer.rgbdShader.begin();
	}
	
	//qtRenderer.bind();
	//undistortedImage.getTextureReference().bind();
	player.getTextureReference().bind();
	renderer.getMesh().drawFaces();
	//renderer.drawPointCloud();
//	undistortedImage.getTextureReference().unbind();
	player.getTextureReference().unbind();
	//qtRenderer.unbind();
	if(renderer.applyShader){
		renderer.rgbdShader.end();
	}
	
	glDisable(GL_DEPTH_TEST);
	glPopMatrix();
	
	cam.end();
	
	fbo.end();
	
	if(saveCurrentFrame){
		fbo.getTextureReference().readToPixels(savingImage.getPixelsRef());
		char filename[512];
		sprintf(filename, "%s/save_%d_%05d.png", saveFolder.c_str(), uniqueRand, ofGetFrameNum());
		savingImage.saveImage(filename);
		saveCurrentFrame = false;
	}
	fbo.draw(300, 75, 1280*.75, 720*.75);
	
	timeline.setOffset(ofVec2f(0, ofGetHeight() - 200));
	
	timeline.draw();
	gui.draw();
	
	ofSetColor(255);
	ofDrawBitmapString(ofToString(ofGetFrameRate()), ofPoint(20,20));
	
	renderer.update();

}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		if(player.isPlaying()){
			player.stop();
		}
		else{
			player.play();
		}
		
	}
	
	if(key == 'u'){
		doUndistort = !doUndistort;
//		if(doUndistort){
//			renderer.getRGBCalibration().undistort(toCv(player), toCv(undistortedImage));
//			undistortedImage.update();
//		}
//		else{
//			undistortedImage.setFromPixels(player.getPixelsRef());
//		}
		
	}
	if(key == '='){
		renderer.applyShader = !renderer.applyShader;
	}
	
	if(key == 'l'){
		loadNewProject();
	}
	
//	if(key == OF_KEY_UP){
//		renderer.yshift++;
//		renderer.update();
//		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
//	}
//	if(key == OF_KEY_DOWN){
//		renderer.yshift--;
//		renderer.update();
//		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
//	}
//	if(key == OF_KEY_RIGHT){
//		renderer.xshift++;
//		renderer.update();
//		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
//	}
//	if(key == OF_KEY_LEFT){
//		renderer.xshift--;
//		renderer.update();
//		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
//	}
	
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
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}