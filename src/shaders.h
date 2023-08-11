#pragma once
#include "color.h"
#include "uniform.h"
#include <cmath>
#include <random>


glm::vec3 vertexShader(const glm::vec3& vertex, const Uniform& u) {
  glm::vec4 v = glm::vec4(vertex.x, vertex.y, vertex.z, 1);


  glm::vec4 r = u.viewport * u.projection * u.view * u.model * v;


  return glm::vec3(r.x/r.w, r.y/r.w, r.z/r.w);
};

Color fragmentShader(Fragment fragment) {
  return Color{255, 0, 0};
};