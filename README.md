# DSS-Checker
Distributed State Space Builder is a distributed verification implementation based on modular Petri nets. It allows to build a distributed state space instead of the flat state space. Such a graph consists of set of graphs where each one models the behaviours of one module with some global information. A property that concerns one module can be checked by only exploring the graph that is associated with the module.

## Building


- `git clone --recursive https://github.com/chihebabid/DSS-Checker.git`

- `mkdir build`

- `cd build`

- `cmake ..`

- `cmake --build .`


## Testing
