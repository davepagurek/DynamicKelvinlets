#include "ofApp.h"

constexpr unsigned int W = 20;
constexpr unsigned int H = 20;

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetFrameRate(30);

  displacedMesh = make_shared<DisplacedMesh>(ofMesh::sphere(10, 14), Material(20, 0.4));
}

//--------------------------------------------------------------
void ofApp::update() {
  if (ofGetFrameNum() == 15) {
    displacedMesh->addKelvinlet(Kelvinlet{.force = {0, 0, -200}, .center = {0, 0, 10}, .scale = 1});
  }

  displacedMesh->update(ofGetLastFrameTime());
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofClear(ofColor::white);
  ofEnableDepthTest();

  constexpr float scale = 20;

  for (bool outline : {false, true}) {
    ofPushMatrix();
    // Push 1px closer to the camera when drawing the outline so it goes on top of the background
    ofTranslate(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, outline ? 1 : 0);
    ofRotateXDeg(-20);
    ofRotateYDeg(20);
    ofScale(scale);
    ofSetColor(outline ? ofColor::black : ofColor::white);
    if (outline) {
      displacedMesh->drawWireframe();
    } else {
      displacedMesh->draw();
    };
    ofPopMatrix();
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}
