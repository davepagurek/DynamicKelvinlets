float Impulse_W(float s, float reg_s, float scale, float r) {
  return 1.0 / reg_s * (2 * s * s + scale * scale - 3 * r * s) + 1.0 / pow(reg_s, 3) * r * s * s * s;
}

float Impulse_dr_W(float s, float reg_s, float scale, float r) {
  return -3.0 * pow(scale, 4) * r / pow(reg_s, 5);
}

vec2 Impulse_full_W(float s, float scale, float r) {
  float reg_s = reg(s, scale);
  return vec2(Impulse_W(s, reg_s, scale, r), Impulse_dr_W(s, reg_s, scale, r));
}

float Impulse_U(float gamma, float W_plus, float W_minus, float r) {
  return 1.0 / (16.0 * M_PI * gamma * r * r * r) * (W_plus - W_minus);
}

float Impulse_dr_U(float gamma, vec2 W_plus, vec2 W_minus, float r) {
  return 1.0 / (16.0 * M_PI * gamma * r * r * r) * (W_plus.y - W_minus.y - 3.0 / r * (W_plus.x - W_minus.x));
}

vec3 Impulse_displacement(int i, vec3 p) {
  vec3 rVec = p - kelvinletCenters[i];
  float r = length(rVec);
  
  float t = kelvinletTimes[i];
  vec3 force = kelvinletForces[i];
  float scale = kelvinletScales[i];
  
  if (r < 0.01) {
    return 5.0 * t * pow(scale, 4) / (8.0 * M_PI) *
      (1.0 / pow(reg(alpha * t, scale), 7) + 2.0 / pow(reg(beta * t, scale), 7)) * force;
  }
  
  vec2 W_alpha_plus = Impulse_full_W(r + alpha * t, scale, r);
  vec2 W_alpha_minus = Impulse_full_W(r - alpha * t, scale, r);
  vec2 W_beta_plus = Impulse_full_W(r + beta * t, scale, r);
  vec2 W_beta_minus = Impulse_full_W(r - beta * t, scale, r);
  
  float U_alpha = Impulse_U(alpha, W_alpha_plus.x, W_alpha_minus.x, r);
  float U_beta = Impulse_U(beta, W_beta_plus.x, W_beta_minus.x, r);
  
  float dr_U_alpha = Impulse_dr_U(alpha, W_alpha_plus, W_alpha_minus, r);
  float dr_U_beta = Impulse_dr_U(beta, W_beta_plus, W_beta_minus, r);
  
  float A = U_alpha + 2.0 * U_beta + r * dr_U_beta;
  float B = (dr_U_alpha - dr_U_beta) / r;
  
  return (A * mat3(1.0) + B * outerProduct(rVec, rVec)) * force;
}

vec3 Impulse_displacementRK4(int i) {
  vec3 p = position.xyz;
  int steps = 4;
  
  for (int n = 0; n < steps; n++) {
    vec3 v0 = Impulse_displacement(i, p);
    vec3 v1 = Impulse_displacement(i, p + 0.5 * v0);
    vec3 v2 = Impulse_displacement(i, p + 0.5 * v1);
    vec3 v3 = Impulse_displacement(i, p + v2);
  
    p += (v0 + v1 * 2 + v2 * 2 + v3) / (6 * steps);
  }
  
  return p - position.xyz;
}
