cmake_minimum_required(VERSION "3.3")

set(CMAKE_CXX_STANDARD "11")
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(wopl-textconv
  "src/wopl/wopl_file.c"
  "src/wopn/wopn_file.c"
  "src/textconv.cc")

include(GNUInstallDirs)
install(TARGETS wopl-textconv DESTINATION "${CMAKE_INSTALL_BINDIR}")
