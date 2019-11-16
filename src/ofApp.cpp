#include "ofApp.h"

constexpr int FRAME_RATE = 30;

constexpr bool SAVE_SCREENSHOTS = false;

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetFrameRate(FRAME_RATE);
  ofMesh mesh;
  mesh.load("dragon_vrip_res4.ply");
  for (auto& vertex : mesh.getVertices()) {
    vertex = vertex * 15;
    vertex.y *= -1;
    vertex.y += 1.5;
  }

  displacedMesh = make_shared<DisplacedMesh>(mesh, Material(15, 0.4));
//  displacedMesh = make_shared<DisplacedMesh>(ofMesh::sphere(10, 28), Material(20, 0.1));
  displacedMesh->setup();
}

//--------------------------------------------------------------
void ofApp::update() {
//  if (ofGetFrameNum() == 15) {
//    displacedMesh->addKelvinlet(PushKelvinlet({15, 0, 0}, {-0.5, 0, 0}, 1));
//  }
  if (ofGetFrameNum() == 0) {
    displacedMesh->addKelvinlet(ImpulseKelvinlet({0, 0, -15}, {0, 0, 0.5}, 1));
  }

  displacedMesh->update(1.0f / FRAME_RATE);
  if (SAVE_SCREENSHOTS) {
    ofSaveScreen(ofToString(ofGetFrameNum()) + ".png");
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofClear(ofColor::white);
  ofEnableDepthTest();

  constexpr float scale = 170;
//  constexpr float scale = 20;

  for (bool outline : {false, true}) {
    ofPushMatrix();
    // Push 1px closer to the camera when drawing the outline so it goes on top of the background
    ofTranslate(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, outline ? 1 : 0);
    ofRotateXDeg(-20);
    ofRotateYDeg(10);
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
