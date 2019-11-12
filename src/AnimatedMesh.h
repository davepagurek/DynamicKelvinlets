#include "Mesh.h"
#include "ofxAssimpModelLoader.h"

class AnimatedMesh: public Mesh {
public:
  AnimatedMesh(const ofxAssimpModelLoader& mesh, const function<void(const vector<glm::vec3>&, const vector<glm::vec3>&)>& callback);
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
  
  ofMesh mesh;
  function<void(const vector<glm::vec3>&, const vector<glm::vec3>&)> callback;
  array<vector<glm::vec3>, 3> vertices;
  array<float, 3> times;
  int currentIndex = -1;
};
