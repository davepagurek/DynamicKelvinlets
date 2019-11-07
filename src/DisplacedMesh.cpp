#include "DisplacedMesh.h"

constexpr bool USE_SHADER = true;

string slurp(string path) {
  return ofFile(path).readToBuffer().getText();
}

DisplacedMesh::DisplacedMesh(ofMesh mesh, Material material):
  mesh(mesh),
  material(material),
  currentTime(0),
  originalPositions(mesh.getVertices())
{}

void DisplacedMesh::setup() {
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

void DisplacedMesh::update(float elapsedTime) {
  currentTime += elapsedTime;

  // TODO remove kelvinlets with no influence

  if (!USE_SHADER) {
    mesh.getVertices() = originalPositions;

    // Take the superposition of offsets from all applied Kelvinlets
    for (auto& kelvinlet_ts: kelvinlets) {
      const vector<glm::vec3>& offsets = kelvinlet_ts.displacements(material, currentTime);
      for (size_t i = 0; i < mesh.getNumVertices(); i++) {
        mesh.setVertex(i, mesh.getVertex(i) + offsets[i]);
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

void DisplacedMesh::draw() const {
  if (USE_SHADER) shaderStart();
  mesh.draw();
  if (USE_SHADER) shaderEnd();
//  cout << ofGetFrameRate() << endl;
}

void DisplacedMesh::drawWireframe() const {
  if (USE_SHADER) shaderStart();
  mesh.drawWireframe();
  if (USE_SHADER) shaderEnd();
}

glm::vec3 DisplacedMesh::TimeShiftedKelvinlet::displacement(Material material, float t, const glm::vec3& position) const {
  return kelvinlet->displacementRK4(position, material, t - t0);
}

const vector<glm::vec3>& DisplacedMesh::TimeShiftedKelvinlet::displacements(Material material, float t) const {
  static vector<glm::vec3> result;
  result.clear();

  // Apply displacements to every point
  for (auto& point : initialLocations) {
    result.push_back(displacement(material, t, point));
  }

  return result;
}
