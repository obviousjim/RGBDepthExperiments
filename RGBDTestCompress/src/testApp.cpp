#include "testApp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(30);
	
	ofFileDialogResult r = ofSystemLoadDialog("Load uncompressed xkcd directory", true);
	if(r.bSuccess){
		ofDirectory dir(r.getPath());
		dir.allowExt("xkcd");
		dir.listDir();
		unsigned short* tempbuf = new unsigned short[640*480];
		string pngDirPath = r.getPath() + "_pngs";
		ofDirectory pngDir = ofDirectory(pngDirPath);
		if(!pngDir.exists()){
			pngDir.create(true);
		}
		
		for(int i = 0; i < dir.numFiles(); i++){
			string path = dir.getPath(i);
			decoder.readDepthFrame(path, tempbuf);
			string newPath = pngDirPath + "/" + ofFilePath::getBaseName(path) + ".png";
			decoder.saveToCompressedPng(newPath, tempbuf);
			cout << "saved " << path << " to " << newPath << endl;
		}
	}
	
	/*
	rawPix = decoder.readDepthFrame("testframe.xkcd");
	preview = decoder.readDepthFrametoImage("testframe.xkcd");

	pngPixs = new unsigned char[640*480*3];
	for(int i = 0; i < 640*480; i++){
		pngPixs[i*3+0] = rawPix[i] >> 8;
		pngPixs[i*3+1] = rawPix[i];
		pngPixs[i*3+2] = 0;
	}
	
	compressedPreview.setFromPixels(pngPixs, 640,480, OF_IMAGE_COLOR);
	compressedPreview.saveImage("TestCompress.png");
	reloaded.loadImage("TestCompress.png");
	reloadedPix = reloaded.getPixels();
	decompressedPix = new unsigned short[640*480];
	int totalDif = 0;
	for(int i = 0; i < 640*480; i++){
		decompressedPix[i] = (reloadedPix[i*3] << 8) | reloadedPix[i*3+1];;		
		totalDif += abs(decompressedPix[i] - rawPix[i]);
	}
	
	cout << "total difference is " << totalDif << endl;
	recomposedPreview = decoder.convertTo8BitImage(decompressedPix);

	
	struct stat fileInfo;
	int r  = stat(ofToDataPath("testframe.xkcd").c_str(), &fileInfo);
	// tim: dunno if that's the best way to deal with a possible error
	if (r!=0){
		ofLog(OF_LOG_ERROR, "Error while trying to read file stats");
	}
 
	time_t t = fileInfo.st_mtimespec.tv_sec;
	cout << "created at " << ofToString( t ) << endl;
	
#ifdef OF_TARGET_WIN32
//	return fileInfo.st_mtime.tv_sec; // spec.tv_sec;
#elseif
//	return fileInfo.st_mtimespec.tv_sec;
#endif
	 */
	
	
}


//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
//	preview.draw(0,0);
//	compressedPreview.draw(640, 0);
//	recomposedPreview.draw(0, 480);
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