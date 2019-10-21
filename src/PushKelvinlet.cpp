#include "PushKelvinlet.h"

glm::vec3 PushKelvinlet::displacement(glm::vec3 position, Material material, float t) const {
  // No influence at all if the impulse hasn't happened yet
  if(t < 0)
    return {0, 0, 0};

  glm::vec3 rVec = (position - center);
  float r = glm::length(rVec);

  auto reg = [&](float v) -> float { return sqrt(v * v + scale * scale); };

  // asymptotic displacement at r->0 (eqn. 13 from paper)
  if (r < 1e-4) {
    float firstTerm =
        (1.0f / (pow(material.alpha, 2))) *
        ((1.0f / pow(scale, 5)) - (1.0f / pow(reg(material.alpha * t), 5)));
    float secondTerm =
        (2.0f / (pow(material.beta, 2))) *
        ((1.0f / pow(scale, 5)) - (1.0f / pow(reg(material.beta * t), 5)));
    return pow(scale, 4) / (8.0f * M_PI) * (firstTerm + secondTerm) * force;
  }

  auto Q = [&](float s, float w) -> float {
    return 2.0f * (pow(r, 3) / reg(r)) - s * w * ((s / reg(s)) + (w / reg(w))) +
           scale * scale * (s - r) * (1.0f / reg(s) - 1.0f / reg(w));
  };

  auto dr_Q = [&](float s, float w) -> float {
    return 2.0f * r * r * (3 / reg(r) - pow(r, 2) / pow(reg(r), 3)) -
           2.0f * r * (s / reg(s) + w / reg(w)) -
           pow(scale, 2) * r * (s / pow(reg(s), 3) + w / (pow(reg(w), 3)));
  };

  auto drr_Q = [&](float s, float w) {
    return 2.0f * (2.0f * r / reg(r) - s / reg(s) - w / reg(w)) +
           pow(scale, 2) * (2.0f * r / pow(reg(r), 3) - s / pow(reg(s), 3) -
                            w / pow(reg(w), 3)) +
           3.0f * r * pow(scale, 4) *
               (2.0f / pow(reg(r), 5) - 1.0f / pow(reg(s), 5) -
                1.0f / pow(reg(w), 5));
  };

  auto full_Q = [&](float s, float w) -> glm::vec3 {
    return {Q(s, w), dr_Q(s, w), drr_Q(s, w)};
  };

  glm::vec3 Q_alpha = full_Q(r + material.alpha * t, r - material.alpha * t);
  glm::vec3 Q_beta = full_Q(r + material.beta * t , r - material.beta * t);

  auto U = [&](float gamma, float Q) -> float {
    return (1.0f / (16.0f * M_PI * gamma * gamma * pow(r, 3))) * Q;
  };

  float U_alpha = U(material.alpha, Q_alpha.x);
  float U_beta = U(material.beta, Q_beta.x);

  auto dr_U = [&](float gamma, glm::vec3 Q) {
    return 1.0f / (16.0f * M_PI * gamma * gamma * pow(r, 3)) *
           (Q.y - (3 / r) * Q.x);
  };

  auto dr_U_alpha = dr_U(material.alpha, Q_alpha);
  auto dr_U_beta = dr_U(material.beta, Q_beta);

  float A = U_alpha + 2 * U_beta + r * dr_U_beta;
  float B = (dr_U_alpha - dr_U_beta) / r;


  return (A * glm::mat3() + B * glm::outerProduct(rVec, rVec)) * force;
}
