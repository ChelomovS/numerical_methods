#ifndef RNG_VALIDATION_HPP
#define RNG_VALIDATION_HPP

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <vector>

namespace rng {

class WeylGenerator {
public:
  explicit WeylGenerator(double seed = 0.5) : state(seed) {}

  uint32_t operator()() {
    state = std::fmod(state + Alpha, 1.0);
    return static_cast<uint32_t>(state * std::numeric_limits<uint32_t>::max());
  }

  double nextDouble() {
    state = std::fmod(state + Alpha, 1.0);
    return state;
  }

private:
  double state;
  const double Alpha = 0.6180339887498948482;
};

inline double chiSquaredTest(const std::vector<double> &sample,
                             size_t bins = 100) {
  if (sample.empty())
    return 0.0;

  std::vector<int> observed(bins, 0);
  for (double x : sample) {
    size_t bin = std::min(static_cast<size_t>(x * bins), bins - 1);
    ++observed[bin];
  }

  double expected = static_cast<double>(sample.size()) / bins;
  double chi2 = 0.0;
  for (int count : observed) {
    double diff = static_cast<double>(count) - expected;
    chi2 += (diff * diff) / expected;
  }
  return chi2;
}

inline double ksTest(std::vector<double> sample) {
  if (sample.empty())
    return 0.0;

  std::sort(sample.begin(), sample.end());
  double maxDiff = 0.0;
  double n = static_cast<double>(sample.size());

  for (size_t i = 0; i < sample.size(); ++i) {
    double empiricalCdfMinus = static_cast<double>(i) / n;
    double empiricalCdfPlus = static_cast<double>(i + 1) / n;
    double theoreticalCdf = sample[i];

    double diff1 = std::abs(empiricalCdfMinus - theoreticalCdf);
    double diff2 = std::abs(empiricalCdfPlus - theoreticalCdf);
    maxDiff = std::max({maxDiff, diff1, diff2});
  }
  return maxDiff;
}

inline double runsTest(const std::vector<double> &sample) {
  if (sample.size() < 2)
    return 0.0;

  int runs = 1;
  bool up = sample[1] > sample[0];
  for (size_t i = 2; i < sample.size(); ++i) {
    if ((sample[i] > sample[i - 1]) != up) {
      runs++;
      up = !up;
    }
  }

  double n = static_cast<double>(sample.size());
  double expectedRuns = (2.0 * n - 1.0) / 3.0;
  double variance = (16.0 * n - 29.0) / 90.0;

  return std::abs((static_cast<double>(runs) - expectedRuns) /
                  std::sqrt(variance));
}

inline double bitwiseAutocorrelation(const std::vector<uint32_t> &sample,
                                     size_t lag = 1) {
  size_t n = sample.size();
  if (n <= lag)
    return 0.0;

  size_t bitMatches = 0;
  size_t totalBits = (n - lag) * 32;

  for (size_t i = 0; i < n - lag; ++i) {
    uint32_t xorRes = sample[i] ^ sample[i + lag];
    bitMatches += (32 - std::popcount(xorRes));
  }

  double matchRatio = static_cast<double>(bitMatches) / totalBits;
  return std::abs(matchRatio - 0.5);
}

} // namespace rng

#endif