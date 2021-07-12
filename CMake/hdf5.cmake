# add hdf5 if not found

include( FindHDF5 )

if( HDF5_FOUND )

  message( STATUS "HDF5 found" )

# download and build automatically
else()

  message( STATUS "HDF5 not found" )

  # Extract the archive
  file( DOWNLOAD ${hdf5_archive} ${prefix}/hdf5 )

  #file( ARCHIVE_EXTRACT
  #      INPUT ${prefix}/hdf5/CMake-hdf5-1.12.1.tar.gz 
  #      DESTINATION ${prefix}/hdf5 )

  # build the project

  # make sure to enforce the build order - create interface targets for the outputs
  # and make sure they are built before netcdf is. Once this works, try to get a MacOSx
  # CI runner going

endif()
