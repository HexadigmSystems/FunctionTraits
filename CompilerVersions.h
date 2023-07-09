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
// Should normally be #included first in all other files (if you need to
// test these #defined constants for anything in those files - even if not,
// it's a good idea to #include it anyway for future use)
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// GCC? (the real one - see https://stackoverflow.com/a/55926503)
// 
// Also see the following:
//     GCC macros:   https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html#Common-Predefined-Macros
//                   https://gcc.gnu.org/onlinedocs/cpp/macros/predefined-macros.html#c.__GNUC__
//     Clang macros: https://clang.llvm.org/docs/LanguageExtensions.html#builtin-macros
//     Intel macros: https://www.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/macros/additional-predefined-macros.html
//     MSFT macros:  https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
///////////////////////////////////////////////////////////////////
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
    ///////////////////////////////////////////////////////////////
    // See GCC links above. If we come through here then we're
    // either dealing with the real GCC or another 3rd-party
    // compiler that we don't officially support but which claims
    // to be GNU compatible (and therefore #defines __GNUC__ so
    // we'll allow it but not officially support it - read on).
    // Note that there's no easy way to identify the real GCC
    // because of this situation (i.e., other compilers also
    // #defining __GNUC__). Even Clang and Intel, the only other
    // compilers we officially support at this writing (that are
    // GNU compatible), will also #define __GNUC__ in a supported
    // environment unless it's turned off in those compilers
    // (don't know off-hand if any options are available to turn
    // it off, or the precise circumstances in which __GNUC__ may
    // not be #defined, if any, but no matter). We therefore
    // #define the following (GCC) which we can therefore test
    // for whenever required to see if we're targeting the real
    // GCC but any GNU-compatible compiler will also do (though
    // we only officially support the real GCC - we allow others
    // that are compatible but don't officially support them if
    // something goes wrong). To this end, "compatible" compilers
    // *don't* include the ones we officially do support (that
    // also #define __GNUC__), meaning Clang and Intel only at
    // this writing (the only other official compilers we
    // currently support that might also #define __GNUC__ - note
    // that we support MSFT as well but it doesn't #define
    // __GNUC__). The upshot is that if GCC is #defined, it means
    // that we're dealing with either the real GCC or some other
    // 3rd-party compiler that claims to be compatible with it,
    // but not any of the other official compilers we support
    // that are also compatible with it. GCC won't be #defined
    // for them that is, so we still need to identify them using
    // their own official #defined constants (__clang__ and
    // __INTEL_COMPILER only at this writing).
    ///////////////////////////////////////////////////////////////
    #define GCC __GNUC__
#endif

// Microsoft
#ifdef _MSC_VER
    /////////////////////////////////////////////////////////////
    // MSFT compiler? Note that Intel also #defines _MSC_VER on
    // Windows, as well as _MSC_FULL_VER and _MSC_EXTENSIONS.
    // See the following:
    //
    // https://github.com/cpredef/predef/blob/master/Compilers.md#user-content-microsoft-visual-c
    // https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-8/additional-predefined-macros.html
    /////////////////////////////////////////////////////////////
    #include <tchar.h> // To pick up TCHAR and _T macro

    /////////////////////////////////////////////////////////////
    // #defined constants corresponding to the predefined MSFT
    // constants _MSC_VER and _MSC_FULL_VER. One such constant
    // exists for each modern version of Visual Studio.
    // Unfortunately MSFT doesn't publish each _MSC_FULL_VER at
    // this writing (after extensive searching - go figure), so
    // the constants below were picked up from the following
    // (unofficial) links at this writing (presumably accurate):
    //
    //   Pre-defined Compiler Macros
    //   https://sourceforge.net/p/predef/wiki/Compilers/
    // 
    //   Microsoft Visual C++
    //   https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B#Internal_version_numbering    //
    //
    // Also see the following for details on how to use the macros
    // _MSC_VER and _MSC_FULL_VER in general (they can be compared
    // against the #defined constants below):
    //
    //   Visual C++ Compiler Version (By Gabriel Dos Reis at MSFT)
    //   https://blogs.msdn.microsoft.com/vcblog/2016/10/05/visual-c-compiler-version/
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
    #define MSC_VER_2017_V15_1            1910 // No change
    #define MSC_VER_2017_V15_2            1910 // No change
    #define MSC_VER_2017_V15_3            1911
    #define MSC_VER_2017_V15_4            1911 // No change
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
    #define MSC_VER_2019_V16_8            1928
    #define MSC_VER_2019_V16_9            1928 // No change
    #define MSC_VER_2019_V16_11           1929

    // Visual Studio 2022
    #define MSC_VER_2022                  1930
    #define MSC_FULL_VER_2022             193030705
    #define MSC_VER_2022_V17_2            1932
    #define MSC_VER_2022_V17_3            1933

    // Note: Callers still need to #include <iostream>
    #if defined(_UNICODE) || defined(UNICODE)
        #define tcout std::wcout
    #else
        #define tcout std::cout
    #endif
#else
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
    // therefore wchart_t-based on MSFT platforms). We
    // #define it here to do nothing however (just below),
    // meaning everything will remain "char" based on
    // non-MSFT platforms (see TCHAR just below). Only
    // when compiling for MSFT will we rely on their own
    // (real) _T macro from <tchar.h> (#included earlier
    // when targeting MSFT), which appends an "L" as
    // described for Unicode builds. TCHAR-based string
    // literals will therefore normally be "wchar_t" in
    // most MSFT apps (since almost all MSFT apps in the
    // real world are compiled that way now - again,
    // "wchar_t" is used to store UTF-16 on all modern
    // MSFT platforms)
    ///////////////////////////////////////////////////////
    #define _T(x) x

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

    ////////////////////////////////////////////////////
    // Always char-based in this release. See comments
    // just above. Note that callers also still need
    // to #include <iostream>
    ////////////////////////////////////////////////////
    #define tcout std::cout
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
// MSFT? Note that _MSVC_LANG is only available however
// beginning in Visual Studio 2015 Update 3 (see
// predefined macros in MSDN). Note that if _MSVC_LANG
// is #defined then it's guaranteed to be 201402L or
// greater (i.e., C++14 or greater). Again, see this in
// MSDN (it's the same value as the __cplusplus macro)
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
        // "char" in this release - see its declaration just
        // above), and normally (usually) "std::wstring" on
        // MSFT platforms (since TCHAR normally resolves to
        // "wchar_t" on MSFT platforms - again, see this just
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

        ///////////////////////////////////////////////////////
        // GetCompilerName(). WYSIWYG.
        ///////////////////////////////////////////////////////
        inline constexpr std::basic_string_view<TCHAR> GetCompilerName() noexcept
        {
            /////////////////////////////////////////////////
            // Note that Intel can #define _MSC_VER as well.
            // See here at this writing (GCC and Clang don't
            // #define it, though a GCC-compatible compiler
            // might but we only officially support the real
            // GCC for now so not gonna wory about it):
            //
            //     https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-8/additional-predefined-macros.html
            /////////////////////////////////////////////////
            #if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
                return _T("Microsoft");
            #elif defined(__clang__)
                return _T("Clang");
            #elif defined(GCC)
                return _T("GCC or compatible");
            #elif defined(__INTEL_COMPILER)
                return _T("Intel");
            #else
                static_assert(false, "Unknown compiler in use. Only Clang, GCC, Intel and Microsoft are currently supported");

                ///////////////////////////////////////////////
                // Return value req'd to shut compiler up
                // (though "static_assert" just above shuts
                // compilation down anyway - following still
                // requried though to prevent an error message 
                // about a missing return value)
                ///////////////////////////////////////////////
                return _T("");
            #endif
        }
    #endif // #if CPP17_OR_LATER
}

#endif // COMPILER_VERSIONS