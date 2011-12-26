#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	doUndistort = false;
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofBackground(0);
	
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
	
	player.play();

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
	
	player.update();
	
	if(player.isFrameNew()){
		//cout << "new frame?" << endl;
	//qtRenderer.update();
	//if(qtRenderer.isFrameNew()){
		if(doUndistort){
			renderer.getDepthCalibration().undistort(toCv(player), toCv(undistortedImage));
			undistortedImage.update();
		}
		else{
			undistortedImage.setFromPixels(player.getPixelsRef());
		}
		
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
	ofBackground(255*.0);
	
	cam.begin();
	glEnable(GL_DEPTH_TEST);
	glPushMatrix();
	ofScale(1, -1, 1);
	if(renderer.applyShader){
		renderer.rgbdShader.begin();
	}
	
	//qtRenderer.bind();
	//undistortedImage.getTextureReference().bind();
	player.getTextureReference().bind();
	renderer.getMesh().drawFaces();
	//renderer.drawPointCloud();
	undistortedImage.getTextureReference().unbind();
	player.getTextureReference().unbind();
	//qtRenderer.unbind();
	if(renderer.applyShader){
		renderer.rgbdShader.end();
	}
	
	glDisable(GL_DEPTH_TEST);
	glPopMatrix();
	
	cam.end();
	
	timeline.draw();

	ofSetColor(255);
	ofDrawBitmapString(ofToString(ofGetFrameRate()), ofPoint(20,20));
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
	
	if(key == OF_KEY_UP){
		renderer.yshift++;
		renderer.update();
		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
	}
	if(key == OF_KEY_DOWN){
		renderer.yshift--;
		renderer.update();
		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
	}
	if(key == OF_KEY_RIGHT){
		renderer.xshift++;
		renderer.update();
		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
	}
	if(key == OF_KEY_LEFT){
		renderer.xshift--;
		renderer.update();
		cout << "shifts: " << renderer.xshift << " " << renderer.yshift << endl;
	}
	
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