/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <iostream>
#include <vector>

#define LEFT 0
#define RIGHT 1

// Vault structure
struct vault {
    int best = -1;
    int nr_rows{};
    int nr_columns{};
    int max_moves{};

    std::vector<std::vector<int>> matrix;
};

// Reads the input and returns a vault structure
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

// Verifies if the vault is already solved
bool verify(const struct vault &v) {
    for (int i_row = 0; i_row < v.nr_rows; i_row++)
        for (int i_column = 0; i_column < v.nr_columns; i_column++)
            if (v.matrix[i_row][i_column] != i_row + 1)
                return false;

    return true;
}

// Rotates a vault submatrix - works by replacing values
void rotate(struct vault &v, int x, int y, bool side, bool undo) {
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

    // Restoring the number of moves or adding one
    if (undo)
        v.max_moves++;
    else
        v.max_moves--;
}

// Locks rows by removing solved rows from the vault - must be used after preprocessing
struct vault lock_rows(struct vault& v) {
    struct vault locked = v;
    bool ignore_top = false;
    bool ignore_bot = false;
    int digit_updater = 0;

    for (int i_row = 0; i_row < v.nr_rows; i_row++) {
        bool lock_top_row = true;
        bool lock_bot_row = true;

        for (int i_column = 0; i_column < v.nr_columns; i_column++) {
            // Checks if the row is solved
            // If it isn't, it will set the lock to false

            // First row going down each iteration
            if (v.matrix[i_row][i_column] != i_row + 1)
                lock_top_row = false;
            // Last row going up each iteration
            if (v.matrix[v.nr_rows - i_row - 1][i_column] != v.nr_rows - i_row)
                lock_bot_row = false;
        }

        // Removes the top row if it was solved
        if (!ignore_top && lock_top_row) {
            locked.matrix.erase(locked.matrix.begin());
            locked.nr_rows--;
            digit_updater++;
        }

        // Else ignores following rows going down
        // Rows can't be locked if the rows above them are not solved
        else
            ignore_top = true;

        // Removes the bottom row if it was solved
        if (!ignore_bot && lock_bot_row) {
            locked.matrix.pop_back();
            locked.nr_rows--;
        }

        // Else ignores following rows going up
        // Rows can't be locked if the rows below them are not solved
        else
            ignore_bot = true;

        // If both top and bottom rows are set to be ignored, no need going through the cycle again
        if (ignore_bot && ignore_top)
            break;
    }

    // Updates matrix and fixes the numbers if top rows were removed
    for (int i_row = 0; i_row < locked.nr_rows; i_row++)
        for (int i_column = 0; i_column < locked.nr_columns; i_column++)
            locked.matrix[i_row][i_column] -= digit_updater;

    return locked;
}

// Verifies if the vault can be solved with the current number of moves
bool min_moves(struct vault& v) {
    for (int i = 0; i < v.nr_rows; i++) {
        int required = 0;
        for (int j = 0; j < v.nr_columns; j++) {
            // Calculates distance from current line to the number's line-1
            // Depending on which is greater, it will add or subtract 1
            if (v.matrix[i][j] > i)
                required += abs(v.matrix[i][j] - i) - 1;
            else
                required += abs(v.matrix[i][j] - i) + 1;
        }
        if (required > v.max_moves)
            return false;
    }
    return true;
}

// Preprocesses the vault to check if it can be solved
// Checks for particular cases without solution
bool preprocess(const struct vault &v) {
    // 2x2 diagonal matrix check
    // For example, [[1, 2], [2, 1]] can't be solved
    if (v.nr_rows == 2 && v.nr_columns == 2)
        if (v.matrix[0][0] == v.matrix[1][1] || v.matrix[0][1] == v.matrix[1][0])
            return false;

    // Number quantity check
    // Counts the number of occurrences of each number and places them in a vector
    // If the number of occurrences is not equal to the number of columns, the vault can't be solved
    std::vector<int> numbers(v.nr_rows);
    for (int row = 0; row < v.nr_rows; row++)
        for (int column = 0; column < v.nr_columns; column++)
            numbers[v.matrix[row][column] - 1]++;
    for (int i = 0; i < v.nr_rows; i++)
        if (numbers[i] != v.nr_columns)
            return false;

    return true;
}

// Main function that processes the vault
// Uses DFS along with backtracking to find the best solution
int process(struct vault &v) {
    // Base cases
    if (v.best >= v.max_moves)
        return -1;
    if (verify(v))
        return v.max_moves;
    if (v.max_moves == 0)
        return -1;
    if (!min_moves(v))
        return -1;

    // Initializes the best variable
    // It will store the temporary best number of moves found
    int best = -1;

    // Iterates through the vault rotating each submatrix both left and right
    for (int i_row = 0; i_row < v.nr_rows - 1; i_row++) {
        for (int i_column = 0; i_column < v.nr_columns - 1; i_column++) {
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

    // Updates the best variable if a better solution was found
    if (best > v.best)
        v.best = best;

    return v.best;
}

int main() {
    // Makes IO faster
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int nr_vaults;
    std::cin >> nr_vaults;

    for (int i = 0; i < nr_vaults; i++) {
        int best = -1;
        struct vault v = read();

        // Checks if the vault is already solved
        if (verify(v))
            best = v.max_moves;
        // Checks if the vault can't be solved
        else if (preprocess(v)) {
            // Locks rows and processes the vault
            v = lock_rows(v);
            best = process(v);
        }

        if (best == -1)
            std::cout << "the treasure is lost!\n";
        else
            std::cout << v.max_moves - best << std::endl;
    }

    return 0;
}