# DSS-Checker
Distributed State Space Builder is a distributed verification implementation based on modular Petri nets. It allows to build a distributed state space instead of the flat state space. Such a graph consists of set of graphs where each one models the behaviours of one module with some global information. A property that concerns one module can be checked by only exploring the graph that is associated with the module.

## Building


- `git clone --recursive https://github.com/chihebabid/DSS-Checker.git`

- `cd DSS-Checker ; mkdir build ; cd build`

- `cmake ..`

- `cmake --build .`


## Usage

```
./distributedstatespace [OPTIONS]

Options:
`-h,--help                   Print this help message and exit`
--file Path:FILE REQUIRED   Petri net model file


Algorithm:
--algorithm TEXT            DSS building algorithm (default: DSS)


Print:
--dot-output                Save the output graph in a dot file
--txt-output                Save the output graph in a formatted text file
```
