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
	
	shouldSaveCameraPoint = false;
	shouldClearCameraMoves = false;
	
	
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

	loadCompositions();

	gui.addSlider("X Linear Shift", currentXShift, -25, 25);
	gui.addSlider("Y Linear Shift", currentYShift, -35, 50);
	gui.addSlider("Camera Speed", cam.speed, .1, 40);
	gui.addToggle("Draw Pointcloud", drawPointcloud);
	gui.addToggle("Draw Wireframe", drawWireframe);
	gui.addToggle("Draw Mesh", drawMesh);
	gui.addSlider("Point Size", pointSize, 1, 10);
	gui.addSlider("Line Thickness", lineSize, 1, 10);
	gui.addSlider("Edge Cull", currentEdgeCull, 1, 500);
	gui.addSlider("Z Far Clip", farClip, 2, 5000);
	gui.addSlider("Simplify", currentSimplify, 1, 4);
	
	gui.addSlider("LightX", lightpos.x, -1500, 1500);
	gui.addSlider("LightY", lightpos.y, -1500, 1500);
	gui.addSlider("LightZ", lightpos.z, -1500, 1500);
	
	
	gui.addToggle("Clear Camera Moves", shouldClearCameraMoves);
	gui.addToggle("Set Camera Point", shouldSaveCameraPoint);
	
	gui.addTitle("");
	gui.addToggle("Render Composition", startRenderMode);

	gui.loadFromXML();
	gui.toggleDraw();
	
//	ofEnableLighting();
//	light.setPosition(ofGetWidth()*.5, ofGetHeight()*.25, 0);
//	light.enable();	
}

//--------------------------------------------------------------
bool testApp::loadNewProject(){
	allLoaded = false;
	ofSystemAlertDialog("Select Someone's Name");
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
//			if(testFile.find("txt") != string::npos){
//				//loadMarkerFile(dataDirectory.getPath(i));
//			}
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
			cam.cameraPositionFile = currentCompositionDirectory + "camera_position.xml";
			cam.loadCameraPosition();

			ofDirectory compFolder(currentCompositionDirectory);
			if(!compFolder.exists()){
				compFolder.create(true);
			}
			
			string currentCompositionFile = currentCompositionDirectory+"compositionsettings.xml";
			projectsettings.loadFile(currentCompositionFile);
			saveComposition();
			refreshCompButtons();
			allLoaded = true;
			return true;
		}
		else{
			ofSystemAlertDialog("Couldn't find one of the following: Calib==[" + calibrationDirectory + "] Video==[" + videoPath + "] Depth==[" + depthImageDirectory + "]");
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

	light.setPosition(lightpos.x, lightpos.y, lightpos.z);
	
	if(startRenderMode){
		startRenderMode = false;
		currentlyRendering = true;
		saveFolder = currentCompositionDirectory + "rendered/";
		ofDirectory outputDirectory(saveFolder);
		if(!outputDirectory.exists()) outputDirectory.create(true);
		hiResPlayer->play();
		hiResPlayer->setSpeed(0);
		hiResPlayer->setVolume(0);

		renderer.setRGBTexture(*hiResPlayer);
		renderer.setTextureScale(1.0, 1.0);
//		currentSimplify = 1;
//		currentRenderFrame = timeline.getInFrame();
		startCameraPlayback();
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
	
	if(shouldClearCameraMoves){
		cameraRecorder.reset();
		shouldClearCameraMoves = false;
	}
	
	if(shouldSaveCameraPoint){
		cameraRecorder.sample(lowResPlayer->getCurrentFrame());
		shouldSaveCameraPoint = false;	
	}
	
	if(currentXShift != renderer.xshift || 
	   currentYShift != renderer.yshift || 
	   currentSimplify != renderer.getSimplification() ||
	   currentEdgeCull != renderer.edgeCull ||
	   farClip != renderer.farClip){
		
		renderer.farClip = farClip;
		renderer.xshift = currentXShift;
		renderer.yshift = currentYShift;
		renderer.edgeCull = currentEdgeCull;
		renderer.setSimplification(currentSimplify);
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

//			if(depthPixelDecodeBuffer[index] < currentZThresh){
//				depthPixelDecodeBuffer[index] = 0;
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
//	ofEnableLighting();
	
	cam.begin(ofRectangle(0, 0, fbo.getWidth(), fbo.getHeight()));
	
	if(!drawPointcloud && !drawWireframe && !drawMesh){
		drawPointcloud = true;
	}
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
	
//	light.setPointLight();

//	for(int i = 0; i < renderer.getMesh().getNormals().size(); i++){
//		ofLine(renderer.getMesh().getVertices()[i], 
//			   renderer.getMesh().getVertices()[i] + renderer.getMesh().getNormals()[i]*10);
//	}
	cam.end();
//	ofDisableLighting();
	
	fbo.end();	
	fboRectangle.height = (timeline.getDrawRect().y - fboRectangle.y - 20);
	fboRectangle.width = 16.0/9.0*fboRectangle.height;
	ofDrawBitmapString(currentCompositionDirectory, ofPoint(fboRectangle.x, fboRectangle.y-15));
	fbo.getTextureReference().draw(fboRectangle);
	if(currentlyRendering){
		fbo.getTextureReference().readToPixels(savingImage.getPixelsRef());
		char filename[512];
		sprintf(filename, "%s/save_%05d.png", saveFolder.c_str(), hiResPlayer->getCurrentFrame());
		savingImage.saveImage(filename);
		
		//stop when finished
		if(currentRenderFrame > timeline.getOutFrame()){
			finishRender();
		}
	}
	
	if(sampleCamera){
		ofDrawBitmapString("RECORDING CAMERA", ofPoint(600, 10));
	}
	if(playbackCamera){
		ofDrawBitmapString("PLAYBACK CAMERA", ofPoint(600, 10));
	}
	
	ofPushStyle();
	ofSetColor(100,100,255, 200);
	for(int i = 0; i < comps.size(); i++){
		if(comps[i]->batchExport){
			ofRect(*comps[i]->toggle);
		}
	}
	ofPopStyle();
	
	timeline.draw();
	gui.draw();
	
	ofSetColor(255);
	ofDrawBitmapString(ofToString(ofGetFrameRate()), ofPoint(20,20));
}

//--------------------------------------------------------------
void testApp::loadCompositions(){
	ofSystemAlertDialog("Select the MediaBin containing everyone's names");

	ofFileDialogResult r = ofSystemLoadDialog("Select Media Bin", true);
	if(r.bSuccess){
		mediaBinDirectory = r.getPath();
		refreshCompButtons();
	}
}

//--------------------------------------------------------------
void testApp::refreshCompButtons(){
	ofDirectory dir(mediaBinDirectory);
	dir.listDir();
	int mediaFolders = dir.numFiles();
	int currentCompButton = 0;
	for(int i = 0; i < mediaFolders; i++){
		
		string compositionsFolder = dir.getPath(i) + "/compositions/";
		ofDirectory compositionsDirectory(compositionsFolder);
		if(!compositionsDirectory.exists()){
			compositionsDirectory.create(true);
		}
		
		compositionsDirectory.listDir();
		int numComps = compositionsDirectory.numFiles();
		int compx = newCompButton->x+newCompButton->width;
		for(int c = 0; c < numComps; c++){
			Comp* comp;
			
			if(currentCompButton >= comps.size()){
				comp = new Comp();
				comp->load  = new ofxMSAInteractiveObjectWithDelegate();
				comp->load->setup();
				comp->load->setDelegate(this);
				
				comp->toggle = new ofxMSAInteractiveObjectWithDelegate();
				comp->toggle->setup();
				comp->toggle->setDelegate(this);				
				comps.push_back(comp);
			}
			else{
				comp = comps[currentCompButton];
			}
			comp->batchExport = false;
			comp->toggle->setPosAndSize(compx-25,currentCompButton*25,25,25);
			comp->load->setPosAndSize(compx, currentCompButton*25, 500, 25);
			comp->fullCompPath = compositionsDirectory.getPath(c);
			vector<string> compSplit = ofSplitString(comp->fullCompPath, "/", true, true);
			string compLabel = compSplit[compSplit.size()-3] + ":" + compSplit[compSplit.size()-1];
			
			comp->load->setLabel(compLabel);
			
//			comps.push_back(fullCompPath);				
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
	projectsettings.setValue("pointSize", pointSize);
	projectsettings.setValue("lineSize", lineSize);
	projectsettings.setValue("cameraFile", cameraSaveFile);
	projectsettings.setValue("pointcloud", drawPointcloud);
	projectsettings.setValue("wireframe", drawWireframe);
	projectsettings.setValue("mesh", drawMesh);
	projectsettings.setValue("currentEdgeCull", currentEdgeCull);
	projectsettings.setValue("farClip",farClip);
	
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
		for(int i = 0; i < comps.size(); i++){
			
			if(comps[i]->toggle == object){
				comps[i]->batchExport = !comps[i]->batchExport;
				break;
			}

			if(comps[i]->load == object){
				currentCompositionDirectory = comps[i]->fullCompPath + "/";
				cam.cameraPositionFile = currentCompositionDirectory + "camera_position.xml";
				cam.loadCameraPosition();
				cout << "loading comp " << currentCompositionDirectory << " clicked comp button is " << comps[i]->load->getLabel() << endl;
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
						if(cameraRecorder.getSamples().size() > 0){
							timeline.setInPointAtFrame(cameraRecorder.getFirstFrame());
							timeline.setOutPointAtFrame(cameraRecorder.getLastFrame());
						}
					}
					cam.speed = projectsettings.getValue("cameraSpeed", 20.);
					currentXShift = projectsettings.getValue("shiftx", 0.);
					currentYShift = projectsettings.getValue("shifty", 0.);
					pointSize = projectsettings.getValue("pointSize", 1);
					lineSize = projectsettings.getValue("lineSize", 1);
					currentEdgeCull = projectsettings.getValue("edgeCull", 50);
					farClip = projectsettings.getValue("farClip", 5000);
					drawPointcloud = projectsettings.getValue("pointcloud", false);
					drawWireframe = projectsettings.getValue("wireframe", false);
					drawMesh = projectsettings.getValue("mesh", false);
					
					allLoaded = true;
					break;
				}
			}		
		}
	}
}

//--------------------------------------------------------------

void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){

}

void testApp::finishRender(){
	currentlyRendering = false;
	stopCameraPlayback();
	renderer.setRGBTexture(*lowResPlayer);
	renderer.setTextureScale(1.0*lowResPlayer->getWidth()/hiResPlayer->getWidth(), 
							 1.0*lowResPlayer->getHeight()/hiResPlayer->getHeight());	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(currentlyRendering){
		if(key == ' '){
			finishRender();
			renderer.setRGBTexture(*lowResPlayer);
		}
		return;
	}
	
	if(key == ' '){
		if(lowResPlayer->getSpeed() != 0.0){
			lowResPlayer->setSpeed(0);
			stopCameraPlayback();
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
	
//	if (key == 'j' && markers.isLoaded()) {
//		currentMarker--;
//		if(currentMarker < 0) currentMarker = markers.getMarkers().size() - 1;		
//		lowResPlayer->setFrame(markers.getMarkers()[currentMarker].calculatedFrame);
//	}
//
//	if (key == 'l' && markers.isLoaded()) {
//		currentMarker = (currentMarker + 1) % markers.getMarkers().size();
//		lowResPlayer->setFrame(markers.getMarkers()[currentMarker].calculatedFrame);
//	}

	if(key == 'f'){
		ofToggleFullscreen();
	}
	
	//RECORD CAMERA
	if(key == 'R'){	
		if(sampleCamera){
			sampleCamera = false;
		}
		else {
			cameraRecorder.reset();
			sampleCamera = true;
			lowResPlayer->setSpeed(1.0);
		}
	}
	
	//PLAYBACK CAMERA
	if(key == 'P'){
		toggleCameraPlayback();
	}
}

void testApp::stopCameraPlayback(){
	playbackCamera = false;
	lowResPlayer->setSpeed(0.);
	hiResPlayer->setSpeed(0.);
	cam.applyRotation = true;
	cam.applyTranslation = true;
}

void testApp::startCameraPlayback(){
	if(cameraRecorder.getSamples().size() > 0){
		playbackCamera = true;
		cam.applyRotation = false;
		cam.applyTranslation = false;
		timeline.setInPointAtFrame(cameraRecorder.getFirstFrame());
		timeline.setOutPointAtFrame(cameraRecorder.getLastFrame());
		
		if(currentlyRendering){
			hiResPlayer->setSpeed(1.0);
		}
		else{
			lowResPlayer->setSpeed(1.0);
		}
	}
	else{
		ofSystemAlertDialog("Save a camera move in order to play back");
	}
}

void testApp::toggleCameraPlayback(){
	if(playbackCamera){
		stopCameraPlayback();
	}
	else {
		startCameraPlayback();
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
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}