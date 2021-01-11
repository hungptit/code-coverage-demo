#include "Eigen/Core"
#include "Eigen/SparseCore"
#include <cstdint>
#include <iostream>
#include <ostream>

namespace std {
ostream &operator<<(ostream &os, const Eigen::Triplet<double> &item) {
  os << "{" << item.row() << "," << item.col() << "," << item.value() << "}";
  return os;
}

ostream &operator<<(ostream &os,
                    const std::vector<Eigen::Triplet<double>> &triplets) {
  uint32_t count = 0;
  os << "[";
  for (uint32_t idx = 0; idx < triplets.size(); ++idx) {
    os << (count++ == 0 ? "" : ",") << triplets[idx];
  }
  os << "]";
  return os;
}

} // namespace std
