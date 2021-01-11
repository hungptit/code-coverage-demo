#include "Eigen/Core"
#include "Eigen/SparseCore"
#include "encoding/legendre.hpp"
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "doctest-helpers.hpp"

TEST_CASE("Invalid order") {
  CHECK_THROWS(encoding::Legendre(0));
  CHECK_NOTHROW(encoding::Legendre(1));
}

TEST_CASE("Basics") {
  std::vector<Eigen::Triplet<double>> triplets;
  constexpr double x = 0.5;
  SUBCASE("order = 1") {
    std::vector<Eigen::Triplet<double>> expected_results = {{0, 1, 1},
                                                            {0, 2, x}};

    encoding::Legendre legendre(1u);
    legendre.encode(triplets, 0, 1, x);
    CHECK_EQ(triplets, expected_results);
  }

  SUBCASE("order = 2") {
    std::vector<Eigen::Triplet<double>> expected_results = {
        {0, 1, 1}, {0, 2, x}, {0, 3, (3 * x * x - 1) / 2}};

    encoding::Legendre legendre(2u);
    legendre.encode(triplets, 0, 1, 0.5);
    CHECK_EQ(triplets, expected_results);
  }

  SUBCASE("inf") {
    const double y = std::ldexp(-1.0, 1000); // y = -2^(1000)
    constexpr double inf = std::numeric_limits<double>::infinity();

    std::vector<Eigen::Triplet<double>> expected_results = {
        {0, 0, 1}, {0, 1, y}, {0, 2, inf}, {0, 3, -inf}};

    encoding::Legendre legendre(3u);
    legendre.encode(triplets, 0, 0, y);
    CHECK_EQ(triplets, expected_results);
  }

  SUBCASE("order = 4") {
    std::vector<Eigen::Triplet<double>> expected_results = {
        {0, 0, 1},
        {0, 1, x},
        {0, 2, (3 * x * x - 1) / 2},
        {0, 3, (5 * x * x * x - 3 * x) / 2},
        {0, 4, (35 * x * x * x * x - 30 * x * x + 3) / 8}};

    encoding::Legendre legendre(4u);
    legendre.encode(triplets, 0, 0, 0.5);
    CHECK_EQ(triplets, expected_results);
  }

  SUBCASE("nan") {
    const double NaN = std::nan("");

    std::vector<Eigen::Triplet<double>> expected_results = {{0, 0, 1},
                                                            {0, 1, NaN}};

    encoding::Legendre legendre(1u);
    legendre.encode(triplets, 0, 0, NaN);
    CHECK_EQ(triplets, expected_results);
  }
}
