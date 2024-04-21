/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <chrono>

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

void print_int_vector(std::vector<int> v) {
    for (int i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}

void print_grid(struct card_grid &grid) {
    // print grid.state
    std::cout << "State: ";
    for (int i = 0; i < grid.nr_columns; i++) {
        if (grid.state & (1 << i))
            std::cout << "1 ";
        else
            std::cout << "0 ";
    }

    std::cout << std::endl;

    print_int_vector(grid.cards_left);
    for (int i = 0; i < grid.nr_rows; i++) {
        for (int j = 0; j < grid.nr_columns; j++) {
            if (grid.matrix[i] & (1 << j))
                std::cout << "X ";
            else
                std::cout << "- ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void generate_permutations(struct card_grid &grid) {
    // Generate possible states
    std::vector<uint32_t> states;
    uint32_t max_state = 1 << grid.nr_columns; // Calculate the maximum state
    for (uint32_t state = 0; state < max_state; ++state) {
        states.push_back(state);
    }

    // Generate the initial permutation with 'cards_per_row' 1's followed by 0's
    uint32_t initial_permutation = (1 << grid.cards_per_row) - 1;
    int i = 0;

    do {
        grid.permutations.push_back(initial_permutation);
        // Check the state for the current permutation
        for (const auto &state : states) {
            if ((grid.permutations[i] & state) == grid.permutations[i])
                grid.available_permutations[state].push_back(i);
        }

        // Generate the next permutation using bitwise operations
        uint32_t t = initial_permutation | (initial_permutation - 1);
        initial_permutation = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(initial_permutation) + 1));
        i++;
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

int create_key(const std::vector<int> &cards_left, int row) {
    std::vector<int> sorted_cards = cards_left;
    std::sort(sorted_cards.begin(), sorted_cards.end());

    int key = 0;
    for (int i : sorted_cards)
        key = (key << 3) + (key << 1) + i;
    key = (key << 3) + (key << 1) + (row + 1);

    return key;
}

bool grid_solved(struct card_grid &grid, int row) {
    return row == grid.nr_rows - 1;
}

bool impossible_grid(struct card_grid &grid, int row) {
    int rows_left = grid.nr_rows - row;
    for (int i = 0; i < grid.nr_columns; i++) {
        // If the number of cards left is greater than the number of rows left
        if (grid.cards_left[i] > rows_left)
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

bool case_in_memo(int key, std::unordered_map<int, unsigned long long> &memo) {
    return memo.find(key) != memo.end();
}

bool is_last_row(struct card_grid &grid, int row) {
    return row == grid.nr_rows - 1;
}

bool preprocess(struct card_grid &grid) {
    return grid.nr_rows * grid.cards_per_row == grid.nr_columns * grid.cards_per_column;
}

unsigned long long process(struct card_grid &grid, int row, std::unordered_map<int, unsigned long long> &memo) {
    // print_grid(grid);

    int key = create_key(grid.cards_left, row);

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
    if (is_last_row(grid, row))
        solutions = grid.available_permutations[grid.state].size();
    else {
        // For each available permutation considering the state of the grid
        for (int permutation_i : grid.available_permutations[grid.state]) {
            grid.matrix[row] = grid.permutations[permutation_i];
            calculate_cards_left(grid, row);
            solutions += process(grid, row + 1, memo);
            grid.cards_left = save_cards_left;
            grid.state = save_state;
        }
    }
    grid.matrix[row] = 0;

    memo[key] = solutions;
    return solutions;
}

int main(int argc, char const *argv[]) {

    int nr_tests;
    std::cin >> nr_tests;

    for (int i = 0; i < nr_tests; i++) {
        // Timer in nano seconds
        auto start = std::chrono::high_resolution_clock::now();
        struct card_grid grid = read_input();
        std::unordered_map<int, unsigned long long> memo;

        if (preprocess(grid))
            std::cout << process(grid, 0, memo) << std::endl;
        else
            std::cout << 0 << std::endl;

        // End timer and print how much time it took
        if (argc > 1 && std::string(argv[1]) == "-t") {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
        }
    }
}
