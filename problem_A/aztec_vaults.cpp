/*
 * Read the problem A - "Aztec Vault" in EA2024_Trabalho1 at Mooshak (or aztec_vaults.md).
 */

#include <iostream>
#include <map>
#include <vector>

// Stores information about a vault
struct vault
{
    std::vector<std::vector<int>> matrix;
    int max_moves = 0;

    // Define the less than operator for two vaults
    bool operator<(const struct vault &other) const
    {
        // Compare matrices lexicographically
        if (matrix != other.matrix)
        {
            return matrix < other.matrix;
        }

        // If matrices are equal, compare max_moves
        return max_moves < other.max_moves;
    }
};

// Reads the input in the format given to store vault's information needed for the problem
// Stores the information in the given matrix
struct vault read_vault()
{
    struct vault vault;
    int nr_rows, nr_columns, nr_max_moves;
    std::cin >> nr_rows >> nr_columns >> nr_max_moves;

    // Declare the matrix to represent the vault with the given sizes
    std::vector<std::vector<int>> vault_matrix(nr_rows, std::vector<int>(nr_columns));

    // Initialize the vault with the values coming from the input
    for (int i_row = 0; i_row < nr_rows; i_row++)
        for (int i_column = 0; i_column < nr_columns; i_column++)
            std::cin >> vault_matrix[i_row][i_column];

    vault.matrix = vault_matrix;
    vault.max_moves = nr_max_moves;
    return vault;
}

// Rotates the matrix in the given 2x2 block.
// If rotate_right == false, rotate the 2x2 block left instead
struct vault rotate_handle(struct vault vault, int handle_x, int handle_y, bool rotate_right = true)
{
    int temp = vault.matrix[handle_y][handle_x];

    if (rotate_right)
    {
        vault.matrix[handle_y][handle_x] = vault.matrix[handle_y + 1][handle_x];
        vault.matrix[handle_y + 1][handle_x] = vault.matrix[handle_y + 1][handle_x + 1];
        vault.matrix[handle_y + 1][handle_x + 1] = vault.matrix[handle_y][handle_x + 1];
        vault.matrix[handle_y][handle_x + 1] = temp;
    }
    else
    {
        vault.matrix[handle_y][handle_x] = vault.matrix[handle_y][handle_x + 1];
        vault.matrix[handle_y][handle_x + 1] = vault.matrix[handle_y + 1][handle_x + 1];
        vault.matrix[handle_y + 1][handle_x + 1] = vault.matrix[handle_y + 1][handle_x];
        vault.matrix[handle_y + 1][handle_x] = temp;
    }

    vault.max_moves--;
    return vault;
}

// Whether the vault is solved or not
bool opened_vault(const struct vault vault)
{
    int nr_rows = vault.matrix.size();
    int nr_columns = vault.matrix[0].size();

    for (int i_row = 0; i_row < nr_rows; i_row++)
        for (int i_column = 0; i_column < nr_columns; i_column++)
            if (vault.matrix[i_row][i_column] != i_row + 1)
                return false;

    return true;
}

// Counts the given vault minimum rotations needed to solve it.
// If the minimum rotations exceed the maximum, return -1
int solve_vault_dfs(struct vault vault, std::map<struct vault, int> &computed_vaults)
{
    // If the vault has been computed
    if (computed_vaults.find(vault) != computed_vaults.end())
        return computed_vaults[vault];

    if (opened_vault(vault))
    {
        computed_vaults[vault] = vault.max_moves;
        return vault.max_moves;
    }
    else if (vault.max_moves == 0)
    {
        computed_vaults[vault] = -1;
        return -1;
    }

    int nr_rows = vault.matrix.size();
    int nr_columns = vault.matrix[0].size();

    // The values below will always be smaller than the current remaining moves
    int max_right_moves = -1;
    int max_left_moves = -1;

    // Create every possibility with the given vault and store
    // the one that has the vault solved with the most number of moves remaining
    for (int i_row = 0; i_row < nr_rows - 1; i_row++)
    {
        for (int i_column = 0; i_column < nr_columns - 1; i_column++)
        {
            int left_moves, right_moves;
            right_moves = solve_vault_dfs(rotate_handle(vault, i_column, i_row), computed_vaults);
            if (right_moves > max_right_moves)
                max_right_moves = right_moves;

            left_moves = solve_vault_dfs(rotate_handle(vault, i_column, i_row, false), computed_vaults);
            if (left_moves > max_left_moves)
                max_left_moves = left_moves;
        }
    }

    // Choose the case that had the highest remaining moves
    int max_remaining_moves = max_right_moves > max_left_moves ? max_right_moves : max_left_moves;

    computed_vaults[vault] = max_remaining_moves;
    return max_remaining_moves;
}

// Count the occurrence of a pattern, which is represented by an integer, within a given vault
int count_pattern_occurrence(const struct vault vault, int pattern)
{
    int nr_rows = vault.matrix.size();
    int nr_columns = vault.matrix[0].size();
    int pattern_occurrence = 0;

    for (int i_row = 0; i_row < nr_rows; i_row++)
        for (int i_column = 0; i_column < nr_columns; i_column++)
            pattern_occurrence += vault.matrix[i_row][i_column] == pattern;

    return pattern_occurrence;
}

// Whether the vault is solvable or not
bool solvable_vault(const struct vault vault)
{
    int nr_rows = vault.matrix.size();
    int nr_columns = vault.matrix[0].size();

    for (int pattern = 1; pattern < nr_rows + 1; pattern++)
    {
        if (count_pattern_occurrence(vault, pattern) != nr_columns)
            return false;
    }
    return true;
}

// Begins the recursive process of solving a vault
int solve_vault(const struct vault vault)
{
    if (!solvable_vault(vault))
        return -1;

    // computed_vaults will be used for memoization to optimize the function
    std::map<struct vault, int> computed_vaults;
    return solve_vault_dfs(vault, computed_vaults);
}

int main()
{
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

    return 0;
}