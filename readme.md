
# 1. Windows and C++ exceptions

> (c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj/

- [1. Windows and C++ exceptions](#1-windows-and-c-exceptions)
    - [1.0.1. Sampling repo and the current MS STL status](#101-sampling-repo-and-the-current-ms-stl-status)
  - [1.1. Usage](#11-usage)
  - [1.2. Thoughts and Issues](#12-thoughts-and-issues)
    - [1.2.1. MS STL and SEH](#121-ms-stl-and-seh)
      - [1.2.1.1. Down the Rabbit Hole](#1211-down-the-rabbit-hole)
      - [1.2.1.2. Into the realm of Windows](#1212-into-the-realm-of-windows)
      - [1.2.1.3. Use Watson instead?](#1213-use-watson-instead)
    - [1.2.2. SEH friendly, standard Windows C++, you can try](#122-seh-friendly-standard-windows-c-you-can-try)
    - [1.2.3. COM, C++ and /kernel builds](#123-com-c-and-kernel-builds)
  - [1.3. Conclusion(s)](#13-conclusions)


What seems to be the issue? The issue seems to be it is not clearly documented how to use standard C++ and MS STL without C++ exceptions, on Windows. Many people are even unaware they can use any MS STL without exceptions.

NOTE: Although ms docs page on '/kernel' says: no RTTI, no exceptions, user defined new() and delete() operators, etc. As of today (2020 Oct.) RTTI is not switched off if you do not explicitly switch it off, with `/GR-` . Reading that doc page, I was under impression using the `/kernel` switch, does auto-magically switch off all the things documentation mentions as not available when it is used.

Since C++98 one can use standard C++ feature macros:

| Feature	| Macro name |	Value |	Header
|-----------|------------|--------|-------
| Run-time type identification (dynamic_cast, typeid)|	__cpp_rtti	| 199711 |	predefined
| Exception handling |	__cpp_exceptions | 199711 | predefined

<h3>&nbsp;</h3>

WIN32 is C API. To achieve C compatibility on Windows builds we use CL predefined macros:

```cpp
// defined if /kernel switch is used
#ifdef _KERNEL_MODE
printf(  ": _KERNEL_MODE is defined");
#else // ! _KERNEL_MODE
printf(  ": _KERNEL_MODE is NOT defined");
#endif // !_KERNEL_MODE

// depends on the _KERNEL_MODE
#if _HAS_EXCEPTIONS
printf(  ": _HAS_EXCEPTIONS == 1");
#else
printf(  ": _HAS_EXCEPTIONS == 0");
#endif // _HAS_EXCEPTIONS

#if _CPPRTTI 
printf(  ": _CPPRTTI == 1");
#else
printf(  ": _CPPRTTI == 0");
#endif // ! _CPPRTTI

#if _CPPUNWIND 
printf(  ": _CPPUNWIND == 1");
#else // ! _CPPUNWIND 
printf(  ": _CPPUNWIND == 0");
#endif //! _CPPUNWIND 
```
<span id="sehbuild" name="sehbuild">Ditto, the "SEH build" we define as:
</span>

```cpp
#define SEH_BUILD \
(_HAS_EXCEPTIONS == 0) && (_CPPRTTI == 0)
 && (  _CPPUNWIND == 0 )
```

Please do remember `SEH_BUILD` is invented in here to simplify what we mean. SEH is always available and always in use by the Windows implementation.

**Windows no C++ exceptions build** -- is cl.exe C/C++ build without any `/EH` switch or with a famous `/kernel` switch. [SEH](https://docs.microsoft.com/en-us/cpp/cpp/structured-exception-handling-c-cpp?view=vs-2019) is intrinsic to Windows. SEH is in the foundations of Windows programming. RTTI can exist in that situation. 

### 1.0.1. Sampling repo and the current MS STL status

I have made [this little project](https://github.com/DBJDBJ/kernel_ms_stl) to approve or disapprove your doubts. Also on the official side, things are happening around the issue of MS STL and "SEH builds":

- https://github.com/microsoft/STL/issues/1289
- https://github.com/microsoft/STL/issues/639


My findings and thoughts are also in comment or snippets in the samples. What is perhaps interesting in here is to use this project, to try and check different parts of MS STL behaviour, in the SEH builds. And then follow through debugger, to where is that code taking you into the depths of the vast MS STL code base.

## 1.1. Usage

Of course this is strictly Windows code. And [this is **VS Code** project](https://github.com/DBJDBJ/kernel_ms_stl). If you are reading this, it is safe to assume you know how to use VS Code to do C++ builds. Thus you will know how to adjust the `.vscode` folder to your situation. There is also clean.cmd; hint: it clears the [cruft](https://en.wikipedia.org/wiki/Cruft#:~:text=Cruft%20is%20a%20jargon%20word,dysfunctional%20elements%20in%20computer%20software.) left after the builds.

You can put your own code in the `program.cpp`. Function `extern "C" int program (int argc , char ** argv )` is where the user code starts. It is called from a micro framework where the full SEH handling is implemented.

If SEH exception is raised that is caught inside `dbj_main`, and new "minidump" dmp file is created. You are informed where is it saved; what is the full path. Since SEH is intrinsic to Windows and CL.exe, that `dbj_main` always works and catches all potential SE's.

To open that file you need **Visual Studio**. After which [in the upper right corner](https://docs.microsoft.com/en-us/visualstudio/debugger/using-dump-files?view=vs-2019), you will spot the link to the native debugging entitled ["Debug With Native Only"](https://docs.microsoft.com/en-us/visualstudio/debugger/media/dbg_dump_summarypage.png?view=vs-2019). Click on that and soon you will be pushed to the point where the actual C++ or SEH exception was thrown from. Thus you need Visual Studio too.

![vstudio_minidump_dialogue](https://docs.microsoft.com/en-us/visualstudio/debugger/media/dbg_dump_summarypage.png?view=vs-2019)

Generating minidump is one very powerful feature. Almost all of[ my/our Windows apps are implementing it](https://github.com/DBJDBJ/kernel_ms_stl/blob/master/readme.md).

## 1.2. Thoughts and Issues

**Standard and SEH**

Standard C++ does not mention SEH, Windows or otherwise. It seems (at least to me) author of C++ (one Mr B. Stroustrup) has expressed explicit dislike for MSFT SEH, in this very recent paper: [P1947R0]( 
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1947r0.pdf). 

My opinion: There is a **lot** of customers who are "windows only" and a lot of projects serving them. Especially those needing low level Windows "server side" high performance components. In that situation one has to do as Windows demands.

But what that exactly means for you if you are in constant need of MS STL? So how is standard C++ standard library, implemented by Microsoft team, aka MS STL, C++ exception throwing designed and implemented? (circa 2020 OCT)

### 1.2.1. MS STL and SEH

From inside MS STL, as far as I can see, only eight exceptions are thrown. And they are thrown by calling eight `[[noreturn]]` functions. 

All 8 are in (above defined) [SEH builds](#sehbuild), compiled as SEH raising functions. AS evident in existing MS STL source available, circa 2020 Q4, are  implemented inside https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp. Good, but what is calling them?

#### 1.2.1.1. Down the Rabbit Hole

MS STL using or not using C++ exceptions, depends on the `_HAS_EXCEPTIONS` inbuilt macro.

From wherever in MS STL, exception is to be thrown, one of those eight functions is called. As an example if you do this

```cpp
std::vector<bool> bv_{ true, true, true } ;
// throws standard C++ exception, but
// transparently becomes SE (Structured Exception)
// in case /kernel is used or there is simply no /EH switch
auto never = bv_.at(22);
```
std vector `at()` is very simple
```cpp
// <vector> #2590
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
Where that `_Xout_of_range()` is declared inside `<xutility>`, together with friends
```cpp
// <xutility> #5817
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_alloc();
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char*);
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char*);
```
There is eight of them. Above six (and other two for: `bad_function_call` and `regex_error`) are defined (implemented) in the above mentioned https://github.com/microsoft/STL/blob/master/stl/src/xthrow.cpp . Available as part of MS STL open source. 

NOTE: that is as far as you can follow through debugger while using VS Code.

Back to our `_Xout_of_range` friend inside `xthrow.cpp`
```cpp
// xthrow.cpp # 24
[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char* const _Message) {
    _THROW(out_of_range(_Message));
}
```
And that _THROW is defined in relation to, do we have, or do we not have C++ exceptions compiled in the current build. Meaning: depending on what is the value of the `_HAS_EXCEPTIONS` macro. 

All is revealed in the following section of `<yvals.h>`
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
`_invoke_watson()`? Yes, all in time. Thus, if we look back into 
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

And, for some people, this is the point of contention where MS STL is leaving the realm of ISO C++ and entering the realm of Windows C++ and SEH. 

#### 1.2.1.2. Into the realm of Windows

(If irrelevant for this analysis, code is left out) Back to `<exception>`:

```cpp
#if _HAS_EXCEPTIONS

// <exception> # 32
#include <malloc.h>
#include <vcruntime_exception.h>
```
Thus one can understand in case of C++ exceptions in MS STL use, this `<vcruntime_exception.h>` gets involved. If curious please follow that path. That is where `std::exception` is actually implemented. Back to `<exception>` at hand:
```cpp
// <exception> # 63
#else // !  _HAS_EXCEPTIONS
```
That is the point where SEH world begins. For `_HAS_EXCEPTIONS == 0`, there is this global `_Raise_handler`
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
That is obviously called (from somewhere) to set the global raise handler, in the `_HAS_EXCEPTIONS == 0` scenario.

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
`bad_alloc`, `bad_array_new_length`, `bad_exception` also have different SEH versions for _HAS_EXCEPTIONS == 0 scenario. 

Back to investigation. That std exception `_Raise()` method uses two levels of indirection: `_Raise_handler` global function pointer and protected `_Doraise` method, before eventually calling the `_RAISE` macro. Remember all of of that is inside `[[noreturn]] void _Raise() const` method on the non standard MS STL version of `std::exception`.

And if we point back to our above `<yvalsh>` mentioning above, we shall understand, for `_HAS_EXCEPTIONS == 0` scenario,  `_RAISE(x)` macro is defined as:
```cpp
// <yvals.h> # 475
#ifdef _DEBUG
#define _RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define _RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG
```
Currently I am not 100% clear this is what is happening. I have not had enough time to make `_invoke_watson()` work. I think it is sage to assume that global pointer, used inside `_Raise()` method Does some work so that app does not just "disappear in thin air" like my feeble attempt currently does.

```cpp
    if (_STD _Raise_handler) {
        (*_STD _Raise_handler)(*this); // call raise handler if present
    }
```

#### 1.2.1.3. Use Watson instead? 

"Dr Watson" is coming [straight from Windows lore](https://devblogs.microsoft.com/oldnewthing/20051114-00/?p=33353). And possibility of using that in 2020 for MS STL is interesting, to put it mildly. For `_HAS_EXCEPTIONS == 0` scenario, MS STL on each SEH raised, (as it seems) actually calls [Dr Watson to do the job](https://docs.microsoft.com/en-us/windows-hardware/drivers/devtest/28725-use-watson-instead). Sherlock is nowhere to be seen. Reminder: `<yvals.h>` is part of MS STL open source.

Ok then is it really true good doctor is called? Well, if I do native debugging on the core dump I create, I can see the source of the SE raised is indeed in `xthrow.cpp`. Dutifully reported as "Microsoft C++ exception". But not much elase. This I do not know was it Dr Watson called or not. I assume it was, since looking to the source we can see it supposedly being called from that `_Raise()` method.

If really true, that is fine and OK as we have "come out on the other side" into the wonderful kingdom of Windows Drivers. Driver Technologies, Tools for Testing Drivers and namely this little known and peculiar old and cranky office of "Windows Error Reporting"; to the inner circle of Windows Elders, known under the acronym of [WER](https://docs.microsoft.com/en-us/windows/win32/wer/windows-error-reporting).

[WER](https://en.wikipedia.org/wiki/Windows_Error_Reporting) is Windows Legacy. WER basically is that place from where you can call back to daddy and complain. 

*"... enables users to notify Microsoft of application faults, kernel faults, unresponsive applications, and other application specific problems. Microsoft can use the error reporting feature to provide customers with troubleshooting information, solutions, or updates for their specific problems. Developers can use this infrastructure to receive information that can be used to improve their applications..."*

But do not fret. We do not do that. 

Windows as you know it today is actually Windows NT. And one of the foundation stones of Win NT are "Structured Exceptions" aka [SEH](https://en.wikipedia.org/wiki/Microsoft-specific_exception_handling_mechanisms#SEH).  If really called, `_invoke_watson` is a nop return function, it raises the SE aka "Structured Exception". Alternatively you can sober up and simply call

```cpp
// get around a good doctor
RaiseException( YOUR_SE_UID , 0, 0, {});
```
What my main does is simply catch all, any and every SE on the application top level. Hint: `main()` is the good place for you.

Ditto, in case of [SE caught](https://docs.microsoft.com/en-us/windows/win32/debug/using-an-exception-handler), what I do is create and save a minidump specific to my application. And looking into that minidump file with Visual Studio, I can pinpoint the issue that made the application misbehave. That includes every possible issue, not just C++ exceptions being thrown. 

And that is very powerful. 

I simply have this standard SE aware main in each and every of my WIN apps. That is **not** very complicated and has a lot of benefits. 

Please do understand SE is inbuilt in Windows and in the CL compiler and there are SE intrinsics too. Including the [keywords added](https://docs.microsoft.com/en-us/windows/win32/debug/abnormaltermination) to both C and C++. Thus that main works in every possible build. Not just our self-invented "seh builds".

### 1.2.2. SEH friendly, standard Windows C++, you can try 

Now you know how that mechanism and design works. You can try that too in your C++ Windows, SEH friendly code.

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
WARNING: I am not yet 100% sure that will work. I am not able to follow that path while inside MS STL with a debugger.

But in any case and always. To enjoy the SEH benefits, your Windows main should always be "SEH enabled", and look something like this:
```cpp
// The "Standard" main()
// this works in all kinds of Windows builds
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
That will always work c++ exception or no C++ exceptions. In case you want C++ exceptions you can not mix that in the same function, so you just call some entry point function, into your standard C++ app from the `main()` above.

Ditto. If you build with `/EHsc` your app will be: c++ exceptions **and** SEH enabled. SEH intrinsics are **always** enbaled, and are in `<excpt.h>`

> In Windows C/C++, SEH is always there

 ### 1.2.3. COM, C++ and /kernel builds

[MSFT COM](https://en.wikipedia.org/wiki/Component_Object_Model) predates C++ standardizations. Just like SEH does. Since 1993 (gasp!) working on Windows code, one will meet COM, sooner or latter. And only those who like pain, useth COM natively through its C API. 

Thus some few  years after, ["Compiler COM Support"](https://docs.microsoft.com/en-us/cpp/cpp/compiler-com-support?redirectedfrom=MSDN&view=vs-2019) was designed, implemented and inbuilt into a CL compiler. And, that was designed to use C++ exceptions. Not SEH. Which is strange.

In 2020 Q4, if and when attempting using `<comdef.h>` types, in what we have defined above as "SEH builds", C++ exceptions are replaced with SEH. Almost transparently.

You need to know right now, [SEH things are happening in there "by accident"](https://github.com/MicrosoftDocs/cpp-docs/issues/2494#issuecomment-701200395). 

Pleas do not rely on `<comdef.h>` "SEH builds" combination until further notice. "Compiler COM Support" and SEH is accidental combination that just happens to work, 2020 Q4.

My opinion? MSFT ["Compiler COM Support"](https://docs.microsoft.com/en-us/cpp/cpp/compiler-com-support?redirectedfrom=MSDN&view=vs-2019) I do like, it is C++, it is simple and it "just works". But thesem halcion days of ISO C++, somebody has decided \<comdef.h> and friends will firmly stay inside c++ exceptions territory. Which is weird since the whole the MS STL can switch easily to SEH builds from C++ exceptions.

Easily but not transparently, since in the [SEH build](#sehbuild), keywords `try`,`throw` and `catch` do not compile.

## 1.3. Conclusion(s)

Many people are, but I am not that perturbed with MS STL apparent duality and ability to transparently switch of C++ exceptions easily.

Neither many projects are usurped too. Why not? Let me repeat the facts. Most of the customers are on Windows. WIN32 is primary Windows API. WIN32 is C. Same as UTF16 (`wchar_t`) is standard on windows, not `char`, or this '\\' is path delimiter and not this '/', in tha same manner, SEH is standard on Windows, not C++ exceptions.

For Windows code, SEH is the norm. Embrace it.
