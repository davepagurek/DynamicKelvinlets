#include "ofApp.h"

constexpr unsigned int W = 20;
constexpr unsigned int H = 20;

constexpr bool SAVE_SCREENSHOTS = false;

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetFrameRate(30);

  ofMesh mesh;

  auto coordToIndex = [=](unsigned int x, unsigned int y) { return y * W + x; };

  for (unsigned int x = 0; x < W; x++) {
    for (unsigned int y = 0; y < H; y++) {
      mesh.addVertex(glm::vec3(x, y, 0));
      mesh.addColor(ofColor::black);

      if (x < W - 1 && y < H - 1) {
        mesh.addIndices({coordToIndex(x, y), coordToIndex(x, y + 1), coordToIndex(x + 1, y)});
        mesh.addIndices({coordToIndex(x + 1, y), coordToIndex(x + 1, y + 1), coordToIndex(x, y + 1)});
      }
    }
  }

  displacedMesh = make_shared<DisplacedMesh>(mesh, Material(1, 0.1));
}

//--------------------------------------------------------------
void ofApp::update() {
  if (ofGetFrameNum() == 15) {
    displacedMesh->addKelvinlet( ImpulseKelvinlet({0, 50, 0}, {W / 2, H / 2, 0}, 1));
  }

  displacedMesh->update(ofGetLastFrameTime());
  if (SAVE_SCREENSHOTS) {
    ofSaveScreen(ofToString(ofGetFrameNum())+".png");
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofClear(ofColor::white);

  constexpr float scale = 20;

  ofPushMatrix();
  ofTranslate(WINDOW_WIDTH / 2 - scale * W / 2, WINDOW_HEIGHT / 2 - scale * H / 2);
  ofScale(scale);
  displacedMesh->drawWireframe();
  ofPopMatrix();
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
