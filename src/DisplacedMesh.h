#pragma once

#include "ofMain.h"
#include "Kelvinlet.h"

class DisplacedMesh {
public:
  DisplacedMesh(ofMesh mesh, Material material);
  void update(float elapsedTime);
  void addKelvinlet(Kelvinlet kelvinlet);
  void draw() const;
  void drawWireframe() const;
  
private:
  // Because Kelvinlets measure distances from their centers to points at the time
  // of the force's application, we need to save the location of the mesh's vertices
  // at the time of force application.
  struct TimeShiftedKelvinlet {
    Kelvinlet kelvinlet;
    float t0;
    vector<glm::vec3> initialLocations;
    
    vector<glm::vec3> displacements(Material material, float t) const;
  };
  
  ofMesh mesh;
  Material material;
  float currentTime;
  vector<glm::vec3> originalPositions;
  vector<TimeShiftedKelvinlet> kelvinlets;
};
