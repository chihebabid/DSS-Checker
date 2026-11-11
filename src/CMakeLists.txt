# set minimum cmake version
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# include MPI
#find_package(MPI REQUIRED)
#include_directories(${MPI_INCLUDE_PATH})

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
add_executable(distributedstatespace
        main.cpp
        misc.h

        # Petri
        Petri/ConstructPetriFromFile.cpp
        Petri/ConstructPetriFromFile.h
        Petri/ModularPetriNet.cpp
        Petri/ModularPetriNet.h
        Petri/Node.cpp
        Petri/Node.h
        Petri/PetriNet.cpp
        Petri/PetriNet.h
        Petri/Place.cpp
        Petri/Place.h
        Petri/Transition.cpp
        Petri/Transition.h
        Petri/TransitionFusionSet.cpp
        Petri/TransitionFusionSet.h

        # DSS
        DSS/ArcSync.cpp
        DSS/ArcSync.h
        DSS/DSSBuilder.cpp
        DSS/DSSBuilder.h
        DSS/DistributedState.cpp
        DSS/DistributedState.h
        DSS/DistributedSS.cpp
        DSS/DistributedSS.h
        DSS/Marking.cpp
        DSS/Marking.h
        DSS/MarkingArray.cpp
        DSS/MarkingArray.h
        DSS/MetaGraph.cpp
        DSS/MetaGraph.h
        DSS/MetaState.cpp
        DSS/MetaState.h
        DSS/ModuleSS.cpp
        DSS/ModuleSS.h
        DSS/ProductSCC.cpp
        DSS/ProductSCC.h
        DSS/SCC.cpp
        DSS/SCC.h

        # SpotMC
        SpotMC/DSSIterator.cpp
        SpotMC/DSSIterator.h
        SpotMC/DSSKripke.cpp
        SpotMC/DSSKripke.h
        SpotMC/DSSState.cpp
        SpotMC/DSSState.h
        SpotMC/ParseFormulaFile.cpp
        SpotMC/ParseFormulaFile.h

        # Legacy
        Legacy/ArcSG.cpp
        Legacy/ArcSG.h
        Legacy/ListMarking.cpp
        Legacy/ListMarking.h
        Legacy/NodeSG.cpp
        Legacy/NodeSG.h
        Legacy/NodeSGSimplified.cpp
        Legacy/NodeSGSimplified.h
)

target_link_libraries(distributedstatespace
        bddx
        spot

        ${OPENSSL_LIBRARIES}
        cli11
)
