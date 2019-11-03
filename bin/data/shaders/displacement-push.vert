float Push_Q(float s, float w, float r, float scale) {
  return 2.0 * (pow(r, 3) / reg(r, scale)) - s * w * ((s / reg(s, scale)) + (w / reg(w, scale))) +
         scale * scale * (s - r) * (1.0 / reg(s, scale) - 1.0 / reg(w, scale));
}

float Push_dr_Q(float s, float w, float r, float scale) {
  return 2.0 * r * r * (3 / reg(r, scale) - pow(r, 2) / pow(reg(r, scale), 3)) -
         2.0 * r * (s / reg(s, scale) + w / reg(w, scale)) -
         pow(scale, 2) * r * (s / pow(reg(s, scale), 3) + w / (pow(reg(w, scale), 3)));
}

vec2 Push_full_Q(float s, float w, float r, float scale) {
  return vec2(Push_Q(s, w, r, scale), Push_dr_Q(s, w, r, scale));
}

float Push_U(float gamma, float Q, float r) {
  return (1.0 / (16.0 * M_PI * gamma * gamma * pow(r, 3))) * Q;
}

float Push_dr_U(float gamma, vec2 Q, float r) {
  return 1.0 / (16.0 * M_PI * gamma * gamma * pow(r, 3)) *
         (Q.y - (3 / r) * Q.x);
}

vec3 Push_displacement(int i, vec3 p) {
  vec3 rVec = p - kelvinletCenters[i];
  float r = length(rVec);

  float t = kelvinletTimes[i];
  vec3 force = kelvinletForces[i];
  float scale = kelvinletScales[i];

  if (r < 0.01) {
    float firstTerm =
        (1.0 / (pow(alpha, 2))) *
        ((1.0 / pow(scale, 5)) - (1.0 / pow(reg(alpha * t, scale), 5)));
    float secondTerm =
        (2.0 / (pow(beta, 2))) *
        ((1.0 / pow(scale, 5)) - (1.0 / pow(reg(beta * t, scale), 5)));
    return pow(scale, 4) / (8.0 * M_PI) * (firstTerm + secondTerm) * force;
  }

  vec2 Q_alpha = Push_full_Q(r + alpha * t, r - alpha * t, r, scale);
  vec2 Q_beta = Push_full_Q(r + beta * t, r - beta * t, r, scale);

  float U_alpha = Push_U(alpha, Q_alpha.x, r);
  float U_beta = Push_U(beta, Q_beta.x, r);

  float dr_U_alpha = Push_dr_U(alpha, Q_alpha, r);
  float dr_U_beta = Push_dr_U(beta, Q_beta, r);

  float A = U_alpha +2 * U_beta + r * dr_U_beta;
  float B = (dr_U_alpha - dr_U_beta) / r;

  return (A * mat3(1.0) + B * outerProduct(rVec, rVec)) * force;
}

vec3 Push_displacementRK4(int i) {
  vec3 p = position.xyz;
  int steps = 4;
  
  for (int n = 0; n < steps; n++) {
    vec3 v0 = Push_displacement(i, p);
    vec3 v1 = Push_displacement(i, p + 0.5 * v0);
    vec3 v2 = Push_displacement(i, p + 0.5 * v1);
    vec3 v3 = Push_displacement(i, p + v2);
    
    p += (v0 + v1 * 2 + v2 * 2 + v3) / (6 * steps);
  }
  
  return p - position.xyz;
}
