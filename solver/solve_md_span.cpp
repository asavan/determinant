#include "solver.h"
#include <string>
#include <stdexcept>
#include <mdspan>

namespace {

    static constexpr int SIZE = 3;
    static constexpr int SIZE_SQR = SIZE * SIZE;
    static constexpr int INF = 512;
    static constexpr int MINUS_INF = -511;
    using Extents2D = std::extents<int, SIZE, SIZE>;


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

    void copy_matrix(std::mdspan<const int, Extents2D> src, std::array<int, 9>& dst) {
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                dst[i * SIZE + j] = src[i, j];
            }
        }
    }
    
    BestResult next_step(std::mdspan<int, Extents2D> matrix, bool(&digits)[SIZE_SQR], int step, int best1, int best2, bool need_fill_result) {

        BestResult answer;

        if (step == SIZE_SQR) {
            if (need_fill_result) {
                copy_matrix(matrix, answer.m);
            }
            int det = determinant<SIZE>(matrix);
            answer.result = det;
            // hack set answer to valid state
            answer.i = 0;
            return answer;
        }


        for (int k = 0; k < SIZE_SQR; ++k) {
            if (digits[k]) {
                continue;
            }
            digits[k] = true;
            for (int i = 0; i < SIZE; ++i) {
                for (int j = 0; j < SIZE; ++j) {
                    if (matrix[i, j] != 0) {
                        continue;
                    }
                    matrix[i, j] = k + 1;
                    BestResult sub_result = next_step(matrix, digits, step + 1, best1, best2, false);
                    int res;
                    if (!sub_result.is_valid()) {
                        res = is_first(step) ? INF: MINUS_INF;
                    }
                    else {
                        res = sub_result.result;
                    }
                    // int res = who_wins(matrix, digits, d + 1, best1, best2);


                    if (is_first(step)) {
                        if (best2 < res) {
                            best2 = res;
                            answer.i = i;
                            if (need_fill_result) {
                                answer.j = j;
                                answer.k = k;
                                copy_matrix(matrix, answer.m);
                            }
                            answer.result = res;
                        }
                    }
                    else {
                        if (best1 > res) {
                            best1 = res;
                            answer.i = i;
                            if (need_fill_result) {                                
                                answer.j = j;
                                answer.k = k;
                                copy_matrix(matrix, answer.m);
                            }
                            answer.result = res;
                        }
                    }

                    matrix[i, j] = 0;

                    if ((!is_first(step) && res <= best2) || (is_first(step) && res >= best1)) {                        
                        digits[k] = false;
                        return answer;
                    }
                }
            }

            digits[k] = false;
        }
        // answer.result = is_first(step) ? best2 : best1;
        return answer;
    }
}

BestResult solve_md_span(const std::vector<std::vector<int>>& matrix_) {
    if (matrix_.size() != SIZE) {
        throw std::invalid_argument("wrong matrix size");
    }
    int matrix_arr[SIZE_SQR];
    std::mdspan<int, Extents2D> matrix(matrix_arr);
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            matrix[i, j] = matrix_[i][j];
        }
    }
    bool digits[SIZE_SQR]{};
    int step = 0;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            int value = matrix[i, j];
            if (value != 0) {
                ++step;
                int index = value - 1;
                if (digits[index]) {
                    throw std::invalid_argument("repeated digit in matrix <" + std::to_string(value) + ">");
                }
                digits[index] = true;
            }
        }
    }

    int best1 = INF;
    int best2 = MINUS_INF;
    BestResult res = next_step(matrix, digits, step, best1, best2, true);
    return res;
}

BestResult solve_solve_md_span_full() {
    std::vector<std::vector<int>> matrix = {
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0}
    };

    return solve_md_span(matrix);
}
