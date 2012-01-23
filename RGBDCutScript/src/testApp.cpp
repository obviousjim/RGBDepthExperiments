#include "testApp.h"
#include "Poco/File.h"
//--------------------------------------------------------------
void testApp::setup(){

	ofSystemAlertDialog("Choose a Depth Directory");
	ofFileDialogResult r = ofSystemLoadDialog("Choose Depth Directory", true);
	string depthDirectoryPath;
	if(r.bSuccess){
		depthDirectoryPath = r.getPath();
	}
	else{
		ofSystemAlertDialog("Must load Depth");
		ofExit(0);
	}
	
	ofSystemAlertDialog("Choose pairings file");
	r = ofSystemLoadDialog("Choose pairings file", false);
	string pairingsFile;
	if(r.bSuccess){
		pairingsFile = r.getPath();
	}
	else{
		ofSystemAlertDialog("Must load pairings");
		ofExit(0);
	}
	
	
	ofSystemAlertDialog("Choose FCP Markers");
	r = ofSystemLoadDialog("Choose FCP Markers", false);	
	string markersFile;
	if(r.bSuccess){
		markersFile = r.getPath();
	}
	else {
		ofSystemAlertDialog("Must load marker file");
		ofExit(0);
	}
	
	//Load the markers
	markers.setVideoFPS(24);
	markers.addTypeFilter("Sequence");
	if(!markers.parseMarkers(markersFile)){
		ofSystemAlertDialog("Markers file failed to load");
		ofExit(0);
	}
	
	if(markers.getMarkers().size() == 0){
		ofSystemAlertDialog("Markers file is empty");
		ofExit(0);	
	}

	if(markers.getMarkers().size() % 2 != 0){
		ofSystemAlertDialog("Markers file has uneven entries " + ofToString( markers.getMarkers().size() ));
		ofExit(0);	
	}
	
	ofDirectory depthDirectory(depthDirectoryPath);
	depthDirectory.allowExt("png");
	depthDirectory.listDir();
	depthDirectory.sort();
	
	//make a new depth folder
	string newDepthDirectoryPath = depthDirectoryPath+"/_converted/"; 
	ofDirectory newDepthDirectory(newDepthDirectoryPath);
	newDepthDirectory.create(true);
	
	//for each marker pair, get the depth in and out frames
	if(!oldPairings.loadPairingFile(pairingsFile)){
		ofSystemAlertDialog("Pairings file failed to parse");
		ofExit(0);	
	}
	
	if(markers.getMarkers().size() % 2 != 0){
		ofSystemAlertDialog("Markers file has uneven number? " + ofToString( markers.getMarkers().size() ));
		ofExit(0);
	}
	
	map<int,int> oldToNewIndeces;
	//copy the pairs into the new depth folder
	int framesCopied = 0;
	for(int i = 0; i < markers.getMarkers().size(); i+=2){
		int startVideoFrame = markers.getMarkers()[i].calculatedFrame;
		int endVideoFrame = markers.getMarkers()[i+1].calculatedFrame;
		int startDepthFrame = oldPairings.getDepthFrameForVideoFrame(startVideoFrame);
		int endDepthFrame = oldPairings.getDepthFrameForVideoFrame(endVideoFrame);
		cout << "SEGMENT: " << i/2 << endl;
		cout << "	VIDEO FRAMES [ " << startVideoFrame << " - " << endVideoFrame << " ] " << endl;
		cout << "	DEPTH FRAMES [ " << startDepthFrame << " - " << endDepthFrame << " ] " << endl;
		for(int f = startDepthFrame; f <= endDepthFrame; f++){
			if(f < depthDirectory.size()){
				string copyFrom = depthDirectory.getPath(f);
				string copyTo   = ofFilePath::getEnclosingDirectory(depthDirectory.getPath(f)) + "_converted/" + depthDirectory.getName(f);
				//cout << "copying from " << copyFrom << " to " << copyTo << endl;
				//ofFile::copyFromTo(copyFrom, copyTo, false, true);
				Poco::File fileSrc(copyFrom);
				try{
					fileSrc.copyTo(copyTo);
				}catch (Poco::Exception & except){
					ofLog(OF_LOG_ERROR, "moveFromTo - unable to move to, file must be busy");
				}
				
				oldToNewIndeces[f] = framesCopied++;
			}
//			else{
				//ofSystemAlertDialog("Pairings exceeded depth at frame " + ofToString(f));
				//ofExit(0);
//			}
		}
	}
	
	//create a new pairing file that reflects the changes
	for(int i = 0; i < oldPairings.getPairs().size(); i++){
		int oldDepthFrame = oldPairings.getPairs()[i].depthFrame;
		if(oldToNewIndeces.find(oldDepthFrame) == oldToNewIndeces.end()){
			cout << "WARNING -- aligned frame " << oldDepthFrame << " was not included in the new set " << endl;
			continue;
		}
		
		long accruedCutFrames = markers.getMarkers()[0].calculatedFrame;
		int oldVideoFrame = oldPairings.getPairs()[i].videoFrame;
		for(int m = 0; m < markers.getMarkers().size(); m+=2){
			if(m!=0){
				accruedCutFrames += (markers.getMarkers()[m].calculatedFrame - markers.getMarkers()[m-1].calculatedFrame);
			}			
			
			if(markers.getMarkers()[m].calculatedFrame < oldVideoFrame && markers.getMarkers()[m+1].calculatedFrame > oldVideoFrame){
				VideoDepthPair p;
				p.isTimeBased = false;
				p.depthFrame = oldToNewIndeces[oldDepthFrame];
				p.videoFrame = oldVideoFrame - accruedCutFrames;
				newPairings.addAlignedPair(p);
				cout << "ADDING PAIR: " << endl;
				cout << "	VIDEO FRAME  " << oldVideoFrame << " --> " << p.videoFrame << endl;
				cout << "	DEPTH FRAME  " << oldDepthFrame << " --> " << p.depthFrame << endl;
				cout << "	ACCRUED " << accruedCutFrames << " CUT FRAMES " << endl;
				break;
			}			
		}
	}
	string newPairingsPath = newDepthDirectoryPath + ofFilePath::getFileName(pairingsFile, false);
	newPairings.savePairingFile(newPairingsPath);
	cout << "DONE: " << newPairingsPath << endl;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}