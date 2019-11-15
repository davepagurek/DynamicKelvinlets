#ifndef PUSHKELVINLET_H
#define PUSHKELVINLET_H

#include <Kelvinlet.h>

class PushKelvinlet : public Kelvinlet
{
public:
  PushKelvinlet(glm::vec3 Force, glm::vec3 Center, float Scale)
  : Kelvinlet(Force, Center, Scale) {}
  virtual int type() const override { return 1; }
  glm::vec3 displacement(glm::vec3 position, Material material, float t) const override;
};

#endif // PUSHKELVINLET_H
