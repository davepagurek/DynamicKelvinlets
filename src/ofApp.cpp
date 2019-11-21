#include "ofApp.h"
#include "AnimatedMesh.h"
#include "StaticMesh.h"
#include "constants.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetFrameRate(FRAME_RATE);

  if (USE_STATIC_MESH) {
    ofMesh mesh;
    mesh.load("dragon_vrip_res4.ply");
    for (auto& vertex : mesh.getVertices()) {
      vertex = vertex * 15;
      vertex.y *= -1;
      vertex.y += 1.5;
    }
    displacedMesh = make_shared<DisplacedMesh>(StaticMesh(mesh), Material(15, 0.4));

  } else {

    ofxAssimpModelLoader loader;
    loader.loadModel("stomp-baked.dae");
    loader.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
    loader.playAllAnimations();
    loader.disableMaterials();
    loader.disableColors();

    float mass = 0.001;

    displacedMesh = make_shared<DisplacedMesh>(
        AnimatedMesh(loader, mass, kelvinletGenerator(displacedMesh),
                     AnimatedMesh::LoopType::DISCONTINUOUS),
        Material(15, 0.4));
  }

  displacedMesh->setup();
}

//--------------------------------------------------------------
void ofApp::update() {

  if (USE_STATIC_MESH) {
    if (ofGetFrameNum() == 15) {
      displacedMesh->addKelvinlet(PushKelvinlet({0, -200, 0}, {0, 2, 0}, 1));
    }
    if (ofGetFrameNum() == 60 + 15) {
      displacedMesh->addKelvinlet(ImpulseKelvinlet({0, 0, -50}, {0, 0, 5}, 1));
    }
  }

  displacedMesh->update(1.0f / FRAME_RATE);

  if (SAVE_SCREENSHOTS) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(5) << std::to_string(ofGetFrameNum()) << ".png";
    ofSaveScreen(ss.str());
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofClear(ofColor::white);
  ofEnableDepthTest();

  constexpr float scale = USE_STATIC_MESH ? 100 : 50;

  for (bool outline : {false, true}) {
    ofPushMatrix();
    // Push 1px closer to the camera when drawing the outline so it goes on top of the background
    ofTranslate(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, outline ? 1 : 0);
    if (!USE_STATIC_MESH)
      ofRotateZDeg(180);
    ofRotateYDeg(-70);
    ofScale(scale);
    ofSetColor(outline ? ofColor::black : ofColor::white);
    if (outline) {
      displacedMesh->drawWireframe();

      if (SHOW_KELVINLETS) {
        ofSetColor(ofColor::red);
        displacedMesh->drawForces();
      }
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
