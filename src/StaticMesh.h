#pragma once

#include "Mesh.h"

class StaticMesh: public Mesh {
public:
  StaticMesh(const ofMesh& mesh);
  virtual void update(float t) override;
  virtual void draw() override;
  virtual void drawWireframe() override;
  virtual vector<glm::vec3>& getVertices() override;
  virtual vector<glm::vec3> getOriginalVertices() override;
  
private:
  ofMesh original;
  ofMesh mesh;
};
