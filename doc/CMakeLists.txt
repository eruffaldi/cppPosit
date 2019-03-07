project(cppPositDoc)

option(EIGEN_INTERNAL_DOCUMENTATION "Build internal documentation" OFF)


# Set some Doxygen flags
set(DOXY_PROJECT_NAME             "cppPosit")
set(DOXY_PROJECT_VERSION		   "")
set(DOXY_OUTPUT_DIRECTORY_SUFFIX  "")
set(DOXY_BINARY_DIR "\"${CMAKE_CURRENT_BINARY_DIR}\"")
set(DOXY_INPUT                    "\"${CMAKE_CURRENT_SOURCE_DIR}/../include\" \"${CMAKE_CURRENT_SOURCE_DIR}\"")
set(DOXY_HTML_COLORSTYLE_HUE      "220")
set(DOXY_TAGFILES                 "")
if(INTERNAL_DOCUMENTATION)
  set(DOXY_INTERNAL                 "YES")
else(EINTERNAL_DOCUMENTATION)
  set(DOXY_INTERNAL                 "NO")
endif(INTERNAL_DOCUMENTATION)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
)

add_custom_target(doc ALL
  COMMAND doxygen
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})