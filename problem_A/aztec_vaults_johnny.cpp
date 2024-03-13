#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

#include <thread> //! Debug

#define LEFT 0
#define RIGHT 1

bool verbose = false;
int count = 0;

struct vault
{
    int best = -1;
    int nr_rows{};
    int nr_columns{};
    int max_moves{};

    std::vector<std::vector<int>> matrix;
};

struct vault read()
{
    struct vault v;
    std::cin >> v.nr_rows >> v.nr_columns >> v.max_moves;

    std::vector<std::vector<int>> v_matrix(v.nr_rows, std::vector<int>(v.nr_columns));

    for (int i_row = 0; i_row < v.nr_rows; i_row++)
        for (int i_column = 0; i_column < v.nr_columns; i_column++)
            std::cin >> v_matrix[i_row][i_column];

    v.matrix = v_matrix;
    return v;
}

bool verify(const struct vault &v)
{
    for (int i_row = 0; i_row < v.nr_rows; i_row++)
        for (int i_column = 0; i_column < v.nr_columns; i_column++)
            if (v.matrix[i_row][i_column] != i_row + 1)
                return false;

    return true;
}

void rotate(struct vault &v, int x, int y, bool side, bool undo)
{
    count++;
    int temp = v.matrix[y][x];

    // Rotate right
    if (side)
    {
        v.matrix[y][x] = v.matrix[y + 1][x];
        v.matrix[y + 1][x] = v.matrix[y + 1][x + 1];
        v.matrix[y + 1][x + 1] = v.matrix[y][x + 1];
        v.matrix[y][x + 1] = temp;
    }
    // Rotate left
    else
    {
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

// "Locks" rows by removing solved rows from the vault
// Must be used after preprocessing or else errors may occur
struct vault lock_rows(const struct vault vault)
{
    struct vault locked_vault = vault;
    bool ignore_top_row = false;
    bool ignore_bot_row = false;
    int top_rows_removed = 0;

    for (int i_row = 0; i_row < vault.nr_rows; i_row++)
    {
        bool lock_top_row = true;
        bool lock_bot_row = true;

        for (int i_column = 0; i_column < vault.nr_columns; i_column++)
        {
            // In both the below cases, we will be checking if the current row is solved
            // If it isn't, mark them as not needing to be locked

            // First row going down each iteration
            if (vault.matrix[i_row][i_column] != i_row + 1)
                lock_top_row = false;
            // Last row going up each iteration
            if (vault.matrix[vault.nr_rows - i_row - 1][i_column] != vault.nr_rows - i_row)
                lock_bot_row = false;
        }

        // Treat the rows that need to be locked
        // If the top row was solved, remove it from the matrix
        if (!ignore_top_row && lock_top_row)
        {
            locked_vault.matrix.erase(locked_vault.matrix.begin());
            locked_vault.nr_rows--;
            top_rows_removed++;
        }
        // Else, set to ignore the next rows coming from the top, because rows cant be locked if the rows above them are not solved
        else
            ignore_top_row = true;

        // If the bottom row was solved, remove it from the matrix
        if (!ignore_bot_row && lock_bot_row)
        {
            locked_vault.matrix.pop_back();
            locked_vault.nr_rows--;
        }
        // Else, set to ignore the next rows coming from the bottom, because rows cant be locked if the rows below them are not solved
        else
            ignore_bot_row = true;

        // If both top and bottom rows are set to be ignored, no need going through the cycle again
        if (ignore_bot_row && ignore_top_row)
            break;
    }

    for (int i_row = 0; i_row < locked_vault.nr_rows; i_row++)
        for (int i_column = 0; i_column < locked_vault.nr_columns; i_column++)
            locked_vault.matrix[i_row][i_column] -= top_rows_removed;

    return locked_vault;
}

struct vault rotate_vault(const struct vault vault, bool side)
{
    struct vault rotated_vault;
    rotated_vault.matrix = std::vector<std::vector<int>>(vault.nr_columns, std::vector<int>(vault.nr_rows));

    if (side == LEFT)
    {
        for (int i_row = vault.nr_rows - 1; i_row >= 0; i_row--)
            for (int i_column = 0; i_column < vault.nr_columns; i_column++)
                rotated_vault.matrix[i_column][i_row] = vault.matrix[i_row][i_column];
        // After transposing the vault, the rows are reversed, so reverse them to be in the order needed
        std::reverse(rotated_vault.matrix.begin(), rotated_vault.matrix.end());
    }
    else
    {
        for (int i_row = 0; i_row < vault.nr_rows; i_row++)
            for (int i_column = 0; i_column < vault.nr_columns; i_column++)
                rotated_vault.matrix[i_column][i_row] = vault.matrix[i_row][i_column];
        // After transposing the vault, the columns are reversed, so reverse them to be in the order needed
        for (std::vector<int> &row : rotated_vault.matrix)
            std::reverse(row.begin(), row.end());
    }

    rotated_vault.nr_rows = vault.nr_columns;
    rotated_vault.nr_columns = vault.nr_rows;
    rotated_vault.max_moves = vault.max_moves;

    return rotated_vault;
}

struct vault lock_columns(const struct vault vault)
{
    // Rotate the vault so the removal of the columns will be easier, since they are now rows
    struct vault locked_vault = rotate_vault(vault, LEFT);
    bool ignore_left_column = false;
    bool ignore_right_column = false;

    for (int i_column = 0; i_column < vault.nr_columns; i_column++)
    {
        bool lock_left_column = true;
        bool lock_right_column = true;

        for (int i_row = 0; i_row < vault.nr_rows; i_row++)
        {
            // In both the below cases, we will be checking if the current column is solved.
            // If it isn't, mark them as not needing to be locked

            // Left column going right each iteration
            if (vault.matrix[i_row][i_column] != i_row + 1)
                lock_left_column = false;
            // Right column going left each iteration
            if (vault.matrix[i_row][vault.nr_columns - i_column - 1] != i_row + 1)
                lock_right_column = false;
        }
        // Treat the columns that need to be locked
        // If the left column was solved, remove it from the matrix
        if (!ignore_left_column && lock_left_column)
        {
            // The left column is removed by erasing the bottom row in the matrix because it was rotated
            locked_vault.matrix.pop_back();
            // The number of rows are decreased and not columns because when the rotation is undone,
            // the nr of rows becomes the nr of columns, and vice-versa
            locked_vault.nr_rows--;
        }
        // Else, set to ignore the next columns coming from the left,
        // because left columns can't be locked if the column left of them are not solved
        else
            ignore_left_column = true;

        // If the nr of rows (which will be the nr of columns) are 3, stop locking because a n*2 matrix can have impossible cases
        if (locked_vault.nr_rows == 2)
            break;

        // If the right column was solved, remove it from the matrix
        if (!ignore_right_column && lock_right_column)
        {
            // The right column is removed by erasing the top row in the matrix because it was rotated
            locked_vault.matrix.erase(locked_vault.matrix.begin());
            // The number of rows are decreased and not columns because when the rotation is undone,
            // the nr of rows becomes the nr of columns, and vice-versa
            locked_vault.nr_rows--;
        }
        // Else, set to ignore the next columns coming from the right,
        // because right columns can't be locked if the column right of them are not solved
        else
            ignore_right_column = true;

        // If both left and right columns are set to be ignored, no need going through the cycle again
        // Or if the nr of rows (which will be the nr of columns) are 2 because a n*1 matrix can't be solved
        if (ignore_right_column && ignore_left_column || locked_vault.nr_rows == 2)
            break;
    }

    return rotate_vault(locked_vault, RIGHT);
}

bool preprocess(const struct vault &v)
{
    // TODO: Adapt to make it work on not only 2x2 diagonal matrixes
    // 2x2 diagonal matrix check
    if (v.nr_rows == 2 && v.nr_columns == 2)
        if (v.matrix[0][0] == v.matrix[1][1] || v.matrix[0][1] == v.matrix[1][0])
        {
            if (verbose)
                std::cout << "\033[1;31m[Fail: 2x2 diagonal matrix]\033[0m" << std::endl;
            return false;
        }
    // Number quantity check
    std::vector<int> numbers(v.nr_rows);
    for (int row = 0; row < v.nr_rows; row++)
        for (int column = 0; column < v.nr_columns; column++)
            numbers[v.matrix[row][column] - 1]++;
    for (int i = 0; i < v.nr_rows; i++)
        if (numbers[i] != v.nr_columns)
        {
            if (verbose)
                std::cout << "\033[1;31m[Fail: number quantity]\033[0m" << std::endl;
            return false;
        }

    return true;
}

int process(struct vault &v)
{
    if (v.best >= v.max_moves)
        return -1;
    if (verify(v))
        return v.max_moves;
    if (v.max_moves == 0)
        return -1;

    // Minimum amount of moves check
    for (int i = 0; i < v.nr_rows; i++)
        for (int j = 0; j < v.nr_columns; j++)
            if (v.matrix[i][j] - (i + 1) > v.max_moves)
                return -1;

    int best = -1;

    for (int i_row = 0; i_row < v.nr_rows - 1; i_row++)
    {
        for (int i_column = 0; i_column < v.nr_columns - 1; i_column++)
        {
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

int main(int argc, char const *argv[])
{
    if (argc > 1 && std::string(argv[1]) == "-d")
        verbose = true;

    std::clock_t total_time = std::clock();

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int nr_vaults;
    std::cin >> nr_vaults;

    for (int i = 0; i < nr_vaults; i++)
    {
        count = 0;
        double duration;
        std::clock_t case_time = std::clock();

        int best = -1;
        struct vault v = read();

        if (verify(v))
            best = v.max_moves;
        else if (preprocess(v))
        {
            v = lock_rows(v);
            best = process(v);
        }

        if (best == -1)
            std::cout << "the treasure is lost!\n";
        else
            std::cout << v.max_moves - best << std::endl;

        duration = (std::clock() - case_time) / (double)CLOCKS_PER_SEC;
        if (verbose)
        {
            std::cout << "\033[1;32m[Chronometer: " << duration << " seconds]\033[0m" << std::endl;
            std::cout << "\033[1;32m[Rotation count: " << count << "]\033[0m" << std::endl;
        }
    }

    if (verbose)
    {
        double total_duration = (std::clock() - total_time) / (double)CLOCKS_PER_SEC;
        std::cout << "\033[1;33m[Total time: " << total_duration << " seconds]\033[0m" << std::endl;
    }

    return 0;
}