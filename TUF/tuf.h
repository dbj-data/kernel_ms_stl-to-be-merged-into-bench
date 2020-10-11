#pragma once
/* (c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj 

test function is: `bool (*)(void);` thus must return true or false

TUF_REGISTRY_MAX_CAPACITY_DEFAULT is 0xFF aka 255
if you do not provide TUF_REGISTRY_MAX_CAPACITY that will be used

on exception thrown or SE raised everything stops

NOTE: dtd lib not used, simplicity achieved but naivete not gained

*/

#include "../nanoclib/nanoclib.h"
#include <assert.h>
#include <stdio.h>

#undef SX
#ifdef _DEBUG
#define SX(F, ...) printf("\n%s[%4d] %12s : " F, __FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__)
#else
#define SX(F,...)
#endif

#undef  TUF_REGISTRY_MAX_CAPACITY_DEFAULT
#define TUF_REGISTRY_MAX_CAPACITY_DEFAULT 0xFFU

#ifndef TUF_REGISTRY_MAX_CAPACITY
#define TUF_REGISTRY_MAX_CAPACITY  TUF_REGISTRY_MAX_CAPACITY_DEFAULT
#endif // TUF_REGISTRY_MAX_CAPACITY

// TUF Unit registration
// non unique names will not compile, as ever 
#define TUFU(name)                                                 \
    bool name (void);                                              \
     static bool _CRT_CONCATENATE(tufu_reg_rezult_ , __COUNTER__ ) \
     = dbj::tuf::REG(#name, name );                                \
     inline bool name (void)

// ------------------------------------------------------------------------
namespace dbj
{
    namespace tuf
    {
        // ------------------------------------------------------------------------
        template <typename FP>
        struct fp_map final
        {
            struct name_fp final
            {
                const char *name;
                FP fp;
            }; // name_fp

            constexpr inline static unsigned fp_max_capacity = TUF_REGISTRY_MAX_CAPACITY;
            inline static name_fp register_[0xFF]{};
            inline static unsigned fp_map_level{};

            constexpr static bool add(const char *name, FP fp)
            {
                // DBJ: add locking in here
                if (fp_map_level == (fp_max_capacity - 1))
                {
                    assert(("TUF registry overflow", false));
                    return false;
                }

                register_[fp_map_level] = {name, fp};
                fp_map_level++;
                return true;
            }

            static void call_all( const char * tu_prompt = nullptr )
            {
                for (unsigned K = 0; K < fp_map_level; ++K)
                {
                    auto &nfp = register_[K];
                    if (nfp.fp) {
                        if ( tu_prompt ) DBJ_INFO("%s", tu_prompt );
                        (void)nfp.fp();
                    }
                }
            }

            static bool call_name( const char * needle_ , const char * tu_prompt = nullptr )
            {
                for (unsigned K = 0; K < fp_map_level; ++K)
                {
                    auto &nfp = register_[K];
                    if (! strcmp( needle_ , nfp.name ) ) 
                    {
                        if (nfp.fp) {
                            if ( tu_prompt ) DBJ_INFO("%s", tu_prompt );
                            return nfp.fp();
                        }
                    }
                }
                return false ; // needle_ not found, or fp found by that name was null
            }

        }; // fp_map

        // ------------------------------------------------------------------------

        using test_fun = bool (*)(void);

        using TUF_MAP = fp_map<test_fun>;

        // dbj: this could be templated by TUF_MAP
        inline bool REG(const char *name, test_fun func)
        {
            return TUF_MAP::add(name, func);
        }

        /**/
    } // namespace tuf
} // namespace dbj

namespace
{
    /**/
    TUFU(dbj)
    {
        printf("\nHello from \n%s", __FUNCSIG__);
        return true;
    }

    TUFU(bbj)
    { 
        printf("\nHello from \n%s", __FUNCSIG__);
        return true;
    }

    extern "C" inline int test_tuf(int, char **)
    {
        using dbj::tuf::TUF_MAP ;

        SX("%d", TUF_MAP::fp_map_level);
        SX("%p", (TUF_MAP::register_[0]).fp);

        TUF_MAP::call_all(        "------------------------------------------------------------");

        TUF_MAP::call_name("dbj", "---------| found by name |----------------------------------");
        TUF_MAP::call_name("bbj", "---------| found by name |----------------------------------");

        return 42;
    }

} // namespace
