STRASSEN TIME COMPARISON


DEV LOGS:

OCT 18 2023

implemented strassen using parallel add/subtract for matrixes, using spawn
for the recursive calls, using openmp. based on the parallel algorithm
given in algorithms book by thomas cormen.

OCT 30 2023
had to make all matrixes be passed by reference as a parameter
because using malloc twice to declare an empty matrix gave
segmentation faults when reading matrix for sizes >4.

NOV 3 2023
running recursive parallelstrassen for n sizes too small is
a waste. too many threads are used on matrixes of size 2 4 8
etc. when n is 32 or less, it runs regular parallel matrix
multiplication algorithm. Got the idea from here:
https://github.com/spectre900/Parallel-Strassen-Algorithm
thought it'd help speed it up.

For sequential it will do the same but without the parallel
in the regular multiplication algorithm.

NOV 5 2023

The results of the last experiment made it so that at lower values,
the time of the parallel one is less than the sequential one because
it is just parallelizing the regular matrix multiplication algorithm.

128x128 ends up doing the regular matrix multiplication 4 times,
divides the matrix in 4 matrixes of 32x32, so the spawn works to
speed it up even more. As the matrix grows in size, the amount of
threads needed to keep it sped up, having enough threads for all
the procedures it spawns, is too much for my computer as I only have
8 threads. To do a 256x256 you'd need 16 threads (4x4) to run all the
regular 32x32 matrices with 1 thread each. 

As it requires more threads than it has, the process slows over and 
makes it worse than the sequential. Also at 2^11, the result of the
matrix was not possible to calculate using values of 0-9 in the squares,
it terminated the process automatically. Will test in the future if
using matrices with numbers between 0-3 helps out.


NOV 24 2023

I have decided to not only test the 0-3 matrices today but also create a
parallel version of the algorithm using MPI. This version will be made for
8 processors right now but I might change it later. The concept will be to
divide the code into 7 subprocesses, for the recursive strassen calls.
Researching and stuff got the basic idea from this paper:
https://www.researchgate.net/figure/A-model-of-MPI-task-processes-graph-to-implement-Strassens-algorithm_fig2_294260033

To do this I had to research communication between mpi processes and found
mpi send and mpi recv.

Results:

2^7

sequential: 0.006794s
parallel-8: 0.004655
sequentialfull: 0.198930s


2^8

sequential: 0.048688s
parallel-8: 0.032180s
sequentialfull: 1.348830s

2^9

sequential: 2.408721s
parallel-8: 1.787046s
sequentialfull: 12.885163s

2^10

sequential: 67.671080s
parallel-8: 98.865656s
sequentialfull: killed

With these results I'm sure I implemented the code properly now, as 
the amount of time it takes in parallel is noticeably less than in 
sequential. But it eventually reaches the point where it needs more 
threads to continue making it faster than sequential. Process still
terminates on its own if the matrix size is 2^11 though.

Last thing. Don't know how to make it so it only divides 7 subprocesses and
doesnt try to do a bunch of new ones at the same time in recursive function
fashion. Dividing it into 7 subprocesses means there will only be 8 at a 
time total. Not however many recursive function calls there are. I have
to find a way to make sure it only does it once and not again using just
1 function.


NOV 26 2023

Adding the additional parameter 'count' in the strassen call, I can make it
so it only runs the subprocess spawns once. When the strassen func is initially
called, it'll call count = 0, whereas every other will use count = 1. For MPI_Recv
I tried multiple arguments for the status parameter. Tried MPI_STATUS as a basic thing,
tried leaving it without this parameter and it compiled when I put 
MPI_STATUS_IGNORE, src = https://learn.microsoft.com/en-us/message-passing-interface/mpi-status-structure


With 1 processor it runs properly. But with more than 1, aka doing the parallel work,
it gives segmentation fault. The problem is I don't know why it's doing that because
I'm running with 2^4 size matrix, so it should just go to regular multiplication.

DEC 3 2023

Having the full working algorithm with MPI (sort of) I asked my dad to lend me his
pc to run since his has 12 cores. I can't use it all the time cause he's working so
I can't debug as much. I made a version for 4 processors that I can run on my computer
as well. It actually gives me errors so it doesn't finish the process properly, but
I printed out the times before the program gave me the segmentation fault, so I have
the results of the strassen time, just not the final matrix. So it only has the 
partial time for it and not how long it takes to do the final process of uniting
the matrixes.


DEC 5 2023

I fixed the problem so I should be able to run MPI-4 and MPI-8 properly now. I fixed it by
removing the count variable and just making the function strassensub, which is what
the worker processes call. These are the times:


2^7

MPI-4: 0.005079s 
MPI-8: 0.001418s


2^8

MPI-4: 0.044847s
MPI-8: 0.009144s

2^9

MPI-4: 1.734129s
MPI-8: 0.351249s

2^10

MPI-4: killed
MPI-8: 8.448492s
