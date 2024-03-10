#include <iostream>
#include <vector>
#include <ctime>

#define LEFT 0
#define RIGHT 1

bool verbose = false;

struct vault {
    int best = -1;
    int max_moves{};
    int nr_rows{};
    int nr_columns{};
    std::vector<std::vector<int>> matrix;
};

struct vault read() {
    struct vault vault;
    std::cin >> vault.nr_rows >> vault.nr_columns >> vault.max_moves;

    std::vector<std::vector<int>> vault_matrix(vault.nr_rows, std::vector<int>(vault.nr_columns));

    for (int i_row = 0; i_row < vault.nr_rows; i_row++)
        for (int i_column = 0; i_column < vault.nr_columns; i_column++)
            std::cin >> vault_matrix[i_row][i_column];

    vault.matrix = vault_matrix;
    return vault;
}

bool verify(const struct vault& v) {
    for (int i_row = 0; i_row < v.nr_rows; i_row++)
        for (int i_column = 0; i_column < v.nr_columns; i_column++)
            if (v.matrix[i_row][i_column] != i_row + 1)
                return false;

    return true;
}

void rotate(struct vault& v, int x, int y, int side, bool undo) {
    int temp = v.matrix[y][x];

    // Rotate right
    if (side) {
        v.matrix[y][x] = v.matrix[y + 1][x];
        v.matrix[y + 1][x] = v.matrix[y + 1][x + 1];
        v.matrix[y + 1][x + 1] = v.matrix[y][x + 1];
        v.matrix[y][x + 1] = temp;
    }
        // Rotate left
    else {
        v.matrix[y][x] = v.matrix[y][x + 1];
        v.matrix[y][x + 1] = v.matrix[y + 1][x + 1];
        v.matrix[y + 1][x + 1] = v.matrix[y + 1][x];
        v.matrix[y + 1][x] = temp;
    }

    if (undo)
        v.max_moves++;
    else
        v.max_moves--;
}

bool preprocess(const struct vault& v) {
    // 2x2 diagonal matrix check
    if (v.nr_rows == 2 && v.nr_columns == 2)
        if (v.matrix[0][0] == v.matrix[1][1] || v.matrix[0][1] == v.matrix[1][0]) {
            if (verbose) std::cout << "* 2x2 diagonal matrix: check failed" << std::endl;
            return false;
        }
    // Number quantity check
    std::vector<int> numbers(v.nr_rows);
    for (int row = 0; row < v.nr_rows; row++)
        for (int column = 0; column < v.nr_columns; column++)
            numbers[v.matrix[row][column] - 1]++;
    for (int i = 0; i < v.nr_rows; i++)
        if (numbers[i] != v.nr_columns) {
            std::cout << "* Number quantity: check failed" << std::endl;
            return false;
        }

    // Minimum amout of moves check
    for (int i = 0; i < v.nr_rows; i++) {
        int required = 0;
        for (int j = 0; j < v.nr_columns; j++) {
            if (v.matrix[i][j] > i) required += abs(v.matrix[i][j] - i) - 1;
            else required += abs(v.matrix[i][j] - i) + 1;
        }
        if (required > v.max_moves) {
            if (verbose) std::cout << "* Minimum amout of moves: check failed" << std::endl;
            return false;
        }
    }
    return true;
}

//TODO: Function to lock lines
void lock() {}

int process(struct vault& v) {
    if (v.best >= v.max_moves) return -1;
    if (verify(v)) return v.max_moves;
    if (v.max_moves == 0) return -1;

    int best_remaining = -1; // From all iterations

    for (int i_row = 0; i_row < v.nr_rows - 1; i_row++) {
        for (int i_column = 0; i_column < v.nr_columns - 1; i_column++) {
            int remaining; // From current iteration

            // Rotate left
            rotate(v, i_column, i_row, LEFT, false);
            remaining = process(v);
            if (remaining > best_remaining) best_remaining = remaining;
            rotate(v, i_column, i_row, RIGHT, true);

            // Rotate right
            rotate(v, i_column, i_row, RIGHT, false);
            remaining = process(v);
            if (remaining > best_remaining) best_remaining = remaining;
            rotate(v, i_column, i_row, LEFT, true);
        }
    }

    if (best_remaining > v.best)
        v.best = best_remaining;

    return v.best;
}

int main(int argc, char const *argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-d") verbose = true;

    std::clock_t start = std::clock();
    double duration;

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int nr_vaults;
    std::cin >> nr_vaults;

    for (int i = 0; i < nr_vaults; i++) {
        int answer = -1;
        struct vault v = read();

        if (verify(v)) answer = v.max_moves;
        else if (preprocess(v)) answer = process(v);

        if (answer == -1) std::cout << "the treasure is lost!\n";
        else std::cout << v.max_moves - answer << std::endl;
    }

    duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
    if (verbose) std::cout << "* Chronometer: " << duration << " seconds" << std::endl;
    return 0;
}