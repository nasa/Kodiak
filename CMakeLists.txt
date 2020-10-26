# Notices:
#
# Copyright 2017 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.
# All Rights Reserved.
#
# Disclaimers:
#
# No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED,
# IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT
# SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS,
# HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS
# IS."
#
# Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT,
# ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING
# FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH
# MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.

cmake_minimum_required(VERSION 3.12)
project(Kodiak)


############################################################
#
# Import FILIB
#
############################################################

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

find_package(FILIB REQUIRED)


############################################################
#
# Language and compilation flags
#
############################################################

add_compile_options("-std=c++14" "-fPIC")
add_compile_options("-Wno-unused-command-line-argument")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "GCC detected: adding specific flags")
    add_compile_options("-frounding-math")
endif()

if(${DEBUG})
    message(WARNING "Building in DEBUGGING mode: all optimizations are disabled")
    add_definitions(-DDEBUG=1)
    add_compile_options("-Wall" "-pedantic" "-Wextra")
    add_compile_options("-g" "-O0" "-fsanitize=undefined")
    add_link_options("-fsanitize=undefined")
else()
    add_compile_options("-O3")
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    message(STATUS "OS X found: adding specific linker options")
    add_link_options("-undefined" "dynamic_lookup" ${CXXLNFLAGS})
endif()

############################################################
#
# CPM Package Manager
#
############################################################

set(CPM_DOWNLOAD_VERSION 0.27.2)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION} AND CPM_VERSION STREQUAL CPM_DOWNLOAD_VERSION))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

############################################################
#
# Third Party libraries
#
############################################################

CPMFindPackage(
    NAME Boost
    GITHUB_REPOSITORY Orphis/boost-cmake
    VERSION 1.67.0
	FIND_PACKAGE_ARGUMENTS "COMPONENTS system thread"
)

############################################################
#
# Kodiak
#
############################################################

add_library(KodiakObjects OBJECT
	src/Bifurcation.cpp
	src/Bifurcation.hpp
	src/Expressions/Boolean/Bool.cpp
	src/Expressions/Boolean/Bool.hpp
	src/BranchAndBoundDF.hpp
	src/kodiak.hpp
	src/MinMax.cpp
	src/MinMax.hpp
	src/MinMaxSystem.cpp
	src/MinMaxSystem.hpp
	src/Node.cpp
	src/Node.hpp
	src/Paver.cpp
	src/Paver.hpp
	src/Real.cpp
	src/Real.hpp
	src/System.cpp
	src/System.hpp
	src/types.cpp
	src/types.hpp
	src/Expressions/Boolean/LegacyCertainty.cpp
	src/Expressions/Boolean/LegacyCertainty.hpp
	src/Expressions/Boolean/ConstantNode.cpp
	src/Expressions/Boolean/ConstantNode.hpp
	src/Expressions/Boolean/Node.cpp
	src/Expressions/Boolean/Node.hpp
	src/Expressions/Boolean/Certainty.cpp
	src/Expressions/Boolean/Certainty.hpp
	src/LegacyBool.cpp
	src/LegacyBool.hpp
	src/Value.hpp
	src/Environment.cpp
	src/Environment.hpp
	src/NewPaver.cpp
	src/NewPaver.hpp
	src/NewMinMaxSystem.cpp
	src/NewMinMaxSystem.hpp
	src/Adapters/PRECiSA.cpp
	src/Adapters/PRECiSA.hpp
	src/Adapters/Codiak.h
	src/Adapters/Codiak.cpp
	src/Expressions/Boolean/NotNode.cpp
	src/Expressions/Boolean/NotNode.hpp
	src/Expressions/Boolean/RelationNode.cpp
	src/Expressions/Boolean/RelationNode.hpp
	src/Expressions/Boolean/AndNode.cpp
	src/Expressions/Boolean/AndNode.hpp
	src/Expressions/Boolean/OrNode.cpp
	src/Expressions/Boolean/OrNode.hpp
	src/Expressions/Boolean/ImplicationNode.cpp
	src/Expressions/Boolean/ImplicationNode.hpp
	src/Expressions/Boolean/InvalidBooleanNode.cpp
	src/Expressions/Boolean/InvalidBooleanNode.hpp
	src/Expressions/Boolean/VariableNode.cpp
	src/Expressions/Boolean/VariableNode.hpp
	src/BooleanChecker.cpp
	src/BooleanChecker.hpp
)
target_include_directories(KodiakObjects
	PUBLIC
		src
		src/Adapters
)
target_link_libraries(KodiakObjects
    PUBLIC
	    FILIB::filib++
		Boost::system
)


############################################################
#
# Kodiak Libraries
#
############################################################

add_library(KodiakDynamic SHARED $<TARGET_OBJECTS:KodiakObjects>)
target_link_libraries(KodiakDynamic PUBLIC KodiakObjects)
set_target_properties(KodiakDynamic PROPERTIES OUTPUT_NAME kodiak)

add_library(KodiakStatic STATIC $<TARGET_OBJECTS:KodiakObjects>)
target_link_libraries(KodiakStatic PUBLIC KodiakObjects)
set_target_properties(KodiakStatic PROPERTIES OUTPUT_NAME kodiak)


############################################################
#
# Other modules
#
############################################################

add_subdirectory(examples)

if(EXISTS tests)
  add_subdirectory(tests)
endif()
