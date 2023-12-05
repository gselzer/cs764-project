# Starting and stopping Powers of 2

START=30
END=33

make clean
make
for i in `seq $START $END`;
do
  NUM_RECORDS=$(echo "2 ^ $i" | bc)
  ./sort -c $NUM_RECORDS -s 1000 -o test.txt
done
