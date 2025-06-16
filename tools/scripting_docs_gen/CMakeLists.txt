#  SuperTux - Scripting reference generator
#  Copyright (C) 2023 Vankata453
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.1)

project(ScriptingDocsGen)
set(CMAKE_CXX_STANDARD 17)

# Add SuperTux CMake module path.
get_filename_component(PROJECT_SOURCE_DIR ../../ ABSOLUTE)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/mk/cmake)

# Allow for generating SuperTux documentation directly from here.
include(SuperTux/BuildDocumentation)

# Find packages
# find_package(TINYXML2 REQUIRED)

# Set include directories
include_directories(${TINYXML2_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Search for source files
file(GLOB SRCFILES *.cpp)
add_executable(ScriptingDocsGen ${SRCFILES})

# Link libraries
target_link_libraries(ScriptingDocsGen PUBLIC tinyxml2)
