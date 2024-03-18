/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <iostream>
#include <vector>

struct card_grid
{
    int nr_rows{};
    int nr_columns{};
    int cards_per_row{};
    int cards_per_column{};

    std::vector<std::vector<int>> matrix;
};

// Reads the input and returns a card grid structure
struct card_grid read_input()
{
    struct card_grid card_grid;
    std::cin >> card_grid.nr_columns >> card_grid.nr_rows;
    std::cin >> card_grid.cards_per_row >> card_grid.cards_per_column;

    // Initialize the card grid matrix with zeros
    card_grid.matrix = std::vector<std::vector<int>>(card_grid.nr_rows, std::vector<int>(card_grid.nr_columns, 0));

    return card_grid;
}

int process(struct card_grid card_grid)
{
    return 1;
}

int main(int argc, char const *argv[])
{
    int nr_tests;
    std::cin >> nr_tests;

    for (int i = 0; i < nr_tests; i++)
        std::cout << process(read_input()) << std::endl;

    return 0;
}
