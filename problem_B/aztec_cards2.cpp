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

    std::vector<int> rows_left{};
    std::vector<int> columns_left{};

    std::vector<std::vector<bool>> matrix;
};

// Reads the input and returns a card grid structure
struct card_grid read_input() {
    struct card_grid card_grid;
    std::cin >> card_grid.nr_columns >> card_grid.nr_rows;
    std::cin >> card_grid.cards_per_row >> card_grid.cards_per_column;

    card_grid.rows_left = std::vector<int>(card_grid.nr_rows, card_grid.cards_per_row);

    card_grid.columns_left = std::vector<int>(card_grid.nr_columns, card_grid.cards_per_column);

    // Initialize the card grid matrix with zeros
    card_grid.matrix = std::vector<std::vector<bool>>(card_grid.nr_rows, std::vector<bool>(card_grid.nr_columns, 0));

    return card_grid;
}

static bool grid_solved(struct card_grid grid) {
    // Calculate the sum of grid.rows_left and grid.columns_left
    int sum = 0;
    for (int i = 0; i < grid.nr_rows; i++)
        sum += grid.rows_left[i];

    for (int i = 0; i < grid.nr_columns; i++)
        sum += grid.columns_left[i];

    return sum == 0;
}

static bool has_card(struct card_grid grid, int row, int column) {
    return grid.matrix[row][column];
}

static bool reached_invalid_row(struct card_grid grid, int row) {
    return row == grid.nr_rows;
}

void print_vector(std::vector<int> v) {
    for (int i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}

void print_grid(struct card_grid grid) {
    for (int i = 0; i < grid.nr_rows; i++) {
        for (int j = 0; j < grid.nr_columns; j++) {
            if (has_card(grid, i, j))
                std::cout << "X ";
            else
                std::cout << "- ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void place_first_cards_row(struct card_grid &grid, int row) {
    for (int i = 0; i < grid.cards_per_column; i++) {
        grid.matrix[row][i] = true;
        grid.columns_left[i]--;
    }

    grid.rows_left[row] = 0;
}

void remove_cards_row(struct card_grid &grid, int row) {
    // Assuming that this function is only called with the last permutation of the row
    for (int i = grid.nr_columns - grid.cards_per_row; i < grid.nr_columns; i++) {
        grid.matrix[row][i] = false;
        grid.columns_left[i]++;
    }
}

void process(struct card_grid grid, int row, int &solutions) {
    print_grid(grid);
    if (grid_solved(grid)) {
        solutions++;
        return;
    }

    if (reached_invalid_row(grid, row))
        return;

    // Place the first cards of the current row
    place_first_cards_row(grid, row);

    do {
        process(grid, row + 1, solutions);
        // Generate all possible permutations of the current row
    } while (prev_permutation(grid.matrix[row].begin(), grid.matrix[row].end()));

    // When all permutations are done, remove the cards from the current row
    remove_cards_row(grid, row);

    return;
}

int main() {
    int nr_tests;
    std::cin >> nr_tests;

    for (int i = 0; i < nr_tests; i++) {
        int solutions = 0;
        process(read_input(), 0, solutions);
        std::cout << solutions << std::endl;
    }
    return 0;
}
