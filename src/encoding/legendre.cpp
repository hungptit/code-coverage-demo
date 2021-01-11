#include "legendre.hpp"
#include <cmath>
#include <cstdint>
#include <iostream>

namespace encoding {
Legendre::Legendre(const uint32_t order) : order_(order) {
  if (order_ < 1) {
    throw std::logic_error("The order of the Legendre must be greater than 0.");
  }
}

void Legendre::encode(std::vector<Eigen::Triplet<double>> &triplets,
                      uint32_t row, uint32_t col, double x) {
  triplets.emplace_back(row, col, 1.0);
  triplets.emplace_back(row, col + 1, x);
  if (order_ == 1)
    return;

  double p_prev = 1;
  double p_current = x;

  for (uint32_t n = 2; n <= order_; ++n) {
    double p_next = 0.0;
    if (std::isfinite(p_current)) {
      const double k = 1 / static_cast<double>(n);
      p_next = (2 - k) * x * p_current - (1 - k) * p_prev;
    } else {
      p_next = x * p_current;
    }
    triplets.emplace_back(row, col + n, p_next);
    p_prev = p_current;
    p_current = p_next;
  }
}
} // namespace encoding
