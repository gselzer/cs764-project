# README - CS 764 Semester Project - Team 4
* Gabriel Selzer - 9076571836
* Nitigya Kargeti - 9086226645
* Rafael Vincenty-Vargas - 9086209146
* Sweksha Shukla - 9083762790

## Techniques Implemented & Group Contributions

### CPU Cache Merge Sort - Implemented by Nitigya

The implementation can be found in `Run.cpp`, at the very end of the file. The functionality has been commented out as we now use quicksort instead, however we have preserved it for grading purposes. The CPU Cache Merge sort sorts groups of Records such that the total number of bytes in the sort fits within the CPU Cache. We specifically use less space than the given amount to leave space for e.g. Instructions, other objects, etc.

### External Merge Sort - Implemented by Gabe and Nitigya (Rafael and Sweksha advised on call)
The external merge sort lives in `LoserTree.cpp`. The `LoserTree` class is responsible for producing a sorted stream of `Record`s given a set of `Run` objects. The `MultiStageLoserTree` class makes use of many levels of `LoserTree` objects as `Record`s are inducted into the sort, and as `Record`s are spilled, first to the SSD and then to the HDD.

### Executable Compilation - Implemented by Nitigya
Made the entire refactor into different folders for header files and cpp files. In addition Generated additional code in addition to the starter code by implementing Unit Testings for Scan and Sort. In additon implemented the Main Function which created the entire trace files and Controls the Control Variable.

### Quicksort - Implemented by Rafael and Sweksha

This implementation can be found in `DynamicRun::sort`, within `Run.cpp`.

### Tournament Trees - Implemented by Gabe

The implementation can be found in `LoserTree.cpp`, within the `LoserTree` class.

### Verification of Record Content and Order - Implemented by Gabe

The content verification plan/iterator live in `VerifyContent.cpp.` and make use of the `xor` operation to ensure equal content. A single `Record` object is used to efficiently `xor` each scanned `Record` as it enters the sort; all `Record`s are again `xor`ed as they leave the sort. A `RowCount` object is used to keep track of the number of `Record`s that have been seen before the sort but not after the sort - therefore, we ensure that when we are done with the sort, we have seen both the same number of `Record`s before and after the sort, and the same content.

The order verification plan/iterator live in VerifyOrder.cpp. The `Iterator` makes use of a single `Record` object, which is used to keep track of the prior value it saw, and in every `VerifyOrderIterator::next` call we (1) assert that the returned `Record` is less than the `Record` before and (2) update the last `Record` seen to be the newly returned `Record`.

### Offset Value Coding - Implemented by Gabe and Nitigya (Rafael and Sweksha advised on call)
The offset value codes can be found as state of the `Record` class, in `Record.h`. They are used within `LoserTree::buildTree` and `LoserTree::replayGame`, both in `LoserTree.cpp`. They are calculated after the initial quicksort of the cache-sized runs, in `DynamicRun::sort` (in `Run.cpp`), and are updated as needed within the `LoserTree` class.

### Cache Sized Miniruns - Implemented by Gabe (Rafael and Sweksha advised on call)

Cache-sized Miniruns are created in `ExternalMergeSortIterator::next` (within `Sort.cpp`). These runs originally made use of the internal merge sort algorithm implemented by Nitigya, but were converted to use quicksort once implemented by Rafael and Sweksha as descripted above.

### Minimum Count of Row & Column Comparisons
This requirement is satisfied by the use of Offset Value Coding within the Tournament Trees, which was shown in class to guarantee a bounded number of row & column comparisons. Please see those sections for their locations. 

### Dynamically Sized Records - Implemented by Gabe and Nitigya, initial coding by Rafael and Sweksha
This requirement is satisfied by the `char[]` objects within the `Record` class (in `Record.h`). In practice, given a prescribed `Record` size (in the number of bytes)

### Spilling (Memory to SSD), (SSD to HDD) - Implemented by Gabe and Nitigya, (Rafael advised on call)
### Device-Optimized Page Sizes - Implemented by Nitigya
This requirement is satisfied by the `pageSize` argument of the `DynamicRun` constructor. In `ExternalMergeSortIterator::next` (within `Sort.cpp`), CPU-Cache Sized pages (we actually use 500KB deliberately instead of 1MB to allow room in the CPU cache for instructions, quicksort data, etc) are created and held in DRAM until the DRAM is (nearly) full. At that point, we create a `DynamicRun` with a page size equivalent to the SSD `latency * bandwidth`, which was shown in class to be nearly optimal, to flush our CPU-sized Runs to SSD - this flushing happens in `MultiStageLoserTree::flushCacheRuns` (within `LoserTree.cpp`). That `DynamicRun` writes to file in increments of the SSD page size. Finally, once the SSD is (nearly) full, we create a new `DynamicRun` object with a page size equivalent ot the HDD `latency * bandwidth`, to flush our HDD size Runs to HDD - this flusing happens in `MultiStageLoserTree::flushSSDRuns` (within `LoserTree.cpp`).

### Exception Handling - Implemented by Sweksha and Rafael
Various `try`/`catch` blocks throughout the repo

## Writeup:

The work for this project was largely driven by the desire to enable a 120 GB sort - therefore techniques were largely adopted in order to achieve that goal. Others were included as needed to fulfill the graded requirements. At the time of completion, we aspired to complete both of those goals.

The project state is mostly complete. We know that we can run three of the four "graded" sort operations. The largest sort operation (`120GB`), throws a segmentation fault when approximately 80GB of data is spilled to the HDD.

Running the program can be accomplished using the syntax requested in the assignment description:

```bash
./sort -c 1000000 -s 50 -o results/test_DRAM.txt
```
For expidited grading, check out `./grading.sh`.
