/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

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

static bool can_place_card(struct card_grid grid, int row, int column) {
    return grid.rows_left[row] > 0 && grid.columns_left[column] > 0;
}

static bool reached_end_of_grid(struct card_grid grid, int current_pos) {
    return current_pos >= ((grid.nr_rows * grid.nr_columns) - 1);
}

static bool has_card(struct card_grid grid, int row, int column) {
    return grid.matrix[row][column];
}

void place_card(struct card_grid &grid, int row, int column) {
    grid.matrix[row][column] = true;
    grid.rows_left[row]--;
    grid.columns_left[column]--;
}

void remove_card(struct card_grid &grid, int row, int column) {
    grid.matrix[row][column] = false;
    grid.rows_left[row]++;
    grid.columns_left[column]++;
}

void print_vector(std::vector<int> v) {
    for (int i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}

void print_grid(struct card_grid grid, int row, int column) {
    for (int i = 0; i < grid.nr_rows; i++) {
        for (int j = 0; j < grid.nr_columns; j++) {
            if (grid.matrix[i][j])
                std::cout << "X ";
            else
                std::cout << "- ";
        }
        std::cout << std::endl;
    }
    std::cout << row << " " << column << std::endl;
    std::cout << std::endl;
}

void process(struct card_grid grid, int position, int &solutions) {
    int row = position / grid.nr_columns;
    int column = position % grid.nr_columns;

    if (can_place_card(grid, row, column))
        place_card(grid, row, column);
    // print_grid(grid, row, column);

    if (grid_solved(grid)) {
        //  std::cout << "solved!" << std::endl;
        solutions++;
        return;
    }

    if (reached_end_of_grid(grid, position)) {
        if (has_card(grid, row, column))
            remove_card(grid, row, column);

        return;
    }

    bool removed_card = false;
    do {
        removed_card = false;
        process(grid, position + 1, solutions);
        if (has_card(grid, row, column)) {
            remove_card(grid, row, column);
            removed_card = true;
        }
    } while (removed_card);

    return;
}

int main(int argc, char const *argv[]) {
    int nr_tests;
    std::cin >> nr_tests;

    for (int i = 0; i < nr_tests; i++) {
        int solutions = 0;
        process(read_input(), 0, solutions);
        std::cout << solutions << std::endl;
    }
    return 0;
}
