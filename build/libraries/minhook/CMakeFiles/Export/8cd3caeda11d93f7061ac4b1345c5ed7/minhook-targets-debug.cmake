#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "minhook::minhook" for configuration "Debug"
set_property(TARGET minhook::minhook APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(minhook::minhook PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/minhook.x32d.lib"
  )

list(APPEND _cmake_import_check_targets minhook::minhook )
list(APPEND _cmake_import_check_files_for_minhook::minhook "${_IMPORT_PREFIX}/lib/minhook.x32d.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
