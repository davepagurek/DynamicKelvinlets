#pragma once

#include "ofMain.h"
#include "Kelvinlet.h"
#include "ImpulseKelvinlet.h"
#include "PushKelvinlet.h"
#include "Mesh.h"

#include <functional>

#include "ofxAssimpModelLoader.h"

class DisplacedMesh {
public:
  template<typename MeshType>
  DisplacedMesh(const MeshType& mesh, Material material):
    mesh(make_shared<MeshType>(mesh)),
    material(material),
    currentTime(0)
  {}
  
  void setup();
  void update(float elapsedTime);
  
  template <typename KelvinletType>
  void addKelvinlet(KelvinletType kelvinlet) {
//    cout << kelvinlet.center << endl;
    kelvinlets.push_back(DisplacedMesh::TimeShiftedKelvinlet{
      .kelvinlet=make_shared<KelvinletType>(kelvinlet),
      .t0=currentTime
    });
  }
  
  void draw();
  void drawWireframe();
  void drawForces();

  shared_ptr<Mesh> getMesh() const;
  void setMesh(const shared_ptr<Mesh> &value);

private:
  // Because Kelvinlets measure distances from their centers to points at the time
  // of the force's application, we need to save the location of the mesh's vertices
  // at the time of force application.
  struct TimeShiftedKelvinlet {
    const shared_ptr<Kelvinlet> kelvinlet;
    float t0;
    
    glm::vec3 displacement(Material material, float t, const glm::vec3& position) const;
    const vector<glm::vec3>& displacements(Material material, float t, const vector<glm::vec3>& vertices) const;
  };
  
  ofShader shader;
  void shaderStart() const;
  void shaderEnd() const;
  
  shared_ptr<Mesh> mesh;
  Material material;
  float currentTime;
  list<TimeShiftedKelvinlet> kelvinlets;
  ofMesh forceMesh;
};
