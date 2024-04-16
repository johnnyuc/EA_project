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
    int row;
    std::vector<int> cards_left{};
    bool operator==(const struct key &other) const {
        return row == other.row && cards_left == other.cards_left;
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
        std::size_t h1 = std::hash<int>{}(k.row);
        std::size_t h2 = std::hash<std::vector<int>>{}(k.cards_left);
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};

struct card_grid {
    int nr_rows{};
    int nr_columns{};
    int cards_per_row{};
    int cards_per_column{};

    std::vector<int> cards_left{};
    std::vector<int> mirrored_cards_left{};

    std::vector<std::vector<bool>> permutations{};

    std::vector<std::vector<bool>> matrix;
};

std::vector<std::vector<bool>> generatePermutations(int numTrue, int size) {
    // Generate the initial permutation with 'numTrue' true values followed by false values
    std::vector<bool> initial_permutation(size);
    fill_n(initial_permutation.begin(), numTrue, true);

    // Vector to store all permutations, including the initial permutation
    std::vector<std::vector<bool>> permutations{initial_permutation};

    // Generate all permutations
    while (prev_permutation(initial_permutation.begin(), initial_permutation.end()))
        permutations.push_back(initial_permutation);

    return permutations;
}

// Reads the input and returns a card grid structure
struct card_grid read_input() {
    struct card_grid card_grid;
    std::cin >> card_grid.nr_columns >> card_grid.nr_rows;
    std::cin >> card_grid.cards_per_column >> card_grid.cards_per_row;

    card_grid.cards_left = std::vector<int>(card_grid.nr_columns, card_grid.cards_per_column);

    // Initialize the card grid matrix with zeros
    card_grid.matrix = std::vector<std::vector<bool>>(card_grid.nr_rows, std::vector<bool>(card_grid.nr_columns, 0));

    // Initialize the permutations vector
    card_grid.permutations = generatePermutations(card_grid.cards_per_row, card_grid.nr_columns);

    return card_grid;
}

static bool grid_solved(struct card_grid grid, int row) {
    // Calculate the sum of grid.columns_left
    int sum = 0;
    for (int i = 0; i < grid.nr_columns; i++)
        sum += grid.cards_left[i];

    return sum == 0 && row == grid.nr_rows;
}

static bool reached_invalid_row(struct card_grid grid, int row) {
    return row == grid.nr_rows;
}

static void print_vector(std::vector<int> v) {
    for (int i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}

static void print_grid(struct card_grid grid) {
    print_vector(grid.cards_left);
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

static bool impossible_grid(struct card_grid grid) {
    for (int i = 0; i < grid.nr_columns; i++) {
        if (grid.cards_left[i] < 0)
            return true;
    }

    return false;
}

static std::vector<int> calculate_cards_left(struct card_grid grid, int row) {
    std::vector<int> cards_left = grid.cards_left;
    for (int i = 0; i < grid.nr_columns; i++)
        cards_left[i] -= (grid.matrix[row][i]);

    return cards_left;
}

long process(struct card_grid &grid, int row, std::unordered_map<struct key, long> &memo) {
#ifdef VERBOSE
    std::cout << "Row: " << row << " Cards left: ";
    print_vector(grid.cards_left);
    print_grid(grid);
#endif

    if (impossible_grid(grid))
        return 0;

    // print row and cards_left

    if (memo.find({row, grid.cards_left}) != memo.end()) {
#ifdef VERBOSE
        std::cout << "================================Found ABOVE matrix in memo================================" << std::endl;
#endif

        return memo[{row, grid.cards_left}];
    }

    if (grid_solved(grid, row))
        return 1;

    if (reached_invalid_row(grid, row))
        return 0;

    long solutions = 0;
    std::vector<int> save_columns_left = grid.cards_left;
    // For each permutation
    for (int i = 0; i < grid.permutations.size(); i++) {
        grid.matrix[row] = grid.permutations[i];
        grid.cards_left = calculate_cards_left(grid, row);
        long permutation_solutions = process(grid, row + 1, memo);
        grid.cards_left = save_columns_left;

        memo.insert({{row, grid.cards_left}, permutation_solutions});
        solutions += permutation_solutions;
    }
    grid.matrix[row] = std::vector<bool>(grid.nr_columns, false);

    return solutions;
}

static void print_memo(std::unordered_map<struct key, long> memo) {
    std::cout << "Memo:" << std::endl;
    for (auto const &pair : memo) {
        std::cout << "Row: " << pair.first.row << " Cards left: ";
        print_vector(pair.first.cards_left);
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
