#.rst:
# FindJulia
# ---------
#
# Finds Julia library for embedding
#
# This will define the following variables::
#
#   Julia_FOUND    - True if the system has Julia
#   Julia_VERSION  - The version of Julia which was found
#


find_program(Julia_EXECUTABLE julia DOC "Julia executable")

if(NOT Julia_EXECUTABLE)
    return()
endif()


execute_process(
  COMMAND ${Julia_EXECUTABLE} --version
  OUTPUT_VARIABLE Julia_VERSION
  RESULT_VARIABLE RESULT
)
if(RESULT EQUAL 0)
string(REGEX REPLACE ".*([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1"
    Julia_VERSION ${Julia_VERSION})
endif()


execute_process(
  COMMAND ${Julia_EXECUTABLE} -e "print(Sys.BINDIR)"
  OUTPUT_VARIABLE Julia_HOME
  RESULT_VARIABLE RESULT
)

get_filename_component(Julia_DIR ${Julia_HOME} DIRECTORY)



set(Julia_DEFINITIONS -fPIC -DJULIA_INIT_DIR="${Julia_HOME}")
set(Julia_INCLUDE_DIRS ${Julia_DIR}/include/julia)
set(Julia_LIBRARY_DIRS ${Julia_DIR}/lib ${Julia_DIR}/lib/julia)

find_library( Julia_LIBRARIES
    NAMES ${Julia_LIBRARY_NAME} julia
    PATHS ${Julia_LIBRARY_DIRS}
)


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Julia
  FOUND_VAR Julia_FOUND
  REQUIRED_VARS
    Julia_DIR
    Julia_HOME
    Julia_EXECUTABLE
    Julia_DEFINITIONS
    Julia_INCLUDE_DIRS
    Julia_LIBRARIES
    Julia_LIBRARY_DIRS
  VERSION_VAR Julia_VERSION
)
