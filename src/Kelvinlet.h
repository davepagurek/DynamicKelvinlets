#pragma once

#include "ofMain.h"
#include "Material.h"

struct Kelvinlet {
  glm::vec3 force;
  glm::vec3 center;
  float scale;
  
  glm::vec3 displacementRK4(glm::vec3 position, Material material, float t) const;
  
private:
  glm::vec3 displacement(glm::vec3 position, Material material, float t) const;
};
