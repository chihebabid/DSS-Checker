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
        ArcSG.cpp
        ArcSG.h
        DSS/Marking.cpp
        DSS/Marking.h
        ProductSCC.cpp
        ProductSCC.h
        PetriNet.cpp
        PetriNet.h
        Node.cpp
        Node.h
        Place.cpp
        Place.h
        Transition.cpp
        Transition.h
        ListMarquage.cpp
        ListMarquage.h
        Fusion.cpp
        Fusion.h
        NodeSGSimplified.cpp
        NodeSGSimplified.h
        ArcSG.cpp
        ArcSG.h
        DistributedState.cpp
        DistributedState.h
        DSS/MetaGraph.cpp
        DSS/MetaGraph.h
        DSS/MetaState.cpp
        DSS/MetaState.h
        DSS/StateGraph.cpp
        DSS/StateGraph.h
        DSS/SCC.cpp
        DSS/SCC.h
        NodeSG.cpp
        NodeSG.h
        ArcSync.cpp
        ArcSync.h
        DSS/DSSBuilder.cpp DSS/DSSBuilder.h DSS/ModuleSS.cpp DSS/ModuleSS.h
        SpotMC/DSSKripke.cpp
        SpotMC/DSSKripke.h
        SpotMC/DSSIterator.cpp
        SpotMC/DSSIterator.h
        SpotMC/DSSState.cpp
        SpotMC/DSSState.h
        misc.h)

target_link_libraries(distributedstatespace
        bddx
        spot
        ${MPI_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        cli11
)
