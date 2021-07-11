# add hdf5 if not found
set( hdf5_archive_url
    "https://github.com/ORNL-Fusion/hdf5_archive/blob/main/CMake-hdf5-1.12.1.tar.gz?raw=true" )

set( hdf5_archive ${prefix}/hdf5.tar.gz )
file( DOWNLOAD ${hdf5_archive_url} ${hdf5_archive} )
file( ARCHIVE_EXTRACT 
      INPUT ${hdf5_archive}
      DESTINATION ${prefix} )

execute_process( COMMAND bash build-unix.sh
                 WORKING_DIRECTORY ${prefix}/CMake-hdf5-1.12.1 )

add_custom_target( hdf5_build )

add_custom_command( TARGET hdf5_build
                    PRE_BUILD COMMAND bash build-unix.sh 
                    WORKING_DIRECTORY ${prefix}/CMake-hdf5-1.12.1 )


add_library( hdf5 INTERFACE )

add_dependencies( hdf5 hdf5_build )

# Captain! Can you break up the string?
set( hdf5_install_dir 
     "${prefix}/CMake-hdf5-1.12.1/build/_CPack_Packages/Linux/STGZ/HDF5-1.12.1-Linux/HDF_Group/HDF5/1.12.1" )

target_include_directories( hdf5 INTERFACE ${hdf5_install_dir}/include )

# Captain! Add a check for if the user is running on a mac - replace .so with .dylib or .a
# to be platform independent
target_link_libraries( hdf5 INTERFACE ${hdf5_install_dir}/lib/libhdf5.so )
