# LaPath: Learning Automata algorithm for the shortest path problem

```python
Copyright (C) 2014-2021 by Constantine Kyriakopoulos, zfox@users.sourceforge.net
Version: 1.0.2
License: GNU GPL Version 3
```


## About the project

The shortest path problem is solved by many methods including heuristics that offer lower complexity in expense of accuracy. There are many use-cases where the lower accuracy is acceptable in return of lower consumption of computing resources or the ability to adapt to a constantly changing operating environment.

Learning Automata (LA) are adaptive mechanisms requiring feedback from the operating environment to converge to certain states. In the context of network routing, LA residing at intermediate nodes along a path, exploit feedback from the destination node for reducing, e.g., path's length. This is the case since each Automaton after several iterations (depending on topology’s resources) starts pointing with higher probability to a neighbouring node which leads to a shorter path. Specifically, each LA that resides in a node, evaluates all physical neighbours with a probability number. After each feedback value from a destination node, this LA increases the probability number of the node leading to the destination and decreases all other probabilities of neighbouring nodes. So, subsequent tries (iterations) to reach the same destination have more chances to follow shorter routes. When all LA converge to certain neighbours, the path that is formed carries the properties that stem from the feedback value, e.g., low number of hops, leading to shorter paths.

This is a heuristic method, i.e., optimal results are not always feasible. According to topology’s resources like the node and edge numbers, the proper number of iterations must be used. More iterations lead to paths with higher probability of being optimal but more computing resources are consumed.


## Prerequisites to build

There are only two requirements, i.e., the Boost Library and the availability of C++20 or C++17 standard. Boost is utilised for parsing the JSON representation of the topology. There is also the option to insert edges using the <em>AdaptiveSystem</em> interface, so in this case the JSON dependence can be commented out. Tested with Clang 12 and libc++ from the LLVM project. Build with 'mkdir build && cd build; cmake -DCMAKE_BUILD_TYPE=Release ../ && make' from the main source directory.


## Usage

Create a new instance of <em>LaSystem</em> in your code passing as arguments the JSON topology file and the number of iterations (a default iteration number is also provided but it won’t return the shortest paths under all topology sizes). Next, execute the method <em>path(src, dest)</em> where <em>src</em> is the source node and <em>dest</em> the destination to reach. This returns the valid path which the LA converge to.


## Related work

The probability updating scheme Linear Reward-Inaction Algorithm [1] updates the probability numbers of LA in each iteration. The choice of the next neighbour to route to, is provided by Broadcast Scheduling Algorithm A [2]. LaPath implementation is adapted from Ref. [3] which provides energy-efficient routes in backbone topologies. 

```python
[1] Thathachar, M.A. and Sastry, P.S., 2004. Networks of learning automata: Techniques for online stochastic optimization. Springer Science & Business Media.
[2] Vaidya, N.H. and Hameed, S., 1999. Scheduling data broadcast in asymmetric communication environments. Wireless Networks, 5(3), pp.171-182.
[3] Kyriakopoulos, C.A. et al, 2016. On the use of learning automata for energy saving in optical backbone networks. Electronic Notes in Discrete Mathematics, 51, pp.15-22.
```

## Changelog

<pre>
1.0.X    2020-XX-XX    XX
1.0.2    2020-11-21    Interface for edge insertion improvement
1.0.1    2020-11-16    Execution time improvement
1.0      2020-11-10    Initial public release
</pre>
