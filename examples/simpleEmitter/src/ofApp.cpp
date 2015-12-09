#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
  ofBackground(0);
  
  pexIndex = 0;
  pexNames.push_back("drugs");
  pexNames.push_back("comet");
  pexNames.push_back("fire-fall");
  pexNames.push_back("fire-camp");
  pexNames.push_back("the-eye");

  string filePath = "particles/" + pexNames[pexIndex] + ".pex";
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
  emitter.setPosition(mousePosition);
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
  string filePath = "particles/" + pexNames[pexIndex] + ".pex";
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

