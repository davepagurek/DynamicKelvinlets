#include "DisplacedMesh.h"

constexpr bool USE_SHADER = true;

string slurp(string path) {
  return ofFile(path).readToBuffer().getText();
}

void DisplacedMesh::setup() {
  if (USE_SHADER) {
    ofShaderSettings settings;
    
    string vertexShaderSource =
      slurp("shaders/header.vert") +
      slurp("shaders/displacement-impulse.vert") +
      slurp("shaders/displacement-push.vert") +
      slurp("shaders/footer.vert");
    string fragmentShaderSource = slurp("shaders/displacement.frag");
    
    settings.shaderSources[GL_VERTEX_SHADER] = vertexShaderSource;
    settings.shaderSources[GL_FRAGMENT_SHADER] = fragmentShaderSource;
    shader.setup(settings);
  }
}

void DisplacedMesh::update(float elapsedTime) {
  currentTime += elapsedTime;
  mesh->update();

  // TODO remove kelvinlets with no influence

  if (!USE_SHADER) {
    mesh->getVertices() = mesh->getOriginalVertices();
    // Take the superposition of offsets from all applied Kelvinlets
    for (auto& kelvinlet_ts: kelvinlets) {
      const vector<glm::vec3>& offsets = kelvinlet_ts.displacements(material, currentTime, mesh->getOriginalVertices());
      for (size_t i = 0; i < mesh->getOriginalVertices().size(); i++) {
        mesh->getVertices()[i] += offsets[i];
      }
    }
  }
}

void DisplacedMesh::shaderStart() const {
  shader.begin();

  shader.setUniform1f("alpha", material.alpha);
  shader.setUniform1f("beta", material.beta);
  shader.setUniform1i("numKelvinlets", kelvinlets.size());
  
  static vector<int> types;
  static vector<float> centers;
  static vector<float> forces;
  static vector<float> times;
  static vector<float> scales;
  
  types.clear();
  centers.clear();
  forces.clear();
  times.clear();
  scales.clear();
  
  for (auto& k : kelvinlets) {
    types.push_back(k.kelvinlet->type());
    for (size_t i : {0, 1, 2}) centers.push_back(k.kelvinlet->center[i]);
    for (size_t i : {0, 1, 2}) forces.push_back(k.kelvinlet->force[i]);
    times.push_back(currentTime - k.t0);
    scales.push_back(k.kelvinlet->scale);
  }
  
  shader.setUniform1iv("kelvinletTypes", types.data(), kelvinlets.size());
  shader.setUniform3fv("kelvinletCenters", centers.data(), kelvinlets.size());
  shader.setUniform3fv("kelvinletForces", forces.data(), kelvinlets.size());
  shader.setUniform1fv("kelvinletTimes", times.data(), kelvinlets.size());
  shader.setUniform1fv("kelvinletScales", scales.data(), kelvinlets.size());
}

void DisplacedMesh::shaderEnd() const {
  shader.end();
}

void DisplacedMesh::draw() {
  if (USE_SHADER) shaderStart();
  mesh->draw();
  if (USE_SHADER) shaderEnd();
//  cout << ofGetFrameRate() << endl;
}

void DisplacedMesh::drawWireframe() {
  if (USE_SHADER) shaderStart();
  mesh->drawWireframe();
  if (USE_SHADER) shaderEnd();
}

glm::vec3 DisplacedMesh::TimeShiftedKelvinlet::displacement(Material material, float t, const glm::vec3& position) const {
  return kelvinlet->displacementRK4(position, material, t - t0);
}

const vector<glm::vec3>& DisplacedMesh::TimeShiftedKelvinlet::displacements(Material material, float t, const vector<glm::vec3>& vertices) const {
  static vector<glm::vec3> result;
  result.clear();

  // Apply displacements to every point
  for (auto& point : vertices) {
    result.push_back(displacement(material, t, point));
  }

  return result;
}


DisplacedMesh::StaticMesh::StaticMesh(const ofMesh& mesh): original(mesh), mesh(mesh) {}
void DisplacedMesh::StaticMesh::update() {}
void DisplacedMesh::StaticMesh::draw() {
  mesh.draw();
}
void DisplacedMesh::StaticMesh::drawWireframe() {
  mesh.drawWireframe();
}
vector<glm::vec3>& DisplacedMesh::StaticMesh::getVertices() {
  return mesh.getVertices();
}
vector<glm::vec3> DisplacedMesh::StaticMesh::getOriginalVertices() {
  return original.getVertices();
}

DisplacedMesh::AnimatedMesh::AnimatedMesh(const ofxAssimpModelLoader& loaderMesh): original(loaderMesh) {
  mesh.addVertices(original.getCurrentAnimatedMesh(0).getVertices());
  mesh.addIndices(original.getCurrentAnimatedMesh(0).getIndices());
}
void DisplacedMesh::AnimatedMesh::update() {
  original.update();
}
void DisplacedMesh::AnimatedMesh::draw() {
  if (USE_SHADER) {
    original.drawFaces();
  } else {
    mesh.draw();
  }
}
void DisplacedMesh::AnimatedMesh::drawWireframe() {
  if (USE_SHADER) {
    original.drawWireframe();
  } else {
    mesh.drawWireframe();
  }
}
vector<glm::vec3>& DisplacedMesh::AnimatedMesh::getVertices() {
  return mesh.getVertices();
}
vector<glm::vec3> DisplacedMesh::AnimatedMesh::getOriginalVertices() {
  return original.getCurrentAnimatedMesh(0).getVertices();
}


DisplacedMesh::AnimatedMesh::TransformFreeScene::TransformFreeScene(const ofxAssimpModelLoader& other): ofxAssimpModelLoader(other) {}
void DisplacedMesh::AnimatedMesh::TransformFreeScene::update() {
  ofxAssimpModelLoader::update();
  modelMeshes[0].matrix.makeIdentityMatrix();
  modelMatrix.makeIdentityMatrix();
}
