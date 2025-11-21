#!/bin/bash

# Program to execute
PROGRAM="./objpool"

# Number of runs
RUNS=100

# Check if the program exists
if [ ! -f "$PROGRAM" ]; then
  echo "Error: Program $PROGRAM not found."
  exit 1
fi

# Initialize total time
TOTAL_TIME=0

# Run the program 100 times
for ((i=1; i<=RUNS; i++))
do
  echo "Running iteration $i..."

  # Measure time using `time` command
  TIME_OUTPUT=$(/usr/bin/time -f "%e" $PROGRAM 2>&1| tail -n1)

  # Extract elapsed time in seconds and convert to milliseconds
  ELAPSED_MS=$(awk -v time="$TIME_OUTPUT" 'BEGIN { printf("%.0f", time * 1000) }')
  TOTAL_TIME=$((TOTAL_TIME + ELAPSED_MS))

  echo "Iteration $i took $ELAPSED_MS ms"
done

# Calculate average time
AVERAGE_TIME=$((TOTAL_TIME / RUNS))

echo "-----------------------------------"
echo "Total time for $RUNS runs: ${TOTAL_TIME} ms"
echo "Average time per run: ${AVERAGE_TIME} ms"
echo "-----------------------------------"

