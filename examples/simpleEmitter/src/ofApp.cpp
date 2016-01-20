#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
  ofBackground(64);
  
  pexIndex = 0;
  
  // Find all particles pex (xml) file
  string path = "particles";
  ofDirectory dir(path);
  dir.allowExt("pex");
  dir.listDir();

  //go through and print out all the paths
  for(int i = 0; i < dir.size(); i++){
    pexNames.push_back(dir.getName(i));
  }

  string filePath = "particles/" + pexNames[pexIndex];
  emitter.setPosition(ofVec2f(300,300));
  if (!emitter.loadFromXml(filePath)) {
    ofLog(OF_LOG_ERROR, "Failed to load emitter definition");
  }
}

//--------------------------------------------------------------
void ofApp::update(){
  emitter.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  
  ofSetColor(255);
  ofDrawBitmapString(pexNames[pexIndex] + " [" + ofToString(emitter.getParticlesCount()) + "/" + ofToString(emitter.getMaxParticles()) + "]", 20, 20);

//  emitter.setPosition(mousePosition);
  emitter.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
  mousePosition = ofVec2f(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  pexIndex = (pexIndex+1) % pexNames.size();
  string filePath = "particles/" + pexNames[pexIndex];
  if (!emitter.loadFromXml(filePath)) {
    ofLog(OF_LOG_ERROR, "Failed to load emitter definition");
  }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

