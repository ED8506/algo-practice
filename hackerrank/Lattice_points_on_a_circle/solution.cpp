#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <bits/stdc++.h>

using namespace std;

namespace {
using Int = long long;
template <typename T> using Box = forward_list<T>;

pair<vector<Int>, unordered_map<Int, Int>> primes_up_to(const Int x) {
    assert(x > 0);
    vector<bool> prime(x + 1, true);
    prime[0] = false;
    prime[1] = false;

    unordered_map<Int, Int> min_prime_factors_by_compound;

    for (Int i = 2; i <= x;) {
        for (Int k = 2; i * k <= x; ++k) {
            prime[i * k] = false;

            if (!min_prime_factors_by_compound.count(i * k)) {
                min_prime_factors_by_compound.insert({i * k, i});
            }
        }

        do {
            ++i;
        } while (i <= x && !prime[i]);
    }

    vector<Int> ans;

    for (Int i = 2; i != static_cast<Int>(prime.size()); ++i) {
        if (prime.at(i)) ans.push_back(i);
    }

    return {ans, min_prime_factors_by_compound};
}

void keep_c1m4(vector<Int> &primes) {
    const auto initial_size = primes.size();

    primes.erase(remove_if(primes.begin(), primes.end(),
                           [](const Int p) { return p % 4 != 1; }),
                 primes.end());

    assert(primes.size() <= initial_size);
}

bool has_only_c3m4_or_2_as_prime_factors(
    const unordered_map<Int, Int> &min_prime_factors_by_compound, Int x) {

    const auto ok = [](const Int p) -> bool { return p == 2 || p % 4 == 3; };

    while (min_prime_factors_by_compound.count(x)) {
        const Int p = min_prime_factors_by_compound.at(x);
        if (!ok(p)) return false;

        x = x / p;
    }

    return ok(x);
}

vector<Int> factorize(unordered_map<Int, Int> min_prime_factors_by_compound,
                      Int m) {
    vector<Int> ans;

    while (min_prime_factors_by_compound.count(m)) {
        const Int p = min_prime_factors_by_compound.at(m);

        if (ans.size()) {
            assert(p != ans.back());
        }

        ans.push_back(p);
        m /= p;
    }

    if (ans.size()) {
        assert(m != ans.back());
    }

    if (m > 1) ans.push_back(m);

    return ans;
}

Box<Int> exp_ltd(const Int max_val, const Int base, const Int exponent) {
    Int ans = 1;

    for (Int i = 0; i != exponent; ++i) {
        ans *= base;
        if (ans > max_val) return Box<Int>{};
    }

    return Box<Int>{ans};
}

vector<Int> basis_powers(vector<Int> factorization) {
    transform(factorization.begin(), factorization.end(), factorization.begin(),
              [](const Int p) {
                  assert(p % 2 && p > 1);
                  return (p - 1) / 2;
              });

    return factorization;
}

vector<size_t> first_subvector_indices(const size_t sz) {
    vector<size_t> ans(sz);
    iota(ans.begin(), ans.end(), 0U);
    return ans;
}

Box<vector<size_t>> next_subvector_indices(const size_t sz,
                                           vector<size_t> indices) {
    assert(indices.size() > 0);
    Box<size_t> rightmost_not_max;

    for (auto i = indices.size(); i-- != 0U;) {
        assert(indices.at(i) < sz);
        const auto max_here = sz - 1 - (indices.size() - 1 - i);

        if (indices.at(i) != max_here) {
            rightmost_not_max = {i};
            break;
        }
    }

    if (rightmost_not_max.empty()) return {};

    iota(indices.begin() + rightmost_not_max.front(), indices.end(),
         indices[rightmost_not_max.front()] + 1);

    return {indices};
}

vector<Int> subvector(const vector<Int> &xs, const vector<size_t> &indices) {
    vector<Int> ans(indices.size());

    transform(indices.cbegin(), indices.cend(), ans.begin(),
              [&xs](const size_t i) {
                  assert(i < xs.size());
                  return xs.at(i);
              });

    return ans;
}

Box<Int> power_combination(const Int N, const vector<Int> &bases,
                           const vector<Int> &pows) {
    assert(bases.size() == pows.size());

    vector<Box<Int>> exps(bases.size());

    transform(bases.cbegin(), bases.cend(), pows.cbegin(), exps.begin(),
              [N](const Int b, const Int e) { return exp_ltd(N, b, e); });

    return accumulate(exps.cbegin(), exps.cend(), Box<Int>{1},
                      [N](const auto agg, const auto x) -> Box<Int> {
                          if (agg.empty() || x.empty() ||
                              (agg.front() * x.front() > N)) {
                              return Box<Int>{};
                          }

                          return Box<Int>{agg.front() * x.front()};
                      });
}

vector<Int> power_combinations(const Int N, vector<Int> bases,
                               const vector<Int> &pows) {
    sort(bases.begin(), bases.end());
    vector<Int> ans;

    do {
        const auto candidate = power_combination(N, bases, pows);
        if (!candidate.empty()) ans.push_back(candidate.front());
    } while (next_permutation(bases.begin(), bases.end()));

    return ans;
}

vector<Int> basis_solutions(const Int N, const vector<Int> &c1m4_primes,
                            const vector<Int> &pows) {
    vector<Int> ans;

    for (auto indices =
             Box<vector<size_t>>{first_subvector_indices(pows.size())};
         !indices.empty(); indices = next_subvector_indices(c1m4_primes.size(),
                                                            indices.front())) {
        const auto new_tail = power_combinations(
            N, subvector(c1m4_primes, indices.front()), pows);

        if (new_tail.empty()) break;

        ans.insert(ans.end(), new_tail.cbegin(), new_tail.cend());
    }

    sort(ans.begin(), ans.end());
    return ans;
}

Int map_sum_solutions(const Int N, const Int m, const function<Int(Int)> map) {
    static auto result_pair = primes_up_to(400LL * 1000LL * 1000LL);
    auto primes = result_pair.first;
    auto min_pf = result_pair.second;

    keep_c1m4(primes);

    const auto m_factors = factorize(min_pf, m);

    auto basis = m > 1 ? basis_solutions(N, primes, basis_powers(m_factors))
                       : vector<Int>{1};

    const Int max_multiplier = N / basis.front();
    vector<Int> multipliers(max_multiplier - 2 + 1);

    iota(multipliers.begin(), multipliers.end(), 2);

    multipliers.erase(remove_if(multipliers.begin(), multipliers.end(),
                                [&min_pf = min_pf](const Int m) {
                                    return !has_only_c3m4_or_2_as_prime_factors(
                                        min_pf, m);
                                }),
                      multipliers.end());

    multipliers.insert(multipliers.cbegin(), 1);

    Int ans = 0;

    for (const Int b : basis) {
        for (const Int m : multipliers) {
            assert(b * m > 0);

            if (b * m <= N) {
                ans += map(b * m);
                assert(ans > 0LL);
            } else {
                break;
            }
        }
    }

    return ans;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
Int count_solutoins(const Int N, const Int m) {
    return map_sum_solutions(N, m, [](const Int x) { return 1; });
}
#pragma GCC diagnostic pop

Int sum_solutoins(const Int N, const Int m) {
    return map_sum_solutions(N, m, [](const Int x) { return x; });
}
} // namespace

TEST_CASE("factorize") {
    SECTION("2") {
        REQUIRE(factorize(primes_up_to(13).second, 2) == vector<Int>{2});
    }

    SECTION("3") {
        REQUIRE(factorize(primes_up_to(13).second, 3) == vector<Int>{3});
    }

    SECTION("105") {
        REQUIRE(factorize(primes_up_to(105).second, 105) ==
                vector<Int>{3, 5, 7});
    }
}

TEST_CASE("power_combination") {
    SECTION("degenerate") {
        const auto actual = power_combination(100, {}, {});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == 1);
    }

    SECTION("underflow") {
        const auto actual = power_combination(100, {2, 3}, {1, 2});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == 18);
    }

    SECTION("overflow") {
        const auto actual = power_combination(17, {2, 3}, {1, 2});
        REQUIRE(actual.empty());
    }
}

TEST_CASE("power_combinations") {
    SECTION("size 2") {
        REQUIRE(power_combinations(9999, {1, 2}, {1, 2}) == vector<Int>{4, 2});
    }

    SECTION("size 3") {
        REQUIRE(power_combinations(9999, {1, 2, 3}, {0, 1, 2}) ==
                vector<Int>{18, 12, 9, 3, 4, 2});
    }
}

TEST_CASE("basis_solutions") {
    SECTION("13 3") {
        auto result_pair = primes_up_to(13);
        auto primes = result_pair.first;
        keep_c1m4(primes);

        const auto actual = basis_solutions(13, primes, {1});
        REQUIRE(actual == vector<Int>{5, 13});
    }
}

TEST_CASE("first_subvector_indices for size 3") {
    REQUIRE(first_subvector_indices(3) == vector<size_t>{0, 1, 2});
}

TEST_CASE("subvector typical") {
    REQUIRE(subvector({2, 4, 8}, {0, 2}) == vector<Int>{2, 8});
}

TEST_CASE("next_subvector_indices triple for the vector size of 4") {
    SECTION("first step") {
        const auto actual = next_subvector_indices(4, {0, 1, 2});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == vector<size_t>{0, 1, 3});
    }

    SECTION("second step") {
        const auto actual = next_subvector_indices(4, {0, 1, 3});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == vector<size_t>{0, 2, 3});
    }

    SECTION("third step") {
        const auto actual = next_subvector_indices(4, {0, 2, 3});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == vector<size_t>{1, 2, 3});
    }

    SECTION("done") {
        const auto actual = next_subvector_indices(4, {1, 2, 3});
        REQUIRE(actual.empty());
    }
}

TEST_CASE("next_subvector_indices singleton for the vector size of 3") {
    SECTION("first step") {
        const auto actual = next_subvector_indices(3, {0});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == vector<size_t>{1});
    }

    SECTION("second step") {
        const auto actual = next_subvector_indices(3, {1});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == vector<size_t>{2});
    }

    SECTION("done") {
        const auto actual = next_subvector_indices(3, {2});
        REQUIRE(actual.empty());
    }
}

TEST_CASE("next_subvector_indices quad for the vector of size 100") {
    SECTION("split") {
        const auto actual = next_subvector_indices(100, {0, 1, 98, 99});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == vector<size_t>{0, 2, 3, 4});
    }

    SECTION("spread") {
        const auto actual = next_subvector_indices(100, {3, 17, 56, 70});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == vector<size_t>{3, 17, 56, 71});
    }

    SECTION("shifting the head") {
        const auto actual = next_subvector_indices(100, {66, 97, 98, 99});
        REQUIRE(!actual.empty());
        REQUIRE(actual.front() == vector<size_t>{67, 68, 69, 70});
    }

    SECTION("done") {
        const auto actual = next_subvector_indices(100, {96, 97, 98, 99});
        REQUIRE(actual.empty());
    }
}

TEST_CASE("has_only_c3m4_or_2_as_prime_factors") {
    SECTION("37") {
        REQUIRE(37 % 4 == 1);

        auto result_pair = primes_up_to(1000);
        REQUIRE(!has_only_c3m4_or_2_as_prime_factors(result_pair.second, 37));
    }
}

TEST_CASE("problem statement samples", "[samples]") {
    SECTION("zero count") { REQUIRE(count_solutoins(1000, 1) == 433); }
    SECTION("one count") { REQUIRE(count_solutoins(100000000000LL, 87) == 1); }

    SECTION("one sum") {
        REQUIRE(sum_solutoins(100000000000LL, 87) == 79345703125LL);
    }

    SECTION("two count") { REQUIRE(count_solutoins(100000000000LL, 31) == 3); }

    SECTION("two sum") {
        const Int expected = 30517578125 + 61035156250 + 91552734375;
        REQUIRE(sum_solutoins(100000000000LL, 31) == expected);
    }

    SECTION("13 3 count") { REQUIRE(count_solutoins(13, 3) == 3); }
    SECTION("13 3 sum") { REQUIRE(sum_solutoins(13, 3) == 28); }
}

TEST_CASE("ProjectEuler.net problem 233", "[233]") {
    REQUIRE(sum_solutoins(100000000000LL, 105) == 42);
}

// int main() {
//     size_t q;
//     cin >> q;

//     for (auto i = 0U; i != q; ++i) {
//         Int N, m;
//         cin >> N >> m;
//         cout << count_solutoins(N, m) << '\n';
//     }
// }
