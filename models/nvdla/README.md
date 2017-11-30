# NVDLA

This is a NVDLA SystemC module

## How to include this model in a project:

* Copy the misc/FindNVDLA.cmake in your project repository.
* Add the following lines into your CMakeLists.txt:
    ``` cmake
    FIND_PACKAGE(NVDLA)
    if(NVDLA_FOUND)
      INCLUDE_DIRECTORIES(${NVDLA_INCLUDE_DIRS})
      LINK_DIRECTORIES(${NVDLA_LIBRARY_DIRS})
    else()
      MESSAGE(FATAL_ERROR "NVDLA not found.")
    endif()
    ```

* Do the same for GreenLib as this module requires GreenLib.

You're now able to use NVDLA headers and link the toplevel with NVDLA_LIBRARIES.
