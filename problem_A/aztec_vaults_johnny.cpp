#include <iostream>
#include <vector>
#include <ctime>

#define LEFT 0
#define RIGHT 1

bool verbose = false;
int count_locks = 0, count = 0;

struct vault {
    int best = -1;
    int nr_rows{};
    int nr_columns{};
    int max_moves{};

    std::vector<std::vector<int>> matrix;
};

struct vault read() {
    struct vault v;
    std::cin >> v.nr_rows >> v.nr_columns >> v.max_moves;

    std::vector<std::vector<int>> v_matrix(v.nr_rows, std::vector<int>(v.nr_columns));

    for (int i_row = 0; i_row < v.nr_rows; i_row++)
        for (int i_column = 0; i_column < v.nr_columns; i_column++)
            std::cin >> v_matrix[i_row][i_column];

    v.matrix = v_matrix;
    return v;
}

bool verify(const struct vault& v) {
    for (int i_row = 0; i_row < v.nr_rows; i_row++)
        for (int i_column = 0; i_column < v.nr_columns; i_column++)
            if (v.matrix[i_row][i_column] != i_row + 1)
                return false;

    return true;
}

void rotate(struct vault& v, int x, int y, int side, bool undo) {
    count++;
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

    if (undo) v.max_moves++;
    else v.max_moves--;
}

bool preprocess(const struct vault& v) {
    // 2x2 diagonal matrix check
    if (v.nr_rows == 2 && v.nr_columns == 2)
        if (v.matrix[0][0] == v.matrix[1][1] || v.matrix[0][1] == v.matrix[1][0]) {
            if (verbose) std::cout << "\033[1;31m[Fail: 2x2 diagonal matrix]\033[0m" << std::endl;
            return false;
        }
    // Number quantity check
    std::vector<int> numbers(v.nr_rows);
    for (int row = 0; row < v.nr_rows; row++)
        for (int column = 0; column < v.nr_columns; column++)
            numbers[v.matrix[row][column] - 1]++;
    for (int i = 0; i < v.nr_rows; i++)
        if (numbers[i] != v.nr_columns) {
            std::cout << "\033[1;31m[Fail: number quantity]\033[0m" << std::endl;
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
            if (verbose) std::cout << "\033[1;31m[Fail: minimum amout of moves]\033[0m" << std::endl;
            return false;
        }
    }
    return true;
}

void lock(struct vault& v, std::vector<bool>& locks) {
    // Top to bottom
    for (int i = 0; i < v.nr_rows; i++) {
        locks[i] = true;
        for (int j = 0; j < v.nr_columns; j++)
            if (v.matrix[i][j] != i + 1) {
                locks[i] = false;
                break; // Inner
            }
        if (!locks[i]) break; // Outer
    }

    // Bottom to top
    for (int i = v.nr_rows - 1; i >= 0; i--) {
        locks[i] = true;
        for (int j = 0; j < v.nr_columns; j++)
            if (v.matrix[i][j] != i + 1) {
                locks[i] = false;
                break; // Inner
            }
        if (!locks[i]) break; // Outer
    }
}

int process(struct vault& v) {
    std::vector<bool> locks(v.nr_rows, false);
    lock(v, locks);

    if (v.best >= v.max_moves || v.max_moves == 0) return -1;
    if (verify(v)) return v.max_moves;

    int best = -1;

    for (int i_row = 0; i_row < v.nr_rows - 1; i_row++) {
        for (int i_column = 0; i_column < v.nr_columns - 1; i_column++) {

            // Locked rows
            if (locks[i_row]) continue;

            // Rotate left
            rotate(v, i_column, i_row, LEFT, false);
            best = std::max(process(v), best);
            rotate(v, i_column, i_row, RIGHT, true);

            // Rotate right
            rotate(v, i_column, i_row, RIGHT, false);
            best = std::max(process(v), best);
            rotate(v, i_column, i_row, LEFT, true);
        }
    }

    if (best > v.best)
        v.best = best;

    return v.best;
}

int main(int argc, char const *argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-d") verbose = true;

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int nr_vaults;
    std::cin >> nr_vaults;

    for (int i = 0; i < nr_vaults; i++) {
        count = 0;
        std::clock_t start = std::clock();
        double duration;

        int best = -1;
        struct vault v = read();

        if (verify(v)) best = v.max_moves;
        else if (preprocess(v)) best = process(v);

        if (best == -1) std::cout << "the treasure is lost!\n";
        else std::cout << v.max_moves - best << std::endl;

        duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
        if (verbose) std::cout << "\033[1;32m[Chronometer: " << duration << " seconds]\033[0m" << std::endl;
        std::cout << "\033[1;32m[Count: " << count << "| Count locks: " << count_locks << "]\033[0m" << std::endl;
    }

    return 0;
}