# Could I include this in a function?
include( ExternalProject )

# function to download external data give it 1 argument?
# does it even need an external target? It could probably just link one in right
# You're going to replace all the function names - also do some checks on ARGVN?
function( download_goodies )

# No bool or multivalue arguments, only single values. Currently you can parse others.
# If this external project creates a new target, can you access it? looks like it
cmake_parse_arguments( libconfig 
                       ""
                       "target_name;prefix;git_repository;configure_command;install_command"
                       "" 
                       ${ARGN} )

# The options below can be extracted as properties.
ExternalProject_Add( ${libconfig_target_name} 

  PREFIX ${libconfig_prefix}
  GIT_REPOSITORY ${libconfig_git_repository}
  CONFIGURE_COMMAND ${libconfig_configure_command}
  INSTALL_COMMAND ${libconfig_install_command}
)

endfunction()
