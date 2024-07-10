### dynamic-array

**purpose:** A data type which has the properties of a linked list and an array. 

**composition**

The data type is implemented entirely in dynamic-array.hpp.

All other files are for testing.

**compilation**

running *make* compiles main.cpp and test.cpp.

**info**

It is implemented as a xor linked list, where each node holds an array.\
The more nodes with smaller arrays the less time erasing and inserting consume and the more time indexing consumes\
the less nodes with bigger  arrays the more time erasing and inserting consume and the less time indexing consumes\

Run test.out for comparison between the data type and std::vector
