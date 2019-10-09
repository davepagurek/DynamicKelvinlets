#include "Kelvinlet.h"

#include <algorithm>

glm::vec3 Kelvinlet::displacementRK4(glm::vec3 position, Material material, float t) const {
  glm::vec3 v0 = displacement(position, material, t);
  glm::vec3 v1 = displacement(position + 0.5 * v0, material, t);
  glm::vec3 v2 = displacement(position + 0.5 * v1, material, t);
  glm::vec3 v3 = displacement(position + v2, material, t);

  return (v0 + v1 * 2 + v2 * 2 + v3) / 6;
}

glm::vec3 Kelvinlet::displacement(glm::vec3 position, Material material, float t) const {
  // No influence at all if the impulse hasn't happened yet
  if (t < 0)
    return {0, 0, 0};

  glm::vec3 rVec = (position - center);
  double r = glm::length(rVec);

  auto reg = [&](double v) -> double { return sqrt(v * v + scale * scale); };

  if (r < 1e-4) {
    return 5.0f * t * pow(scale, 4) / (8.0f * M_PI) *
           (1.0f / pow(reg(material.alpha * t), 7) + 2.0f / pow(reg(material.beta * t), 7)) * force;
  }

  auto W = [&](double s, double reg_s) -> double {
    return 1.0f / reg_s * (2 * s * s + scale * scale - 3 * r * s) + 1.0f / pow(reg_s, 3) * r * s * s * s;
  };
  auto dr_W = [&](double s, double reg_s) -> double { return -3.0f * pow(scale, 4) * r / pow(reg_s, 5); };
  auto drr_W = [&](double s, double reg_s) -> double {
    return -3.0f * pow(scale, 4) * (pow(reg_s, 2) - 5 * r * s) / pow(reg_s, 7);
  };

  auto full_W = [&](double s) -> glm::vec3 {
    double reg_s = reg(s);
    return {W(s, reg_s), dr_W(s, reg_s), drr_W(s, reg_s)};
  };

  glm::vec3 W_alpha_plus = full_W(r + material.alpha * t);
  glm::vec3 W_alpha_minus = full_W(r - material.alpha * t);
  glm::vec3 W_beta_plus = full_W(r + material.beta * t);
  glm::vec3 W_beta_minus = full_W(r - material.beta * t);

  auto U = [&](double gamma, double W_plus, double W_minus) -> double {
    return 1.0f / (16.0f * M_PI * gamma * r * r * r) * (W_plus - W_minus);
  };

  double U_alpha = U(material.alpha, W_alpha_plus.x, W_alpha_minus.x);
  double U_beta = U(material.beta, W_beta_plus.x, W_beta_minus.x);

  auto dr_U = [&](double gamma, glm::vec3 W_plus, glm::vec3 W_minus) -> double {
    return 1.0f / (16.0f * M_PI * gamma * r * r * r) * (W_plus.y - W_minus.y - 3.0f / r * (W_plus.x - W_minus.x));
  };

  double dr_U_alpha = dr_U(material.alpha, W_alpha_plus, W_alpha_minus);
  double dr_U_beta = dr_U(material.beta, W_beta_plus, W_beta_minus);

  double A = U_alpha + 2 * U_beta + r * dr_U_beta;
  double B = (dr_U_alpha - dr_U_beta) / r;
  return (A * glm::mat3() + B * glm::outerProduct(rVec, rVec)) * force;
}
