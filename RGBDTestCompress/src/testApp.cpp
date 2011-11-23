#include "testApp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

//--------------------------------------------------------------
void testApp::setup(){
//	ofSetFrameRate(30);
	ofSystemAlertDialog("Load a directory of .xkcd files");
	ofFileDialogResult r = ofSystemLoadDialog("Load uncompressed xkcd directory", true);
	if(r.bSuccess){
		
		ofDirectory dir(r.getPath());
		dir.allowExt("xkcd");
		dir.listDir();
		
		unsigned short* lastbuf = new unsigned short[640*480];
		unsigned short* tempbuf = new unsigned short[640*480];
		string pngDirPath = r.getPath() + "_pngs";
		ofDirectory pngDir = ofDirectory(pngDirPath);
		if(!pngDir.exists()){
			pngDir.create(true);
		}

		ofSystemAlertDialog("Optionally load a pairing file to fix");
		r = ofSystemLoadDialog("Load pairing file to repair", false);
		if(r.bSuccess){
			pairingFileLoaded = true;
			pairingFileName = r.getPath();
			oldPairing.loadPairingFile(pairingFileName);
		}
		else{
			pairingFileLoaded = false;
		}
		
		
		time_t original = 0;
		time_t last = 0; 
		int total;
		int currentOutputFile = 0;
		for(int i = 0; i < dir.numFiles(); i++){
			
			string path = dir.getPath(i);
			memcpy(lastbuf, tempbuf, sizeof(unsigned short)*640*480);
			decoder.readDepthFrame(path, tempbuf);
			
			//check if we have a matching pair for this frame
			int thisDepthFrame = ofToInt(dir.getName(i).substr(dir.getName(i).find(".xkcd")-5,5) );
			if(pairingFileLoaded){
				for(int p = 0; p < oldPairing.getPairs().size(); p++){
					VideoDepthPair pair = oldPairing.getPairs()[p];
					if(pair.depthFrame == thisDepthFrame){
						pair.depthFrame = currentOutputFile;
						newPairing.addAlignedPair(pair);
						cout << "CONVERTED PAIR: " << thisDepthFrame << " to " << pair.depthFrame << endl;
						break;
					}
				}
			}
			
			//check for duplicate
			if(i != 0){
				bool identical = true;

				for (int p = 0; p < 640*480; p++) {
					if(lastbuf[p] != tempbuf[p]){
						identical = false;
						break;
					}
				}
				
				if(identical) continue;
			}
			
			char framestr[1024];
			sprintf(framestr, "%05d", currentOutputFile);
			string newPath = pngDirPath + "/frame__" + string(framestr) + ".png";

			decoder.saveToCompressedPng(newPath, tempbuf);
			currentOutputFile++;
			
			struct stat fileInfo;
			int r  = stat(path.c_str(), &fileInfo);
			time_t t = fileInfo.st_mtimespec.tv_sec;
			if(i == 0){
				original = t;
				last = 0;
			}
			
			if( (t-original) != last){
				cout << "time " << last << " had " << total << " frames " << endl;
				total = 1;
				last = t-original;
			}
			else{
				total++;
			}
			
			//cout << "file " << path << " created at " << ( t-original ) << " " << fileInfo.st_mtimespec.tv_nsec << endl;
			cout << "saved " << path << " to " << newPath << endl;
		}
		if(pairingFileLoaded){
			newPairing.savePairingFile(pairingFileName+".corrected.xml");
		}
	}
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