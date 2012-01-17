#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofBackground(0);

	saveCurrentFrame = false;
	onRenderMode = false;
	
	savingImage.setUseTexture(false);
	savingImage.allocate(1920,1080, OF_IMAGE_COLOR);
	
	fboRectangle = ofRectangle(300, 75, 1280*.75, 720*.75);
	fbo.allocate(1920, 1080, GL_RGB, 4);
	
	if(projectsettings.loadFile("projectsettings.xml")){
		string loadedAlignmentDir = projectsettings.getValue("alignmentDir", "");
		if(!loadAlignmentMatrices(loadedAlignmentDir)){
			loadNewProject();
		}
		cout << "loaded alignment " << loadedAlignmentDir << endl;
		
		string loadedVideoFile = projectsettings.getValue("videoFile", "");
		if(!loadVideoFile(loadedVideoFile)){
			loadNewProject();
		}
		cout << "loaded video " << loadedVideoFile << endl;
		
		string loadedDepthSequence = projectsettings.getValue("depthSequence", "");
		if(!loadDepthSequence(loadedDepthSequence)){
			loadNewProject();
		}
		cout << "loaded depth " << loadedDepthSequence << endl;
		
		allLoaded = true;
	}
	else {
		loadNewProject();
	}
	
	renderer.setRGBTexture(lowResPlayer);
	//renderer.setRGBTexture(undistortedImage);
	
	
	cam.speed = 40;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.loadCameraPosition();
	cam.setFarClip(30000);
	
	depthPixelDecodeBuffer = depthSequence.currentDepthRaw;
	
	string videoThumbsPath = ofFilePath::removeExt(videoPath);
	if(!ofDirectory(videoThumbsPath).exists()){
		ofDirectory(videoThumbsPath).create(true);
	}

	cout << "setting up timeline " << endl;

	timeline.setup();
	timeline.setOffset(ofVec2f(0, ofGetHeight() - 200));

	videoTimelineElement.setup();
	
	cout << "setting duration to  timeline " << lowResPlayer.getTotalNumFrames() << endl;
	
	timeline.setDurationInFrames(lowResPlayer.getTotalNumFrames());
	timeline.addElement("Video", &videoTimelineElement);

	cout << "added video element" << endl;

	//videoTimelineElement.toggleThumbs();
	videoTimelineElement.setVideoPlayer(lowResPlayer, videoThumbsPath);

	cout << "setting video player on element " << endl;

	gui.addSlider("X Linear Shift", renderer.xshift, -20, 20);
	gui.addSlider("Y Linear Shift", renderer.yshift, -150, 15);
	gui.addToggle("Use Distorted bits", renderer.useDistorted);
	gui.addToggle("Start Render", onRenderMode);
	
	gui.loadFromXML();
	gui.toggleDraw();
	
	cam.setScale(1, -1, 1);

	onRenderMode = false;
}

bool testApp::loadNewProject(){
	allLoaded = false;
	ofSystemAlertDialog("Select Directory");
	ofFileDialogResult r;
	r = ofSystemLoadDialog("Get Directory", true);
	if(r.bSuccess){
		ofDirectory dataDirectory(r.getPath());
		dataDirectory.listDir();
		int numFiles = dataDirectory.numFiles();
		string calibrationDirectory = "";
		string videoPath = "";
		string depthImageDirectory = "";
		for(int i = 0; i < numFiles; i++){
			
			string testFile = dataDirectory.getName(i);
			
			if(testFile.find("calibration") != string::npos){
				calibrationDirectory = dataDirectory.getPath(i);
			}
			   
			if(testFile.find("depth") != string::npos){
				depthImageDirectory = dataDirectory.getPath(i);
			}
			
			if(testFile.find("mov") != string::npos){
			 	if(testFile.find("small") == string::npos){
					videoPath = dataDirectory.getPath(i);
				}
			}
		}
	
		if(calibrationDirectory != "" && videoPath != "" && depthImageDirectory != ""){
			if(!loadAlignmentMatrices(calibrationDirectory)){
				ofSystemAlertDialog("Load Failed -- Couldn't Load Calibration Direcotry.");
				return false;
			}

			if(!loadVideoFile(videoPath)){
				ofSystemAlertDialog("Load Failed -- Couldn't load video.");
				return false;
			}
			
			if(!loadDepthSequence(depthImageDirectory)){
				ofSystemAlertDialog("Load Failed -- Couldn't load dpeth iamges.");
				return false;
			}
			return true;
		}
	}
	allLoaded = true;
	return false;
	
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
	
	if(!hiResPlayer.loadMovie(path)){
		ofSystemAlertDialog("Load Failed -- Couldn't load hi res video file.");
		return false;
	}
	
	string lowResPath = ofFilePath::removeExt(path) + "_small.mov";
	if(!lowResPlayer.loadMovie(lowResPath)){
		ofSystemAlertDialog("Load Failed -- Couldn't load low res video file.");
		return false;		
	}
	
	if(!sequencer.loadPairingFile(ofFilePath::removeExt(path) + "_pairings.xml")){
		ofSystemAlertDialog("Load Failed -- Couldn't load pairings file.");
		return false;				
	}
	
	renderer.setTextureScale(1.0*lowResPlayer.getWidth()/hiResPlayer.getWidth(), 
							 1.0*lowResPlayer.getHeight()/hiResPlayer.getHeight());
	saveFolder = "saveout_" + ofFilePath::getBaseName(path);
	uniqueRand = ofGetMonth()+ofGetDay()+ofGetMinutes()+ofGetSeconds();
	ofDirectory savdir(saveFolder);
	if(!savdir.exists()){
		savdir.create(true);
	}
	
	return true;
}

bool testApp::loadAlignmentMatrices(string path){
	projectsettings.setValue("alignmentDir", path);
	projectsettings.saveFile();
	
	return renderer.setup(path);
}

//--------------------------------------------------------------
void testApp::update(){
	if(!allLoaded) return;
	
//	if(onRenderMode){
//		if(player.isPlaying()){
//			player.stop();
//		}
//		
//		//player.setFrame(player.getCurrentFrame()+1);
//		videoTimelineElement.selectFrame(videoTimelineElement.getSelectedFrame()+1);
//
//		cout << "current frame is " << lowResPlayer.getCurrentFrame() << endl;
//	}
	
	lowResPlayer.update();
	
	if(lowResPlayer.isFrameNew()){
		saveCurrentFrame = onRenderMode;
				
		long depthFrame;
		if(sequencer.isSequenceTimebased()){
			long movieMillis = lowResPlayer.getPosition() * lowResPlayer.getDuration()*1000;
			depthFrame = sequencer.getDepthFrameForVideoFrame(movieMillis);
			depthSequence.selectTime(depthFrame);
		}
		else{
			depthFrame = sequencer.getDepthFrameForVideoFrame(lowResPlayer.getCurrentFrame());
			depthSequence.selectFrame(depthFrame);
		}
		
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
	ofScale(1, -1, 1);
	
	lowResPlayer.getTextureReference().bind();
	//renderer.getMesh().drawFaces();
	renderer.drawPointCloud();

	lowResPlayer.getTextureReference().unbind();
	
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
	
	
	fbo.getTextureReference().draw(fboRectangle);
	
	timeline.draw();
	gui.draw();
	
	ofSetColor(255);
	ofDrawBitmapString(ofToString(ofGetFrameRate()), ofPoint(20,20));
	
	renderer.update();

}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		if(lowResPlayer.isPlaying()){
			lowResPlayer.stop();
		}
		else{
			lowResPlayer.play();
		}		
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
	cam.usemouse = fboRectangle.inside(x, y);
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
	timeline.setOffset(ofVec2f(0, ofGetHeight() - timeline.getDrawRect().height));
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}