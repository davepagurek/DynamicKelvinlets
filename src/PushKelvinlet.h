#ifndef PUSHKELVINLET_H
#define PUSHKELVINLET_H

#include <Kelvinlet.h>

class PushKelvinlet : public Kelvinlet
{
public:
  PushKelvinlet(glm::vec3 Force, glm::vec3 Center, float Scale)
  : Kelvinlet(Force, Center, Scale) {}
  glm::vec3 displacement(glm::vec3 position, Material material, float t) const;
};

#endif // PUSHKELVINLET_H
