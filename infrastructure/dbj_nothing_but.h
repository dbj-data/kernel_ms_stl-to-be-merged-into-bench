#pragma once

#include "../nanoclib/nanoclib.h"
#include <type_traits>

namespace dbj {
template<typename T>
struct nothing_but final 
{
    static_assert( std::is_trivially_constructible_v<T> ) ;

    using type = nothing_but ;
    using value_type = T ;
    using value_type_ref = T & ;
    using value_type_cref = T const & ;

     nothing_but( ) noexcept = default ;
     // strictly by value only?
     nothing_but(T instance_ ) noexcept : value(instance_) {   }

    template < typename U >
    nothing_but(U const & never_ ) : value(never_)   {  
        // static_assert(false, "should never be here?");
      }

    // no casting to ref OR val is allowed
    // by deleting these two
     operator value_type_ref () noexcept = delete ;
     operator value_type_cref() const noexcept = delete ;

    value_type_ref ref () noexcept { return value; } 
    value_type_cref ref () const noexcept { return value; } 

    friend bool is_empty(  const value_type_ref specimen_ ) 
    {
        // by empty we mean trivialy constructed
        static Nb_int empty_ ; 
        return  specimen_ ==  empty_ ;
    }

    private:
    // apparently copy and move is NP here
    // all is by value
    T value{} ;
}; // nothing_but

} // dbj

int sample_nothing_but( int argc, char ** argv)
{
    using Nb_int = dbj::nothing_but<int> ;

    auto mover = [] ( Nb_int nint_1, Nb_int nint_2 = {} ) -> Nb_int  { return nint_1 = nint_2; };

    Nb_int  nint_2(0xFFUL) ; // 0
    Nb_int  nint_1 ; // 0

    SX("%d", nint_1.ref() );
    // no casting to ref OR val is allowed
    // SX("%d", (Nb_int::value_type)nint_ );

    SX("%d", (nint_1 = 13).ref() );
    SX("%d", (nint_2 = nint_1).ref() );
    SX("%d", mover( nint_1, nint_2 ).ref()) ;

return 42;
}