
#include "dbj_kernel.h"

extern "C" int main (int argc, char ** argv)  {
 return dbj_main (argc, argv) ;
}

#if 0
// --------------------------------------------------------------------------
struct program_cl {
    
    program_cl() = delete ;

    explicit program_cl( int argc , char ** argv ) :
       args( argv + 1 )        
    {
        strcpy( prog_path, argv[0]) ;
        strcpy( prog_basename, strrchr( prog_path, '\\') );
    }

  char ** args{0};
  char prog_path[0xFF * 4 /*aka 1024*/]{0};
  char prog_basename[0xFF]{0};
}; // program_cl
#endif // 0