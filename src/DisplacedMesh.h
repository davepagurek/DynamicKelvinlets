#pragma once

#include "ofMain.h"
#include "Kelvinlet.h"
#include "ImpulseKelvinlet.h"
#include "PushKelvinlet.h"

class DisplacedMesh {
public:
  DisplacedMesh(ofMesh mesh, Material material);
  void setup();
  void update(float elapsedTime);
  
  template <typename KelvinletType>
  void addKelvinlet(KelvinletType kelvinlet) {
    glm::vec3 offset;
    for (auto& k : kelvinlets) {
      offset += k.displacement(material, currentTime, kelvinlet.center);
    }
    // Invert current transform to bring force center into material space
    kelvinlet.center -= offset;
    kelvinlets.push_back(DisplacedMesh::TimeShiftedKelvinlet{
      .kelvinlet=make_shared<KelvinletType>(kelvinlet),
      .t0=currentTime,
      .initialLocations=mesh.getVertices()
    });
  }
  
  void draw() const;
  void drawWireframe() const;
  
private:
  // Because Kelvinlets measure distances from their centers to points at the time
  // of the force's application, we need to save the location of the mesh's vertices
  // at the time of force application.
  struct TimeShiftedKelvinlet {
    const shared_ptr<Kelvinlet> kelvinlet;
    float t0;
    vector<glm::vec3> initialLocations;
    
    glm::vec3 displacement(Material material, float t, const glm::vec3& position) const;
    const vector<glm::vec3>& displacements(Material material, float t) const;
  };
  
  ofShader shader;
  void shaderStart() const;
  void shaderEnd() const;
  
  ofMesh mesh;
  Material material;
  float currentTime;
  vector<glm::vec3> originalPositions;
  vector<TimeShiftedKelvinlet> kelvinlets;
};
