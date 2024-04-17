/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <iostream>
#include <unordered_map>
#include <vector>

// Using a structure as a key to a map: https://en.cppreference.com/w/cpp/utility/hash
struct key {
    int row;
    std::vector<int> rows_left{};
    std::vector<int> columns_left{};
    bool operator==(const struct key &other) const {
        return row == other.row && rows_left == other.rows_left && columns_left == other.columns_left;
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
        std::size_t h2 = std::hash<std::vector<int>>{}(k.rows_left);
        std::size_t h3 = std::hash<std::vector<int>>{}(k.columns_left);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

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
    std::cin >> card_grid.cards_per_column >> card_grid.cards_per_row;

    card_grid.rows_left = std::vector<int>(card_grid.nr_rows, card_grid.cards_per_row);

    card_grid.columns_left = std::vector<int>(card_grid.nr_columns, card_grid.cards_per_column);

    // Initialize the card grid matrix with zeros
    card_grid.matrix = std::vector<std::vector<bool>>(card_grid.nr_rows, std::vector<bool>(card_grid.nr_columns, 0));

    return card_grid;
}

static bool grid_solved(struct card_grid &grid) {
    // Calculate the sum of grid.rows_left and grid.columns_left
    int sum = 0;
    for (int i = 0; i < grid.nr_rows; i++)
        sum += grid.rows_left[i];

    for (int i = 0; i < grid.nr_columns; i++)
        sum += grid.columns_left[i];

    return sum == 0;
}

static bool can_place_card(struct card_grid &grid, int row, int column) {
    return grid.rows_left[row] > 0 && grid.columns_left[column] > 0;
}

static bool reached_end_of_grid(struct card_grid &grid, int current_pos) {
    return current_pos >= ((grid.nr_rows * grid.nr_columns) - 1);
}

static bool has_card(struct card_grid &grid, int row, int column) {
    return grid.matrix[row][column];
}

static void place_card(struct card_grid &grid, int row, int column) {
    grid.matrix[row][column] = true;
    grid.rows_left[row]--;
    grid.columns_left[column]--;
}

static void remove_card(struct card_grid &grid, int row, int column) {
    grid.matrix[row][column] = false;
    grid.rows_left[row]++;
    grid.columns_left[column]++;
}

static void print_vector(std::vector<int> &v) {
    for (int i = 0; i < v.size(); i++)
        std::cout << v[i] << " ";
    std::cout << std::endl;
}

static void print_grid(struct card_grid &grid, int row, int column) {
    print_vector(grid.columns_left);
    for (int i = 0; i < grid.nr_rows; i++) {
        for (int j = 0; j < grid.nr_columns; j++) {
            if (i == row && j == column)
                std::cout << "* ";
            else if (grid.matrix[i][j])
                std::cout << "X ";
            else
                std::cout << "- ";
        }
        std::cout << grid.rows_left[i] << std::endl;
    }
    std::cout << std::endl;
}

static bool case_in_memo(std::unordered_map<struct key, long> &memo, struct card_grid &grid, int row) {
    return memo.find({row, grid.rows_left, grid.columns_left}) != memo.end();
}

long process(struct card_grid &grid, std::unordered_map<struct key, long> &memo, int position) {
    int row = position / grid.nr_columns;
    int column = position % grid.nr_columns;

    print_grid(grid, row, column);

    if (can_place_card(grid, row, column)) {
        place_card(grid, row, column);
    }

    if (case_in_memo(memo, grid, row)) {
        std::cout << "Case in memo: " << memo[{row, grid.rows_left, grid.columns_left}] << std::endl;
        return memo[{row, grid.rows_left, grid.columns_left}];
    }

    if (grid_solved(grid)) {
        std::cout << "Grid solved" << std::endl;
        memo[{row, grid.rows_left, grid.columns_left}] = 1;
        remove_card(grid, row, column);
        return 1;
    }

    if (reached_end_of_grid(grid, position)) {
        if (has_card(grid, row, column))
            remove_card(grid, row, column);
        memo[{row, grid.rows_left, grid.columns_left}] = 0;
        return 0;
    }

    bool removed_card = false;
    long solutions = 0;
    do {
        removed_card = false;
        solutions += process(grid, memo, position + 1);
        if (has_card(grid, row, column)) {
            remove_card(grid, row, column);
            removed_card = true;
        }
    } while (removed_card);

    memo[{row, grid.rows_left, grid.columns_left}] = solutions;
    return solutions;
}

int main(int argc, char const *argv[]) {
    int nr_tests;
    std::cin >> nr_tests;

    for (int i = 0; i < nr_tests; i++) {
        std::unordered_map<struct key, long> memo;
        struct card_grid grid = read_input();

        std::cout << process(grid, memo, 0) << std::endl;
    }
    return 0;
}