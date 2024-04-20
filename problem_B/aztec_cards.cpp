/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>

// #define VERBOSE

// Using a structure as a key to a map: https://en.cppreference.com/w/cpp/utility/hash
struct key {
    int rows_left;
    std::vector<int> columns_left{};
    bool operator==(const struct key &other) const {
        return rows_left == other.rows_left && columns_left == other.columns_left;
    }
};

// Hashing vector<int> https://stackoverflow.com/questions/10405030/c-unordered-map-fail-when-used-with-a-vector-as-key
template <>
struct std::hash<std::vector<int>> {
    std::size_t operator()(const std::vector<int> &v) const {
        std::hash<int> hasher;
        std::size_t seed = 0;
        for (int i : v) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
template <>
struct std::hash<struct key> {
    std::size_t operator()(const struct key &k) const noexcept {
        std::size_t h1 = std::hash<int>{}(k.rows_left);
        std::size_t h2 = std::hash<std::vector<int>>{}(k.columns_left);
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};

struct card_grid {
    int nr_rows{};
    int nr_columns{};
    int cards_per_row{};
    int cards_per_column{};

    std::vector<int> cards_left{};
    std::vector<int> cards_left_sorted{};
    std::vector<bool> state{};

    std::vector<std::vector<bool>> permutations{};
    std::unordered_map<std::vector<bool>, std::vector<int>> available_permutations{};

    std::vector<std::vector<bool>> matrix;
};

void print_bool_vector(std::vector<bool> v) {
    for (int i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}

void print_int_vector(std::vector<int> v) {
    for (int i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}

void print_available_permutations(struct card_grid &grid) {
    for (auto const &pair : grid.available_permutations) {
        std::cout << "State: ";
        print_bool_vector(pair.first);
        std::cout << "Permutations: ";
        print_int_vector(pair.second);
    }
}

void print_grid(struct card_grid &grid) {
    print_int_vector(grid.cards_left);
    for (int i = 0; i < grid.nr_rows; i++) {
        for (int j = 0; j < grid.nr_columns; j++) {
            if (grid.matrix[i][j])
                std::cout << "X ";
            else
                std::cout << "- ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void generate_permutations(struct card_grid &grid) {
    // Generate the initial permutation with 'numTrue' true values followed by false values
    std::vector<bool> initial_permutation(grid.nr_columns);
    fill_n(initial_permutation.begin(), grid.cards_per_row, true);

    // Generate all permutations
    do {
        grid.permutations.push_back(initial_permutation);
    } while (prev_permutation(initial_permutation.begin(), initial_permutation.end()));
}

void generate_available_permutations(struct card_grid &grid) {
    // Generate all possible states
    std::vector<std::vector<bool>> states;
    for (int i = 0; i < (1 << grid.nr_columns); ++i) {
        std::vector<bool> state(grid.nr_columns);
        for (int j = 0; j < grid.nr_columns; ++j) {
            state[j] = i & (1 << j);
        }
        states.push_back(state);
        grid.available_permutations[state] = std::vector<int>();
    }

    // Iterate over all permutations
    for (int i = 0; i < grid.permutations.size(); ++i) {
        // Iterate over all states
        for (const auto &state : states) {

            bool can_apply = true;
            std::vector<int> cards_left(grid.nr_columns);
            for (int j = 0; j < grid.nr_columns; ++j) {
                // Check if the permutation can be applied to the state
                if (grid.permutations[i][j] && !state[j]) {
                    can_apply = false;
                    break;
                }
            }

            if (can_apply)
                grid.available_permutations[state].push_back(i);
        }
    }
}

// Reads the input and returns a card grid structure
struct card_grid read_input() {
    struct card_grid grid;
    std::cin >> grid.nr_columns >> grid.nr_rows;
    std::cin >> grid.cards_per_column >> grid.cards_per_row;

    grid.cards_left = std::vector<int>(grid.nr_columns, grid.cards_per_column);
    grid.cards_left_sorted = grid.cards_left;
    grid.state = std::vector<bool>(grid.nr_columns, true);

    // Initialize the card grid matrix with zeros
    grid.matrix = std::vector<std::vector<bool>>(grid.nr_rows, std::vector<bool>(grid.nr_columns, 0));

    // Initialize the permutations vector
    generate_permutations(grid);

    generate_available_permutations(grid);

    return grid;
}

bool grid_solved(struct card_grid &grid, int row) {
    return row == grid.nr_rows - 1;
}

bool reached_invalid_row(struct card_grid &grid, int row) {
    return row == grid.nr_rows;
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
    std::vector<int> cards_left = grid.cards_left;
    for (int i = 0; i < grid.nr_columns; i++) {
        grid.cards_left[i] -= (grid.matrix[row][i]);
        grid.state[i] = grid.cards_left[i] > 0;
    }
    grid.cards_left_sorted = grid.cards_left;
    std::sort(grid.cards_left_sorted.begin(), grid.cards_left_sorted.end());
}

void add_to_memo(struct card_grid &grid, int row, long solutions, std::unordered_map<struct key, long> &memo) {
    memo[{grid.nr_rows - row - 1, grid.cards_left_sorted}] = solutions;
#ifdef VERBOSE
    std::cout << "Inserted rows_left: " << grid.nr_rows - row - 1 << " Cards left: ";
    print_int_vector(grid.cards_left);
    std::cout << "Solutions: " << solutions << std::endl;
#endif
}

bool case_in_memo(struct card_grid &grid, int row, std::unordered_map<struct key, long> &memo) {
    return memo.find({grid.nr_rows - row - 1, grid.cards_left_sorted}) != memo.end();
}

bool preprocess(struct card_grid &grid) {
    return grid.nr_rows * grid.cards_per_row == grid.nr_columns * grid.cards_per_column;
}

long process(struct card_grid &grid, int row, std::unordered_map<struct key, long> &memo) {
#ifdef VERBOSE
    std::cout << "Row: " << row << " Cards left: ";
    print_int_vector(grid.cards_left);
    print_grid(grid);
#endif

    if (case_in_memo(grid, row, memo)) {
#ifdef VERBOSE
        std::cout << "================================Found ABOVE matrix in memo: " << memo[{grid.nr_rows - row - 1, grid.cards_left_sorted}] << "================================" << std::endl;
#endif
        return memo[{grid.nr_rows - row - 1, grid.cards_left_sorted}];
    }

    if (impossible_grid(grid, row)) {
        add_to_memo(grid, row, 0, memo);
        return 0;
    }

    if (grid_solved(grid, row)) {
        add_to_memo(grid, row, 1, memo);
        return 1;
    }

    long solutions = 0;
    std::vector<int> save_columns_left = grid.cards_left;
    // For each available permutation considering the state of the grid
    for (int permutation_i : grid.available_permutations[grid.state]) {
        grid.matrix[row] = grid.permutations[permutation_i];
        calculate_cards_left(grid, row);
        long permutation_solutions = process(grid, row + 1, memo);
        solutions += permutation_solutions;
        grid.cards_left = save_columns_left;
    }
    grid.matrix[row] = std::vector<bool>(grid.nr_columns, false);

    add_to_memo(grid, row, solutions, memo);
    return solutions;
}

static void print_memo(std::unordered_map<struct key, long> &memo) {
    std::cout << "Memo:" << std::endl;
    for (auto const &pair : memo) {
        std::cout << "Row: " << pair.first.rows_left << " Cards left: ";
        print_int_vector(pair.first.columns_left);
        std::cout << "Solutions: " << pair.second << std::endl;
    }
}

int main(int argc, char *argv[]) {
    int nr_tests;
    std::cin >> nr_tests;

    for (int i = 0; i < nr_tests; i++) {
        struct card_grid grid = read_input();
        // Start timer
        auto start = std::chrono::high_resolution_clock::now();
        std::unordered_map<struct key, long> memo;
        std::cout << process(grid, 0, memo) << std::endl;

#ifdef VERBOSE
        print_memo(memo);
#endif

        // End timer and print how much time it took
        if (argc > 1 && std::string(argv[1]) == "-t") {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
        }
    }
    return 0;
}
