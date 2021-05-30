#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <iterator>

#include "libconfig.h++"

/* instead of yagni, make an extendability note */

/* This class wraps the libconfig object and inserts
   a key-string mapping in between. This creates an
   internal separation between the internal representation
   of the configuration and the I/O details of obtaining it
   */

/* Captain! How should the local vs full module path be handled? How about functions
   that concatenate and return vs just getting the local path? */

/* extensibility note: augment this to derive from a string query base class */
class libconfig_string_query
{
  public:

  libconfig_string_query( std::string libconfig_file = "" );

  template< typename T >
  void operator()( std::string const query_key, T &query_value ) const
  {
    bool success = cfg.lookupValue( query_key, query_value );

    if( success == false ) 
    {
      std::cout << "invalid query key: " << query_key << std::endl;
      exit(0);
    }
  }

  private:

  libconfig::Config cfg;
};

/* Captain! Can you use reflection to generate the names of the fields based on the
   key? */
class config_module_base
{
  public:

  config_module_base( class libconfig_string_query const &query,
                      std::string module_path = "");

  /* get config sub-module */
  std::shared_ptr< config_module_base > get( int key );

  /* a config value */
  template< typename T >
  void get( int key, T &val );

  template< typename T >
  void generate_sub_module( int key );

  protected:

  std::string const &get_module_path() { return module_path; }

  /* data structure maps internal representation ---> string name of config in config file */
  std::unordered_map< int, std::string > lookup;

  /* data structure maps internal representation ---> string name of a child module in 
     config file */
  std::unordered_map< int, std::shared_ptr< config_module_base > > sub_modules;

  /* string name of this modules path in the config file */
  std::string const module_path;

  /* I/O interface to the config file */
  class libconfig_string_query const &query;
};

/* Captain! Move the derived stuff into different files! Then you can implement these
   specifically for the unit tests and migrate them over to the main codebase if
   they end up being useful. Also add documentation about how to add new configurations */
class ionization_process final : public config_module_base
{
  public:

  /* Captain! Document this */
  enum : int
  {
    file_string,
    temp_grid_string,
    dense_grid_string,
    charge_state_string,
    temp_grid_var_name,
    dense_grid_var_name,
    coeff_var_name
  };

  ionization_process( class libconfig_string_query const &query,
                      std::string module_path );
};

class impurity_particle_source final : public config_module_base
{
  public:

  /* Captain! Document this */
  enum : int
  {
    source_material_z,
    ionization,
    recombination
  };

  impurity_particle_source( class libconfig_string_query const &query,
                            std::string module_path = "impurityParticleSource" );
};

/* Captain! Filling in these now */
class use final : public config_module_base
{
  public:

  enum : int
  {
    use_cuda,
    use_openmp,
    use_mpi,
    /* Captain! Can the next 2 be combined into 1? */
    useionization,
    use_ionization,
    userecombination,
    useperpdiffusion,
    usecoulombcollisions,
    usefriction,
    useanglescattering,
    useheating,
    usethermalforce,
    usesurfacemodel,
    usesheathefield,
    biased_surface,
    usepresheathefield,
    bfield_interp,
    lc_interp,
    generate_lc,
    efield_interp,
    presheath_interp,
    density_interp,
    temp_interp,
    flowv_interp,
    gradt_interp,
    odeint,
    fixedseeds,
    fixed_seeds,
    particleseeds ,
    geom_trace ,
    geom_hash,
    geom_hash_sheath,
    particle_tracks,
    particle_source_space,
    particle_source_energy,
    particle_source_angle,
    particle_source_file,
    spectroscopy,
    use3dtetgeom,
    flux_ea,
    usecylsymm,
    usefieldalignedvalues,
    force_eval,
    check_compatibility,
    use_sort,
    use_adaptive_dt
  };

  use( class libconfig_string_query const &query,
       std::string module_path = "flags" );
};

/* top-level config module */
class gitr_config_interface final : public config_module_base
{
  public:

  gitr_config_interface();
};
