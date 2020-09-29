#pragma once
/*
try and have no includes in here ...
*/
#define DBJ_APP_NAME "DBJ_PSEUDO_RANDOMS"
#define DBJ_APP_VERSION "1.0.0"

#define dbg(x) std::cout << "\n" << std::setw(24) << #x << " : " << (x) 

// note: no \n in here
#define SX(F,X) printf("%s : " F, #X, (X) )

#define DBJ_PROMPT(m) \
printf("\n\n" VT100_BG_BLUE "%-60s\n %-59s\n%-60s" VT100_RESET "\n\n", " ", m , " " )

#define DBJ_PRNT(VT100_, FMT_, m_) \
printf("\n" VT100_  "%s-60\r " FMT_  VT100_RESET , " " , m_ )



