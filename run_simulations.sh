#!/bin/bash

# Define the values you want to test
#bimodal_values=(2 4 8 16)  # You can adjust these values as needed

# Loop through each value
#for value in "${bimodal_values[@]}"; do
  #echo "Running simulation with bimodal value: $value"
  #./sim bimodal "$value" jpeg_trace.txt
  #echo "------------------------------------"
 #done
#./sim bimodal 4 jpeg_trace.txt

#!/bin/bash

# Define an array for the values of the second parameter
gshare_values1=(7 8 9 10 11 12 13 14 15 16 17 18 19 20)  # Example values for the second parameter

# Loop through each value in gshare_values1
for value1 in "${gshare_values1[@]}"; do
  # For value2, loop from 0 up to value1
  for ((value2=0; value2<=value1; value2++)); do
    echo "Running simulation with gshare values: $value1 and $value2"
    ./sim gshare "$value1" "$value2" gcc_trace.txt
    echo "------------------------------------"
  done
done