/* 
 * Read the problem A - "Aztec Vault" in EA2024_Trabalho1 at Mooshak (or aztec_vaults.md).
 */

#include <iostream>
#include <vector>

// Stores information about a vault
struct vault {
    std::vector<std::vector<int>> matrix;
    int max_moves = 0; // Initialization
};

// Reads the input in the format given to store vault's information needed for the problem
// Stores the information in the given matrix
struct vault read_vault() {
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
void rotate_handle(vault &vault, int handle_x, int handle_y, bool rotate_right = true) {
    std::vector<std::vector<int>> &mat = vault.matrix;
    int temp = mat[handle_y][handle_x];

    if (rotate_right) {
        mat[handle_y][handle_x] = mat[handle_y + 1][handle_x];
        mat[handle_y + 1][handle_x] = mat[handle_y + 1][handle_x + 1];
        mat[handle_y + 1][handle_x + 1] = mat[handle_y][handle_x + 1];
        mat[handle_y][handle_x + 1] = temp;
    } else {
        mat[handle_y][handle_x] = mat[handle_y][handle_x + 1];
        mat[handle_y][handle_x + 1] = mat[handle_y + 1][handle_x + 1];
        mat[handle_y + 1][handle_x + 1] = mat[handle_y + 1][handle_x];
        mat[handle_y + 1][handle_x] = temp;
    }
}

// Counts the given vault minimum rotations needed to solve it.
// If the minimum rotations exceed the maximum, return -1
int solve_vault(const struct vault& vault) {
    const int nr_rows = static_cast<int>(vault.matrix.size());
    const int nr_columns = static_cast<int>(vault.matrix[0].size());
    const int max_moves = vault.max_moves;

    // Brute-force approach: try all possible combinations of rotations
    int min_rotations = INT_MAX;

    for (int i = 0; i < (1 << (nr_rows * nr_columns)); ++i) {
        // Apply rotations according to the bitmask
        struct vault current_vault = vault;
        int rotations = 0;
        for (int j = 0; j < nr_rows * nr_columns; ++j) {
            if (i & (1 << j)) {
                int row = j / nr_columns;
                int col = j % nr_columns;
                rotate_handle(current_vault, col, row);
                ++rotations;
            }
        }

        // Check if the current configuration solves the vault
        bool solved = true;
        for (int row = 0; row < nr_rows; ++row) {
            for (int col = 0; col < nr_columns; ++col) {
                if (current_vault.matrix[row][col] != 0) {
                    solved = false;
                    break;
                }
            }
            if (!solved) break;
        }

        // Update min_rotations if the current configuration is valid and requires fewer rotations
        if (solved) min_rotations = std::min(min_rotations, rotations);
    }

    // If no valid configuration found within the allowed moves, return -1
    return (min_rotations == INT_MAX || min_rotations > max_moves) ? -1 : min_rotations;
}

int main()
{
    // Makes IO faster
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int nr_test_cases;
    std::cin >> nr_test_cases;

    for (int i = 0; i < nr_test_cases; i++) {
        int min_rotations = solve_vault(read_vault());

        if (min_rotations == -1)
            std::cout << "the treasure is lost!\n";
        else
            std::cout << min_rotations;
    }

    return 0;
}