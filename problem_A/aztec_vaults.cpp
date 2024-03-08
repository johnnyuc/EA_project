/*
 * Read the problem A - "Aztec Vault" in EA2024_Trabalho1 at Mooshak (or aztec_vaults.md).
 */

#include <iostream>
#include <vector>
#include <ctime>
#include <string.h>

// Stores information about a vault
struct vault
{
    std::vector<std::vector<int>> matrix;
    int max_moves = 0;
    int nr_rows;
    int nr_columns;
};

// Reads the input in the format given to store vault's information needed for the problem
// Stores the information in the given matrix
struct vault read_vault()
{
    struct vault vault;
    std::cin >> vault.nr_rows >> vault.nr_columns >> vault.max_moves;

    // Declare the matrix to represent the vault with the given sizes
    std::vector<std::vector<int>> vault_matrix(vault.nr_rows, std::vector<int>(vault.nr_columns));

    // Initialize the vault with the values coming from the input
    for (int i_row = 0; i_row < vault.nr_rows; i_row++)
        for (int i_column = 0; i_column < vault.nr_columns; i_column++)
            std::cin >> vault_matrix[i_row][i_column];

    vault.matrix = vault_matrix;
    return vault;
}

// Rotates the matrix in the given 2x2 block.
// If rotate_right == false, rotate the 2x2 block left instead
// "undo" is set to "true" if the call should be undoing a rotation and restoring the previous state
void rotate_handle(struct vault *vault, int handle_x, int handle_y, bool rotate_right, bool undo)
{
    int temp = vault->matrix[handle_y][handle_x];

    if (rotate_right)
    {
        vault->matrix[handle_y][handle_x] = vault->matrix[handle_y + 1][handle_x];
        vault->matrix[handle_y + 1][handle_x] = vault->matrix[handle_y + 1][handle_x + 1];
        vault->matrix[handle_y + 1][handle_x + 1] = vault->matrix[handle_y][handle_x + 1];
        vault->matrix[handle_y][handle_x + 1] = temp;
    }
    else
    {
        vault->matrix[handle_y][handle_x] = vault->matrix[handle_y][handle_x + 1];
        vault->matrix[handle_y][handle_x + 1] = vault->matrix[handle_y + 1][handle_x + 1];
        vault->matrix[handle_y + 1][handle_x + 1] = vault->matrix[handle_y + 1][handle_x];
        vault->matrix[handle_y + 1][handle_x] = temp;
    }

    if (undo)
        vault->max_moves++;
    else
        vault->max_moves--;
}

// Whether the vault is solved or not
bool opened_vault(const struct vault vault)
{
    for (int i_row = 0; i_row < vault.nr_rows; i_row++)
        for (int i_column = 0; i_column < vault.nr_columns; i_column++)
            if (vault.matrix[i_row][i_column] != i_row + 1)
                return false;

    return true;
}
// Counts the given vault minimum rotations needed to solve it.
// If the minimum rotations exceed the maximum, return -1
int solve_vault_dfs(struct vault *vault, int *best_remaining_moves)
{
    // Cut redundant cases by checking if the current state already has
    // fewer remaining moves than the best remaining moves found
    if (*best_remaining_moves >= vault->max_moves)
        return -1;

    if (opened_vault(*vault))
        return vault->max_moves;

    if (vault->max_moves == 0)
        return -1;

    // The value below will always be equal or smaller than the first move returned from the recursion
    int max_remaining_moves = -1;

    // Create every possibility with the given vault and store
    // the one that has the vault solved with the most number of moves remaining
    for (int i_row = 0; i_row < vault->nr_rows - 1; i_row++)
    {
        for (int i_column = 0; i_column < vault->nr_columns - 1; i_column++)
        {
            int remaining_moves;

            // Rotate the vault right and see the maximum amount of remaining moves the resulting state will return
            rotate_handle(vault, i_column, i_row, true, false);
            remaining_moves = solve_vault_dfs(vault, best_remaining_moves);
            if (remaining_moves > max_remaining_moves)
                max_remaining_moves = remaining_moves;
            // Restore the vault to the previous state
            rotate_handle(vault, i_column, i_row, false, true);

            // Rotate the vault left and see the maximum amount of remaining moves the resulting state will return
            rotate_handle(vault, i_column, i_row, false, false);
            remaining_moves = solve_vault_dfs(vault, best_remaining_moves);
            if (remaining_moves > max_remaining_moves)
                max_remaining_moves = remaining_moves;
            // Restore the vault to the previous state
            rotate_handle(vault, i_column, i_row, true, true);
        }
    }

    // If the current solution has more remaining moves than the previous best, set it as the new best
    if (max_remaining_moves > *best_remaining_moves)
        *best_remaining_moves = max_remaining_moves;

    return max_remaining_moves;
}

void lock_row(std::vector<int> *row)
{
}

struct vault lock_solved_rows(const struct vault vault)
{
    struct vault locked_vault = vault;
    bool ignore_top_row = false;
    bool ignore_bot_row = false;

    // TODO: Analyze how the cycle will know when the top and bottom rows hit each other
    for (int i_row = 0; i_row < vault.nr_rows; i_row++)
    {
        bool lock_top_row = true;
        bool lock_bot_row = true;

        for (int i_column = 0; i_column < vault.nr_columns; i_column++)
        {
            locked_vault.matrix[i_row][i_column]--;
            locked_vault.matrix[vault.nr_rows - i_row - 1][vault.nr_columns - i_column - 1]--;

            // In both the below cases, we will be checking if the current row is solved. If it is, mark them as needing to be locked.
            // This will be treated later

            // First row going down each iteration
            if (vault.matrix[i_row][i_column] != i_row + 1)
                lock_top_row = false;
            // Last row going up each iteration
            if (vault.matrix[vault.nr_rows - i_row - 1][vault.nr_columns - i_column - 1] != vault.nr_rows - i_row)
                lock_bot_row = false;
        }

        // Treat the rows that need to be locked
        // If the top row was solved, remove it from the matrix
        if (!ignore_top_row && lock_top_row)
            locked_vault.matrix.erase(locked_vault.matrix.begin());
        // Else, set to ignore the next rows coming from the top, because rows cant be locked if the rows above them are not solved
        else
            ignore_top_row = true;

        // If the bottom row was solved, remove it from the matrix
        if (!ignore_bot_row && lock_bot_row)
            locked_vault.matrix.pop_back();
        // Else, set to ignore the next rows coming from the bottom, because rows cant be locked if the rows below them are not solved
        else
            ignore_top_row = true;

        // If both top and bottom rows are set to be ignored, no need going through the cycle again
        if (ignore_bot_row && ignore_top_row)
            break;
    }

    // TODO: Decrement every value in the matrix for each top row removed
}

// Count the occurrence of a pattern, which is represented by an integer, within a given vault
int count_pattern_occurrence(const struct vault vault, int pattern)
{
    int pattern_occurrence = 0;

    for (int i_row = 0; i_row < vault.nr_rows; i_row++)
        for (int i_column = 0; i_column < vault.nr_columns; i_column++)
            pattern_occurrence += vault.matrix[i_row][i_column] == pattern;

    return pattern_occurrence;
}

// Whether the vault is solvable or not
bool solvable_vault(const struct vault vault)
{
    // Verify if it's a 2x2 diagonal matrix
    if (vault.nr_rows == 2 && vault.nr_columns == 2)
        if (vault.matrix[0][0] == vault.matrix[1][1] || vault.matrix[0][1] == vault.matrix[1][0])
            return false;

    // Verify if the patterns are possible to be completed
    for (int pattern = 1; pattern < vault.nr_rows + 1; pattern++)
        if (count_pattern_occurrence(vault, pattern) != vault.nr_columns)
            return false;

    return true;
}

// Begins the recursive process of solving a vault
int solve_vault(struct vault vault)
{
    // Despite checking if it is open after the recursion begins,
    // checking here prevents errors when locking rows
    if (opened_vault(vault))
        return vault.max_moves;

    if (!solvable_vault(vault))
        return -1;

    int best_remaining_moves = -1;
    solve_vault_dfs(&vault, &best_remaining_moves);
    return best_remaining_moves;
}

int main(int argc, char const *argv[])
{
    // Debug
    std::clock_t start = std::clock();
    double duration;

    // Makes IO faster
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int nr_test_cases;
    std::cin >> nr_test_cases;

    for (int i = 0; i < nr_test_cases; i++)
    {
        struct vault vault = read_vault();
        int remaining_moves = solve_vault(vault);

        if (remaining_moves == -1)
            std::cout << "the treasure is lost!\n";
        else
            std::cout << vault.max_moves - remaining_moves << std::endl;
    }

    duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
    if (argc == 2 && strcmp(argv[1], "-d") == 0)
        std::cout << "Took " << duration << " seconds" << std::endl;

    return 0;
}