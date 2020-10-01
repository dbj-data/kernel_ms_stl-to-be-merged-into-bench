
#include "dbj_kernel.h"

extern "C" int main (int argc, char ** argv)  
{
  dbj_nanoc_win_vt100_initor_();
 return dbj_main (argc, argv) ;
}

#if 0
// --------------------------------------------------------------------------
struct command_line final {
    
    program_cl() = delete ;

    explicit command_line( int argc , char ** argv ) :
       args( argv + 1 ) , prog_path( argv[0])       
    {
        strcpy( prog_basename, strrchr( prog_path, '\\') );
    }

  char ** args{0};
  const char * prog_path ;
  char prog_basename[0xFF]{0};
}; // command_line
#endif // 0