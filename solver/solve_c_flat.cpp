#include "solver.h"
#include <string>
#include <stdexcept>
#include <bitset>
#include <algorithm>


namespace {
    static constexpr short INF = 512;
    static constexpr short MINUS_INF = -511;

    inline int min(int a, int b) {
        return b + ((a - b) & (a - b) >> 31);
    }

    inline int max(int a, int b) {
        return a - ((a - b) & (a - b) >> 31);
    }

    template <char _Size>
    int determinant(const char(&a)[_Size * _Size]);

    template<>
    int determinant<3>(const char(&a)[9]) {
        return
            a[0] * a[4] * a[8] +
            a[6] * a[1] * a[5] +
            a[3] * a[7] * a[2]
            - a[2] * a[4] * a[6]
            - a[1] * a[3] * a[8]
            - a[0] * a[5] * a[7];
    }

    template<>
    int determinant<2>(const char(&a)[4]) {
        return a[0] * a[3] - a[1] * a[2];
    }

    template <char _Size>
    int who_wins2(char(&matrix)[_Size * _Size], bool(&digits)[_Size * _Size], short step, int best1, int best2);

    template <char _Size>
    int who_wins1(char(&matrix)[_Size * _Size], bool(&digits)[_Size * _Size], short step, int best1, int best2) {
        constexpr char SIZE_SQR = _Size * _Size;        
        if (step == SIZE_SQR) {
            return determinant<_Size>(matrix);
        }        
        bool used_first_digit = false;
        for (unsigned char k = 0; k < SIZE_SQR; ++k) {
            if (digits[k]) {
                continue;
            }
            if (step == SIZE_SQR - 2) {
                if (used_first_digit) {
                    break;
                }
                used_first_digit = true;
            }
            digits[k] = true;
            for (char i = 0; i < SIZE_SQR; ++i) {
                if (matrix[i] != 0) {
                    continue;
                }
                matrix[i] = k + 1;
                int res = who_wins2<_Size>(matrix, digits, step + 1, best1, best2);
                matrix[i] = 0;

                if (res >= best1) {
                    digits[k] = false;
                    return res;
                }
                best2 = max(best2, res);
            }
            digits[k] = false;
        }
        return best2;
    }

    template <char _Size>
    int who_wins2(char(&matrix)[_Size * _Size], bool(&digits)[_Size * _Size], short step, int best1, int best2) {
        constexpr char SIZE_SQR = _Size * _Size;

        if (step == SIZE_SQR) {
            return determinant<_Size>(matrix);
        }
        bool used_first_digit = false;
        for (char k = 0; k < SIZE_SQR; ++k) {
            if (digits[k]) {
                continue;
            }
            if (step == SIZE_SQR - 2) {
                if (used_first_digit) {
                    break;
                }
                used_first_digit = true;
            }
            digits[k] = true;
            for (char i = 0; i < SIZE_SQR; ++i) {
                if (matrix[i] != 0) {
                    continue;
                }
                matrix[i] = k + 1;
                int res = who_wins1<_Size>(matrix, digits, step + 1, best1, best2);
                matrix[i] = 0;
                if (res <= best2) {
                    digits[k] = false;
                    return res;
                }
                best1 = min(best1, res);

            }
            digits[k] = false;
        }
        return best1;
    }


    template <char _Size>
    short who_wins(char(&matrix)[_Size * _Size], bool(&digits)[_Size * _Size], short step, short best1, short best2) {
        constexpr char SIZE_SQR = _Size * _Size;

        if (step == SIZE_SQR) {
            return determinant<_Size>(matrix);
        }

        bool used_first_digit = false;
        const bool is_first_step = is_first(step);
        for (char k = 0; k < SIZE_SQR; ++k) {
            if (digits[k]) {
                continue;
            }
            if (step == SIZE_SQR - 2) {
                if (used_first_digit) {
                    break;
                }
                used_first_digit = true;
            }
            digits[k] = true;
            for (char i = 0; i < SIZE_SQR; ++i) {
                if (matrix[i] != 0) {
                    continue;
                }
                matrix[i] = k + 1;
                short res = who_wins<_Size>(matrix, digits, step + 1, best1, best2);

                if (is_first_step) {
                    if (best2 < res) {
                        best2 = res;
                    }
                }
                else {
                    if (best1 > res) {
                        best1 = res;
                    }
                }

                matrix[i] = 0;

                if ((!is_first_step && res <= best2) || (is_first_step && res >= best1)) {
                    digits[k] = false;
                    return res;
                }

            }
            digits[k] = false;
        }
        return is_first_step ? best2 : best1;
    }

    template <char _Size>
    BestResult next_step(char(&matrix)[_Size * _Size], bool(&digits)[_Size * _Size], char step, int best1, int best2) {
        constexpr char SIZE_SQR = _Size * _Size;
        BestResult answer;
        answer.size = _Size;
        bool is_first_step = is_first(step);

        for (char k = 0; k < SIZE_SQR; ++k) {
            if (digits[k]) {
                continue;
            }
            digits[k] = true;
            for (char i = 0; i < SIZE_SQR; ++i) {
                if (matrix[i] != 0) {
                    continue;
                }
                matrix[i] = k + 1;
                // short res = who_wins<_Size>(matrix, digits, step + 1, best1, best2);;
                bool need_feel_matrix = false;
                int res = 0;
                if (is_first_step) {
                    res = who_wins2<_Size>(matrix, digits, step + 1, best1, best2);
                    if (best2 < res) {
                        best2 = res;
                        need_feel_matrix = true;
                    }
                }
                else {
                    res = who_wins1<_Size>(matrix, digits, step + 1, best1, best2);
                    if (best1 > res) {
                        best1 = res;
                        need_feel_matrix = true;
                    }
                }

                if (need_feel_matrix) {
                    answer.i = i;
                    answer.k = k;
                    std::copy_n(matrix, SIZE_SQR, answer.m.begin());
                    answer.result = res;
                }

                matrix[i] = 0;
            }
            digits[k] = false;
        }

        return answer;
    }

    template <char _Size>
    BestResult solve_matrix_flat_(const std::vector<std::vector<int>>& matrix_) {
        if (matrix_.size() != _Size) {
            throw std::invalid_argument("wrong matrix size");
        }
        constexpr char SIZE_SQR = _Size * _Size;
        char matrix[SIZE_SQR];
        for (char i = 0; i < SIZE_SQR; ++i) {
            matrix[i] = (char)matrix_[i / _Size][i % _Size];
        }
        bool digits[SIZE_SQR]{};
        char step = 0;
        for (char i = 0; i < SIZE_SQR; ++i) {
            char value = matrix[i];
            if (value != 0) {
                ++step;
                char index = value - 1;
                if (digits[index]) {
                    throw std::invalid_argument("repeated digit in matrix <" + std::to_string(value) + ">");
                }
                digits[index] = true;
            }
        }

        int best1 = INF;
        int best2 = MINUS_INF;
        return next_step<_Size>(matrix, digits, step, best1, best2);
    }
}

BestResult solve_matrix_flat(const std::vector<std::vector<int>>& matrix) {
    if (matrix.size() == 2) {
        return solve_matrix_flat_<2>(matrix);
    }
    if (matrix.size() == 3) {
        return solve_matrix_flat_<3>(matrix);
    }
    throw std::invalid_argument("wrong matrix size");
}

BestResult solve_matrix_flat2(const std::vector<std::vector<int>>& matrix) {
    return solve_matrix_flat_<3>(matrix);
}
