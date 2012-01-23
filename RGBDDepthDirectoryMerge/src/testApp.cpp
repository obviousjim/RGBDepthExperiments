#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	string directory1, directory2, outputDirectory, pairingsFile1, pairingsFile2;
	ofSystemAlertDialog("Select First Depth Directory");
	ofFileDialogResult r = ofSystemLoadDialog("Load first directory", true);
	if(r.bSuccess){
		directory1 = r.getPath();
	}
	else{
		ofExit(0);
	}

	ofSystemAlertDialog("Select Pairings For First Depth Directory");
	r = ofSystemLoadDialog("Load pairings", false);
	if(r.bSuccess){
		pairingsFile1 = r.getPath();
		cout << "LOADED FIRST PAIRINGS FILE " << pairingsFile1 << endl;
		if(!pairings1.loadPairingFile(pairingsFile1)){
			ofSystemAlertDialog("Pairings file failed");
			ofExit(0);
		}
	}
	else {
		ofExit(0);
	}
	
	ofSystemAlertDialog("Select Second Depth Directory");
	r = ofSystemLoadDialog("Load second directory", true);
	if(r.bSuccess){
		directory2 = r.getPath();
	}
	else{
		ofExit(0);
	}

	ofSystemAlertDialog("Select Pairings For Second Depth Directory");
	r = ofSystemLoadDialog("Load pairings", false);
	if(r.bSuccess){
		pairingsFile2 = r.getPath();
		cout << "LOADED SECOND PAIRINGS FILE " << pairingsFile2 << endl;
		if(!pairings2.loadPairingFile(pairingsFile2)){
			ofSystemAlertDialog("Pairings file failed");
			ofExit(0);
		}
	}
	else {
		ofExit(0);
	}

	ofSystemAlertDialog("Select Output Directory");
	r = ofSystemLoadDialog("Load output directory", true);
	if(r.bSuccess){
		outputDirectory = r.getPath();
		ofDirectory dir1(directory1);
		ofDirectory dir2(directory2);
		dir1.allowExt("png");
		dir2.allowExt("png");
		dir1.listDir();
		dir1.sort();
		dir2.listDir();
		dir2.sort();
		
		for(int i = 0; i < dir1.numFiles(); i++){
			string copyFrom = dir1.getPath(i);
			char newFileName[1024];
			sprintf(newFileName, "frame__%05d.png", i);
			string copyTo = outputDirectory + "/" + string(newFileName);
			cout << "copying " << copyFrom << " to " << copyTo << endl;
			Poco::File fileSrc(copyFrom);
			try{
				fileSrc.copyTo(copyTo);
			}catch (Poco::Exception & except){
				ofLog(OF_LOG_ERROR, "moveFromTo - unable to move to, file must be busy");
			}			
		}		
		int start = dir1.numFiles();
		for(int i = 0; i < dir2.numFiles(); i++){
			string copyFrom = dir2.getPath(i);
			char newFileName[1024];
			sprintf(newFileName, "frame__%05d.png", start + i);
			string copyTo = outputDirectory + "/" + string(newFileName);
			cout << "copying " << copyFrom << " to " << copyTo << endl;
			Poco::File fileSrc(copyFrom);
			try{
				fileSrc.copyTo(copyTo);
			}catch (Poco::Exception & except){
				ofLog(OF_LOG_ERROR, "moveFromTo - unable to move to, file must be busy");
			}
		}
		
		//update pairings
		for(int i = 0; i < pairings1.getPairs().size(); i++){
			VideoDepthPair p;
			p.isTimeBased = false;
			p.videoFrame = pairings1.getPairs()[i].videoFrame;
			p.depthFrame = pairings1.getPairs()[i].depthFrame;
			cout << "PAIRINGS ONE COPY ACROSS" << endl;
			cout << "	VIDEO " << p.videoFrame << " DEPTH " << p.depthFrame << endl;
			newPairings.addAlignedPair(p);
		}
			
		for(int i = 0; i < pairings2.getPairs().size(); i++){
			VideoDepthPair p;
			p.isTimeBased = false;
			p.videoFrame = pairings2.getPairs()[i].videoFrame;
			p.depthFrame = pairings2.getPairs()[i].depthFrame + start;
			cout << "PAIRINGS TWO COPY ACROSS" << endl;
			cout << "	VIDEO " << p.videoFrame << " DEPTH " << p.depthFrame << endl;
			newPairings.addAlignedPair(p);
		}
		string newPairingFile = outputDirectory + "/merged_pairings.xml";
		cout << "saving pairings file to " << newPairingFile << endl;
		newPairings.savePairingFile(newPairingFile);
	}
	else{
		ofExit(0);
	}
	
	
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