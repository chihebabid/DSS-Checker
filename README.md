# DSS-Checker

Distributed State Space Builder is a distributed verification implementation based on modular Petri nets. It allows to build a distributed state space instead of the flat state space. Such a graph
consists of set of graphs where each one models the behaviours of one module with some global information. A property that concerns one module can be checked by only exploring the graph that is
associated with the module.

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
--mc Path:FILE              LTL property file


Algorithm:
--algorithm TEXT            DSS building algorithm (default: DSS)


Print:
--dot-output                Save the output graph in a dot file
--svg-output                Render the output graph in an SVG file (requires Graphviz)
--txt-output                Save the output graph in a formatted text file
```
### Rendering the graph

`--dot-output` writes a dot file that can be rendered with Graphviz (`dot -Tsvg model.pn.dot -o model.svg`).
Graphviz cannot draw an arc from a metastate to itself, so a reflexive synchronisation arc is then
shown through a small label node next to the metastate.

`--svg-output` renders the graph directly in `model.pn.svg`, with these arcs drawn as loops from the
bottom-left to the bottom-right corner of the metastate. It needs Graphviz (`dot` and `neato`) in the `PATH`.

## Sample

In order to check the LTL property contained in file `Formulas/ERK1.ltl` on the Petri net model of file `Models/ERK1/ERK1.pn`, one should copy these files in the same localisation of the executable and execute the following command:

`./distributedstatespace --file ERK1.pn --mc ERK1.LTL`