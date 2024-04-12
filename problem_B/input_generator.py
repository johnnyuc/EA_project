import sys
import random

random.seed(0)
# Open file
f = open("problem_B/inputs/aztec_cards_input_generated.txt", "w")
f.write("100\n")
for i in range(1, 100):
    nr_columns = random.randint(2, 24)
    nr_rows = random.randint(2, 24)
    cards_per_column = random.randint(1, 7)
    cards_per_row = random.randint(1, 7)

    # Write to file
    f.write(f"{nr_columns} {nr_rows}\n{cards_per_column} {cards_per_row}\n")

f.close()

