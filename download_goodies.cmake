# What happens if we put this in the function?
include( ExternalProject )

# Add checks for ARGVN?
function( download_goodies )

# See if you can use the alternative syntax specifically for functions...
cmake_parse_arguments( tmp 
                       ""
                       "target_name;prefix;git_repository;configure_command;install_command"
                       "cmake_args" 
                       ${ARGN} )

# All of the options below can be extracted as properties.
ExternalProject_Add( ${tmp_target_name} 

  PREFIX ${tmp_prefix}
  GIT_REPOSITORY ${tmp_git_repository}
  CONFIGURE_COMMAND ${tmp_configure_command}
  CMAKE_ARGS ${tmp_cmake_args}
  INSTALL_COMMAND ${tmp_install_command}
)

endfunction()
