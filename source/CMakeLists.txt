#------------------------------------------------------------------------------#
# Distributed under the OSI-approved Apache License, Version 2.0.  See
# accompanying file Copyright.txt for details.
#------------------------------------------------------------------------------#

add_subdirectory(adios2)
add_subdirectory(utils)

if(ADIOS2_HAVE_HDF5_VOL)
  add_subdirectory(h5vol)
endif()
