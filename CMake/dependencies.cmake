# Handle external dependencies

include( ExternalProject )

set( prefix "${CMAKE_BINARY_DIR}/external" )

# CUDA

include( CMake/cuda.cmake ) # ---> creates target CUDA::cudart

# HDF5
include( CMake/hdf5.cmake )

list( APPEND dependencies hdf5 )

# MPI

include( CMake/mpi.cmake ) # ---> creates target mpi

set( dependencies "" )

# Thrust

include( CMake/thrust.cmake ) # ---> creates target thrust

list( APPEND dependencies thrust )

# Libconfig

include( CMake/libconfig.cmake ) # ---> creates target libconfig

list( APPEND dependencies libconfig )

# NETCDF

include( CMake/netcdf.cmake ) # ---> creates target netcdf

list( APPEND dependencies netcdf )
