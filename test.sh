# Starting and stopping Powers of 2

START=0
END=20

make clean
make
for ((i=$START; i<=$END; i++))
do
  ./sort -c $((2**$i)) -s 1000 -o test.txt
done
