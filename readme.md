
# 1. Windows and C++ exceptions

> (c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj/

- [1. Windows and C++ exceptions](#1-windows-and-c-exceptions)
  - [1.1. Usage](#11-usage)
  - [1.2. Thoughts and Issues](#12-thoughts-and-issues)
    - [1.2.1. MS STL and SEH](#121-ms-stl-and-seh)
      - [1.2.1.1. Down the Rabbit Hole of MS STL](#1211-down-the-rabbit-hole-of-ms-stl)
      - [1.2.1.2. Into the realm of Windows](#1212-into-the-realm-of-windows)
      - [1.2.1.3. Use Watson instead?](#1213-use-watson-instead)
    - [1.2.2. SEH friendly, standard Windows C++, you can do too](#122-seh-friendly-standard-windows-c-you-can-do-too)
    - [1.2.3. COM, C++ and /kernel builds](#123-com-c-and-kernel-builds)
  - [1.3. Conclusion(s)](#13-conclusions)


What seems to be the issue? The issue seems to be it is not clearly documented how to use standard C++ and MS STL without C++ exceptions, on Windows. Many people are even unaware they can use any MS STL without exceptions.

**SEH build** -- is cl.exe C/C++ build without any `/EH` switch or with a famous `/kernel` switch. [SEH](https://docs.microsoft.com/en-us/cpp/cpp/structured-exception-handling-c-cpp?view=vs-2019) is intrinsic to Windows. SEH is in the foundations of Windows programming.

I have made [this little project](https://github.com/DBJDBJ/kernel_ms_stl) to approve or disapprove your doubts. Also on the official side, things are happening around this issue:

- https://github.com/microsoft/STL/issues/1289
- https://github.com/microsoft/STL/issues/639


My findings and thoughts are also in comment or snippets in the samples. What is perhaps interesting in here is to use this project, to try and check different parts of MS STL behaviour, in the SEH builds. And then follow through debugger, to where is that code taking you into the depths of the vast MS STL code base.

## 1.1. Usage

Of course this is strictly Windows code. And [this is **VS Code** project](https://github.com/DBJDBJ/kernel_ms_stl). If you are reading this, it is safe to assume you know how to use VS Code to do C++ builds. Thus you will know how to adjust the `.vscode` folder to your situation. There is also clean.cmd; hint: it clears the cruft left after the builds.

You can put your own code in the `program.cpp`. `extern "C" int program (int argc , char ** argv )` is where the user code starts. It is called from a micro framework where the SEH handling is implemented.

If SEH exception is raised that is caught inside `dbj_main`, and new "minidump" dmp file is created. You are informed where is it saved; what is the full path. Since SEH is intrinsic to Windows and CL.exe, that `dbj_main` always works and catches all potential SE's.

To open that file you need **Visual Studio**. After which [in the upper right corner](https://docs.microsoft.com/en-us/visualstudio/debugger/using-dump-files?view=vs-2019), you will spot the link to the native debugging entitled ["Debug With Native Only"](https://docs.microsoft.com/en-us/visualstudio/debugger/media/dbg_dump_summarypage.png?view=vs-2019). Click on that and soon you will be pushed to the point where the actual C++ or SEH exception was thrown from. Thus you need Visual Studio too.

![vstudio_minidump_dialogue](https://docs.microsoft.com/en-us/visualstudio/debugger/media/dbg_dump_summarypage.png?view=vs-2019)

Generating minidump is one very powerful feature. Almost all of[ my/our Windows apps are implementing it](https://github.com/DBJDBJ/kernel_ms_stl/blob/master/readme.md).

## 1.2. Thoughts and Issues

**Standard and SEH**

It seems (at least to me) author of C++ (one Mr B. Stroustrup) has expressed explicit dislike for MSFT SEH, in this very recent paper: 
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1947r0.pdf . 

Thing is there is a lot of customers who are "windows only" and a lot of projects serving them. Especially those needing low level Windows "server side" high performance components.

### 1.2.1. MS STL and SEH

So how is standard C++ standard library, implemented by Microsoft team, aka MS STL, C++ exception throwing designed and implemented? (circa 2020 OCT)

From inside MS STL, as far as I can see, only eight exceptions are thrown. And they are thrown by calling eight `noreturn` functions. 

All SEH raising 8 functions, existing in MS STL source available, circa 2020 Q4, are  implemented inside https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp. But what is calling them?

#### 1.2.1.1. Down the Rabbit Hole of MS STL

MS STL is using and depends on `_HAS_EXCEPTIONS` inbuilt macro.

From wherever in MS STL exception is to be thrown one of those eight is called. As an example if you do this

```cpp
std::vector<bool> bv_{ true, true, true } ;
// throws standard C++ exception, but
// transparently becomes SE (Structured Exception)
// in case /kernel is used or there is simply no /EH switch
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
Where `Xran()` is one of the only two noreturn points available inside the `std::vector<T>`
```cpp
// <vector> # 2837
    [[noreturn]] void _Xlen() const {
        _Xlength_error("vector<bool> too long");
    }
    // called from std::vector at() method
    [[noreturn]] void _Xran() const {
        _Xout_of_range("invalid vector<bool> subscript");
    }
```
Where that `_Xout_of_range` is declared inside `<xutility>`, together with friends
```cpp
// <xutility> #5817
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_alloc();
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char*);
```
There is eight of them. Above six (and other two for: `bad_function_call` and `regex_error`) are defined (implemented) in the above mentioned https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp . Available as part of MS STL open source. Back to our `_Xout_of_range` friend
```cpp
// xthrow.cpp # 24
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char* const _Message) {
    _THROW(out_of_range(_Message));
}
```
And that _THROW is defined in relation to, do we have, or do we not have C++ exceptions compiled in the current build. Meaning: depending on what is the value of the `_HAS_EXCEPTIONS` macro. 

All is in the following section of `<yvals.h>`
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
You will understand, in case of no C++ exceptions that becomes
```cpp
// xthrow.cpp # 24
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char* const _Message) {
    out_of_range(_Message)._Raise() ;
}
```

Which means on the instance of `std::out_of_range` exception type there has to be this little peculiar `_Raise()` method. 

And, for some people, this is the point of contention where MS STL is leaving the realm of ISO C++ and entering the realm of Windows C++. 

#### 1.2.1.2. Into the realm of Windows

If irrelevant for this analysis, code was left out bellow.

```cpp
#if _HAS_EXCEPTIONS

// <exception> # 32
#include <malloc.h>
#include <vcruntime_exception.h>
```
Thus above one can understand in case of C++ exceptions in use this `<vcruntime_exception.h>` gets involved. If curious please follow that path. That is where `std::exceptions` is actually implemented. Back to `<exception>`:
```cpp
// <exception> # 63
#else // !  _HAS_EXCEPTIONS
```
For `_HAS_EXCEPTIONS == 0`, there is this global `_Raise_handler`
```cpp
using _Prhand = void(__cdecl*)(const exception&);
// <exception> # 76
extern _CRTIMP2_PURE_IMPORT _Prhand _Raise_handler; // pointer to raise handler
```
And in MS STL there is separate version of `std::exception` in existence for `_HAS_EXCEPTIONS == 0` scenario
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
That is obviously called to set the global raise handler, before any of the exceptions can be used in the `_HAS_EXCEPTIONS == 0` scenario.

And here is this above mentioned little and peculiar `_Raise()` method which is used instead of the `throw` keyword which is forbidden in the `_HAS_EXCEPTIONS == 0` scenario:

```cpp    
// <exception> # 106
[[noreturn]] void __CLR_OR_THIS_CALL _Raise() const 
{ // raise the exception
    if (_STD _Raise_handler) {
        (*_STD _Raise_handler)(*this); // call raise handler if present
    }
    _Doraise(); // call the protected virtual
    _RAISE(*this); // raise this exception
}

protected:
    virtual void __CLR_OR_THIS_CALL _Doraise() const {} // perform class-specific exception handling
}; // eof std::exception for   _HAS_EXCEPTIONS == 0 scenario
// <exception> # 198
#endif // _HAS_EXCEPTIONS
```
`bad_alloc`, `bad_array_new_length`, `bad_exception` also have different versions for _HAS_EXCEPTIONS == 0 scenario. Back to investigation. That std exception `_Raise()` method uses two levels of indirection: `_Raise_handler` global function pointer and protected `_Doraise` method, before eventually calling the `_RAISE` macro. Remember all of of that is inside `[[noreturn]] void _Raise() const` method on the non standard MS STL version of `std::exception`.

And if we point back to our above `<yvalsh>` mention we shall understand, for `_HAS_EXCEPTIONS == 0` scenario,  `_RAISE(x)` macro is defined as:
```cpp
// <yvals.h> # 475
#ifdef _DEBUG
#define _RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG
```

#### 1.2.1.3. Use Watson instead? 

And that is interesting, to put it mildly. For `_HAS_EXCEPTIONS == 0` scenario, MS STL on each SEH raised, actually calls [Dr Watson to do the job](https://docs.microsoft.com/en-us/windows-hardware/drivers/devtest/28725-use-watson-instead). Sherlock is nowhere to be seen.

But that is fine and OK as we have "come out on the other side" into the wonderful kingdom of Windows Drivers. Driver Technologies, Tools for Testing Drivers and namely this little known office of "Windows Error Reporting" to the inner circle known under the acronym of [WER](https://docs.microsoft.com/en-us/windows/win32/wer/windows-error-reporting).

WER basically is that place from where you can call back to daddy and complain. 

*"... enables users to notify Microsoft of application faults, kernel faults, unresponsive applications, and other application specific problems. Microsoft can use the error reporting feature to provide customers with troubleshooting information, solutions, or updates for their specific problems. Developers can use this infrastructure to receive information that can be used to improve their applications..."*

But do not fret. We do not do that. 

Windows as you know it today is actually Windows NT. And one of the foundation stones of Win NT are "Structured Exceptions" aka [SEH](https://en.wikipedia.org/wiki/Microsoft-specific_exception_handling_mechanisms#SEH).  `_invoke_watson` simply raises the SE aka "Structured Exception". And what I do is I simply catch it on the top level. Hint: `main()` is the good place.

Ditto, in case of [SE caught](https://docs.microsoft.com/en-us/windows/win32/debug/using-an-exception-handler), what I do is create and save a minidump specific to my application. And looking into that minidump file with Visual Studio, I can pinpoint the issue that made the application misbehave. That includes every possible issue, not just C++ exceptions being thrown. And that is very powerful. 

I simply have this standard SE aware main in each and every of my WIN apps. That is **not** very complicated and has a lot of benefits. 

Please do understand SE is inbuilt in Windows and in the CL compiler and there are SE intrinsics too. Including the [keywords added](https://docs.microsoft.com/en-us/windows/win32/debug/abnormaltermination) to both C and C++.

### 1.2.2. SEH friendly, standard Windows C++, you can do too

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
    const char * what () const noexcept { return msg_ ; 
    explicit  error( constants::error_type ex_ ) noexcept : err_(ex_)   
    { /* here make the message by the code */ }
    constants::error_type code() const {  return err_ ;    }

#ifndef MY_RAISE 
#ifdef _DEBUG
#define MY_RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define MY_RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG    
#endif // !  DBJ_RAISE

    // we raise the SEH from here
    // and through calling _invoke_watson
    [[noreturn]] void raise() const { 
        DBJ_RAISE(*this) ;
    }

} ; // eof error
```
We will always use the `MY_THROW` macro
```cpp
#if _HAS_EXCEPTIONS == 0
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
To enjoy the SEH benefits, your Windows main should always be "SEH enabled"
```cpp
// The "Standard" main()
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

If you build with `/EHsc` your app will be: c++ exceptions **and** SEH enabled. If built without, you will be intrinsic SEH enabled. SEH intrinsics are in `<excpt.h>`

> In Windows C/C++, SEH is always there

 ### 1.2.3. COM, C++ and /kernel builds

["Compiler COM Support"](https://docs.microsoft.com/en-us/cpp/cpp/compiler-com-support?redirectedfrom=MSDN&view=vs-2019) was designed, implemented and tested to also use C++ exceptions. Not SEH.[MSFT COM](https://en.wikipedia.org/wiki/Component_Object_Model) predates C++ standardizations. Just like SEH does.

In 2020 Q4, if and when attempting C++ `/kernel`  or no `/EH` builds, C++ exceptions are replaced with SEH. 

You need to know right now [things are happening in there "by accident"](https://github.com/MicrosoftDocs/cpp-docs/issues/2494#issuecomment-701200395). Pleas do not rely on `<comdef.h>` `/kernel` or not `/EH` combination until further notice. 

"Compiler COM Support" and SEH is accidental combination that works 2020 Q4.

MSFT ["Compiler COM Support"](https://docs.microsoft.com/en-us/cpp/cpp/compiler-com-support?redirectedfrom=MSDN&view=vs-2019) I do like, it is C++, it is simple and it "just works". But these days it is meeting the not-so-simple standard C++, and WG 21 representatives and followers, inside MSFT. So somebody has decided \<comdef.h> and friends will firmly stay inside c++ exceptions territory. Which is very wierd since all of the MS STL does easily switch between SEH and C++ exceptions.

## 1.3. Conclusion(s)

Many people are, but I am not that perturbed with MS STL apparent duality and ability to transparently switch of C++ exceptions easily.

Neither many projects are usurped too. Why not? Here are the facts. Most of the customers are on Windows. WIN32 is primary Windows API. WIN32 is C. Same as UTF16 (`wchar_t`) is standard on windows, not `char`, SEH is standard on Windows, not C++ exceptions.

For Windows code, SEH is the OS norm.
