#ifndef COMPILER_VERSIONS
#define COMPILER_VERSIONS

/////////////////////////////////////////////////////////////////////////////
// LICENSE NOTICE
// --------------
// Copyright (c) Hexadigm Systems
//
// Permission to use this software is granted under the following license:
// https://www.hexadigm.com/GenericLib/License.html
//
// This copyright notice must be included in this and all copies of the
// software as described in the above license.
//
// DESCRIPTION
// -----------
// Contains miscellaneous #defined constants to identify the target version
// of C++ in effect, and other miscellaneous compiler-related declarations.
// Should typically be #included first in all other files so they're
// immediately available if you need to test these #defined constants for
// anything in those files (so while #including it first isn't mandatory,
// it's usually a good idea so they're immediately ready for use)
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// The following #ifs are used to define one and only one of the following
// constants in order to indicate which compiler is currently in use. Only
// one of the following will therefore ever be #defined (they are all
// mutually exclusive):
//
//   GCC_COMPILER
//   MICROSOFT_COMPILER
//   CLANG_COMPILER
//   INTEL_COMPILER
//
// Note that while we rely on the following predefined (compiler-specific)
// constants to determine the above (respectively) ...
//
//   __GNUC__
//   _MSC_VER
//   __clang__
//   __INTEL_LLVM_COMPILER
//
// .. we can't always rely on these predefined constants in certain
// compiler-specific code since some of these constants are #defined by
// other other compilers as well. This makes it impossible to tell which
// compiler is actually running based on checks of these predefined
// constants alone. For instance, both the Clang compiler and Intel
// compilers #define __clang__ (since the Intel compiler is based on Clang),
// so you can't rely on __clang__ to determine if the real Clang compiler is
// actually running and not the Intel compiler, assuming you need to know
// this (depending on what you're checking __clang__ for). Similarly, both
// Clang and Intel also #define _MSC_VER when running in Microsoft VC++
// compatibility mode (i.e., when compiling code for Windows), so if
// _MSC_VER is #defined then you don't know if it's actually the native
// (Microsoft) VC++ compiler running or possibly Clang or Intel running in
// Microsoft VC++ compatibility mode (such as when you install Clang and/or
// Intel on Windows and then select one of them to compile your app,
// overriding the native Microsoft VC++ compiler - they can also be run
// from other OSs to explicitly compile Windows apps).
//
// The following #ifs therefore #define the first group of constants above
// for use when you need to know the real compiler that's actually in use.
// The second group of constants above (the native predefined constants for
// each compiler), can still be applied however whenever the actual, real
// compiler doesn't matter (for whatever your purpose is). For instance, you
// would check _MSC_VER instead of MICROSOFT_COMPILER when the actual
// Microsoft VC++ compiler doesn't matter, i.e., all that matters is that a
// compiler that's compatible with VC++ is running, such as Clang or Intel
// when either is running in Microsoft VC++ compatibility mode (so for most
// intents and purposes if _MSC_VER is #defined then you can proceed as if
// the real VC++ compiler is running - if another like Clang or Intel is
// actually running then because they've #defined _MSC_VER it means they're
// running in Microsoft compatibility VC++ mode so their behaviour should be
// the same as VC++ anyway - caution advised however to make sure the
// behaviour is in fact the same depending on what you're doing, since it's
// not always 100% compatible). If you need to know if the real VC++
// compiler is running however then you would simply check
// MICROSOFT_COMPILER instead.
//
// IMPORTANT:
// ---------
// The order we check compilers in the following code is relevant so don't
// change it if you're not sure what you're doing. For instance, we check
// the Intel compiler first since it normally #defines __clang__ as well
// (since the Intel DPC++/C++ compiler is based on Clang), but the Clang
// compiler never #defines __INTEL_LLVM_COMPILER so Intel needs to be
// checked first (otherwise if __clang__ is #defined and you check it first
// then you can't tell if it's the real Clang or Intel that #defined it).
/////////////////////////////////////////////////////////////////////////////
#if defined(__INTEL_LLVM_COMPILER) // See https://www.intel.com/content/www/us/en/docs/dpcpp-cpp-compiler/developer-guide-reference/2023-2/use-predefined-macros-to-specify-intel-compilers.html
    #define INTEL_COMPILER __INTEL_LLVM_COMPILER
#elif defined(__clang__) // See https://clang.llvm.org/docs/LanguageExtensions.html#builtin-macros
    #define CLANG_COMPILER __clang__
#elif defined(__GNUC__) // See https://gcc.gnu.org/onlinedocs/cpp/macros/predefined-macros.html#c.__GNUC__
                        //     https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html#Common-Predefined-Macros
                        //     https://stackoverflow.com/a/55926503
    #define GCC_COMPILER __GNUC__
#elif defined(_MSC_VER) // See https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
                        //     https://devblogs.microsoft.com/cppblog/visual-c-compiler-version/
                        // Note: __INTEL_COMPILER identifies the classic Intel compiler which
                        //       is now deprecated by Intel (so we longer support it)
    #define MICROSOFT_COMPILER _MSC_VER
#else
    #error "Unsupported compiler (GCC, Microsoft, Clang and Intel DPC++/C++ are the only ones supported at this writing)"
#endif

//////////////////////////////////////////////////////////////
// CPPXX_OR_LATER and CPPXX (#defined) constants that can be
// tested to determine which version of C++ is in effect
// (C++14, C++17, C++20, etc.). Alternatively, you can also
// use the CPPXX_OR_EARLIER constants #defined just after the
// following instead of CPPXX_OR_LATER. The CPPXX_OR_EARLIER
// constants are simply negations of the CPPXX_OR_LATER
// constants so choose whichever is more appropriate for your
// needs (they can be mixed and matched). In either case,
// note that we always assume C++11 or greater so no specific
// constant exists to target earlier versions (C++98 the only
// official earlier version). Lastly, note that a new
// constant should be added for each new release of C++.
//
// Example usage:
//
//     #if CPP17_OR_LATER
//          // Not available until C++17
//          #include <string_view>
//     #endif
//////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// MSFT? (_MSC_VER #defined, but also includes any
// other compiler we support running in Microsoft VC++
// compatibility mode). Note that _MSVC_LANG is only
// defined however beginning in Visual Studio 2015
// Update 3 (see predefined macros in MSDN). Note that
// if _MSVC_LANG is #defined then it's guaranteed to be
// 201402L or greater (i.e., C++14 or greater). Again,
// see this in MSDN (it's the same value as the
// __cplusplus macro)
////////////////////////////////////////////////////////
#if defined(_MSC_VER) && defined(_MSVC_LANG)
	#define CPP_VERSION _MSVC_LANG
/////////////////////////////////////////////////////////
// In MSFT (opposed to other compilers like GCC, Clang,
// etc., where __cplusplus is always assumed to be
// accurately #defined), __cplusplus is only accurately
// #defined starting in VS 2017 15.7 Preview 3, but only
// if the /Zc:__cplusplus switch is set. See:
//
//     MSVC now correctly reports __cplusplus
//     https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
//
// It's always 199711L otherwise (erroneously indicating
// C++98). Since we already tested for the presence of
// _MSVC_LANG just above however, which is #defined
// starting in Visual Studio 2015 Update 3 (as noted
// above), then if we drop into the following #elif it
// means we must be targeting an earlier version of VC++
// (prior to Visual Studio 2015 Update 3), so __cplusplus
// is guaranteed to be 199711L at this point, as just
// described (again, erroneously indicating C++98).
// That's fine for our purposes however, since all the
// CPPXX_OR_LATER constants #defined below will be set
// to zero in this case, including our earliest constant
// CPP14_OR_LATER (so not even C++14 will be considered
// in effect by anyone testing that constant). Only C++11
// should therefore be considered in effect by default
// (implicitly), not any later versions (since all our
// constants below will test zero), and not C++98 itself
// (even though __cplusplus is 199711L which targets it
// - we always implicitly assume C++11 or greater since
// all modern MSFT compilers now support it - we don't
// support any earlier versions which are ancient now)
/////////////////////////////////////////////////////////
#elif defined(__cplusplus)
    #define CPP_VERSION __cplusplus
#else
    #define CPP_VERSION 0L // #defined constants that follow will now target C++11 (we don't support anything earlier)
#endif

//////////////////////////////////////////////////////////
// Next version of C++ after 2023? This version is still
// unknown at this writing but the version # we're
// testing here (202400L) will realistically work for our
// purposes (to see if we're targeting the next version
// after 2023). Presumably it will be 202400L or greater,
// and C++23, which hasn't been released yet at this
// writing, will presumably be "2023XX" for some "XX".
// We'll properly update this however once the version
// that succceeds C++23 is officially released (so
// although the version we're testing here is still
// potentially brittle, would be surprised if it didn't
// work)
//////////////////////////////////////////////////////////
#if CPP_VERSION >= 202400L
    //#define CPPXX 1 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23 0
    #define CPP20 0
    #define CPP17 0
    #define CPP14 0
    #define CPP11 0
    // #define CPPXX_OR_LATER 1 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23_OR_LATER 1
    #define CPP20_OR_LATER 1
    #define CPP17_OR_LATER 1
    #define CPP14_OR_LATER 1
#elif CPP_VERSION > 202002L // C++23 (testing for > C++20 here so if true then we must be dealing with C++23 at this point)
    //#define CPPXX 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23 1
    #define CPP20 0
    #define CPP17 0
    #define CPP14 0
    #define CPP11 0
    // #define CPPXX_OR_LATER 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23_OR_LATER 1
    #define CPP20_OR_LATER 1
    #define CPP17_OR_LATER 1
    #define CPP14_OR_LATER 1
#elif CPP_VERSION > 201703L // C++20 (testing for > C++17 here so if true then we must be dealing with C++20 at this point)
    //#define CPPXX 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23 0
    #define CPP20 1
    #define CPP17 0
    #define CPP14 0
    #define CPP11 0
    // #define CPPXX_OR_LATER 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23_OR_LATER 0
    #define CPP20_OR_LATER 1
    #define CPP17_OR_LATER 1
    #define CPP14_OR_LATER 1
#elif CPP_VERSION > 201402L // C++17 (testing for > C++14 here so if true then we must be dealing with C++17 at this point)
    //#define CPPXX 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23 0
    #define CPP20 0
    #define CPP17 1
    #define CPP14 0
    #define CPP11 0
    // #define CPPXX_OR_LATER 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23_OR_LATER 0
    #define CPP20_OR_LATER 0
    #define CPP17_OR_LATER 1
    #define CPP14_OR_LATER 1
#elif CPP_VERSION == 201402L // C++14 (testing for this exact version)
    //#define CPPXX 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23 0
    #define CPP20 0
    #define CPP17 0
    #define CPP14 1
    #define CPP11 0
    // #define CPPXX_OR_LATER 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23_OR_LATER 0
    #define CPP20_OR_LATER 0
    #define CPP17_OR_LATER 0
    #define CPP14_OR_LATER 1
#else // C++11 (only other possibility so we implicitly assume C++11 - only (official) earlier version is C++98 which we don't support)
    //#define CPPXX 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23 0
    #define CPP20 0
    #define CPP17 0
    #define CPP14 0
    #define CPP11 1
    // #define CPPXX_OR_LATER 0 // TBD (update "XX" to handle the next version after C++23)
    #define CPP23_OR_LATER 0
    #define CPP20_OR_LATER 0
    #define CPP17_OR_LATER 0
    #define CPP14_OR_LATER 0
#endif

#undef CPP_VERSION // Done with this

//////////////////////////////////////////////////////////////////
// CPPXX_OR_EARLIER (#defined) constants that can be tested
// instead of the CPPXX_OR_LATER #defined constants above. You
// can use either the CPPXX_OR_LATER and/or CPPXX_OR_EARLIER
// constants based on your needs (to determine which version of
// C++ is in effect, C++14, C++17, C++20, etc.). We always assume
// C++11 or greater however so no specific constant exists to
// target earlier versions (C++98 the only official earlier
// version). Note that a new CPPXX_OR_EARLIER constant should be
// added for each new release of C++
//
// Example usage:
//
//     #if CPP17_OR_EARLIER
//          // Do whatever for C++17 or earlier
//     #else
//          // Do whatever for C++20 or greater
//     #endif
//////////////////////////////////////////////////////////////////
//#define CPP23_OR_EARLIER (!CPPXX_OR_LATER) // TBD (update "XX" to handle the next version after C++23)
#define CPP20_OR_EARLIER (!CPP23_OR_LATER)
#define CPP17_OR_EARLIER (!CPP20_OR_LATER)
#define CPP14_OR_EARLIER (!CPP17_OR_LATER)

// <string_view> not available until C++17
#if CPP17_OR_LATER
    #include <string_view>
#endif

///////////////////////////////////////////////////////
// MSFT? (or any other compiler we support running in
// Microsoft VC++ compatibility mode - "Clang" and
// "Intel" are the only ones we currently support that
// are affected)
///////////////////////////////////////////////////////
#if defined(_MSC_VER)
    //////////////////////////////////////////////////////////////
    // Mainly to pick up TCHAR and _T macros (for our purposes
    // here). It's expected that <tchar.h> will be found in the
    // #include search path which is normally the case when
    // compiling for Windows. For non-Microsoft compilers however
    // we manually declare TCHAR and _T in the #else clause below
    // (unless a particular non-Microsoft compiler is running in
    // Microsoft VC++ compatibility mode in which case they'll
    // also #define _MSC_VER so will come through here instead,
    // no different than the real VC++ - Clang and Intel compilers
    // are the only ones we currently support that are affected).
    //////////////////////////////////////////////////////////////
    #include <tchar.h>

    //////////////////////////////////////////////////////////////////
    // Macros to extract the major and minor version numbers from the
    // predefined Microsoft constant _MSC_VER (pass the latter
    // constant to macros MSC_GET_VER_MAJOR or MSC_GET_VER_MINOR as
    // required), or the build number from the predefined Microsoft
    // constant _MSC_FULL_VER (pass the latter constant to macro
    // MSC_GET_VER_BUILD). For instance, if you're currently running
    // Visual C++ 2019 and _MSC_VER is 1920 and _MSC_FULL_VER is
    // 192027508, then the following will display "19.20.27508" (but
    // you normally shouldn't pass _MSC_VER and _MSC_FULL_VER directly
    // as seen in this example, since the constants described just
    // below are easier - read on):
    //
    //     // Displays "19.20.27508"
    //     tcout << MSC_GET_VER_MAJOR(_MSC_VER) << "." <<
    //              MSC_GET_VER_MINOR(_MSC_VER) << "." <<
    //              MSC_GET_VER_BUILD(_MSC_FULL_VER);
    //
    // Note that you need not call these macros directly however (as
    // seen in the above example) when you wish to pass _MSC_VER
    // itself (to extract the major or minor version number), or
    // _MSC_FULL_VER (to extract the build number). You can simply
    // rely on the constants MSC_VER_MAJOR, MSC_VER_MINOR or
    // MSC_VER_BUILD (respectively) instead, which are #defined just
    // below. They in turn call these "GET" macros for you, passing
    // _MSC_VER or _MSC_FULL_VER as required. The above example can
    // therefore be more cleanly written like so (yielding the same
    // result as the above but less verbose):
    //
    //     // Displays "19.20.27508"
    //     tcout << MSC_VER_MAJOR << "." <<
    //              MSC_VER_MINOR << "." <<
    //              MSC_VER_BUILD;
    //
    // You therefore only need to call the following "GET" macros
    // directly when you need to explicitly pass arguments other than
    // the predefined Microsoft constants _MSC_VER and _MSC_FULL_VER.
    // For instance, if you wish to extract the major, minor and
    // build numbers for, say, the first release version of Visual
    // Studio 2022 (version 19.30.30705), then you can pass the
    // following Visual Studio 2022 constants (#defined later on)
    // to these "GET" macros like so:
    //
    //     // Displays "19.30.30705"
    //     tcout << MSC_GET_VER_MAJOR(MSC_VER_2022) << "." <<
    //              MSC_GET_VER_MINOR(MSC_VER_2022) << "." <<
    //              MSC_GET_VER_BUILD(MSC_FULL_VER_2022);
    //
    // Again, you normally only need to call these "GET" macros
    // directly when passing args other than the predefined Microsoft
    // constants _MSC_VER and _MSC_FULL_VER (since in the latter case
    // you can just rely on the constants MSC_VER_MAJOR, MSC_VER_MINOR
    // and MSC_VER_BUILD instead, as described above).
    //////////////////////////////////////////////////////////////////
    #define MSC_GET_VER_MAJOR(MSC_VER) (MSC_VER / 100)
    #define MSC_GET_VER_MINOR(MSC_VER) (MSC_VER % 100)
    #define MSC_GET_VER_BUILD(MSC_FULL_VER) (MSC_FULL_VER % 100000)

    ///////////////////////////////////////////////////////////////////
    // #defined constants storing the compiler's major, minor, build
    // and revision numbers (for Microsoft C++). E.g., using MSFT's
    // own example for _MSC_VER, _MSC_FULL_VER and MSC_VER_BUILD found
    // at the following link (though I've changed the build number in
    // their example from "00" to "03" to reduce possible confusion -
    // "00" not an ideal choice for an example):
    //
    //    Predefined macros
    //    https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170&redirectedfrom=MSDN
    //
    // Then given the above (example) where the version number is
    // 15.03.20706.01, the predefined Microsoft constants would be:
    //
    //    _MSC_VER = 1503 (major and minor number)
    //    _MSC_FULL_VER = 150320706 (major, minor and build number)
    //    _MSC_BUILD  = 1 (revision number)
    //
    // The major, minor, build and revision numbers are then
    // extracted from the above to populate these constants.
    //
    //     Example (using the above values)
    //     --------------------------------
    //     // Displays "15.3.20706.1"
    //     tcout << MSC_VER_MAJOR << "." << // 15
    //              MSC_VER_MINOR << "." << // 3
    //              MSC_VER_BUILD << "." << // 20706
    //              MSC_VER_REVISION;       // 1
    ///////////////////////////////////////////////////////////////////
    #define MSC_VER_MAJOR MSC_GET_VER_MAJOR(_MSC_VER)
    #define MSC_VER_MINOR MSC_GET_VER_MINOR(_MSC_VER)
    #define MSC_VER_BUILD MSC_GET_VER_BUILD(_MSC_FULL_VER)
    #define MSC_VER_REVISION _MSC_BUILD

    /////////////////////////////////////////////////////////////
    // #defined constants corresponding to the predefined MSFT
    // constants _MSC_VER and _MSC_FULL_VER. One such constant
    // exists for each modern version of Visual Studio. Note
    // while MSFT does publish the values for _MSC_VER here at
    // this writing:
    //
    //     Predefined macros
    //     https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
    //
    // Unfortunately they don't publish each _MSC_FULL_VER at
    // this writing (after extensive searching - go figure), so
    // the constants below were picked up from the following
    // (unofficial) links instead (not sure where they got it
    // from but hopefully accurate):
    //
    //     Microsoft Visual C++
    //     https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B#Internal_version_numbering
    //
    //     Pre-defined Compiler Macros (Note: no longer being updated!)
    //     https://sourceforge.net/p/predef/wiki/Compilers/
    //
    // Also see the following for details on how to use the macros
    // _MSC_VER and _MSC_FULL_VER in general (they can be compared
    // against the #defined constants below):
    //
    //     Visual C++ Compiler Version (By Gabriel Dos Reis of MSFT)
    //     https://devblogs.microsoft.com/cppblog/visual-c-compiler-version/
    /////////////////////////////////////////////////////////////////

    // Visual Studio 2005
    #define MSC_VER_2005                  1400
    #define MSC_FULL_VER_2005             140050727

    // Visual Studio 2008
    #define MSC_VER_2008                  1500
    #define MSC_FULL_VER_2008             150021022
    #define MSC_FULL_VER_2008_SP1         150030729

    // Visual Studio 2010
    #define MSC_VER_2010                  1600
    #define MSC_FULL_VER_2010             160030319
    #define MSC_FULL_VER_2010_SP1         160040219

    // Visual Studio 2012
    #define MSC_VER_2012                  1700
    #define MSC_FULL_VER_2012             170050727

    // Visual Studio 2013
    #define MSC_VER_2013                  1800
    #define MSC_FULL_VER_2013             180021005

    // Visual Studio 2015
    #define MSC_VER_2015                  1900
    #define MSC_FULL_VER_2015             190023026
    #define MSC_FULL_VER_2015_UPDATE_1    190023506
    #define MSC_FULL_VER_2015_UPDATE_2    190023918
    #define MSC_FULL_VER_2015_UPDATE_3    190024210

    // Visual Studio 2017
    #define MSC_VER_2017                  1910
    #define MSC_FULL_VER_2017             191025017
    #define MSC_VER_2017_V15_3            1911
    #define MSC_VER_2017_V15_5            1912
    #define MSC_VER_2017_V15_6            1913
    #define MSC_VER_2017_V15_7            1914
    #define MSC_VER_2017_V15_8            1915
    #define MSC_VER_2017_V15_9            1916

    // Visual Studio 2019
    #define MSC_VER_2019                  1920
    #define MSC_FULL_VER_2019             192027508
    #define MSC_VER_2019_V16_1            1921
    #define MSC_VER_2019_V16_2            1922
    #define MSC_VER_2019_V16_3            1923
    #define MSC_VER_2019_V16_4            1924
    #define MSC_VER_2019_V16_5            1925
    #define MSC_VER_2019_V16_6            1926
    #define MSC_VER_2019_V16_7            1927
    #define MSC_VER_2019_V16_8_AND_9      1928
    #define MSC_FULL_VER_2019_16_9        192829500 // Search for this value here: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
    #define MSC_VER_2019_V16_10_AND_11    1929
    #define MSC_FULL_VER_2019_16_11       192930100 // Search for this value here: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170

    // Visual Studio 2022
    #define MSC_VER_2022                  1930
    #define MSC_FULL_VER_2022             193030705
    #define MSC_VER_2022_V17_1            1931
    #define MSC_VER_2022_V17_2            1932
    #define MSC_VER_2022_V17_3            1933
    #define MSC_VER_2022_V17_4            1934
    #define MSC_VER_2022_V17_5            1935
    #define MSC_VER_2022_V17_6            1936

    ///////////////////////////////////////////////////////////////
    // Note: Callers still need to explicitly #include <iostream>
    // in their own files if using "tcout" however (to pick up
    // "std::wout" or (very rarely) "std::cout" - we don't
    // #include <iostream> in this file that is)
    ///////////////////////////////////////////////////////////////
    #if defined(_UNICODE) || defined(UNICODE)
        #define tcout std::wcout
    #else
        #define tcout std::cout
    #endif
#else
    ///////////////////////////////////////////////////////
    // Again, ancient technique from MSFT we'll adopt for
    // non-MSFT compilers. All strings and other character-
    // based types we need will be based on TCHAR from here
    // on (char). Only MSFT compilers will use "wchar_t"
    // instead normally (for Unicode builds).
    //
    // IMPORTANT: Note that you should not change this
    // type to anything else on non-MSFT platforms. It
    // must remain "char" in this release. If it were
    // changed to something else like "wchar_t" we'd have
    // to deal with "char <==> wchar_t" conversions at
    // times which this release is not currently designed
    // to do. The use of TCHAR for non-MSFT platforms
    // simply allows us to more easily deal with character
    // types in a consistent way (by always using TCHAR
    // for any character types we process), whether we're
    // targetting MSFT or not. On MSFT platforms it will
    // normally be "wchar_t" as noted above, but "char" is
    // also supported if someone compiles their program
    // that way (though it's highly unlikely on Windows
    // anymore - Windows apps are normally compiled for
    // UTF-16 where TCHAR is "wchar_t" but "char" is also
    // supported if compiled for ANSI). On non-MSFT
    // platforms however only "char" is supported for now.
    ///////////////////////////////////////////////////////
    using TCHAR = char; // IMPORTANT - Don't change this. See comments above.

    ///////////////////////////////////////////////////////
    // Native (very ancient and ugly) MSFT specific macro
    // we'll adopt for other compilers as well (so we
    // #define it here for GCC/Clang/Intel and any others
    // we may support in the future). Used on MSFT
    // platforms to append an "L" to all string literals
    // (and less frequently character constants) so that,
    // say, "Your string" becomes L"Your string" instead
    // (when compiling for UTF-16 on Windows, as is
    // normally the case there - all string literals are
    // therefore wchar_t-based on MSFT platforms). We
    // #define it here to do nothing however meaning
    // everything will remain "char" based on non-MSFT
    // platforms (see TCHAR just above). Only when
    // compiling for MSFT will we rely on their own (real)
    // _T macro from <tchar.h> (#included earlier when
    // targeting MSFT), which appends an "L" as described
    // for Unicode builds. TCHAR-based string literals
    // will therefore normally be "wchar_t" in most MSFT
    // apps (since almost all MSFT apps in the real world
    // are compiled that way now - again, "wchar_t" is
    // used to store UTF-16 on all modern MSFT platforms)
    ///////////////////////////////////////////////////////
    #define _T(x) x

    ///////////////////////////////////////////////////////
    // Always char-based in this release. See TCHAR alias
    // above. Callers still need to explicitly #include
    // <iostream> in their own files however if using
    // "tcout" (to pick up "std::cout" since we don't
    // #include <iostream> in this file)
    ///////////////////////////////////////////////////////
    #define tcout std::cout
#endif

//////////////////////////////////////////////////////////////////
// CONCEPTS_SUPPORTED. #defined constant to control whether
// concepts should be used or not wherever required in all
// remaining code. Concepts only became available in C++20 so by
// default we turn it on in C++20 or later (set the constant to 1
// below), and off in C++17 and earlier (set it to 0 below). In
// C++17 and earlier we therefore rely on "static_assert" instead
// of concepts but if you prefer to always rely on
// "static_assert" even in C++20 and later you can just
// permanently set the constant to 0 below. This may even be
// preferable since I often find "static_assert" messages more
// visible and clear than concept messages (in some situations
// anyway). In any case it's your responsibility to explicitly
// test for CONCEPTS_SUPPORTED as required and apply concepts if
// true or "static_assert" otherwise. One way to do this we
// consistently rely on is as per the following example (though
// you need not follow this model but it works well IMHO). Let's
// say you want a concept to ensure some type is "const" using
// "std::is_const". You could therefore declare a concept like so
// (note that I've applied the suffix "_c" to the concept name as
// a convention to indicate it's a concept but it's up to you if
// you want to follow this convention):
//
//     #if CONCEPTS_SUPPORTED
//         template <typename T>
//         concept IsConst_c = std::is_const_v<T>;
//
//         #define IS_CONST_C StdExt::IsConst_c // Assuming the "StdExt" namespace for this example
//     #else
//         #define STATIC_ASSERT_IS_CONST(T) static_assert(std::is_const_v<T>, \
//                                                         "\"" #T "\" must be const")
//         #define IS_CONST_C typename
//     #endif
//
// Then in any code that requires this concept you can simply do
// this:
//
//     template<IS_CONST_C T>
//     struct Whatever
//     {
//         #if !CONCEPTS_SUPPORTED
//             ///////////////////////////////////////////////
//             // Kicks in if concepts are NOT supported,
//             // otherwise IS_CONST_C concept kicks in just
//             // above instead (latter simply resolves to
//             // the "typename" keyword when concepts aren't
//             // supported)
//             ///////////////////////////////////////////////
//             STATIC_ASSERT_IS_CONST(T);
//         #endif
//
//         // ...
//     }
//
// Template arg "T" in the above struct therefore utilizes the
// IS_CONST_C concept if concepts are supported but if not, then
// IS_CONST_C simply resolves to the "typename" keyword instead
// and the STATIC_ASSERT_IS_CONST macro then kicks in to do a
// "static_assert". You can therefore apply the above technique
// to all your concepts and we do throughout all remaining code
// where applicable (though in some cases we don't #define a
// macro like STATIC_ASSERT_IS_CONST if the concept is for
// internal use only and we don't require such a macro depending
// on the situation).
//////////////////////////////////////////////////////////////////
#if CPP20_OR_LATER
    #define CONCEPTS_SUPPORTED 1
#else
    #define CONCEPTS_SUPPORTED 0
#endif

namespace StdExt
{
    // "basic_string_view" not available until C++17
    #if CPP17_OR_LATER
        ///////////////////////////////////////////////////////
        // All uses of "std::basic_string_view" we need will
        // rely on this from here on (less verbose than using
        // "basic_string_view<TCHAR>" directly everywhere). It
        // therefore always resolves to "std::string_view" on
        // non-MSFT platforms (since TCHAR must *always* be
        // "char" in this release - see its declaration further
        // above), and normally (usually) "std::wstring" on
        // MSFT platforms (since TCHAR normally resolves to
        // "wchar_t" on MSFT platforms - again, see this further
        // above).
        //
        // Note that in theory we should add a similar "using"
        // statement for other things like "std::basic_string"
        // as well (naming it "tstring"), among other classes,
        // but for now the following is the only type we ever
        // use (but we can add more using statements later on
        // an as-needed basis).
        //
        // Lastly, we could convert all functions, etc. that
        // deal with TCHARs into templates instead, with a
        // template arg specifying the character type, but
        // we don't do that for now since the code isn't
        // designed to handle character conversions (too much
        // overhead for now but we can always revisit things
        // if ever required).
        ///////////////////////////////////////////////////////
        using tstring_view = std::basic_string_view<TCHAR>;
    #endif // #if CPP17_OR_LATER

    ///////////////////////////////////////////////////////
    // GetCompilerName(). WYSIWYG.
    ///////////////////////////////////////////////////////
    inline constexpr const TCHAR * GetCompilerName() noexcept
    {
        #if defined(GCC_COMPILER)
            return _T("GCC or compatible");
        #elif defined(MICROSOFT_COMPILER)
            return _T("Microsoft Visual C++");
        #elif defined(CLANG_COMPILER)
            return _T("Clang");
        #elif defined(INTEL_COMPILER)
            return _T("Intel oneAPI DPC++/C++");
        #else
            /////////////////////////////////////////////////////
            // Note: Shouldn't be possible to come through here
            // at this writing since the same #error message
            // would have already been displayed earlier (when
            // the compiler constants just above were #defined)
            /////////////////////////////////////////////////////
            #error "Unsupported compiler (GCC, Microsoft, Clang and Intel are the only ones supported at this writing)"
        #endif
    }
}

#endif // #ifndef COMPILER_VERSIONS (#include guard)