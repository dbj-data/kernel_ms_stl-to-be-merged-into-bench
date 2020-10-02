
# 1. C++, Windows and the SEH

> (c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj/

- [1. C++, Windows and the SEH](#1-c-windows-and-the-seh)
  - [1.1. Usage](#11-usage)
  - [1.2. Thoughts and Issues](#12-thoughts-and-issues)
    - [1.2.1. Bjarne and SEH](#121-bjarne-and-seh)
    - [1.2.2. MS STL and SEH](#122-ms-stl-and-seh)
      - [1.2.2.1. Down the Rabbit Hole](#1221-down-the-rabbit-hole)
      - [1.2.2.2. Into the realm of Windows](#1222-into-the-realm-of-windows)
      - [1.2.2.3. Use Watson instead?](#1223-use-watson-instead)
    - [1.2.3. SEH friendly C++ you can do](#123-seh-friendly-c-you-can-do)
    - [1.2.4. COM, C++ and /kernel builds](#124-com-c-and-kernel-builds)


What seems to be the issue? The issue seems to be it is not clearly documented how to use MS STL, while using [the cl /kernel switch](https://docs.microsoft.com/en-us/cpp/build/reference/kernel-create-kernel-mode-binary?view=vs-2019). Or while simply not using any '/EH' switch.

I am using this little project to approve or disapprove your doubts. On the official side, things are happening around this issue:

- https://github.com/microsoft/STL/issues/1289
- https://github.com/microsoft/STL/issues/639


Findings and thoughts are in the main.cpp. In comment or snippets and samples. What is interesting here is to use this project, to try and use different parts of MS STL in cl kernel build. And then follow through debugger where is your sample taking you in the depths of the vast MS STL code base.

## 1.1. Usage

Of course this is strictly Windows code. This is **VS Code** project. 

If you are reading this it is safe to assume you know how to use VS Code to do C++ builds. Hint: CTRL+SHIFT+B. There is also clean.cmd, hint: it clears the cruft left after the builds.

You can put your own code in the program.cpp. `extern "C" int program (int argc , char ** argv )` is where the user code starts. 

If SEH exception is raised that is caught in `dbj_main`, and "minidump" dmp file is created. You are informed where is it, and what is the full path. SEH is intrinsic to Windows and CL.exe. Thus that `dbj_main` always works and catches all potential SE's.

To open that file you need **Visual Studio**. After which in the upper right corner like "smallish windows" you will spot the link to native debugging. Click on that and soon you will be jumped to the point where the actual C++ or SEH exception was thrown from. Thus you need Visual Studio too.

## 1.2. Thoughts and Issues

Prpbably the best page on MS Docs on the subject of SEH vs standard C++ is [here](https://docs.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=vs-2019#default-exception-handling-behavior). If needed, please read and understand before proceeding.

### 1.2.1. Bjarne and SEH

It seems (at least to me) Bjarne has expressed explicit dislike for MSFT SEH
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1947r0.pdf

### 1.2.2. MS STL and SEH

So how is MS STL exception throwing designed and implemented? (circa 2020 OCT)

From inside MS STL it appears only eight exceptions are thrown. And they are thrown by calling eight `noreturn` functions.
Bellow is a grouping of all SEH raising 8 functions, existing in MS STL source available.
```cpp
// <xutility> #5817

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_alloc();
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char*);
```
There is two more (declared/defined somewhere/elsewhere)
```cpp
// probably in <functional>
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_function_call();
// probably in <regex>
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xregex_error(const regex_constants::error_type _Code);
```

Implementation of those eight is in https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp

#### 1.2.2.1. Down the Rabbit Hole

From wherever in MS STL if exception is to be thrown one of those eight is called. As an example if you do this
```cpp
std::vector<bool> bv_{ true, true, true } ;
// throws standard C++ exception when not built with /kernel
auto never = bv_.at(22);
```
`at()` is very simple
```cpp
// <vector> #2581
    _NODISCARD reference at(size_type _Off) {
        if (size() <= _Off) {
            _Xran();
        }

        return (*this)[_Off];
    }
```
Where `Xran()` is on of the only two noreturn points 
```cpp
// <vector> # 2837
    [[noreturn]] void _Xlen() const {
        _Xlength_error("vector<bool> too long");
    }

    [[noreturn]] void _Xran() const {
        _Xout_of_range("invalid vector<bool> subscript");
    }
```
And that `_Xout_of_range` is declared in xutility, with friends
```cpp
// <xutility> #5817
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_alloc();
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char*);
```
And defined in the above mentioned https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp 
```cpp
// xthrow.cpp # 24
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char* const _Message) {
    _THROW(out_of_range(_Message));
}
```
And that _THROW is defined in relation to do we have or do we not have C++ exceptions in the current build 
```cpp
//<yvals.h> # 447
// EXCEPTION MACROS
#if _HAS_EXCEPTIONS
#define _TRY_BEGIN try {
#define _CATCH(x) \
    }             \
    catch (x) {
#define _CATCH_ALL \
    }              \
    catch (...) {
#define _CATCH_END }

#define _RERAISE  throw
#define _THROW(x) throw x

#else // _HAS_EXCEPTIONS
#define _TRY_BEGIN \
    {              \
        if (1) {
#define _CATCH(x) \
    }             \
    else if (0) {
#define _CATCH_ALL \
    }              \
    else if (0) {
#define _CATCH_END \
    }              \
    }

#ifdef _DEBUG
#define _RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG

#define _RERAISE
#define _THROW(x) x._Raise()
#endif // _HAS_EXCEPTIONS
```
Thus if we look back into 
```cpp
// xthrow.cpp # 24
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char* const _Message) {
    _THROW(out_of_range(_Message));
}
```
We will understand, in case of no C++ exceptions that becomes
```cpp
// xthrow.cpp # 24
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char* const _Message) {
    out_of_range(_Message)._Raise() ;
}
```

#### 1.2.2.2. Into the realm of Windows

Which means on the instance of `std::out_of_range` exception type there has to be this little peculiar `_Raise()` method. And, for some people, this is the point of contention where MS STL is leaving the realm of ISO C++ and entering the realm of Windows. (irrelevant for this analysis, code left out bellow)

```cpp
#if _HAS_EXCEPTIONS

// <exception> # 32
#include <malloc.h>
#include <vcruntime_exception.h>
```
```cpp
// <exception> # 63
#else // !  _HAS_EXCEPTIONS
```
For _HAS_EXCEPTIONS==0 scenario, there is this global `_Raise_handler`
```cpp
using _Prhand = void(__cdecl*)(const exception&);
// <exception> # 76
extern _CRTIMP2_PURE_IMPORT _Prhand _Raise_handler; // pointer to raise handler
```
And in MS STL there is version of std::exception in existence for _HAS_EXCEPTIONS==0 scenario
```cpp
// <exception> # 81
class exception { // base of all library exceptions
public:
    static _STD _Prhand _Set_raise_handler(_STD _Prhand _Pnew) { // register a handler for _Raise calls
        const _STD _Prhand _Pold = _STD _Raise_handler;
        _STD _Raise_handler      = _Pnew;
        return _Pold;
    }
```
That is obviously called to set the global raise handler, before any of the exceptions can be used in _HAS_EXCEPTIONS==0 scenario.
```cpp
    // this constructor is necessary to compile
    // successfully header new for _HAS_EXCEPTIONS==0 scenario
    explicit __CLR_OR_THIS_CALL exception(const char* _Message = "unknown", int = 1) noexcept : _Ptr(_Message) {}
```
And here is this above mentioned little and peculiar `_Raise()` method which is used instead of the `throw` keyword which is forbidden in CL _HAS_EXCEPTIONS==0 scenario
```cpp    
// <exception> # 106
    [[noreturn]] void __CLR_OR_THIS_CALL _Raise() const { // raise the exception
        if (_STD _Raise_handler) {
            (*_STD _Raise_handler)(*this); // call raise handler if present
        }

        _Doraise(); // call the protected virtual
        _RAISE(*this); // raise this exception
    }

protected:
    virtual void __CLR_OR_THIS_CALL _Doraise() const {} // perform class-specific exception handling
}; // eof std::exception for   _HAS_EXCEPTIONS==0 scenario
// <exception> # 198
#endif // _HAS_EXCEPTIONS
```
`bad_alloc`, `bad_array_new_length`, `bad_exception` also have different versions for _HAS_EXCEPTIONS==0 scenario. Back to investigation. That std exception `_Raise()` method uses two more levels of indirection: `_Raise_handler` global function pointer and protected `_Doraise` method, before eventually calling the `_RAISE` macro. Remember all of of that is inside `[[noreturn]] void _Raise() const` method on the non standard MS STL version of `std::exception`.

And if we point back to our above `<yvalsh>` mention we shall understand, for `_HAS_EXCEPTIONS==0` scenario,  `_RAISE(x)` macro is defined as:
```cpp
// <yvals.h> # 475
#ifdef _DEBUG
#define _RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG
```

#### 1.2.2.3. Use Watson instead? 

And that is interesting, to put it mildly. For `_HAS_EXCEPTIONS==0` scenario, MS STL on each exception raise, actually calls [Dr Watson to do the job](https://docs.microsoft.com/en-us/windows-hardware/drivers/devtest/28725-use-watson-instead). Sherlock is nowhere to be seen.

But that is fine and OK as we have "come out on the other side" into the wonderful kingdom of Windows Drivers. Driver Technologies, Tools for Testing Drivers and namely this little known office of "Windows Error Reporting" to the inner circle known under the acronym of [WER](https://docs.microsoft.com/en-us/windows/win32/wer/windows-error-reporting).

WER basically is that place from where you can call back to daddy and complain. 

*"... enables users to notify Microsoft of application faults, kernel faults, unresponsive applications, and other application specific problems. Microsoft can use the error reporting feature to provide customers with troubleshooting information, solutions, or updates for their specific problems. Developers can use this infrastructure to receive information that can be used to improve their applications..."*

But do not fret. 

Windows as you know it is actually Windows NT. And one of the foundation stones of Win NT are "Structured Exceptions" aka [SEH](https://en.wikipedia.org/wiki/Microsoft-specific_exception_handling_mechanisms#SEH).  `_invoke_watson` simply raises the SE aka "Structued Exception". What I do is I simply catch it on the top level. `main()` is the good place.

In case of [SE caught](https://docs.microsoft.com/en-us/windows/win32/debug/using-an-exception-handler) I create and save a minidump specific to my application. And looking into that minidump with Visual Studio, I can pinpoint the issue that made the application misbehave. That includes every possible issue, not just C++ exceptions being thrown. And that is very powerful. 

I simply have this standard SE aware main in each and every of my WIN apps. That is now very complicated and has a lot of benefits. 

Before next section, please do understand SE is inbuilt in the CL compiler and there are SE intrinsics too. Including the [keywords added](https://docs.microsoft.com/en-us/windows/win32/debug/abnormaltermination) to both C and C++.


### 1.2.3. SEH friendly C++ you can do
Now you know how that mechanism and design works. You can do that too in your C++ Windows, SEH friendly code.

```cpp
namespace my {
    inline namespace constants {
        enum error_type { 
        error_ctype,
        error_syntax
    };
    } // my namespace constants

// my::error does not inherit from std::exception
struct error final 
{ 
    error () = default ;

    const char * msg_ {"unknown"};

    const char * what () const noexcept { return msg_ ; }

    explicit  error( constants::error_type ex_ ) noexcept : err_(ex_)   
    { /* here make the message by the code */ }

    constants::error_type code() const {
        return err_ ;
    }

#ifndef MY_RAISE 
#ifdef _DEBUG
#define MY_RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define MY_RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG    
#endif // !  DBJ_RAISE

    // we raise the SEH through calling _invoke_watson
    [[noreturn]] void raise() const { 
        DBJ_RAISE(*this) ;
    }

} ; // eof error
```
We will always use the `MY_THROW` macro
```cpp
#if _HAS_EXCEPTIONS==0
      MY_THROW(x) x.raise() ;
#else
      MY_THROW(x) throw x ;
#endif
```
Lastly, there is always a function that does the raise, and does not return. A level of indirection to improve the change-ability of the design always exist:
```cpp
[[noreturn]] inline void __cdecl 
    error_throw (const constants::error_type code_) 
    {
        MY_THROW( error(code_) ) ;
    }
} // eof my ns
```
Your main should always be "SEH enabled"
```cpp
// build without /EHsc or any other /EH 
// or use the /kernel switch
extern "C" int main (int argc, char ** argv) 
{
     __try 
    { 
        __try {
           my::error_throw( constants::error_type::error_syntax ) ;
        }
        __finally {
            // will be always visited
        }
    }
    __except ( 1 /* 1 == EXCEPTION_EXECUTE_HANDLER */ ) 
    { 
        // your code here
    }
    return 0 ;
}
```
That will always work c++ exception or no C++ exceptions. In case you want C++ exceptions you can not mix that in the same function, so you just call some entry point into your standard C++ app from the `main()` above.

If you build with `/EHsc` your app will be: c++ exceptions **and** SEH enabled. If built without, you will be only SEH enabled.

> In Windows C/C++, SEH is always enabled


<!-- 
 1.2.4. The mythical (MS STL)"CORE"

First mentioned here (by Billy O Neal) :
https://devblogs.microsoft.com/cppblog/stl-features-and-fixes-in-vs-2017-15-8/

\<begin citation>

<i>
The header structure of the STL was changed to allow use of a subset of the library in 
conditions where the user can’t link with msvcp140.dll, such as driver development. 
(Previously the hard dependency on msvcp and pragma detect mismatch were injected by our 
base configuration header included by everything.) The following headers are now 
considered “core” and don’t inject our runtime dependencies (though we do still assume 
some form of CRT headers are present):

```
<cstddef>
<cstdlib>
<initializer_list>
<ratio>
<type_traits>
```

We aren’t actually driver developers ourselves and are interested in feedback
 in this area if there are things we can do to make these more usable in constrained environments.
 </i>

 \<end citation>
 
 Is there still such a thing as "MS STL Core"? If not is it on the roadmap? Is this going to be a MS STL part that will work with the new /kernel- switch variant, I first noticed 2020SEP28 published on-line?
 -->

 ### 1.2.4. COM, C++ and /kernel builds
 
  When attempting C++ /kernel builds, you need to know right now [things are happening in there "by accident"](https://github.com/MicrosoftDocs/cpp-docs/issues/2494#issuecomment-701200395). Pleas do not rely on `<comdef.h>` /kernel combination until further notice.



