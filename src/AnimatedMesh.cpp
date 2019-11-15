#include "AnimatedMesh.h"
#include "constants.h"

AnimatedMesh::AnimatedMesh(const ofxAssimpModelLoader& loaderMesh, const KelvinletGenerator& callback):
original(loaderMesh),
callback(callback)
{
  mesh.addVertices(original.getCurrentAnimatedMesh(0).getVertices());
  mesh.addIndices(original.getCurrentAnimatedMesh(0).getIndices());
}
void AnimatedMesh::update(float t) {
  original.update();
  if (currentIndex == -1) {
    for (int i = vertices.size()-1; i > 0; --i) {
      times[i] = t - (vertices.size() - i) * 0.01;
      vertices[i] = getOriginalVertices();
    }
  }
  currentIndex = (currentIndex + 1) % vertices.size();
  times[currentIndex] = t;
  vertices[currentIndex] = getOriginalVertices();
  
  int prevIndex = (currentIndex - 1 + vertices.size()) % vertices.size();
  int twicePrevIndex = (currentIndex - 2 + vertices.size()) % vertices.size();
  
  vector<glm::vec3> accelerations(vertices[currentIndex].size());
  for (int i = 0; i < vertices[currentIndex].size(); ++i) {
    auto& current = vertices[currentIndex][i];
    auto& prev = vertices[prevIndex][i];
    auto& twicePrev = vertices[twicePrevIndex][i];
    
    auto currentVelocity = (current - prev)/(times[currentIndex] - times[prevIndex]);
    auto prevVelocity = (prev - twicePrev)/(times[prevIndex] - times[twicePrevIndex]);
    
    accelerations[i] = (currentVelocity - prevVelocity)/(times[currentIndex] - times[prevIndex]);
  }
  
  callback(vertices[currentIndex], accelerations);
}
void AnimatedMesh::draw() {
  if (USE_SHADER) {
    original.drawFaces();
  } else {
    mesh.draw();
  }
}
void AnimatedMesh::drawWireframe() {
  if (USE_SHADER) {
    original.drawWireframe();
  } else {
    mesh.drawWireframe();
  }
}
vector<glm::vec3>& AnimatedMesh::getVertices() {
  return mesh.getVertices();
}
vector<glm::vec3> AnimatedMesh::getOriginalVertices() {
  return original.getCurrentAnimatedMesh(0).getVertices();
}


AnimatedMesh::TransformFreeScene::TransformFreeScene(const ofxAssimpModelLoader& other): ofxAssimpModelLoader(other) {}
void AnimatedMesh::TransformFreeScene::update() {
  ofxAssimpModelLoader::update();
  modelMeshes[0].matrix.makeIdentityMatrix();
  modelMatrix.makeIdentityMatrix();
}
