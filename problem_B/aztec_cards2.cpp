/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <algorithm>
#include <iostream>
#include <vector>

struct card_grid {
    int nr_rows{};
    int nr_columns{};
    int cards_per_row{};
    int cards_per_column{};

    std::vector<int> columns_left{};

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

    card_grid.columns_left = std::vector<int>(card_grid.nr_columns, card_grid.cards_per_column);

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
        sum += grid.columns_left[i];

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
    print_vector(grid.columns_left);
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
        if (grid.columns_left[i] < 0)
            return true;
    }

    return false;
}

static std::vector<int> calculate_columns_left(struct card_grid grid, int row) {
    std::vector<int> columns_left = grid.columns_left;
    for (int i = 0; i < grid.nr_columns; i++) {
        if (grid.matrix[row][i])
            columns_left[i]--;
    }

    return columns_left;
}

void process(struct card_grid grid, int row, long &solutions) {
    // print_grid(grid);
    if (impossible_grid(grid))
        return;

    if (grid_solved(grid, row)) {
        solutions++;
        return;
    }

    if (reached_invalid_row(grid, row))
        return;

    // For each permutation
    std::vector<int> save_columns_left = grid.columns_left;
    for (int i = 0; i < grid.permutations.size(); i++) {
        grid.matrix[row] = grid.permutations[i];
        grid.columns_left = calculate_columns_left(grid, row);
        process(grid, row + 1, solutions);
        grid.columns_left = save_columns_left;
    }
    grid.matrix[row] = std::vector<bool>(grid.nr_columns, false);

    return;
}

int main() {
    int nr_tests;
    std::cin >> nr_tests;

    for (int i = 0; i < nr_tests; i++) {
        struct card_grid grid = read_input();
        long solutions = 0;
        process(grid, 0, solutions);
        std::cout << solutions << std::endl;
    }
    return 0;
}
