
# 1. MS STL + cl /kernel builds

> (c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj/

- [1. MS STL + cl /kernel builds](#1-ms-stl--cl-kernel-builds)
  - [1.1. Usage](#11-usage)
  - [1.2. Thoughts and Issues](#12-thoughts-and-issues)
    - [1.2.1. Bjarne and SEH](#121-bjarne-and-seh)
    - [1.2.2. MS STL and SEH](#122-ms-stl-and-seh)
    - [1.2.3. SEH friendly C++](#123-seh-friendly-c)
    - [1.2.4. The mythical (MS STL)"CORE"](#124-the-mythical-ms-stlcore)

What seems to be the issue? The issue seems to be it is largely undocumented how to use MS STL with cl, while using [the cl /kernel switch](https://docs.microsoft.com/en-us/cpp/build/reference/kernel-create-kernel-mode-binary?view=vs-2019).

I am using this little project to approve or disapprove my doubts. On the official side, things are happening around this issue, albeit not that quickly:

- https://github.com/microsoft/STL/issues/1289
- https://github.com/microsoft/STL/issues/639


Findings and thoughts are in the main.cpp. In comment or snippets and samples. What is interesting here is to use this project, to try and use different parts of MS STL in cl kernel build. And then follow through debugger where is your sample taking you in the depths of the vast MS STL code base.

## 1.1. Usage

Of course this is strictly Windows code. This is **VS Code** project. For debug builds make sure main.cpp is the current file in your VS Code, and then do (Ctrl+Shit+D) or click on the Debug icon on the left side toolbar, to actually start debugging. That action will first (and always) do the  debug build. 

If you are reading this it is safe to assume you know how to use VS Code to do C++ builds. For release builds there is build.cmd . It calls clean.cmd, you can use on its own too.

If SEH exception is thrown that is caught in main, and "minidump" dmp file is created. You are informed where, and what is the full path.

To open that file you need **Visual Studio**. After which in the upper right corner like "smallish windows" you will spot the link to native debugging. Click on that and soon you will be jumped to the point where the actual C++ or SEH exception was thrown from. Thus you need Visual Studio too.

## 1.2. Thoughts and Issues

Prpbably the best page on MS Docs on the subject of SEH vs standard C++ is [here](https://docs.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=vs-2019#default-exception-handling-behavior). If needed, please read and understand before proceeding.

### 1.2.1. Bjarne and SEH

It seems (at least to me) Bjarne has expressed explicit dislike for MSFT SEH
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1947r0.pdf

### 1.2.2. MS STL and SEH
It also seems bellow is a grouping of all SEH raising 6 functions, existing in MS STL.
```cpp
// <xutility> #5817

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_alloc();
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char*);
```
there is two more (declared/defined somewhere/elsewhere)
```cpp
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_function_call();
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xregex_error(const regex_constants::error_type _Code);
```

Implementation of all eight is in https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp

### 1.2.3. SEH friendly C++
(It seems ) This is how that mechanism/design works (as an example ) :
```cpp
namespace my {
    inline namespace constants {
        enum error_type { 
        error_ctype,
        error_syntax
    };
    } // namespace constants

// does not inherit from std::exception
struct error final 
{ 
    error () = default ;

    const char * msg_ {"error"};

    explicit  error( constants::error_type ex_ ) 
    : err_(ex_) 
    { /* default ctor body */ }

    _NODISCARD constants::error_type code() const {
        return err_ ;
    }

#ifndef DBJ_RAISE 
#ifdef _DEBUG
#define DBJ_RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define DBJ_RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG    
#endif // !  DBJ_RAISE

    // MS STL do not throw instance of this type
    // there is no throw in /kernel builds
    // we raise the SEH through calling _invoke_watson
    void raise() const noexcept { 
        DBJ_RAISE(*this) ;
    }

```
 _RAISE is to be found also in yvals.h as :
```cpp
#ifdef _DEBUG
#define _RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG

    constants::error_type err_ ;
} ; // error
```
That might mean c++ type of c++ exception is simply erased in /kernel builds?
Or not. MS STL _THROW macro is used for _NO_EXCEPTIONS builds and that simply does the following:

```cpp
// <yvals> #482
_THROW(x) x._Raise()
```
with _Raise() in `<exception> #186`, on /kernel version of std::exception for _NO_EXCEPTION builds

There is always a function that does not return. A level of indirection to improve the change-ability of the design always exist:
```cpp
[[noreturn]] inline void __cdecl 
    my_error_throw (const constants::error_type code_) 
    {
      error(code_).raise() ;
    }
} // my
```

### 1.2.4. The mythical (MS STL)"CORE"

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



