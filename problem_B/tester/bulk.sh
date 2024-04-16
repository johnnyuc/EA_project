#!/bin/bash

# Function to validate input
validate_input() {
    if [[ ! $1 =~ ^[0-9]+$ ]]; then
        echo "Error: $2 must be a positive integer." >&2
        exit 1
    fi
}

# Function to print current sizes
print_sizes() {
    echo "Current sizes: N = $1, M = $2, C = $3, R = $4"
}

# Check if correct number of arguments are provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <program>"
    exit 1
fi

PROGRAM="$1"

# Validate program path
if [ ! -x "$PROGRAM" ]; then
    echo "Error: Program '$PROGRAM' not found or not executable." >&2
    exit 1
fi

# Iterate through all combinations of N and M
for (( N = 2; N <= 14; N++ )); do
    for (( M = 2; M <= 14; M++ )); do
        # Iterate through all combinations of C and R where N*C == M*R
        for (( C = 1; C <= N; C++ )); do
            R=$(( M * C / N ))
            if [[ $(( M * C )) -eq $(( N * R )) ]]; then
                # Write to the output file
                FILENAME="generator"
                echo "1" > "$FILENAME"
                echo "$N $M" >> "$FILENAME"
                echo "$C $R" >> "$FILENAME"
                
                # Print current sizes
                print_sizes "$N" "$M" "$C" "$R"
                
                # Run the program with the generated input file and measure execution time
                START_TIME=$(date +%s.%N)
                "$PROGRAM" < "$FILENAME" > /dev/null
                END_TIME=$(date +%s.%N)
                
                # Calculate execution time in milliseconds
                EXECUTION_TIME=$(echo "($END_TIME - $START_TIME) * 1000" | bc -l | awk '{printf "%.2f", $1}')
                echo "Execution time: ${EXECUTION_TIME} milliseconds"
            fi
        done
    done
done

echo "Execution completed."
