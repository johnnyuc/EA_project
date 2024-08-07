/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <iostream>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <unordered_map>

struct card_grid {
    int nr_rows{};
    int nr_columns{};
    int cards_per_row{};
    int cards_per_column{};

    std::vector<int> cards_left{};
    uint32_t state{};

    std::vector<uint32_t> permutations{};
    std::unordered_map<uint32_t, std::vector<uint32_t>> available_permutations{};

    std::vector<uint32_t> matrix;
};

void generate_permutations(struct card_grid &grid) {
    // Generate the initial permutation with 'cards_per_row' 1's followed by 0's
    uint32_t initial_permutation = (1 << grid.cards_per_row) - 1;

    do {
        grid.permutations.push_back(initial_permutation);
        // Generate the next permutation using bitwise operations
        uint32_t t = initial_permutation | (initial_permutation - 1);
        initial_permutation = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(initial_permutation) + 1));
    } while (initial_permutation < (1 << grid.nr_columns));
}

// Reads the input and returns a card grid structure
struct card_grid read_input() {
    struct card_grid grid;
    std::cin >> grid.nr_columns >> grid.nr_rows;
    std::cin >> grid.cards_per_column >> grid.cards_per_row;

    grid.cards_left = std::vector<int>(grid.nr_columns, grid.cards_per_column);
    grid.state = (1 << grid.nr_columns) - 1;

    // Initialize the card grid matrix
    grid.matrix = std::vector<uint32_t>(grid.nr_rows, 0);

    // Initialize the permutations vector
    generate_permutations(grid);

    return grid;
}

int create_key(const std::vector<int> &cards_left, int cards_per_row) {
    std::vector<int> sorted_cards = cards_left;
    std::sort(sorted_cards.begin(), sorted_cards.end());

    int key = 0;
    for (int i : sorted_cards)
        key = (key << 3) + (key << 1) + i;
    key = (key << 3) + (key << 1) + (cards_per_row + 1);

    return key;
}

inline bool grid_solved(struct card_grid &grid, int row) {
    return row == grid.nr_rows - 1;
}

bool impossible_grid(struct card_grid &grid, int row) {
    int rows_left = grid.nr_rows - row;
    for (int i = 0; i < grid.nr_columns; i++) {
        // If the number of cards left is greater than the number of rows left
        if (grid.cards_left[i] < 0 || grid.cards_left[i] > rows_left)
            return true;
    }

    return false;
}

void calculate_cards_left(struct card_grid &grid, int row) {
    for (int i = 0; i < grid.nr_columns; i++) {
        grid.cards_left[i] -= grid.matrix[row] & (1 << i) ? 1 : 0;
        if (grid.cards_left[i] == 0)
            grid.state = grid.state & ~(1 << i);
    }
}

inline bool case_in_memo(int key, std::unordered_map<int, unsigned long long> &memo) {
    return memo.find(key) != memo.end();
}

inline bool preprocess(struct card_grid &grid) {
    return grid.nr_rows * grid.cards_per_row == grid.nr_columns * grid.cards_per_column;
}

unsigned long long process(struct card_grid &grid, int row, std::unordered_map<int, unsigned long long> &memo) {
    int key = create_key(grid.cards_left, grid.cards_per_row);

    if (case_in_memo(key, memo)) {
        return memo[key];
    }

    if (impossible_grid(grid, row)) {
        memo[key] = 0;
        return 0;
    }

    if (grid_solved(grid, row)) {
        memo[key] = 1;
        return 1;
    }

    unsigned long long solutions = 0;
    std::vector<int> save_cards_left = grid.cards_left;
    uint32_t save_state = grid.state;
    // For each available permutation considering the state of the grid
    for (unsigned int permutation : grid.permutations) {
        grid.matrix[row] = permutation;
        calculate_cards_left(grid, row);
        solutions += process(grid, row + 1, memo);
        grid.cards_left = save_cards_left;
        grid.state = save_state;
    }

    grid.matrix[row] = 0;

    memo[key] = solutions;
    return solutions;
}

int main() {
    int nr_tests;
    std::cin >> nr_tests;

    std::unordered_map<int, unsigned long long> memo;
    for (int i = 0; i < nr_tests; i++) {
        struct card_grid grid = read_input();

        if (preprocess(grid)) std::cout << process(grid, 0, memo) << std::endl;
        else std::cout << 0 << std::endl;
    }
}
