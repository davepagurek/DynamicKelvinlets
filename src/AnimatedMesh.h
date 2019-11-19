#include "Mesh.h"
#include "KelvinletGenerator.h"
#include "ofxAssimpModelLoader.h"

class AnimatedMesh: public Mesh {
public:
  enum LoopType { CONTINUOUS, DISCONTINUOUS };
  
  AnimatedMesh(const ofxAssimpModelLoader& mesh, const KelvinletGenerator& callback, LoopType loopType = LoopType::CONTINUOUS);
  virtual void update(float t) override;
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
  
  bool smoothLoop;
  ofMesh mesh;
  KelvinletGenerator callback;
  array<vector<glm::vec3>, 3> vertices;
  array<float, 3> times;
  int currentIndex = -1;
  int numFrames = 0;
};
