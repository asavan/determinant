#include <iostream> 
#include <algorithm>
#include <utility>
#include <array>
#include <set>
#include <climits>
#include <mdspan>

namespace {
    static constexpr int SIZE = 3;
    static constexpr int SIZE_SQR = SIZE * SIZE;
    using Extents2D = std::extents<int, SIZE, SIZE>;

    template <std::size_t... Is>
    constexpr auto make_1_to_n_impl(std::index_sequence<Is...>) {
        return std::array<int, sizeof...(Is)>{ (static_cast<int>(Is) + 1)... };
    }

    template <std::size_t N>
    constexpr auto make_1_to_n() {
        return make_1_to_n_impl(std::make_index_sequence<N>{});
    }

    template <std::size_t N>
    constexpr auto make_1_to_n2() {
        std::array<int, N> arr{};
        for (std::size_t i = 0; i < N; ++i) {
            arr[i] = static_cast<int>(i + 1);
        }
        return arr;
    }


    template <size_t _Size>
    int determinant(std::mdspan<const int, std::extents<int, _Size, _Size>> a);

    template<>
    int determinant<3>(std::mdspan<const int, Extents2D> a) {
        return
            a[0, 0] * a[1, 1] * a[2, 2] +
            a[2, 0] * a[0, 1] * a[1, 2] +
            a[1, 0] * a[2, 1] * a[0, 2]
            - a[2, 0] * a[1, 1] * a[0, 2]
            - a[0, 1] * a[1, 0] * a[2, 2]
            - a[0, 0] * a[1, 2] * a[2, 1];
    }

    template<>
    int determinant<2>(std::mdspan<const int, std::extents<int, 2, 2>> a) {
        return a[0, 0] * a[1, 1] - a[1, 0] * a[0, 1];
    }
}

void solve_permutation() {
    int positive_count = 0;
    int negative_count = 0;
    int max_det_count = 0;
    int min_det_count = 0;

    int zero_count = 0;
    int max_det = INT_MIN;
    int min_det = INT_MAX;
    std::set<int> dets;
    std::array<int, SIZE_SQR> digits = make_1_to_n<SIZE_SQR>();
    // Usage
    //constexpr auto seq = make_1_to_n<SIZE_SQR>();
    // static_assert(digits[0] == 1 && digits[4] == 5);

    do {
            // Pass only the data pointer; no runtime dimension arguments needed
            std::mdspan<const int, Extents2D> matrix(digits.data());

            int det = determinant(matrix);
            dets.insert(det);
            if (det > 0) {
                ++positive_count;
                if (det > max_det) {
                    max_det = det;
                    max_det_count = 1;
                }
                else if (det == max_det) {
                    ++max_det_count;
                }
            }
            else if (det < 0) {
                ++negative_count;
                if (det < min_det) {
                    min_det = det;
                    min_det_count = 1;
                }
                else if (det == min_det) {
                    ++min_det_count;
                }
            }
            else {
                ++zero_count;
            }
    } while (std::next_permutation(begin(digits), end(digits)));
    std::cout << "Max det " << *dets.rbegin() << std::endl;
    std::cout << "Min det " << *dets.begin() << std::endl;
    std::cout << "Positive count " << positive_count << std::endl;
    std::cout << "Negative count " << negative_count << std::endl;
    std::cout << "Zero count " << zero_count << std::endl;

    std::cout << "Max det count " << max_det_count << std::endl;
    std::cout << "Min det count " << min_det_count << std::endl;

    for (int det : dets) {
        std::cout << det << " ";
    }
    std::cout << std::endl;
    std::cout << "det size " << dets.size() << std::endl;

    int prev = 0;
    int hole_count = 0;
    for (int det : dets) {
        if (det > 0) {
            if (det - prev > 1) {
                std::cout <<" hole " << det << std::endl;
                ++hole_count;
                // break;
            }
        }
        prev = det;
        
    }
    std::cout << "Hole count " << hole_count << std::endl;

}
