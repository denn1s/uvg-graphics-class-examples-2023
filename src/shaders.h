#pragma once
#include "color.h"
#include "uniform.h"
#include <cmath>
#include <random>


glm::vec3 vertexShader(const glm::vec3& vertex, const Uniform& u) {
  glm::vec4 v = glm::vec4(vertex.x, vertex.y, vertex.z, 1);


  glm::vec4 r = v * u.model * u.view * u.projection * u.viewport;


  return glm::vec3(r.x/r.w, r.y/r.w, r.z/r.w);
};

Color fragmentShader(Fragment fragment) {
  // Initialize a random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 255);

  // Generate random values for the red, green, and blue channels
  int red = dis(gen);
  int green = dis(gen);
  int blue = dis(gen);

  return Color{red, green, blue};
};