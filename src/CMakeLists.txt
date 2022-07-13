# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# include MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# include openssl
find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})




add_library(spot SHARED IMPORTED) # or STATIC instead of SHARED
set_target_properties(spot PROPERTIES
  IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/libspot.so"
  INTERFACE_INCLUDE_DIRECTORIES "/usr/include/spot/"
)

add_library(bddx SHARED IMPORTED) # or STATIC instead of SHARED
set_target_properties(bddx PROPERTIES
  IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/libbddx.so"
  INTERFACE_INCLUDE_DIRECTORIES "/usr/include/spot/"
)





# Hybrid SOG executable
add_executable(distributedstatespace main.cpp
    ConstructPetriFromFile.cpp
    ConstructPetriFromFile.h
    ModularPetriNet.cpp
    ModularPetriNet.h
    ModularSpace.cpp
    ModularSpace.h
    ArcSG.cpp
    ArcSG.h
    Automata.cpp
    Automata.h
    Arc.cpp
    Arc.h
    Marking.cpp
    Marking.h
    ProductSCC.cpp
    ProductSCC.h
    PetriNet.cpp
    PetriNet.h
    PileRed.cpp
    PileRed.h
    PilePhase1.cpp
    PilePhase1.h
    Node.cpp
    Node.h
    Place.cpp
    Place.h
    Fils.cpp
    Fils.h
    IntNode.cpp
    IntNode.h
    Transition.cpp
    Transition.h
    ListMarquage.cpp
    ListMarquage.h
    ListSync.cpp
    ListSync.h
    Fusion.cpp
    Fusion.h
    Graphe.cpp
    Graphe.h
    GrapheSync.cpp
    GrapheSync.h
    NodeSGSimplified.cpp
    NodeSGSimplified.h
    ArcSG.cpp
    ArcSG.h
    IntNode.cpp
    IntNode.h
    ElementRed.cpp
    ElementRed.h
    Element_dss.cpp
    Element_dss.h
    Operations.cpp
    Operations.h
    PElement_dss.cpp
    PElement_dss.h
    DistributedState.cpp
    DistributedState.h
    MetaGraph.cpp
    MetaGraph.h
    MetaState.cpp
    MetaState.h
    StateGraph.cpp
    StateGraph.h    
    RElement_dss.cpp
    RElement_dss.h
    SCC.cpp
    SCC.h
    InternalArc.cpp
    InternalArc.h    
    GrapheSync.cpp
    GrapheSync.h
    NodeSG.cpp
    NodeSG.h
    ArcSync.cpp
    ArcSync.h
    Element.cpp
    Element.h
    ElementPhase1.cpp
    ElementPhase1.h
    PElement.cpp
    PElement.h
        BaseBuilder.cpp BaseBuilder.h)

target_link_libraries(distributedstatespace
  bddx
  spot
  ${MPI_LIBRARIES}
  ${OPENSSL_LIBRARIES}
        cli11
)
