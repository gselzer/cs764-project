# Starting and stopping Powers of 2

START=10
END=11

make clean
make
for ((i=$START; i<=$END; i++))
do
  ./sort -c $((2**$i)) -s 1000 -o test.txt
done
