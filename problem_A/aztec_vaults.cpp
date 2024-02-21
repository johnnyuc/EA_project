/* Read the problem A - "Aztec Vault" in EA2024_Trabalho1 at Mooshak (or aztec_vaults.md).
 */

#include <cmath>
#include <iostream>
#include <vector>

// Stores information about a vault
struct vault
{
    std::vector<std::vector<int>> representation;
    int max_moves;
};

// Reads the input in the format given to store vault's information needed for the problem
// Stores the information in the given matrix
struct vault read_vault_info()
{
    struct vault ret_vault;
    int nr_rows, nr_columns, nr_max_moves;
    std::cin >> nr_rows >> nr_columns >> nr_max_moves;

    // Declare the matrix to represent the vault with the given sizes
    std::vector<std::vector<int>> vault_representation(nr_rows, std::vector<int>(nr_columns));

    // Initialize the vault with the values coming from the input
    for (int i_row = 0; i_row < nr_rows; i_row++)
        for (int i_column = 0; i_column < nr_columns; i_column++)
            std::cin >> vault_representation[i_row][i_column];

    ret_vault.representation = vault_representation;
    ret_vault.max_moves = nr_max_moves;
    return ret_vault;
}

// Rotates the matrix in the given 2x2 block.
// If rotate_right == false, rotate the 2x2 block left instead
void rotate_handle(vault &vault, int handle_x, int handle_y, bool rotate_right = true)
{
}

// Counts the given vault minimum rotations needed to solve it.
// If the minimun rotations exceed the maximum, return -1
int solve_vault(struct vault vault)
{
}

int main()
{
    // Makes IO faster
    std::ios_base::sync_with_stdio(0);
    std::cin.tie(0);

    int nr_test_cases;
    std::cin >> nr_test_cases;

    for (int i = 0; i < 3; i++)
    {
        int min_rotations = solve_vault(read_vault_info());

        if (min_rotations == -1)
            std::cout << "the treasure is lost!\n";
        else
            std::cout << min_rotations;
    }

    return 0;
}