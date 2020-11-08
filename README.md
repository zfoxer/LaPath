# LaPath: Learning Automata algorithm for the shortest path problem

```python
Copyright (C) 2014-2020 by Constantine Kyriakopoulos
Version: 1.0
License: GNU GPL Version 3
```


## About the project

The shortest path problem is solved by many methods. Heuristics offer lower complexity in expense of accuracy. There are many use cases where the lower accuracy is acceptable in return of lower consumption of computing resources.

Learning Automata (LA) are adaptive mechanisms requiring feedback from the execution environment to converge to certain states. In the context of network routing, LA residing at intermediate nodes of a path exploit feedback from the destination node for reducing, e.g., path's length. This happens since each Automaton after several iterations (depending on topology’s resources) starts pointing with higher probability to a neighbouring node which leads to a shorter path. Specifically, each LA that resides in a node, evaluates all physical neighbours with a probability number. After each feedback value from a destination node, this LA increases the probability number of the node leading to the destination and decreases all other probabilities of neighbouring nodes. So, subsequent tries (iterations) to reach the same destination have more chances to follow the same route. When all LA converge to certain neighbours, the path that is formed carries the properties that stem from the feedback value, e.g., low number of hops, leading to shortest paths.

This is a heuristic method, i.e., optimal results are not always feasible. According to topology’s resources like the node and edge numbers, the proper number of iterations must be used. More iterations lead to paths with higher probability of being optimal but more computing resources are consumed.


## Prerequisites to build

There are only two requirements, i.e., the Boost Library and compatibility with the C++20 standard. Suggested is Boost Library 1.67 for parsing the JSON representation of the topology. Also, to compile the code, enable C++20 compatibility with the parameter ‘-std=c++2a’ when using, e.g., Clang 11. You can use ‘-stdlib=libc++’ to enable the STL implementation of the LLVM project.


## Usage

Create a new instance of LaSystem in your code passing as parameters the JSON topology file and the number of iterations (default iteration number is provided but it won’t return the shortest paths under all topology sizes). Next, execute the method ‘path(src, dest)’ where ‘src’ is the source node and ‘dest’ the destination to reach. This returns the valid path which the LA converge to.


## Related work

The linear probability updating scheme Linear Reward-Inaction Algorithm [1] is used to update the probability numbers of LA after each iteration. The choice of the next neighbour for routing is provided by Broadcast Scheduling Algorithm A in [2].

```python
[1] Thathachar, M.A. and Sastry, P.S., 2004. Networks of learning automata: Techniques for online stochastic optimization. Springer Science & Business Media.

[2] Vaidya, N.H. and Hameed, S., 1999. Scheduling data broadcast in asymmetric communication environments. Wireless Networks, 5(3), pp.171-182.
```
