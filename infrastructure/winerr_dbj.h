#pragma once
#ifndef  __cplusplus
#error Only available in C++ compiler
#endif

/* 

taken from:

/***
* comdef.h - Native C++ compiler COM support - main definitions header
*
* Copyright (c) Microsoft Corporation. All rights reserved.
*
****

(c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj 

_com_error from comdef.h transposed into non-com windows errors utility
it might be thrown indeed but same as original it does not inherit from
std::exception.

NOTE: this is free of std lib but not free of comdef.h
*/

#include <comdef.h>
#include <wchar.h>
#include <yvals.h> // _RAISE and _THROW
#include "dbj_version_helpers.h"

#ifdef _COMDEF_NOT_WINAPI_FAMILY_DESKTOP_APP
#pragma message("_COMDEF_NOT_WINAPI_FAMILY_DESKTOP_APP defined")
#else
#pragma message("_COMDEF_NOT_WINAPI_FAMILY_DESKTOP_APP NOT defined")
#endif

#ifdef _NATIVE_WCHAR_T_DEFINED
#pragma message("_NATIVE_WCHAR_T_DEFINED defined")
#else
#pragma message("_NATIVE_WCHAR_T_DEFINED NOT defined")
#endif

#if _HAS_EXCEPTIONS == 0 
// coppied form yval.h
#define DBJ_THROW(x) x._Raise()
#ifdef _DEBUG
#define DBJ_RAISE(x) _invoke_watson(_CRT_WIDE(#x), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else // _DEBUG
#define DBJ_RAISE(x) _invoke_watson(nullptr, nullptr, nullptr, 0, 0)
#endif // _DEBUG
#else // _HAS_EXCEPTIONS != 0
#define DBJ_THROW(x) throw x
#endif // _HAS_EXCEPTIONS != 0


namespace dbj {

inline wchar_t* DuplicateString(const wchar_t* message )
    {
        wchar_t* value = nullptr;
        if (message != nullptr)
        {
            auto length = ::wcslen(message) + 1; // add 1 for null terminator
            value = (wchar_t *)LocalAlloc(0, length * sizeof(wchar_t) + 1);
            if (value == nullptr)
            {
                // dbj: need to think of a replacement
                _com_issue_error(E_OUTOFMEMORY);
            }
            ::wmemcpy_s(value, length, message, length);
        }
        return value;
    }

class _win_error final 
{
explicit _win_error( ) noexcept = delete ;

public:

explicit _win_error(HRESULT hr_) noexcept
    : hresult_(hr_),
        message_(nullptr)
{
}

 _win_error(HRESULT hr, const wchar_t* message) 
 : hresult_ (hr), message_ (nullptr)
{
        message_ = DuplicateString(message);
}

 _win_error(const _win_error& that) noexcept
    : hresult_(that.hresult_), message_(NULL)
{

}

 ~_win_error() noexcept
{
    if (message_ != NULL) {
        LocalFree((HLOCAL)message_);
    }
}

 _win_error& operator=(const _win_error& that) noexcept
{
    if (this != &that) {
        this->~_win_error();
        this->_win_error::_win_error(that);
    }
    return *this;
}

    _win_error(_win_error&& other) noexcept
      :  hresult_ ( other.hresult_ ) ,
        message_ ( other.message_ )
    {
        other.message_ = nullptr;
    }

 _win_error & operator = (_win_error&& other) noexcept
    {
        hresult_ = other.hresult_  ;
    if (message_ != NULL) 
        LocalFree((HLOCAL)message_);
        message_ = other.message_ ;
        other.message_ = nullptr;
    }

 HRESULT Error() const noexcept
{
    return hresult_;
}

 WORD WCode() const noexcept
{
    return HRESULTToWCode(hresult_);
}

// dbj: must not free the result
 const TCHAR * ErrorMessage() const noexcept
{
    if (message_ == NULL) {
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
                          FORMAT_MESSAGE_FROM_SYSTEM|
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      hresult_,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&message_,
                      0,
                      NULL);
        if (message_ != NULL) {
            #ifdef UNICODE
            size_t const nLen = wcslen(message_);
            #else
            size_t const nLen = strlen(message_);
            #endif
            if (nLen > 1 && message_[nLen - 1] == '\n') {
                message_[nLen - 1] = 0;
                if (message_[nLen - 2] == '\r') {
                        message_[nLen - 2] = 0;
                }
            }
        }
        else {
            message_ = (LPTSTR)LocalAlloc(0, 32 * sizeof(TCHAR));
            if (message_ != NULL) {
                WORD wCode = WCode();
                if (wCode != 0) {
                    _COM_PRINTF_S_1(message_, 32, TEXT("IDispatch error #%d"), (int)wCode);
                }
                else {
                    _COM_PRINTF_S_1(message_, 32, TEXT("Unknown error 0x%0lX"), hresult_);
                }
            }
        }
    }
    return message_;
}

 static HRESULT WCodeToHRESULT(WORD wCode) noexcept
{
    return wCode >= 0xFE00 ? WCODE_HRESULT_LAST : WCODE_HRESULT_FIRST + wCode;
}

 static WORD HRESULTToWCode(HRESULT hr) noexcept
{
    return (hr >= WCODE_HRESULT_FIRST && hr <= WCODE_HRESULT_LAST)
        ? WORD(hr - WCODE_HRESULT_FIRST)
        : 0;
}

// alignment with std::exception
const wchar_t * what () const noexcept{
     return ErrorMessage() ;
}

// same design as MS STL std::exception 
// raises the SE
__declspec(noreturn) void _Raise() {

// NOTE: 
// basetsd.h
// typedef _W64 unsigned long ULONG_PTR, *PULONG_PTR;
// thus ULONG_PTR is not a pointer
static ULONG_PTR DBJ_SE_UID = 0xff ; 
const ULONG_PTR * DBJ_SE_UID_PP = & DBJ_SE_UID ;

        RaiseException( E_FAIL , EXCEPTION_NONCONTINUABLE, 1, { DBJ_SE_UID_PP }) ;
}

private:
    enum {
        WCODE_HRESULT_FIRST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x200),
        WCODE_HRESULT_LAST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF+1, 0) - 1
    };
    mutable HRESULT         hresult_{};
    mutable /*TCHAR*/ wchar_t *         message_{};
}; // _win_error

// 
inline void __declspec(noreturn) __stdcall _win_raise_error(HRESULT hr, const wchar_t * message = nullptr )
{
    /*
    if _HAS_EXCEPTIONS == 0 _THROW(x) compiles to x._Raise()
    */
    if ( message ) 
    DBJ_THROW( _win_error(hr, message ) ) ;
    else
    DBJ_THROW( _win_error(hr) ) ;
}

 inline bool is_win10 () noexcept
{
    if (!IsWindows10OrGreater())
       DBJ_THROW( _win_error( E_NOTIMPL, L"You need at least Windows 10. This version not supported") ) ;
     return true ;
}

} // ns dbj