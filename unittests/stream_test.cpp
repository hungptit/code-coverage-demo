#include "Eigen/Core"
#include "Eigen/SparseCore"
#include "encoding/legendre.hpp"
#include "stream.hpp"
#include <cmath>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "doctest-helpers.hpp"

TEST_CASE("Basic tests") {
  std::vector<Eigen::Triplet<double>> triplets;
  constexpr double x = 0.5;
  encoding::Legendre legendre(2u);

  SUBCASE("Finite values") {
    legendre.encode(triplets, 0, 1, 0.5);
    std::stringstream ss;
    ss << triplets;
    std::string expected_results = "[{0,1,1},{0,2,0.5},{0,3,-0.125}]";
    CHECK_EQ(ss.str(), expected_results);
  }

  SUBCASE("inf values") {
    constexpr double inf = std::numeric_limits<double>::infinity();
    legendre.encode(triplets, 0, 1, -inf);
    std::stringstream ss;
    ss << triplets;
    std::string expected_results = "[{0,1,1},{0,2,-inf},{0,3,inf}]";
    CHECK_EQ(ss.str(), expected_results);
  }

  SUBCASE("NaN values") {
    const double NaN = std::nan("");
    legendre.encode(triplets, 0, 1, NaN);
    std::stringstream ss;
    ss << triplets;
    std::string expected_results = "[{0,1,1},{0,2,nan},{0,3,nan}]";
    CHECK_EQ(ss.str(), expected_results);
  }
}
