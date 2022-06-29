# Stack-Memory-Allocator
<p>This is a simple stack-based memory allocator, written in c++. A large amount of memory is allocated beforehand by calling malloc instead of calling malloc each time a new memory needs to be allocated in the heap. A pointer is used to point to the to of the stack, where new allocations are done. </p>
<p>To free a previously allocated memory, a header is added before the allocated data and its pointer is stored. Whenever needed, we can jump to address of any such header and free all the memory from that location onwards </p>
<p> Below screenshot is for allocation of 5000 objects of long int, i.e, around 20KB of data.</p>
<img src = "https://user-images.githubusercontent.com/52422585/176453965-23b8a893-dab6-4b11-abba-8fdf64414fbc.png"/>
