#pragma once

#include "Eigen/Core"
#include "Eigen/SparseCore"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

namespace std {
    bool operator==(const Eigen::Triplet<double> &lhs, const Eigen::Triplet<double> &rhs) {
        if (lhs.row() != rhs.row()) return false;
        if (lhs.col() != rhs.col()) return false;

        constexpr double epsilon = 1e-8;
        if (std::isfinite(lhs.value())) {
            return lhs.value() == doctest::Approx(rhs.value()).epsilon(epsilon);
        }

        // NaN
        if (std::isnan(lhs.value())) {
            return std::isnan(rhs.value());
        }

        // Inf
        return lhs.value() == rhs.value();
    }

    bool operator==(const std::vector<Eigen::Triplet<double>> &lhs, const std::vector<Eigen::Triplet<double>> &rhs) {
        if (lhs.size() != rhs.size()) {
            return false;
        }

        for (uint32_t idx = 0; idx < lhs.size(); ++idx) {
            if (lhs[idx] == rhs[idx]) continue;
            return false;
        }

        return true;
    }
} // namespace std
