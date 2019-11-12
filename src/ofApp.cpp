#include "ofApp.h"

constexpr int FRAME_RATE = 30;

constexpr bool SAVE_SCREENSHOTS = false;

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetFrameRate(FRAME_RATE);

  ofxAssimpModelLoader loader;
  loader.loadModel("elephant-skinned-baked-27.dae");
  loader.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
  loader.playAllAnimations();
  loader.disableMaterials();
  loader.disableColors();
//  auto mesh = loader.getMesh(loader.getMeshNames().front());
//  for (auto& vertex : mesh.getVertices()) {
//    vertex *= 0.25;
//  }
  
//  ofMesh mesh;
//  mesh.load("dragon_vrip_res4.ply");
//  for (auto& vertex : mesh.getVertices()) {
//    vertex = vertex * 15;
//    vertex.y *= -1;
//    vertex.y += 1.5;
//  }

  displacedMesh = make_shared<DisplacedMesh>(DisplacedMesh::AnimatedMesh(loader, [&](const vector<glm::vec3>& vertices, const vector<glm::vec3>& accelerations) {
    if (ofGetFrameNum() < 3) return;
    
    list<ImpulseKelvinlet> potentialKelvinlets;
    for (size_t i = 0; i < accelerations.size(); ++i) {
      if (glm::length2(accelerations[i]) > 2000) {
        glm::vec3 force = 0.01*accelerations[i];
        potentialKelvinlets.push_back(ImpulseKelvinlet(force, vertices[i], 0.2));
      }
    }
    
    bool changed;
    do {
      changed = false;
      
      for (auto a = potentialKelvinlets.begin(); a != potentialKelvinlets.end(); ++a) {
        for (auto b = next(a); b != potentialKelvinlets.end();) {
          if (glm::distance(a->center, b->center)/(a->scale + b->scale) < 1) {
            // Combine kelvinlets
            float weight = a->scale / (a->scale + b->scale);
            a->scale = max(a->scale, glm::distance(a->center, b->center) + b->scale);
            a->force = a->force + b->force / max(1.f, glm::distance(a->center, b->center));
            a->center = weight * a->center + (1 - weight) * b->center;
            b = potentialKelvinlets.erase(b);
          } else {
            ++b;
          }
        }
      }
    } while (changed);
    
    for (const auto& k : potentialKelvinlets) displacedMesh->addKelvinlet(k);
  }), Material(15, 0.4));
//  displacedMesh = make_shared<DisplacedMesh>(ofMesh::sphere(10, 28), Material(20, 0.1));
  displacedMesh->setup();
}

//--------------------------------------------------------------
void ofApp::update() {
//  loader.update();
//  if (ofGetFrameNum() == 15) {
//    displacedMesh->addKelvinlet(PushKelvinlet({0, -200, 0}, {0, 2, 0}, 1));
//  }
//  if (ofGetFrameNum() == 60+15) {
//    displacedMesh->addKelvinlet(ImpulseKelvinlet({0, 0, -50}, {0, 0, 5}, 1));
//  }

  displacedMesh->update(1.0f / FRAME_RATE);
  if (SAVE_SCREENSHOTS) {
    ofSaveScreen(ofToString(ofGetFrameNum()) + ".png");
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofClear(ofColor::white);
  ofEnableDepthTest();

//  constexpr float scale = 170;
  constexpr float scale = 50;

  for (bool outline : {false, true}) {
    ofPushMatrix();
    // Push 1px closer to the camera when drawing the outline so it goes on top of the background
    ofTranslate(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, outline ? 1 : 0);
//    ofRotateYDeg(-10);
//    ofRotateXDeg(-90);
    ofRotateZDeg(180);
    ofRotateYDeg(-70);
    ofScale(scale);
//    ofScale(0.25);
    ofSetColor(outline ? ofColor::black : ofColor::white);
    if (outline) {
      displacedMesh->drawWireframe();
//      ofSetColor(ofColor::red);
//      displacedMesh->drawForces();
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
