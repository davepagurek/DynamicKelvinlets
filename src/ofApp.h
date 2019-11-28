#pragma once

#include "ofMain.h"
#include "DisplacedMesh.h"

constexpr unsigned int WINDOW_WIDTH = 1024;
constexpr unsigned int WINDOW_HEIGHT = 768;

class ofApp : public ofBaseApp {

public:
  void setup();
  void update();
  void draw();

  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseEntered(int x, int y);
  void mouseExited(int x, int y);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);

private:
  shared_ptr<DisplacedMesh> displacedMesh;
  std::vector<ofImage> frames;
};
