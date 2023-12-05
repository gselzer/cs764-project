# Starting and stopping Powers of 2

START=0
END=20

make clean
make

if [ -d "results" ]; then
  rm -rf results
fi
mkdir results

./sort -c 1000000 -s 50 -o results/test_DRAM.txt

./sort -c 2500000 -s 50 -o results/test_SSD.txt

./sort -c 12000000 -s 1000 -o results/test_HDD.txt

./sort -c 120000000 -s 1000 -o results/test_BIG.txt
