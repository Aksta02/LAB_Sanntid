
# TTK4147 - Real-time Systems  

## Exercise - Memory

In this exercise you will be introduced to some concepts that are important for
C programming and necessary for this course, even if it is not directly related
to real time. Only parts of the information you need in the exercises is given
in the exercise text. You may need to find information on the internet and
from other sources, or ask the student assistants, they are there to help you.

## Memory

Dynamic memory allows memory to be allocated while the program is running. In C
dynamic memory allocation uses the functions `malloc()` and `free()`. This is
typically used when the required memory size for a program is not known when it
is compiled. It is important to free all the memory you have allocated, or you
can get a memory leak. This means that as the program runs it takes more memory
than it returns, which over time will crash the system when there is no more
memory available. Dynamic memory is often avoided for real-time systems because
of the danger of memory leaks and the increase in code complexity. But it is
still often needed, especially in communication handling.

In addition to the normal RAM memory, computers often also have a swap
partition. This is an area on the "slow" memory, which can be used to
temporarily expand the working memory in case the RAM is full. By default the
RPi has 2048MB of swap. Verify this by running `htop` and look for the Swp-row. 

#### TASK A

The program below allocates memory for a very large 4GB matrix of 64-bit
integers:

```c
    long xy_size    = 1000*1000*500;       // 4 GB (sizeof(long) = 8 bytes)
    long x_dim      = 100;
    long y_dim      = xy_size/x_dim;   
     
    long** matrix   = malloc(y_dim*sizeof(long*));

    for(long y = 0; y < y_dim; y++){
        matrix[y] = malloc(x_dim*sizeof(long));
    }tition. This is an area on the "slow" memory, which can be used to
temporarily expand the working memory in case the RAM is full. By default the
RPi has 2048MB of swap. Verify this by running `htop` and look for the Swp-row. 


    printf("Allocation complete (press any key to continue...)\n");
    getchar();
```

The RPi at the lab have 4GB of RAM, which means there shouldn’t be enough space
to allocate this matrix. Open a terminal and run `htop`. Here you can see the total memory
used by the system. Run the program, and observe the effects on the memory
usage.

What happens? Fyller memory umiddelbart til ca 3.55 GB av 3.71, swap gikk opp til 451 M av 2 GB

What is the difference between "Memory" and "Swap"? Forskjellen er at "Memory" er mye raskere, men har en kapasitets begresning på 4 GB hos oss. "Swap" er da nødminne, som brukes om "Memory" overbelastes. "Swap" er da mye tregere og har lagring på ssd'en.

Now change the shape of the matrix by increasing `x_dim` first to 1000, then
10000, and run the program again. Note that changing `x_dim` only changes the
shape of the allocated memory, the actual amount stays the same.

What happens? For x_dim = 1000 brukes bare ca halvparten av minne til RAM'en på ca 2.05 GB. For x_dim = 10000 brukes 328 MB av 3.71 G. Det at minnebruken reduseres gir mening med tanke på at vi reduserer antall malloc-kall. Hvert malloc kall har et overhengende minnebruk, slik at når vi går fra 5 000 000 kall ved x_dim = 100, til 50 000 med x_dim = 10 000 gir det totalt mening.

Run the program with `x_dim` = 10000 again, but this time initialize it by
writing zeros to all the memory (previously the memory was not initialized):

```c
    memset(matrix[y], 0, x_dim*sizeof(long));
```

Explain why something different happens when the memory is also initialized.
Nå fylles RAM'en igjnn og "swap" bruker 330 MB. Dette er grunnet t matrisen ikke lenger er tom, og vi allokerer reelle tall inn i matrisen. 


Hint: does `malloc()` always allocate the memory right away? Nei, denne bare reservere og fortelle cpu'en at dette minnet skal brukes.

#### TASK B

A common application of dynamic memory is to have a list that will grow and
shrink in size as the program runs. The beginning of some code for a simple
dynamic array is available in `array.c` and `array.h`. Implement `array_insertBack()`, but without
implementing a way to grow the size of the array if there isn’t enough capacity
(we’ll get back to that later). Create an array with some small capacity, then
`insertBack` more elements than there is capacity for, and run the program. What
happens?

Vi laget et array med kapasitet på 4, men siden array_insertBack() ikke sjekker kapasitet fortsetter programmet å legge til tal etter vi har nådde kapasitet en stund før det kræsjer. Dermed for vi uforventa oppførsel og store tal på dei siste punkta.

Compile and run the program again, but now add the compiler flags
`-g -fsanitize=address`, if you are using a makefile you also have to add the
address sanitizer to the linker flags. What happens now?

Feil vi får:

AddressSanitizer: CHECK failed: sanitizer_allocator_primary64.h:131 "((kSpaceBeg)) == ((address_range.Init(TotalSpaceSize, PrimaryAllocatorName, kSpaceBeg)))" (0x500000000000, 0xfffffffffffffff4) (tid=1994)
    <empty stack>

Ville gått bra på PC, men feiler i Raspbery PI


AddressSanitizer is an invaluable tool for making sure you don’t get your
pointers and memory all tangled up. If you are writing code that uses pointers
in any non-trivial way, you should probably enable this memory error detector –
as long as the platform supports it. And if it doesn’t, see if you can copy the
relevant code to a platform that does while you work on it, then port it back
after it has been tested.

#### TASK C

Implement `array_reserve()`, such that it can grow the capacity of the array.
You will have to relocate the existing data by first allocating new space,
moving the existing data, and then freeing the old data.

Fix `array_insertBack()` so that it detects a lack of capacity and reserves more
if necessary. Run and verify that the code works.

#### TASK D

Typically when increasing the capacity of a dynamic array, you would increase
the capacity by some factor, say 2x or 1.5x. Let’s say our array currently has a
capacity of 2 elements. From here, we continuously insert new elements at the
back, increasing the capacity as necessary. Draw out (or otherwise visualize)
what would happen to the available memory if each new relocation requires a
contiguous section of memory that is 2x the size of the previous section. Then
do the same thing for 1.5x.

In what cases will you be able to reuse memory that you have freed up?
