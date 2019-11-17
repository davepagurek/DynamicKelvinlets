#include "StaticMesh.h"

StaticMesh::StaticMesh(const ofMesh& mesh): original(mesh), mesh(mesh) {}
void StaticMesh::update(float t) {}
void StaticMesh::draw() {
  mesh.draw();
}
void StaticMesh::drawWireframe() {
  mesh.drawWireframe();
}
vector<glm::vec3>& StaticMesh::getVertices() {
  return mesh.getVertices();
}
vector<glm::vec3> StaticMesh::getOriginalVertices() {
  return original.getVertices();
}
