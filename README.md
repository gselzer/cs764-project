# README - CS 764 Semester Project - Team 4
* Gabriel Selzer - 9076571836
* Nitigya Kargeti - 9086226645
* Rafael Vincenty-Vargas - 9086209146
* Sweksha Shukla - 9083762790

## Techniques Implemented & Group Contributions

### CPU Cache Merge Sort - Implemented by Nitigya

The implementation can be found in Run.cpp, on line X. The CPU Cache Merge sort sorts groups of Records such that the total number of bytes in the sort fits within the CPU Cache. We specifically use less space than the given amount to leave space for e.g. Instructions, other objects, etc.

### Quicksort - Implemented by Rafael and Sweksha

This implementation can be found in Run.cpp.

### Tournament Trees - Implemented by Gabe

The implementation can be found in LoserTree.cpp, on line X.

### External Merge Sort - Implemented by Gabe and Nitigya

The external merge sort lives in LoserTree.cpp	

### Verification of Record Content and Order - Implemented by Gabe

The content verification plan/iterator live in VerifyContent.cpp.
The order verification plan/iterator live in VerifyOrder.cpp.

### Offset Value Coding - Implemented by Gabe

The offset value codes can be found as state of the Record class, in Record.cpp.

### Cache Sized Miniruns - Implemented by Gabe

These Cache-Sized mini runs make use of the internal merge sort, implemented by Nitigya. The CacheSizedRun class lives in Run.cpp, which the internal merge sort 

### Spilling (Memory to SSD), (SSD to HDD) - Implemented by Gabe, (Rafael advised on call)
Spilling from Memory to SSD can be found in LoserTree, within the MultiStageLoserTree class.

File Backed Runs are used to spill to both SSD and HDD - in practice, they write to and read from a std::tmpfile. Because we cannot actually decide whether we physically spill to SSD or HDD, each edits a shared state, which virtually keeps track of how many spilled to each device. The logic for spilling is simple - if there’s room for a run on SSD, it goes there. Otherwise, we spill to HDD.

### Minimum Count of Row & Column Comparisons
This requirement is satisfied by the use of Offset Value Coding within the Tournament Trees, which was shown in class to guarantee a bounded number of row & column comparisons. Please see those sections for their locations. 

## Writeup:

The work for this project was largely driven by the desire to enable a 120 GB sort - therefore techniques were largely adopted in order to achieve that goal. Others were included as needed to fulfill the graded requirements. At the time of completion, we (aspire) to complete both of those goals.

The project state is TODO

Running the program is TODO
