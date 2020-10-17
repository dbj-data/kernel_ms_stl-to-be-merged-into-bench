#pragma once

#include "nanoclib.h"

// --------------------------------------------------------------------------
struct command_line final {

   constexpr inline static int max_arg_count{0xFF};

  int argc_{0} ;
  char * args[max_arg_count]{0};
  char * prog_path{} ;
  char * prog_basename {0};
    
   command_line() = delete ;

 
    explicit command_line( int argc , char * const * const argv ) noexcept :
       argc_( argc - 1) ,
       prog_path( _strdup(argv[0]) ),
          prog_basename( command_line::base_name_(argv[0]) )     
    {
        DBJ_ASSERT(max_arg_count > argc);
            // stay on the safe side,  copy from ** argv
        for ( int k = 0; k < argc_ ; ++k ){
           args[k] = _strdup(argv[k+1] );
        }
    }

    ~command_line() {
        for ( int k = 0; k < argc_ ; ++k ){
           free( args[k] ); 
        }
        free(prog_path);
        free(prog_basename);
    }

    // no copy
    command_line( command_line const &) noexcept = delete ;
    command_line & operator = ( command_line const &) noexcept = delete ;

    // move
    command_line( command_line &&) noexcept = default ;
    command_line & operator = ( command_line &&) noexcept = default ;

    void for_each ( bool (*cb)(const char*) ) 
    {
        for ( int k = 0; k < argc_ ; ++k ){
           if (cb( args[k] )) continue ;
        }
    }

    static char * base_name_ ( const char * path_ ) noexcept 
    {
       DBJ_ASSERT( path_ != nullptr ) ;

       #ifdef _WIN32
       static char path_delim = '\\';
       #else
       static char path_delim = '/';
       #endif

       int str_len_ = strlen(path_);
       DBJ_ASSERT( str_len_ > 2 ) ; // sanity check
       const char * last_delim_pos = strrchr( path_,  path_delim) ;

       if ( last_delim_pos == nullptr) {
          return  _strdup( path_) ;
       }
       DBJ_ASSERT( last_delim_pos + 1 ) ;
       return  _strdup( last_delim_pos + 1) ;
    }
}; // command_line
