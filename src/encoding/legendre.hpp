#pragma once

#include "Eigen/Core"
#include "Eigen/SparseCore"
#include <cstdint>
#include <vector>

namespace encoding {
struct Legendre {
  Legendre() = default;
  explicit Legendre(const uint32_t order);

  void encode(std::vector<Eigen::Triplet<double>> &triplets, uint32_t row,
              uint32_t col, double x);

  // We use quadratic polynomials by default
  const uint32_t order_ = 2;
};

} // namespace encoding
