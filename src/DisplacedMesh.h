#pragma once

#include "ofMain.h"
#include "Kelvinlet.h"
#include "ImpulseKelvinlet.h"
#include "PushKelvinlet.h"

#include "ofxAssimpModelLoader.h"

class DisplacedMesh {
public:
  
  // Abstract class representing any displaceable mesh
  class Mesh {
  public:
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void drawWireframe() = 0;
    virtual vector<glm::vec3>& getVertices() = 0;
    virtual vector<glm::vec3> getOriginalVertices() = 0;
  };
  
  class StaticMesh: public Mesh {
  public:
    StaticMesh(const ofMesh& mesh);
    virtual void update() override;
    virtual void draw() override;
    virtual void drawWireframe() override;
    virtual vector<glm::vec3>& getVertices() override;
    virtual vector<glm::vec3> getOriginalVertices() override;
    
  private:
    ofMesh original;
    ofMesh mesh;
  };
  
  class AnimatedMesh: public Mesh {
  public:
    AnimatedMesh(const ofxAssimpModelLoader& mesh);
    virtual void update() override;
    virtual void draw() override;
    virtual void drawWireframe() override;
    virtual vector<glm::vec3>& getVertices() override;
    virtual vector<glm::vec3> getOriginalVertices() override;
    
  private:
    class TransformFreeScene: public ofxAssimpModelLoader {
    public:
      TransformFreeScene(const ofxAssimpModelLoader& other);
      void update();
    };
    TransformFreeScene original;
    ofMesh mesh;
  };
  
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
    glm::vec3 offset;
    for (auto& k : kelvinlets) {
      offset += k.displacement(material, currentTime, kelvinlet.center);
    }
    // Invert current transform to bring force center into material space
    kelvinlet.center -= offset;
    kelvinlets.push_back(DisplacedMesh::TimeShiftedKelvinlet{
      .kelvinlet=make_shared<KelvinletType>(kelvinlet),
      .t0=currentTime
    });
  }
  
  void draw();
  void drawWireframe();
  
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
  vector<TimeShiftedKelvinlet> kelvinlets;
};
