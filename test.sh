# Starting and stopping Powers of 2

START=0
END=20

make clean
make

if [ -f "results" ]; then
  rm -rf results
fi
mkdir results

for i in `seq $START $END`;
do
  NUM_RECORDS=$(echo "2 ^ $i" | bc)
  ./sort -c $NUM_RECORDS -s 1000 -o results/test$i.txt
done
