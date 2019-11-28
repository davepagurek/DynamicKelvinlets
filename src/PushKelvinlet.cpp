#define _USE_MATH_DEFINES
#include "PushKelvinlet.h"

glm::vec3 PushKelvinlet::displacement(glm::vec3 position, Material material, float t) const {
  // No influence at all if the impulse hasn't happened yet
  if(t < 0)
    return {0, 0, 0};

  glm::vec3 rVec = (position - center);
  float r = glm::length(rVec);

  // save powers of scale
  double scale_2 = scale * scale;
  double scale_3 = scale_2 * scale;
  double scale_4 = scale_3 * scale;
  double scale_5 = scale_4 * scale;

  auto reg = [&](double v) -> double { return sqrt(v * v + scale_2); };

  // asymptotic displacement at r->0 (eqn. 13 from paper)
  if (r < 1e-4) {
    double firstTerm =
        (1.0f / (material.alpha * material.alpha)) *
        ((1.0f / scale_5) - (1.0f / pow(reg(material.alpha * t), 5)));

    double secondTerm = (2.0f / (material.beta * material.beta)) *
                        ((1.0f / scale_5) -
                         (1.0f / pow(reg(material.beta * t), 5)));

    return (scale_4) / (8.0f * M_PI) *
           (firstTerm + secondTerm) * force;
  }

  auto Q = [&](double s, double w) -> double {
    return 2.0f * ((r * r * r) / reg(r)) -
           s * w * ((s / reg(s)) + (w / reg(w))) +
           scale_2 * (s - r) * (1.0f / reg(s) - 1.0f / reg(w));
  };

  auto dr_Q = [&](double s, double w) -> double {
    return 2.0f * r * r * (3 / reg(r) - (r * r) / pow(reg(r), 3)) -
           2.0f * r * (s / reg(s) + w / reg(w)) -
           (scale_2) * r * (s / pow(reg(s), 3) + w / (pow(reg(w), 3)));
  };

  auto drr_Q = [&](float s, float w) {
    return 2.0f * (2.0f * r / reg(r) - s / reg(s) - w / reg(w)) +
           (scale_2) * (2.0f * r / pow(reg(r), 3) - s / pow(reg(s), 3) -
                              w / pow(reg(w), 3)) +
           3.0f * r * (scale_4) *
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
