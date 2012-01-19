#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofBackground(0);
	playerElementAdded = false;

	cam.speed = 40;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.setFarClip(30000);
	cam.setScale(1, -1, 1);
	cam.targetNode.setScale(1,-1,1);
	cameraRecorder.camera = &cam;
	cam.loadCameraPosition();
	
	hiResPlayer = NULL;
	lowResPlayer = NULL;
	startRenderMode = false;
	currentlyRendering = false;
	allLoaded = false;
	
	sampleCamera = false;
	playbackCamera = false;
	
	savingImage.setUseTexture(false);
	savingImage.allocate(1920,1080, OF_IMAGE_COLOR);
	
	fboRectangle = ofRectangle(250, 100, 1280*.75, 720*.75);
	fbo.allocate(1920, 1080, GL_RGB, 4);
	
	newCompButton = new ofxMSAInteractiveObjectWithDelegate();
	newCompButton->setLabel("New Comp");
	newCompButton->setDelegate(this);
	newCompButton->setPosAndSize(fboRectangle.x+fboRectangle.width+50, 0, 150, 25);
	
	saveCompButton = new ofxMSAInteractiveObjectWithDelegate();
	saveCompButton->setLabel("Save Comp");
	saveCompButton->setDelegate(this);
	saveCompButton->setPosAndSize(fboRectangle.x+fboRectangle.width+50, 25, 150, 25);
	cout << "compbuttons " << compbuttons.size() << endl;
	
	loadCompositions();
		
	gui.addSlider("X Linear Shift", currentXShift, -20, 20);
	gui.addSlider("Y Linear Shift", currentYShift, -35, 35);
	gui.addSlider("Camera Speed", cam.speed, .1, 40);
	gui.addToggle("Draw Pointcloud", drawPointcloud);
	gui.addToggle("Draw Wireframe", drawWireframe);
	gui.addToggle("Draw Mesh", drawMesh);
	gui.addSlider("Point Size", pointSize, 1, 10);
	gui.addSlider("Line Thickness", lineSize, 1, 10);
	gui.addTitle("");
	gui.addToggle("Render Composition", startRenderMode);

	gui.loadFromXML();
	gui.toggleDraw();
	
	
	cout << "setting up timeline " << endl;
}

//--------------------------------------------------------------
bool testApp::loadNewProject(){
	allLoaded = false;
	ofSystemAlertDialog("Select Directory");
	ofFileDialogResult r;
	r = ofSystemLoadDialog("Get Directory", true);
	if(r.bSuccess){
		currentMediaFolder = r.getPath();
		ofDirectory dataDirectory(currentMediaFolder);
		dataDirectory.listDir();
		int numFiles = dataDirectory.numFiles();
		string calibrationDirectory = "";
		videoPath = "";
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
			
			//assume
			if(testFile.find("txt") != string::npos){
				loadMarkerFile(dataDirectory.getPath(i));
			}
		}
		
		if(calibrationDirectory != "" && videoPath != "" && depthImageDirectory != ""){
			if(!loadAlignmentMatrices(calibrationDirectory)){
				ofSystemAlertDialog("Load Failed -- Couldn't Load Calibration Direcotry.");
				return false;
			}

			if(!loadDepthSequence(depthImageDirectory)){
				ofSystemAlertDialog("Load Failed -- Couldn't load dpeth iamges.");
				return false;
			}
			
			if(!loadVideoFile(videoPath)){
				ofSystemAlertDialog("Load Failed -- Couldn't load video.");
				return false;
			}
						
			ofDirectory compBin(currentMediaFolder + "/compositions/");
			if(!compBin.exists()){
				compBin.create(true);
			}
			compBin.listDir();
			int compNumber = compBin.numFiles();
			currentCompositionDirectory = currentMediaFolder + "/compositions/comp" + ofToString(compNumber) + "/";
			ofDirectory compFolder(currentCompositionDirectory);
			if(!compFolder.exists()){
				compFolder.create(true);
			}
			
			string currentCompositionFile = currentCompositionDirectory+"compositionsettings.xml";
			projectsettings.loadFile(currentCompositionFile);
			refreshCompButtons();
			allLoaded = true;
			return true;
		}
	}
	return false;	
}

//--------------------------------------------------------------
bool testApp::loadDepthSequence(string path){
	projectsettings.setValue("depthSequence", path);
	projectsettings.saveFile();
	
	depthSequence.setup();
	depthSequence.disable();
	
	depthPixelDecodeBuffer = depthSequence.currentDepthRaw;
	renderer.setDepthImage(depthPixelDecodeBuffer);

	return depthSequence.loadSequence(path);
}

//--------------------------------------------------------------
bool testApp::loadVideoFile(string path){
	projectsettings.setValue("videoFile", path);
	projectsettings.saveFile();
	
	if(hiResPlayer != NULL){
		delete hiResPlayer;
	}
	hiResPlayer = new ofVideoPlayer();

	if(!hiResPlayer->loadMovie(path)){
		ofSystemAlertDialog("Load Failed -- Couldn't load hi res video file.");
		return false;
	}
	
	string lowResPath = ofFilePath::removeExt(path) + "_small.mov";
	if(lowResPlayer != NULL){
		delete lowResPlayer;
	}
	lowResPlayer = new ofVideoPlayer();
	if(!lowResPlayer->loadMovie(lowResPath)){
		ofSystemAlertDialog("Load Failed -- Couldn't load low res video file.");
		return false;		
	}
	
	if(!sequencer.loadPairingFile(ofFilePath::removeExt(path) + "_pairings.xml")){
		ofSystemAlertDialog("Load Failed -- Couldn't load pairings file.");
		return false;				
	}
	
	renderer.setTextureScale(1.0*lowResPlayer->getWidth()/hiResPlayer->getWidth(), 
							 1.0*lowResPlayer->getHeight()/hiResPlayer->getHeight());
	
	videoThumbsPath = ofFilePath::removeExt(videoPath);
	if(!ofDirectory(videoThumbsPath).exists()){
		ofDirectory(videoThumbsPath).create(true);
	}
	
	videoTimelineElement.setup();
	timeline.setInOutRange(ofRange(.25,.75));	
		
	if(!playerElementAdded){
		timeline.setup();
		timeline.setOffset(ofVec2f(0, ofGetHeight() - 200));
		timeline.addElement("Video", &videoTimelineElement);
		playerElementAdded = true;
	}
	
	renderer.setRGBTexture(*lowResPlayer);
	
	timeline.setDurationInFrames(lowResPlayer->getTotalNumFrames());
	videoTimelineElement.setVideoPlayer(*lowResPlayer, videoThumbsPath);

	lowResPlayer->play();
	lowResPlayer->setSpeed(0);
	
	return true;
}

bool testApp::loadMarkerFile(string markerPath){
	markers.setVideoFPS(24);
	vector<string> acceptedTypes;
	acceptedTypes.push_back("Sequence");
	markers.setTypeFilters(acceptedTypes);
	if(!markers.parseMarkers(markerPath)){
		ofSystemAlertDialog("Warning -- Markers failed");
		return false;
	}
	projectsettings.setValue("markerPath", markerPath);
	projectsettings.saveFile();
	currentMarker = 0;
	
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

	if(startRenderMode){
		startRenderMode = false;
		currentlyRendering = true;
		saveFolder = currentCompositionDirectory + "rendered/";
		ofDirectory outputDirectory(saveFolder);
		if(!outputDirectory.exists()) outputDirectory.create(true);
		hiResPlayer->play();
		hiResPlayer->setSpeed(0);
		videoTimelineElement.setVideoPlayer(*hiResPlayer, videoThumbsPath);
		renderer.setRGBTexture(*hiResPlayer);
		renderer.setTextureScale(1.0, 1.0);
		currentRenderFrame = timeline.getInFrame();
		playbackCamera = true;
	}

	if(currentlyRendering){
		currentRenderFrame++;
		timeline.setCurrentFrame(currentRenderFrame);
		hiResPlayer->setFrame(currentRenderFrame);
		hiResPlayer->update();
		updateRenderer(*hiResPlayer);		
	}
	else {
		lowResPlayer->update();	
		if(lowResPlayer->isFrameNew()){		
			updateRenderer(*lowResPlayer);
		}
	}
	
	if(currentXShift != renderer.xshift || currentYShift != renderer.yshift){
		renderer.xshift = currentXShift;
		renderer.yshift = currentYShift;
		renderer.update();
	}
}

//--------------------------------------------------------------
void testApp::updateRenderer(ofVideoPlayer& fromPlayer){
	if(sequencer.isSequenceTimebased()){
		long movieMillis = fromPlayer.getPosition() * fromPlayer.getDuration()*1000;
		long depthFrame = sequencer.getDepthFrameForVideoFrame(movieMillis);
		depthSequence.selectTime(depthFrame);
	}
	else {
		long depthFrame = sequencer.getDepthFrameForVideoFrame(fromPlayer.getCurrentFrame());
		depthSequence.selectFrame(depthFrame);
	}
	
	processDepthFrame();
	
	renderer.update();
	
	if(playbackCamera){
		cameraRecorder.moveCameraToFrame(fromPlayer.getCurrentFrame());
	}

	if(sampleCamera){
		cameraRecorder.sample(fromPlayer.getCurrentFrame());
	}
}

//--------------------------------------------------------------
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
	
	ofBackground(255*.2);

	if(!allLoaded) return;

	fbo.begin();
	ofClear(0, 0, 0);
	
	cam.begin(ofRectangle(0, 0, fbo.getWidth(), fbo.getHeight()));
	if(drawPointcloud){
		glPointSize(pointSize);
		renderer.drawPointCloud();
	}
	if(drawWireframe){
		glLineWidth(lineSize);
		renderer.drawWireFrame();
	}
	if(drawMesh){
		renderer.drawMesh();
	}
	cam.end();
	
	fbo.end();	
	fbo.getTextureReference().draw(fboRectangle);
	if(currentlyRendering){
		fbo.getTextureReference().readToPixels(savingImage.getPixelsRef());
		char filename[512];
		sprintf(filename, "%s/save_%05d.png", saveFolder.c_str(), hiResPlayer->getCurrentFrame());
		savingImage.saveImage(filename);
		
		//stop when finished
		if(currentRenderFrame > timeline.getOutFrame()){
			currentlyRendering = false;
		}
	}
	
	
	if(sampleCamera){
		ofDrawBitmapString("RECORDING CAMERA", ofPoint(600, 10));
	}
	if(playbackCamera){
		ofDrawBitmapString("PLAYBACK CAMERA", ofPoint(600, 10));
	}
	
	timeline.draw();
	gui.draw();
	
	ofSetColor(255);
	ofDrawBitmapString(ofToString(ofGetFrameRate()), ofPoint(20,20));
}

//--------------------------------------------------------------
void testApp::loadCompositions(){
	cout << "compbuttons pre " << compbuttons.size() << endl;
	ofFileDialogResult r = ofSystemLoadDialog("Select Media Bin", true);
	if(r.bSuccess){
		cout << "compbuttons post " << compbuttons.size() << endl;
		mediaBinDirectory = r.getPath();
		refreshCompButtons();
	}
}

//--------------------------------------------------------------
void testApp::refreshCompButtons(){
	ofDirectory dir(mediaBinDirectory);
	dir.listDir();
	int mediaFolders = dir.numFiles();
//	for(int i = 0; i < compbuttons.size(); i++){
//		delete compbuttons[i];
//	}
	cout << " num media folders " << mediaFolders << endl;
	int currentCompButton = 0;
	for(int i = 0; i < mediaFolders; i++){
		string compositionsFolder = dir.getPath(i) + "/compositions/";
//		cout << "loading comps from folder " << compositionsFolder << endl;
		ofDirectory compositionsDirectory(compositionsFolder);
		if(!compositionsDirectory.exists()){
			compositionsDirectory.create(true);
		}
		
		compositionsDirectory.listDir();
		int numComps = compositionsDirectory.numFiles();
		cout << " num comp folders " << compbuttons.size() << endl;
		for(int c = 0; c < numComps; c++){
			ofxMSAInteractiveObjectWithDelegate* d;
			if(currentCompButton >= compbuttons.size()){
				d = new ofxMSAInteractiveObjectWithDelegate();
				d->setup();
				d->setDelegate(this);
				compbuttons.push_back(d);
			}
			else{
				d = compbuttons[currentCompButton];
			}
			 
			d->setPosAndSize(newCompButton->x+newCompButton->width, currentCompButton*25, 200, 25);
			string fullCompPath = compositionsDirectory.getPath(c);
			string compLabel = ofFilePath::getFileName(fullCompPath);
			d->setLabel(fullCompPath);
			fullCompPaths.push_back(fullCompPath);				
			currentCompButton++;
		}
	}	
}

//--------------------------------------------------------------
void testApp::newComposition(){	
	loadNewProject();
}

//--------------------------------------------------------------
void testApp::saveComposition(){
	string cameraSaveFile = currentCompositionDirectory + "camera.xml";
	cameraRecorder.writeToFile(cameraSaveFile);
	projectsettings.setValue("cameraSpeed", cam.speed);
	projectsettings.setValue("shiftx", currentXShift);
	projectsettings.setValue("shifty", currentYShift);
	projectsettings.setValue("cameraFile", cameraSaveFile);
	projectsettings.saveFile();	
}

void testApp::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y){
}

void testApp::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y){
}

void testApp::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button){
}

void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){
	if(object == newCompButton){
		newComposition();
	}
	else if(object == saveCompButton){
		saveComposition();		
	}
	else {
		for(int i = 0; i < compbuttons.size(); i++){
			if(compbuttons[i] == object){
				currentCompositionDirectory = fullCompPaths[i] + "/";
				cout << "loading comp " << currentCompositionDirectory << " clicked comp button is " << compbuttons[i]->getLabel() << endl;
				if(projectsettings.loadFile(currentCompositionDirectory+"compositionsettings.xml")){
					string loadedAlignmentDir = projectsettings.getValue("alignmentDir", "");
					if(!loadAlignmentMatrices(loadedAlignmentDir)){
						return;
					}
					cout << "loaded alignment " << loadedAlignmentDir << endl;
					
					string loadedVideoFile = projectsettings.getValue("videoFile", "");
					if(!loadVideoFile(loadedVideoFile)){
						return;
					}
					cout << "loaded video " << loadedVideoFile << endl;
					
					string loadedDepthSequence = projectsettings.getValue("depthSequence", "");
					if(!loadDepthSequence(loadedDepthSequence)){
						return;
					}
					
					string markerPath = projectsettings.getValue("markerPath", "");
					if(markerPath != ""){
						loadMarkerFile(markerPath);
					}
					cout << "loaded depth " << loadedDepthSequence << endl;
					
					string cameraFile = projectsettings.getValue("cameraFile", "");
					if(cameraFile != ""){
						cameraRecorder.loadFromFile(cameraFile);
					}
					cam.speed = projectsettings.getValue("cameraSpeed", 20.);
					currentXShift = projectsettings.getValue("shiftx", 0.);
					currentYShift = projectsettings.getValue("shifty", 0.);
					cout << "setting " << currentXShift << " " << currentYShift << endl;
					allLoaded = true;
				}
			}		
		}
	}
}

//--------------------------------------------------------------

void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		if(lowResPlayer->getSpeed() != 0.0){
			lowResPlayer->setSpeed(0);
		}
		else{
			lowResPlayer->play();
			lowResPlayer->setSpeed(1.0);
		}		
	}
	
	if(key == 'i'){
		timeline.setCurrentTimeToInPoint();	
	}

	if(key == 'o'){
		timeline.setCurrentTimeToOutPoint();
	}
	
	if (key == 'j' && markers.isLoaded()) {
		currentMarker--;
		if(currentMarker < 0) currentMarker = markers.getMarkers().size() - 1;		
		lowResPlayer->setFrame(markers.getMarkers()[currentMarker].calculatedFrame);
	}

	if (key == 'l' && markers.isLoaded()) {
		currentMarker = (currentMarker + 1) % markers.getMarkers().size();
		lowResPlayer->setFrame(markers.getMarkers()[currentMarker].calculatedFrame);
	}

	//RECORD CAMERA
	if(key == 'R'){	
		if(sampleCamera){
			sampleCamera = false;
		}
		else{
			cameraRecorder.reset();
			sampleCamera = true;
		}
	}
	
	//PLAYBACK CAMERA
	if(key == 'P' && cameraRecorder.getSamples().size() > 0){
		playbackCamera = !playbackCamera;
		cam.applyRotation = cam.applyTranslation = !playbackCamera;
		
		if(playbackCamera){
			sampleCamera = false;
			timeline.setInPointAtFrame(cameraRecorder.getFirstFrame());
			timeline.setOutPointAtFrame(cameraRecorder.getLastFrame());
		}
	}
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

//	for(int i = 0; i < compbuttons.size(); i++){
//		compbuttons[i]->setPosAndSize(ofGetWidth()-250, i*25, 200, 25);
//	}
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}