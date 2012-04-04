#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	exporting = false;
	clip = 2500;
	lastClip = 0;
	
	depthDirectoryLoaded = false;
	
	ofSystemAlertDialog("Select Depth Directory");
	ofFileDialogResult r = ofSystemLoadDialog("Load Depth Directory", true);
	if(r.bSuccess){
		inputDirectory = ofDirectory(r.getPath());
		inputDirectory.allowExt("png");
		depthDirectoryLoaded = (inputDirectory.listDir() > 0);
		if(depthDirectoryLoaded){
			string testFramePath = inputDirectory.getPath(inputDirectory.numFiles()/2);
			testFramePNGRaw.loadImage(testFramePath);
			testFrameUncompressed.allocate(640,480, OF_IMAGE_GRAYSCALE);
			testFrameUncompressedTresholded.allocate(640,480, OF_IMAGE_GRAYSCALE);
			compressor.readCompressedPng(testFramePath, testFrameUncompressed.getPixels());
		}
	}
}

//--------------------------------------------------------------
void testApp::update(){
	
	if(!depthDirectoryLoaded){
		return;
	}
		
	if(!exporting){
		if(lastClip != clip){
			unsigned short* src = testFrameUncompressed.getPixels();
			unsigned short* dst = testFrameUncompressedTresholded.getPixels();
			for(int i = 0; i < 640*480; i++){
				if(src[i] < clip){
					dst[i] = src[i];
				}
				else{
					dst[i] = 0;
				}
			}
			lastClip  = clip;
		}
		
		testFrame8Bit = compressor.convertTo8BitImage(testFrameUncompressedTresholded.getPixels());
	}
	else{
		if(currentExportFrame >= inputDirectory.numFiles()){
			exporting = false;
		}
		else {
			//grab newest file
			string framepath = inputDirectory.getPath(currentExportFrame);
			testFramePNGRaw.loadImage(framepath);
			compressor.readCompressedPng(framepath, testFrameUncompressed.getPixels());

			//perform threshold
			unsigned short* src = testFrameUncompressed.getPixels();
			unsigned short* dst = testFrameUncompressedTresholded.getPixels();
			for(int i = 0; i < 640*480; i++){
				if(src[i] < clip){
					dst[i] = src[i];
				}
				else{
					dst[i] = 0;
				}
			}
			//render it for preview
			testFrame8Bit = compressor.convertTo8BitImage(testFrameUncompressedTresholded.getPixels());
			compressor.saveToCompressedPng(exportDirectory + "/" + ofFilePath::getFileName(framepath), testFrameUncompressedTresholded.getPixels());
			//save file
			currentExportFrame++;
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	if(!depthDirectoryLoaded){
		ofPushStyle();
		ofSetColor(255);
		ofDrawBitmapString("No directory loaded.", ofPoint(30, 30));
		ofPopStyle();
		return;
	}
	
	testFramePNGRaw.draw(0, 0);
	if(testFrame8Bit.isAllocated()){
		testFrame8Bit.draw(640, 0);
	}
	
	
	ofPushStyle();
	ofSetColor(255);
	ofDrawBitmapString("Current Threshold: " + ofToString(clip), ofPoint(30, 670));
	if(exporting){
		ofDrawBitmapString("exporting: " + ofToString(currentExportFrame) + "/" +  ofToString(inputDirectory.numFiles()) + "\nPress SHIFT+C to cancel", ofPoint(30, 690));
		
	}
	else{
		ofDrawBitmapString("Click + Drag to change thresh\nHit spacebar to export the sequence.", ofPoint(30, 690));
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		ofFileDialogResult r =  ofSystemLoadDialog("Select Output Folder", true);
		if(r.bSuccess){
			exportDirectory = r.getPath();
			currentExportFrame = 0;
			exporting = true;
		}
	}
	if(exporting && key == 'C'){
		exporting = false;
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

	clip = ofMap(x, 0, ofGetWidth(), 400, 5000, true);
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