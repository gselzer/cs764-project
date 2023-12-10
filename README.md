# README - CS 764 Semester Project - Team 4
* Gabriel Selzer - 9076571836
* Nitigya Kargeti - 9086226645
* Rafael Vincenty-Vargas - 9086209146
* Sweksha Shukla - 9083762790

## Techniques Implemented & Group Contributions

### CPU Cache Merge Sort - Implemented by Nitigya

The implementation can be found in `Run.cpp`, at the very end of the file. The functionality has been commented out as we now use quicksort instead, however we have preserved it for grading purposes. The CPU Cache Merge sort sorts groups of Records such that the total number of bytes in the sort fits within the CPU Cache. We specifically use less space than the given amount to leave space for e.g. Instructions, other objects, etc.

### External Merge Sort - Implemented by Gabriel and Nitigya (Rafael and Sweksha advised on call)
The external merge sort lives in `LoserTree.cpp`. The `LoserTree` class is responsible for producing a sorted stream of `Record`s given a set of `Run` objects. The `MultiStageLoserTree` class makes use of many levels of `LoserTree` objects as `Record`s are inducted into the sort, and as `Record`s are spilled, first to the SSD and then to the HDD.

### Executable Compilation - Implemented by Nitigya
Made the entire refactor into different folders for header files and cpp files. In addition Generated additional code in addition to the starter code by implementing Unit Testings for Scan and Sort. In additon implemented the Main Function created, takes in Command line arguments for `Number of Records`, `Size of the records` and `Filename for Output text files` and writes data onto the entire trace file. 


The trace file Contains Data About the records and size that the project is sorting, The State of the SSD & HDD , The HDD and SSD Total Time and the State Changes between DRAM and Splilling between SSD and HDD. 

### Quicksort - Implemented by Rafael and Sweksha

This implementation can be found in `DynamicRun::sort`, within `Run.cpp`.

### Tournament Trees - Implemented by Gabriel

The implementation can be found in `LoserTree.cpp`, within the `LoserTree` class.

### Verification of Record Content and Order - Implemented by Gabriel

The content verification plan/iterator live in `VerifyContent.cpp.` and make use of the `xor` operation to ensure equal content. A single `Record` object is used to efficiently `xor` each scanned `Record` as it enters the sort; all `Record`s are again `xor`ed as they leave the sort. A `RowCount` object is used to keep track of the number of `Record`s that have been seen before the sort but not after the sort - therefore, we ensure that when we are done with the sort, we have seen both the same number of `Record`s before and after the sort, and the same content.

The order verification plan/iterator live in VerifyOrder.cpp. The `Iterator` makes use of a single `Record` object, which is used to keep track of the prior value it saw, and in every `VerifyOrderIterator::next` call we (1) assert that the returned `Record` is less than the `Record` before and (2) update the last `Record` seen to be the newly returned `Record`.

### Offset Value Coding - Implemented by Gabriel and Nitigya (Rafael and Sweksha advised on call)
The offset value codes can be found as state of the `Record` class, in `Record.h`. They are used within `LoserTree::buildTree` and `LoserTree::replayGame`, both in `LoserTree.cpp`. They are calculated after the initial quicksort of the cache-sized runs, in `DynamicRun::sort` (in `Run.cpp`), and are updated as needed within the `LoserTree` class.

### Cache Sized Miniruns - Implemented by Gabriel (Rafael and Sweksha advised on call)

Cache-sized Miniruns are created in `ExternalMergeSortIterator::next` (within `Sort.cpp`). These runs originally made use of the internal merge sort algorithm implemented by Nitigya, but were converted to use quicksort once implemented by Rafael and Sweksha as descripted above.

### Minimum Count of Row & Column Comparisons
This requirement is satisfied by the use of Offset Value Coding within the Tournament Trees, which was shown in class to guarantee a bounded number of row & column comparisons. Please see those sections for their locations. 

### Dynamically Sized Records - Implemented by Gabriel and Nitigya, initial coding by Rafael and Sweksha
This requirement is satisfied by the `char[]` objects within the `Record` class (in `Record.h`). In practice, given a prescribed `Record` size (in the number of bytes)

### Spilling (Memory to SSD), (SSD to HDD) - Implemented by Gabriel and Nitigya, (Rafael advised on call)
### Device-Optimized Page Sizes - Implemented by Nitigya and Gabriel
This requirement is satisfied by the `pageSize` argument of the `DynamicRun` constructor. In `ExternalMergeSortIterator::next` (within `Sort.cpp`), CPU-Cache Sized pages (we actually use 500KB deliberately instead of 1MB to allow room in the CPU cache for instructions, quicksort data, etc) are created and held in DRAM until the DRAM is (nearly) full. At that point, we create a `DynamicRun` with a page size equivalent to the SSD `latency * bandwidth`, which was shown in class to be nearly optimal, to flush our CPU-sized Runs to SSD - this flushing happens in `MultiStageLoserTree::flushCacheRuns` (within `LoserTree.cpp`). That `DynamicRun` writes to file in increments of the SSD page size. Finally, once the SSD is (nearly) full, we create a new `DynamicRun` object with a page size equivalent ot the HDD `latency * bandwidth`, to flush our HDD size Runs to HDD - this flusing happens in `MultiStageLoserTree::flushSSDRuns` (within `LoserTree.cpp`).

### Exception Handling - Implemented by Sweksha and Rafael
Various `try`/`catch` blocks throughout the repo

## Writeup:

The work for this project was largely driven by the desire to enable a 120 GB sort - therefore techniques were largely adopted in order to achieve that goal. Others were included as needed to fulfill the graded requirements. At the time of completion, we aspired to complete both of those goals.

The project state is mostly complete. We know that we can run three of the four "graded" sort operations. The largest sort operation (`120GB`), throws a segmentation fault when approximately 80GB of data is spilled to the HDD.

We also are aware that running `./sort` on the CSL instructional machines causes our `VerifyOrderIterator` to throw an `AssertionError` around a sort size of approximately 8GB - we know that `./sort -c 7900000 -s 1000 -o task.txt` and all tested sort sized below pass our iterators but `./sort -c 7950000 -s 1000 -o task.txt` and all tested sort sizes above throw the assertion error with a segmentation fault. Because the code path does not change at this sort size, **and because we can sort 12GB on our linux (Multiple Distributions) and macOS machines successfully**, we believe this may be a limitation imposed by the machines on students' operations within the CSL machines. If you encounter this error while grading our project, please let us know and we'd be happy to work around it.

In Adition we tried to debug this issue on CSL machines and found out that our program tries to write out multiple data records to file but is only able to write a few of the records. We believe this could be due to a Rate Limit on File I/O on the CSL machines.

Here is the last lines of output on CSL machine with our Debug Statements :

```bash
Write was fine!
Write was fine!
Write was fine!
Write was fine!
Tried to write out 1048 elements, but wrote out (w1) 387 instead (Faiure 1)
Tried to write out 1006080 elements, but wrote out (w2) 4096 instead (Failure 2)
Tried to write out 1048 elements, but wrote out (w1) 102 instead (Faiure 3)
Tried to write out 1006080 elements, but wrote out (w2) 4096 instead (Failure 4)
Tried to write out 1048 elements, but wrote out (w1) 102 instead (Faiure 5)
Tried to write out 1006080 elements, but wrote out (w2) 4096 instead (Failure 6)
Tried to write out 1048 elements, but wrote out (w1) 102 instead (Faiure 7)
Tried to write out 1006080 elements, but wrote out (w2) 4096 instead (Failure 8)
Tried to write out 1048 elements, but wrote out (w1) 102 instead (Faiure 9)
Tried to write out 1006080 elements, but wrote out (w2) 4096 instead (Failure 10)
Tried to write out 1048 elements, but wrote out (w1) 102 instead (Faiure 11)
Tried to write out 1006080 elements, but wrote out (w2) 4096 instead (Failure 12)
Tried to write out 1048 elements, but wrote out (w1) 102 instead (Faiure 13)
Tried to write out 1006080 elements, but wrote out (w2) 4096 instead (Failure 14)
Tried to write out 1048 elements, but wrote out (w1) 102 instead (Faiure 15)
Tried to write out 1006080 elements, but wrote out (w2) 4096 instead (Failure 16)
...
sort: src/VerifyOrder.cpp:52: virtual Record* VerifyOrderIterator::next(): Assertion `*_last <= *r' failed.
Aborted (core dumped)
```

In contrast we faced no issues as such on our local and test machines (Unix System (Mac OS), Fedora (Linux), Kali(Linux) and Ubuntu(Linux)). 

Running the program can be accomplished using the syntax requested in the assignment description:

```bash
./sort -c 1000000 -s 50 -o results/test_DRAM.txt
```
For expidited grading, check out `./grading.sh`.
