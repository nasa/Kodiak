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

cmake_minimum_required(VERSION 3.5)
project(kodiak)

############################################################
#
# Language and compilation flags
#
############################################################

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_definitions(-Wall -frounding-math -pedantic -Wextra -Wno-parentheses -g -O0)


############################################################
#
# Include and source files
#
############################################################

include_directories(
		src
		src/Adapters
)

set(SOURCE_FILES
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
		src/Value.cpp
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
		)


############################################################
#
# Kodiak Library
#
############################################################

add_library(KodiakDynamic SHARED ${SOURCE_FILES})
target_link_libraries(KodiakDynamic prim)
target_link_libraries(KodiakDynamic boost_serialization)
set_target_properties(KodiakDynamic PROPERTIES OUTPUT_NAME kodiak)

add_library(KodiakStatic STATIC ${SOURCE_FILES})
target_link_libraries(KodiakStatic prim)
target_link_libraries(KodiakStatic boost_serialization)
set_target_properties(KodiakStatic PROPERTIES OUTPUT_NAME kodiak)


############################################################
#
# Other modules
#
############################################################

add_subdirectory(examples)
add_subdirectory(tests)
