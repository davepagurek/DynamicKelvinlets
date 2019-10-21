#include "Kelvinlet.h"

#include <algorithm>

glm::vec3 Kelvinlet::displacementRK4(glm::vec3 position, Material material, float t) const {
  glm::vec3 v0 = displacement(position, material, t);
  glm::vec3 v1 = displacement(position + 0.5 * v0, material, t);
  glm::vec3 v2 = displacement(position + 0.5 * v1, material, t);
  glm::vec3 v3 = displacement(position + v2, material, t);

  return (v0 + v1 * 2 + v2 * 2 + v3) / 6;
}
