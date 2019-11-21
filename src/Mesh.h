#pragma once

#include "ofMain.h"

// Abstract class representing any displaceable mesh
class Mesh {
public:
  virtual void update(float t) = 0;
  virtual void draw() = 0;
  virtual void drawWireframe() = 0;
  virtual vector<glm::vec3>& getVertices() = 0;
  virtual vector<glm::vec3> getOriginalVertices() = 0;
  float mass;
  float getMass() const;
  void setMass(float value);
};
