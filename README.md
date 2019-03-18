# IDA-algorithm
Implementation of a 15 puzzle game https://en.wikipedia.org/wiki/15_puzzle
Algorithm
1. Iterative Deepening A*
(IDA*)
The algorithm follows the Depth-First Search search strategy and can be easily implemented as a
recursive function.
IDA* has two parts:
1. The main loop, initializes the thresholds B and B0 first. If no solution is found the search is
triggered again but with an updated B = B0
threshold.
2. The recursive function that implements the bounded Depth-First Search.

Input
Our solver has to read the initial configuration from a file with the same format as it appears
in the table, that is, a single line, containing a sorted list of indexes separated by a blank space. For
example, instance N = 1 from the table would be:
14 13 15 7 11 12 9 5 6 0 2 1 4 8 10 3
output
Assuming we have a file called “1.puzzle” containting a single line with the configuration of instance
N = 1 from the table, we will call our solver by running the following command:
./15puzzle 1.puzzle
and it will print into the stdout the following information:
1. Initial state of the puzzle.
2. h(s0) heuristic estimate for the initial state.
3. Thresholds the search have used to solve the problem.
4. Number of moves of the optimal solution.
5. Number of generated nodes.
6. Number of expanded nodes.
7. Number of expanded nodes per second.
8. Total Search Time, in seconds.
For example, the output of our solver ./15puzzle 1.puzzle for instance N = 1 is:
Initial State:
14 13 15 7
11 12 9 5
6 0 2 1
4 8 10 3
Initial Estimate = 41
Threshold = 41 43 45 47 49 51 53 55 57
Solution = 57
Generated = 499,911,606
Expanded = 253,079,561
Time (seconds) = 5.12
Expanded/Second = 49,082,100
