#!/bin/bash

# Function to validate input
validate_input() {
    if [[ ! $1 =~ ^[0-9]+$ ]]; then
        echo "Error: $2 must be a positive integer." >&2
        exit 1
    fi
}

# Check if correct number of arguments are provided
if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <N> <M> <C> <R>"
    exit 1
fi

# Validate input arguments
validate_input $1 "N"
validate_input $2 "M"
validate_input $3 "C"
validate_input $4 "R"

# Assign input arguments to variables
N=$1
M=$2
C=$3
R=$4

# Check if N and M are within range [2,14]
if (( N < 2 || N > 14 || M < 2 || M > 14 )); then
    echo "Error: N and M must be between 2 and 14." >&2
    exit 1
fi

# Check if C is within range [1,N] and R is within range [1,M]
if (( C < 1 || C > N || R < 1 || R > M )); then
    echo "Error: C must be between 1 and $N, and R must be between 1 and $M." >&2
    exit 1
fi

# Write to the output file
echo "$N $M" > output.txt
echo "$C $R" >> output.txt

echo "File 'output.txt' created successfully."
