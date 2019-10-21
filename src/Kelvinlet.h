#pragma once

#include "ofMain.h"
#include "Material.h"

class Kelvinlet {
protected:
  glm::vec3 force;
  glm::vec3 center;
  float scale;
  virtual glm::vec3 displacement(glm::vec3 position, Material material, float t) const = 0;
public:
  Kelvinlet(glm::vec3 Force, glm::vec3 Center, float Scale)
      : force(Force), center(Center), scale(Scale) {}
  glm::vec3 displacementRK4(glm::vec3 position, Material material, float t) const;
};
