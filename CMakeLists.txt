cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Simulation)

set(CMAKE_BUILD_TYPE Debug)

find_package(Geant4 REQUIRED ui_all vis_all)

include(${Geant4_USE_FILE})

# Include sources and headers from the specified directory
file(GLOB sources 
    ${PROJECT_SOURCE_DIR}/*.cc
    C:/Users/frisoe/GEANT4/program_files_GDML/share/Geant4/thesis/*.cc
)
file(GLOB headers 
    ${PROJECT_SOURCE_DIR}/*.hh
    C:/Users/frisoe/GEANT4/program_files_GDML/share/Geant4/thesis/*.hh
)

# Copy macro and data files if needed
file(GLOB MACRO_FILES "*.mac")
file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})

# file(GLOB DATA_FILES "*.dat")
# file(COPY ${DATA_FILES} DESTINATION ${PROJECT_BINARY_DIR})

add_executable(thesis
    action.cc
    construction.cc
    detector.cc
    generator.cc
    mysteppingaction.cc
    physics.cc
    runaction.cc
    thesis.cc
    # Add other source files here
)
target_link_libraries(thesis ${Geant4_LIBRARIES})

add_custom_target(Simulation DEPENDS thesis)
