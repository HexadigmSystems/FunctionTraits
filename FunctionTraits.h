#ifndef FUNCTION_TRAITS
#define FUNCTION_TRAITS

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
// Header file implementing the "FunctionTraits" template library (fully)
// documented here https://github.com/HexadigmSystems/FunctionTraits. More
// generally however, the header contains miscellaneous (generic) templates
// similar to those in the native C++ header <type_traits> among others
// (though the library relies on the Pascal naming convention instead of
// "snake case" normally used by the C++ standard itself). Therefore, while
// the header exists to implement the "FunctionTraits" library, and most
// users will be #including it for that purpose only, not all templates in
// the file are directly associated with "FunctionTraits" per se, and
// therefore aren't documented at the library's link above. The header's
// main focus is "FunctionTraits" so all other "public" declarations exist
// merely to support it (more on what "public" means shortly). Nevertheless,
// since these support templates are independent of "FunctionTraits" itself
// and can therefore be used for other purposes in their own right (the
// library contains various useful templates beyond what the standard C++
// library offers), it would be better (cleaner) if the library were
// designed with them separated into their own header, rather than directly
// declaring them in this one (since declaring them in their own header
// means they could be independently #included wherever needed). Doing so
// however means users of the "FunctionTraits" library would have another
// header to add to their source code in addition to "FunctionTraits.h" and
// "CompilerVersions.h" (the only two headers users need to add currently).
// For now then these support templates are declared in this header simply
// to minimize the number of headers users need to add to their code (just
// the latter two ".h" files only). Users can still use these support
// templates for their own independent purposes however, if they wish to
// inspect the code itself (again, since they are not documented at the
// above link).
//
// Please note that all declarations in this file are declared in namespace
// "StdExt". Everything is available for public use except declarations in
// (nested) namespace "StdExt::Private", which are reserved for internal use
// only (implementation details). Macros however don't respect namespaces of
// course but all macros #defined for *internal* use only are documented as
// such throughout this header (and usually #undefined later in the file
// anyway). All others are available for public use if required (and
// therefore aren't #undefined), meaning any that aren't explicitly
// documented for internal use (all such internal macros indicate this).
// Note that macros are a necessary "evil" in C++ sometimes but are kept to
// a minimum (only used where legitimately required).
//
// Finally, note that this header is supported by GCC, Microsoft, Clang and
// Intel compilers only at this writing (C++17 and later - the check for
// CPP17_OR_LATER just below causes all code to be preprocessed out
// otherwise). A separate check below also ensures the minimum supported
// versions of these compilers are running or a #error terminates
// compilation. Note that this file depends on (#includes)
// "CompilerVersions.h" as well. All other dependencies are native C++
// headers with the exception of the native Windows header "tchar.h" on MSFT
// platforms (or any other supported compiler running in Microsoft VC++
// compatibility mode). "tchar.h" must be in the #include search path and
// normally will be when targeting MSFT (in most MSFT C++ project
// environments).
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Our header containing mostly #defined C++ version constants
// indicating which version of C++ is in effect (which we can
// test for below as required - see "C++ Version Macros" in the
// following file). A few other compiler-related declarations
// also exist however. Note that we #include this first so we
// can immediately start using these version constants (in
// particular CPP17_OR_LATER seen just below - we don't support
// earlier versions)
////////////////////////////////////////////////////////////////
#include "CompilerVersions.h"

//////////////////////////////////////////////////////////////
// This header supports C++17 and later only. All code below
// ignored otherwise (preprocessed out).
//////////////////////////////////////////////////////////////
#if CPP17_OR_LATER

//////////////////////////////////////////////////////////////
// GCC compiler running? (the real one or any one compatible
// with it but *not* any compiler we explicitly support which
// includes Clang and Intel only at this writing - we
// independently test for them below but other GCC compatible
// compilers that we don't explicitly support will be handled
// by the following call instead, since they claim to be GCC
// compatible) ...
//////////////////////////////////////////////////////////////
#if defined(GCC_COMPILER)
    ///////////////////////////////////////////////////
    // We only support GCC >= 10.2. Abort compilation
    // otherwise.
    ///////////////////////////////////////////////////
    #if __GNUC__ < 10 || (__GNUC__ == 10 && __GNUC_MINOR__ < 2)
        #error "Unsupported version of GCC detected. Must be >= V10.2 (this header doesn't support earlier versions)"

        /////////////////////////////////////////////////////////////
        // #including this non-existent file forces compilation to
        // immediately stop. Otherwise more errors will occur before
        // compilation finally stops. Just a hack and not really
        // guaranteed to work but testing shows it normally does
        // (for GCC and all other compilers we support at this
        // writing). Other alternative techniques such as #pragmas
        // to stop compilation on the first error aren't guaranteed
        // to be reliable either. They (generally) don't affect the
        // preprocessor for instance so compilation doesn't stop
        // when the #error statement above is encountered. The
        // #error message is displayed but compilation resumes,
        // resulting in additional (extraneous) error messages
        // before compilation finally stops. The upshot is that we'll
        // just rely on the following for now to prevent these
        // additional errors from being displayed after the #error
        // message above (since the extra error messages are just
        // a distraction from the actual #error message above).
        /////////////////////////////////////////////////////////////
        #include <StopCompilingNow> // Hack to force compilation to immediately stop.
                                    // See comments above for details.
    #endif
///////////////////////////////////////////////////////////////////
// Microsoft compiler (VC++) running? (the real VC++, not another
// compiler running in Microsoft VC++ compatibility mode)
///////////////////////////////////////////////////////////////////
#elif defined(MICROSOFT_COMPILER)
    ///////////////////////////////////////////////////////////
    // Are we now running VC++ from a VS2017 release (opposed
    // to VS2019 or later). Can't be from VS2015 or earlier at
    // this point since CPP17_OR_LATER is currently #defined
    // (we checked for it earlier), so we must be targeting
    // VS2017 or later (since C++17 only became available in
    // VS2017). If the following is true then the version of
    // VC++ now running must therefore be from VS2017 (no
    // earlier version of VS possible at this point).
    ///////////////////////////////////////////////////////////
    #if _MSC_VER < MSC_VER_2019
        //////////////////////////////////////////////////////////
        // We only support VC++ >= 19.16 which was released with
        // Visual Studio 2017 version 15.9. Abort compilation
        // otherwise.
        //////////////////////////////////////////////////////////
        #if _MSC_VER >= MSC_VER_2017_V15_9 // VC++ 19.16
            ////////////////////////////////////////////////////////
            // For internal use only (we #undef it later). Compiler
            // is a VS2017 release of MSVC.
            ////////////////////////////////////////////////////////
            #define MSVC_FROM_VISUAL_STUDIO_2017
        #else
            ////////////////////////////////////////////////////
            // Note that VC++ normally stops compiling as soon
            // as this is encountered, so we don't rely on the
            // #include <StopCompilingNow> hack used for the
            // other compilers (above and below). See:
            //
            //     #error directive (C/C++)
            //     https://learn.microsoft.com/en-us/cpp/preprocessor/hash-error-directive-c-cpp?view=msvc-170
            ////////////////////////////////////////////////////
            #error "Unsupported version of Microsoft Visual C++ detected. Must be >= V19.16 (normally released with VS2017 V15.9). This header doesn't support earlier versions."
        #endif
    #endif
// Clang compiler running? (possibly in Microsoft VC++ compatibility mode - ok)
#elif defined(CLANG_COMPILER)
    /////////////////////////////////////////////////////
    // We only support Clang >= 11.0. Abort compilation
    // otherwise.
    /////////////////////////////////////////////////////
    #if __clang_major__ < 11
        #error "Unsupported version of Clang detected. Must be >= V11.0 (this header doesn't support earlier versions)"
        #include <StopCompilingNow> // Hack to force compilation to immediately stop.
                                    // See comments for GCC compiler above for details.
    #endif
// Intel compiler (DPC++/C++) running? (possibly in Microsoft VC++ compatibility mode - ok)
#elif defined(INTEL_COMPILER)
    ////////////////////////////////////////////////////////
    // We only support Intel >= 2021.4.0. Abort compilation
    // otherwise.
    ////////////////////////////////////////////////////////
    #if __INTEL_LLVM_COMPILER < 20210400
        #error "Unsupported version of Intel oneAPI DPC++/C++ detected. Must be >= V2021.4.0 (this header doesn't support earlier versions)"
        #include <StopCompilingNow> // Hack to force compilation to immediately stop.
                                    // See comments for GCC compiler above for details.
    #endif
#else
    #error "Unsupported compiler (GCC, Microsoft, Clang and Intel are the only ones supported at this writing)"
#endif

///////////////////////////////////////////////////
// Sanity check only. Should always be false (for
// internal use only - #defined below if required)
///////////////////////////////////////////////////
#if defined(DECLARE_PUBLIC_MACROS_ONLY)
    #error "DECLARE_PUBLIC_MACROS_ONLY already #defined (for internal use only so never should be)"
#endif

/////////////////////////////////////////////////////////
// When #defined the following activates the C++
// "deprecated" attribute on anything that's been
// deprecated, usually due to name changes and/or design
// improvements. In this case deprecated features are
// still supported but a warning from the "deprecated"
// attribute will notify the user accordingly (with a
// message to upgrade to the new way of doing things -
// note that on MSFT platforms however our deprecated
// messages are ignored by MSVC for some reason). When
// not #defined however then deprecated features will no
// longer be declared so will result in compiler errors
// (so upgrading to the new way of doing things then
// becomes mandatory - the last commit message at
// https://github.com/HexadigmAdmin/FunctionTraits will
// explain what needs to be done).
/////////////////////////////////////////////////////////
#define STDEXT_SUPPORT_DEPRECATED

/////////////////////////////////////////////////////////////////////////////
// If all 3 conditions we're testing here are true then this header is now
// being #included by a client in the module version of the header (the
// module named "FunctionTraits" in "FunctionTraits.cppm" or whatever the
// user may have renamed the latter file's extension to). In this case we
// know modules (a C++20 feature) are supported (the 1st condition tests
// CPP20_OR_LATER for this but will be replaced with the official C++
// feature constant "__cpp_modules" in a later release since it's not yet
// supported by all compilers), and the 2nd condition then checks if
// STDEXT_USE_MODULES was #defined by the user themself, indicating they've
// added the ".cppm" files to their project (in this case the
// "FunctionTraits.cppm" module) and wish to use it. In that case we'll go
// on to declare only #defined (public) macros in this header instead of all
// other C++ declarations that are also normally declared (when
// STDEXT_USE_MODULES isn't #defined), since those C++ declarations will now
// originate from the module itself (which we import via "import
// FunctionTraits" in the code just below, though the user might also do
// this themself). Therefore, when a user #includes this header in the
// module version, only the macros in the header will need to be declared
// since C++ modules don't export #defined macros (we #define an internal
// constant DECLARE_PUBLIC_MACROS_ONLY below to facilitate this). If the
// user hasn't #defined STDEXT_USE_MODULES however then they wish to use
// this header normally (declare everything as usual), even if they've also
// added the module to their project as well (which will still be
// successfully compiled), though that would be very unlikely (why add the
// module if they're not going to use it? - if they've added it then they'll
// normally #define STDEXT_USE_MODULES as well). As for the 3rd condition
// we're testing here, STDEXT_BUILDING_MODULE_FUNCTION_TRAITS (an internal
// constant), this is #defined by the module itself (FunctionTraits.cppm)
// before #including this header in its global fragment section. The module
// then simply exports all public declarations from this header in its
// purview section via "using" declarations.
// STDEXT_BUILDING_MODULE_FUNCTION_TRAITS is therefore #defined only if the
// module itself is now being built. If #defined then it must be the module
// itself now #including us and if not #defined then it must be a client of
// the module #including us instead (which is what the following tests for
// in its 3rd condition - if all 3 conditions are true then it must be a
// user #including us, not the module itself, so we only declare the
// #defined macros in this header instead of all other declarations, as
// described above).
/////////////////////////////////////////////////////////////////////////////
#if CPP20_OR_LATER && defined(STDEXT_USE_MODULES) && !defined(STDEXT_BUILDING_MODULE_FUNCTION_TRAITS) // See comments about CPP20_OR_LATER above (to be
                                                                                                      // replaced by "__cpp_modules" in a future release)
    // "import std" not currently in effect? (C++23 or later)
    #if !defined(STDEXT_IMPORTED_STD)
        ///////////////////////////////////////////////////////////
        // Always pick up <type_traits> even when clients are
        // #including us in the module version of "FunctionTraits"
        // (i.e., when "FunctionTraits.cppm" has been added to the
        // project). Most clients that #include "FunctionTraits.h"
        // would expect <type_traits> to also be picked up so in
        // the module version we still #include it here as a
        // convenience to clients only. Note that when
        // STDEXT_IMPORTED_STD is #defined however (we just tested
        // it above and it's not), then "import std" is currently
        // in effect so <type_traits> is already available.
        ///////////////////////////////////////////////////////////
        #include <type_traits>
    #endif

    //////////////////////////////////////////////////////
    // Importing the "FunctionTraits" module as a
    // convenience to module clients. This way clients
    // can simply #include this header without having to
    // explicitly import the "FunctionTraits" module
    // themselves (since this header does it for them).
    // It's harmless however if they've already imported
    // the "FunctionTraits" module on their own (which is
    // more natural anyway - relying on this header may
    // even confuse some users who might not realize that
    // a #include "FunctionTraits.h" statement is actually
    // importing the "FunctionTraits" module to pick up
    // all public declarations in the header instead of
    // declaring them in the header itself - this is how
    // the header behaves when STDEXT_USE_MODULES is
    // #defined). #including this header however will also
    // pick up all public macros in the header since
    // modules don't export macros (so if clients simply
    // import the "FunctionTraits" module and don't
    // #include this header, they'll have access to all
    // exported declarations in the module which originate
    // from this header, but none of the macros in the
    // header - fine if they don't use any of them
    // though).
    //////////////////////////////////////////////////////
    import FunctionTraits;

    //////////////////////////////////////////////////////
    // All declarations in this module are now available
    // via the import statement just above. Only macros
    // will be missing since modules don't export them
    // (so the above import statement doesn't include
    // them). The following will therefore ensure that
    // only (public) macros will be #defined in the
    // remaining code that follows. All other
    // declarations are preprocessed out because they're
    // already available via the import statement just
    // above.
    //////////////////////////////////////////////////////
    #define DECLARE_PUBLIC_MACROS_ONLY
//////////////////////////////////////////////////////////////
// We're now coming through either because the module version
// of this header isn't installed ("FunctionTraits.cppm" or
// whatever the user may have renamed the extension to), so
// the header is handled normally (we declare everything
// below as usual), or it is installed but the latter file
// (module) is now #including us during its own build (it
// always does so the header is also treated normally when
// that occurs - again, everything is declared below as
// usual).
//////////////////////////////////////////////////////////////
#else

// "import std" not currently in effect? (C++23 or later)
#if !defined(STDEXT_IMPORTED_STD)
    // Standard C/C++ headers
    #include <algorithm>
    #include <array>
    #include <cstddef>
    #include <functional>
    #include <iostream>
    #include <string_view>
    #include <tuple>
    #include <type_traits>
    #include <utility>
#endif

#endif // #if CPP20_OR_LATER && defined(STDEXT_USE_MODULES) && !defined(STDEXT_BUILDING_MODULE_FUNCTION_TRAITS) // See comments about CPP20_OR_LATER above (to be

    /////////////////////////////////////////////////////////////
    // STDEXT_UNBRACKET. Macro which unbrackets a *bracketed*
    // arg. Normally intended to be called without explicit
    // brackets. Instead, it should be passed a single
    // *bracketed* arg so the effect is to remove those brackets,
    // (i.e., the brackets in the arg itself become the macro
    // call brackets when STDEXT_UNBRACKET is invoked so they're
    // effectively removed from the arg - see following
    // example):
    //
    // E.g.
    // ---
    // ///////////////////////////////////////////////////
    // // In this example arg "X" in macro WHATEVER_1 is
    // // passed the following (brackets included) which
    // // is forwarded all the way up to WHATEVER_3:
    // //
    // //   (SomeAlias_t<T, U>)
    // //
    // // WHATEVER_3 now invokes the following as seen
    // // below:
    // //
    // //   STDEXT_UNBRACKET X
    // //
    // // No explicit brackets are included after
    // // STDEXT_UNBRACKET because X itself already contains
    // // them so the above call resolves to the following
    // // (so the comma in the arg is now interpreted as an
    // // arg separator in the call to STDEXT_UNBRACKET so
    // // 2 args are being passed to it):
    // //
    // //   STDEXT_UNBRACKET (SomeAlias_t<T, U>)
    // //
    // // STDEXT_UNBRACKET just yields __VA_ARGS__ as
    // // seen below so it resolves to the following
    // // (i.e., the original brackets in the arg passed
    // // to WHATEVER_1 are now gone - they were eaten
    // // in the call just above - and the comma remains
    // // intact - the original bracketed arg passed to
    // // WHATEVER_1 therefore results but with the
    // // brackets simply removed):
    // //
    // //   SomeAlias_t<T, U>
    // ///////////////////////////////////////////////////
    // #define WHATEVER_3(X) STDEXT_UNBRACKET X
    //
    // #define WHATEVER_2(X) WHATEVER3(X) // Forward to macro above
    // #define WHATEVER_1(X) WHATEVER2(X) // Forward to macro above
    //
    // //////////////////////////////////////////////////
    // // Arg being passed is bracketed so it includes
    // // the (inner) brackets and the comma as well.
    // // The comma is not an arg separator because of
    // // the inner brackets but becomes part of the arg
    // // itself (just another token in the arg)
    // //////////////////////////////////////////////////
    // WHATEVER_1((SomeAlias_t<T, U>))
    /////////////////////////////////////////////////////////////
    #define STDEXT_UNBRACKET(...) __VA_ARGS__

    #define CONCATENATE(x, y) x ## y

    /////////////////////////////////////////////////////////////
    // Macro to stringify whatever args you pass but exercise
    // caution. If you pass another macro directly to this one
    // then the macro isn't expanded first due to the way C++
    // macros work. The raw macro name itself (and any args
    // if it's a function-based macro) will be stringify
    // instead, not the result of expanding the macro args. This
    // is because when C++ macros are called, all args of the
    // outer macro (the one being called), are literally passed
    // to it without being expanded first (in the case of args
    // that are macros themselves). The raw text comprising
    // those macro-based args are just passed as-is, no different
    // than for non-macro args. Only when they're substituted
    // into the outer macro (however the macro does that) are
    // they then resolved in a WYSIWYG manner (all at once as
    // would naturally be expected.
    //
    //   E.g.
    //   ----
    //   // Results in "TEST" (WYSIWYG)
    //   STDEXT_STRINGIFY(TEST)
    //
    //   ///////////////////////////////////////////////////////
    //   // The following call however results in:
    //   //
    //   // "MY_MACRO(Whatever)"
    //   //
    //   // Not (as many might expect):
    //   //
    //   // "My Macro: Whatever"
    //   //
    //   // That's because MY_MACRO(WHATEVER) isn't called
    //   // first. See comments above. Instead, STDEXT_STRINGIFY
    //   // is passed the raw arg it's being passed (not the
    //   // the result of invoking that arg even though it's
    //   // also a macro) so STDEXT_STRINGIFY invokes:
    //   //
    //   //   #MY_MACRO(Whatever)
    //   //
    //   // Which quotes this raw name as seen above. The
    //   // following call however:
    //   //
    //   // #define SOME_OTHER_MACRO(X) Some other macro: X
    //   // SOME_OTHER_MACRO(STDEXT_STRINGIFY(MY_MACRO(Whatever)))
    //   //
    //   // First resolves to:
    //   //
    //   // Some other macro: STDEXT_STRINGIFY(MY_MACRO(Whatever))
    //   //
    //   // And then all macros above are invoked as expected
    //   // (WYSIWYG). Those calls first resolve to this:
    //   //
    //   // Some other macro: STDEXT_STRINGIFY(My Macro: Whatever)
    //   //
    //   // And then finally this (as expected):
    //   //
    //   // Some other macro: "My Macro: Whatever"
    //   ///////////////////////////////////////////////////////
    //   #define MY_MACRO(X) My Macro: X
    //   STDEXT_STRINGIFY(MY_MACRO(Whatever))
    /////////////////////////////////////////////////////////////
    #define STDEXT_STRINGIFY(...) #__VA_ARGS__

    ////////////////////////////////////////////////////////////
    // Stringifies whatever args you pass and then invokes
    // macro _T on the resulting value, Note that you can pass
    // args with embedded commas which are correctly handled
    // (see example below) but embedded brackets aren't
    // supported due to limitations in C++ macros - techniques
    // relying on STDEXT_UNBRACKET above can be written to
    // handle them but we don't bother for now):
    //
    // E.g.
    //
    //     STDEXT_TSTRINGIFY(Test, ok)
    //
    // Yields (quotes included):
    //
    //     _T("Test, ok")
    //
    // _T then resolves as usual, always to "Test, ok" on
    // non-Microsoft platforms, and on Microsoft platforms to
    // L"Test, ok" if UNICODE or _UNICODE is #defined (usually
    // the case), or "Test, ok" otherwise (very rare these
    // days).
    ////////////////////////////////////////////////////////////
    #define STDEXT_TSTRINGIFY(...) _T(STDEXT_STRINGIFY(__VA_ARGS__))

    ////////////////////////////////////////////////////////////
    // Stringifies whatever arg(s) you pass and quotes the
    // resulting value (i.e., adds leading and trailing double
    // quotes). Note that you can pass args with embedded
    // commas which are correctly handled (see example below)
    // but embedded brackets aren't supported due to
    // limitations in C++ macros - techniques relying on
    // STDEXT_UNBRACKET above can be written to handle them but
    // we don't bother for now):
    //
    // E.g.
    //
    //     QUOTE_STRINGIFY(Testing, ok)
    //
    // Yields (quotes included):
    //
    //     "Testing, ok"
    ////////////////////////////////////////////////////////////
    #define QUOTE_STRINGIFY(...) "\"" STDEXT_STRINGIFY(__VA_ARGS__) "\""

    ////////////////////////////////////////////////////////////
    // Stringifies both arg TEMPLATE which is expected to be a
    // template name, and __VA_ARGS__ which should always be a
    // comma delimited list of that template's args (a single
    // arg with no comma or even an empty arg is also
    // acceptable), and then surrounds the stringified template
    // args (__VA_ARGS__) with angled brackets before appending
    // them to the stringified TEMPLATE arg, surrounding the
    // final (resulting) string with double quotes:
    //
    // E.g.
    //
    //     QUOTE_STRINGIFY_TEMPLATE_ARGS(std::tuple_element_t, I, TupleT)
    //
    // Yields (quotes included):
    //
    //     "std::tuple_element_t<I, TupleT>"
    ////////////////////////////////////////////////////////////
    #define QUOTE_STRINGIFY_TEMPLATE_ARGS(TEMPLATE, ...) "\"" STDEXT_STRINGIFY(TEMPLATE) "<" STDEXT_STRINGIFY(__VA_ARGS__) ">\""

    #if defined(USE_CONCEPTS)
        #define STATIC_ASSERT_V(STATIC_ASSERT_FUNCS, BRACKETED_VALUE) STDEXT_UNBRACKET BRACKETED_VALUE
        #define STATIC_ASSERT_T(STATIC_ASSERT_FUNCS, BRACKETED_TYPE) STDEXT_UNBRACKET BRACKETED_TYPE
    #else
        #define STATIC_ASSERT_V(STATIC_ASSERT_FUNCS, BRACKETED_VALUE) (STATIC_ASSERT_FUNCS, STDEXT_UNBRACKET BRACKETED_VALUE)
        #define STATIC_ASSERT_T(STATIC_ASSERT_FUNCS, BRACKETED_TYPE) std::enable_if_t<(STATIC_ASSERT_FUNCS, true), STDEXT_UNBRACKET BRACKETED_TYPE>
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)

#if defined(STDEXT_SUPPORT_DEPRECATED)
    #define ENTITY_DEPRECATED_DUE_TO_NAME_CHANGE(ENTITY, NEW_NAME) [[deprecated(STDEXT_STRINGIFY(ENTITY) " was renamed to " \
                                                                                QUOTE_STRINGIFY(NEW_NAME) ". The old name continues to " \
                                                                                "be supported however (it just defers to the new name), " \
                                                                                "but may be removed in a future release (users should " \
                                                                                "update their code to invoke the new name)") \
                                                                     ]]

    #define TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(NEW_TEMPLATE_NAME) ENTITY_DEPRECATED_DUE_TO_NAME_CHANGE(Template, NEW_TEMPLATE_NAME)
    #define VARIABLE_DEPRECATED_DUE_TO_NAME_CHANGE(NEW_VARIABLE_NAME) ENTITY_DEPRECATED_DUE_TO_NAME_CHANGE(Variable, NEW_VARIABLE_NAME)
    #define ENUM_DEPRECATED_DUE_TO_NAME_CHANGE(NEW_ENUM_NAME) ENTITY_DEPRECATED_DUE_TO_NAME_CHANGE(Enum, NEW_ENUM_NAME)

#endif

// Everything below in this namespace
namespace StdExt
{
    /////////////////////////////////////////////////////
    // AlwaysFalse_v. Used in templates only normally
    // where you need to always pass "false" for some
    // purpose but in a way that's dependent on a
    // template arg (instead of passing false directly).
    // In almost all real-world cases however it will be
    // used as the 1st arg to "static_assert", where
    // the 1st arg must always be false. Can't use
    // "false" directly or it will always trigger the
    // "static_assert" (even if that code is never
    // called or instantiated). Making it a (template)
    // dependent name instead eliminates the problem
    // (use the following IOW instead of "false"
    // directly). See:
    //
    //    // always_false<T>
    //    https://artificial-mind.net/blog/2020/10/03/always-false
    //
    //    How can I create a type-dependent expression that is always false?
    //    https://devblogs.microsoft.com/oldnewthing/20200311-00/?p=103553
    /////////////////////////////////////////////////////
    template <typename...>
    inline constexpr bool AlwaysFalse_v = false;

    ////////////////////////////////////////////////////////
    // AlwaysTrue_v. Similar to "AlwaysFalse_v" just above
    // but used wherever you require "true" in a way that's
    // dependent on a template arg. Very rarely used in
    // practice however (not many uses for it), unlike
    // "AlwaysFalse_v" just above which is more frequently
    // used to trigger a "static_assert" where required.
    // See "AlwaysFalse_v" above.
    ////////////////////////////////////////////////////////
    template <typename...>
    inline constexpr bool AlwaysTrue_v = true;

    /////////////////////////////////////////////////////////////////////
    // Private namespace (for internal use only) used to implement
    // "TypeName_v" declared just after this namespace. Allows you to
    // retrieve the type name for any type as a compile-time string
    // (std::basic_string_view). See "TypeName_v" for full details (it
    // just defers to "TypeNameImpl::Get()" in the following namespace).
    /////////////////////////////////////////////////////////////////////
    namespace Private
    {
        ///////////////////////////////////////////////////////
        // Identical to "tstring_view::operator==()" but this
        // has a bug in the VS2017 release of VC++ (details
        // don't matter since VS2017 is getting old now
        // anyway). The bug was fixed in the VS2019 release of
        // VC++. If (when) the VS2017 release of VC++ is
        // eventually dropped, the function can be removed and
        // calls to it can simply be replaced with a call to
        // "str1 == str2" (i.e., the first return statement
        // seen below).
        ///////////////////////////////////////////////////////
        inline constexpr bool IsEqualTo(tstring_view str1,
                                        tstring_view str2) noexcept
        {
            ////////////////////////////////////////////////
            // Any compiler other than VC++ from Microsoft
            // Visual Studio 2017 ...
            ////////////////////////////////////////////////
            #if !defined(MSVC_FROM_VISUAL_STUDIO_2017)
                return (str1 == str2);
            //////////////////////////////////////////////////
            // VC++ from Visual Studio 2017. Code just above
            // should work there as well but it's buggy
            // (can return false even when true). The
            // following is a manual work-around (equivalent
            // to code above).
            //////////////////////////////////////////////////
            #else
                if (str1.size() == str2.size())
                {
                    for (tstring_view::size_type i = 0; i < str1.size(); ++i)
                    {
                        if (str1[i] != str2[i])
                        {
                            return false;
                        }
                    }

                    return true;
                }
                else
                {
                    return false;
                }
            #endif
        };

        ///////////////////////////////////////////////////
        // Identical to "tstring_view::ends_with()" but
        // this member isn't "constexpr" until C++20. We
        // therefore just defer to the latter in C++20 or
        // later or roll our own (equivalent) otherwise
        // (in C++17 - earlier versions not possible at
        // this point - check for CPP17_OR_LATER at top of
        // file ensures this). Note that this function can
        // be removed and simply replaced with calls to
        // "str.ends_with()" if we ever drop support for
        // C++17.
        ///////////////////////////////////////////////////
        inline constexpr bool EndsWith(tstring_view str,
                                       tstring_view suffix) noexcept
        {
            #if CPP20_OR_LATER
                // Not available until C++20
                return str.ends_with(suffix);
            #else
                ///////////////////////////////////////////////////////
                // Roll our own in C++17 (no earlier version possible
                // at this stage - checked for CPP17_OR_LATER at top
                // of file and preprocessed out any earlier versions).
                ///////////////////////////////////////////////////////
                return str.size() >= suffix.size() &&
                       IsEqualTo(str.substr(str.size() - suffix.size()),
                                 suffix);
            #endif
        }

        ////////////////////////////////////////////////////////////
        // Converts "str" to a "std::array" consisting of all chars
        // in "str" at the indexes specified by the 2nd arg (a
        // collection of indexes in "str" indicating which chars in
        // "str" will be copied into the array). A NULL terminator
        // is also added at the end. The size of the returned
        // "std::array" is therefore the number of indexes in the
        // 2nd arg plus 1 for the NULL terminator. The array itself
        // is populated with all chars in "str" at these particular
        // indexes (again, with a NULL terminator added). The type
        // of the returned array is therefore:
        //
        //     std::array<tchar, sizeof...(I) + 1>
        //
        // Note that in practice this function is usually called to
        // convert an entire constexpr string to a "std::array" so
        // the function is usually called this way:
        //
        //     // Any "constexpr" string
        //     constexpr tstring_view str = _T("Testing");
        //
        //     ///////////////////////////////////////////////////////
        //     // Convert above to a "std::array". The 2nd arg is
        //     // just the sequential sequence {0, 1, 2, ..., N - 1}
        //     // where "N" is "str.size()", so all chars in "str"
        //     // are copied over (in the expected order 0 to N - 1).
        //     ///////////////////////////////////////////////////////
        //     constexpr auto strAsArray = StrToNullTerminatedArray(str, std::make_index_sequence<str.size()>());
        //
        // The 2nd arg above is therefore just the sequence of
        // (std::size_t) integers in the range 0 to the number of
        // characters in "str" - 1. The function therefore returns
        // a "std::array" containing a copy of "str" itself since
        // the 2nd arg specifies every index in "str" (so each
        // character at these indexes is copied to the array). The
        // returned type in the above example is therefore the
        // following (7 characters in the above string plus 1 for
        // the NULL terminator which we always manually add):
        //
        //     std::array<tchar, 8>;
        //
        // Note that having to pass "std::make_index_sequence" as
        // seen in the above example is syntactically ugly however.
        // It's therefore cleaner to call the other
        // "StrToNullTerminatedArray" overload just below instead,
        // which is designed for this purpose (to copy all of "str"
        // which most will usually be doing - see overload below
        // for details). It simply defers to the overload you're
        // now reading but it's a bit cleaner.
        //
        // Even the cleaner overload below is still syntactically
        // ugly however (both overloads are), but unfortunately C++
        // doesn't support "constexpr" parameters at this writing,
        // which would eliminate the issue. The issue is that the
        // 2nd template arg of "std::array" is the size of the
        // array itself and this arg must be known at compile-time
        // of course (since it's a template arg). Since the "str"
        // parameter of both overloads isn't "constexpr" however
        // (since C++ doesn't currently support "constexpr"
        // parameters), neither function can directly pass
        // "str.size()" as the 2nd template parameter to
        // "std::array", even though "str" itself may be
        // "constexpr" at the point of call. It means that even
        // when "str" is "constexpr" at the point of call, the user
        // is still forced to pass "str.size()" as a template
        // parameter at the point of call since the function itself
        // can't legally do it (it can't pass it as the 2nd
        // template arg of "std::array" since "str" isn't
        // "constexpr" inside the function - it must therefore be
        // passed as a template parameter at the point of call
        // instead, which makes the syntax of these functions
        // unnatural). The situation is ugly and unwieldy since it
        // would be much cleaner and more natural to just grab the
        // size from "str.size()" inside the function itself, but
        // the language doesn't support it in a "constexpr" context
        // at this writing (maybe one day).
        //
        // The upshot is that the following overload is designed to
        // circumvent this C++ deficiency but will rarely be called
        // directly by most users (unless it's needed to copy a
        // different sequence of characters from "str" to the
        // returned array other than all of them). If you need to
        // copy all of "str" to the returned array (usually the
        // case), then it's cleaner to call the other overload just
        // below instead. See this for details.
        ////////////////////////////////////////////////////////////
        template <std::size_t... I>
        inline constexpr auto StrToNullTerminatedArray(tstring_view str,
                                                       std::index_sequence<I...>) noexcept
        {
            ///////////////////////////////////////////////////////////
            // Creates a "std::array" and initializes it to all chars
            // in "str" (just a fancy way to copy it but in the same
            // order given by "I" - usually sequential), but also
            // adding a NULL terminator as seen, guaranteeing the
            // array is NULL terminated (so callers can safely depend
            // on this if required - note that whether "str" itself is
            // already NULL terminated is therefore irrelevant to us -
            // harmless if it is but we just ignore it).
            //
            // Note BTW that we could rely on CTAD (Class Template
            // Argument Deduction) in the following call if we wish,
            // and therefore avoid explicitly passing the "std::array"
            // template args in the return value as we're now doing
            // (since it's more clear what's being returned IMHO). If
            // we relied on CTAD instead then we could just return the
            // following instead (less verbose than manually passing
            // the "std::array" template args as we're now doing but
            // again, it's not as clear what's being returned IMHO):
            //
            //     return std::array{str[I]..., _T('\0')};
            //
            // The type of "std:array" would then resolve to the
            // following, where "N" is the number of (std::size_t)
            // integers in parameter pack "I", and there's one extra
            // tchar for the NULL terminator we're manually adding
            // (hence the "+ 1"):
            //
            //     std::array<tchar, N + 1>;
            //
            // In either case, whether we explicitly pass the
            // "std::array" template args or rely on CTAD, parameter
            // pack expansion of "I" occurs as always so if "I" is
            // {0, 1, 2} for instance (though it doesn't have to be in
            // sequential order if a caller requires a different order
            // but usually not), then it resolves to the following
            // (again, note that we manually pass the NULL terminator
            // as seen):
            //
            //     return std::array<tchar, 4>{str[0], str[1], str[2], _T('\0')};
            //
            // Note that things also work if "I" is empty (so its size
            // is zero), normally because "str" is empty so there's
            // nothing to copy. In this case the call simply resolves
            // to the following:
            //
            //     return std::array<tchar, 1>{_T('\0')};
            //
            // The above array therefore stores the NULL terminator
            // only (as would be expected).
            ///////////////////////////////////////////////////////////
            return std::array<tchar, sizeof...(I) + 1>{str[I]..., _T('\0')};
        }

        ////////////////////////////////////////////////////////////
        // See other overload just above. The following overload
        // just defers to it in order to convert "str" to a
        // "std::array" which it then returns (with a NULL
        // terminator always added - see overload above). Note
        // that the following overload exists to clean up the
        // syntax of the above overload a bit when you need to
        // convert the entire "str" arg to a "std::array", as
        // most will usually be doing (though to copy any other
        // sequence of characters from "str" to a "std::array"
        // you'll need to call the above overload directly
        // instead, passing the specific indexes in "str" you
        // wish to copy - very rare though).
        //
        // Note that as explained in the overload above, the
        // syntax for both overloads is ugly due to "constexpr"
        // shortcomings in current versions of C++. The following
        // overload is nevertheless a cleaner version than the
        // one above when you simply need to convert "str" to a
        // "std::array" (again, as most will usually be doing).
        //
        //     Example
        //     -------
        //     // Any "constexpr" string
        //     constexpr tstring_view str = _T("Testing");
        //
        //     //////////////////////////////////////////////////
        //     // The type of "array" returned by the following
        //     // call is therefore the following:
        //     //
        //     //     std::array<tchar, 8>
        //     //
        //     // Note that its size, 8, is the length of
        //     // "Testing" above, 7, plus 1 for the NULL
        //     // terminator, but the NULL terminator doesn't
        //     // originate from the one in the "Testing" string
        //     // literal above. Instead we always manually add
        //     // our own. "str" above therefore need not be NULL
        //     // terminated even though it is in this case
        //     // (since it's initialized from a string literal
        //     // above which does include one - if initialized
        //     // from another source that's not NULL terminated
        //     // however then it doesn't matter since we always
        //     // manually add our own).
        //     //
        //     // Note that as seen in the following call,
        //     // "str.size()" should always be passed as the 1st
        //     // template parameter to "StrToNullTerminatedArray()",
        //     // but having to do this is ugly. That is,
        //     // "StrToNullTerminatedArray()" can simply get
        //     // hold of this by invoking "size()" on the same
        //     // "str" parameter we're passing it, so having to
        //     // separately pass it as a template parameter is
        //     // really unnecessary. However, since the "size()"
        //     // member is required in a "constexpr" context and
        //     // C++ doesn't support "constexpr" function
        //     // arguments at this writing, the function can't
        //     // call "size()" on its "str" arg in a "constexpr"
        //     // context so we're forced to pass it as a
        //     // template parameter instead (for now - maybe C++
        //     // will support "constexpr" function parameters
        //     // one day)
        //     //////////////////////////////////////////////////
        //     constexpr auto strAsArray = StrToNullTerminatedArray<str.size()>(str);
        ////////////////////////////////////////////////////////////
        template <std::size_t Size> // Always pass "str.size()"
        inline constexpr auto StrToNullTerminatedArray(tstring_view str) noexcept
        {
            ////////////////////////////////////////////////
            // Defer to overload just above, passing all
            // indexes that need to be copied from "str"
            // into the resulting array (via the 2nd arg,
            // i.e., zero to the number of indexes in "str"
            // - 1). Note that the "Size" template arg
            // we're passing to "std::make_index_sequence"
            // must always be "str.size()" itself (callers
            // should always call us this way - see
            // function comments above)
            //////////////////////////////////////////////////
            return StrToNullTerminatedArray(str, std::make_index_sequence<Size>());
        }

        ///////////////////////////////////////////////////////////////
        // "TypeNameImplBase". Base class of "TypeNameImpl" that
        // follows just after it. Latter class is a template but the
        // following class isn't. Therefore contains functions that
        // don't depend on template arg "T" of "TypeNameImpl" so no
        // need to declare them there. "TypeNameImpl" simply inherits
        // from the following class instead and defers to it to carry
        // out the actual work (of extracting the type name for its
        // "T" template arg from __PRETTY_FUNCTION__ or (on MSFT
        // platforms) __FUNCSIG__)
        ///////////////////////////////////////////////////////////////
        class TypeNameImplBase
        {
        protected:
            //////////////////////////////////////////////////////////////////
            // *** IMPORTANT ***
            //
            // Must be declared before 1st use or compilation will fail in
            // some compilers (declaration order of function templates
            // matters).
            //
            // GetPrettyFunction(). Returns the predefined string constant
            // __PRETTY_FUNCTION__ or (for MSFT) __FUNCSIG__ (the latter if
            // _MSC_VER is #defined so it also applies to non-Microsoft
            // compilers running in Microsoft VC++ compatibility mode - see
            // "Non-Microsoft compilers targeting Windows" further below).
            // Returns these as a "tstring_view" which lives for the life of
            // the app since it's just a view into the latter strings which
            // are always statically defined (though compilers will normally
            // remove these static strings from the final compiled binary if
            // they determine the returned "tstring_view" is used in a
            // compile-time only context - they're not used at runtime IOW so
            // they can safely be removed).
            //
            // Assuming template arg "T" is a float for instance, each
            // resolves to the following at this writing (where the first
            // three rows show the offsets in the strings for your guidance
            // only, and the rows just after show the actual value of the
            // above strings for the compilers we currently support). Note
            // that each string originates from __PRETTY_FUNCTION__ if
            // _MSC_VER is not #defined or __FUNCSIG__ otherwise (note that
            // when non-Microsoft compilers are running in Microsoft VC++
            // compatibility mode then _MSC_VER will always be #defined so we
            // rely on __FUNCSIG__ as noted, but __PRETTY_FUNCTION__ is still
            // #defined by those compilers as well, even though the format
            // differs from __FUNCSIG__ a bit - we just ignore
            // __PRETTY_FUNCTION__ altogether in this case however):
            //                                                                                                                                       1         1         1         1         1         1         1
            //                                             1         2         3         4         5         6         7         8         9         0         1         2         3         4         5         6
            //                                   012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901
            //   GCC (1 of 2 - described below): static constexpr auto StdExt::Private::TypeNameImplBase::GetPrettyFunction() [with T = float]
            //   GCC (2 of 2 - described below): static constexpr StdExt::tstring_view StdExt::Private::TypeNameImplBase::GetPrettyFunction() [with T = float; StdExt::tstring_view = std::basic_string_view<char>]
            //   Clang:                          static auto StdExt::Private::TypeNameImplBase::GetPrettyFunction() [T = float]
            //   Clang (if _MSC_VER #defined):   static auto __cdecl StdExt::Private::TypeNameImplBase::GetPrettyFunction(void) [T = float]
            //   Intel:                          static auto StdExt::Private::TypeNameImplBase::GetPrettyFunction() [T = float]
            //   Intel (if _MSC_VER #defined):   static auto __cdecl StdExt::Private::TypeNameImplBase::GetPrettyFunction(void) [T = float]
            //   Microsoft VC++:                 auto __cdecl StdExt::Private::TypeNameImplBase::GetPrettyFunction<float>(void) noexcept
            //
            // GCC
            // ---
            // Note that unlike the other compilers, two possible formats
            // exist for GCC as seen above (the others have one), where the
            // one used depends on the return type of "GetPrettyFunction()"
            // itself. GCC format "1 of 2" above is the one currently in
            // effect at this writing since "GetPrettyFunction()" currently
            // returns "auto", which just resolves to
            // "std::basic_string_view<tchar>" (unless someone changed the
            // return type since this writing which is safely handled but
            // read on). Because "GetPrettyFunction()" is *not* returning an
            // alias for another type in this case (it's returning "auto"
            // which is not treated as an alias by GCC), GCC uses format "1
            // of 2". If the return type of "GetPrettyFunction()" is ever
            // changed however so that it returns an alias instead, such as
            // "tstring_view" seen in "2 of 2" above (which is an alias for
            // "std::basic_string_view<tchar>" - note that tchar always
            // resolves to "char" on GCC), then GCC uses format "2 of 2"
            // above instead. In this case it includes (resolves) this alias
            // at the end of __PRETTY_FUNCTION__ itself, as seen. This refers
            // to the "tstring_view = std::basic_string_view<char>" portion
            // of the "2 of 2" string above. It removes this portion of the
            // string in "1 of 2" however since the return value of
            // "GetPrettyFunction()" is not returning an alias in this case
            // but "auto" (or you can change "auto" directly to
            // "std::basic_string_view<tchar>" if you wish since it's still
            // not an alias). The "tstring_view =
            // std::basic_string_view<char>" portion of the "2 of 2" string
            // is no longer required IOW since there's no "tstring_view"
            // alias anymore so GCC removes it. The upshot is that the offset
            // to the type we're trying to extract from __PRETTY_FUNCTION__,
            // a "float" in the above example but whatever the type happens
            // to be, can change depending on the return type of
            // "GetPrettyFunction()" itself. Therefore, unlike the case for
            // all other compilers we currently support, whose offset to the
            // type is always at the same consistent location from the start
            // of __PRETTY_FUNCTION__ or (when _MSC_VER is #defined)
            // __FUNCSIG__, we need to check which format is in effect for
            // GCC in order to protect against possible changes to the return
            // type of "GetPrettyFunction()" itself. "GetTypeNameOffset()",
            // which calculates the offset of "T" into the string (offset of
            // "float" in the above examples), therefore takes this situation
            // into account for GCC only (checking which of these two formats
            // is in effect).
            //
            // Reliance on "float" to calculate the offset and length
            // ------------------------------------------------------
            // Note that float isn't just used for the examples above, it's
            // also used by members "GetTypeNameOffset()" and
            // "GetTypeNameLen()" below to determine the offset into each
            // string of the type name to extract (whatever template arg "T",
            // resolves to), and the type's length, both of which are called
            // by member "ExtractTypeNameFromPrettyFunction()" below to
            // extract the type name from the string. Since type float always
            // returns "float" for all compilers we support (which will
            // likely be the case for any future compiler as well normally,
            // though any type whose string is the same for all supported
            // compilers will do - read on), we can leverage this knowledge
            // to easily determine the offset of type "T" in the string and
            // its length, regardless of what "T" is, without any complicated
            // parsing of the above strings (in order to locate "T" within
            // the string and determine its length). Note that parsing would
            // be a much more complicated approach because type "T" itself
            // can potentially contain any character including angled
            // brackets, square brackets, equal signs, etc., each of which
            // makes it harder to distinguish between those particular
            // characters in "T" itself from their use as delimiters in the
            // actual "pretty" strings (when they aren't part of "T"). It
            // would usually be rare in practice but can happen. For
            // instance, "T" might be a template type (class) called, say,
            // "Bracket", with a non-type template arg of type "char" so
            // "Bracket" can be instantiated (and therefore appear in
            // __PRETTY_FUNCTION__ or __FUNCSIG__) like so (ignoring the
            // class' namespace if any to keep things simple):
            //
            //     Bracket<']'>
            //     Bracket<'>'>
            //     Bracket<'='>
            //     Bracket<';'>
            //
            // This makes it more difficult to parse __PRETTY_FUNCTION__ and
            // __FUNCSIG__ looking for the above type and determining its
            // length because the above strings contain the same characters
            // (potentially) used as delimiters elsewhere in
            // __PRETTY_FUNCTION__ and __FUNCSIG__ (where applicable), so any
            // parsing code would have to deal with this as required (i.e.,
            // distinguishing between these characters in "T" itself and
            // delimiters elsewhere in the "pretty" string is non-trivial).
            // For example, determining if ']' is part of the type itself or
            // an actual delimiter in __PRETTY_FUNCTION__ is less simple than
            // it first appears (not rocket science but a pain nevertheless).
            //
            // To circumvent having to do this (parse the string to deal with
            // this issue), there's a much easier alternative. For the offset
            // of "T" itself, note that it's always the same within each
            // supported compiler regardless of "T" (i.e., the same within
            // GCC, the same within Microsoft, the same within Clang, etc. -
            // it will normally be different for each particular compiler of
            // course but all that matters for our purposes is that it's the
            // same within each one). Within each particular compiler we can
            // therefore simply rely on a known type like "float" to
            // determine it (for that compiler), not "T" itself (since the
            // offset to "T" for any particular compiler will be the same as
            // the offset to "float" or any other type for that matter
            // (within that compiler), so we can arbitrarily just rely on
            // "float" - more on why "float" was chosen later).
            //
            // While the offset to "T" within the "pretty" string is always
            // identical no matter what "T" is (within each compiler), the
            // length of "T" itself will differ of course (depending on what
            // "T" is). However, we know that for any two different types of
            // "T", say "int" and "float" (any two types will do), the
            // "pretty" string containing "int" will be identical to the
            // "pretty" containing "float" except for the difference between
            // "int" and "float" themselves. That's the only difference
            // between these "pretty" strings, i.e., one contains "int" and
            // one contains "float", but the remainder of the "pretty"
            // strings are identical. Therefore, since the string "int" is 3
            // characters long and the string "float" is 5 characters long (2
            // characters longer than "int"), then the length of the "pretty"
            // string containing "int" must be shorter than the "pretty"
            // string containing "float" by 2 characters, since the strings
            // themselves are identical except for the presence of "int" and
            // "float" (within their respective strings). So by simply
            // subtracting the length of the "pretty" string for a "float"
            // from the length of the "pretty" string for any type "T" (i.e.,
            // by simply computing this delta), we know how much longer
            // (positive delta) or shorter (negative delta) the length of "T"
            // must be compared to a "float", since the latter is always 5
            // characters long. We therefore just add this (positive or
            // negative) delta to 5 to arrive at the length of "T" itself.
            // For an "int" for instance, the delta is -2 (the length of its
            // "pretty" string minus the length of the "pretty" string for
            // "float" is always -2), so 5 - 2 = 3 is the length of an "int".
            // For a type longer than "float" it works the same way only the
            // delta is positive in this case. If "T" is an "unsigned int"
            // for instance then the delta is 7 (the length of its "pretty"
            // string minus the length of the "pretty" string for "float" is
            // always 7), so 5 + 7 = 12 is the length of "unsigned int". We
            // can do this for any arbitrary "T" of course to get its own
            // length. We simply compute the delta for its "pretty" string in
            // relation to the "pretty" string for a "float" as described.
            //
            // Note that float was chosen over other types we could have used
            // since the name it generates in its own pretty string is always
            // "float" for all our supported compilers. It's therefore
            // consistent among all supported compilers and its length is
            // always 5, both situations making it a (normally) reliable type
            // to work with in our code. In practice however all the
            // fundamental types or (possibly) some user-defined type could
            // have been used (each of which normally generates the same
            // consistent string as well), but going forward "float" seemed
            // (potentially) less vulnerable to issues like signedness among
            // integral types for instance, or other possible issues. If an
            // integral type like "int" was chosen instead for instance (or
            // "char", or "long", etc.), some future compiler (or perhaps
            // some compiler option) might display it as "signed int" or
            // "unsigned int" instead, depending on the default signedness in
            // effect (and/or some compiler option but regardless, it might
            // not result in a consistent string among all compilers we
            // support or might support in the future).
            //
            // Similarly, if we chose "double" instead it might be displayed
            // as "double" as you would normally expect, but "long double"
            // might also be possible based on some compiler option (so the
            // string "double" may not be consistent either).
            //
            // Or if "bool" were chosen it might normally appear as "bool"
            // but "unsigned char" might be possible too if some backwards
            // compatibility option is turned on for a given compiler (since
            // bools may have been internally declared that as "unsigned
            // char" once-upon-a-time and someone may turn the option on for
            // backwards compatibility reasons if required).
            //
            // In reality it doesn't seem likely this is actually going to
            // happen for any of the fundamental types however, and even
            // "float" itself could potentially become a "double" (or
            // whatever) under some unknown circumstance but for now it seems
            // to be potentially more stable than the other fundamental types
            // so I chose it for that reason only (even if these reasons are
            // a bit flimsy). It normally works.
            //
            // Non-Microsoft compilers targeting Windows
            // -----------------------------------------
            // Note that non-Microsoft compilers targeting Windows will
            // #define _MSC_VER just like the Microsoft compiler does (VC++),
            // indicating they're running in Microsoft VC++ compatibility
            // mode (i.e., will compile VC++ Windows applications). For most
            // intents and purposes we can therefore (usually) just test if
            // _MSC_VER is #defined throughout our code and if true it means
            // that either VC++ is running or some other non-Microsoft
            // compiler is but it's running as if it were VC++. We can
            // therefore usually just ignore the fact that it's not the real
            // VC++ itself since it's running as if it were (so we can just
            // carry on as if the real VC++ is running). However, instead of
            // checking _MSC_VER we can also explicitly check the actual
            // (real) compiler that's running by using our own #defined
            // constants MICROSOFT_COMPILER, CLANG_COMPILER and
            // INTEL_COMPILER instead (the only compilers we currently
            // support that might #define _MSC_VER - MICROSOFT_COMPILER
            // itself refers to the real VC++ compiler so it always #defines
            // it but the other two only #define it when running in Microsoft
            // compatibility mode). For many (actually most) purposes
            // however we can just check _MSC_VER as described above when we
            // don't care whether it's the real VC++ compiler
            // (MICROSOFT_COMPILER #defined) vs the Clang compiler
            // (CLANG_COMPILER #defined) or the Intel compiler
            // (INTEL_COMPILER #defined), where the latter two cases simply
            // mean these non-Microsoft compilers are running in Microsoft
            // VC++ compatibility mode (since _MSC_VER is also #defined).
            //
            // Unfortunately the behavior of these non-Microsoft compilers
            // isn't always 100% compatible with the actual (real) Microsoft
            // compiler itself however. In particular, for the purposes of
            // the function you're now reading, note that the code relies on
            // the predefined Microsoft macro __FUNCSIG__ instead of
            // __PRETTY_FUNCTION__ whenever _MSC_VER is #defined, so the
            // actual (real) compiler doesn't matter to us (since __FUNCSIG__
            // is also #defined for non-Microsoft compilers running in
            // Microsoft VC++ compatibility mode). However, __FUNCSIG__ isn't
            // a string literal or even a macro when using non-Microsoft
            // compilers, unlike when using the actual Microsoft VC++
            // compiler itself. It normally should be a string literal even
            // on non-Microsoft compilers (if they were 100% compatible with
            // VC++) but unfortunately it's not. It's just an array of
            // "const" char when using non-Microsoft compilers but not an
            // actual string literal. We therefore normally shouldn't be able
            // to apply the native Microsoft macro _T to it since _T is just
            // the C++ string literal prefix "L" when compiling for UTF-16 in
            // Windows (usually the case), and the "L" prefix can only be
            // applied to string literals. See the following for details
            // (also consult _T in the MSFT docs if you're not already
            // familiar with it):
            //
            //     Wide string literals
            //     https://learn.microsoft.com/en-us/cpp/cpp/string-and-character-literals-cpp?view=msvc-170#wide-string-literals
            //
            // Since the "L" prefix can only be applied to string literals
            // then it shouldn't work with __FUNCSIG__ when using
            // non-Microsoft compilers, since the latter constant isn't a
            // string literal in non-Microsoft compilers as described. It's
            // only a string literal when using VC++ itself (as officially
            // documented by MSFT), so "L" should only work when compiling
            // with VC++. However, it turns out that "L" does work with
            // __FUNCSIG__ in non-Microsoft compilers, at least the ones we
            // support, even though it's not a string literal in those
            // compilers. While I'm not sure why without further digging
            // (read on), some type of special work-around was apparently
            // introduced by these particular compiler vendors to handle
            // __FUNCSIG__ as a string literal even though it's apparently
            // not defined that way (and presumably applicable to other
            // Microsoft predefined constants in these non-Microsoft
            // compilers as well I assume, though it doesn't impact out
            // situation here). See here for details about the situation but
            // it requires reading through the details to better understand
            // the fix (I haven't myself):
            //
            //    Clang + -fms-extensions: __FUNCSIG__ is not a literal
            //    https://github.com/llvm/llvm-project/issues/114
            //
            //    /////////////////////////////////////////////////////////
            //    // The issue is then cited as closed by "aeubanks" in
            //    // the above link (git commit 856f384), but he later
            //    // indicates that "patch was reverted, reopening", only
            //    // to close it again later on (git commit 878e590, which
            //    // the following links to). Apparently that was the
            //    // final fix.
            //    /////////////////////////////////////////////////////////
            //    https://github.com/llvm/llvm-project/commit/878e590503dff0d9097e91c2bec4409f14503b82
            //
            // Also see the following link which shows changes in a file called
            // "clang/docs/ReleaseNotes.rst"
            //
            //     [clang] Make predefined expressions string literals under -fms-extensions
            //     https://reviews.llvm.org/rG856f384bf94513c89e754906b7d80fbe5377ab53
            //
            // There's a comment in the "clang/docs/ReleaseNotes.rst" file
            // itself that indicates:
            //
            //     "Some predefined expressions are now treated as string
            //      literals in MSVC compatibility mode."
            //
            // I haven't looked into the situation in detail but the upshot
            // is that in the code just below we rely on __FUNCSIG__ whenever
            // _MSC_VER is #defined regardless of the compiler, and then
            // apply the _T macro to __FUNCSIG__ as seen, which works even
            // when using non-Microsoft compilers whose __FUNCSIG__ isn't a
            // string literal (in the compilers we currently support). It's
            // still unclear though if any compiler-specific options might
            // impact the situation but we'll have to live with the
            // uncertainty for now (though the above links would seem to
            // suggest the issue is fixed - __FUNCSIG__ is apparently treated
            // as a string literal again in all non-Microsoft compilers we
            // support).
            //
            // Note that relying on __PRETTY_FUNCTION__ instead when
            // targeting non-Microsoft compilers can't be used as a
            // (viable/practical) alternative to __FUNCSIG__, even though
            // __PRETTY_FUNCTION__ is also defined on non-Microsoft compilers
            // (in addition to __FUNCSIG__, though they each resolve to
            // different formats however). That's because the _T macro can't
            // be applied to __PRETTY_FUNCTION__ since it won't compile
            // because __PRETTY_FUNCTION__ isn't treated as a string literal
            // like __FUNCSIG__ is. _T only works on __FUNCSIG__ itself,
            // again, because a fix was specifically introduced to handle it
            // as described above (to make it compatible with VC++). Without
            // the _T macro we therefore can't (easily) convert
            // __PRETTY_FUNCTION__ to UTF-16 when using non-Microsoft
            // compilers, in particular since the following function (and all
            // others in the class) is "constexpr" so the conversion to
            // UTF-16 needs to be done at compile-time (which is harder to do
            // than at runtime so more trouble than it's worth).
            // __PRETTY_FUNCTION__ therefore can't be used unless we work
            // around the _T situation. For more information on _T in general
            // see here at this writing (for starters):
            //
            //     Unicode Programming Summary
            //     https://learn.microsoft.com/en-us/cpp/text/unicode-programming-summary?view=msvc-170
            //
            // The bottom line is that when _MSC_VER is #defined, the
            // following function assumes the build environment is compatible
            // with VC++ even if a non-Microsoft compiler is actually being
            // used (such as when the GCC, Clang or Intel compiler is
            // explicitly installed as an optional component in Microsoft
            // Visual Studio on Windows, and then selected as the compiler to
            // use by going into your C++ project's properties, selecting
            // "Configuration properties -> General" and choosing the
            // applicable compiler from the "Platform toolset" dropdown). If
            // "LLVM (clang-cl)" is selected for instance (the documented
            // name of the Clang compiler at this writing - see
            // https://learn.microsoft.com/en-us/cpp/build/clang-support-msbuild?view=msvc-170
            // though the name is subject to change by MSFT of course), then
            // the Clang compiler will be used instead of the native VC++
            // compiler. Although our own #defined constant CLANG_COMPILER
            // will then be defined in this case (instead of the
            // MICROSOFT_COMPILER since VC++ itself is no longer the chosen
            // compiler), the native Microsoft constant _MSC_VER will still
            // be #defined which we check for just below (instead of
            // CLANG_COMPILER). __FUNCSIG__ will then be used as seen below
            // (and _T applied to it as seen), since _T will still correctly
            // work on it as described above, even though __FUNCSIG__ isn't a
            // string literal (unlike when the actual Microsoft VC++ compiler
            // is used). Again, we can't rely on __PRETTY_FUNCTION__ in this
            // case even though it will also be #defined since _T can't be
            // applied to it (since it's not a string literal and no
            // work-around was made by the compiler vendors to treat it like
            // a string literal, unlike in the __FUNCSIG__ case).
            //
            // Lastly, note that __FUNCSIG__ isn't formatted the same way for
            // non-Microsoft compilers as it is for VC++ itself. In
            // non-Microsoft compilers, __FUNCSIG__ is formatted with the
            // trailing "[T = float]" syntax (using the examples seen at the
            // top of this comment block), so it follows the same basic
            // format as __PRETTY_FUNCTION__ does in this regard (though the
            // overall format of __FUNCSIG__ does differ slightly from
            // __PRETTY_FUNCTION__ but the differences are cosmetic only so
            // it doesn't affect our code in anyway - __FUNCSIG__ includes
            // the calling convention for instance and __PRETTY_FUNCTION__
            // doesn't but this has no impact on any of the code in this or
            // any other function in this class).
            //////////////////////////////////////////////////////////////////
            template <typename T>
            static constexpr auto GetPrettyFunction() noexcept
            {
                ///////////////////////////////////////////////////////////////
                // MSFT? Note the following (_MSC_VER) is also #defined by
                // non-Microsoft compilers when they're targeting Windows
                // (i.e., when they're running in Microsoft VC++ compatibility
                // mode). We therefore use __FUNCSIG__ in those cases as well.
                // even though those compilers also declare __PRETTY_FUNCTION__
                // (but we don't rely on the latter when _MSC_VER is #defined
                // since we just assume we're dealing with VC++ - the
                // behavior of other compilers running in Microsoft
                // compatibility mode should normally be close enough for our
                // purpose that any differences won't impact us - if they do
                // then it's normally rare and we provide special handling to
                // deal with it). See the section "Non-Microsoft compilers
                // targeting Windows" in long comments above.
                ///////////////////////////////////////////////////////////////
                #if defined(_MSC_VER)
                    return tstring_view(_T(__FUNCSIG__));
                ////////////////////////////////////////////////////////////////
                // Just some useful info on the subject of __PRETTY_FUNCTION__
                // (mainly to do with GCC but provides additional insight):
                //
                //    https://gcc.gnu.org/onlinedocs/gcc/Function-Names.html
                //    https://gcc.gnu.org/onlinedocs/gcc-3.2.3/gcc/Function-Names.html // Earlier version of above but with a bit more info
                //    https://stackoverflow.com/questions/55850013/pretty-function-in-constant-expression
                //    https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66639
                //    https://github.com/gcc-mirror/gcc/blob/41d6b10e96a1de98e90a7c0378437c3255814b16/gcc/cp/NEWS#L337
                //
                // Note that no official documentation seems to exist for Clang
                // or Intel but the latter effectively relies on the Clang
                // front-end anyway now (according to the Intel DPC++/C++ docs
                // so presumably we should rely on the Clang docs but again,
                // nothing about __PRETTY_FUNCTION is mentioned in those docs
                // at this writing).
                ////////////////////////////////////////////////////////////////
                #elif defined(GCC_COMPILER) || \
                      defined(CLANG_COMPILER) || \
                      defined(INTEL_COMPILER)
                    return tstring_view(__PRETTY_FUNCTION__); // Can't use _T on __PRETTY_FUNCTION__ here
                                                              // as described in (long) function comments
                                                              // above. __PRETTY_FUNCTION__ and "tstring_view"
                                                              // are both char-based" strings at this point
                                                              // however so we're ok (still potentially
                                                              // brittle though but we'll live with it for
                                                              // now - should be safe unless this ever
                                                              // changes)
                #else
                    /////////////////////////////////////////////////////
                    // Note: Shouldn't be possible to come through here
                    // at this writing since the same #error message
                    // would have already been displayed earlier (when
                    // the compiler constants just above were #defined
                    // in "CompilerVersions.h")
                    /////////////////////////////////////////////////////
                    #error "Unsupported compiler (GCC, Microsoft, Clang and Intel are the only ones supported at this writing)"
                #endif
            }

    ///////////////////////////////////////////////////////////
    // #define TYPENAME_V_IMPL_2 to use a slightly different
    // implementation. Both produce the same results however
    // and there's no significant advantage of using one over
    // the other. By default it's not #defined which might
    // yield a slightly more efficient implementation (I only
    // suspect) but the difference is negligible. Either
    // implementation is fine (have never investigated which
    // is really better since the difference is miniscule)
    ///////////////////////////////////////////////////////////
    #if !defined(TYPENAME_V_IMPL_2)
        private:
            template <typename T = void>
            static constexpr auto GetOffsetAndLen(tstring_view prettyFunction) noexcept
            {
                //////////////////////////////////////////////////
                // Always passing a "float" here since any type
                // will do (offset we'll be calculating below
                // is always the same regardless of the type).
                // See comments above.
                //////////////////////////////////////////////////
                constexpr tstring_view prettyFunctionFloat = GetPrettyFunction<float>();

                #if defined(GCC_COMPILER)
                    //////////////////////////////////////////////////////////////////
                    // See format of __PRETTY_FUNCTION__ string for "GCC" near the
                    // top of the comments in "GetPrettyFunction()". There are 2
                    // possible formats labeled "1 of 2" and "2 of 2" in the comments
                    // (with an explanation of when each kicks in - see comments for
                    // details). The offset to the type in this string is the same
                    // regardless of these two formats however (for whatever format
                    // is in use), and regardless of the type itself, so we arbitrarily
                    // use float in the code below (again, see comments in
                    // "GetPrettyFunction()" for details). For format "1 of 2" the
                    // string always ends with "= float]" so the offset to the "f" in
                    // this string (i.e., the offset to the type we're after) is
                    // always 6 characters from the end of the string (unless the
                    // compiler vendor makes a breaking change to the string but the
                    // "static_assert" seen just after this class will trap it). This
                    // is identical to the "Clang" and "Intel" case further below so
                    // we do the same check here for GCC. If the following check for
                    // format "1 of 2" is true then the offset will be the same no
                    // matter what the type so we're good (i.e., we can just rely on
                    // the one for float and immediately return this). Unlike "Clang"
                    // and "Intel" however, where the following check must always be
                    // true so we don't need to check for it in those cases (we simply
                    // "static_assert" on it instead), if it's not true for GCC then
                    // we must be dealing with format "2 of 2" instead. In this case
                    // we need to locate "= float;" within the string and the offset
                    // to the type itself is therefore 2 characters after the '='
                    // sign. Again, just like the "1 of 2" case, the offset to the
                    // type will be the same in the "2 of 2" case no matter what the
                    // type so we can just rely on the one for float and immediately
                    // return this.
                    //////////////////////////////////////////////////////////////////

                    // GCC format 1 of 2 (see "GetPrettyFunction()" for details)
                    if constexpr (EndsWith(prettyFunctionFloat, _T("= float]")))
                    {
                        ///////////////////////////////////////////
                        // Offset to the "f" in "= float]" (i.e.,
                        // the 1st character of the type we're
                        // after). Always the same regardless of
                        // the type so our use of "float" here to
                        // calculate it will work no matter what
                        // the type.
                        ///////////////////////////////////////////
                        constexpr auto offset = prettyFunctionFloat.size() - 6;

                        //////////////////////////////////////////////
                        // Length of the type (subtracting 1 targets
                        // the ']' in "= float]", i.e., we get an
                        // offset to that ']' so subtracting "offset"
                        // from that yields the length of the type
                        // itself, i.e. everything from "offset"
                        // INclusive to ']' EXclusive)
                        //////////////////////////////////////////////
                        const auto len = prettyFunction.size() - 1 - offset;

                        return std::make_pair(offset, len);
                    }
                    else // GCC format 2 of 2 (see "GetPrettyFunction()" for details)
                    {
                        constexpr tstring_view::size_type offsetOfEqualSign = prettyFunctionFloat.rfind(_T("= float;"));
                        static_assert(offsetOfEqualSign != tstring_view::npos &&
                                      AlwaysTrue_v<T>); // "AlwaysTrue_v" required to make the "static_assert" depend
                                                        // on template arg "T" so that the "static_assert" won't
                                                        // trigger when the "if constexpr" call above is true (and
                                                        // it always will trigger in older versions of C++ in
                                                        // particular - in newer versions however the call to
                                                        // "AlwaysTrue_v" is no longer necessary since the language
                                                        // or possibly GCC itself (not certain, it's a fuzzy area)
                                                        // was changed so that the call to "static_assert" now
                                                        // implicitly relies on "T" without having to call
                                                        // "AlwaysTrue_v" - we continue to rely on it anyway for
                                                        // now)

                        ///////////////////////////////////////////
                        // Offset to the "f" in "= float;" (i.e.,
                        // the 1st character of the type we're
                        // after). Always the same regardless of
                        // the type so our use of "float" above to
                        // calculate it will work no matter what
                        // the type.
                        ///////////////////////////////////////////
                        constexpr auto offset = offsetOfEqualSign + 2;

                        ////////////////////////////////////////////////////
                        // Length of the type. Note that this calculation
                        // works for all compilers but we don't use it for
                        // them. It's faster and more efficient to
                        // calculate the length as seen for those compilers
                        // though the difference is negligible (likely on
                        // the order of nanoseconds). For this particular
                        // GCC scenario however (GCC format 2 of 2), we do
                        // rely on it since we'd otherwise have to search
                        // for the closing ';' that ends the type, which is
                        // non-trivial since the type name itself can
                        // potentially contain a ';' character (rare as
                        // this will normally be). A forward search for the
                        // closing ';' would therefore have to bypass any
                        // possible ';' characters in the type itself,
                        // again, which is non-trivial. A backwards search
                        // from the end of the string for the ';' would
                        // therefore be easier but even this is potentially
                        // brittle and a waste of time anyway, since the
                        // the following technique easily calculates the
                        // length without having to deal with these issues.
                        // Note that we don't use this calculation for all
                        // other compiler scenarios however (even though it
                        // would also work) since the type's terminating
                        // character in those cases is always at a known,
                        // fixed location so no searching for it is ever
                        // required. It's therefore trivial to calculate
                        // the length for all other compiler scenarios
                        // using the techniques seen for them, though the
                        // following technique would yield the same result
                        // as noted (and the following technique is trivial
                        // in its own right, though a tiny bit less so than
                        // the technique we rely on for all other compiler
                        // scenarios - in any case these are micro-optimizations
                        // only so the impact isn't relevant).
                        ////////////////////////////////////////////////////
                        const auto len = prettyFunction.size() > prettyFunctionFloat.size()
                                         ? (5 + (prettyFunction.size() - prettyFunctionFloat.size()))
                                         : (5 - (prettyFunctionFloat.size() - prettyFunction.size()));

                        return std::make_pair(offset, len);
                    }

                ////////////////////////////////////////////////////////
                // Note: Not checking for _MSC_VER here by design, only
                // for the native Microsoft VC+++ compiler itself. Code
                // just below therefore applies to VC++ only, not to
                // other compilers we support that also #define
                // _MSC_VER (when they're running in Microsoft VC++
                // compatibility mode). They don't format __FUNCSIG__
                // the same way VC++ does so we can't rely on the VC++
                // code just below for them. Note that they *should*
                // format it the same way since they're supposed to be
                // compatible with VC++ when _MSC_VER is #defined but
                // they're not.
                ////////////////////////////////////////////////////////
                #elif defined(MICROSOFT_COMPILER)
                    ///////////////////////////////////////////////////////////////////
                    // See format of __FUNCSIG__ string for "Microsoft (VC++)" near
                    // the top of the comments in "GetPrettyFunction()". Offset to the
                    // type in this string is the same regardless of the type so we
                    // arbitrarily use "float" here (see comments preceding latter
                    // function for details). The __FUNCSIG__ string with type "float"
                    // always ends with "<float>(void) noexcept" so the offset to the
                    // "f" in this string (i.e., the offset to the type we're after)
                    // is always 21 characters from the end (unless MSFT makes a
                    // breaking change to the string). The offset will be the same no
                    // matter what the type so we're good (i.e., we can just rely on
                    // the one for float and immediately return this).
                    ///////////////////////////////////////////////////////////////////
                    static_assert(EndsWith(prettyFunctionFloat, _T("<float>(void) noexcept")));

                    /////////////////////////////////////////////////////
                    // Offset to the "f" in "<float>(void) noexcept"
                    // (i.e., the 1st character of the type we're
                    // after). Always the same regardless of the type so
                    // our use of "float" to calculate it will work no
                    // matter what the type. Note that hardcoding 21
                    // here may seem brittle but it is in fact stable.
                    // If it ever breaks because MSFT changes the format
                    // of __FUNCSIG__ (or we encounter some previously
                    // unknown situation with its current format) than
                    // the "static_assert" just above will trigger so
                    // we're safe. I therefore consider it unnecessary
                    // to search through the string itself to find the
                    // offset to the start of the type when the
                    // following should always work normally (and
                    // quicker too though the difference would be
                    // negligible). Searching though the string is both
                    // a waste of time and potentially brittle as well,
                    // since if the format of __FUNCSIG__ ever changes
                    // then the search might fail or return some
                    // erroneous value. You would therefore still need
                    // to trigger a "static_assert" to trap it. If our
                    // own "static_assert" just above ever triggers we
                    // can review the situation then.
                    /////////////////////////////////////////////////////
                    constexpr auto offset = prettyFunctionFloat.size() - 21;

                    ///////////////////////////////////////////////////
                    // Length of the type (subtracting 16 targets the
                    // '>' in "<float>(void) noexcept", i.e., we get
                    // an offset to that '>' so subtracting "offset"
                    // from that yields the length of the type itself,
                    // i.e., everything between '<' and '>')
                    ///////////////////////////////////////////////////
                    const auto len = prettyFunction.size() - 16 - offset;

                    return std::make_pair(offset, len);
                #elif defined(CLANG_COMPILER) || defined(INTEL_COMPILER)
                    ///////////////////////////////////////////////////////////////////
                    // See format of the strings for Clang and Intel near the top of
                    // the comments in "GetPrettyFunction()" (using __PRETTY_FUNCTION
                    // when _MSC_VER isn't #defined or __FUNCSIG__ otherwise). Offset
                    // to the type in this string is the same regardless of the type
                    // so we arbitrarily use "float" here (see "GetPrettyFunction()"
                    // comments for details). Both __PRETTY_FUNCTION__ and __FUNCSIG__
                    // (based on type "float" we're applying here) always ends with
                    // "= float]" so the offset to the "f" in this string (i.e., the
                    // offset to the type we're after) is always 6 characters from the
                    // end (unless the compiler vendors makes a breaking change to the
                    // string, triggering this "static_assert"). The offset will be the
                    // same no matter what the type so we can just rely on the one for
                    // float and immediately return this.
                    ///////////////////////////////////////////////////////////////////
                    static_assert(EndsWith(prettyFunctionFloat, _T("= float]")));

                    //////////////////////////////////////////////////////
                    // Offset to the "f" in "= float]" (i.e., the 1st
                    // character of the type we're after). Always the
                    // same regardless of the type so our use of "float"
                    // to calculate it will work no matter what the
                    // type. Note that hardcoding 6 here may seem brittle
                    // but it's normally very stable. See the comments in
                    // the MICROSOFT_COMPILER code above (in its own
                    // declaration of "offset" - same thing applies here
                    // as well though applicable to "Clang" and "Intel"
                    // in this case)
                    //////////////////////////////////////////////////////
                    constexpr auto offset = prettyFunctionFloat.size() - 6;

                    //////////////////////////////////////////////
                    // Length of the type (subtracting 1 targets
                    // the ']' in "= float]", i.e., we get an
                    // offset to that ']' so subtracting "offset"
                    // from that yields the length of the type
                    // itself, i.e. everything from "offset"
                    // INclusive to ']' EXclusive)
                    //////////////////////////////////////////////
                    const auto len = prettyFunction.size() - 1 - offset;

                    return std::make_pair(offset, len);
                #else
                    /////////////////////////////////////////////////////
                    // Note: Shouldn't be possible to come through here
                    // at this writing since the same #error message
                    // would have already been displayed earlier (when
                    // the compiler constants just above were #defined
                    // in "CompilerVersions.h")
                    /////////////////////////////////////////////////////
                    #error "Unsupported compiler (GCC, Microsoft, Clang and Intel DPC++/C++ are the only ones supported at this writing)"
                #endif
            }

        protected:
            template <typename T = void>
            static constexpr tstring_view ExtractTypeNameFromPrettyFunction(tstring_view prettyFunction) noexcept
            {
                //////////////////////////////////////////
                // Returns a "std::pair" containing the
                // offset and length of the type name in
                // "prettyFunction" (in members "first"
                // and "second" respectively).
                //////////////////////////////////////////
                const auto offsetAndLen = GetOffsetAndLen(prettyFunction);

                return prettyFunction.substr(offsetAndLen.first, // Offset of the type name in "prettyFunction"
                                             offsetAndLen.second); // Length of the type name in "prettyFunction"
            }
    #else
        private:
            //////////////////////////////////////////////////////////////////////
            // GetTypeNameOffset(). Returns the offset within __PRETTY_FUNCTION__
            // or (when _MSC_VER is #defined) __FUNCSIG__, to the start of the
            // type name within the latter string. Note that as explained in
            // the comments preceding "GetPrettyFunction()" (see this for
            // details), the offset to the type's name within the latter
            // strings is always identical regardless of the type so we can
            // calculate it using any type. No need to do it for any specific
            // type that is but we create the following function as a template
            // anyway, with template arg "T" supposedly representing the type
            // we're targeting (but it's not actually used for that purpose -
            // read on). IOW, we ignore "T" for purposes of calculating the
            // type name's offset and always rely on "float" instead, as
            // described in the "GetPrettyFunction" comments (again, since the
            // type doesn't matter - the result is always the same). We create
            // the following function as a template anyway not because we need
            // to know the type to calculate its offset, but only so that we
            // can make any "static_asserts" in the code below depend on
            // template arg "T" when required. That is, because of how
            // "static_assert" works in C++, if it's used outside a
            // template-based context it will always trigger if its first arg
            // is false. For instance, see the "if constexpr" clause in the
            // GCC_COMPILER code below. When that "if constexpr" condition is
            // true, the "static_assert" seen in the corresponding "else"
            // clause would trigger if this function wasn't a template because
            // the arg being passed to that "static_assert" is false whenever
            // the "if constexpr" condition itself is true. If not for the
            // function's dependence on template arg "T" itself in the
            // "static_assert" (the 1st arg of "static_assert" depends on "T"
            // via a call to "AlwaysTrue_v<T>" - see the call to this below for
            // details), the "static_assert" would always trigger even when
            // the "else" clause isn't in effect (because the corresponding
            // "if constexpr" is true). Because of the "static_assert"
            // dependence on a template arg however (again, via the call seen
            // to "AlwaysTrue_v<T>" in that "static_assert"), the
            // "static_assert" in the "else" clause therefore won't trigger,
            // which is what we require. The bottom line is that while it
            // shouldn't be necessary to make the following function a
            // template since we never use the "T" template arg (we always
            // rely on "float" a described), in order to prevent the
            // "static_assert" from always triggering we make it depend on
            // template arg "T" (so the function itself needs to be a template
            // to accommodate this - yet another C++ joy).
            //////////////////////////////////////////////////////////////////////
            template <typename T = void>
            static constexpr tstring_view::size_type GetTypeNameOffset() noexcept
            {
                //////////////////////////////////////////////////
                // Always passing a "float" here since any type
                // will do (offset we'll be calculating below
                // is always the same regardless of the type).
                // See comments above.
                //////////////////////////////////////////////////
                constexpr tstring_view prettyFunctionFloat = GetPrettyFunction<float>();

                #if defined(GCC_COMPILER)
                    //////////////////////////////////////////////////////////////////
                    // See format of __PRETTY_FUNCTION__ string for "GCC" near the
                    // top of the comments in "GetPrettyFunction()". There are 2
                    // possible formats labeled "1 of 2" and "2 of 2" in the comments
                    // (with an explanation of when each kicks in - see comments for
                    // details). The offset to the type in this string is the same
                    // regardless of these two formats however (for whatever format
                    // is in use), regardless of the type itself, so we arbitrarily
                    // use float in the code below (again, see comments in
                    // "GetPrettyFunction()" for details). For format "1 of 2" the
                    // string always ends with "= float]" so the offset to the "f" in
                    // this string (i.e., the offset to the type we're after) is
                    // always 6 characters from the end of the string (unless the
                    // compiler vendor makes a breaking change to the string but the
                    // "static_assert" seen just after this class will trap it). This
                    // is identical to the "Clang" and "Intel" case further below so
                    // we do the same check here for GCC. If the following check for
                    // format "1 of 2" is true then the offset will be the same no
                    // matter what the type so we're good (i.e., we can just rely on
                    // the one for float and immediately return this). Unlike "Clang"
                    // and "Intel" however, where the following check must always be
                    // true so we don't need to check for it in those cases (we simply
                    // "static_assert" on it instead), if it's not true for GCC then
                    // we must be dealing with format "2 of 2" instead. In this case
                    // we need to locate "= float;" within the string and the offset
                    // to the type itself is therefore 2 characters after the '='
                    // sign. Again, just like the "1 of 2" case, the offset to the
                    // type will be the same in the "2 of 2" case no matter what the
                    // type so we can just rely on the one for float and immediately
                    // return this.
                    //////////////////////////////////////////////////////////////////

                    // GCC format 1 of 2 (see "GetPrettyFunction()" for details)
                    if constexpr (EndsWith(prettyFunctionFloat, _T("= float]")))
                    {
                        ///////////////////////////////////////////
                        // Offset to the "f" in "= float]" (i.e.,
                        // the 1st character of the type we're
                        // after). Always the same regardless of
                        // the type so our use of "float" here to
                        // calculate it will work no matter what
                        // the type.
                        ///////////////////////////////////////////
                        return prettyFunctionFloat.size() - 6;
                    }
                    else // GCC format 2 of 2 (see "GetPrettyFunction()" for details)
                    {
                        constexpr tstring_view::size_type offsetOfEqualSign = prettyFunctionFloat.rfind(_T("= float;"));
                        static_assert(offsetOfEqualSign != tstring_view::npos &&
                                      AlwaysTrue_v<T>); // "AlwaysTrue_v" required to make the "static_assert" depend
                                                        // on template arg "T" so that the "static_assert" won't
                                                        // trigger when the "if constexpr" call above is true (and
                                                        // it always will trigger in older versions of C++ in
                                                        // particular - in newer versions however the call to
                                                        // "AlwaysTrue_v" is no longer necessary since the language
                                                        // or possibly GCC itself (not certain, it's a fuzzy area)
                                                        // was changed so that the call to "static_assert" now
                                                        // implicitly relies on "T" without having to call
                                                        // "AlwaysTrue_v" - we continue to rely on it anyway for
                                                        // now)

                        ///////////////////////////////////////////
                        // Offset to the "f" in "= float;" (i.e.,
                        // the 1st character of the type we're
                        // after). Always the same regardless of
                        // the type so our use of "float" above to
                        // calculate it will work no matter what
                        // the type.
                        ///////////////////////////////////////////
                        return offsetOfEqualSign + 2;
                    }
                ////////////////////////////////////////////////////////
                // Note: Not checking for _MSC_VER here by design, only
                // for the native Microsoft VC+++ compiler itself. Code
                // just below therefore applies to VC++ only, not to
                // other compilers we support that also #define
                // _MSC_VER (when they're running in Microsoft VC++
                // compatibility mode). They don't format __FUNCSIG__
                // the same way VC++ does so we can't rely on the VC++
                // code just below for them. Note that they *should*
                // format it the same way since they're supposed to be
                // compatible with VC++ when _MSC_VER is #defined but
                // they're not.
                ////////////////////////////////////////////////////////
                #elif defined(MICROSOFT_COMPILER)
                    ///////////////////////////////////////////////////////////////////
                    // See format of __FUNCSIG__ string for "Microsoft (VC++)" near
                    // the top of the comments in "GetPrettyFunction()". Offset to the
                    // type in this string is the same regardless of the type so we
                    // arbitrarily use "float" here (see comments preceding latter
                    // function for details). The __FUNCSIG__ string with type "float"
                    // always ends with "<float>(void) noexcept" so the offset to the
                    // "f" in this string (i.e., the offset to the type we're after)
                    // is always 21 characters from the end (unless MSFT makes a
                    // breaking change to the string). The offset will be the same no
                    // matter what the type so we're good (i.e., we can just rely on
                    // the one for float and immediately return this).
                    ///////////////////////////////////////////////////////////////////
                    static_assert(EndsWith(prettyFunctionFloat, _T("<float>(void) noexcept")));

                    //////////////////////////////////////////////////
                    // Offset to the "f" in "<float>(void) noexcept"
                    // (i.e., the 1st character of the type we're
                    // after). Always the same regardless of the type
                    // so our use of "float" to calculate it will
                    // work no matter what the type.
                    //////////////////////////////////////////////////
                    return prettyFunctionFloat.size() - 21;
                #elif defined(CLANG_COMPILER) || defined(INTEL_COMPILER)
                    ///////////////////////////////////////////////////////////////////
                    // See format of the strings for Clang and Intel near the top of
                    // the comments in "GetPrettyFunction()" (using __PRETTY_FUNCTION
                    // when _MSC_VER isn't #defined or __FUNCSIG__ otherwise). Offset
                    // to the type in this string is the same regardless of the type
                    // so we arbitrarily use "float" here (see "GetPrettyFunction()"
                    // comments for details). Both __PRETTY_FUNCTION__ and __FUNCSIG__
                    // (based on type "float" we've applied here) always ends with
                    // "= float]" so the offset to the "f" in this string (i.e., the
                    // offset to the type we're after) is always 6 characters from the
                    // end (unless the compiler vendors makes a breaking change to the
                    // string, triggering this "static_assert"). The offset will be the
                    // same no matter what the type so we can just rely on the one for
                    // float and immediately return this.
                    ///////////////////////////////////////////////////////////////////
                    static_assert(EndsWith(prettyFunctionFloat, _T("= float]")));

                    //////////////////////////////////////////////////////
                    // Offset to the "f" in "= float]" (i.e., the 1st
                    // character of the type we're after). Always the
                    // same regardless of the type so our use of "float"
                    // to calculate it will work no matter what the type.
                    //////////////////////////////////////////////////////
                    return prettyFunctionFloat.size() - 6;
                #else
                    /////////////////////////////////////////////////////
                    // Note: Shouldn't be possible to come through here
                    // at this writing since the same #error message
                    // would have already been displayed earlier (when
                    // the compiler constants just above were #defined
                    // in "CompilerVersions.h")
                    /////////////////////////////////////////////////////
                    #error "Unsupported compiler (GCC, Microsoft, Clang and Intel are the only ones supported at this writing)"
                #endif
            }

            ////////////////////////////////////////////////////////////////
            // *** IMPORTANT ***
            //
            // Must be declared before 1st use or compilation will fail in
            // some compilers like Clang (declaration order of function
            // templates matters).
            //
            // GetTypeNameLen(). Returns the length of the type name within
            // "prettyFunction" where the latter string was returned via a
            // call to "GetPrettyFunction()" (so it originates from
            // __PRETTY_FUNCTION__ or (when _MSC_VER is #defined)
            // __FUNCSIG__). If the type name within "prettyFunction" is
            // (literally) "int" for instance then it returns 3. Note that
            // as explained in the comments preceding function
            // "GetPrettyFunction()", we leverage our knowledge of type
            // float to calculate the length of the type name in
            // "prettyFunction". See "GetPrettyFunction()" for details.
            ////////////////////////////////////////////////////////////////
            static constexpr tstring_view::size_type GetTypeNameLen(tstring_view prettyFunction) noexcept
            {
                constexpr tstring_view prettyFunctionFloat = GetPrettyFunction<float>();

                ///////////////////////////////////////////////////////////////
                // See full explanation in "GetPrettyFunction()" comments.
                // Consult section entitled:
                //
                //     Reliance on "float" to calculate the offset and length
                //
                // As described there, the length of the type name in
                // "prettyFunction" is determined by simply taking the
                // difference (delta) in the length of "prettyFunction" and
                // the length of the pretty function for a "float", and adding
                // this delta to the length of "float" itself (5). "float" was
                // chosen for this purpose but any type with a consistent
                // length among all supported compilers will do. Again, see
                // comments in "GetPrettyFunction()" for details.
                //
                // Lastly, note that since the following is dealing with
                // unsigned numbers, I've coded things to avoid the
                // possibility of a negative number when the length of the
                // pretty strings are subtracted from each other (if we simply
                // did the subtraction without checking for a possible
                // negative result). Subtracting a potentially larger unsigned
                // number from a smaller one gets into the murky area of
                // "negative" unsigned numbers which are never actually
                // negative of course, but simply large unsigned numbers (due
                // to a wrap-around). It's a confusing area and also gets into
                // the possibility of undefined behavior (won't get into that
                // here), so it's much simpler to just avoid this subtraction
                // and code things as seen instead (checking to make sure we
                // don't wind up with a "negative" result so the whole
                // situation avoided).
                ///////////////////////////////////////////////////////////////

                /////////////////////////////////////////////////////////////////
                // If true then the length of the type name in "prettyFunction"
                // is greater than the length of "float" itself (i.e., > 5)
                /////////////////////////////////////////////////////////////////
                if (prettyFunction.size() > prettyFunctionFloat.size())
                {
                    return 5 + (prettyFunction.size() - prettyFunctionFloat.size());
                }
                ////////////////////////////////////////////////////////////
                // Length of type name in "prettyFunction" is less than or
                // equal to the length of "float" itself (i.e., <= 5)
                ////////////////////////////////////////////////////////////
                else
                {
                    return 5 - (prettyFunctionFloat.size() - prettyFunction.size());
                }
            }

        protected:
            static constexpr tstring_view ExtractTypeNameFromPrettyFunction(tstring_view prettyFunction) noexcept
            {
                return prettyFunction.substr(GetTypeNameOffset(), // Offset of the type name in "prettyFunction"
                                             GetTypeNameLen(prettyFunction)); // Length of the type name in "prettyFunction"
            }

    #endif // if !defined(TYPENAME_V_IMPL_2)
        }; // class TypeNameImplBase

        //////////////////////////////////////////////////////////////////////
        // Implementation class for variable template "TypeName_v" declared
        // just after this class but outside of this "Private" namespace (so
        // for public use). The latter variable just invokes static member
        // "Get()" below which carries out all the work. See "TypeName_v" for
        // complete details.
        //
        // IMPORTANT:
        // ---------
        // Note that the implementation below relies on the predefined string
        // __PRETTY_FUNCTION__ or (when _MSC_VER is #defined) __FUNCSIG__
        // (Google these for details). All implementations you can find on
        // the web normally rely on these as does our own "Get()" member
        // below, but unlike most other implementations I've seen, ours
        // doesn't require any changes should you modify any part of the
        // latter function's fully-qualified name or signature (affecting the
        // value of the above predefined strings). Most other implementations
        // I've seen would require changes, even though they should normally
        // be very simple changes (trivial usually but changes nevertheless).
        // Our implementation doesn't require any so users can move the
        // following code to another namespace if they wish, change the
        // function's class name and/or any of its member functions without
        // breaking anything (well, except for its "noexcept" specifier on
        // MSFT platforms only, but nobody will ever need to change this
        // anyway, regardless of platform). Note that the code is fairly
        // small and clean notwithstanding first impressions, lengthy only
        // because of the many comments (the code itself is fairly short and
        // digestible however). It will only break normally if a compiler
        // vendor makes a breaking change to __PRETTY_FUNCTION__ or (when
        // _MSC_VER is #defined) __FUNCSIG__, but this will normally be
        // caught by judicious use of "static_asserts" in the implementation
        // and just after the following class itself (where we arbitrarily
        // test it with a float to make sure it returns "float", a quick and
        // dirty test but normally reliable).
        //////////////////////////////////////////////////////////////////////
        template <typename T>
        class TypeNameImpl : public TypeNameImplBase
        {
        public:
            ////////////////////////////////////////////////////////
            // Implementation function called by variable template
            // "TypeName_v" just after the "Private" namespace this
            // class is declared in. The following function does
            // all the work. See "TypeName_v" for details.
            ////////////////////////////////////////////////////////
            static constexpr tstring_view Get() noexcept
            {
                ///////////////////////////////////////////////////////
                // TYPENAME_V_DONT_MINIMIZE_REQD_SPACE isn't #defined
                // by default so the following normally tests true. If
                // so then the type's name is extracted from
                // __PRETTY_FUNCTION__ or (on MSFT platforms)
                // __FUNCSIG__ and statically stored in "m_TypeName"
                // (a "std::array" just big enough to store the
                // extracted type name plus 1 extra character for a
                // NULL terminator - we always add one in case callers
                // require it). We then simply return a "tstring_view"
                // that wraps "m_TypeName" though its "size()" member
                // doesn't include the NULL terminator itself, as
                // would normally be expected (but the NULL terminator
                // is still safely present in case someone invokes the
                // "data()" member for instance - the string is
                // therefore always safely NULL terminated). The
                // upshot is that __PRETTY_FUNCTION__ or __FUNCSIG__
                // is therefore *not* stored in the compiled binary as
                // they normally would be as compile time (static)
                // strings. They're removed by all supported compilers
                // entirely since they're not being used at runtime
                // and "m_TypeName" is stored instead, which only
                // takes up the minimum space required to store the
                // type name (plus a NULL terminator). We therefore
                // make this the default behavior though the savings
                // is usually negligible (but no point storing all of
                // __PRETTY_FUNCTION__ or __FUNCSIG__ when we only
                // ever target that portion of it containing the type
                // name itself - "m_TypeName" contains a copy of it so
                // there's zero overhead). However, should it ever
                // become necessary for some reason (not normally),
                // users can simply #define
                // TYPENAME_V_DONT_MINIMIZE_REQD_SPACE which removes
                // "m_TypeName" (preprocesses it out), and resorts to
                // storing __PRETTY_FUNCTION__ or __FUNCSIG__ again
                // (as static strings in the compiled binary). We then
                // return a "tstring_view" into these strings instead,
                // targeting the type name within the string. Caution
                // advised however (!) since it's no longer NULL
                // terminated as a result (because it's just a
                // substring view of the type name within
                // __PRETTY_FUNCTION__ or __FUNCSIG__ so no NULL
                // terminator will be present in the latter strings at
                // the character following the substring). Note that
                // the unused remainder of __PRETTY_FUNCTION__ or
                // __FUNCSIG__ within the compiled binary also becomes
                // a waste of space (since the caller is only ever
                // targeting the type name within these strings).
                // Contrast this behavior with the usual default
                // behavior described above, where the only string
                // stored in the binary is "m_TypeName" (always NULL
                // terminated), and this only consumes the actual
                // (minimal) required space to store the type name
                // (plus a NULL terminator). The returned
                // "tstring_view" targets that instead so most users
                // should stick with this behavior and not #define
                // TYPENAME_V_DONT_MINIMIZE_REQD_SPACE unless there's
                // a compelling reason to (but if you do, it's no
                // longer a null-terminated string as noted so
                // exercise caution).
                ///////////////////////////////////////////////////////
                #if !defined(TYPENAME_V_DONT_MINIMIZE_REQD_SPACE)
                    //////////////////////////////////////////////////////
                    // Return static member "m_TypeName" (a "std::array"
                    // storing the type name) as a "tstring_view" (latter
                    // more convenient for end-users to work with than a
                    // "std::array"). Note that current C++ rules don't
                    // allow static objects to be defined in constexpr
                    // functions so we can't define it here (within this
                    // function). Static class members *are* allowed
                    // however so we do that instead.
                    //////////////////////////////////////////////////////
                    return TypeNameArrayToStringView();
                #else
                    /////////////////////////////////////////////////
                    // Extract template arg "T" (whatever string it
                    // resolves to) from __PRETTY_FUNCTION__ or (for
                    // MSFT only) __FUNCSIG__. Returns it as a
                    // "tstring_view" which is safe to return since
                    // the above strings remain alive for the life
                    // of the app (since they're always statically
                    // defined).
                    /////////////////////////////////////////////////
                    return ExtractTypeNameFromPrettyFunction();
                #endif // !defined(TYPENAME_V_DONT_MINIMIZE_REQD_SPACE)
            }

        private:
            using BaseClass = TypeNameImplBase;

            static constexpr tstring_view ExtractTypeNameFromPrettyFunction() noexcept
            {
                return BaseClass::ExtractTypeNameFromPrettyFunction(GetPrettyFunction<T>());
            }

            ///////////////////////////////////////////////
            // Normally tests true (constant not #defined
            // by default). See the same call in "Get()"
            // function above for details (the comments
            // there)
            ///////////////////////////////////////////////
            #if !defined(TYPENAME_V_DONT_MINIMIZE_REQD_SPACE)
                static constexpr tstring_view TypeNameArrayToStringView() noexcept
                {
                    ////////////////////////////////////////////
                    // Return a "tstring_view" wrapping our
                    // "m_TypeName" array member but
                    // subtracting 1 from the array's size so
                    // the NULL terminator isn't included in
                    // the returned "tstring_view" (i.e., its
                    // "size()" member doesn't include it).
                    // However, it's still always present so
                    // users can safely depend on the "data()"
                    // member of the returned "tstring_view"
                    // always pointing to a NULL terminated
                    // string (should they require this). The
                    // "size()" member of the returned
                    // "tstring_view doesn't include the NULL
                    // terminator however, as would normally be
                    // expected (it returns the length of the
                    // actual string only).
                    ////////////////////////////////////////////
                    return tstring_view(m_TypeName.data(), m_TypeName.size() - 1);
                }

                static constexpr auto InitTypeName() noexcept
                {
                    constexpr tstring_view typeName = ExtractTypeNameFromPrettyFunction();

                    ////////////////////////////////////////////////
                    // Create a "std::array" from "typeName" (same
                    // size as "typeName" plus 1 for the NULL
                    // terminator the following function always
                    // manually adds - "typeName" itself is copied
                    // into the array and it's then NULL
                    // terminated). Note that having to pass
                    // "typeName.size()" as a template arg is ugly
                    // given that the function can just call this
                    // itself, but not in a "constexpr" context
                    // which we require (since C++ doesn't support
                    // "constexpr" function parameters at this
                    // writing - see function for details).
                    ////////////////////////////////////////////////
                    return StrToNullTerminatedArray<typeName.size()>(typeName);
                }

                ////////////////////////////////////////////////////////////
                // "auto" resolves to a "std::array" of tchar that stores
                // the type name, where the array's size is just large
                // enough to hold it (but we also add a NULL terminator
                // since some users might require it). If template parameter
                // "T" is an "int" for instance so its length is 3, then
                // "m_TypeName" will be "std::array<tchar, 4>" (one extra
                // character added for the NULL terminator).
                //
                // Note: This member is implicitly "inline". Standard
                // currently states:
                //
                //    "A function or static data member declared with the
                //    constexpr specifier is implicitly an inline function
                //    or variable"
                ////////////////////////////////////////////////////////////
                static constexpr auto m_TypeName = InitTypeName();
            #endif // #if !defined(TYPENAME_V_DONT_MINIMIZE_REQD_SPACE)
        }; // class TypeNameImpl

        static_assert(IsEqualTo(TypeNameImpl<float>::Get(), _T("float")),
                      "A breaking change was detected in template \"TypeNameImpl::Get()\". The format of the "
                      "predefined string __PRETTY_FUNCTION__ or (when _MSC_VER is #defined) __FUNCSIG__ was "
                      "likely changed by the compiler vendor (though it would be very rare). \"TypeNameImpl::Get()\" "
                      "was (arbitrarily) tested with type float but the returned string isn't \"float\" and "
                      "normally should be. The format of __PRETTY_FUNCTION__ (or __FUNCSIG__) was therefore "
                      "(likely) changed since \"TypeNameImpl::Get()\"was written, so its implementation should "
                      "be reviewed and corrected.");
    } // namespace Private

    ////////////////////////////////////////////////////////////////////////
    // TypeName_v. Variable template that returns the literal name of the
    // given template arg T as a compile-time string, suitable for display
    // purposes (WYSIWYG). You can pass any type for T that is to return
    // its name as a "tstring_view". This is always a "std::string_view" on
    // non-MSFT platforms in this release ("tstring_view" always resolves
    // to "std::string_view"), and normally "std::wstring" on MSFT
    // platforms (or more accurately on any compiler that #defines _MSC_VER
    // when they're targeting Windows code). Note that MSFT platforms are
    // normally compiled for UTF-16 based on the #defined Microsoft
    // constants UNICODE and _UNICODE (see these if you're not already
    // familiar), but if your code is in fact compiled for ANSI instead (on
    // MSFT platforms), though it would be very rare these days, then
    // "tstring_view" will correctly resolve to "std::string_view" which we
    // still support. It *always* resolves to "std::string_view" on
    // non-MSFT platforms however (in this release as noted), since
    // supporting anything else means we'd have to deal with the overhead
    // of character conversions (if "wchar_t" or some other character type
    // had to be supported - easier on MSFT platforms only for now, at
    // least for Unicode vs non-Unicode builds, though in reality Unicode
    // builds are almost always the norm there). We can always review the
    // situation for other platforms in a future release if ever required.
    //
    //     EXAMPLE 1
    //     ---------
    //     ///////////////////////////////////////////////////////////
    //     // Returns "float" as a "tstring_view" (literally, quotes
    //     // not included).
    //     ///////////////////////////////////////////////////////////
    //     constexpr tstring_view typeName = TypeName_v<float>;
    //
    //     EXAMPLE 2
    //     ---------
    //     ////////////////////////////////////////////////////////////////
    //     // A type like "std::wstring" produces a far more complicated
    //     // name than the "float" example above of course. For example
    //     // (for all 4 compilers we currently support shown - note that
    //     // the surrounding quotes below are just for legibility and not
    //     // actually returned):
    //     //
    //     //   GCC
    //     //   ---
    //     //   "std::basic_string<wchar_t>" if "-D _GLIBCXX_USE_CXX11_ABI=0" macro is in effect (see https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html)
    //     //   "std::__cxx11::basic_string<wchar_t>" if above macro is set to 1 instead (again, see above link)
    //     //
    //     //   Microsoft
    //     //   ---------
    //     //   "class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t> >"
    //     //
    //     //   Clang and Intel
    //     //   ---------------
    //     //   "std::basic_string<wchar_t>"
    //     ////////////////////////////////////////////////////////////////
    //     constexpr tstring_view typeName = TypeName_v<std::wstring>;
    //
    // Note that this template simply extracts the type name from the
    // predefined strings __PRETTY_FUNCTION__ (for non-MSFT compilers), or
    // __FUNCSIG__ (when _MSC_VER is #defined), noting that these may or
    // may not be defined as a macro or even a string literal depending on
    // the compiler. __FUNCSIG__ is always officially a string literal in
    // VC++ however or specifically treated like one by Clang and Intel,
    // even though it's not a string literal in those compilers (a special
    // fix to treat it like one was implemented in those compilers
    // however). __PRETTY_FUNCTION__ however is not a string literal on
    // all compilers we currently support. See here for GCC for instance
    // https://gcc.gnu.org/onlinedocs/gcc/Function-Names.html).
    //
    // Whether it's a string literal or not doesn't matter for our
    // purposes however since we correctly extract the type name portion
    // from the string regardless (corresponding to template arg "T"),
    // returning this as a "tstring_view". Note that there are no lifetime
    // worries with the returned "tstring_view" since by default we store
    // the name of the type in a static string just big enough to hold it
    // (so it remains alive for the life of the app). This behavior can
    // be disabled however by #defining
    // TYPENAME_V_DONT_MINIMIZE_REQD_SPACE but there's no normally no
    // reason to do so unless a compiler error prevents the current code
    // from compiling for some reason (which should never happen on
    // currently supported compilers normally but we support this constant
    // for future use). If #defined then the returned "tstring_view" will
    // point directly into __PRETTY_FUNCTION__ instead (or __FUNCSIG__
    // when _MSC_VER is #defined), but these strings are also statically
    // defined so there are still no lifetime issues. The "tstring_view"
    // returned is just a view into these static strings which remain
    // alive for the life of the app. Note TYPENAME_V_DONT_MINIMIZE_REQD_SPACE
    //
    // isn't #defined by default since it's more space efficient not to.
    // When #defined it means that all of __PRETTY_FUNCTION__ or
    // __FUNCSIG__ will be included in the compiled binary even though we
    // only access the name portion of it ("TypeName_v" just returns a
    // "tstring_view" pointing to the type name withing these strings).
    // Since TYPENAME_V_DONT_MINIMIZE_REQD_SPACE isn't #defined by default
    // it means that __PRETTY_FUNCTION__ or __FUNCSIG__ will no longer be
    // stored in the compiled binary, only a copy of the type name itself
    // in its own static buffer (normally reducing the space of your
    // compiled binary though unless you call "TypeName_v" with a lot of
    // different types, the savings in space is usually negligible).
    //
    // Lastly, note that the techniques used to extract the type are based
    // on the undocumented format of __PRETTY__FUNCTION and __FUNCSIG__ so
    // there's always the potential they may fail one day (if a compiler
    // vendor ever changes the format or we encounter some previously
    // unknown situation with the existing format, again, since they're
    // not officially documented). However, in practice they're stable,
    // and the techniques we rely on are generally used by many others as
    // well, using similar approaches (though unlike many other
    // implementations, our own implementation isn't vulnerable to changes
    // in the name of the implementation function the following variable
    // relies on - it can be changed to anything without breaking things
    // but read on). Moreover, a "static_assert" just after the
    // implementation class itself ("Private::TypeNameImpl") checks if it
    // actually works for type float as a quick and dirty (but normally
    // reliable) test, i.e., the function should return "float" if passed
    // float as its template arg (arbitrarily chosen for testing but any
    // type whose name is the same on all supported compilers will do -
    // see comments preceding "Private::TypeNameImplBase::GetPrettyFunction"
    // above for further details). If not then the "static_assert" kicks
    // in with an appropriate message (and likely other "static_asserts"
    // in the implementation code itself). Note that I've also coded
    // things not to rely on anything in the fully-qualified name (or
    // return type) of __PRETTY_FUNCTION__ and __FUNCSIG___ themselves
    // (beyond what's necessary), as many implementations of this
    // (general) technique do. Doing so is brittle should a user change
    // anything in the function's signature, such as its name and/or
    // return type (as some users might do). This would break many
    // implementations even though most will (hopefully) be easy to fix.
    // Ours isn't sensitive to the function's signature by design however,
    // again, beyond the minimal requirements necessary for the code to
    // work (given that we're dealing with undocumented strings). Changing
    // the function's fully-qualified name for instance (the actual
    // function name, its member class name, or namespace), and/or its
    // return type (to "tstring_view" or "std::basic_string<tchar>" for
    // instance) won't break the function. Some users may not like the
    // "auto" return type for example and may want to change it to
    // "tstring_view" (declared in "CompilerVersions.h" though this is a
    // more MSFT-centric technique due to their historical use of TCHAR
    // which you can read up on if you're not familiar), or
    // "std::basic_string_view<char>" (or when replacing "char" with TCHAR
    // on MSFT platforms, hence resolving to "wchar_t" usually), or
    // "std::string_view" or "std::wstring_view" (the latter only on MSFT
    // platforms at this writing)
    //
    // The bottom line is that you can change our implementation
    // function's signature if you wish and it will (normally) still work
    // correctly. Note that if a given compiler vendor ever changes the
    // format of __PRETTY_FUNCTION__ itself however (or __FUNCSIG__ when
    // _MSC_VER is #defined), then that could potentially break the
    // function as noted but again, the "static_asserts" in the
    // implementation will normally trigger if this occurs, and you'll
    // then have to fix the implementation function for this template to
    // deal with it ("Private::TypeNameImpl::Get()" as it's currently
    // called at this writing). It seems very unlikely a compiler vendor
    // will ever change the format however, let alone in a way that will
    // break this function, unless it's done to deal with some new C++
    // feature perhaps but who knows.
    //
    // Long story short, our implementation function should normally be
    // very stable but compilation will (normally) fail with an
    // appropriate error if something goes wrong (again, via the
    // "static_asserts" in the private implementation function called by
    // this template).
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr tstring_view TypeName_v = Private::TypeNameImpl<T>::Get();
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    // See this #defined constant for details
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept IsFunction_c = std::is_function_v<T>;
        #endif

        #define IS_FUNCTION_C StdExt::IsFunction_c
        #define STATIC_ASSERT_IS_FUNCTION(T)
    #else
        #define IS_FUNCTION_C typename
        #define STATIC_ASSERT_IS_FUNCTION(T) static_assert(std::is_function_v<T>, \
                                                           QUOTE_STRINGIFY(T) " must be a function type " \
                                                           "(i.e., it must satisfy \"std::is_function\")");
    #endif // #if defined(USE_CONCEPTS)

    // See this #defined constant for details
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept IsClass_c = std::is_class_v<T>;
        #endif

        #define IS_CLASS_C StdExt::IsClass_c
        #define STATIC_ASSERT_IS_CLASS(T)
    #else
        #define IS_CLASS_C typename
        #define STATIC_ASSERT_IS_CLASS(T) static_assert(std::is_class_v<T>, \
                                                        QUOTE_STRINGIFY(T) " must be a class or struct " \
                                                        "(i.e., it must satisfy \"std::is_class\"). This " \
                                                        "normally occurs when invoking any template in " \
                                                        "this library requiring a class or struct type for " \
                                                        "one of its template args and you've passed an " \
                                                        "invalid type instead (a type that doesn't satisfy " \
                                                        "\"std::is_class\").");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    template <typename T>
    inline constexpr bool IsClassOrVoid_v = std::is_class_v<T> || std::is_void_v<T>;
#endif // !defined(DECLARE_PUBLIC_MACROS_ONLY)

    // See this #defined constant for details
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept IsClassOrVoid_c = IsClassOrVoid_v<T>;
        #endif

        #define IS_CLASS_OR_VOID_C StdExt::IsClassOrVoid_c
        #define STATIC_ASSERT_IS_CLASS_OR_VOID(T)
    #else
        #define IS_CLASS_OR_VOID_C typename
        #define STATIC_ASSERT_IS_CLASS_OR_VOID(T) static_assert(StdExt::IsClassOrVoid_v<T>, \
                                                                QUOTE_STRINGIFY(T) " must either be a class type " \
                                                                "or void (i.e., it must satisfy \"std::is_class\" " \
                                                                "or \"std::is_void\"). This error normally occurs " \
                                                                "when invoking any template in this library requiring " \
                                                                "either a class type or void for one of its template " \
                                                                "args, normally \"ReplaceMemberFunctionClass_t\" " \
                                                                "(its \"NewClassT\" template arg must be a class " \
                                                                "type or void), and you've passed an invalid type " \
                                                                "instead (a type that doesn't satisfy \"std::is_class\" " \
                                                                "or \"std::is_void\").");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    namespace Private
    {
        template <typename T>
        using Type_t = T;
    }

    ////////////////////////////////////////////////////////////////////////////
    // IsConstOrVolatile_v. WYSIWYG. Note that the negation means "T" is *not*
    // "const" or "volatile" (neither qualifier present - again, WYSIWYG)
    ////////////////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsConstOrVolatile_v = std::is_const_v<T> ||
                                                std::is_volatile_v<T>;

    /////////////////////////////////////////////////////////////////////////////
    // IsSpecialization. Primary template. See partial specialization just below
    // for details.
    /////////////////////////////////////////////////////////////////////////////
    template <typename T,
              template<typename...> class,
              typename = std::remove_cv_t<T>> // Arg for internal use only (users
                                              // should never explicitly pass it)
    struct IsSpecialization : std::false_type
    {
    };

    /////////////////////////////////////////////////////////////////////////////
    // Partial specialization of (primary) template just above. The following
    // kicks in when the 1st template arg in the primary template above (a type)
    // is an (optionally cv-qualified) specialization of the 2nd template arg (a
    // template), though derivatives of such specializations aren't supported
    // (they will always return false). IOW, this partial specialization kicks
    // in if the 1st template arg (a type) is a type created from a template
    // given by the 2nd template arg (a template). If not then the primary
    // template kicks in above instead (i.e., when the 1st template arg (a type)
    // isn't a type created from the template given by the 2nd template arg (a
    // template), meaning it's not a specialization of that template). Note that
    // the "Template" parameter can be any template taking type args only (any
    // number of them). If "Template" takes any non-type args however then the
    // primary template will always kick in instead (so it always inherits from
    // "std::false_type"). Handling a mixture of both type and non-type template
    // args in "Template" isn't possible in C++ at this writing (in a generic
    // template like "IsSpecialization" AFAIK). If you need to detect such
    // specializations then you'll have to write a custom template to handle it
    // (for your particular use case).
    //
    // IMPORTANT:
    // ---------
    // As noted above, "T" may be "const" and/or "volatile" (the specialization
    // kicks in regardless).
    //
    //    Example
    //    -------
    //    template <typename T>
    //    class Whatever
    //    {
    //       //////////////////////////////////////////////////////
    //       // Make sure type "T" is a "std::vector" (relying on
    //       // our "IsSpecialization_v" helper variable however
    //       // declared just after "IsSpecialization" below)
    //       //////////////////////////////////////////////////////
    //       static_assert(IsSpecialization_v<T, std::vector>
    //                     "Invalid template arg T. Must be a \"std::vector\"");
    //    };
    //
    //    Whatever<std::vector<int>> whatever1; // "static_assert" above succeeds (we're passing a "std::vector")
    //    Whatever<std::list<int>> whatever2; // "static_assert" above fails (we're not passing a "std::vector")
    /////////////////////////////////////////////////////////////////////////////
    template <typename T,
              template<typename...> class Template,
              typename... ArgsT>
    struct IsSpecialization<T,
                            Template,
                            Template<ArgsT...>
                           > : std::true_type
    {
    };

    //////////////////////////////////////////////////////////
    // Helper variable template for "IsSpecialization" above
    // (with the usual "_v" suffix). Set to true if "T" is
    // a specialization of the given "Template" or false
    // otherwise. See "IsSpecialization" above for details.
    //////////////////////////////////////////////////////////
    template <typename T,
              template<typename...> class Template>
    inline constexpr bool IsSpecialization_v = IsSpecialization<T, Template>::value;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

#if defined(USE_CONCEPTS)
    #define REQUIRES_IS_SPECIALIZATION(T, TEMPLATE) requires StdExt::IsSpecialization_v<T, TEMPLATE>
#else
    #define REQUIRES_IS_SPECIALIZATION(T, TEMPLATE)
#endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    /////////////////////////////////////////////////////////////////
    // TemplateArgsTuple - Primary template but should never kick in
    // unless "T" isn't a specialization of "Template", which is
    // illegal for the purposes of this template (so the
    // REQUIRES_IS_SPECIALIZATION concept seen just below will kick
    // in in C++20 or later, or the "static_assert" below in C++17
    // otherwise. If "T" *is* a specialization of "Template" however
    // and it normally will be then our specialization below kicks
    // in instead (see comments for that below).
    /////////////////////////////////////////////////////////////////
    template <typename T,
              template<typename...> class Template,
              typename = std::remove_cv_t<T>> // Arg for internal use only (users
                                              // should never explicitly pass it)
    REQUIRES_IS_SPECIALIZATION(T, Template)
    struct TemplateArgsTuple
    {
        // See this constant for details
        #if !defined(USE_CONCEPTS)
            //////////////////////////////////////////////////
            // Kicks in if concepts not supported, otherwise
            // corresponding concept kicks in in the template
            // declaration above instead (macro for this
            // above resolves to whitespace when concepts
            // aren't supported)
            //////////////////////////////////////////////////
            static_assert(AlwaysFalse_v<T>,
                          "Template arg \"T\" (a type), is not a specialization "
                          "of template arg \"Template\" (a template). This is "
                          "illegal for the purposes of this template "
                          "(TemplateArgsTuple).");
        #endif
    };

    /////////////////////////////////////////////////////////////////
    // TemplateArgsTuple - Specialization that kicks in if the 1st
    // template arg, "T", is a specialization of the 2nd template
    // arg, "Template", where "Template" is any template taking one
    // or more type args (not non-type args - if any are present in
    // "Template" then the primary template of "TemplateArgsTuple"
    // will kick in instead). If "T" is a specialization of
    // "Template" as described then this "TemplateArgsTuple"
    // specialization kicks in and its "ArgsTuple_t" member (alias)
    // will be set to the "std::tuple" of all template args that "T"
    // was specialized on.
    //
    // For instance, given the following:
    //
    //   using VectorArgsTuple = TemplateArgsTuple_t< std::vector<int>, // Specialization of template just below
    //                                                std::vector>;
    //
    // Yields this:
    //
    //    std::tuple< int, std::allocator<int> >
    //
    // The above "std::tuple" is simply specialized on the template
    // args of "std::vector<int>" itself. Note that in this case,
    // the 2nd template arg of the above "std::tuple" is a
    // "std::allocator<int>" because it's the 2nd template arg of
    // "std::vector<int>" (the default template arg of
    // "std::vector"). See the "std::vector" docs for details (its
    // 2nd template arg defaults to a "std::allocator" of its 1st
    // template arg, where its 1st template arg is an "int" in this
    // example so it defaults to "std::allocator<int>").
    /////////////////////////////////////////////////////////////////
    template <typename T,
              template<typename...> class Template,
              typename... ArgsT>
    struct TemplateArgsTuple<T,
                             Template,
                             Template<ArgsT...>
                            >
    {
        using ArgsTuple_t = std::tuple<ArgsT...>;
    };

    /////////////////////////////////////////////////////////////////
    // Helper alias for the above template
    /////////////////////////////////////////////////////////////////
    template <typename T,
              template<typename...> class Template>
    REQUIRES_IS_SPECIALIZATION(T, Template)
    using TemplateArgsTuple_t = typename TemplateArgsTuple<T, Template>::ArgsTuple_t;

    template <std::size_t I,
              typename T,
              template<typename...> class Template>
    ////////////////////////////////////////////////////////////////
    // TODO - Tighten the following up in a future release to trap
    // invalid values of "I" as well (very low priority right now).
    // In the current release, all supported compilers will trigger
    // a "static_assert" in the call to "std::tuple_element_t" just
    // below instead (an implementation detail but testing shows
    // that's what they all currently do, and probably unlikely to
    // ever change - this is fine for our purposes for now since
    // the library itself never passes illegal values for "I" so
    // unless a user calls us with an illegal value of "I" then
    // we're safe - passing an illegal value for "I" therefore
    // isn't supported and results in a hard compiler error even in
    // an SFINAE context but in a future release would be a good
    // idea to trap it).
    ////////////////////////////////////////////////////////////////
    REQUIRES_IS_SPECIALIZATION(T, Template)
    using IthTemplateArg_t = std::tuple_element_t<I, TemplateArgsTuple_t<T, Template> >;

    template <typename T,
              template<typename...> class Template>
    REQUIRES_IS_SPECIALIZATION(T, Template)
    using FirstTemplateArg_t = IthTemplateArg_t<0, T, Template>;

    ///////////////////////////////////////////////////////////
    // Degenerate case of "FirstTemplateArg_t" just above
    // where in the following template the "Template" template
    // arg takes a single type arg only (unlike
    // "FirstTemplateArg_t" whose "Template" template arg can
    // take any number of template args - "FirstTemplateArg_t"
    // and the following template are identical otherwise)).
    // This template is therefore more suitable than calling
    // "FirstTemplateArg_t" directly when the "Template"
    // template arg is a template taking a single template arg
    // only. While "FirstTemplateArg_t" can also be called
    // instead of the following template and will yield the
    // same result (which is why the following template just
    // defers to "FirstTemplateArg_t"), the following template
    // makes it explicitly clear that the "Template" template
    // arg you pass must be a template taking a single
    // template arg only. It's therefore cleaner to call the
    // following instead of "FirstTemplateArg_t" in this case.
    ///////////////////////////////////////////////////////////
    template <typename T,
              template<typename> class Template>
    REQUIRES_IS_SPECIALIZATION(T, Template)
    using TemplateArg_t = FirstTemplateArg_t<T, Template>;

    //////////////////////////////////////////////////////////////////
    // For internal use only (implementation of "IsTemplateBaseOf_v"
    // declared just after this namespace - see this for details)
    //////////////////////////////////////////////////////////////////
    namespace Private
    {
        template <template<typename...> class BaseTemplateT,
                  typename DerivedT>
        struct IsTemplateBaseOfImpl
        {
        private:
            template <typename... ArgsT>
            static constexpr std::true_type Test(const BaseTemplateT<ArgsT...> *);

            static constexpr std::false_type Test(...);

            template <typename DerivedU,
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct Process : std::false_type
            {
            };

            /////////////////////////////////////////////////////////////////////
            // Note: This specialization can't rely on the "FunctionTraitsBase"
            // template args directly in the following template header (which
            // would be the more straightforward way of implementing it) since
            // C++ can't deduce the types this way (using any mainstream
            // technique I've tried). It results in errors similar to the
            // following (a MSFT error but all other supported compilers result
            // in their own similar warnings):
            //
            //   error C2764: 'F': template parameter not used or deducible in
            //   partial specialization
            //   'StdExt::Private::IsFunctionTraitsBase<T>::Process<U,void>'
            //
            // The same error is repeated for every template arg of
            // "FunctionTraitsBase" (none of them can be deduced when referring
            // to them in any "std::void_t" or "std::enable_if_t call below).
            // The following technique works instead (though not as clean as the
            // above technique would be if it were supported).
            /////////////////////////////////////////////////////////////////////
            template <typename DerivedU>
            struct Process <DerivedU,
                            ////////////////////////////////////////////////////////////////////
                            // Invokes "Test" above, passing it a "U *" so the "Test" overload
                            // returning "std::true_type" will kick in if "U" itself is a
                            // "BaseTemplate" specialization or derivative of one. The
                            // following "Process" specialization will therefore kick in which
                            // inherits from "std::true_type" to indicate this (that "U" is a
                            // "BaseTemplate" specialization or derivative). OTOH, the variadic
                            // overload of "Test" above will kick in instead if "U" isn't a
                            // "BaseTemplate" specialization or derivative, and that overload
                            // returns "std::false_type" so SFINAE will therefore fail in the
                            // following call. The primary template of "Process" will therefore
                            // kick in which inherits from "std::false_type" indicating that
                            // "U" isn't a "BaseTemplate" specialization or derivative. This is
                            // the usual reason SFINAE fails, because the variadic version of
                            // "Test" above is invoked (meaning "U" isn't a "BaseTemplate"
                            // specialization or derivative), but SFINAE can fail for other
                            // possible reasons as well (though rare in practice). If "U" is a
                            // "BaseTemplate" derivative for instance but was derived from
                            // "BaseTemplate" using the "private" or "protected" keyword, then
                            // the following will also fail SFINAE simply because the
                            // "BaseTemplate" base class is inaccessible (so it can't invoke
                            // the version of "Test" returning "std::true_type" above because
                            // of the "inaccessibility" issue which will therefore cause SFINAE
                            // to fail here). Unfortunately this means that the primary
                            // template will kick in indicating that "U" isn't a "BaseTemplate"
                            // derivative even though it is (the "BaseTemplate" base class of
                            // "U" simply isn't accessible). In reality this scenario is
                            // extraordinarily unlikely to ever occur however so we won't worry
                            // about it for now (the library itself never creates this
                            // situation so an end-user would have to explicitly create it -
                            // very unlikely). Unfortunately it's difficult to circumvent
                            // however so we'll live with the limitation for now. Trying to
                            // re-factor this code to rely on a more mainstream and (always)
                            // reliable technique based on passing "U" to "std::is_base_of"
                            // proved to be difficult because of C++ issues deducing the
                            // template args of "BaseTemplate" - longer story but I couldn't
                            // find a way to do it (to my surprise). The call to "Test" below
                            // is the best technique I could find and while it will be reliable
                            // under almost all real-world situation, it remains imperfect for
                            // the reason just described. In any case, SFINAE can also fail in
                            // the following call for other (rare) reasons as well. "U" may not
                            // support pointers for instance, i.e., "U *" might be illegal, in
                            // particular if "U" is an abominable function, i.e., a native C++
                            // function type with the "const" and/or "volatile" and/or & or &&
                            // qualifiers. Pointers to such functions are illegal in C++ so the
                            // following will fail SFINAE for that reason (since "U *" itself
                            // is illegal when "U" is an abominable function - our attempt to
                            // create "U *" below will therefore fail). That's fine for our
                            // purposes however since "U" isn't a "BaseTemplate" specialization
                            // or derivative in this case anyway. It's an abominable function
                            // type or possibly some other type that doesn't support pointers
                            // though such types are very rare in C++. Our primary template
                            // therefore correctly kicks in in such cases which correctly
                            // inherits from "std::false_type" since such types are never
                            // "BaseTemplate" specializations or derivatives anyway (i.e., if
                            // "U" is a "BaseTemplate" specialization or derivative then since
                            // "U" is just a struct then "U *" itself is always legal).
                            ////////////////////////////////////////////////////////////////////
                            std::enable_if_t< decltype(Test(std::declval<DerivedU *>()))::value >
                            >
                            : std::true_type
            {
            };

        public:
            static constexpr bool Value = Process<std::remove_cv_t<DerivedT>>::value;
        };
    }

    template <template<typename...> class BaseTemplateT,
              typename DerivedT>
    inline constexpr bool IsTemplateBaseOf_v = Private::IsTemplateBaseOfImpl<BaseTemplateT, DerivedT>::Value;

    ///////////////////////////////////////////////////////
    // "std::remove_cvref_t" not available until C++20 so
    // we roll our own for C++17 or earlier (and name it
    // using our normal Pascal casing to prevent any
    // confusion and/or collision with the C++20 version,
    // even though we defer to it in C++20 and later)
    ///////////////////////////////////////////////////////
    #if CPP20_OR_LATER
        template <typename T>
        using RemoveCvRef_t = std::remove_cvref_t<T>;
    #else
        template <typename T>
        using RemoveCvRef_t = std::remove_cv_t<std::remove_reference_t<T>>;
    #endif

    template <typename T>
    using RemovePtrRef_t = std::remove_pointer_t<std::remove_reference_t<T>>;

    ////////////////////////////////////////////////////////////
    // Equivalent of "std::basic_string::npos" among a few
    // other occurrences of "npos" in the C++ standard, but no
    // generic version of it exists at namespace scope so we
    // create our own here. Note the cast from -1 below is
    // crude but legal (and used in the C++ standard itself).
    // Alternatively we could rely on
    // "std::numeric_limits<std::size_t>::max()" but we'll just
    // stick with the following (it's more recognizable to
    // most)...
    ////////////////////////////////////////////////////////////
    inline constexpr std::size_t size_t_npos = static_cast<std::size_t>(-1);

    template <std::size_t Val1, std::size_t Val2>
    inline constexpr bool LessThanOrEqual_v = (Val1 <= Val2);
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <std::size_t Val1, std::size_t Val2>
            concept LessThanOrEqual_c = LessThanOrEqual_v<Val1, Val2>;
        #endif

        #define REQUIRES_LESS_THAN_OR_EQUAL_C(VAL1, VAL2) requires StdExt::LessThanOrEqual_c<VAL1, VAL2>
        #define STATIC_ASSERT_LESS_THAN_OR_EQUAL(VAL1, VAL2)
        #define STATIC_ASSERT_LESS_THAN_OR_EQUAL_F(VAL1, VAL2)
    #else
        #define REQUIRES_LESS_THAN_OR_EQUAL_C(VAL1, VAL2)
        #define STATIC_ASSERT_LESS_THAN_OR_EQUAL(VAL1, VAL2) static_assert(StdExt::LessThanOrEqual_v<VAL1, VAL2>, \
                                                                           QUOTE_STRINGIFY(VAL1) " must be less than " \
                                                                           "or equal to " QUOTE_STRINGIFY(VAL2));

        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <std::size_t Val1,
                      std::size_t Val2>
            inline constexpr void StaticAssertLessThanOrEqual() noexcept
            {
                STATIC_ASSERT_LESS_THAN_OR_EQUAL(Val1, Val2)
            }
        #endif
        #define STATIC_ASSERT_LESS_THAN_OR_EQUAL_F(VAL1, VAL2) StdExt::StaticAssertLessThanOrEqual<VAL1, VAL2>()
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    template<std::size_t Begin, // Inclusive
             std::size_t End> // Not inclusive (so 1 beyond the actual end in the usual C++ way)
    REQUIRES_LESS_THAN_OR_EQUAL_C(Begin, End)
    inline constexpr std::size_t CountRemaining_v = STATIC_ASSERT_V(STATIC_ASSERT_LESS_THAN_OR_EQUAL_F(Begin, End),
                                                                    (End - Begin));

    template<std::size_t Begin, // Inclusive
             std::size_t End, // Not inclusive (so 1 beyond the actual end in the usual C++ way)
             std::size_t Count>
    REQUIRES_LESS_THAN_OR_EQUAL_C(Begin, End)
    inline constexpr bool CountExceedsRemaining_v = STATIC_ASSERT_V(STATIC_ASSERT_LESS_THAN_OR_EQUAL_F(Begin, End),
                                                                    (Count > CountRemaining_v<Begin, End>));

    template<std::size_t Begin, // Inclusive
             std::size_t End, // Not inclusive (so 1 beyond the actual end in the usual C++ way)
             std::size_t Count = size_t_npos>
    REQUIRES_LESS_THAN_OR_EQUAL_C(Begin, End)
    inline constexpr std::size_t MinOfCountOrRemaining_v = STATIC_ASSERT_V(STATIC_ASSERT_LESS_THAN_OR_EQUAL_F(Begin, End),
                                                                           (std::min(Count, CountRemaining_v<Begin, End>)));

    ////////////////////////////////////////////////////////////////////////
    // For internal use only (implementation of "MakeIndexSequenceStartAt"
    // declared just after this namespace - see this for details)
    ////////////////////////////////////////////////////////////////////////
    namespace Private
    {
        template <std::size_t Begin, std::size_t Size>
        #if defined(USE_CONCEPTS)
            requires (!CountExceedsRemaining_v<Begin, size_t_npos, Size>)
        #endif
        class MakeIndexSequenceStartAtImpl
        {
            #if !defined(USE_CONCEPTS)
                ///////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concept kicks in in the template
                // declaration above instead
                ///////////////////////////////////////////////////
                static_assert(!CountExceedsRemaining_v<Begin, size_t_npos, Size>);
            #endif

            template <std::size_t... Indices>
            static constexpr auto Process(std::index_sequence<Indices...>) -> std::index_sequence<(Begin + Indices)...>;

        public:
            using Type = decltype(Process(std::make_index_sequence<Size>()));
        };
    }

    ////////////////////////////////////////////////////////////////////////
    // MakeIndexSequenceStartAt. Identical to "std::make_index_sequence"
    // but the returned "std::index_sequence" starts at "Begin" instead of
    // zero (0).
    //
    // E.g.,
    //
    //    //////////////////////////////////////////////////////////
    //    // Yields the following (a "std::index_sequence" starting
    //    // at 3 for a length of 5):
    //    //
    //    //    std::integer_sequence<std::size_t, 3, 4, 5, 6, 7>
    //    //////////////////////////////////////////////////////////
    //    using IndexSequenceStartAt3Size5 = MakeIndexSequenceStartAt<3, 5>;
    //
    //    ////////////////////////////////////////////////////////
    //    // Yields an empty sequence because "Size" is zero (0):
    //    //
    //    //    std::integer_sequence<std::size_t>
    //    ////////////////////////////////////////////////////////
    //    using EmptyIndexSequence = MakeIndexSequenceStartAt<3, 0>;
    //////////////////////////////////////////////////////////////////////
    template <std::size_t Begin,
              std::size_t Size>
    using MakeIndexSequenceStartAt = typename Private::MakeIndexSequenceStartAtImpl<Begin, Size>::Type;

    ////////////////////////////////////////////////////////////////////////////
    // "IsForEachFunctor" (primary template). Determines if template arg "T" is
    // a functor type whose "operator()" member has the following signature and
    // is also callable in the context of whether "T" is "const" and/or
    // "volatile" and or an lvalue or rvalue (read on):
    //
    //     template <std::size_t I>
    //     bool operator()() [const] [volatile] [&|&&];
    //
    // If "T" contains this function AND it can also be called on an instance
    // of "T" (again, read on), then "T" qualifies as a functor that can be
    // passed to function template "ForEach()" declared later on. Note that the
    // specialization just below does the actual work of checking for this. It
    // simply invokes the above function template in an unevaluated context
    // using "std::declval()" as follows:
    //
    //     T::operator<0>()
    //
    // If the call succeeds then "operator()" must not only exist as a member
    // of "T" with a "bool" return type (we check for this as well), but it's
    // also callable in the context of whether "T" is "const" and/or "volatile"
    // and or an lvalue or rvalue. Note that to qualify as an lvalue for
    // purposes of "IsForEachFunctor", "T" must be an lvalue reference such as
    // "int &". To qualify as an rvalue, "T" must either be a non-reference
    // type such as a plain "int" OR an rvalue reference such as "int &&". The
    // behavior of what qualifies as an lvalue and rvalue for purposes of
    // using "IsForEachFunctor" therefore follows the usual perfect forwarding
    // rules for function template arguments (where an lvalue reference for "T"
    // means the function's "T &&" argument collapses to an lvalue reference,
    // and a non-reference type or rvalue reference type for "T" means the
    // function's "T &&" argument is an rvalue reference - read up on perfect
    // forwarding for details if you're not already familiar this).
    //
    // As an example, if "T" contains the above "operator()" template but it's
    // not declared with the "const" qualifier but "T" itself is (declared
    // "const"), then calling the function as shown above fails since you can't
    // call a non-const member function using a constant object. The primary
    // template therefore kicks in, inheriting from "std::false_type".
    // Similarly, if "operator()" is declared with the "&&" reference qualifier
    // (rare as this is in practice), but "T" is an lvalue reference, then
    // again, the above call fails since you can't invoke a member function
    // declared with the "&&" (rvalue) reference qualifier using an lvalue
    // reference. Again, the primary template will therefore kick in (once
    // again inheriting from "std::false_type"). The upshot is that not only
    // must "operator()" exist as a member of "T", it must also be callable in
    // the context of "T" itself (whether "T" is "const" and/or "volatile"
    // and/or an lvalue reference, rvalue reference or non-reference - in all
    // cases "operator()" must be properly declared to support calls based on
    // the cv-qualifiers and/or lvalue/rvalue state of "T" itself as described
    // above). If it is then the specialization kicks in and "IsForEachFunctor"
    // therefore inherits from "std::true_type" (meaning "T" is a suitable
    // functor type for passing as the 2nd template arg of function template
    // "ForEach()").
    ////////////////////////////////////////////////////////////////////////////
    template <typename T,
              typename = void> // Arg for internal use only (users should never explicitly pass it)
    struct IsForEachFunctor : std::false_type
    {
    };

    /////////////////////////////////////////////////////////////////
    // Specialization of "IsForEachFunctor" (primary) template just
    // above. This specialization does all the work. See primary
    // template above for details. Note that calls to "declval(T)"
    // return "T &&" (unless "T" is void - see "declval()" docs) so
    // the following call to this works according to the usual
    // perfect forwarding rules in C++. That is, if "T" is an lvalue
    // reference then "declval()" returns an lvalue reference (due
    // to the C++ reference collapsing rules), otherwise "T" must be
    // an a non-reference or an rvalue reference so "declval"
    // returns an rvalue reference (in either case). In all cases
    // "declval()" therefore returns the same function argument type
    // used in a perfect forwarding function. Such arguments are
    // always of the type "T &&" which resolves to an lvalue
    // reference only if "T" is an lvalue reference (again, due to
    // the C++ reference collapsing rules), or an rvalue reference
    // if "T" is a non-reference or rvalue reference. "declval()"
    // therefore returns the exact same type as a perfectly
    // forwarded argument after plugging in "T" which is what we
    // require here (we're invoking "operator()" on that argument in
    // the following call in an unevaluated context simply to make
    // sure it can be invoked - the primary template kicks in via
    // SFINAE otherwise).
    /////////////////////////////////////////////////////////////////
    template <typename T>
    struct IsForEachFunctor<T,
                            // See explanation of "declval()" in comments above
                            std::enable_if_t< std::is_same_v< decltype(std::declval<T>().template operator()<std::size_t(0)>()),
                                                              bool
                                                            >
                                            >
                           > : std::true_type
    {
    };

    /////////////////////////////////////////////////////////
    // Helper variable template for "IsForEachFunctor" just
    // above (with the usual "_v" suffix).
    /////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsForEachFunctor_v = IsForEachFunctor<T>::value;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept ForEachFunctor_c = IsForEachFunctor_v<T>;
        #endif

        #define FOR_EACH_FUNCTOR_C StdExt::ForEachFunctor_c
        #define STATIC_ASSERT_IS_FOR_EACH_FUNCTOR(T)
    #else
        #define FOR_EACH_FUNCTOR_C typename
        #define STATIC_ASSERT_IS_FOR_EACH_FUNCTOR(T) static_assert(StdExt::IsForEachFunctor_v<T>, \
                                                                   QUOTE_STRINGIFY(T) " must be a class/struct with a non-static, " \
                                                                   "template-based function call operator similar to the following:\n" \
                                                                   "\n" \
                                                                   "template <std::size_t I>\n" \
                                                                   "bool operator()() const\n" \
                                                                   "\n" \
                                                                   "The function should return true to continue iterating or false to " \
                                                                   "stop iterating, similar to a \"break\" statement in a regular \"for\" " \
                                                                   "loop. Note that the function must return a \"bool\" but otherwise " \
                                                                   "must simply be invocable on an instance of " QUOTE_STRINGIFY(T) " (so " \
                                                                   "whether it's declared \"const\", \"noexcept\", etc. is immaterial so " \
                                                                   "long as it can be specialized on a \"std::size_t\" and invoked on an " \
                                                                   "instance of " QUOTE_STRINGIFY(T) " as noted");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    /////////////////////////////////////////////////////
    // In C++20 and later we rely on a locally defined
    // lambda declared in function template "ForEach()"
    // instead (that does the same thing as class
    // "ForEachImpl" does below in C++17 or earlier).
    /////////////////////////////////////////////////////
    #if CPP17_OR_EARLIER
        /////////////////////////////////////////////////////
        // For internal use only (by "ForEach()" just after
        // this namespace)
        /////////////////////////////////////////////////////
        namespace Private
        {
            /////////////////////////////////////////////////////////////
            // class ForEachImpl. Private implementation class used by
            // function template "ForEach()" declared just after this
            // private namespace. See this for details. Note that in the
            // following class, "ForEachFunctorT" is always the same
            // template type passed to "ForEach()", so either "T &" for
            // the lvalue case or just plain "T" or "T &&" for the rvalue
            // case (for the "rvalue" case however usually just plain
            // "T" as per the usual perfect forwarding rules when
            // invoking such functions via implicit type deduction).
            ///////////////////////////////////////////////////////////
            template <std::size_t N, typename ForEachFunctorT>
            class ForEachImpl
            {
                STATIC_ASSERT_IS_FOR_EACH_FUNCTOR(ForEachFunctorT)

            public:
                /////////////////////////////////////////////////////////
                // Constructor. Note that we always pass "functor" by a
                // "std::forward" reference so "functor" resolves to "&"
                // or "&&" accordingly
                /////////////////////////////////////////////////////////
                constexpr ForEachImpl(ForEachFunctorT&& functor) noexcept
                    ///////////////////////////////////////////////////////////
                    // "std::forward" required when "ForEachFunctorT" is "&&".
                    // Both "functor" and "m_Functor" are therefore "&&" but
                    // "functor" is still an lvalue because it's named (rvalue
                    // references that are named are lvalues!). Therefore
                    // illegal to bind an lvalue (functor) to an rvalue
                    // reference (m_Functor) so "std::forward" is required to
                    // circumvent this (for the "&&" case it returns an "&&"
                    // reference but it's now unnamed so an rvalue and therefore
                    // suitable for initializing "m_Functor")
                    ///////////////////////////////////////////////////////////
                    : m_Functor(std::forward<ForEachFunctorT>(functor))
                {
                }

                template <std::size_t I>
                constexpr bool Process() const
                {
                    if constexpr (I < N)
                    {
                        //////////////////////////////////////////////////////////
                        // Note: Call to "std::forward()" here required to:
                        //
                        //    1) Perfect forward "m_Functor" back to "&" or "&&"
                        //       accordingly
                        //    2) In the "&&" case, invoke "operator()" in the
                        //       context of an rvalue (in particular, can't do
                        //       this on "m_Functor" directly, without invoking
                        //       "std::forward", since "m_Functor" is an lvalue
                        //       so the lvalue version of "operator()" would kick
                        //       in in the following call instead!!)
                        //////////////////////////////////////////////////////////
                        if (std::forward<ForEachFunctorT>(m_Functor).template operator()<I>())
                        {
                            ////////////////////////////////////////////////
                            // Recursive call (in spirit anyway - actually
                            // stamps out and runs another version of this
                            // function specialized on I + 1)
                            ////////////////////////////////////////////////
                            return Process<I + 1>();
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return true;
                    }
                }

            private:
                /////////////////////////////////////////////////////
                // Note: Resolves to "&" in the lvalue case or "&&"
                // otherwise (by the usual C++ reference collapsing
                // rules - "ForEachFunctorT" is always "T &" in the
                // former case or either plain "T" or "T &&"
                // otherwise)
                /////////////////////////////////////////////////////
                ForEachFunctorT &&m_Functor;
            };
        } // namespace Private
    #endif // #if CPP17_OR_EARLIER

    ///////////////////////////////////////////////////////////////////
    // ForEach(). Generic function template effectively equivalent to
    // a "for" loop but used at compile time to execute the given
    // "functor" "N" times (unless the "functor" arg returns false on
    // any of those iterations in which case processing immediately
    // stops and "false" is immediately returned - read on).
    //
    // Note that "operator()" in your functor must be declared as
    // follows (and will typically be "const" as seen though it can be
    // non-const as well if required, and/or noexcept as well)
    //
    //     template <std::size_t I>
    //     bool operator()() const
    //     {
    //         // Process "I" accordingly
    //
    //         /////////////////////////////////////////////////////
    //         // "true" means continue processing so this function
    //         // will immediately be called again specialized on
    //         // "I + 1" until "N" is reached (EXclusive). Return
    //         // false instead to immediately stop processing
    //         // (equivalent to a "break" statement in a regular
    //         // "for" loop)
    //         /////////////////////////////////////////////////////
    //         return true;
    //     }
    //
    // "ForEach()" effectively stamps out "N" copies of "operator()"
    // above, once for each "I" in the range 0 to N - 1 inclusive
    // (unless "N" is zero in which case "operator()" is still stamped
    // out for "I" equal zero but never actually called - it's only
    // stamped out to validate that "ForEachFunctorT" is a valid
    // template arg but this means your functor shouldn't do anything
    // that might be illegal when "N" is zero, like invoke
    // "std::tuple_element_t" for instance, since index zero would be
    // illegal in this case so compilation will fail). In any case,
    // note that your functor should return true as seen above unless
    // you wish to "break" like in a normal "for" loop, in which case
    // it should return false. Either value (true or false) is
    // ultimately returned by the function. If true then "I" is simply
    // incremented by 1 and "operator()" is immediately called again
    // with the newly incremented "I" (stamping out a new copy of
    // "operator()" with the newly incremented "I"). If false then
    // processing immediately exits instead and no new copies of
    // "operator()" are stamped out (for "I + 1" and beyond).
    //
    //    Example (iterates all elements of a "std::tuple" but
    //             see the helper function "ForEachTupleType()"
    //             declared later which wraps the following so
    //             it's cleaner - for tuples you should rely
    //             on it instead normally but it ultimately
    //             just defers to "ForEach()" anyway)
    //    -----------------------------------------------------
    //    std::Tuple<int, float, double> myTuple; // Tuple with 3 elements whose types we wish to iterate (and display)
    //    using TupleT = decltype(myTuple); // Type of the above tuple
    //
    //    //////////////////////////////////////////////////////
    //    // Lambda (template) stamped out and invoked 3 times
    //    // by "ForEach()" below (for I=0, I=1 and I=2). See
    //    // "ForEach()" comments below.
    //    //////////////////////////////////////////////////////
    //    const auto displayTupleElement = []<std::size_t I>()
    //                                     {
    //                                         // Type of the this tuple element (for the current "I")
    //                                         using TupleElement_t = std::tuple_element_t<I, TupleT>;
    //
    //                                         /////////////////////////////////////////////////
    //                                         // Displays (literally) "int" when I=0, "float"
    //                                         // when I=1 and "double" when I=2 (for this
    //                                         // particular tuple - see "myTuple above)
    //                                         /////////////////////////////////////////////////
    //                                         std::cout << TypeName_v<TupleElement_t> << "\n";
    //
    //                                         //////////////////////////////////////////////
    //                                         // Return false here instead if you need to
    //                                         // stop processing (equivalent to "break" in
    //                                         // a normal "for" loop). Returning "true"
    //                                         // instead as we're doing here means all
    //                                         // tuple elements will be processed.
    //                                         //////////////////////////////////////////////
    //                                         return true;
    //                                     };
    //
    //     ///////////////////////////////////////////////////////////////////
    //     // Call "ForEach()" which iterates (effectively does a "foreach")
    //     // of the above lambda "N" times, where "N" is explicitly passed
    //     // as the 1st template arg to "ForEach()" (the number of elements
    //     // in our tuple, 3 in this case), and the 2nd template arg, the
    //     // type of the functor, "ForEachFunctorT", is implicitly deduced
    //     // from the arg itself. We're passing "displayTupleElement" so
    //     // the 2nd template arg is implicitly deduced to the type of this
    //     // functor, i.e., whatever class the compiler generates for the
    //     // above lambda behind the scenes (recall that a lambda is just
    //     // syntactic sugar for a compiler-generated functor - the functor's
    //     // class is called its "closure type"). The upshot is that
    //     // "operator()" in this functor (the above lambda) is effectively
    //     // stamped out 3 times and invoked for each (for I=0, I=1 and I=2)
    //     ///////////////////////////////////////////////////////////////////
    //     ForEach<std::tuple_size_v<TupleT>>(displayTupleElement);
    ///////////////////////////////////////////////////////////////////
    template <std::size_t N, FOR_EACH_FUNCTOR_C ForEachFunctorT>
    inline constexpr bool ForEach(ForEachFunctorT&& functor)
    {
        // Lambda templates not supported until C++20
        #if defined(LAMBDA_TEMPLATES_SUPPORTED)
            ////////////////////////////////////////////////////////////
            // Lambda template we'll be calling "N" times for the given
            // template arg "N" in "ForEach()" (the function you're now
            // reading). Each iteration invokes "functor", where
            // processing stops after "N" calls to "functor" or
            // "functor" returns false, whichever comes first (false only
            // returned if "functor" wants to break like a normal "for"
            // loop, which rarely happens in practice so we usually
            // iterate "N" times)
            //
            // IMPORTANT:
            // ---------
            // The parameter "functor" (we'll call this the "outer"
            // functor) is always a reference type so it's always of
            // the form "T&" or "T&&". The following lambda captures it
            // by (lvalue) reference as seen so it effectively creates
            // an (lvalue) reference (member) variable that either
            // looks like this:
            //
            //     T& &functor;
            //
            // or this:
            //
            //     T&& &functor;
            //
            // We'll call this the "inner" functor. In either case
            // however the usual reference collapsing rules then kick
            // in so it always resolves to this (Google for these
            // rules if you're not already familiar):
            //
            //     T& functor;
            //
            // IOW, the inner "functor" is always an lvalue reference
            // inside the lambda even when the outer "functor" is an
            // rvalue reference. We therefore need to invoke
            // "std::forward" on it in the lambda below so that we get
            // back the correct "&" or "&&" reference of the outer
            // "functor" itself, which we then invoke in the correct
            // "&" or "&&" context below. See comments below.
            ////////////////////////////////////////////////////////////
            const auto process = [&functor]<std::size_t I>
                                 (const auto &process)
                                 {
                                     if constexpr (I < N)
                                     {
                                         //////////////////////////////////////////////////////////
                                         // See comments above. We call "std::forward()" here to
                                         // convert the inner "functor" back to the correct "&" or
                                         // "&&" reference type of the outer "functor" (note that
                                         // the inner "functor" member variable we're working with
                                         // here is always an lvalue reference as described in the
                                         // comments above). We then immediately invoke the
                                         // functor in this context noting that it wouldn't work
                                         // correctly in the "&&" case if we assigned the return
                                         // value of "std::forward" to a variable first and then
                                         // invoked the functor. We need to invoke the functor
                                         // using the return value of "std::forward" directly,
                                         // on-the-fly as seen. If we assigned it to a variable
                                         // first instead then in the "&&" case we'd have a
                                         // variable of type "&&" but since the variable has a
                                         // name it would be an lvalue, since named variables are
                                         // always lvalues (even for "&&" reference variables).
                                         // Invoking the functor on that lvalue variable would
                                         // therefore call the functor in the context of an
                                         // lvalue, not an rvalue which is what we require (so,
                                         // for instance, if the "operator()" member of "functor"
                                         // has an "&&" reference qualifier, though this would be
                                         // rare in practice but still always possible, a compiler
                                         // error would occur since we'd be attempting to call
                                         // that function using an lvalue which isn't legal -
                                         // doing it using the rvalue returned directly by
                                         // "std::forward()" works correctly).
                                         //////////////////////////////////////////////////////////
                                         if (std::forward<ForEachFunctorT>(functor).template operator()<I>())
                                         {
                                             ////////////////////////////////////////////////
                                             // Recursive call (in spirit anyway - actually
                                             // stamps out and runs another version of this
                                             // function specialized on I + 1)
                                             ////////////////////////////////////////////////
                                             return process.template operator()<I + 1>(process);
                                         }
                                         else
                                         {
                                             return false;
                                         }
                                     }
                                     else
                                     {
                                         return true;
                                     }
                                 };

            return process.template operator()<0>(process);
        #else
            STATIC_ASSERT_IS_FOR_EACH_FUNCTOR(ForEachFunctorT)

            //////////////////////////////////////////////////////////
            // Create an instance of "Private::ForEachImpl" and
            // invoke its "Process()" member function template,
            // passing 0 for its template arg (to start processing
            // the first iteration). The latter function then
            // recursively invokes itself, calling "functor" "N"
            // times or until "functor" returns false, whichever
            // comes first (false only returned if "functor" wants to
            // break like a normal "for" loop, which rarely happens
            // in practice so we usually iterate "N" times)
            //
            // Note: The "ForEachFunctorT" template arg we're passing
            // here as the 2nd template arg of class of
            // "Private::ForEachImpl" means that the latter's
            // constructor will take the same arg type as "functor"
            // itself (& or && as usual). We then call "std::forward"
            // as seen to pass "functor" to the constructor. Note
            // that "std::forward" is mandatory here because
            // "functor" is an lvalue even in the "&&" case (since
            // it's named - named rvalue references are still
            // lvalues). Attempting to pass "functor" directly would
            // therefore cause a compiler error since you can't bind
            // an lvalue (functor) to an rvalue reference (the arg
            // type that the "Private::ForEachImpl" constructor is
            // expecting in this case).
            ///////////////////////////////////////////////////////////
            return Private::ForEachImpl<N, ForEachFunctorT>(std::forward<ForEachFunctorT>(functor)).template Process<0>();
        #endif
    }

    ////////////////////////////////////////////////////////
    // "IsTupleSpecialization_v". Returns true if "T" is a
    // "std::tuple" specialization or false otherwise. Note
    // that "std::tuple" derivatives always return false,
    // since in the general case, types derived from a
    // specialization don't qualify as the specialization
    // itself, they only inherit from it (subtle but
    // meaningful difference). Note that "std" helpers like
    // "std::tuple_size" won't even work with "std::tuple"
    // derivatives (they fail to compile), and "std::tuple"
    // derivatives aren't normally valid in C++ anyway,
    // since the "std::function" destructor isn't virtual so
    // developers shouldn't normally derive from it (this
    // library never does).
    /////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsTupleSpecialization_v = IsSpecialization_v<T, std::tuple>;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept Tuple_c = IsTupleSpecialization_v<T>;
        #endif

        #define TUPLE_C StdExt::Tuple_c
        #define STATIC_ASSERT_IS_TUPLE_MSG(T)
        #define STATIC_ASSERT_IS_TUPLE(T)
        #define STATIC_ASSERT_IS_TUPLE_F(T)
    #else
        #define TUPLE_C typename
        #define STATIC_ASSERT_IS_TUPLE_MSG(T) QUOTE_STRINGIFY(T) " must be a \"std::tuple\" " \
                                              "specialization. This error normally occurs when " \
                                              "invoking a template in this library requiring " \
                                              "a \"std::tuple\" specialization for one of its " \
                                              "template args and you've passed an invalid type " \
                                              "instead (not a \"std::tuple\" specialization). " \
                                              "Note that the name of the template arg might " \
                                              "not be called " QUOTE_STRINGIFY(T) " depending on " \
                                              "the particular template you're calling but it's " \
                                              "not a \"std::tuple\" specialization regardless " \
                                              "(in this case it's simply called " QUOTE_STRINGIFY(T) \
                                              " in a lower-level template where your type was " \
                                              "passed and this \"static_asset\" was triggered)."

        #define STATIC_ASSERT_IS_TUPLE(T) static_assert(StdExt::IsTupleSpecialization_v<T>, \
                                                        STATIC_ASSERT_IS_TUPLE_MSG(T));

        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename TupleT>
            inline constexpr void StaticAssertIsTuple() noexcept
            {
                STATIC_ASSERT_IS_TUPLE(TupleT)
            }
        #endif
        #define STATIC_ASSERT_IS_TUPLE_F(T) StdExt::StaticAssertIsTuple<T>()
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    template <TUPLE_C TupleT,
              std::size_t I>
    inline constexpr bool IndexLessThanTupleSize_v = STATIC_ASSERT_V(STATIC_ASSERT_IS_TUPLE_F(TupleT),
                                                                     (I < std::tuple_size_v<TupleT>));
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename TupleT, std::size_t I>
            concept IndexLessThanTupleSize_c = IndexLessThanTupleSize_v<TupleT, I>;
        #endif

        #define REQUIRES_I_LESS_THAN_TUPLE_SIZE(TUPLE, I) requires StdExt::IndexLessThanTupleSize_c<TUPLE, (I)>
        #define STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE_MSG(TUPLE, I)
        #define STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE(TUPLE, I)
        #define STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE_F(TUPLE, I)
    #else
        #define REQUIRES_I_LESS_THAN_TUPLE_SIZE(TUPLE, I)
        #define STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE_MSG(TUPLE, I) "Invalid tuple index " QUOTE_STRINGIFY(I) ". Must be less than " \
                                                                   "the number of types in the tuple as returned by " \
                                                                   QUOTE_STRINGIFY_TEMPLATE_ARGS(std::tuple_size_v, TUPLE) " (for " \
                                                                   "the given template arg " QUOTE_STRINGIFY(TUPLE) "). This error " \
                                                                   "normally occurs when invoking any template in this library " \
                                                                   "taking an \"I\" template arg used to index into a \"std::tuple\" " \
                                                                   "but the index is invalid (greater than or equal to the tuple's " \
                                                                   "size - it must be less than as required by the specific template " \
                                                                   "you're calling or a non-existent type in the \"std::tuple\" is " \
                                                                   "being targeted otherwise). The \"std::tuple\" may not have been " \
                                                                   "passed directly to the template you're calling however, depending " \
                                                                   "on the template, but might be used internally, though the error " \
                                                                   "normally still occurs due to an invalid template arg you're " \
                                                                   "passing (usually template arg " QUOTE_STRINGIFY(I) " itself). " \
                                                                   "For instance, it will occur if a value for template arg \"I\" " \
                                                                   "is passed to a \"FunctionTraits\" template and it's greater " \
                                                                   "than or equal to the number of args in the function you're " \
                                                                   "targeting (if the particular template requires that it be less " \
                                                                   "than the number of args - a non-existent arg in that function " \
                                                                   "is being targeted otherwise). Check the template(s) you're " \
                                                                   "calling that may be directly or indirectly indexing into a " \
                                                                   "\"std::tuple\" using an invalid index (such as a \"FunctionTraits\" " \
                                                                   "template indexing into a function argument list but targeting a " \
                                                                   "non-existent arg)."
        #define STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE(TUPLE, I) static_assert(StdExt::IndexLessThanTupleSize_v<TUPLE, (I)>, \
                                                                             STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE_MSG(TUPLE, I));

        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename TupleT, std::size_t I>
            inline constexpr void StaticAssertIndexLessThanTupleSize() noexcept
            {
                STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE(TupleT, I)
            }
        #endif
        #define STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE_F(TUPLE, I) StdExt::StaticAssertIndexLessThanTupleSize<TUPLE, (I)>()
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    template <TUPLE_C TupleT, std::size_t I>
    inline constexpr bool IndexLessThanOrEqualToTupleSize_v = STATIC_ASSERT_V(STATIC_ASSERT_IS_TUPLE_F(TupleT),
                                                                              (I <= std::tuple_size_v<TupleT>));
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename TupleT, std::size_t I>
            concept IndexLessThanOrEqualToTupleSize_c = IndexLessThanOrEqualToTupleSize_v<TupleT, I>;
        #endif

        #define REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TUPLE, I) requires StdExt::IndexLessThanOrEqualToTupleSize_c<TUPLE, (I)>
        #define STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE_MSG(TUPLE, I)
        #define STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TUPLE, I)
        #define STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE_F(TUPLE, I)
    #else
        #define REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TUPLE, I)
        #define STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE_MSG(TUPLE, I) "Invalid tuple index " QUOTE_STRINGIFY(I) ". Must be less than " \
                                                                               "or equal to the number of types in the tuple as returned by " \
                                                                               QUOTE_STRINGIFY_TEMPLATE_ARGS(std::tuple_size_v, TUPLE) " (for " \
                                                                               "the given template arg " QUOTE_STRINGIFY(TUPLE) "). This error " \
                                                                               "normally occurs when invoking any template in this library " \
                                                                               "taking an \"I\" template arg used to index into a \"std::tuple\" " \
                                                                               "but the index is invalid (greater than the tuple's size - it " \
                                                                               "must be less than or equal as required by the specific template " \
                                                                               "you're calling). The \"std::tuple\" may not have been passed " \
                                                                               "directly to the template you're calling however, depending on the " \
                                                                               "template, but might be used internally, though the error normally " \
                                                                               "still occurs due to an invalid template arg you're passing " \
                                                                               "(usually template arg " QUOTE_STRINGIFY(I) " itself). For instance, " \
                                                                               "it will occur if a value for template arg \"I\" is passed to a " \
                                                                               "\"FunctionTraits\" template and it's greater than the number of " \
                                                                               "args in the function you're targeting (if the particular template " \
                                                                               "requires that it be less than or equal to the number of args). " \
                                                                               "Check the template(s) you're calling that may be directly or " \
                                                                               "indirectly indexing into a \"std::tuple\" using an invalid index " \
                                                                               "(such as a \"FunctionTraits\" template indexing into a function " \
                                                                               "argument list)."
        #define STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TUPLE, I) static_assert(StdExt::IndexLessThanOrEqualToTupleSize_v<TUPLE, (I)>, \
                                                                                         STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE_MSG(TUPLE, I));
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename TupleT, std::size_t I>
            inline constexpr void StaticAssertIndexLessThanOrEqualToTupleSize() noexcept
            {
                STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TupleT, I)
            }
        #endif
        #define STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE_F(TupleT, I) StdExt::StaticAssertIndexLessThanOrEqualToTupleSize<TupleT, (I)>()
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    ///////////////////////////////////////////////////////////////
    // For internal use only (implementation of "SubTuple_t"
    // declared just after this namespace - see this for details)
    ///////////////////////////////////////////////////////////////
    namespace Private
    {
        template <typename TupleT,
                  std::size_t I,
                  std::size_t Count>
        inline constexpr std::size_t MinOfCountOrTupleTypesRemainingImplArgsValidated_v = MinOfCountOrRemaining_v<I, std::tuple_size_v<TupleT>, Count>;

    #if defined(USE_CONCEPTS)
        #define MIN_OF_COUNT_OR_TUPLE_TYPES_REMAINING_V_IMPL Private::MinOfCountOrTupleTypesRemainingImplArgsValidated_v<TupleT, I, Count>
    #else
        // Primary template
        template <typename TupleT,
                  std::size_t I,
                  std::size_t Count,
                  typename = void> // Arg for internal use only (users should never explicitly pass it)
        class MinOfCountOrTupleTypesRemainingImpl
        {
            /////////////////////////////////////////////////////////
            // Always triggers at this point! (since specialization
            // just below didn't kick in)
            /////////////////////////////////////////////////////////
            static_assert(AlwaysFalse_v<TupleT>,
                          STATIC_ASSERT_IS_TUPLE_MSG(TupleT));
        public:
            ///////////////////////////////////////////////////////
            // Above "static_assert" always triggers so the
            // following "Value" can be omitted. If omitted
            // however then our call to access this value further
            // below might result in an "undeclared value" error
            // of some kind (since the value hasn't been
            // declared). This will only lead to confusion since
            // the above "static_assert" is the actual source of
            // the problem (so it's the only error message users
            // need to see). We therefore declare the following to
            // eliminate any extraneous "undeclared value" error
            // that might occur if it weren't declared.
            ///////////////////////////////////////////////////////
            static constexpr std::size_t Value = 0;
        };

        // Specialization of above template
        template <typename TupleT,
                  std::size_t I, // Renamed "Index" here by design (so we can call it "I"
                                     // in our "ValueImpl" structs below - can't call it "I"
                                     // in both places since it might trigger a "shadowing"
                                     // warning on some compilers)
                  std::size_t Count>
        class MinOfCountOrTupleTypesRemainingImpl<TupleT,
                                                  I,
                                                  Count,
                                                  std::enable_if_t< IsTupleSpecialization_v<TupleT> >
                                                 >
        {
            // Primary template
            template <std::size_t InnerI,
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct ValueImpl
            {
                /////////////////////////////////////////////////////////
                // Always triggers at this point! (since specialization
                // just below didn't kick in)
                /////////////////////////////////////////////////////////
                static_assert(AlwaysFalse_v<TupleT>,
                              STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE_MSG(TupleT, I));

                ///////////////////////////////////////////////////////
                // Above "static_assert" always triggers so the
                // following "Value" can be omitted. If omitted
                // however then our call to access this value further
                // below might result in an "undeclared value" error
                // of some kind (since the value hasn't been
                // declared). This will only lead to confusion since
                // the above "static_assert" is the actual source of
                // the problem (so it's the only error message users
                // need to see). We therefore declare the following to
                // eliminate any extraneous "undeclared value" error
                // that might occur if it weren't declared.
                ///////////////////////////////////////////////////////
                static constexpr std::size_t Value = 0;
            }; // class ValueImpl

            // Specialization of above template
            template <std::size_t InnerI>
            struct ValueImpl<InnerI,
                             std::enable_if_t< IndexLessThanOrEqualToTupleSize_v<TupleT, InnerI> >
                            >
            {
                // By design (sanity check only, should always succeed)
                static_assert(InnerI == I);

                static constexpr std::size_t Value = MinOfCountOrTupleTypesRemainingImplArgsValidated_v<TupleT, I, Count>;
            }; // class ValueImpl

        public:
            //////////////////////////////////////////////////////////
            // Passing "I" as a template arg rather than have
            // "ValueImpl" rely on the latter directly. We need to
            // pass it in as template arg to prevent possible
            // compiler warnings and/or errors from occurring when
            // "ValueImpl" is specialized above (since having
            // "ValueImpl" rely on "I" directly can run afoul of C++
            // template arg dependency rules and/or the C++ two-phase
            // template lookup rules - passing them as template args
            // here circumvents those issues). Note that "I" can
            // still be directly accessed within "ValueImpl" above,
            // once the primary template or specialization of
            // "ValueImpl" is actually selected by the compiler, but
            // relying on it in the selection process itself is rife
            // with issues (such as using it in the call to
            // "std::enable_if_t" above - obscure and arcane errors
            // can occur depending on how it's done but passing it in
            // as a fresh template arg in the following call and
            // relying on that instead in the template selection
            // process above steers clear of these errors)
            //////////////////////////////////////////////////////////
            static constexpr std::size_t Value = ValueImpl<I>::Value;
        };

        #define MIN_OF_COUNT_OR_TUPLE_TYPES_REMAINING_V_IMPL Private::MinOfCountOrTupleTypesRemainingImpl<TupleT, I, Count>::Value
    #endif // #if defined(USE_CONCEPTS)
    } // namespace Private

    //////////////////////////////////////////////////////////////////
    // SubTuple_t. Alias template for a "std::tuple" of all types in
    // template arg "TupleT" (a "std::tuple"), from index "I"
    // inclusive for a length of "Count" (so effectively the
    // "std::tuple" equivalent of "std::basic_string::substr" but
    // dealing with types instead of characters). Note that "I" must
    // be less than or equal to the size of "TupleT". If it equals
    // the size of "T" however then an empty tuple is always returned
    // (since "Count" is cut off here - read on). If "Count" exceeds
    // the number of types remaining in "TupleT" from index "I" then
    // all remaining types in "TupleT" are included (i.e., all types
    // from index "I" to the end of the tuple - the default arg for
    // "Count" ensures this). Note that if "Count" equals zero then
    // an empty tuple results and this is guaranteed if "I" equals
    // the size of "TupleT" itself (since "Count" will be forcibly
    // set to zero here, i.e., cut off as previously noted - an empty
    // tuple therefore results).
    //
    // E.g.,
    //
    //    using TupleT = std::tuple<char,              // 0
    //                              int,               // 1
    //                              long,              // 2
    //                              float,             // 3
    //                              double,            // 4
    //                              std::string_view>; // 5
    //
    //    ///////////////////////////////////////////////////////
    //    // Yields the following ("std::tuple" of all types in
    //    // "TupleT" from index 2 inclusive for a length of 3):
    //    //
    //    //    std::tuple<long, float, double>
    //    ///////////////////////////////////////////////////////
    //    using TupleAtIndex2Size3 = SubTuple_t<TupleT, 2, 3>;
    //
    //    /////////////////////////////////////////////////////////
    //    // Yields the following (all types in "TupleT" from
    //    // index 2 till the end - the default arg for "Count"
    //    // is "size_t_npos" which ensures this):
    //    //
    //    //    std::tuple<long, float, double, std::string_view>
    //    /////////////////////////////////////////////////////////
    //    using TupleAtIndex2TillEnd = SubTuple_t<TupleT, 2>;
    //
    //    //////////////////////////////////////////////////////////
    //    // Yields an empty tuple because "Count" arg is zero (0)
    //    //
    //    //    std::tuple<>
    //    //////////////////////////////////////////////////////////
    //    using EmptyTuple = SubTuple_t<TupleT, 2, 0>;
    //////////////////////////////////////////////////////////////////
    template <TUPLE_C TupleT,
              std::size_t I,
              std::size_t Count = size_t_npos>
    REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TupleT, I)
    inline constexpr std::size_t MinOfCountOrTupleTypesRemaining_v = MIN_OF_COUNT_OR_TUPLE_TYPES_REMAINING_V_IMPL;
    // Done with this (for private use of above template only) ...
    #undef MIN_OF_COUNT_OR_TUPLE_TYPES_REMAINING_V_IMPL

    ///////////////////////////////////////////////////////////////
    // For internal use only (implementation of "SubTuple_t"
    // declared just after this namespace - see this for details)
    ///////////////////////////////////////////////////////////////
    namespace Private
    {
        template <typename TupleT,
                  std::size_t I,
                  std::size_t Count>
        class SubTupleImplArgsValidated
        {
            template <std::size_t... Indices>
            static constexpr auto Process(std::index_sequence<Indices...>) -> std::tuple< std::tuple_element_t<Indices, TupleT>... >;

            static constexpr std::size_t ClampedCount = MinOfCountOrTupleTypesRemaining_v<TupleT, I, Count>;

        public:
            using Type = decltype(Process(MakeIndexSequenceStartAt<I, ClampedCount>()));
        };

    #if defined(USE_CONCEPTS)
        #define SUB_TUPLE_T_IMPL SubTupleImplArgsValidated
    #else
        // Primary template
        template <typename TupleT,
                  std::size_t I,
                  std::size_t Count,
                  typename = void> // Arg for internal use only (users should never explicitly pass it)
        class SubTupleImpl
        {
            /////////////////////////////////////////////////////////
            // Always triggers at this point! (since specialization
            // just below didn't kick in)
            /////////////////////////////////////////////////////////
            static_assert(AlwaysFalse_v<TupleT>,
                          STATIC_ASSERT_IS_TUPLE_MSG(TupleT));
        public:
            ///////////////////////////////////////////////////////
            // Above "static_assert" always triggers so the
            // following "Type" can be omitted. If omitted however
            // then our call to access this type further below
            // might result in an "undeclared type" error of some
            // kind (since the type hasn't been declared). This
            // will only lead to confusion since the above
            // "static_assert" is the actual source of the problem
            // (so it's the only error message users need to see).
            // We therefore declare the following to eliminate any
            // extraneous "undeclared type" error that might occur
            // if it weren't declared.
            ///////////////////////////////////////////////////////
            using Type = std::tuple<>;
        };

        // Specialization of above template
        template <typename TupleT,
                  std::size_t I,
                  std::size_t Count>
        class SubTupleImpl<TupleT,
                           I,
                           Count,
                           std::enable_if_t< IsTupleSpecialization_v<TupleT> >
                          >
        {
            // Primary template
            template <std::size_t InnerI,
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct TypeImpl
            {
                /////////////////////////////////////////////////////////
                // Always triggers at this point! (since specialization
                // just below didn't kick in)
                /////////////////////////////////////////////////////////
                static_assert(AlwaysFalse_v<TupleT>,
                              STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE_MSG(TupleT, I));

                ///////////////////////////////////////////////////////
                // Above "static_assert" always triggers so the
                // following "Type" can be omitted. If omitted however
                // then our call to access this type further below
                // might result in an "undeclared type" error of some
                // kind (since the type hasn't been declared). This
                // will only lead to confusion since the above
                // "static_assert" is the actual source of the problem
                // (so it's the only error message users need to see).
                // We therefore declare the following to eliminate any
                // extraneous "undeclared type" error that might occur
                // if it weren't declared.
                ///////////////////////////////////////////////////////
                using Type = std::tuple<>;
            }; // class TypeImpl

            // Specialization of above template
            template <std::size_t InnerI>
            struct TypeImpl<InnerI,
                            std::enable_if_t< IndexLessThanOrEqualToTupleSize_v<TupleT, InnerI> >
                           >
            {
                // By design (sanity check only, should always succeed)
                static_assert(InnerI == I);

                using Type = typename SubTupleImplArgsValidated<TupleT, I, Count>::Type;
            }; // class TypeImpl

        public:
            //////////////////////////////////////////////////////////
            // Passing "I" as a template arg rather than have
            // "TypeImpl" rely on the latter directly. We need to
            // pass it in as template arg to prevent possible
            // compiler warnings and/or errors from occurring when
            // "TypeImpl" is specialized above (since having
            // "TypeImpl" rely on "I" directly can run afoul of C++
            // template arg dependency rules and/or the C++ two-phase
            // template lookup rules - passing them as template args
            // here circumvents those issues). Note that "I" can
            // still be directly accessed within "TypeImpl" above,
            // once the primary template or specialization of
            // "TypeImpl" is actually selected by the compiler, but
            // relying on it in the selection process itself is rife
            // with issues (such as using it in the call to
            // "std::enable_if_t" above - obscure and arcane errors
            // can occur depending on how it's done but passing it in
            // as a fresh template arg in the following call and
            // relying on that instead in the template selection
            // process above steers clear of these errors)
            //////////////////////////////////////////////////////////
            using Type = typename TypeImpl<I>::Type;
        };

        #define SUB_TUPLE_T_IMPL SubTupleImpl
    #endif // #if defined(USE_CONCEPTS)
    } // namespace Private

    //////////////////////////////////////////////////////////////////
    // SubTuple_t. Alias template for a "std::tuple" of all types in
    // template arg "TupleT" (a "std::tuple"), from index "I"
    // inclusive for a length of "Count" (so effectively the
    // "std::tuple" equivalent of "std::basic_string::substr" but
    // dealing with types instead of characters). Note that "I" must
    // be less than or equal to the size of "TupleT". If it equals
    // the size of "T" however then an empty tuple is always returned
    // (since "Count" is cut off here - read on). If "Count" exceeds
    // the number of types remaining in "TupleT" from index "I" then
    // all remaining types in "TupleT" are included (i.e., all types
    // from index "I" to the end of the tuple - the default arg for
    // "Count" ensures this). Note that if "Count" equals zero then
    // an empty tuple results and this is guaranteed if "I" equals
    // the size of "TupleT" itself (since "Count" will be forcibly
    // set to zero here, i.e., cut off as previously noted - an empty
    // tuple therefore results).
    //
    // E.g.,
    //
    //    using TupleT = std::tuple<char,              // 0
    //                              int,               // 1
    //                              long,              // 2
    //                              float,             // 3
    //                              double,            // 4
    //                              std::string_view>; // 5
    //
    //    ///////////////////////////////////////////////////////
    //    // Yields the following ("std::tuple" of all types in
    //    // "TupleT" from index 2 inclusive for a length of 3):
    //    //
    //    //    std::tuple<long, float, double>
    //    ///////////////////////////////////////////////////////
    //    using TupleAtIndex2Size3 = SubTuple_t<TupleT, 2, 3>;
    //
    //    /////////////////////////////////////////////////////////
    //    // Yields the following (all types in "TupleT" from
    //    // index 2 till the end - the default arg for "Count"
    //    // is "size_t_npos" which ensures this):
    //    //
    //    //    std::tuple<long, float, double, std::string_view>
    //    /////////////////////////////////////////////////////////
    //    using TupleAtIndex2TillEnd = SubTuple_t<TupleT, 2>;
    //
    //    //////////////////////////////////////////////////////////
    //    // Yields an empty tuple because "Count" arg is zero (0)
    //    //
    //    //    std::tuple<>
    //    //////////////////////////////////////////////////////////
    //    using EmptyTuple = SubTuple_t<TupleT, 2, 0>;
    //////////////////////////////////////////////////////////////////
    template <TUPLE_C TupleT,
              std::size_t I,
              std::size_t Count = size_t_npos>
    REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TupleT, I)
    using SubTuple_t = typename Private::SUB_TUPLE_T_IMPL<TupleT,
                                                          I,
                                                          Count>::Type;
    // Done with this (for private use of above template only) ...
    #undef SUB_TUPLE_T_IMPL

    //////////////////////////////////////////////////////////////////
    // For internal use only (implementation of "TupleModifyTuple_t"
    // declared just after this namespace - see this for details)
    //////////////////////////////////////////////////////////////////
    namespace Private
    {
        template <typename TupleT,
                  std::size_t I,
                  std::size_t Count,
                  typename NewTypesTupleT>
        class TupleModifyTupleImplArgsValidated
        {
            static constexpr std::size_t ClampedCount = MinOfCountOrTupleTypesRemaining_v<TupleT, I, Count>;

        public:
            using Type = decltype(std::tuple_cat(std::declval<SubTuple_t<TupleT, 0, I>>(), // std::tuple of all types in "TupleT" from index 0 INclusive
                                                                                           // to "I" EXclusive (if I is zero then returned tuple is empty)
                                                 std::declval<NewTypesTupleT>(),
                                                 std::declval<SubTuple_t<TupleT, I + ClampedCount>>())); // std::tuple of all types in "TupleT" from
                                                                                                         // index "I + Count" INclusive to the end
                                                                                                         // (last type) in "TupleT" INclusive ("Count"
                                                                                                         // is cut off at the end of "TupleT" if it
                                                                                                         // exceeds it - if "I" equals the size of
                                                                                                         // "TupleT" then returned tuple is empty)
        };

    #if defined(USE_CONCEPTS)
        #define TUPLE_MODIFY_TUPLE_IMPL TupleModifyTupleImplArgsValidated
    #else
        // Primary template
        template <typename TupleT,
                  std::size_t I,
                  std::size_t Count,
                  typename NewTypesTupleT,
                  typename = void> // Arg for internal use only (users should never explicitly pass it)
        class TupleModifyTupleImpl
        {
            /////////////////////////////////////////////////////////
            // Always triggers at this point! (since specialization
            // just below didn't kick in)
            /////////////////////////////////////////////////////////
            static_assert(AlwaysFalse_v<TupleT>,
                          STATIC_ASSERT_IS_TUPLE_MSG(TupleT));
        public:
            ///////////////////////////////////////////////////////
            // Above "static_assert" always triggers so the
            // following "Type" can be omitted. If omitted however
            // then our call to access this type further below
            // might result in an "undeclared type" error of some
            // kind (since the type hasn't been declared). This
            // will only lead to confusion since the above
            // "static_assert" is the actual source of the problem
            // (so it's the only error message users need to see).
            // We therefore declare the following to eliminate any
            // extraneous "undeclared type" error that might occur
            // if it weren't declared.
            ///////////////////////////////////////////////////////
            using Type = std::tuple<>;
        }; // class TupleModifyTupleImpl

        // Specialization of above template
        template <TUPLE_C TupleT,
                  std::size_t I,
                  std::size_t Count,
                  TUPLE_C NewTypesTupleT>
        class TupleModifyTupleImpl<TupleT,
                                   I,
                                   Count,
                                   NewTypesTupleT,
                                   std::enable_if_t< IsTupleSpecialization_v<TupleT> >
                                  >
        {
            // Primary template
            template <std::size_t InnerI, // Same as template arg "I" in the outer (parent) "TupleModifyTupleImpl" class
                      typename InnerNewTypesTupleT, // Same as template arg "NewTypesTupleT" in the outer (parent) "TupleModifyTupleImpl" class
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct TypeImpl
            {
                // By design (sanity checks only, should always succeed)
                static_assert(InnerI == I);
                static_assert(std::is_same_v<InnerNewTypesTupleT, NewTypesTupleT>);

                ////////////////////////////////////////////////////////
                // Either or both always trigger at this point! (since
                // specialization just below didn't kick in)
                ////////////////////////////////////////////////////////
                STATIC_ASSERT_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TupleT, I) // "static_assert" if "I" is greater than the size of "TupleT"
                STATIC_ASSERT_IS_TUPLE(NewTypesTupleT) // "static_assert" if "NewTypesTupleT" isn't a "std::tuple" specialization

                ///////////////////////////////////////////////////////
                // Either or both of the above "static_assert" always
                // triggers so the following "Type" can be omitted. If
                // omitted however then our call to access this type
                // further below might result in an "undeclared type"
                // error of some kind since the type hasn't been
                // declared. This will only lead to confusion since
                // either or both of the above "static_asserts" are
                // the actual source of the problem (so they're the
                // only error messages users need to see). We
                // therefore declare the following to eliminate any
                // extraneous "undeclared type" error that might occur
                // if it weren't declared.
                ///////////////////////////////////////////////////////
                using Type = std::tuple<>;
            }; // class TypeImpl

            // Specialization of above template
            template <std::size_t InnerI,
                      typename InnerNewTypesTupleT>
            struct TypeImpl<InnerI, // Same as template arg "I" in the outer (parent) "TupleModifyTupleImpl" class
                            InnerNewTypesTupleT, // Same as template arg "NewTypesTupleT" in the outer (parent) "TupleModifyTupleImpl" class
                            std::enable_if_t< IndexLessThanOrEqualToTupleSize_v<TupleT, InnerI> &&
                                              IsTupleSpecialization_v<InnerNewTypesTupleT>
                                            >
                           >
            {
                // By design (sanity checks only, should always succeed)
                static_assert(InnerI == I);
                static_assert(std::is_same_v<InnerNewTypesTupleT, NewTypesTupleT>);

                using Type = typename TupleModifyTupleImplArgsValidated<TupleT,
                                                                        I,
                                                                        Count,
                                                                        NewTypesTupleT>::Type;
            }; // class TypeImpl

        public:
            //////////////////////////////////////////////////////////
            // Passing both "I" and "NewTypesTupleT" as template args
            // rather than have "TypeImpl" rely on the latter
            // directly. We need to pass them in as template args to
            // prevent possible compiler warnings and/or errors from
            // occurring when "TypeImpl" is specialized above (since
            // having "TypeImpl" rely on "I" and "NewTypesTupleT"
            // directly can run afoul of C++ template arg dependency
            // rules and/or the C++ two-phase template lookup rules -
            // passing them as template args here circumvents those
            // issues). Note that "I" and "NewTypesTupleT" can still
            // be directly accessed within "TypeImpl" above, once the
            // primary template or specialization of "TypeImpl" is
            // actually selected by the compiler, but relying on them
            // in the selection process itself is rife with issues
            // (such as using them in the call to "std::enable_if_t"
            // above - obscure and arcane errors can occur depending
            // on how it's done but passing them in as fresh template
            // args in the following call and relying on those
            // instead in the template selection process above steers
            // clear of these errors)
            //////////////////////////////////////////////////////////
            using Type = typename TypeImpl<I, NewTypesTupleT>::Type;
        }; // class TupleModifyTupleImpl

        #define TUPLE_MODIFY_TUPLE_IMPL TupleModifyTupleImpl
    #endif // #if defined(USE_CONCEPTS)
    } // namespace Private

    ///////////////////////////////////////////////////////////////////////////////
    // Type alias identical to "TupleT" (a "std::tuple") but where all types in
    // "TupleT" starting at (zero-based) index "I" for a length of "Count" have
    // been replaced with the types in "NewTypesTupleT". The purpose is therefore to
    // simply replace the existing types in "TupleT" starting at index "I" for a
    // length of "Count" with the types in "NewTypesTupleT", yielding an alias for the
    // resulting "std::tuple" (identical to "TupleT" except all types from index
    // "I" for a length of "Count" have been replaced with the types in
    // "NewTypesTupleT"). Note that "I" must be less than or equal to the number of
    // types in the "TupleT" or a constraints violation will occur in C++20 or
    // later (via the REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE concept seen in
    // the template's declaration), or a "static_assert" in C++17 (i.e., you must
    // target an existing range to replace in the tuple's types so "I" must be
    // within bounds - "Count" can exceed the number of types remaining from index
    // "I" however since the template will gracefully cap it at the number of
    // existing types actually remaining - it makes no sense to replace more types
    // than actually remain from index "I" that is, though you can replace them
    // with more types if "NewTypesTupleT" contains more).
    //
    // Note that passing zero (0) for the "Count" arg can be used to insert new
    // types into the tuple's existing types at index "I" instead of replacing the
    // existing types there, since zero (0) effectively means no existing types
    // will be replaced. The types in "NewTypesTupleT" will effectively be inserted at
    // index "I" instead, shifting over all remaining types from index "I"
    // inclusive until the end of the tuple (to accommodate the types in
    // "NewTypesTupleT"). The helper template "TupleInsert_t" (and its cousin
    // "TupleInsertTuple_t") exists for this specific purpose however and should
    // ideally be used instead (since the template's name better reflects its
    // purpose and the template is cleaner than calling "TupleModifyTuple_t"
    // directly in this case). See "Example 2" below.
    //
    // Note that passing "std::tuple_size_v<TupleT>" (the number of types in
    // "TupleT") for "I" can be used to append new types to the end of the tuple's
    // existing types instead of replacing existing types since "I" targets the
    // end of the existing types in this case. The helper template "TupleAppend_t"
    // (and its cousin "TupleAppendTuple_t") exists for this specific purpose
    // however and should ideally be used instead (again, the template's name
    // better reflects its purpose and the template is cleaner than calling
    // "TupleModifyTuple_t" directly in this case). See "Example 3" below.
    //
    // Note that passing an empty parameter pack for "NewTypesTupleT" can be used to
    // delete the types at index "I" for a length of "Count" instead of replacing
    // them with new types (since they're being replaced with an empty tuple so
    // they're not replaced with anything - they're effectively deleted instead).
    // The helper template "TupleDelete_t" exists for this specific purpose and
    // should ideally be used instead (again, since the template's name better
    // reflects its purpose and the template is cleaner than calling
    // "TupleModifyTuple_t" directly in this case). See "Example 4" below.
    //
    // Note that this template is the workhorse of all other tuple modification
    // templates in this header. It can effectively be used to carry out any type
    // of change to a tuple's existing types from replacing existing types, to
    // inserting, appending and deleting types as described above (though in the
    // rare case where you need to target a non-contiguous range of types you'll
    // have to call it more than once). To this end, note that all other tuple
    // modification templates in the library (e.g., "TupleInsert_t",
    // "TupleAppend_t", "TupleDelete_t", etc.) can be carried out by directly
    // invoking "TupleModifyTuple_t" with suitable template args (and the other
    // tuple modification templates do ultimately defer back to
    // "TupleModifyTuple_t" to carry out their actual work). The other tuple
    // modification templates should normally be used instead however, since they
    // are specialized for the most common use cases (and are therefore cleaner
    // and more suitably named than calling "TupleModifyTuple_t" directly).
    //
    // Examples:
    //
    //   // Sample tuple used in all examples below
    //   using TupleT = std::tuple<char, long, float, double, std::string>;
    //
    //   ////////////////////////////////////////////////////////////////////
    //   // Example 1 (replacing types in a tuple)
    //   // --------------------------------------
    //   // Use the helper alias "TupleModify_t" to modify (replace) the
    //   // types in "TupleT" at (zero-based) index 1 for a length of 2 (so
    //   // the "long" and the "float") with the new types being passed below
    //   // (i.e., the "long" and "float" in "TupleT" are replaced with the
    //   // types being passed). The resulting tuple is therefore:
    //   //
    //   //    std::tuple<char, int, unsigned char, bool, double, std::string>
    //   //
    //   // Note that you can also rely on template "TupleModifyTuple_t" if
    //   // you wish to pass the "NewTypesTupleT" parameter pack below as a
    //   // "std::tuple" instead. The templates are identical otherwise.
    //   ////////////////////////////////////////////////////////////////////
    //   using TupleT_WithTypesReplacedAtIndex1Size2 = TupleModify_t<TupleT, 1, 2, int, unsigned char, bool>;
    //
    //   ////////////////////////////////////////////////////////////////////
    //   // Example 2 (inserting new types in a tuple)
    //   // ------------------------------------------
    //   // By passing zero (0) for "Count", the types you pass (parameter
    //   // pack "NewTypesTupleT") can be inserted into "TupleT" instead of
    //   // replacing the existing types in "TupleT" (i.e., nothing in
    //   // "TupleT" is replaced since "Count" is zero so the types being
    //   // passed, "NewTypesTupleT", are effectively inserted at index "I"
    //   // instead of replacing the existing types there - the existing
    //   // types remain intact but are just shifted over to accommodate the
    //   // new types being inserted). If doing so however then you can just
    //   // rely on the helper templates "TupleInsert_t" or its cousin
    //   // "TupleInsertTuple_t" instead (both declared further below), which
    //   // just defer to the following template, passing zero as described
    //   // (see alternate "using" statement just below the following call
    //   // for an example - both "using" statements are equivalent)
    //   //
    //   // The following example inserts the new types into "TupleT" at
    //   // index 1 (inserted because "Count" is zero as described just
    //   // above). The resulting alias is therefore:
    //   //
    //   //     std::tuple<char, int, unsigned char, bool, long, float, double, std::string>
    //   ////////////////////////////////////////////////////////////////////
    //   using TupleT_WithNewTypesInsertedAtIndex1 = TupleModify_t<TupleT, 1, 0, int, unsigned char, bool>;
    //
    //   /////////////////////////////////////////////////////
    //   // Cleaner alternative to just above (helper alias
    //   // declared later on), which just invokes the above
    //   // but the name better reflects its purpose and the
    //   // "Count" arg of zero (0) seen just above no longer
    //   // needs to be explicitly passed (it's implicit)
    //   /////////////////////////////////////////////////////
    //   using TupleT_WithNewTypesInsertedAtIndex1 = TupleInsert_t<TupleT, 1, int, unsigned char, bool>;
    //
    //   ////////////////////////////////////////////////////////////////////
    //   // Example 3 (appending new types at the end of a tuple)
    //   // -----------------------------------------------------
    //   // By passing "std::tuple_size_v<TupleT>" for the "I" template arg
    //   // (i.e., the number of types in "TupleT" - the "Count" is then
    //   // immaterial since the template will always force it to zero in
    //   // this case), the types you pass (parameter pack "NewTypesTupleT")
    //   // will be appended to the end of "TupleT" instead of replacing any
    //   // existing types in "TupleT" with "NewTypesTupleT" (i.e., nothing
    //   // in "TupleT" is replaced since "I" is the end of the tuple so
    //   // "NewTypesTupleT" is effectively appended to the end). If doing
    //   // so however then you can just rely on the helper template
    //   // "TupleAppend_t" or its cousin "TupleAppendTuple_t" instead (both
    //   // declared further below), which just defers to the following
    //   // template, passing "std::tuple_size_v<TupleT>" for index "I" as
    //   // described (see alternate "using" statement just below the
    //   // following call for an example - both "using" statements are
    //   // equivalent).
    //   //
    //   // The following example appends the new types to the end of
    //   // "TupleT" (appended because "I" is "std::tuple_size_v<TupleT>" as
    //   // described just above). The resulting alias is therefore:
    //   //
    //   //     std::tuple<char, long, float, double, std::string, int, unsigned char, bool>
    //   ////////////////////////////////////////////////////////////////////////////
    //   using TupleT_WithNewTypesAppendedAtEnd = TupleModify_t<TupleT, std::tuple_size_v<TupleT>, 0, int, unsigned char, bool>;
    //
    //   /////////////////////////////////////////////////////
    //   // Cleaner alternative to just above (helper alias
    //   // declared later on), which just invokes the above
    //   // but the name better reflects its purpose and
    //   // neither the "I" arg of "std::tuple_size_v<TupleT>"
    //   // seen just above nor the "Count" arg of 0 seen just
    //   // above needs to be explicitly passed (they are both
    //   // implicit).
    //   /////////////////////////////////////////////////////
    //   using TupleT_WithNewTypesAppendedAtEnd = TupleAppend_t<TupleT, int, unsigned char, bool>;
    //
    //   ////////////////////////////////////////////////////////////////////
    //   // Example 4 (deleting types from a tuple)
    //   // ---------------------------------------
    //   // By passing an empty parameter pack for "NewTypesTupleT", you can
    //   // effectively delete the types in "TupleT" at index "I" for a
    //   // length of "Count" instead of replacing them, since they're being
    //   // replaced with the types in an empty parameter pack. Since it's
    //   // empty the types in "TupleT" at index "I" for a length of "Count"
    //   // are therefore replaced with nothing (so effectively deleted). If
    //   // doing so however then you can just rely on "TupleDelete_t"
    //   // instead or its cousin "TupleDeleteTuple_t". It just defers to the
    //   // following template, passing an empty parameter pack for
    //   // "NewTypesTupleT" as described (see alternate "using" statement
    //   // just below the following call for an example - both "using"
    //   // statements are equivalent).
    //   //
    //   // The following example deletes the types in "TupleT" at index 1
    //   // for a length of 2, i.e., the "long" and the "float" are deleted
    //   // (since no types are being passed for the "NewTypesTupleT" arg -
    //   // it's an empty parameter pack so the "long" and the "float" types
    //   // at index 1 and 2 are replaced with nothing, i.e., effectively
    //   // deleted). The resulting alias is therefore:
    //   //
    //   //     std::tuple<char, double, std::string>
    //   ////////////////////////////////////////////////////////////////////
    //   using TupleT_WithTypesDeletedAtIndex1Size2 = TupleModify_t<TupleT, 1, 2>;
    //
    //   /////////////////////////////////////////////////////
    //   // Cleaner alternative to just above (helper alias
    //   // declared later on), which just invokes the above
    //   // but the name better reflects its purpose
    //   /////////////////////////////////////////////////////
    //   using TupleT_WithTypesDeletedAtIndex1Size2 = TupleDelete_t<TupleT, 1, 2>;
    ///////////////////////////////////////////////////////////////////////////////
    template<TUPLE_C TupleT,
             std::size_t I,
             std::size_t Count,
             TUPLE_C NewTypesTupleT
            >
    REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TupleT, I)
    using TupleModifyTuple_t = typename Private::TUPLE_MODIFY_TUPLE_IMPL<TupleT,
                                                                         I,
                                                                         Count,
                                                                         NewTypesTupleT>::Type;
    // Done with this (for private use of above template only) ...
    #undef TUPLE_MODIFY_TUPLE_IMPL

    ///////////////////////////////////////////////////////////////////////////////
    // TupleModify_t. Identical to "TupleModifyTuple_t" just above but takes a
    // parameter pack for the last arg instead of a "std::tuple". Just converts
    // that parameter pack to a "std::tuple" however and defers to template just
    // above
    ///////////////////////////////////////////////////////////////////////////////
    template<TUPLE_C TupleT,
             std::size_t I,
             std::size_t Count,
             typename... NewTypesT
            >
    REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TupleT, I)
    // Defer to template just above
    using TupleModify_t = TupleModifyTuple_t<TupleT,
                                             I,
                                             Count,
                                             std::tuple<NewTypesT...> > ;

    ///////////////////////////////////////////////////////////////////////////////
    // TupleInsertTuple_t. Alias identical to "TupleT" (a "std::tuple") after
    // inserting the types in "TypesToInsertTupleT" into "TupleT" at
    // (zero-based) index "I" in "TupleT", shifting the existing "TupleT"
    // types at index "I" over to accommodate the newly inserted types. This
    // template is just the special case of template "TupleModifyTuple_t"
    // where the "Count" arg of the latter template is zero (0). See example
    // 2 in "TupleModifyTuple_t" for details (the following call implements
    // that example's code). Note that if "I" is the size of "TupleT" itself
    // (i.e., it equals "std::tuple_size_v<TupleT>"), then the following
    // inserts "TypesToInsertTupleT" at the end of "TupleT" so effectively
    // appends it. In this case you can therefore just rely on template
    // "TupleAppendTuple_t" further below instead (which just defers to us,
    // passing "std::tuple_size_v<TupleT>" as described).
    ///////////////////////////////////////////////////////////////////////////////
    template<TUPLE_C TupleT,
             std::size_t I,
             TUPLE_C TypesToInsertTupleT
            >
    REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TupleT, I)
    using TupleInsertTuple_t = TupleModifyTuple_t<TupleT,
                                                  I,
                                                  0, // Count
                                                  TypesToInsertTupleT>; // NewTypesT

    ///////////////////////////////////////////////////////////////////////////////
    // TupleInsert_t. Identical to "TupleInsertTuple_t" just above but takes a
    // parameter pack for the last arg instead of a "std::tuple". Just converts
    // that parameter pack to a "std::tuple" however and defers to template just
    // above.
    ///////////////////////////////////////////////////////////////////////////////
    template<TUPLE_C TupleT,
             std::size_t I,
             typename... TypesToInsertT
            >
    REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TupleT, I)
    // Defer to template just above
    using TupleInsert_t = TupleInsertTuple_t<TupleT,
                                             I,
                                             std::tuple<TypesToInsertT...> >;

    ///////////////////////////////////////////////////////////////////////////////
    // TupleAppendTuple_t. Alias identical to "TupleT" (a "std::tuple") after
    // appending the types in "TypesToAppendTupleT" to the end of "TupleT". This
    // template is just the special case of template "TupleInsertTuple_t" where
    // its "I" template arg is the size of "TupleT" itself (i.e., it equals
    // "std::tuple_size_v<TupleT>"), which in turn is the special case of
    // "TupleModifyTuple_t" where "I" is the size of "TupleT" (again,
    // "std::tuple_size_v<TupleT>") and "Count" is zero (0). See both of the
    // latter templates for details and example 3 in "TupleModifyTuple_t" in
    // particular (the following call ultimately implements that example's
    // code).
    ///////////////////////////////////////////////////////////////////////////////
    template<TUPLE_C TupleT,
             TUPLE_C TypesToAppendTupleT
            >
    using TupleAppendTuple_t = TupleInsertTuple_t<TupleT,
                                                  std::tuple_size_v<TupleT>, // I
                                                  TypesToAppendTupleT>;

    /////////////////////////////////////////////////////////////////////////////
    // TupleAppend_t. Identical to "TupleAppendTuple_t" just above but takes a
    // parameter pack for the last arg instead of a "std::tuple". Just converts
    // that parameter pack to a "std::tuple" however and defers to template just
    // above.
    /////////////////////////////////////////////////////////////////////////////
    template<TUPLE_C TupleT,
             typename... TypesToAppendT
            >
    // Defer to template just above
    using TupleAppend_t = TupleAppendTuple_t<TupleT,
                                             std::tuple<TypesToAppendT...>>;

    ///////////////////////////////////////////////////////////////////////////////
    // TupleDelete_t. Alias identical to "TupleT" (a "std::tuple") after deleting
    // the types at (zero-based) index "I" for a length of "Count" (default args
    // below delete all types in "Tuple"T resulting in an empty "std::tuple").
    // This template is just the special case of "TupleModifyTuple_t" where its
    // "NewTypesT" arg is an empty "std::tuple". That is, the call to
    // "TupleModify_t" below is being passed an empty parameter pack which the
    // latter template simply converts to an empty "std::tuple" and passes to
    // "TupleModifyTuple_t". See example 4 in "TupleModifyTuple_t" for details
    // (the following call implements that example's code).
    ///////////////////////////////////////////////////////////////////////////////
    template<TUPLE_C TupleT,
             std::size_t I = 0,
             std::size_t Count = size_t_npos>
    REQUIRES_I_LESS_THAN_OR_EQUAL_TO_TUPLE_SIZE(TupleT, I)
    using TupleDelete_t = TupleModify_t<TupleT,
                                        I,
                                        Count>;

    ////////////////////////////////////////////////////////////////////////////
    // "IsForEachTupleTypeFunctor" (primary template). Determines if template arg
    // "T" is a functor type whose "operator()" member has the following
    // signature and is also callable in the context of whether "T" is "const"
    // and/or "volatile" and or an lvalue or rvalue (read on):
    //
    //     template <std::size_t I, typename TupleElementT>
    //     bool operator()() [const] [volatile] [&|&&];
    //
    // If "T" contains this function AND it can also be called on an instance
    // of "T" (again, read on), then "T" qualifies as a functor that can be
    // passed to function template "ForEachTupleType()" declared later on. Note
    // that the specialization just below does the actual work of checking for
    // this. It simply invokes the above function template in an unevaluated
    // context using "std::declval()" as follows:
    //
    //     T::operator<0, void>()
    //
    // If the call succeeds then "operator()" must not only exist as a member
    // of "T" with a "bool" return type (we check for this as well), but it's
    // also callable in the context of whether "T" is "const" and/or "volatile"
    // and or an lvalue or rvalue. Note that to qualify as an lvalue for
    // purposes of "IsForEachTupleTypeFunctor", "T" must be an lvalue reference
    // such as "int &". To qualify as an rvalue, "T" must either be a
    // non-reference type such as a plain "int" OR an rvalue reference such as
    // "int &&". The behavior of what qualifies as an lvalue and rvalue for
    // purposes of using "IsForEachTupleTypeFunctor" therefore follows the usual
    // perfect forwarding rules for function template arguments (where an
    // lvalue reference for "T" means the function's "T &&" argument collapses
    // to an lvalue reference, and a non-reference type or rvalue reference
    // type for "T" means the function's "T &&" argument is an rvalue reference
    // - read up on perfect forwarding for details if you're not already
    // familiar this).
    //
    // As an example, if "T" contains the above "operator()" template but it's
    // not declared with the "const" qualifier but "T" itself is (declared
    // "const"), then calling the function as shown above fails since you can't
    // call a non-const member function using a constant object. The primary
    // template therefore kicks in, inheriting from "std::false_type".
    // Similarly, if "operator()" is declared with the "&&" reference qualifier
    // (rare as this is in practice), but "T" is an lvalue reference, then
    // again, the above call fails since you can't invoke a member function
    // declared with the "&&" (rvalue) reference qualifier using an lvalue
    // reference. Again, the primary template will therefore kick in (once
    // again inheriting from "std::false_type"). The upshot is that not only
    // must "operator()" exist as a member of "T", it must also be callable in
    // the context of "T" itself (whether "T" is "const" and/or "volatile"
    // and/or an lvalue reference, rvalue reference or non-reference - in all
    // cases "operator()" must be properly declared to support calls based on
    // the cv-qualifiers and/or lvalue/rvalue state of "T" itself as described
    // above). If it is then the specialization kicks in and
    // "IsForEachTupleTypeFunctor" therefore inherits from "std::true_type"
    // (meaning "T" is a suitable functor type for passing as the 2nd template
    // arg of function template "ForEachTupleType()").
    ////////////////////////////////////////////////////////////////////////////
    template <typename T,
              typename = void> // Arg for internal use only (users should never explicitly pass it)
    struct IsForEachTupleTypeFunctor : std::false_type
    {
    };

    /////////////////////////////////////////////////////////////////
    // Specialization of "IsForEachTupleTypeFunctor" (primary)
    // template just above. This specialization does all the work.
    // See primary template above for details. Note that calls to
    // "declval(T)" return "T &&" (unless "T" is void - see
    // "declval()" docs) so the following call to this works
    // according to the usual perfect forwarding rules in C++. That
    // is, if "T" is an lvalue reference then "declval()" returns an
    // lvalue reference (due to the C++ reference collapsing rules),
    // otherwise "T" must be a non-reference or an rvalue reference
    // so "declval" returns an rvalue reference (in either case). In
    // all cases "declval()" therefore returns the same function
    // argument type used in a perfect forwarding function. Such
    // arguments are always of the type "T &&" which resolves to an
    // lvalue reference only if "T" is an lvalue reference (again,
    // due to the C++ reference collapsing rules), or an rvalue
    // reference if "T" is a non-reference or rvalue reference.
    // "declval()" therefore returns the exact same type as a
    // perfectly forwarded argument after plugging in "T" which is
    // what we require here (we're invoking "operator()" on that
    // argument in the following call in an unevaluated context
    // simply to make sure it can be invoked - the primary template
    // kicks in via SFINAE otherwise).
    /////////////////////////////////////////////////////////////////
    template <typename T>
    struct IsForEachTupleTypeFunctor<T,
                                     //////////////////////////////////////////////////////
                                     // See explanation of "declval()" in comments above.
                                     // Note BTW that we're passing "int" as the 2nd
                                     // template arg in the following call, but the type
                                     // we pass is arbitrary (since it's just for SFINAE
                                     // purposes only here). We explicitly don't pass
                                     // "void" however since function parameter types
                                     // can't be "void" in C++, so if we come through here
                                     // when processing a function argument list, such as
                                     // when "ForEachArg()" is called (since it ultimately
                                     // defers to us), it's better not to pass "void" in
                                     // case the following implementation of "operator()"
                                     // chokes on it for some reason (unlikely but it may
                                     // be expecting a valid function parameter type so
                                     // who knows what it might do if we pass "void" which
                                     // isn't a legal function parameter type as noted).
                                     //////////////////////////////////////////////////////
                                     std::enable_if_t<std::is_same_v<decltype(std::declval<T>().template operator()<std::size_t(0), int /* Don't pass "void" here, see comments just above */>()),
                                                                     bool
                                                                    >
                                                     >
                                    > : std::true_type
    {
    };

    /////////////////////////////////////////////////////////
    // Helper variable template for "IsForEachTupleTypeFunctor"
    // just above (with the usual "_v" suffix).
    /////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsForEachTupleTypeFunctor_v = IsForEachTupleTypeFunctor<T>::value;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept ForEachTupleTypeFunctor_c = IsForEachTupleTypeFunctor_v<T>;
        #endif

        #define FOR_EACH_TUPLE_TYPE_FUNCTOR_C StdExt::ForEachTupleTypeFunctor_c
        #define STATIC_ASSERT_IS_FOR_EACH_TUPLE_TYPE_FUNCTOR(T)
    #else
        #define FOR_EACH_TUPLE_TYPE_FUNCTOR_C typename
        #define STATIC_ASSERT_IS_FOR_EACH_TUPLE_TYPE_FUNCTOR(T) static_assert(StdExt::IsForEachTupleTypeFunctor_v<T>, \
                                                                              "Invalid template arg " QUOTE_STRINGIFY(T) ". This error occurs " \
                                                                              "when invoking function template \"ForEachTupleType()\" or any " \
                                                                              "template that defers to it, such as \"ForEachArg()\" (whose " \
                                                                              "template arg is called \"ForEachArgFunctorT\" but corresponds " \
                                                                              "to template arg " QUOTE_STRINGIFY(T) "). This template arg must " \
                                                                              "be a class/struct with a non-static, template-based function " \
                                                                              "call operator similar to the following:\n" \
                                                                              "\n" \
                                                                              "template <std::size_t I, typename TypeT>\n" \
                                                                              "bool operator()() const\n" \
                                                                              "\n" \
                                                                              "The function should return true to continue iterating or false to " \
                                                                              "stop iterating, similar to a \"break\" statement in a regular \"for\" " \
                                                                              "loop. Note that the function must return a \"bool\" but otherwise " \
                                                                              "must simply be invocable on an instance of " QUOTE_STRINGIFY(T) " (so " \
                                                                              "whether it's declared \"const\", \"noexcept\", etc. is immaterial so " \
                                                                              "long as it can be specialized on a \"std::size_t\" and an arbitrary " \
                                                                               "type, and invoked on an instance of " QUOTE_STRINGIFY(T) " as noted.");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    // /////////////////////////////////////////////////////////////////////
    // For C++17 only (though we're testing the following constant instead
    // but read on) we'll rely on the "ProcessTupleType" functor declared
    // just below which is used exclusively by function template
    // "ForEachTupleType()" declared just afterwards. For C++20 and later
    // however we'll rely on a lambda template instead (the equivalent of
    // the struct just below but locally declared in function template
    // "ForEachTupleType()" so it's cleaner). Note that lambda templates
    // aren't supported until C++20 but we're relying on the following
    // constant to test for it instead of checking CPP20_OR_LATER, just in
    // case we're now running under an early version of C++20 where lambda
    // templates aren't supported yet. The following is guaranteed never to
    // be #defined in C++17 (by design), but it may not be #defined in
    // early versions of C++20 either, if lambda templates aren't available
    // yet (depending on the compiler).
    // /////////////////////////////////////////////////////////////////////
    #if !defined(LAMBDA_TEMPLATES_SUPPORTED)
        ///////////////////////////////////////////////////
        // For internal use only (by "ForEachTupleType()"
        // just after this namespace)
        ///////////////////////////////////////////////////
        namespace Private
        {
            /////////////////////////////////////////////////////////////
            // class ProcessTupleType. Private implementation class used
            // by function template "ForEachTupleType()" declared just
            // after this private namespace. See this for details.  Note
            // that in the following class, "ForEachTupleTypeFunctorT"
            // is always the same template type passed to
            // "ForEachTupleType()", so either "T &" for the lvalue case
            // or just plain "T" or "T &&" for the rvalue case (for the
            // "rvalue" case however usually just plain "T" as per the
            // usual perfect forwarding rules when invoking such
            // functions via implicit type deduction).
            ///////////////////////////////////////////////////////////
            template <typename TupleT,
                      typename ForEachTupleTypeFunctorT>
            class ProcessTupleType
            {
            public:
                /////////////////////////////////////////////////////////
                // Constructor. Note that we always pass "functor" by a
                // "std::forward" reference so "functor" resolves to "&"
                // or "&&" accordingly
                /////////////////////////////////////////////////////////
                constexpr ProcessTupleType(ForEachTupleTypeFunctorT &&functor) noexcept
                    ///////////////////////////////////////////////////////////
                    // "std::forward" required when "ForEachTupleTypeFunctorT"
                    // is "&&". Both "functor" and "m_Functor" are therefore
                    // "&&" but "functor" is still an lvalue because it's
                    // named (rvalue references that are named are lvalues!).
                    // Therefore illegal to bind an lvalue (functor) to an
                    // rvalue reference (m_Functor) so "std::forward" is
                    // required to circumvent this (for the "&&" case it
                    // returns an "&&" reference but it's now unnamed so an
                    // rvalue and therefore suitable for initializing
                    // "m_Functor")
                    ///////////////////////////////////////////////////////////
                    : m_Functor(std::forward<ForEachTupleTypeFunctorT>(functor))
                {
                }

                template <std::size_t I>
                constexpr bool operator()() const
                {
                    using TupleElement_t = std::tuple_element_t<I, TupleT>;

                    //////////////////////////////////////////////////////////
                    // Note: Call to "std::forward()" here required to:
                    //
                    //    1) Perfect forward "m_Functor" back to "&" or "&&"
                    //       accordingly
                    //    2) In the "&&" case, invoke "operator()" in the
                    //       context of an rvalue (in particular, can't do
                    //       this on "m_Functor" directly, without invoking
                    //       "std::forward", since "m_Functor" is an lvalue
                    //       so the lvalue version of "operator()" would kick
                    //       in in the following call instead!!)
                    //////////////////////////////////////////////////////////
                    return std::forward<ForEachTupleTypeFunctorT>(m_Functor).template operator()<I, TupleElement_t>();
                }

            private:
                /////////////////////////////////////////////////////
                // Note: Resolves to "&" in the lvalue case or "&&"
                // otherwise (by the usual C++ reference collapsing
                // rules - "ForEachTupleTypeFunctorT" is always "T &"
                // in the former case or either plain "T" or "T &&"
                // otherwise)
                /////////////////////////////////////////////////////
                ForEachTupleTypeFunctorT &&m_Functor;
            };
        } // namespace Private
    #endif // #if CPP17_OR_EARLIER

    /////////////////////////////////////////////////////////////////////////
    // ForEachTupleType(). Iterates all tuple elements in "TupleT" where
    // "TupleT" is a "std::tuple" specialization, and invokes the given
    // "functor" for each. The effect of this function is that all types in
    // "TupleT" are iterated or none if "TupleT" is empty. For each type in
    // "TupleT", the given "functor" is invoked as noted. "functor" must be
    // a functor with the following functor (template) signature (where
    // items in square brackets are optional - note that an invalid functor
    // will trigger the FOR_EACH_TUPLE_TYPE_FUNCTOR_C  concept in C++20 or
    // later, or a "static_assert" in C++17, since we don't support earlier
    // versions - compile fails either way)
    //
    //     template <std::size_t I, typename TupleElementT>
    //     bool operator()() [const] [volatile] [ref-qualifier] [noexcept];
    //
    // Note that lambdas with this signature are also (therefore) supported
    // in C++20 or later (since lambda templates aren't supported in earlier
    // versions of C++ so you'll have to roll your own functor template if
    // targeting C++17). Note that free functions or other non-static member
    // functions besides "operator()" are not currently supported (but may
    // be in a future release):
    //
    //     Example
    //     -------
    //     ////////////////////////////////////////////////////////
    //     // Some tuple whose types you wish to iterate (display
    //     // in this example - see "displayTupleType" lambda
    //     // below).
    //     ////////////////////////////////////////////////////////
    //     std::tuple<int, float, double> someTuple;
    //
    //     /////////////////////////////////////////////////////////////////
    //     // Lambda that will be invoked just below, once for each type in
    //     // "someTuple()" above (where template arg "I" is the zero-based
    //     // "Ith" index of the type in "someTuple", and "TupleElementT" is
    //     // its type). Note that lambda templates are supported in C++20
    //     // and later only. For C++17 (this header doesn't support earlier
    //     // versions), you need to roll your own functor instead (with a
    //     // template version of "operator()" equivalent to this lambda)
    //     /////////////////////////////////////////////////////////////////
    //     const auto displayTupleType = []<std::size_t I, typename TupleElementT>()
    //                                   {
    //                                       /////////////////////////////////////////////////////////
    //                                       // Display the type of the (zero-based) "Ith" type in
    //                                       // "someTuple" using the following format (shown here
    //                                       // for "I" equals zero so "TupleElementT" is therefore
    //                                       // an "int", but whatever "I" and "TupleElementT" are
    //                                       // on each iteration - note that we increment zero-based
    //                                       // "I" by 1 however to display it as 1-based since it's
    //                                       // more natural for display purposes):
    //                                       //
    //                                       //    1) int
    //                                       //
    //                                       // Note that we never come through here if the tuple
    //                                       // represented by "someTuple" is empty.
    //                                       /////////////////////////////////////////////////////////
    //                                       tcout << (I + 1) << _T(") ") << TypeName_v<TupleElementT> << _T("\n");
    //
    //                                       //////////////////////////////////////////////
    //                                       // Return true to continue iterating (false
    //                                       // would stop iterating, equivalent to a
    //                                       // "break" statement in a regular "for" loop)
    //                                       //////////////////////////////////////////////
    //                                       return true;
    //                                   };
    //
    //     ///////////////////////////////////////////////////////////////
    //     // Iterate all tuple elements in "someTuple" meaning once for
    //     // each type in "someTuple()" above. Invokes "displayTupleType"
    //     // above for each. Outputs the following:
    //     //
    //     //     1) int
    //     //     2) float
    //     //     3) double
    //     //
    //     // Note that "ForEachTupleType()" returns true if the functor
    //     // you pass ("displayTupleType" in this example) returns true
    //     // or false otherwise (useful if your functor needs to "break"
    //     // like a normal "for" loop for any reason - iteration
    //     // immediately stops if it returns false and the following
    //     // therefore returns false). Always returns true in the above
    //     // example (and typically in most real-world code as well)
    //     ///////////////////////////////////////////////////////////////
    //     ForEachTupleType<decltype(someTuple)>(displayTupleType);
    /////////////////////////////////////////////////////////////////////////
    template <TUPLE_C TupleT,
              FOR_EACH_TUPLE_TYPE_FUNCTOR_C ForEachTupleTypeFunctorT>
    inline constexpr bool ForEachTupleType(ForEachTupleTypeFunctorT&& functor)
    {
        // See this constant for details
        #if !defined(USE_CONCEPTS)
            //////////////////////////////////////////////////
            // Kicks in if concepts not supported, otherwise
            // corresponding concepts kick in in the template
            // declaration above instead (macros for these
            // above resolve to the "typename" keyword when
            // concepts aren't supported and the following
            // "static_asserts" are then used instead)
            //////////////////////////////////////////////////
            STATIC_ASSERT_IS_TUPLE(TupleT) // "static_assert" if "TupleT" isn't a "std::tuple" specialization
            STATIC_ASSERT_IS_FOR_EACH_TUPLE_TYPE_FUNCTOR(ForEachTupleTypeFunctorT)
        #endif

        /////////////////////////////////////////////////////////////////
        // IMPORTANT
        // ---------
        // (Note: You need not read this (very) long explanation since
        // we've correctly worked around it as described below, but I've
        // included it here for posterity).
        //
        // The following call checks for zero as seen (true only when
        // "TupleT" is empty), in order to completely bypass the call to
        // "ForEach()" seen further below (when "TupleT" isn't empty).
        // There's no need to call "ForEach()" further below if the
        // tuple is empty since there's nothing to iterate. Note however
        // that the latter call to "ForEach()" itself will also handle
        // an empty tuple, since it will immediately return true if its
        // "N" template arg is zero (0). The following check for an
        // empty tuple would therefore seem to be redundant
        // (unnecessary), since "ForEach()" also correctly deals with
        // the situation. However, there's a subtle issue at play here
        // that requires us not to call "ForEach()" when the tuple is in
        // fact empty. Doing so would mean that the "operator()"
        // template member of the "processTupleType" functor we're
        // passing to "ForEach" below will be specialized on zero (its
        // "std::size_t" template arg will be zero), even though it will
        // never actually be called by "ForEach()" (since the tuple is
        // empty). "operator()" is still specialized on zero however
        // because when "ForEach()" is stamped out on the template args
        // we pass it, "IsForEachFunctor_v" is called to check if the
        // 2nd template arg of "ForEach()" is a valid functor type. This
        // occurs via a "static_assert" in C++17 or a concept in C++20
        // or later, but in either case "IsForEachFunctor_v" is called
        // and the implementation for this stamps out a copy of the
        // functor's "operator()" template arg, again, doing so with
        // template arg zero (0). "operator()" isn't actually called at
        // this time however, only stamped out (specialized on template
        // arg zero), simply to make sure the functor type being passed
        // is valid. If not then "IsForEachFunctor_v" will return false
        // and the "static_assert" in C++17 or concept in C++20 or later
        // will correctly trap the situation at compile time (i.e., an
        // invalid type is being passed for the 2nd template arg of
        // "ForEach"). However, if the 1st template arg of "ForEach",
        // namely "N" (the number of iterations it will be called for),
        // is zero itself, then although the functor's "operator()"
        // template member will never be called since "N" is zero as
        // noted (so there will be no iterations), "IsForEachFunctor_v"
        // itself will still stamp out "operator()" with its
        // "std::size_t" template arg set to zero (0) as just described.
        // Note that "IsForEachFunctor_v" has no choice but to stamp out
        // "operator()" since there's no other way in C++ for it do its
        // job at this writing (check if a type is a functor with a
        // template member, in this case "operator()" with a
        // "std::size_t" template arg).
        //
        // There's a problem with this situation however. Our
        // (identical) implementation of "operator()" seen in both the
        // lambda below (for C++20 and later), or in class
        // "Private::ProcessTupleType" (C++17 only), both call
        // "std::tuple_element_t<I, TupleT>", but if the "TupleT"
        // template are we're passing to this is empty, then it's
        // illegal to call it for any index including zero (since the
        // tuple is empty so even index zero is out of bounds). Template
        // "I" in this call (zero) is therefore an invalid index so
        // compilation will therefore fail (but not in C++17 for reasons
        // unknown - read on). The upshot is that when dealing with an
        // empty tuple, i.e., the "TupleT" template arg of the function
        // you're now reading is empty, we can't call "ForEach()"
        // because even though it won't iterate the functor it's being
        // passed (since the "N" template arg of "ForEach()" will be
        // zero in this case), the call to "IsForEachFunctor_v" that
        // occurs merely by stamping out "ForEach()" will cause the call
        // to "std::tuple_element_t<I, TupleT>" in our functor to choke
        // during compilation (since it's stamped out with its "I"
        // template arg set to zero but the "TupleT" template arg is
        // empty so "I" is an illegal index). Again, this occurs even
        // though the code is never actually called, but it's illegal
        // even to stamp it out in this case.
        //
        // Please note that as previously mentioned, this situation only
        // occurs in C++20 or later. In this case the lambda seen in the
        // code below would trigger the problem without explicit action
        // to circumvent it (which the following "if constexpr" does),
        // but in C++17 the problem doesn't occur. In C++17 the same
        // code seen in the lambda below exists but in
        // "Private::ProcessTupleType::operator()" instead (the lambda
        // below targets C++20 or later only), but for reasons unknown
        // (I haven't investigated), in C++17 no compilation error
        // occurs on the call to "std::tuple_element_t<I, TupleT>" (as
        // described). It's likely (presumably) not being stamped out
        // in this case. Testing shows however that if we allowed the
        // same code to be compiled in C++20, instead of the lambda
        // below (that we actually rely on in C++20 or later), it will
        // also fail. IOW, this whole situation surrounding
        // "std::tuple_element_t<I, TupleT>" only starts surfacing in
        // C++20 so something clearly has changed between C++17 and
        // C++20 (since what compiled without error in C++17 now fails
        // in C++20). This should be investigated but it's a moot point
        // for now since the following "if constexpr" statement prevents
        // the issue altogether.
        /////////////////////////////////////////////////////////////////
        if constexpr (std::tuple_size_v<TupleT> != 0)
        {
            // Lambda templates not supported until C++20
            #if defined(LAMBDA_TEMPLATES_SUPPORTED)
                ////////////////////////////////////////////////////////
                // Lambda template we'll be calling once for each type
                // in template arg "TupleT" (equivalent to the
                // "Private::ProcessTupleType" functor in C++17 or
                // earlier below). Each iteration invokes "functor",
                // where processing stops after all types in "TupleT"
                // have been processed or "functor" returns false,
                // whichever comes first  (false only returned if
                // "functor" wants to break like a normal "for" loop,
                // which rarely happens in practice so we usually
                // iterate all tuple types in "TupleT")
                //
                // IMPORTANT:
                // ---------
                // See the IMPORTANT comments about the capture of
                // "functor" in function template "ForEach()" (in the
                // comments preceding its own lambda template). The
                // same situation described there applies here as well.
                ////////////////////////////////////////////////////////
                const auto processTupleType = [&functor]<std::size_t I>()
                                              {
                                                  using TupleElement_t = std::tuple_element_t<I, TupleT>;

                                                  /////////////////////////////////////////////////////
                                                  // IMPORTANT:
                                                  // ---------
                                                  // See the IMPORTANT comments inside the lambda of
                                                  // function template "ForEach()" (just before its
                                                  // own call to "std::forward()"). The same situation
                                                  // described there applies here as well.
                                                  /////////////////////////////////////////////////////
                                                  return std::forward<ForEachTupleTypeFunctorT>(functor).template operator()<I, TupleElement_t>();
                                              };
            #else // C++17 (using our own functor instead, equivalent to above lambda) ...
                ////////////////////////////////////////////////////////////
                // Create an instance of "Private::ProcessTupleType", a
                // functor we'll be calling once for each type in template
                // arg "TupleT" (equivalent to the lambda in C++20 or later
                // above). Each iteration invokes "functor", where
                // processing stops after all types in "TupleT" have been
                // processed or "functor" returns false, whichever comes
                // first (false only returned if "functor" wants to break
                // like a normal "for" loop, which rarely happens in
                // practice so we usually iterate all types in "TupleT")
                //
                // Note: The "ForEachTupleTypeFunctorT" template arg we're
                // passing here as the 2nd template arg of class of
                // "Private::ProcessTupleType" means that the latter's
                // constructor will take the same arg type as "functor"
                // itself (& or && as usual). We then call "std::forward"
                // as seen to pass "functor" to the constructor. Note that
                // "std::forward" is mandatory here because "functor" is an
                // lvalue even in the && case (since it's named - named
                // rvalue references are still lvalues). Attempting to pass
                // "functor" directly would therefore cause a compiler
                // error since you can't bind an lvalue (functor) to an
                // rvalue reference (the arg type that the
                // "Private::ProcessTupleType" constructor is expecting in
                // this case).
                ////////////////////////////////////////////////////////////
                const Private::ProcessTupleType<TupleT, ForEachTupleTypeFunctorT> processTupleType(std::forward<ForEachTupleTypeFunctorT>(functor));
            #endif

            ///////////////////////////////////////////////////////////////
            // Defer to this function template further above, passing the
            // number of elements in the tuple as the function's template
            // arg, and "processTupleType" as the function's functor arg.
            // The "ForEachFunctorT" template arg of "ForEach()" is
            // therefore deduced to be the type of "processTupleType".
            // "ForEach()" will then invoke this functor once for each
            // element in the tuple given by "TupleT".
            ///////////////////////////////////////////////////////////////
            return ForEach<std::tuple_size_v<TupleT>>(processTupleType);
        }
        else
        {
            return true;
        }
    }
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    // For internal use only
    namespace Private
    {
        /////////////////////////////////////////////////////////////////////////
        // "FunctionHasQualifiers" (primary template). Assuming "T" satisfies
        // "std::is_function" (if not then the template's result isn't relevant
        // - read on), the primary template kicks in if "T" is an abominable
        // function, i.e., it has at least one cv-qualifier (const and/or
        // volatile) and/or a reference-qualifier (i.e. & or &&). If none are
        // present then "T" is a free function so the specialization (template)
        // just below kicks in instead. Note that this template is only invoked
        // by "IsFreeFunction_v" and "IsAbominableFunction_v" below (for their
        // private use), each of which also calls "std::is_function" on "T", so
        // the following template and its specialization below are only relevant
        // when "T" actually satisfies "std::is_function". If not then both
        // "IsFreeFunction_v" and "IsAbominableFunction_v" will always return
        // false since "T" doesn't even qualify as a function, so the result of
        // the following template and its specialization are then irrelevant
        // (i.e., if "T" doesn't actually satisfy "std::is_function" then the
        // result of this template is immaterial since both "IsFreeFunction_v"
        // and "IsAbominableFunction_v" will return false regardless).
        /////////////////////////////////////////////////////////////////////////
        template <typename T,
                  typename = void> // Arg for internal use only (users should never explicitly pass it)
        struct FunctionHasQualifiers : std::true_type
        {
        };

        /////////////////////////////////////////////////////////////////////////
        // FunctionHasQualifiers (specialization of above template). Assuming
        // "T" satisfies "std::is_function" (if not then the template's result
        // isn't relevant - see primary template above for details), the
        // following specialization kicks in if "T" is a free function, i.e., it
        // has no cv-qualifiers or ref-qualifiers. Code works by simply
        // attempting to add a pointer to "T", resulting in "T *". When "T"
        // satisfies "std::is_function" however then this will only work if "T"
        // has no cv-qualifiers or ref-qualifiers, i.e., it's a free function.
        // If it has any cv-qualifiers or ref-qualifiers however, i.e., it's an
        // "abominable" function, then adding a pointer to it is illegal in C++
        // so it will immediately fail SFINAE and the primary template will
        // therefore kick in instead. Otherwise, adding a pointer to "T" will
        // succeed meaning "T" has no cv-qualifiers so it's a free function as
        // noted. See section 2.2 here:
        //
        //     Abominable Function Types (by Alisdair Meredith)
        //     https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html
        //
        // You can't create a pointer to a function that has any of these
        // qualifiers (note that we're not dealing with pointer-to-member syntax
        // here which does allow it, but pointers to the actual raw (native) C++
        // function type which doesn't allow it).
        /////////////////////////////////////////////////////////////////////////
        template <typename T>
        struct FunctionHasQualifiers<T,
                                     std::void_t<T *> // Succeeds only if "T" is a free function (see comments above)
                                    > : std::false_type
        {
        };

        template <typename T>
        inline constexpr bool FunctionHasQualifiers_v = FunctionHasQualifiers<T>::value;
    } // namespace Private

    ///////////////////////////////////////////////////////////////////////////////
    // "IsFreeFunction_v". Returns true if "T" is a free function, which also
    // includes static member functions ("free" will refer to both from here on),
    // or false otherwise. In order to understand how "IsFreeFunction_v" works,
    // you first need to understand C++ function type syntax. Note that function
    // types in C++ are structured as follows (square brackets indicating optional
    // items - note that this syntax isn't 100% precise but close enough for this
    // discussion):
    //
    //   ReturnType [CallingConvention] (Args [, ...]) [const] [volatile] [&|&&] [noexcept[(true|false)]]
    //
    // All function types including non-static member functions are typed this
    // way. For non-static member functions however, when dealing with situations
    // involving their type, normally developers target them using
    // pointer-to-member syntax but the function's underlying type is still
    // structured as seen above (no different than for free functions). This may
    // surprise many not used to seeing the actual type of a non-static member
    // function since it's rarely encountered this way in most real-world code. If
    // you have the following for instance:
    //
    //     class Whatever
    //     {
    //     public:
    //         int SomeFunc(float) const;
    //     };
    //
    // And you need to deal with "SomeFunc" based on its type, you would normally
    // do this:
    //
    //     using SomeFuncType = decltype(&Whatever::SomeFunc);
    //
    // "SomeFuncType" therefore resolves to the following:
    //
    //     int (Whatever::*)(float) const;
    //
    // You can then invoke "SomeFunc" using this pointer, the usual reason you
    // need to work with its type. However, this is a pointer type (to a member of
    // "Whatever" that happens to be a function), not the actual type of
    // "SomeFunc" itself, which is actually this:
    //
    //     int (float) const;
    //
    // The above syntax follows the general structure of all functions described
    // further above. It's also what "std::is_function" takes as its template arg
    // and it will return true in this case, i.e., you can do this:
    //
    //     using F = int (float) const;
    //     constexpr bool isFunction = std::is_function_v<F>; // true
    //
    // However, the above appears to be a free function but it can't be since a
    // free function can't be "const". However, if you remove the "const":
    //
    //     using F = int (float);
    //     constexpr bool isFunction = std::is_function_v<F>; // true
    //
    // Now it really appears to be a free function type even though it was a
    // non-static member function type before removing the "const", so how can you
    // tell the difference. In fact you can't distinguish between this non-static
    // member function type (which it was just before removing the "const" so in
    // theory still is), and a free function that has the same type. They're both
    // the same, in this case a function taking a "float" and returning an "int".
    // Without knowing if it came from a class or not it's just a function type
    // and that's all. You can therefore have a free function with this type and a
    // non-static member function with the same type. The (major) difference is
    // that a non-static member function must be wired to the class it belongs to
    // before you can invoke it, and the usual pointer-to-member syntax allows
    // developers to do so. Nevertheless, the function's actual type (syntax) is
    // still that of a normal (non-static) function, as emphasized. In the wild
    // it's rarely ever encountered this way however as previously noted (for a
    // non-static member function). The context in which the actual (non-pointer)
    // type of a non-static member function needs to be dealt with rarely ever
    // comes up for most developers, though there are some rare circumstances when
    // it can (but I won't bother showing an example here - few will ever come
    // across it). In any case, for purposes of "IsFreeFunction_V" we're creating
    // here, we want to return "true" when template arg "T" refers to a "free"
    // function but not a non-static member function. Since there's no way to
    // distinguish between the two however when targeting a function like so:
    //
    //     using F = int (float);
    //
    // Then for purposes of "IsFreeFunction_v" we simply always assume that it is
    // a free function (but read on). It normally will be in the real world
    // because this syntax is almost always used exclusively to deal with free
    // functions, opposed a non-static member functions, where pointer-to-member
    // syntax is normally used instead. Nevertheless, there are ways of getting
    // hold of a non-static member function's type though it usually takes some
    // extra effort. If you have such a type however and pass it for template arg
    // "T", "IsFreeFunction_v" will appear to erroneously return "true", at least
    // for the case just above. It would therefore (seemingly) misidentify it as a
    // free function but it should be understood that the intention of
    // "IsFreeFunction_v" isn't to identify whether "T" originated from a free
    // function or not. Its purpose is to determine if it *qualifies* as a "free"
    // function even if "T" originated from a non-static member function. So long
    // as it represents a valid free function type, then "IsFreeFunction_v" will
    // return true, even if "T" actually came from a non-static member function
    // (with the same type). "IsFreeFunction_v" doesn't care where it originates
    // from IOW, only that it represents a valid free function. Whether it
    // actually came from one or not is immaterial since the origin of "T" doesn't
    // matter.
    //
    // One might therefore ask, why not just rely on "std::is_function" instead.
    // The difference is that "std::is_function" will return true for *any*
    // function type, even if the type you pass has a cv-qualifier and/or
    // reference-qualifier (again, see the general syntax of functions earlier in
    // these comments). The cv-qualifiers ("const" or "volatile") and
    // reference-qualifiers (& and &&) aren't legal qualifiers on a free function
    // so if you have any of these ...
    //
    //     using F1  = int (float) const [noexcept];
    //     using F2  = int (float) volatile [noexcept];
    //     using F3  = int (float) const volatile [noexcept];
    //     using F4  = int (float) & [noexcept];
    //     using F5  = int (float) const & [noexcept];
    //     using F6  = int (float) volatile & [noexcept];
    //     using F7  = int (float) const volatile & [noexcept];
    //     using F8  = int (float) && [noexcept];
    //     using F9  = int (float) const && [noexcept];
    //     using F10 = int (float) volatile && [noexcept];
    //     using F11 = int (float) const volatile && [noexcept];
    //
    // ... then none of them qualify as a free function. "std::is_function" will
    // still return true for these however since they're still valid functions.
    // "IsFreeFunction_v" won't however, and that's the difference between the
    // two. It will return "false" instead since they don't qualify as free
    // functions. It's illegal in C++ to use them in any context where they might
    // be identified as free functions. That's the only difference between
    // "std::is_function" and "IsFreeFunction_v". For all other function types,
    // namely, those that don't include the above (cv and reference) qualifiers,
    // "std::is_function" and "IsFreeFunction_v" will both return the same true or
    // false value. You should therefore call "IsFreeFunction_v" when you need to
    // filter out functions with the above qualifiers. If "IsFreeFunction_v"
    // returns true, it not only means that "T" is a function type (passing a
    // non-function type like "int" for instance would obviously return false, as
    // would pointers/references to functions which are pointer/reference types,
    // not actual function types), but that it also qualifies as a free function
    // as well, meaning it has none of the above qualifiers. If "IsFreeFunction_v"
    // returns false however, it either means "T" isn't a function whatsoever
    // (again, if you pass a non-function type like "int" or even a function
    // pointer or reference for instance), or "T" *is* a function but it has one
    // of the above qualifiers so therefore isn't a "free" function (doesn't
    // qualify as one). Note that function types with cv and reference qualifiers
    // are informally known as "abominable" function types. They are described
    // here at this writing (note that the author, Alisdair Meredith, is the C++
    // Standard Committee Library Working Group chair at this writing):
    //
    //     Abominable Function Types
    //     https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html
    //
    // "IsFreeFunction_v" therefore simply filters out abominable function types,
    // returning false if "T" is in fact an abominable function (or a non-function
    // type altogether, including pointers and references to functions). For all
    // other functions (actual function types only, again, not pointer and
    // references to functions), it returns true which is the definition of a
    // "free" function in this context (a function type that's not an abominable
    // function).
    //
    // Lastly, see "FunctionHasQualifiers" just below for details on how to
    // actually determine if the above qualifiers are present or not (i.e., how we
    // filter out abominable functions)
    ///////////////////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsFreeFunction_v = std::is_function_v<T> &&
                                             !Private::FunctionHasQualifiers_v<T>;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept FreeFunction_c = IsFreeFunction_v<T>;
        #endif

        #define FREE_FUNCTION_C StdExt::FreeFunction_c
        #define STATIC_ASSERT_IS_FREE_FUNCTION(T)
    #else
        #define FREE_FUNCTION_C typename
        #define STATIC_ASSERT_IS_FREE_FUNCTION(T) static_assert(StdExt::IsFreeFunction_v<T>, \
                                                                QUOTE_STRINGIFY(T) " must be a free function type, " \
                                                                "i.e., it must satisfy \"std::is_function\" but must " \
                                                                "not have any cv-qualifiers or ref-qualifiers (i.e., " \
                                                                "it can't be an \"abominable\" function)");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    template <typename T>
    inline constexpr bool IsFreeFunctionPointer_v = std::is_pointer_v<T> && // Note: Returns false for non-static member
                                                                            // pointers (to functions or data) as well as
                                                                            // for abominable functions (since pointers to
                                                                            // abominable functions aren't legal in the
                                                                            // language so "T" can never be one). Returns
                                                                            // true for static member pointers however (to
                                                                            // functions or data).
                                                    std::is_function_v<std::remove_pointer_t<T>>; // Note: "std::remove_pointer_t" doesn't remove
                                                                                                  // the pointer for non-static member pointers so
                                                                                                  // "std::is_function_v" will always return false
                                                                                                  // for such pointers (but it's irrelevant here
                                                                                                  // anyway since the call just above always returns
                                                                                                  // false for such pointers as well)

    ///////////////////////////////////////////////////////////////////////
    // IsAbominableFunction_v. Checks if template arg "T" is an abominable
    // function. See comments in "IsFreeFunction_v". If the following
    // returns true then as described in the latter comments, "T" must be
    // one of the following types (using the function "int (float)" here
    // as an example to show all possible qualifiers)
    //
    //    using T1  = int (float) const [noexcept];
    //    using T2  = int (float) volatile [noexcept];
    //    using T3  = int (float) const volatile [noexcept];
    //    using T4  = int (float) & [noexcept];
    //    using T5  = int (float) const & [noexcept];
    //    using T6  = int (float) volatile & [noexcept];
    //    using T7  = int (float) const volatile & [noexcept];
    //    using T8  = int (float) && [noexcept];
    //    using T9  = int (float) const && [noexcept];
    //    using T10 = int (float) volatile && [noexcept];
    //    using T11 = int (float) const volatile && [noexcept];
    ///////////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsAbominableFunction_v = std::is_function_v<T> &&
                                                   Private::FunctionHasQualifiers_v<T>;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept AbominableFunction_c = IsAbominableFunction_v<T>;
        #endif

        #define ABOMINABLE_FUNCTION_C StdExt::AbominableFunction_c
        #define STATIC_ASSERT_IS_ABOMINABLE_FUNCTION(T)
    #else
        #define ABOMINABLE_FUNCTION_C typename
        #define STATIC_ASSERT_IS_ABOMINABLE_FUNCTION(T) static_assert(IsAbominableFunction_v<T>, \
                                                                      QUOTE_STRINGIFY(T) " must be an abominable function " \
                                                                      "type, i.e., it must satisfy \"std::is_function\" and " \
                                                                      "have at least one cv-qualifier and/or ref-qualifier");
    #endif // #if defined(USE_CONCEPTS)

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept MemberFunctionPointer_c = std::is_member_function_pointer_v<T>;
        #endif

        #define MEMBER_FUNCTION_POINTER_C StdExt::MemberFunctionPointer_c
        #define STATIC_ASSERT_IS_MEMBER_FUNCTION_POINTER(T)
    #else
        #define MEMBER_FUNCTION_POINTER_C typename
        #define STATIC_ASSERT_IS_MEMBER_FUNCTION_POINTER(T) static_assert(std::is_member_function_pointer_v<T>, \
                                                                          QUOTE_STRINGIFY(T) " must be a member function pointer, " \
                                                                          "i.e., it must satisfy \"std::is_member_function_pointer\"");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    namespace Private
    {
        template <typename T>
        inline constexpr bool IsFreeOrMemberFunctionPointer_v = IsFreeFunctionPointer_v<T> ||
                                                                std::is_member_function_pointer_v<T>;

        #if defined(USE_CONCEPTS)
            template <typename T>
            concept FreeOrMemberFunctionPointer_c = IsFreeOrMemberFunctionPointer_v<T>;

            // For internal use only (we #undef both later)
            #define FREE_OR_MEMBER_FUNCTION_POINTER_C StdExt::Private::FreeOrMemberFunctionPointer_c
            #define STATIC_ASSERT_IS_FREE_OR_MEMBER_FUNCTION_POINTER(T)
        #else
            // For internal use only (we #undef both later)
            #define FREE_OR_MEMBER_FUNCTION_POINTER_C typename
            #define STATIC_ASSERT_IS_FREE_OR_MEMBER_FUNCTION_POINTER(T) static_assert(StdExt::Private::IsFreeOrMemberFunctionPointer_v<T>, \
                                                                                      QUOTE_STRINGIFY(T) " must be a free or member " \
                                                                                      "function pointer");
        #endif // #if defined(USE_CONCEPTS)
    } // namespace Private

    ///////////////////////////////////////////////////////////////////
    // "IsNonOverloadedFunctor" (primary template ). Primary template
    // inherits from "std::false_type" if "T" is *not* a
    // non-overloaded, non-static functor. Otherwise, if it is a
    // non-overloaded, non-static functor then the specialization just
    // below kicks in instead (inheriting from "std::true_type"). In
    // this case (i.e., the specialization kicks in) then by
    // definition "T" must be a class or struct with a non-overloaded,
    // non-static member function called "operator()". This will
    // usually be non-static but static versions of "operator()" were
    // legalized in C++23 though this template only handles (detects)
    // non-static functors. Note that if non-static "T::operator()"
    // exists but is overloaded then the primary template will kick in
    // since which "operator()" to use becomes ambiguous. In this case
    // you'll have to target the specific "operator()" overload you're
    // interested by taking its address and doing a "static_cast" to
    // the exact signature you're interested in (in order to
    // disambiguate it for whatever purpose you require). You can't
    // rely on this template IOW since it will fail to identify "T" as
    // a functor due to the ambiguity (hence the template's name - it
    // can detect non-overloaded functors only).
    ///////////////////////////////////////////////////////////////////
    template <typename T,
              typename = void> // Arg for internal use only (users should never explicitly pass it)
    struct IsNonOverloadedFunctor : std::false_type
    {
    };

    /////////////////////////////////////////////////////////////////
    // Specialization of "IsNonOverloadedFunctor" (primary) template
    // just above. This specialization does all the work. See
    // primary template above for details.
    /////////////////////////////////////////////////////////////////
    template <typename T>
    struct IsNonOverloadedFunctor<T,
                                  std::enable_if_t< AlwaysTrue_v<decltype(&T::operator())> &&  // True if "T" is a class with a non-overloaded "operator()" member
                                                    std::is_member_function_pointer_v<decltype(&T::operator())> // True if "operator()" is a *non-static* member function
                                                                                                                // (static versions were legalized in C++23 but we don't
                                                                                                                // detect those here, only non-static versions)
                                                  >
                                 > : std::true_type
    {
    };

    // Usual "_v" helper variable for above template
    template <typename T>
    inline constexpr bool IsNonOverloadedFunctor_v = IsNonOverloadedFunctor<T>::value;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept NonOverloadedFunctor_c = IsNonOverloadedFunctor_v<T>;
        #endif

        #define NON_OVERLOADED_FUNCTOR_C StdExt::NonOverloadedFunctor_c
        #define STATIC_ASSERT_IS_NON_OVERLOADED_FUNCTOR(T)
    #else
        #define NON_OVERLOADED_FUNCTOR_C typename
        #define STATIC_ASSERT_IS_NON_OVERLOADED_FUNCTOR(T) static_assert(StdExt::IsNonOverloadedFunctor_v<T>, \
                                                                         "\"T\" must be a non-overloaded, non-static functor (i.e., " \
                                                                         "a class or struct with a non-overloaded, non-static " \
                                                                         "member function called operator \"operator()\")");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    ///////////////////////////////////////////////////////////////////
    // "IsNonOverloadedStaticFunctor" (primary template ). Primary
    // template inherits from "std::false_type" if "T" is *not* a
    // non-overloaded, static functor. Otherwise, if it is a
    // non-overloaded, static functor then the specialization just
    // below kicks in instead (inheriting from "std::true_type"). In
    // this case (i.e., the specialization kicks in) then by
    // definition "T" must be a class or struct with a non-overloaded,
    // static member function called "operator()". This will usually
    // be non-static but static versions of "operator()" were
    // legalized in C++23, which this template detects (never
    // non-static functors). Note that if static "T::operator()"
    // exists but is overloaded then the primary template will kick in
    // since which "operator()" to use becomes ambiguous. In this case
    // you'll have to target the specific "operator()" overload you're
    // interested by taking its address and doing a "static_cast" to
    // the exact signature you're interested in (in order to
    // disambiguate it for whatever purpose you require). You can't
    // rely on this template IOW since it will fail to identify "T" as
    // a functor due to the ambiguity (hence the template's name - it
    // can detect non-overloaded functors only).
    ///////////////////////////////////////////////////////////////////
    template <typename T,
              typename = void> // Arg for internal use only (users should never explicitly pass it)
    struct IsNonOverloadedStaticFunctor : std::false_type
    {
    };

    ///////////////////////////////////////////////////////////////
    // Specialization of "IsNonOverloadedStaticFunctor" (primary)
    // template just above. This specialization does all the work.
    // See primary template above for details.
    ///////////////////////////////////////////////////////////////
    template <typename T>
    struct IsNonOverloadedStaticFunctor<T,
                                        std::enable_if_t< AlwaysTrue_v<decltype(&T::operator())> && // True if "T" is a class with a non-overloaded "operator()" member
                                                          IsFreeFunctionPointer_v<decltype(&T::operator())> // True if "operator()" is a *static* member function
                                                                                                            // (static versions of "operator()" were legalized in
                                                                                                            // C++23 - we only detect static versions of this here,
                                                                                                            // not non-static versions)
                                                        >
                                       > : std::true_type
    {
    };

    // Usual "_v" helper variable for above template
    template <typename T>
    inline constexpr bool IsNonOverloadedStaticFunctor_v = IsNonOverloadedStaticFunctor<T>::value;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept NonOverloadedStaticFunctor_c = IsNonOverloadedStaticFunctor_v<T>;
        #endif

        #define NON_OVERLOADED_STATIC_FUNCTOR_C StdExt::NonOverloadedStaticFunctor_c
        #define STATIC_ASSERT_IS_NON_OVERLOADED_STATIC_FUNCTOR(T)
    #else
        #define NON_OVERLOADED_STATIC_FUNCTOR_C typename
        #define STATIC_ASSERT_IS_NON_OVERLOADED_STATIC_FUNCTOR(T) static_assert(StdExt::IsNonOverloadedStaticFunctor_v<T>, \
                                                                                "\"T\" must be a non-overloaded, static functor (i.e., " \
                                                                                "a class or struct with a non-overloaded, static " \
                                                                                "member function called operator \"operator()\"). Note " \
                                                                                "that static versions of \"operator()\" were legalized " \
                                                                                "in C++23.");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    ////////////////////////////////////////////////////////
    // Variable template returning true if "T" is a
    // "std::function" specialization or "false" otherwise.
    // Note that "std::function" derivatives always return
    // false, since in the general case, types derived from
    // a specialization don't qualify as the specialization
    // itself, they only inherit from it (subtle but
    // meaningful difference). "std::function" derivatives
    // aren't normally valid in C++ anyway, since the
    // "std::function" destructor isn't virtual so
    // developers shouldn't normally derive from it (this
    // library never does).
    ////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsStdFunctionSpecialization_v = IsSpecialization_v<T, std::function>;
#endif

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept StdFunction_c = IsStdFunctionSpecialization_v<T>;
        #endif

        #define STD_FUNCTION_C StdExt::StdFunction_c
        #define STATIC_ASSERT_IS_STD_FUNCTION(T)
    #else
        #define STD_FUNCTION_C typename
        #define STATIC_ASSERT_IS_STD_FUNCTION(T) static_assert(StdExt::IsStdFunctionSpecialization_v<T>, \
                                                               QUOTE_STRINGIFY(T) " must be a \"std::function\" specialization");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    /////////////////////////////////////////////////////////////////
    // StdFunctionTemplateArg_t. Returns the type that template arg
    // T, which must be a "std::function" specialization, was
    // specialized on (i.e., returns the function type that was
    // passed as the template arg to "std::function" itself, which
    // is the function type that it wraps). Legal types that can be
    // passed to "std::function" are types that satisfy
    // "std::is_function" excluding the following:
    //
    //   1) noexcept functions
    //   2) Variadic functions
    //   3) Abominable functions
    //   4) Functions with calling conventions that differ from the
    //      default (so if a calling convention is explicitly
    //      present in some function then it must match the default
    //      calling convention or all bets are off). Calling
    //      conventions aren't part of the C++ standard itself so
    //      "std::function" doesn't officially handle any that
    //      differ from the default on the calling platform.
    //
    // Note that some compilers may allow "std::function" to be
    // specialized on the excluded functions above but it's not
    // sanctioned by the C++ standard itself (so developers
    // shouldn't normally pass such functions since it's technically
    // unsupported in C++ even if some compilers permit it).
    /////////////////////////////////////////////////////////////////
    template <STD_FUNCTION_C T>
    using StdFunctionTemplateArg_t = TemplateArg_t<T, std::function>;

    ///////////////////////////////////////////////////////////////////////////
    // Declarations mostly associated with struct "FunctionTraits" declared
    // later on (the latter struct itself plus its various base classes and
    // other support declarations including helper templates mostly associated
    // with "FunctionTraits"). "FunctionTraits" itself is used to retrieve
    // information about any function including (most notably) its return type
    // and the type of any of its arguments (write traits also exist in
    // addition to read traits). Less frequently used traits are also
    // available. Simply pass the function's type as the template's only
    // argument (see next paragraph), but see the helper alias templates
    // "ReturnType_t" and "ArgType_t" declared later on for an even easier
    // (less verbose) way to access the return type and arg types of any
    // function (a full complement of helper templates exist for every
    // possible trait). The helper templates take the same template arg and
    // simply wrap the appropriate aliases of "FunctionTraits" demo'd in the
    // example below. They're easier to use than "FunctionTraits" directly
    // however and should normally be relied on. The example below shows how
    // to do it using "FunctionTraits" directly but the helper aliases should
    // normally be used instead.
    //
    // Please note that template arg F can be any legal C++ syntax for a
    // function's type, so it can refer to free functions, class member
    // functions, pointers and references to functions (references legal in
    // C++ for free functions only, including static member functions),
    // functors (classes with "operator()" declared though overloads are
    // ambiguous so aren't supported - a compiler error will occur), functions
    // with different calling conventions (__cdecl, __stdcall, etc. but read
    // on), "noexcept", "const" and "volatile" for non-static member
    // functions, lvalue and rvalue references declared on non-static member
    // functions (though these are rarely used in practice), and variadic
    // functions (those whose last arg is "..."). A specialization therefore
    // exists for every permutation of the above, noting that if the primary
    // template itself kicks in then no specialization exists for template
    // argument "F". It means that "F" is not a function type so a compiler
    // error will occur (our own concept will trigger in C++20 or greater, or
    // a compiler-specific "function template not defined" error of some kind
    // for C++17 - both are SFINAE-friendly). Lastly, please note that calling
    // conventions like "stdcall", etc. may be replaced by compilers with
    // "cdecl" under certain circumstances, in particular when compiling for
    // 64 bit environments (opposed to 32 bit). That is, if you declare a
    // function with a particular calling convention other than "cdecl"
    // itself, your compiler might change it to "cdecl" depending on your
    // compile-time environment (though exceptions may exist however, such as
    // the "vectorcall" calling convention which remains unchanged in some 64
    // bit environments). "FunctionTraits" will therefore reflect this so its
    // "CallingConvention" member will correctly indicate "cdecl", not the
    // calling convention your function is actually declared with (since the
    // compiler changed it). Note that a given compiler may or may not produce
    // warnings about this situation when it quietly replaces your calling
    // convention with "cdecl".
    //
    //     Example
    //     -------
    //     ////////////////////////////////////////////////////////////////
    //     // Free function (member functions and non-overloaded functors
    //     // also supported - see comments above)
    //     ////////////////////////////////////////////////////////////////
    //     float SomeFunction(const std::string &arg1, double arg2, int arg3);
    //
    //     /////////////////////////////////////////////////////////////
    //     // Apply "FunctionTraits" to above function (note: template
    //     // arg "SomeFunction" in the following can optionally be
    //     // "&SomeFunction", i.e., pointers to functions are also
    //     // supported as are references to functions though they're
    //     // not widely used in practice - note that references to
    //     // functions only apply to free functions however since
    //     // references to non-static member functions aren't supported
    //     // in C++ itself)
    //     ////////////////////////////////////////////////////////////
    //     using SomeFuncTraits = FunctionTraits<decltype(SomeFunction)>;
    //     // using SomeFuncTraits = FunctionTraits<decltype(&SomeFunction)>; Same as above but using a function pointer
    //     // using SomeFuncTraits = FunctionTraits<float (const std::string &, double, int)>; // Same as above but manually passing the
    //                                                                                         // signature (just to show we can)
    //
    //     using ReturnType_t = typename SomeFuncTraits::ReturnType_t; // Function's return type (float)
    //     using Arg3Type_t = typename SomeFuncTraits::template ArgType_t<2>; // Type of "arg3" (int)
    //
    //     ////////////////////////////////////////////////////////////////
    //     // Type of "arg3" returned as a "tstring_view" (so returns
    //     // "int" (literally) as a "tstring_view", quotes not included)
    //     ////////////////////////////////////////////////////////////////
    //     constexpr tstring_view arg3TypeName = TypeName_v<Arg3Type_t>;
    //
    // The above example demonstrates how to retrieve the return type and
    // the type of any argument (3rd argument in this example). Overall
    // you can retrieve the following info:
    //
    //     1) Function's return type
    //     2) Function's argument types. Note that for formal parameters
    //        declared as pass by value (opposed to reference), the "const"
    //        keyword (if present) isn't included in the arguments's type
    //        (the compiler removes it which prevents us from returning it,
    //        but still respects it if you try to change the parameter in
    //        the function, though that has nothing to do with this template)
    //     3) Whether the function is variadic (i.e., its last arg is "...")
    //     4) Whether the function has been declared "noexcept"
    //     5) The function's calling convention (__cdecl, __stdcall, etc.)
    //     6) For non-static member functions only, whether the function is
    //        declared "const" (since latter doesn't apply to static member
    //        functions)
    //     7) For non-static member functions only, whether the function is
    //        declared "volatile" (since latter doesn't apply to static member
    //        functions)
    //     8) For non-static member functions only, whether the function is
    //        declared with a reference-qualifier ""& or "&&" (since latter
    //        doesn't apply to static member functions)
    //     9) For non-static member functions only, the class the function
    //        belongs to (not available for *static* member functions
    //        unfortunately since compilers don't provide this information in
    //        the partial specializations "FunctionTraits" rely on - the class
    //        for static member functions simply isn't available, unlike for
    //        non-static member functions)
    //
    // Note that the following function info *isn't* available however
    // because there are no C++ trait techniques that support it (that
    // I'm aware of at this writing). Looked into it but couldn't find
    // (ergonomically correct) any way to do it using either the language
    // itself or any compiler tricks (such as a predefined macro - none
    // provide the required info).
    //
    //     1) Whether a non-static member function is declared virtual
    //     2) Whether a non-static member function is declared with the "override"
    //        keyword
    //     3) The class (or struct) of a static member function
    //
    // IMPORTANT: The techniques below are almost entirely based on Raymond
    // Chen's work from MSFT, but I've modified them to deal with various
    // issues that he didn't (like handling member functions among other
    // things). Surprisingly his work only deals with free functions (again,
    // I added support for member functions), but there were also some other
    // issues. Note that Raymond wrote a series of 7 articles in his
    // (well-known) blog showing how he developed this code. The articles are
    // as follows (in the order he posted them):
    //
    //     Deconstructing function pointers in a C++ template
    //     https://devblogs.microsoft.com/oldnewthing/20200713-00/?p=103978
    //
    //     Deconstructing function pointers in a C++ template, the noexcept complication
    //     https://devblogs.microsoft.com/oldnewthing/20200714-00/?p=103981
    //
    //     Deconstructing function pointers in a C++ template, vexing variadics
    //     https://devblogs.microsoft.com/oldnewthing/20200715-00/?p=103984
    //
    //     Deconstructing function pointers in a C++ template, the calling convention conundrum
    //     https://devblogs.microsoft.com/oldnewthing/20200716-00/?p=103986
    //
    //     Deconstructing function pointers in a C++ template, trying to address the calling convention conundrum
    //     https://devblogs.microsoft.com/oldnewthing/20200717-00/?p=103989
    //
    //     Deconstructing function pointers in a C++ template, addressing the calling convention conundrum
    //     https://devblogs.microsoft.com/oldnewthing/20200720-00/?p=103993
    //
    //     Deconstructing function pointers in a C++ template, returning to enable_if
    //     https://devblogs.microsoft.com/oldnewthing/20200721-00/?p=103995
    //
    // Unfortunately he didn't consolidate all this into one long article to
    // make it easier to read (they're just individual blogs), nor post his
    // code in a downloadable file for immediate use. Anyone implementing his
    // code therefore has to copy and paste it but also has to read through
    // the articles to understand things (in order to make it production-ready).
    //
    // Note that he also doesn't #include the few standard C++ headers his
    // code relies on either but none of this matters to anyone using the code
    // below. All the work's been done so you don't need to understand the
    // details, just how to use the "FunctionTraits" template itself (see
    // example further above)
    ///////////////////////////////////////////////////////////////////////////

    // For internal use only
    namespace Private
    {
        //////////////////////////////////////////////////
        // Helper variable returning "true" if "T" is a
        // non-static member function pointer with no
        // cv-qualifiers on the pointer itself (opposed
        // to the non-static member function it points
        // to), or false otherwise
        //////////////////////////////////////////////////
        template <typename T>
        inline constexpr bool IsMemberFunctionNonCvPointer_v = std::is_member_function_pointer_v<T> && // Note that "std::is_member_function_pointer_v"
                                                                                                       // returns true for cv-qualified pointers as well
                                                               !IsConstOrVolatile_v<T>;

        //////////////////////////////////////////////
        // Concept for above template (see following
        // #defined constant for details)
        //////////////////////////////////////////////
        #if defined(USE_CONCEPTS)
            template <typename T>
            concept MemberFunctionNonCvPointer_c = IsMemberFunctionNonCvPointer_v<T>;

            // For internal use only (we #undef both later)
            #define MEMBER_FUNCTION_NON_CV_POINTER_C StdExt::Private::MemberFunctionNonCvPointer_c
            #define STATIC_ASSERT_IS_MEMBER_FUNCTION_NON_CV_POINTER(T)
        #else
            // For internal use only (we #undef both later)
            #define MEMBER_FUNCTION_NON_CV_POINTER_C typename
            #define STATIC_ASSERT_IS_MEMBER_FUNCTION_NON_CV_POINTER(T) static_assert(StdExt::Private::IsMemberFunctionNonCvPointer_v<T>, \
                                                                                     QUOTE_STRINGIFY(T) " must be a non-cv-qualified pointer " \
                                                                                     "to a non-static member function (i.e., it must satisfy " \
                                                                                     "\"std::is_member_function_pointer_v\"");
        #endif // #if defined(USE_CONCEPTS)
    } // namespace Private

    /////////////////////////////////////////////////////////////////
    // IsTraitsFreeFunction_v. Variable template set to true if "T"
    // is any of the following or false otherwise:
    //
    //   1) A free function including static member functions (but
    //      not abominable functions which always return false -
    //      see https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html
    //   2) An (optionally cv-qualified) pointer to a free function
    //      (1 above)
    //   3) A reference to a free function (1 above)
    //   4) A reference to an (optionally cv-qualified) pointer to a
    //      free function (2 above)
    /////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsTraitsFreeFunction_v = IsFreeFunction_v< RemovePtrRef_t<T> >;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept TraitsFreeFunction_c = IsTraitsFreeFunction_v<T>;
        #endif

        #define TRAITS_FREE_FUNCTION_C StdExt::TraitsFreeFunction_c
        #define STATIC_ASSERT_IS_TRAITS_FREE_FUNCTION(T)
    #else
        #define TRAITS_FREE_FUNCTION_C typename
        #define STATIC_ASSERT_IS_TRAITS_FREE_FUNCTION(T) static_assert(StdExt::IsTraitsFreeFunction_v<T>, \
                                                                       "\"T\" must be a free function which includes static member " \
                                                                       "functions (non-static member functions are always excluded), " \
                                                                       "pointers and references to free functions, and references " \
                                                                       "to pointers to free functions. In all cases the underlying " \
                                                                       "function must satisfy \"std::is_function\" but not have any " \
                                                                       "cv-qualifiers or ref-qualifiers (the definition of a \"free\" " \
                                                                       "function in this library - functions with such qualifiers " \
                                                                       "are informally known as \"abominable\" functions which don't " \
                                                                       "qualify as \"free\" functions).");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    ///////////////////////////////////////////////////////////////////
    // IsTraitsMemberFunction_v. Variable template set to true if "T"
    // is either of the following or false otherwise:
    //
    //   1) An (optionally cv-qualified) pointer to a non-static member
    //      function
    //   2) A reference to an (optionally cv-qualified) pointer to a
    //      non-static member function
    //
    // Note that references to non-static member functions aren't
    // supported by C++ so should never be encountered. You can have
    // references to pointers to non-static members functions (hence
    // 2 above), but not references to the actual non-static member
    // function itself.
    ///////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsTraitsMemberFunction_v = std::is_member_function_pointer_v< std::remove_reference_t<T> >; // Note that "std::is_member_function_pointer_v"
                                                                                                                      // returns true for cv-qualified pointers as well
                                                                                                                      // (so no need for us to call "remove_cv_t" before
                                                                                                                      // invoking "std::is_member_function_pointer_v)
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept TraitsMemberFunction_c = IsTraitsMemberFunction_v<T>;
        #endif

        #define TRAITS_MEMBER_FUNCTION_C StdExt::TraitsMemberFunction_c
        #define STATIC_ASSERT_IS_TRAITS_MEMBER_FUNCTION(T)
    #else
        #define TRAITS_MEMBER_FUNCTION_C typename
        #define STATIC_ASSERT_IS_TRAITS_MEMBER_FUNCTION(T) static_assert(StdExt::IsTraitsMemberFunction_v<T>, \
                                                                         QUOTE_STRINGIFY(T) " must be an (optionally " \
                                                                         "cv-qualified) non-static member function pointer " \
                                                                         "or a reference to one (in either case the pointer " \
                                                                         "must satisfy \"std::is_member_function_pointer_v\")");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    namespace Private
    {
        ///////////////////////////////////////////////////////////////////
        // IsTraitsFreeOrMemberFunction_v. Variable template set to "true"
        // if "T" is any of the following or false otherwise:
        //
        //   1) A free function including static member functions (but
        //      abominable function types always return false by design -
        //      see https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html
        //   2) An (optionally cv-qualified) pointer to a free function
        //   3) A reference to a free function
        //   4) A reference to an (optionally cv-qualified) pointer to a free
        //      function
        //   5) An (optionally cv-qualified) pointer to a non-static member
        //      function
        //   6) A reference to an (optionally cv-qualified) pointer to a
        //      non-static member function
        ///////////////////////////////////////////////////////////////////
        template <typename T>
        inline constexpr bool IsTraitsFreeOrMemberFunction_v = IsTraitsFreeFunction_v<T> ||
                                                               IsTraitsMemberFunction_v<T>;

        //////////////////////////////////////////////
        // Concept for above template (see following
        // #defined constant for details)
        //////////////////////////////////////////////
        #if defined(USE_CONCEPTS)
            template <typename T>
            concept IsTraitsFreeOrMemberFunction_c = IsTraitsFreeOrMemberFunction_v<T>;

            // For internal use only (we #undef both later)
            #define TRAITS_FREE_OR_MEMBER_FUNCTION_C StdExt::Private::IsTraitsFreeOrMemberFunction_c
            #define STATIC_ASSERT_IS_TRAITS_FREE_OR_MEMBER_FUNCTION(T)
        #else
            // For internal use only (we #undef both later)
            #define TRAITS_FREE_OR_MEMBER_FUNCTION_C typename
            #define STATIC_ASSERT_IS_TRAITS_FREE_OR_MEMBER_FUNCTION(T) static_assert(StdExt::Private::IsTraitsFreeOrMemberFunction_v<T>, \
                                                                                     "\"F\" must be a free function which includes static " \
                                                                                     "member functions but not \"abominable\" functions, " \
                                                                                     "or a pointer or a reference to a free function, or a " \
                                                                                     "reference to a pointer to a free function, or a pointer " \
                                                                                     "to a non-static member function, or a reference to a " \
                                                                                     "pointer to a non-static member function. In all cases " \
                                                                                     "pointers may be optionally cv-qualified.");
        #endif // #if defined(USE_CONCEPTS)

        template <typename T>
        inline constexpr bool IsTraitsFreeFunctionOrAbominableFunction_v = IsTraitsFreeFunction_v<T> ||
                                                                           IsAbominableFunction_v<T>;

        //////////////////////////////////////////////
        // Concept for above template (see following
        // #defined constant for details)
        //////////////////////////////////////////////
        #if defined(USE_CONCEPTS)
            template <typename T>
            concept IsTraitsFreeFunctionOrAbominableFunction_c = IsTraitsFreeFunctionOrAbominableFunction_v<T>;

            // For internal use only (we #undef both later)
            #define TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION_C StdExt::Private::IsTraitsFreeFunctionOrAbominableFunction_c
            #define STATIC_ASSERT_IS_TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION(T)

        #else
            // For internal use only (we #undef both later)
            #define TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION_C typename
            #define STATIC_ASSERT_IS_TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION(T) static_assert(StdExt::Private::IsTraitsFreeFunctionOrAbominableFunction_v<T>, \
                                                                                                  "\"T\" must satisfy \"std::is_function\" or be a pointer or a " \
                                                                                                  "reference to a function that satisfies it (or a reference to a " \
                                                                                                  "pointer). \"T\" therefore either qualifies as a \"free\" function " \
                                                                                                  "by this library or an \"abominable\" function (in the latter case " \
                                                                                                  "a function satisfying \"std::is_function\" with at least one " \
                                                                                                  "cv-qualifier and/or ref-qualifier - note that pointers and " \
                                                                                                  "references to abominable functions aren't legal in the language " \
                                                                                                  "so will never be encountered).");
        #endif // #if defined(USE_CONCEPTS)
    } // namespace Private

    template <typename T>
    inline constexpr bool IsTraitsStdFunction_v = IsStdFunctionSpecialization_v< std::remove_reference_t<T> >;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept TraitsStdFunction_c = IsTraitsStdFunction_v<T>;
        #endif

        #define TRAITS_STD_FUNCTION_C StdExt::TraitsStdFunction_c
        #define STATIC_ASSERT_IS_TRAITS_STD_FUNCTION(T)
    #else
        #define TRAITS_STD_FUNCTION_C typename
        #define STATIC_ASSERT_IS_TRAITS_STD_FUNCTION(T) static_assert(StdExt::IsTraitsStdFunction_v<T>, \
                                                                      QUOTE_STRINGIFY(T) " must be a \"std::function\" " \
                                                                      "specialization or a reference to one");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    ///////////////////////////////////////////////////////////////////
    // IsTraitsFunctor_v. Variable template set to "true" if "T" is
    // either of the following AND it's not a "std::function"
    // specialization or false otherwise (if a "std::function"
    // specialization then it's technically a functor but we don't
    // treat it as one - its function traits target the type that
    // "std::function" itself is specialized on):
    //
    //   1) A functor
    //   2) A reference to a functor
    //
    // See "struct IsNonOverloadedFunctor" primary template earlier
    // for details on what constitutes a functor (the usual and
    // expected C++ definition of a "functor" meaning "T" must be a
    // class or struct with a member function called "operator()" but
    // for our purposes "T::operator()" can't be overloaded or the
    // following is guaranteed to return false - we don't support
    // overloaded functors by design but again, see the "struct
    // IsNonOverloadedFunctor" primary template for details).
    ///////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsTraitsFunctor_v = IsNonOverloadedFunctor_v< std::remove_reference_t<T> > &&
                                              !IsTraitsStdFunction_v<T>;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept TraitsFunctor_c = IsTraitsFunctor_v<T>;
        #endif

        #define TRAITS_FUNCTOR_C StdExt::TraitsFunctor_c
        #define STATIC_ASSERT_IS_TRAITS_FUNCTOR(T)
    #else
        #define TRAITS_FUNCTOR_C typename
        #define STATIC_ASSERT_IS_TRAITS_FUNCTOR(T) static_assert(StdExt::IsTraitsFunctor_v<T>, \
                                                                 QUOTE_STRINGIFY(T) " must be a non-overloaded, " \
                                                                 "non-static functor or a reference to one (i.e., a " \
                                                                 "class or struct with a non-overloaded, non-static " \
                                                                 "function call operator \"operator()\"). Note that " \
                                                                 "\"std::function\" specializations don't qualify for " \
                                                                 "the purposes here (i.e., while they are technically " \
                                                                 "non-overloaded, non-static functors they will still " \
                                                                 "trigger this \"static_assert\").");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    ///////////////////////////////////////////////////////////////////
    // IsTraitsStaticFunctor_v. Variable template set to "true" if "T"
    // is either of the following or false otherwise:
    //
    //   1) A static functor (legalized in C++23 so not possible in
    //      earlier versions unless already provided it as a
    //      non-standard C++ extension)
    //   2) A reference to a static functor
    //
    // See "struct IsNonOverloadedStaticFunctor" primary template
    // earlier for details on what constitutes a static functor (the
    // usual and expected C++ definition of a "functor", i.e., "T"
    // must be a class or struct with a member function called
    // "operator()" but in this case its "operator()" is static,
    // (legalized in C++23) and for our purposes here it can't be
    // overloaded or the following is guaranteed to return false - we
    // don't support overloaded functors by design but again, see the
    // "struct IsNonOverloadedStaticFunctor" primary template for
    // details).
    ///////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsTraitsStaticFunctor_v = IsNonOverloadedStaticFunctor_v< std::remove_reference_t<T> >;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept TraitsStaticFunctor_c = IsTraitsStaticFunctor_v<T>;
        #endif

        #define TRAITS_STATIC_FUNCTOR_C StdExt::TraitsStaticFunctor_c
        #define STATIC_ASSERT_IS_TRAITS_STATIC_FUNCTOR(T)
    #else
        #define TRAITS_STATIC_FUNCTOR_C typename
        #define STATIC_ASSERT_IS_TRAITS_STATIC_FUNCTOR(T) static_assert(StdExt::IsTraitsStaticFunctor_v<T>, \
                                                                        QUOTE_STRINGIFY(T) " must be a non-overloaded, " \
                                                                        "static functor or a reference to one (i.e., a " \
                                                                        "class or struct with a non-overloaded, static " \
                                                                        "function call operator \"operator()\")");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    ///////////////////////////////////////////////////////////////////
    // IsTraitsNonStaticOrStaticFunctor_v. Variable template set to
    // "true" if "T" is either of the following or false otherwise:
    //
    //   1) A non-static or static functor (usually the former but
    //      "static" versions of "operator()" were legalized in C++23
    //      so they also qualify)
    //   2) A reference to 1 above
    //
    // See "struct IsNonOverloadedFunctor" primary template earlier
    // for details on what constitutes a non-static functor (the usual
    // and expected C++ definition of a "functor", i.e., "T" must be a
    // class or struct with a non-static member function called
    // "operator()" but in this case its "operator()" can be static as
    // well (the latter was legalized in C++23). For our purposes here
    // however it can't be overloaded or the following is guaranteed
    // to return false (we don't support overloaded functors by design
    // but again, see the "struct IsNonOverloadedFunctor" primary
    // template for details).
    ///////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsTraitsNonStaticOrStaticFunctor_v = IsTraitsFunctor_v<T> ||
                                                               IsTraitsStaticFunctor_v<T>;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept TraitsNonStaticOrStaticFunctor_c = IsTraitsNonStaticOrStaticFunctor_v<T>;
        #endif

        #define TRAITS_NON_STATIC_OR_STATIC_FUNCTOR_C StdExt::TraitsNonStaticOrStaticFunctor_c
        #define STATIC_ASSERT_IS_TRAITS_NON_STATIC_OR_STATIC_FUNCTOR(T)
    #else
        #define TRAITS_NON_STATIC_OR_STATIC_FUNCTOR_C typename
        #define STATIC_ASSERT_IS_TRAITS_NON_STATIC_OR_STATIC_FUNCTOR(T) static_assert(StdExt::IsTraitsNonStaticOrStaticFunctor_v<T>, \
                                                                                      QUOTE_STRINGIFY(T) " must be a non-overloaded, " \
                                                                                      "non-static or static functor or a reference to " \
                                                                                      "one (i.e., a class or struct with a non-overloaded, " \
                                                                                      "non-static or static function call operator " \
                                                                                      "\"operator()\", usually non-static but static " \
                                                                                      "were legalized in C++23 so are also supported)");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    /////////////////////////////////////////////////////////////////////////////
    // "IsTraitsFunction_v". Variable template set to true if "T" is a function
    // type supported by this library or "false" otherwise. If true then it's
    // suitable for passing as the "F" template arg to any of the templates in
    // this library, normally struct "FunctionTraits" or any of its helper
    // templates (most users rely on the latter which just wrap the former). A
    // supported function refers to any of the following types (in each case
    // referring to a C++ type, not an actual instance of the type - note that
    // pointer types may be cv-qualified):
    //
    // 1) Free functions (which includes static member functions). A "free"
    //    function is any function satisfying "std::is_function" not including
    //    "abominable" functions (which are supported by this library but not
    //    classified as "free" functions - see 8 below). See comments preceding
    //    "IsFreeFunction_v" for complete details on free functions.
    // 2) Pointers to free functions (i.e., pointers to 1 above)
    // 3) References to free functions (i.e., references to 1 above)
    // 4) References to pointers to free functions (i.e., references to
    //    pointers to 1 above)
    // 5) Pointers to non-static member functions (any type satisfying
    //    "std::is_member_function_pointer")
    // 6) References to pointers to non-static member functions (i.e,
    //    references to pointers to 5 above). Note that references to
    //    non-static member functions are not supported by C++ itself,
    //    only references to non-static member function pointers are.
    // 7) Non-overloaded functors (optionally cv-qualified) or references
    //    to them. In either case refers to a class with a non-overloaded
    //    "operator()" member (usually non-static but static also legal in
    //    C++23), noting that overloaded functors are not supported since
    //    they would create an ambiguous situation (which "operator()" to
    //    choose). If you need to deal with them then you'll normally need
    //    to target the specific "operator()" overload you're interested in
    //    by taking its address and casting it to the exact signature you
    //    need to target (and passing that type to this library instead of
    //    the functor's type). Note that non-generic lambdas are also
    //    supported since lambdas are just syntactic sugar for creating
    //    functors on-the-fly. *Generic* lambdas however aren't supported
    //    using a functor's type because they are not functors in the
    //    conventional sense (but actually templates behind the scenes).
    //    They can still be accessed however as described in "Example 3" of
    //    the comments preceding the "FunctionTraits" specialization for
    //    functors. Search this file for "Example 3 (generic lambda)"
    //    (quotes not included).
    // 8) Abominable functions, i.e., any type satisfying "std::is_function"
    //    not including functions with cv-qualifiers and/or "ref-qualifiers"
    //    (see https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html)
    //
    //    For example, this is the complete list of abominable functions for
    //    function "int (float)":
    //
    //     using T1  = int (float) const [noexcept];
    //     using T2  = int (float) volatile [noexcept];
    //     using T3  = int (float) const volatile [noexcept];
    //     using T4  = int (float) & [noexcept];
    //     using T5  = int (float) const & [noexcept];
    //     using T6  = int (float) volatile & [noexcept];
    //     using T7  = int (float) const volatile & [noexcept];
    //     using T8  = int (float) && [noexcept];
    //     using T9  = int (float) const && [noexcept];
    //     using T10 = int (float) volatile && [noexcept];
    //     using T11 = int (float) const volatile && [noexcept];
    // 9) "std::function" specializations (optionally cv-qualified) or
    //     references to them
    /////////////////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsTraitsFunction_v = IsTraitsFreeFunction_v<T> || // 1) Is "T" a free function (includes static member functions).
                                                                            //    See (long) comments just above "IsFreeFunction_v" for
                                                                            //    details. Note that this item (1) excludes pointers and
                                                                            //    references to free functions which are handled by items 2-4
                                                                            //    below. This item (1) only targets the function's actual C++
                                                                            //    type which is never a pointer or a reference. The actual C++
                                                                            //    type for functions isn't used that often in practice however
                                                                            //    (typically), since free function names decay to a pointer to
                                                                            //    the function in most expressions so item 2 below is usually
                                                                            //    encountered more frequently (though item 1 does occur sometimes
                                                                            //    so we handle it here).
                                                                            // 2) Is "T" an (optionally cv-qualified) pointer to a free function
                                                                            //    (the most common case for free functions in practice since free
                                                                            //    function names decay to a pointer to the function in most
                                                                            //    expressions)
                                                                            // 3) Is "T" a reference to a free function (rare in practice)
                                                                            // 4) Is "T" a reference to an (optionally cv-qualified) pointer to
                                                                            //    a free function (rare in practice)
                                                                            // X) None of the above hold so "T" isn't a free function (or
                                                                            //    a pointer to one or a reference to one or a reference to a
                                                                            //    pointer to one) so we go on to check if it's a pointer to a
                                                                            //    non-static member function ...
                                               IsTraitsMemberFunction_v<T> || // 1) Is T an (optionally cv-qualified) pointer to a non-static
                                                                              //    member function (the most common case for non-static
                                                                              //    member functions in practice) or ...
                                                                              // 2) Is T a reference to an (optionally cv-qualified) pointer
                                                                              //    to a non-static member function (rare in practice)
                                                                              // X) Neither 1 or 2 above hold noting that "T" *cannot*
                                                                              //    be a reference to a member function (not supported
                                                                              //    in C++ itself so we don't check for it) though 2 just
                                                                              //    above does support references to pointers to non-static
                                                                              //    member functions
                                               IsTraitsFunctor_v<T> || // Is "T" a non-overloaded, non-static functor or a reference to one
                                                                       // but not a "std::function". If "true" then "T" has a non-static
                                                                       // member function named "operator()" by definition and it's not
                                                                       // overloaded in our case since overloads would be ambiguous.
                                                                       // "std::function however is handled separately below, even though
                                                                       // it's technically a non-overloaded, non-static functor as well
                                                                       // (but for our purposes we don't treat them as functors).
                                               IsTraitsStaticFunctor_v<T> || // Is "T" a non-overloaded, static functor or a reference to
                                                                             // one. If "true" then "T" has a static member function named
                                                                             // "operator()" by definition and it's not overloaded in our
                                                                             // case since overloads would be ambiguous.
                                               IsAbominableFunction_v<T> || // Is "T" an abominable function
                                               IsTraitsStdFunction_v<T>; // Is "T" a "std::function" specialization or reference to one
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            //////////////////////////////////////////////////////
            // Concept evaluating to true if "T" is any function
            // type supported by this library (so any type
            // suitable for passing as the "F" template arg to
            // "struct FunctionTraits" but most users will rely
            // on the helper templates that wrap this instead).
            // See the following (IsTraitsFunction_v) for complete
            // details on what qualifies as a valid function in
            // this library (i.e., what the following will return
            // true for - the concept is used by all helper
            // templates in the library to ensure the "F"
            // template arg of each helper template is valid).
            //////////////////////////////////////////////////////
            concept TraitsFunction_c = IsTraitsFunction_v<T>;
        #endif

        #define TRAITS_FUNCTION_C StdExt::TraitsFunction_c
        #define STATIC_ASSERT_IS_TRAITS_FUNCTION(T)
    #else
        #define TRAITS_FUNCTION_C typename
        #define STATIC_ASSERT_IS_TRAITS_FUNCTION(T) static_assert(StdExt::IsTraitsFunction_v<T>, \
                                                                  QUOTE_STRINGIFY(T) " isn't a function type suitable for passing " \
                                                                  "to \"FunctionTraits\" or any of its helper templates. See comments " \
                                                                  "preceding \"StdExt::IsTraitsFunction_v\" for details, but for all " \
                                                                  "intents and purposes any legal type identifying a function will " \
                                                                  "normally do (i.e., free functions which include static member " \
                                                                  "functions, pointers and references to free functions, references " \
                                                                  "to pointers to free functions, pointers to non-static member " \
                                                                  "functions, references to pointers to non-static member functions, " \
                                                                  "non-overloaded functors or references to them, \"abominable\" " \
                                                                  "functions, and \"std::function\" specializations or references " \
                                                                  "to them. " QUOTE_STRINGIFY(T) " doesn't qualify as any of these.");
    #endif // #if defined(USE_CONCEPTS)

    #define STDEXT_CC_DEFAULT

    ////////////////////////////////////////////////////////
    // MSFT? (or any other compiler we support but running
    // in Microsoft VC++ compatibility mode)
    ////////////////////////////////////////////////////////
    #if defined(_MSC_VER)
        ///////////////////////////////////////////////////////////////////
        // See https://learn.microsoft.com/en-us/cpp/cpp/argument-passing-and-naming-conventions?view=msvc-170
        ///////////////////////////////////////////////////////////////////
        #define STDEXT_CC_CDECL __cdecl
        #define STDEXT_CC_STDCALL __stdcall
        #define STDEXT_CC_FASTCALL __fastcall
        #define STDEXT_CC_VECTORCALL __vectorcall
        #define STDEXT_CC_THISCALL __thiscall

        /////////////////////////////////////////////////////////////
        // "regcall" calling convention only available on Clang and
        // Intel compilers (among the ones we currently support)
        /////////////////////////////////////////////////////////////
        #if defined(CLANG_COMPILER) || defined(INTEL_COMPILER)
            ////////////////////////////////////////////////////////////////////
            // See https://clang.llvm.org/docs/AttributeReference.html#regcall
            // See https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-10/c-c-calling-conventions.html
            ////////////////////////////////////////////////////////////////////
            #define STDEXT_CC_REGCALL __regcall
        #endif
    #elif defined(GCC_COMPILER) || \
          defined(CLANG_COMPILER) || \
          defined(INTEL_COMPILER)

        /////////////////////////////////////////////////////////////////////////////
        // For GCC: https://gcc.gnu.org/onlinedocs/gcc/x86-Function-Attributes.html
        // For Clang: https://clang.llvm.org/docs/AttributeReference.html#calling-conventions
        // For Intel: https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-10/c-c-calling-conventions.html
        /////////////////////////////////////////////////////////////////////////////
        #define STDEXT_CC_CDECL __attribute__((cdecl))
        #define STDEXT_CC_STDCALL __attribute__((stdcall))
        #define STDEXT_CC_FASTCALL __attribute__((fastcall))
        #define STDEXT_CC_THISCALL __attribute__((thiscall))

        /////////////////////////////////////////////////////////////
        // Note: "regcall" calling convention only available on
        // Clang and Intel compilers (among the ones we currently
        // support). See here regarding GCC's support for vectorcall
        // (still not supported by GCC at this writing):
        //
        //     Bug 89485 - Support vectorcall calling convention on windows
        //     https://gcc.gnu.org/bugzilla/show_bug.cgi?id=89485
        /////////////////////////////////////////////////////////////
        #if defined(CLANG_COMPILER) || defined(INTEL_COMPILER)
            // See https://clang.llvm.org/docs/AttributeReference.html#regcall
            // See https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-10/c-c-calling-conventions.html
            #define STDEXT_CC_VECTORCALL __attribute__((vectorcall))
            #define STDEXT_CC_REGCALL __attribute__((regcall))
        #endif
    #else
        /////////////////////////////////////////////////////
        // Note: Shouldn't be possible to come through here
        // at this writing since the same #error message
        // would have already been displayed earlier (when
        // the compiler constants just above were #defined
        // in "CompilerVersions.h")
        /////////////////////////////////////////////////////
        #error "Unsupported compiler (GCC, Microsoft, Clang and Intel are the only ones supported at this writing)"
    #endif

    ///////////////////////////////////////////////////////////////////
    // Always the case at this writing (variadic functions are always
    // STDEXT_CC_CDECL on the platforms we currently support), though
    // in a future release we may want to handle this better should we
    // ever support a platform where other calling conventions that
    // support variadic functions are available (but probably
    // unlikely). For now we only support variadic functions whose
    // calling convention is STDEXT_CC_CDECL and in practice this
    // calling convention is almost always universally required for
    // variadic functions (even if some platforms support other
    // calling conventions that can be used). MSFT for instance, under
    // its docs for the /Gd (__cdecl), /Gr (__fastcall), /Gv
    // (__vectorcall) or /Gz (__stdcall) default calling convention
    // options (see
    // https://learn.microsoft.com/en-us/cpp/build/reference/gd-gr-gv-gz-calling-convention?view=msvc-170
    // at this writing), indicates this:
    //
    //    "Functions that take a variable number of arguments must be
    //    marked __cdecl"
    //
    // And later on at the same link ...
    //
    //    "A member function that takes a variable number of arguments
    //    "always uses the __cdecl calling convention.
    //
    // And under MSFT's __thiscall documentation currently found here
    // at this writing: https://learn.microsoft.com/en-us/cpp/cpp/thiscall?view=msvc-160:
    //
    //    "_thiscall calling convention is used on C++ class member
    //     functions on the x86 architecture. It's the default calling
    //     convention used by member functions that don't use variable
    //     arguments (vararg functions)"
    //
    // And later on at the same link:
    //
    //     "vararg member functions use the __cdecl calling convention"
    //
    // Note that when using our "FunctionTraits" class or any
    // of its helper templates, if a variadic function is passed to
    // any of these templates with a calling convention other than
    // STDEXT_CC_CDECL (though it should never happen at this writing
    // on the platforms we support), then our TRAITS_FUNCTION_C
    // concept will usually kick in for C++20 and later, or a
    // "template not defined" error of some kind in C++17
    // ("FunctionTraits" isn't available in earlier versions - it's
    // preprocessed out). On MSFT platforms however variadic functions
    // with calling conventions other than "__cdecl" are automatically
    // changed to "__cdecl" so no compiler error occurs. See the
    // following by Raymond Chen from MSFT:
    //
    //    "If you try to declare a variadic function with an incompatible
    //    calling convention, the compiler secretly converts it to cdecl"
    //    https://devblogs.microsoft.com/oldnewthing/20131128-00/?p=2543
    //
    // If we ever do handle other calling conventions for variadic
    // functions then additional specializations for "FunctionTraits"
    // will have to be created to handle them (plus some other
    // adjustments to the "FunctionTraits" code).
    ///////////////////////////////////////////////////////////////////
    #define STDEXT_CC_VARIADIC STDEXT_CC_CDECL

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    #if defined(GCC_COMPILER)
        ///////////////////////////////////////////////////////////////////
        // When targeting 64 bit builds the above STDEXT_CC_? calling
        // conventions are usually ignored (automatically replaced by the
        // compiler with STDEXT_CC_CDECL), except for STDEXT_CC_CDECL
        // itself of course (obviously not replaced with itself), and
        // STDEXT_CC_VECTORCALL or STDEXT_CC_REGCALL (they aren't replaced
        // in 64 bits builds on the compilers that support them - GCC
        // supports neither, MSFT supports STDEXT_CC_VECTORCALL but not
        // STDEXT_CC_REGCALL, and Clang and Intel support both). It's also
        // possible that some calling conventions may not be supported
        // depending on other compiler options though I haven't looked
        // into this. The bottom line is that this can result in many
        // annoying warnings about unsupported calling convention
        // attributes (in 64 bit builds in particular) so we'll
        // temporarily turn these warnings off here and back on later
        // (only if they were already on).
        ///////////////////////////////////////////////////////////////////
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wattributes" // E.g., "warning: 'stdcall' attribute ignored [-Wattributes]"
                                                      // See https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Diagnostic-Pragmas.html#Diagnostic-Pragmas

    // For Clang see https://clang.llvm.org/docs/UsersManual.html#controlling-diagnostics-via-pragmas
    // Note that Intel also relies on Clang as described in the comments just below
    #elif defined(CLANG_COMPILER) || \
          defined(INTEL_COMPILER)

        /////////////////////////////////////////////////
        // According to the Intel DPC++/C++ docs here:
        //
        //   Error Handling
        //   https://www.intel.com/content/www/us/en/docs/dpcpp-cpp-compiler/developer-guide-reference/2023-0/error-handling.html
        //
        // It says:
        //
        //    For a summary of warning and error options,
        //    see the Clang documentation.
        //
        // Where "Clang documentation" is this link:
        //
        //   https://clang.llvm.org/docs/UsersManual.html#options-to-control-error-and-warning-messages
        //
        // Hence the following works for Intel as well
        // (for the DPC++/C++ compiler which is what we
        // support, not the "Classic" Intel compiler
        // which they've deprecated and replaced with
        // the DPC++/C++ compiler - we don't support
        // the "Classic" compiler)
        /////////////////////////////////////////////////

        //////////////////////////////////////////////////////
        // See GCC_COMPILER call to its own "diagnostic push"
        // above (comments there apply here as well)
        //////////////////////////////////////////////////////
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wignored-attributes" // E.g., "warning: 'stdcall' calling convention is not
                                                                // supported for this target [-Wignored-attributes]"
                                                                // See https://clang.llvm.org/docs/DiagnosticsReference.html#wignored-attributes
        #pragma clang diagnostic ignored "-Wunknown-attributes" // E.g., "warning: unknown attribute 'stdcall' ignored
                                                                // [-Wunknown-attributes]". Not normally encountered
                                                                // however. All calling conventions above are legal in
                                                                // Clang but ignoring this warning just in case. See
                                                                // https://clang.llvm.org/docs/DiagnosticsReference.html#wunknown-attributes
        #pragma clang diagnostic ignored "-Wambiguous-ellipsis" // E.g., "warning: '...' in this location creates a
                                                                // C-style varargs function[-Wambiguous - ellipsis]
    #endif

    enum class CallingConvention
    {
        ///////////////////////////////////////////////////////////
        // *** IMPORTANT ***
        // -----------------
        // Don't change the order OR value of these, our design
        // depends on it. Dirty but we'll live with it (makes
        // life easier for our purposes). Note that if adding new
        // calling conventions (very rare), make sure "Last" is
        // updated below (doing so changes the enum for existing
        // users however but so be it - usually harmless though)
        ///////////////////////////////////////////////////////////
        Cdecl,
        Stdcall,
        Fastcall,
        Vectorcall,
        Thiscall,
        Regcall,
        Last = Regcall, // For internal use only. IMPORTANT: Make sure this
                        // is always set to the last value just above.
        Variadic = Cdecl
    };

    // Number of calling conventions we support
    inline constexpr std::size_t CallingConventionCount_v = static_cast<std::size_t>(CallingConvention::Last) + 1;

    ///////////////////////////////////////////////////////////////////////////
    // CallingConventionToString(). WYSIWYG
    ///////////////////////////////////////////////////////////////////////////
    inline constexpr tstring_view CallingConventionToString(const CallingConvention callingConvention) noexcept
    {
        constexpr std::array callingConventions = { _T("cdecl"),
                                                    _T("stdcall"),
                                                    _T("fastcall"),
                                                    _T("vectorcall"),
                                                    _T("thiscall"),
                                                    _T("regcall")
                                                  };

        ////////////////////////////////////////////
        // Triggers if a new calling convention is
        // added to "CallingConvention" enum (very
        // rare), but not yet added to above array
        // (so add it)
        ////////////////////////////////////////////
        static_assert(callingConventions.size() == CallingConventionCount_v);

        /////////////////////////////////////////////////////
        // Returned as a "tstring_view" (constructor taking
        // a null-terminated string implicitly called)
        /////////////////////////////////////////////////////
        return callingConventions[static_cast<std::size_t>(callingConvention)];
    }

    // For internal use only
    namespace Private
    {
        class AnyClass
        {
        };
    }

    ////////////////////////////////////////////////////////////////////////////
    // CallingConventionReplacedWithCdecl(). For the given "CallingConventionT"
    // template arg, returns true if the compiler will change a function with
    // the latter calling convention to the "cdecl" calling convention or false
    // otherwise (for a free function if "IsFreeFunc" is "true" or a non-static
    // member function if "false"). Note that compilers (the ones we support
    // anyway) will sometimes change a function's explicitly declared calling
    // convention to "cdecl" depending on the target platform or compiler
    // options in effect, in particular when compiling for 64 bit versus 32
    // bit. The following tests for this by simply taking an arbitrary function
    // type and assigning it the calling convention you pass ("CallingConventionT"),
    // and comparing this function's type to the same function but with the
    // "cdecl" calling convention instead. Since their calling conventions are
    // different, the types should normally test *not* equal since a function's
    // calling convention is considered part of a function's type (on most
    // platforms including all those we support). If two function types are
    // identical in every respect except for the calling convention then
    // they're not considered equal since the calling conventions don't match.
    // In this case we therefore return false meaning the compiler didn't
    // replace the calling convention with "cdecl" (i.e., it respects
    // "CallingConventionT" when explicitly declared on a function). OTOH, if
    // the two function types do test equal, then the compiler must have
    // changed the function with the "CallingConventionT" calling convention to
    // the "cdecl" calling convention, i.e., it doesn't respect the given
    // "CallingConventionT" when explicitly declared on a function (because it
    // changed it to "cdecl"). True is therefore returned meaning functions
    // explicitly declared with the "CallingConventionT" calling convention
    // aren't respected since the compiler replaces that calling convention
    // with "cdecl". Please note however that false is always returned by
    // design if you pass "CallingConvention::Cdecl" itself for the
    // "CallingConventionT" arg, since for the intended purpose of this
    // function, the compiler doesn't replace a function that's already "cdecl"
    // with "cdecl" (since it's already "cdecl" we don't consider it replaced).
    /////////////////////////////////////////////////////////////////////////////
    template <CallingConvention CallingConventionT,
              bool IsFreeFunc>
    inline constexpr bool CallingConventionReplacedWithCdecl() noexcept
    {
        // Free or static member function ...
        if constexpr (IsFreeFunc)
        {
            if constexpr (CallingConventionT == CallingConvention::Cdecl)
            {
                ///////////////////////////////////////////////////////////////////
                // STDEXT_CC_CDECL never replaced with itself (for the purposes
                // of this function - it's already "cdecl" so it's not considered
                // replaced)
                ///////////////////////////////////////////////////////////////////
                return false;
            }
            // For internal use only (#undefined further below)
            #define IS_REPLACED_WITH_CDECL(CC) std::is_same_v<void CC (), void STDEXT_CC_CDECL ()>
            else if constexpr (CallingConventionT == CallingConvention::Stdcall)
            {
                return IS_REPLACED_WITH_CDECL(STDEXT_CC_STDCALL);
            }
            else if constexpr (CallingConventionT == CallingConvention::Fastcall)
            {
                return IS_REPLACED_WITH_CDECL(STDEXT_CC_FASTCALL);
            }
            else if constexpr (CallingConventionT == CallingConvention::Vectorcall)
            {
                #if defined(STDEXT_CC_VECTORCALL)
                    return IS_REPLACED_WITH_CDECL(STDEXT_CC_VECTORCALL);
                #else
                    /////////////////////////////////////////////////////////////
                    // Always triggers by design! (note that the 1st arg below
                    // is designed to be dependent on a template arg to prevent
                    // "static_assert" from always triggering even when we don't
                    // come through here - just how C++ works, though in this
                    // particular context passing false directly will actually
                    // be fine on some compilers but testing shows it's not
                    // always the case on others, possibly depending on the
                    // version)
                    /////////////////////////////////////////////////////////////
                    static_assert(AlwaysFalse_v<decltype(CallingConventionT)>,
                                  "\"CallingConvention::Vectorcall\" not supported by "
                                  "this compiler (so it makes no sense to pass it)");
                    return true;
                #endif
            }
            else if constexpr (CallingConventionT == CallingConvention::Thiscall)
            {
                /////////////////////////////////////////////////////////////
                // Always triggers by design! (note that the 1st arg below
                // is designed to be dependent on a template arg to prevent
                // "static_assert" from always triggering even when we don't
                // come through here - just how C++ works, though in this
                // particular context, passing false directly will actually
                // be fine on some compilers but testing shows it's not
                // always the case on others, possibly depending on the
                // version)
                /////////////////////////////////////////////////////////////
                static_assert(AlwaysFalse_v<decltype(CallingConventionT)>,
                              "\"CallingConvention::Thiscall\" not supported for "
                              "free functions (so it makes no sense to pass it)");
                return true;
            }
            else if constexpr (CallingConventionT == CallingConvention::Regcall)
            {
                #if defined(STDEXT_CC_REGCALL)
                    return IS_REPLACED_WITH_CDECL(STDEXT_CC_REGCALL);
                #else
                    /////////////////////////////////////////////////////////////
                    // Always triggers by design! (note that the 1st arg below
                    // is designed to be dependent on a template arg to prevent
                    // "static_assert" from always triggering even when we don't
                    // come through here - just how C++ works, though in this
                    // particular context, passing false directly will actually
                    // be fine on some compilers but testing shows it's not
                    // always the case on others, possibly depending on the
                    // version)
                    /////////////////////////////////////////////////////////////
                    static_assert(AlwaysFalse_v<decltype(CallingConventionT)>,
                                  "\"CallingConvention::Regcall\" not supported by "
                                  "this compiler (so it makes no sense to pass it)");
                    return true;
                #endif
            }
            #undef IS_REPLACED_WITH_CDECL // Done with this (for internal use only)
            else
            {
                /////////////////////////////////////////////////////////////
                // Always triggers by design! (note that the 1st arg below
                // is designed to be dependent on a template arg to prevent
                // "static_assert" from always triggering even when we don't
                // come through here - just how C++ works, though in this
                // particular context, passing false directly will actually
                // be fine on some compilers but testing shows it's not
                // always the case on others, possibly depending on the
                // version)
                /////////////////////////////////////////////////////////////
                static_assert(AlwaysFalse_v<decltype(CallingConventionT)>, \
                              "Unknown \"CallingConventionT\"");
                return true;
            }
        }
        // Non-static member function ...
        else
        {
            if constexpr (CallingConventionT == CallingConvention::Cdecl)
            {
                /////////////////////////////////////////////////////////////
                // STDEXT_CC_CDECL never replaced with itself (for purposes
                // of this function), and it's always supported regardless
                // (so it never gets replaced, let alone with itself)
                /////////////////////////////////////////////////////////////
                return false;
            }
            else
            {
                // For internal use only (#undefined further below)
                #define IS_REPLACED_WITH_CDECL(CC) \
                    std::is_same_v<void (CC Private::AnyClass::*)(), \
                                   void (STDEXT_CC_CDECL Private::AnyClass::*)()>

                if constexpr (CallingConventionT == CallingConvention::Stdcall)
                {
                    return IS_REPLACED_WITH_CDECL(STDEXT_CC_STDCALL);
                }
                else if constexpr (CallingConventionT == CallingConvention::Fastcall)
                {
                    return IS_REPLACED_WITH_CDECL(STDEXT_CC_FASTCALL);
                }
                else if constexpr (CallingConventionT == CallingConvention::Vectorcall)
                {
                    #if defined(STDEXT_CC_VECTORCALL)
                        return IS_REPLACED_WITH_CDECL(STDEXT_CC_VECTORCALL);
                    #else
                        /////////////////////////////////////////////////////////////
                        // Always triggers by design! (note that the 1st arg below
                        // is designed to be dependent on a template arg to prevent
                        // "static_assert" from always triggering even when we don't
                        // come through here - just how C++ works, though in this
                        // particular context, passing false directly will actually
                        // be fine on some compilers but testing shows it's not
                        // always the case on others, possibly depending on the
                        // version)
                        /////////////////////////////////////////////////////////////
                        static_assert(AlwaysFalse_v<decltype(CallingConventionT)>,
                                      "\"CallingConvention::Vectorcall\" not supported by "
                                      "this compiler (so it makes no sense to pass it)");
                        return true;
                    #endif
                }
                else if constexpr (CallingConventionT == CallingConvention::Thiscall)
                {
                    return IS_REPLACED_WITH_CDECL(STDEXT_CC_THISCALL);
                }
                else if constexpr (CallingConventionT == CallingConvention::Regcall)
                {
                    #if defined(STDEXT_CC_REGCALL)
                        return IS_REPLACED_WITH_CDECL(STDEXT_CC_REGCALL);
                    #else
                        /////////////////////////////////////////////////////////////
                        // Always triggers by design! (note that the 1st arg below
                        // is designed to be dependent on a template arg to prevent
                        // "static_assert" from always triggering even when we don't
                        // come through here - just how C++ works, though in this
                        // particular context, passing false directly will actually
                        // be fine on some compilers but testing shows it's not
                        // always the case on others, possibly depending on the
                        // version)
                        /////////////////////////////////////////////////////////////
                        static_assert(AlwaysFalse_v<decltype(CallingConventionT)>,
                                      "\"CallingConvention::Regcall\" not supported by "
                                      "this compiler (so it makes no sense to pass it)");
                        return true;
                    #endif
                }
                #undef IS_REPLACED_WITH_CDECL// Done with this (for internal use only)
                else
                {
                    ////////////////////////////////////////////////////////////
                    // Always triggers by design! (note that the 1st arg below
                    // is designed to be dependent on a template arg to prevent
                    // "static_assert" from always triggering - just how C++
                    // works, though in this particular context, passing false
                    // directly will actually be fine on some compilers but
                    // testing shows it's not always the case on others,
                    // possibly depending on the version)
                    ////////////////////////////////////////////////////////////
                    static_assert(AlwaysFalse_v<decltype(CallingConventionT)>,
                                  "Unknown \"CallingConventionT\"");
                    return true;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////
    // Refers to the optional "&" or "&&" ref-qualifiers that
    // can be added just after the argument list of non-static
    // member functions (similar to adding optional
    // cv-qualifiers though the purpose is different of course).
    // These are rarely used in practice though so the "None"
    // enum is usually in effect for most member functions. Read
    // up on "ref-qualifiers" for further details (in the
    // context of non-static member functions)
    /////////////////////////////////////////////////////////////
    enum class FunctionReference
    {
        None, // No ref-qualifiers present
        LValue, // "&" ref-qualifier present
        RValue // "&&" ref-qualifier present
    };

#if defined(STDEXT_SUPPORT_DEPRECATED)
    ////////////////////////////////////////////////////////////////////
    // Same as above enumerator (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    ////////////////////////////////////////////////////////////////////
    enum class RefQualifier
    {
        #define ENUM_REFQUALIFER_DEPRECATED  ENUM_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionReference)
        None   ENUM_REFQUALIFER_DEPRECATED = static_cast<int>(FunctionReference::None),
        LValue ENUM_REFQUALIFER_DEPRECATED = static_cast<int>(FunctionReference::LValue),
        RValue ENUM_REFQUALIFER_DEPRECATED = static_cast<int>(FunctionReference::RValue)
        #undef ENUM_REFQUALIFER_DEPRECATED
    };
#endif

    ///////////////////////////////////////////////////////////////////////////
    // FunctionReferenceToString(). WYSIWYG
    ///////////////////////////////////////////////////////////////////////////
    inline constexpr tstring_view FunctionReferenceToString(const FunctionReference functionReference,
                                                            const bool useAmpersands = true) noexcept
    {
        tstring_view str;

        switch (functionReference)
        {
            case FunctionReference::None:
                str = _T("");
                break;
            case FunctionReference::LValue:
                str = useAmpersands ? _T("&") : _T("lvalue");
                break;
            case FunctionReference::RValue:
                str = useAmpersands ? _T("&&") : _T("rvalue");
                break;
        }

        return str;
    }

    //////////////////////////////////////////////////////////////////////////////
    // "FunctionClassification" enumerator. All functions handled by this library
    // are classified as described below. This refers to template arg "F" passed
    // to most of the function's templates (which must satisfy the criteria below
    // or a constraints violation will occur in C++20 or later, or an undefined
    // "FunctionTraits" error in C++17 (earlier versions aren't supported)
    //////////////////////////////////////////////////////////////////////////////
    enum class FunctionClassification
    {
        ////////////////////////////////////////////////////////////////////////////////
        // Template arg "F" is a free function defined as any of the following:
        //
        // 1) Any type satisfying "std::is_function" not including abominable functions
        //    (see enum "Abominable" further below). "Free" functions therefore never
        //    contain the function qualifiers "const" and/or "volatile" and/or "&" or
        //    "&" (if they do then they are classified as enumerator "Abominable" below
        //    instead). Note that "Free" functions are always declared at namespace
        //    scope or as static member functions. Non-static member functions as well
        //    as non-static functors including lambdas never qualify (they are always
        //    classified as enum "NonStaticMember" just below unless a functor with a
        //    *static* "operator()" member is passed for "F" - see 5 and 6 below).
        // 2) Optionally cv-qualified pointer to 1 above, i.e., a pointer to any type
        //    satisfying "std::is_function" (non-static member function pointers never
        //    qualify - they are always classified as enum "NonStaticMember" below
        //    instead). Note that pointers to abominable functions aren't possible in
        //    C++ so if a pointer to a function satisfying "std::is_function" is
        //    encountered, the function is guaranteed not to have the qualifiers "const"
        //    and/or "volatile" and/or "&" or "&".
        // 3) Reference (lvalue or rvalue) to 2 above, i.e., a reference to an optionally
        //    cv-qualified pointer to any type satisfying "std::is_function" (references
        //    to non-static member function pointers never qualify - they are always
        //    classified as enum "NonStaticMember" below instead). Note that if
        //    encountered then the function the pointer points to is guaranteed not to
        //    be abominable (since pointers to abominable functions aren't possible in
        //    C++ - see 2 above).
        // 4) Reference (lvalue or rvalue) to any type satisfying 1 above, i.e., a
        //    reference to any type satisfying "std::is_function" (references to
        //    non-static member functions aren't legal in C++ so they will never be
        //    encountered). Note that references to abominable functions aren't possible
        //    in C++ so if a reference to a function satisfying "std::is_function" is
        //    encountered, the function is guaranteed not to have the qualifiers "const"
        //    and/or "volatile" and/or "&" or "&".
        // 5) Any non-overloaded *static* functor (i.e., "F" is an optionally
        //    cv-qualified class or struct with a non-overloaded, *static* "operator()"
        //    member - static versions of "operator()" were legalized in C++23). The
        //    static "operator()" member of "F" is classified as a free function (as
        //    all static member functions are).
        // 6) Reference (lvalue or rvalue) to 5 above (i.e., "F" is a reference to an
        //    optionally cv-qualified class or struct with a non-overloaded, *static*
        //    "operator()" member)
        // 7) Any optionally cv-qualified "std::function" specialization. Note that
        //    such specializations can only be legally specialized on free functions
        //    in C++ (with certain restrictions but always free). The function's type
        //    is the type that "F", a "std::function" specialization, is specialized
        //    on (i.e., its template arg which is always a free function).
        // 8) Reference (lvalue or rvalue) to 7 above (i.e., a reference to an
        //    optionally cv-qualified "std::function" specialization, again, where
        //    the function type is what the "std::function" that "F" refers to is
        //    specialized on)
        ////////////////////////////////////////////////////////////////////////////////
        Free,

        ////////////////////////////////////////////////////////////////////////////////
        // Template arg "F" is a non-static member function defined as any of the
        // following:
        //
        // 1) Any type satisfying "std::is_member_function_pointer" (i.e., an optionally
        //    cv-qualified pointer to a non-static member function)
        // 2) Reference (lvalue or rvalue) to 1 above (i.e., a reference to an optionally
        //    cv-qualified pointer to any type satisfying "std::is_member_function_pointer")
        //    Note that references to non-static member functions aren't possible in C++
        //    so will never be encountered (only references to non-static member function
        //    *pointers*).
        // 3) Any non-overloaded, non-static functor including lambdas. In either case
        //    "F" is an optionally cv-qualified class or struct with a non-overloaded,
        //    non-static "operator()" member (lambda types always qualify since the
        //    compiler always generates a functor for the lambda behind the scenes,
        //    known as the lambda's "closure type"). The non-static "operator()" member
        //    of "F" is therefore classified as a non-static member function (as all
        //    non-static member functions are). If the functor's "operator()" member is
        //    static however which was legalized in C++23, then it's classified as enum
        //    "Free" instead (see items 5 and 6 under enum "Free" above).
        // 4) Reference (lvalue or rvalue) to 3 above (i.e., "F" is a reference to an
        //    optionally cv-qualified non-overloaded, non-static functor including
        //    lambdas - see 3 above). Again, if the functor's "operator()" member is
        //    static which was legalized in C++23, then it's classified as enum "Free"
        //    instead.
        ////////////////////////////////////////////////////////////////////////////////
        NonStaticMember,

        ////////////////////////////////////////////////////////////////////////////////
        // Template arg "F" is an "abominable" function defined as follows (note that
        // pointers and/or references to abominable functions aren't legal in C++ so
        // will never be encountered - a compiler error will result if attempting to
        // create one, hence no criteria exists below to handle them since they're
        // not possible):
        //
        // 1) Any type satisfying "std::is_function" that's "const" and/or "volatile"
        //    and/or contains "&" or "&" (the definition of an "abominable" function).
        //    If the type satisfies "std::function" but is neither "const" nor
        //    "volatile" nor contains "&" or "&" (none of these qualifiers are present),
        //    then the function is classified as enum "Free" above instead.
        //
        // See https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html
        // for further details.
        ////////////////////////////////////////////////////////////////////////////////
        Abominable
    };

    //////////////////////////////////////////////////////////////////////////////
    // "FunctionOrigin" enumerator.  For functions handled by this library (arg
    // "F" of all templates), if "F" isn't a function type or a pointer or
    // reference to one, then the function represented by "F" must indirectly
    // originate from "F" but "F" isn't actually the function itself (only its
    // "origin"). The following enumerator therefore identifies what "F" actually
    // is (i.e., what the function it represents originates from). The library's
    // "FunctionTraitsFunctionOrigin_v" and "FunctionOrigin_v" templates (most
    // will normally use the latter which just defers to the former) are used to
    // return this enumerator. E.g., if "F" is a functor instead of an actual
    // function, then "FunctionOrigin_v" will return "FunctionOrigin::Functor"
    // since that's where the function represented by "F" originates from (a
    // functor, and specifically its "operator()" member).
    //////////////////////////////////////////////////////////////////////////////
    enum class FunctionOrigin
    {
        ///////////////////////////////////////////////////////////////
        // "F" has no origin. It's a function in its own right or a
        // reference or pointer to one (or a reference to a pointer).
        // Any type "F" satisfying "std::is_function" or pointers or
        // references to types satisfying it (including references
        // to such pointers), or any type satisfying
        // "std::is_member_function_pointer" (including references to
        // such pointers - note that direct references to non-static
        // member functions aren't legal in C++), will always be
        // assigned this origin ("None"). All others will be assigned
        // one of the other enumerators below.
        ///////////////////////////////////////////////////////////////
        None,

        ///////////////////////////////////////////////////////////////
        // "F" is a non-static functor or reference to one (the origin
        // of its non-static "operator()" member is "F"). Note that
        // "std::function" specializations are excluded (they're
        // always assigned enumerator "StdFunction" below even though
        // "std::function" specializations are still technically
        // functors). Note that the following starts at 7 so that 1 to
        // 6 remain available to accommodate the following origins
        // which may be added in a future release (still under
        // consideration). The enum's values aren't relevant to end
        // users (nor us) but if they are added I'd prefer to see them
        // come first (no reason they have to but the order will be
        // cleaner for reasons not worth explaining here):
        //
        //   RawFunction
        //   PointerToFunction
        //   ReferenceToFunction
        //   ReferenceToPointerToFunction
        //   PointerToMemberFunction
        //   ReferenceToPointerToMemberFunction"
        ///////////////////////////////////////////////////////////////
        Functor = 7,

        ///////////////////////////////////////////////////////////////
        // "F" is a static functor or reference to one (the origin of
        // its static "operator()" member is "F"). Only possible in
        // C++23 or later (static versions of "operator()" weren't
        // legalized until C++23).
        ///////////////////////////////////////////////////////////////
        StaticFunctor,

        ///////////////////////////////////////////////////////////////
        // "F" is a "std::function" specialization or reference to one
        // (the origin of the function passed via its template arg is
        // "F")
        ///////////////////////////////////////////////////////////////
        StdFunction
    };

    ///////////////////////////////////////////////////////////////////////////
    // For use by the following templates (and the lower-level templates they
    // wrap):
    //
    //   IsArgTypesSame_v
    //   IsArgTypesSameTuple_v
    ///////////////////////////////////////////////////////////////////////////
    enum class AndVariadic
    {
        IsFalse = 0, // "F" must not be variadic (usually the case). Must
                     // remain 0 and "IsTrue" below 1 (both explicitly set
                     // here to make things more clear). Don't change this,
                     // our design depends on it! Allows us to conveniently
                     // cast the enumerator to/from a "bool" though for the
                     // "to" case "Ignore" below needs to be checked first,
                     // since cast makes no sense for it (so it must be
                     // avoided). Dirty but convenient (and safe) so we'll
                     // live with it.
        IsTrue = 1,  // "F" must be variadic (rarely the case). Must remain
                     // 1 (see above).
        Ignore       // Variadic status of "F" is ignored (rarely passed)
    };

    //////////////////////////////////////////////////////////////
    // "IsValidReturnType" (primary template). Primary template
    // inherits from "std::false_type" if "T" is not a valid C++
    // function return type.
    //////////////////////////////////////////////////////////////
    template <typename T,
              typename = void // Arg for internal use only (users should never explicitly pass it)
             >
    struct IsValidReturnType : std::false_type
    {
    };

    //////////////////////////////////////////////////////////////////
    // Specialization of "IsValidReturnType" (primary) template just
    // above. Kicks in if "T" is a valid C++ function return type;
    //////////////////////////////////////////////////////////////////
    template <typename T>
    struct IsValidReturnType<T,
                             std::void_t< T () >
                            >
                            : std::true_type
    {
    };

    // Usual "_v" helper variable for above template
    template <typename T>
    inline constexpr bool IsValidReturnType_v = IsValidReturnType<T>::value;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept ValidReturnType_c = IsValidReturnType_v<T>;
        #endif
        #define VALID_RETURN_TYPE_C StdExt::ValidReturnType_c
        #define STATIC_ASSERT_IS_VALID_RETURN_TYPE_MSG
        #define STATIC_ASSERT_IS_VALID_RETURN_TYPE(T)
        #define STATIC_ASSERT_IS_VALID_RETURN_TYPE_F(T)
    #else
        #define VALID_RETURN_TYPE_C typename
        #define STATIC_ASSERT_IS_VALID_RETURN_TYPE_MSG(T) QUOTE_STRINGIFY(T) " is not a valid (legal) C++ " \
                                                          "function return type. This error normally occurs " \
                                                          "when invoking \"ReplaceReturnType_t\" to replace " \
                                                          "a function's return type and an illegal type was " \
                                                          "passed for the \"NewReturnTypeT\" template arg " \
                                                          "(one that's not a legal return type in C++)."

        #define STATIC_ASSERT_IS_VALID_RETURN_TYPE(T) static_assert(StdExt::IsValidReturnType_v<T>, \
                                                                    STATIC_ASSERT_IS_VALID_RETURN_TYPE_MSG(T));

        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            inline constexpr void StaticAssertValidReturnType() noexcept
            {
                STATIC_ASSERT_IS_VALID_RETURN_TYPE(T)
            }
        #endif
        #define STATIC_ASSERT_IS_VALID_RETURN_TYPE_F(T) StdExt::StaticAssertValidReturnType<T>()
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    ////////////////////////////////////////////////////////////////
    // "IsValidFunctionArgTypesTuple" (primary template ). Primary
    // template inherits from "std::false_type" if either of the
    // following 2 checks fail (meaning "T" either isn't a
    // "std::tuple" specialization - see 1 below - or it is but it's
    // specialized on at least one type that's not a valid C++
    // parameter type - see 2 below). If both checks succeed then
    // our partial specialization just below kicks in instead
    // (inheriting from "std::true_type").
    //
    // 1) "T" is an (optionally cv-qualified) "std::tuple"
    //    specialization
    // 2) The types that "T" is specialized on (its tuple types)
    //    *all* qualify as valid C++ function arg (parameter) types.
    //    Note that almost all types in C++ are valid (can be used
    //    as function parameter types) with rare exceptions, most
    //    notably "void" (you can't create a function parameter type
    //    of type "void"). Its presence in a parameter list of type
    //    "(void)" means an empty parameter list but "void" itself
    //    in that context is not an actual parameter, and if other
    //    parameter types are present in addition to "void" it's
    //    outright illegal. Other illegal parameter types can also
    //    exist, such as an abominable function type, but they would
    //    very rarely be encountered normally. Free (Non-abominable)
    //    function types however, while not legal parameter types,
    //    are considered valid, since they decay into function
    //    pointer types instead (no different than when you declare
    //    function pointers as parameter types). Array types are also
    //    valid since they decay into pointers as well (to the type
    //    of the array's elements, no different than when you
    //    declare arrays as parameter types). In any case, the
    //    following primary template kicks in if any of the types in
    //    "T" are illegal similar to when they're illegal in your
    //    own function declarations (or if "T" isn't even a
    //    "std::tuple" via 1 above).
    ////////////////////////////////////////////////////////////////
    template <typename T,
              typename = std::remove_cv_t<T>, // Arg for internal use only (users should never explicitly pass it)
              typename = void // Arg for internal use only (users should never explicitly pass it)
             >
    struct IsValidFunctionArgTypesTuple : std::false_type
    {
    };

    /////////////////////////////////////////////////////////////////
    // Specialization of "IsValidFunctionArgTypesTuple" (primary)
    // template just above. This specialization does all the work.
    // It kicks in if *both* of the following hold (see primary
    // template for details):
    //
    // 1) "T" is an (optionally cv-qualified) "std::tuple"
    //    specialization
    // 2) The types that "T" is specialized on (its tuple types)
    //    *all* qualify as valid C++ function arg (parameter) types,
    //    including when the tuple is empty (has no types so the
    //    function arg list will simply be empty in this case). See
    //    primary template for further details ("void" will fail if
    //    present in the tuple for instance - it's not a valid
    //    function arg type).
    /////////////////////////////////////////////////////////////////
    template <typename T,
              typename... Ts>
    struct IsValidFunctionArgTypesTuple<T,
                                        std::tuple<Ts...>,
                                        std::void_t< void (Ts...) >
                                       >
                                       : std::true_type
    {
    };

    // Usual "_v" helper variable for above template
    template <typename T>
    inline constexpr bool IsValidFunctionArgTypesTuple_v = IsValidFunctionArgTypesTuple<T>::value;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept ValidFunctionArgTypesTuple_c = IsValidFunctionArgTypesTuple_v<T>;
        #endif
        #define VALID_FUNCTION_ARG_TYPES_TUPLE_C StdExt::ValidFunctionArgTypesTuple_c
        #define STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_TUPLE_MSG
        #define STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_TUPLE(T)
    #else
        #define VALID_FUNCTION_ARG_TYPES_TUPLE_C typename
        #define STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_MSG_CONCLUSION "you've passed one or more types that are not valid " \
                                                                         "(legal) C++ function parameter types (which you've " \
                                                                         "either passed via a \"std:tuple\" or as one or more " \
                                                                         "standalone type args, depending on the template). Note " \
                                                                         "that such types must qualify as legal C++ function " \
                                                                         "argument (parameter) types or this \"static_assert\" " \
                                                                         "will occur. Otherwise you're trying to update a function's " \
                                                                         "args but passing one or more illegal argument types. Note " \
                                                                         "that with very rare exceptions, most notably type \"void\", " \
                                                                         "almost all C++ types can be used in a function parameter " \
                                                                         "list, but type \"void\" is not valid however, and even " \
                                                                         "though the parameter list \"(void)\" is a special case " \
                                                                         "that's syntactically equivalent to an empty parameter " \
                                                                         "list, \"void\" itself is not an actual parameter so " \
                                                                         "can't be passed to the template you called (if in fact " \
                                                                         "you've done so). Call \"ArgsMakeVoid_t\" instead if you " \
                                                                         "wish to explicitly create an empty parameter list (other " \
                                                                         "templates also exist that can be combined to accomplish " \
                                                                         "the same thing but are more verbose). Regardless of the " \
                                                                         "reason you arrived here however, the function argument " \
                                                                         "types you've passed to the template you called are illegal " \
                                                                         "in some way (as described)."
        #define STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_TUPLE_MSG(T) QUOTE_STRINGIFY(T) " is either not a \"std::tuple\" " \
                                                                       "specialization or it is a \"std::tuple\" specialization " \
                                                                       "but one or more of the types it's specialized on is not a " \
                                                                       "valid (legal) C++ function argument (parameter) type. This " \
                                                                       "error normally occurs when invoking any \"FunctionTraits\" " \
                                                                       "template in this library used to update a function's " \
                                                                       "arguments, such as \"ReplaceArgsTuple_t\" (among others), " \
                                                                       "and you've either passed a type that's not a \"std::tuple\" " \
                                                                       "specialization (if the particular template requires it), " \
                                                                       "or " \
                                                                       STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_MSG_CONCLUSION
        #define STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_TUPLE(T) static_assert(StdExt::IsValidFunctionArgTypesTuple_v<T>, \
                                                                                 STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_TUPLE_MSG(T));
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    ///////////////////////////////////////////////////////////////////
    // "IsValidFunctionArgTypes". Inherits from "std::true_type" if
    // all types in the template's parameter pack are valid (legal)
    // C++ function arg (parameter) types or "std::false_type"
    // otherwise. Note that type "void" isn't a legal type for
    // instance (you can't declare a function parameter of type
    // "void") and it's the most common one that might be encountered
    // normally. Its presence in a parameter list of type "(void)"
    // means an empty parameter list, but "void" itself in that
    // context is not an actual parameter (so this template traps
    // that), and if other parameter types are present in addition to
    // "void", it's simply illegal (so again, this template traps it).
    // Any other type would be very rare (such as an abominable
    // function type or a pointer or reference to one). This template
    // simply determines if "Ts" has any illegal function types
    // whatsoever.
    ///////////////////////////////////////////////////////////////////
    template <typename... Ts>
    struct IsValidFunctionArgTypes : std::bool_constant< IsValidFunctionArgTypesTuple_v< std::tuple<Ts...> > >
    {
    };

    // Usual "_v" helper variable for above template
    template <typename... Ts>
    inline constexpr bool IsValidFunctionArgTypes_v = IsValidFunctionArgTypes<Ts...>::value;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept ValidFunctionArgType_c = IsValidFunctionArgTypes_v<T>;
        #endif
        #define VALID_FUNCTION_ARG_TYPE_C StdExt::ValidFunctionArgType_c
        #define STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_MSG(T)
        #define STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES(T)
    #else
        #define VALID_FUNCTION_ARG_TYPE_C typename
        #define STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_MSG(T) QUOTE_STRINGIFY(T) " contains one or more invalid C++ " \
                                                                 "function argument (parameter) types. This error normally " \
                                                                 "occurs when invoking any \"FunctionTraits\" template in " \
                                                                 "this library used to update a function's arguments, such " \
                                                                 "as \"ReplaceArgsTuple_t\" (among others), and " \
                                                                 STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_MSG_CONCLUSION
        #define STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES(T) static_assert(StdExt::IsValidFunctionArgTypes_v<T>, \
                                                                           STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_MSG(T));
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    template <typename T>
    inline constexpr bool IsForEachArgFunctor_v = IsForEachTupleTypeFunctor_v<T>;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept ForEachArgFunctor_c = IsForEachArgFunctor_v<T>;
        #endif

        #define FOR_EACH_ARG_FUNCTOR_C StdExt::ForEachArgFunctor_c
    #else
        #define FOR_EACH_ARG_FUNCTOR_C typename
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    /////////////////////////////////////////////////////////////
    // For internal use only (implementation details for struct
    // "FunctionTraits" and its helper templates, both declared
    // after this namespace)
    /////////////////////////////////////////////////////////////
    namespace Private
    {
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

        /////////////////////////////////////////////////////////
        // Always true (undefined) by default unless an end-user
        // explicitly #defines it (we never do). If so then
        // function write traits won't be available in
        // "FunctionTraits" (we won't declare them). Only read
        // traits will be available. Note that there's usually
        // little reason for users to #define the following
        // constant however, unless they never use function write
        // traits (usually the case for most - read traits is far
        // more common), and just want to eliminate the overhead
        // of function write traits (fair enough though the
        // overhead isn't that significant - write traits *are*
        // required to support the library's function detection
        // templates declared later on however). Note that most
        // users will likely never #define the following so write
        // traits will always be available by default (unless
        // they're compiling in VC++ from Microsoft Visual Studio
        // 2017 where write traits aren't supported due to bugs
        // in that compiler - see test for #defined constant
        // MSVC_FROM_VISUAL_STUDIO_2017 just below)
        /////////////////////////////////////////////////////////
        #if !defined(REMOVE_FUNCTION_WRITE_TRAITS)
            //////////////////////////////////////////////////////
            // The following evaluates to true for all compilers
            // other than the version of VC++ released with
            // Microsoft Visual Studio 2017. Function write
            // traits will therefore be supported for all
            // compilers except the latter. See comments just
            // below.
            //////////////////////////////////////////////////////
            #if !defined(MSVC_FROM_VISUAL_STUDIO_2017)
                //////////////////////////////////////////////////////////
                // #define the following constant which is tested for
                // throughout the remainder of this file. Since we're
                // #defining it here function write traits will therefore
                // be supported for all compilers other than VC++ from
                // Visual Studio 2017 (as checked just above). Two
                // unrelated compiler bugs exist in that version which
                // cause compilation of our function write traits to fail
                // (details not worth getting into here since use of that
                // version is likely in significant decline by now). The
                // following will therefore be #undefined for that
                // compiler only meaning that function write traits will
                // be available in all compilers we support except VC++
                // from Visual Studio 2017. In that version they'll be
                // preprocessed out based on the following constant being
                // #undefined so they won't exist at all (and therefore
                // won't be available to end users). Only function read
                // traits will therefore be available in versions of VC++
                // from Visual Studio 2017. Function write traits will
                // only be available in versions of VC++ from Visual
                // Studio 2019 or later (as well as all non-Microsoft
                // compilers). Note that this constant is for internal
                // use only (we #undef it later).
                //////////////////////////////////////////////////////////
                #define FUNCTION_WRITE_TRAITS_SUPPORTED
            #endif
        #endif

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
        // Forward declaration of "struct FunctionTraitsBase"
        template <TRAITS_FREE_OR_MEMBER_FUNCTION_C F,
                  IS_FUNCTION_C RawTypeT,
                  typename ReturnTypeT,
                  CallingConvention CallingConventionT,
                  IS_CLASS_OR_VOID_C MemberFunctionClassT,
                  bool IsVariadicT,
                  bool IsConstT,
                  bool IsVolatileT,
                  FunctionReference FunctionReferenceT,
                  bool IsNoexceptT,
                  typename... ArgsT>
        struct FunctionTraitsBase;

        /////////////////////////////////////////////////////////////////////////
        // "IsFunctionTraitsBaseImpl" used to implement "IsFunctionTraitsBase_v"
        // just below. "Value" member of the following struct is set to "true"
        // if "T" is a "FunctionTraitsBase" specialization or any derivative of
        // one (normally the latter) or "false" otherwise.
        /////////////////////////////////////////////////////////////////////////
        template <typename T>
        struct IsFunctionTraitsBaseImpl
        {
        private:
            template <TRAITS_FREE_OR_MEMBER_FUNCTION_C F,
                      IS_FUNCTION_C RawTypeT,
                      typename ReturnTypeT,
                      CallingConvention CallingConventionT,
                      IS_CLASS_OR_VOID_C MemberFunctionClassT,
                      bool IsVariadicT,
                      bool IsConstT,
                      bool IsVolatileT,
                      FunctionReference FunctionReferenceT,
                      bool IsNoexceptT,
                      typename... ArgsT>
            static constexpr std::true_type Test(const FunctionTraitsBase<F,
                                                                          RawTypeT,
                                                                          ReturnTypeT,
                                                                          CallingConventionT,
                                                                          MemberFunctionClassT,
                                                                          IsVariadicT,
                                                                          IsConstT,
                                                                          IsVolatileT,
                                                                          FunctionReferenceT,
                                                                          IsNoexceptT,
                                                                          ArgsT...> &);

            static constexpr std::false_type Test(...);

            ///////////////////////////////////////////////////////////////
            // Note: Template "U" in the following template is always the
            // same as the "T" template arg for "IsFunctionTraitsBaseImpl"
            // itself (since we always pass "T" to the following
            // template), but we can't remove "U" from the following and
            // rely on "T" directly in the "Process" specialization below.
            // We need to pass "T" to this template and rely on "U"
            // instead (again, we always pass "T" itself). If we eliminate
            // "U" and rely directly on "T", and "T *" isn't legal (i.e. a
            // pointer to "T" isn't legal, such as when "T" is an
            // abominable function - see comments in the "Process"
            // specialization below), then compilation of the "Process"
            // specialization below will immediately fail because the call
            // to "std::remove_cv_t><T *>" in the specialization below
            // will be immediately processed when
            // "IsFunctionTraitsBaseImpl" itself is compiled (due to the
            // illegal pointer). "T" is already known at that time that is
            // so it will be immediately plugged into "std::remove_cv_t><T
            // *>" and compilation will therefore fail if "T *" is
            // illegal. The use of "U" in the following however ensures
            // that won't happen. Instead, "std::remove_cv_t><U *>" is
            // effectively delay-compiled until "Process" itself is
            // specialized so it will therefore be compiled in a proper
            // SFINAE context (at that time). Compilation therefore won't
            // fail. SFINAE will simply fail instead so the following
            // primary template will then kick as required.
            ///////////////////////////////////////////////////////////////
            template <typename U,
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct Process : std::false_type
            {
            };

            /////////////////////////////////////////////////////////////////////
            // Note: This specialization can't rely on the "FunctionTraitsBase"
            // template args directly in the following template header (which
            // would be the more straightforward way of implementing it) since
            // C++ can't deduce the types this way (using any mainstream
            // technique I've tried). It results in errors similar to the
            // following (a MSFT error but all other supported compilers result
            // in their own similar warnings):
            //
            //   error C2764: 'F': template parameter not used or deducible in
            //   partial specialization
            //   'StdExt::Private::IsFunctionTraitsBase<T>::Process<U,void>'
            //
            // The same error is repeated for every template arg of
            // "FunctionTraitsBase" (none of them can be deduced when referring
            // to them in any "std::void_t" or "std::enable_if_t call below).
            // The following technique works instead (though not as clean as the
            // above technique would be if it were supported).
            /////////////////////////////////////////////////////////////////////
            template <typename U>
            struct Process<U,
                           ////////////////////////////////////////////////////////////////////
                           // Invokes "Test" above, passing it a "U" instance (reference) via
                           // the call to "declval" below so the "Test" overload returning
                           // "std::true_type" will kick in if "U" itself is a
                           // "FunctionTraitsBase" specialization or derivative of one. The
                           // following "Process" specialization will therefore kick in which
                           // inherits from "std::true_type" to indicate this (that "U" is a
                           // "FunctionTraitsBase" specialization or derivative of one). OTOH,
                           // the variadic overload of "Test" above will kick in instead if
                           // "U" isn't a "FunctionTraitsBase" specialization or derivative,
                           // and that overload returns "std::false_type" so SFINAE will
                           // therefore fail in the following call. The primary template of
                           // "Process" above will therefore kick in which inherits from
                           // "std::false_type" indicating that "U" isn't a
                           // "FunctionTraitsBase" specialization or derivative. This is the
                           // usual reason SFINAE fails, because the variadic version of
                           // "Test" above is invoked (meaning "U" isn't a
                           // "FunctionTraitsBase" specialization or derivative of one), but
                           // SFINAE can fail for other possible reasons as well (though rare
                           // in practice). If "U" is a "FunctionTraitsBase" derivative for
                           // instance but was derived from "FunctionTraitsBase" using the
                           // "private" or "protected" keyword, then the following will also
                           // fail SFINAE simply because the "FunctionTraitsBase" class of "U"
                           // is inaccessible (so the version of "Test" returning
                           // "std::true_type" above won't kick in because of the
                           // "inaccessibility" issue which will therefore cause SFINAE to
                           // fail here). This means that the primary template will kick in
                           // indicating that "U" is a "FunctionTraitsBase" derivative but the
                           // "FunctionTraitsBase" base class of "U" isn't accessible
                           // (according to the usual C++ rules for accessing base classes
                           // derived via the private or protected keyword). In reality this
                           // scenario is extraordinarily unlikely to ever occur however so we
                           // won't worry about it for now (the library itself never creates
                           // this situation so an end-user would have to explicitly create it
                           // - very unlikely). Unfortunately it's difficult to circumvent so
                           // we'll live with the limitation for now (assuming it even is a
                           // limitation - it's usually the desired behavior in fact). Trying
                           // to re-factor this code to rely on a more mainstream technique
                           // based on passing "U" to "std::is_base_of" instead proved to be
                           // difficult because of C++ issues deducing the template args of
                           // "FunctionTraitsBase" - longer story but I couldn't find a way to
                           // do it (to my surprise). The call to "Test" below is the best
                           // technique I could find for now and while it will be reliable
                           // under almost all real-world situation, it remains a potential
                           // issue for the reason just described (i.e., "std::false_type"
                           // results if the "FunctionTraitsBase" is inaccessible as just
                           // described).
                           //
                           // In any case, note that SFINAE can also fail in the following
                           // call for other (rare) reasons as well. If "U" is "void" for
                           // instance, then even the variadic version of "Test" above won't
                           // kick in so SFINAE immediately fails (since "void" can't be
                           // converted to "..."). Or, as another example, "U" may not support
                           // references for instance, i.e., "U &" and "U &&" might be
                           // illegal, in particular if "U" is an abominable function, i.e., a
                           // native C++ function type with the "const" and/or "volatile"
                           // and/or & or && qualifiers. References (or pointers) to such
                           // functions are illegal in C++ so the following will fail SFINAE
                           // for that reason (since "U &&" itself is illegal when "U" is an
                           // abominable function - our attempt to create "declval(U)" below
                           // will therefore fail). That's fine for our purposes however since
                           // "U" isn't a "FunctionTraitsBase" specialization or derivative of
                           // one in this case anyway. It's an abominable function type or
                           // possibly some other type that doesn't support references though
                           // I'm not sure if any others exist in C++ (at the top of my head).
                           // Our primary template therefore correctly kicks in in such cases
                           // which correctly inherits from "std::false_type" since such types
                           // are never "FunctionTraitsBase" specializations or derivatives of
                           // them (i.e., if "U" is a "FunctionTraitsBase" specialization or
                           // derivative of one then since "U" is just a struct then "U"
                           // itself is always legal normally - any other type wouldn't be).
                           ////////////////////////////////////////////////////////////////////
                           std::enable_if_t< decltype(Test(std::declval<U>()))::value >
                          >
                          : std::true_type
            {
            };

        public:
            static constexpr bool Value = Process<std::remove_cv_t<T>>::value;
        }; // struct IsFunctionTraitsBaseImpl

        ///////////////////////////////////////////////////////////////////
        // Returns "true" if "T" is a "FunctionTraitsBase" specialization
        // or any derivative of one (usually the latter) or "false"
        // otherwise.
        ///////////////////////////////////////////////////////////////////
        template <typename T>
        inline constexpr bool IsFunctionTraitsBase_v = IsFunctionTraitsBaseImpl<T>::Value;

        #if defined(USE_CONCEPTS)
            template <typename T>
            concept IsFunctionTraitsBase_c = IsFunctionTraitsBase_v<T>;

            // For internal use only (we #undef it later)
            #define FUNCTION_TRAITS_BASE_C StdExt::Private::IsFunctionTraitsBase_c
        #else
            // For internal use only (we #undef it later)
            #define FUNCTION_TRAITS_BASE_C typename

            template <typename FunctionTraitsBaseT>
            struct StaticAssertInvalidFunctionTraitsBase : std::false_type
            {
                // Always triggers at this point!
                static_assert(AlwaysFalse_v<FunctionTraitsBaseT>,
                              "Template arg " QUOTE_STRINGIFY(FunctionTraitsBaseT) \
                              " must be a \"FunctionTraitsBase\" derivative (normally a " \
                              "\"FunctionTraits\" specialization). This error normally " \
                              "occurs when using any of the library's helper templates " \
                              "taking \"F1\" and \"F2\" template args, usually one of " \
                              "the templates with the term \"Match\" in its name (for " \
                              "comparing the arg types and/or return type of two functions), " \
                              "and template arg \"F2\" either isn't a function type or " \
                              "not a function type supported by this library (normally " \
                              "the former since all mainstream C++ function types are " \
                              "supported by this library). Note that template arg \"F1\" " \
                              "itself has already been established as a valid (supported) " \
                              "function type in this case, hence this \"static_assert\" " \
                              "only applies when template arg \"F2\" is invalid as noted. " \
                              "Please note that if you invoked a template with the term " \
                              "\"Match\" in it using \"struct FunctionTraits\" directly " \
                              "instead of one of its helper templates (i.e., you directly " \
                              "called the template as a member of \"struct FunctionTraits\" " \
                              "itself), the same issue just described still exists, only " \
                              "\"F2\" now refers to the (invalid) \"FunctionTraits\" template " \
                              "arg you passed to the \"Match\" template you invoked (the " \
                              "template arg is merely called \"F2\" when invoking the " \
                              "\"Match\" template using its \"FunctionTraits\" helper template " \
                              "instead of as member of \"struct FunctionTraits\" directly).");
            };
        #endif // #if defined(USE_CONCEPTS)

        template <TRAITS_FREE_OR_MEMBER_FUNCTION_C F, // Function's full type. Use the "FunctionType_t"
                                                      // member in the class to return this.
                  #undef TRAITS_FREE_OR_MEMBER_FUNCTION_C // Done with this just above (for internal use only)
                  IS_FUNCTION_C RawTypeT, // Function's raw (native) C++ type (raw version of "F" above). Use
                                          // the "RawType_t" member in the class to return this.
                  typename ReturnTypeT, // Function's return type (use the "ReturnType_t" member to return this)
                  CallingConvention CallingConventionT, // Function's calling convention implicitly or explicitly
                                                        // declared in the function (enum corresponds to our
                                                        // #defined STDEXT_CC_? constants, whichever the function
                                                        // uses). Use the "CallingConvention_v" member in the class
                                                        // to return this.
                  IS_CLASS_OR_VOID_C MemberFunctionClassT, // Applicable to non-static member functions only (use
                                                           // the "MemberFunctionClass_t member in the class to
                                                           // return this). Stores the class this non-static
                                                           // member function belongs to. Always "void" if not a
                                                           // non-static member function.
                  bool IsVariadicT, // "true" if function is variadic (last arg is "...") or "false" otherwise
                                    // (use the "IsVariadic_v" member in the class to return this)
                  bool IsConstT, // Applicable to non-static member functions or abominable functions only (use
                                 // the "IsConst_v" member in the class to return this). Set to "true" if a
                                 // "const" member function or "false" otherwise. Always false for free functions.
                  bool IsVolatileT, // Applicable to non-static member functions or abominable functions only (use
                                    // the "IsVolatile_v" member in the class to return this). Set to "true" if a
                                    // "volatile" member function or "false" otherwise. Always false for free functions.
                  FunctionReference FunctionReferenceT, // Applicable to non-static member functions or abominable
                                                        // functions only (use the "FunctionReference_v" member in the
                                                        // class to return this). Set to the given "FunctionReference"
                                                        // enumerator if one is present in the function (& or &&).
                                                        // Always "FunctionReference::None" for free functions.
                  bool IsNoexceptT, // "true" if the function is declared "noexcept" or "false" otherwise (use the
                                    // "IsNoexcept_v" member in the class to return this)
                  typename... ArgsT> // Function's arguments (types) in left-to-right order of declaration (as
                                     // would be expected). Use the "ArgTypes_t" member in the class to return
                                     // this (or the "ArgType_t" helper template to index into it).
        struct FunctionTraitsBase
        {
            // See this constant for details
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concepts kick in in the template
                // declaration above instead (macros for these
                // above resolve to the "typename" keyword when
                // concepts aren't supported and the following
                // "static_asserts" are then used instead)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_TRAITS_FREE_OR_MEMBER_FUNCTION(F)
                #undef STATIC_ASSERT_IS_TRAITS_FREE_OR_MEMBER_FUNCTION // Done with this just above (for internal use only))
                STATIC_ASSERT_IS_FUNCTION(RawTypeT)
                STATIC_ASSERT_IS_CLASS_OR_VOID(MemberFunctionClassT)
            #endif

            ///////////////////////////////////////////////////////////////
            // Template args "MemberFunctionClassT", "IsConstT",
            // "IsVolatileT" and "FunctionReferenceT" only apply when "F"
            // is a non-static member function pointer (or a reference to
            // one). Otherwise we're dealing with a free function so
            // they're not applicable (so we make sure of this) ...
            ///////////////////////////////////////////////////////////////
            static_assert(IsTraitsMemberFunction_v<F> || // Always true when "F" is a non-static member function
                                                         // pointer or a reference to one (pointer may be cv-qualified).
                                                         // Otherwise "F" must be a free function, or a pointer to one,
                                                         // or a reference to one, or a reference to a pointer to one.
                                                         // In all free function cases the 4 conditions just below must
                                                         // hold (since they don't apply to free functions) ...
                          (std::is_void_v<MemberFunctionClassT> &&
                           !IsConstT &&
                           !IsVolatileT &&
                           FunctionReferenceT == StdExt::FunctionReference::None));

            ///////////////////////////////////////////////////////////////////////
            // Function's full type. Same as template arg "F" used to instantiate
            // each "FunctionTraits" specialization (which all derive from
            // "FunctionTraitsBase"). If the template arg "F" passed to
            // "FunctionTraits" is a functor however (a class/struct with an
            // "operator()" member), then the "F" we receive here isn't that
            // functor (its class/struct), but the type of "operator()" in the
            // functor. If the template arg "F" passed to "FunctionTraits" is a
            // "std::function" specialization then the "F" we receive here isn't
            // that specialization but the function type it was specialized on
            ///////////////////////////////////////////////////////////////////////
            using FunctionType_t = F;

            // Helper variable for above type (returns it as a WYSIWYG "tstring_view")
            static constexpr tstring_view FunctionTypeName_v = TypeName_v<FunctionType_t>;

            ///////////////////////////////////////////////////////////////////////
            // Function's raw type. Same as template arg "F" used to instantiate
            // each "FunctionTraits" specialization (which all derive from
            // "FunctionTraitsBase"), but stripped of any references and pointers
            // (including for non-static member functions). The resulting type is
            // therefore just the raw (native) C++ function type of "F". If the
            // template arg "F" passed to "FunctionTraits" is a functor however (a
            // class/struct with an "operator()" member), then the "F" we receive
            // here isn't that functor (its class/struct), but the type of
            // "operator()" in the functor. "RawTypeT" is therefore the raw type
            // of "operator()". If the template arg "F" passed to "FunctionTraits"
            // is a "std::function" specialization then the "F" we receive here
            // isn't that specialization but the function type it was specialized
            // on.
            ///////////////////////////////////////////////////////////////////////
            using RawType_t = RawTypeT;

            // Helper variable for above type (returns it as a WYSIWYG "tstring_view")
            static constexpr tstring_view RawTypeName_v = TypeName_v<RawType_t>;

            ////////////////////////////////////////////////////////////////////////
            // Function's return type. The syntax for accessing this directly is a
            // bit verbose however since you have to apply the "typename" keyword.
            // You should therefore normally rely on the helper alias templates
            // "ReturnType_t" or "FunctionTraitsReturnType_t" instead (declared
            // later in this file). The former just defers to the latter and should
            // usually be used (it's easier). See these for details (plus
            // examples).
            ////////////////////////////////////////////////////////////////////////
            using ReturnType_t = ReturnTypeT;

            // Helper variable for above type (returns it as a WYSIWYG "tstring_view")
            static constexpr tstring_view ReturnTypeName_v = TypeName_v<ReturnType_t>;

            template <typename T>
            static constexpr bool IsReturnTypeSame_v = std::is_same_v<ReturnType_t, T>;

            static constexpr bool IsReturnTypeVoid_v = std::is_void_v<ReturnType_t>;

        #if defined(STDEXT_SUPPORT_DEPRECATED)
            VARIABLE_DEPRECATED_DUE_TO_NAME_CHANGE(IsReturnTypeVoid_v)
            static constexpr bool IsVoidReturnType_v = IsReturnTypeVoid_v; // Defer to variable just above
        #endif

            //////////////////////////////////////////////////////////
            // "IsReturnTypeMatch_v" implementation declared further
            // below (following #defined here so we can apply this
            // line in both the concepts or "static_assert"
            // implementations just below)
            //////////////////////////////////////////////////////////
            #define IS_RETURN_TYPE_MATCH IsReturnTypeSame_v<typename FunctionTraitsBaseT::ReturnType_t>

    #if defined(USE_CONCEPTS)
            #define IS_RETURN_TYPE_MATCH_V_IMPL IS_RETURN_TYPE_MATCH
    #else
        ///////////////////////////////////////////////////////////
        // "IsReturnTypeMatch_v" (private) implementation. Latter
        // variable declared just below this implementation.
        ///////////////////////////////////////////////////////////
        private:
            template <typename FunctionTraitsBaseT,
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct IsReturnTypeMatchImpl : StaticAssertInvalidFunctionTraitsBase<FunctionTraitsBaseT>
            {
            };

            template <typename FunctionTraitsBaseT>
            struct IsReturnTypeMatchImpl<FunctionTraitsBaseT,
                                         std::enable_if_t< IsFunctionTraitsBase_v<FunctionTraitsBaseT> >
                                        >
                                        : std::bool_constant<IS_RETURN_TYPE_MATCH>
            {
            };

            #define IS_RETURN_TYPE_MATCH_V_IMPL IsReturnTypeMatchImpl<FunctionTraitsBaseT>::value
        public:
    #endif // #if defined(USE_CONCEPTS)

            template <FUNCTION_TRAITS_BASE_C FunctionTraitsBaseT>
            static constexpr bool IsReturnTypeMatch_v = IS_RETURN_TYPE_MATCH_V_IMPL;
            // Done with these (for private use of above template only) ...
            #undef IS_RETURN_TYPE_MATCH_V_IMPL
            #undef IS_RETURN_TYPE_MATCH

            /////////////////////////////////////////////////////////////////////////
            // Function's calling convention (implicitly or explicitly declared in
            // the function, usually the former but it doesn't matter). Note that if
            // a calling convention isn't supported, usually based on your compiler
            // options (target environment, etc.), then the compiler will replace it
            // with the "cdecl" calling convention normally (so the following will
            // reflect that). If you declare a function as "stdcall" for instance
            // (or any other besides "cdecl"), and it's not supported based on your
            // compiling options, then the compiler will replace it with "cdecl"
            // instead (and you may or may not receive a compiler warning that this
            // occurred depending on your platform and compiler options). Note that
            // "cdecl" itself is always (realistically) supported by all compilers
            // AFAIK (those we support at least but very likely all others as well)
            /////////////////////////////////////////////////////////////////////////
            static constexpr CallingConvention CallingConvention_v = CallingConventionT; // Adding "_v" suffix so name isn't identical to "enum class CallingConvention"
                                                                                         // itself (to prevent confusion with the latter - also prevents an error in Clang
                                                                                         // if declared with the same name, unless "enum class" is explicitly present in
                                                                                         // the type itself but using a different variable name here eliminates the issue
                                                                                         // altogether). Note BTW that we apply "_v" to all variable templates outside the
                                                                                         // class but not for variable names inside (but making an exception in this case).
                                                                                         // Maybe we should (and apply _t suffix to class aliases as well), but leaving
                                                                                         // things as-is for now.

            // Helper variable for above variable (returns it as a WYSIWYG "tstring_view")
            static constexpr tstring_view CallingConventionName_v = CallingConventionToString(CallingConvention_v);

            //////////////////////////////////////////////////////////////////////
            // Applicable to non-static member functions only. Stores the class
            // (type) this non-static member function belongs to. Always "void"
            // otherwise. Use the "FunctionClassification" member below to check
            // this. Note that at this writing, the following isn't supported for
            // static member functions (it's always void for them) since C++
            // doesn't provide any way to distinguish between ordinary free
            // functions (i.e., those declared at namespace scope) and static
            // member functions (i.e., "free" functions declared at class scope).
            // Both qualify as "free" functions and there's no way to determine
            // the scope (namespace-based vs static so class-based)
            //////////////////////////////////////////////////////////////////////
            using MemberFunctionClass_t = MemberFunctionClassT;

            // Helper variable for above type (returns it as a WYSIWYG "tstring_view")
            static constexpr tstring_view MemberFunctionClassName_v = std::is_void_v<MemberFunctionClass_t> ? tstring_view(_T("")) // Free or abominable function (so there is no class)
                                                                                                            : TypeName_v<MemberFunctionClass_t>; // Non-static member function

            ///////////////////////////////////////////////////////////////////////
            // "true" if the function is variadic (last arg of function is "...")
            // or "false" otherwise. Note that "..." refers to the old-school
            // definition in C, so nothing to do with variadic template arguments
            // in C++.
            ///////////////////////////////////////////////////////////////////////
            static constexpr bool IsVariadic_v = IsVariadicT;

            ////////////////////////////////////////////////////////////////////////
            // Is non-static member function declared with the "const" keyword.
            // Applicable to non-static member functions and abominable functions
            // only (use the "FunctionClassification" member further below to check
            // this). Always false otherwise (N/A in this case).
            ////////////////////////////////////////////////////////////////////////
            static constexpr bool IsConst_v = IsConstT;

            ////////////////////////////////////////////////////////////////////////
            // Is non-static member function declared with the "volatile" keyword.
            // Applicable to non-static member functions and abominable functions
            // only (use the "FunctionClassification" member further below to check
            // this). Always false otherwise (N/A in this case).
            ////////////////////////////////////////////////////////////////////////
            static constexpr bool IsVolatile_v = IsVolatileT;

            //////////////////////////////////////////////////////////////////////
            // Is non-static member function declared with a reference-qualifier (&
            // or &&). Applicable to non-static member functions and abominable
            // functions only (use the "FunctionClassification" member further
            // below to check this). Always "FunctionReference::None" otherwise
            // (N/A in this case).
            //////////////////////////////////////////////////////////////////////
            static constexpr FunctionReference FunctionReference_v = FunctionReferenceT;

            // Helper variable for above variable (returns it as a WYSIWYG "tstring_view")
            template <bool UseAmpersands = true>
            static constexpr tstring_view FunctionReferenceName_v = FunctionReferenceToString(FunctionReference_v,
                                                                                              UseAmpersands);

            // "true" if the function is declared "noexcept" or "false" otherwise
            static constexpr bool IsNoexcept_v = IsNoexceptT;

            ///////////////////////////////////////////////////////////////////////
            // Function's arguments (types) in left-to-right order of declaration
            // (as would be expected). The syntax for accessing this directly is
            // verbose however (using the "Args" struct below), so you should
            // normally rely on the helper alias templates "ArgType_t" (usually),
            // or less frequently "FunctionTraitsArgType_t" instead (both declared
            // later in this file). The former just defers to the latter. See
            // these for details (plus examples).
            ///////////////////////////////////////////////////////////////////////
            using ArgTypes_t = std::tuple<ArgsT...>;

            ////////////////////////////////////////////////////////////
            // Number of arguments in the function. This is officially
            // called "arity" but the term is obscure so we'll stick
            // with a name everyone can relate to.
            ////////////////////////////////////////////////////////////
            static constexpr std::size_t ArgCount_v = std::tuple_size_v<ArgTypes_t>;

            static constexpr bool IsArgListEmpty_v = (ArgCount_v == 0 && !IsVariadic_v);

            template <std::size_t I /* Zero-based */>
            static constexpr bool IndexLessThanArgCount_v = (I < ArgCount_v);

            #if defined(USE_CONCEPTS)
                // For internal use only (we #undef it later)
                #define REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_ARG_COUNT(I) requires BaseClass::template IndexLessThanArgCount_v<(I)>
            #else
                // For internal use only (we #undef it later)
                #define REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_ARG_COUNT(I)
            #endif // #if defined(USE_CONCEPTS)

            template <std::size_t I /* Zero-based */>
            static constexpr bool IndexLessThanOrEqualToArgCount_v = (I <= ArgCount_v);

            #if defined(USE_CONCEPTS)
                // For internal use only (we #undef it later)
                #define REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) requires BaseClass::template IndexLessThanOrEqualToArgCount_v<(I)>
            #else
                // For internal use only (we #undef it later)
                #define REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I)
            #endif // #if defined(USE_CONCEPTS)

            //////////////////////////////////////////////
            // For "requires" clause in "ArgType_t" just
            // below (as well as the library's helper
            // templates for the latter, declared at
            // namespace scope later on but which also
            // ultimately rely on it)
            //////////////////////////////////////////////
            template <std::size_t I /* Zero-based */,
                      bool ReturnVoidIfInvalidIndex>
            static constexpr bool IndexLessThanArgCountOrReturnVoid_v = IndexLessThanArgCount_v<I> || ReturnVoidIfInvalidIndex;

        //////////////////////////////////////////////////////////
        // "ArgType_t" (private) implementation. Latter variable
        // declared just below this implementation.
        //////////////////////////////////////////////////////////
        private:
            template <std::size_t I,
                  #if !defined(USE_CONCEPTS)
                      bool ReturnVoidIfInvalidIndex,
                  #endif
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct ArgTypeImpl
            {
                #if !defined(USE_CONCEPTS)
                    static_assert(ReturnVoidIfInvalidIndex,
                                  "Invalid index \"I\" (non-existent function arg). This "
                                  "error normally occurs when invoking any \"FunctionTraits\" "
                                  "template taking an \"I\" template arg in order to process "
                                  "a function's arg(s) in some way, and an invalid index was "
                                  "passed for \"I\" (the arg's zero-based index into the "
                                  "function's arg list). Note that the template you called "
                                  "invoked member \"FunctionTraits::ArgType_t\" (or more "
                                  "commonly the helper template \"ArgType_t\" that wraps it), "
                                  "assuming you didn't call this template directly. No matter "
                                  "how it was called however, template arg \"I\" must be less "
                                  "than the number of args in the function as returned by "
                                  "the \"FunctionTraits::ArgCount_v\" member (or more commonly "
                                  "the helper template \"ArgCount_v\" that wraps it), since "
                                  "the \"ReturnVoidIfInvalidIndex\" template arg of "
                                  "\"ArgType_t\" is false. Note that the name of the latter "
                                  "template arg can vary depending on the template you're "
                                  "calling but ultimately corresponds to the "
                                  "\"ReturnVoidIfInvalidIndex\" template arg of template "
                                  "\"ArgType_t\" (you either called it directly to arrive "
                                  "here or the template you called indirectly invoked it). "
                                  "Pass \"true\" for the latter template arg instead to "
                                  "suppress this error and have \"ArgType_t\" return "
                                  "\"void\" instead (again, the name of this template arg "
                                  "depends on the template you're calling). The behavior "
                                  "of returning \"void\" will then vary depending on the "
                                  "template you're calling (if not \"ArgType_t\" itself). "
                                  "See the documentation for the specific template you're "
                                  "calling for details.");
                #endif // #if !defined(USE_CONCEPTS)

                using Type = void;
            };

            template <std::size_t I
                  #if !defined(USE_CONCEPTS)
                      , bool ReturnVoidIfInvalidIndex
                  #endif
                     >
            struct ArgTypeImpl<I,
                           #if !defined(USE_CONCEPTS)
                               ReturnVoidIfInvalidIndex,
                           #endif

                           //////////////////////////////////////
                           // Not Visual Studio 16.4 or earlier
                           // (usually the case) ...
                           //////////////////////////////////////
                           #if !defined(_MSC_VER) || (_MSC_VER >= MSC_VER_2019_V16_5)
                               #define IMPL IndexLessThanArgCount_v<I>
                           #else // Visual Studio 16.4 or earlier (usually not)
                               /////////////////////////////////////////////
                               // Call to "IndexLessThanArgCount_v" just
                               // above is buggy in Visual Studio 2019
                               // V16.4 or earlier (MSVC V19.24 or
                               // earlier). Results in a compiler error
                               // (fixed by MSFT in V16.5). The following
                               // is a work-around (same as
                               // "IndexLessThanArgCount_v" just above but
                               // no reliance on a template).
                               /////////////////////////////////////////////
                               #define IMPL I < ArgCount_v
                           #endif

                               std::enable_if_t< IMPL >
                               #undef IMPL
                              >
            {
                using Type = std::tuple_element_t<I, ArgTypes_t>;
            };

        public:
            ////////////////////////////////////////////////////////////////////////
            // ArgType_t. Helper alias for "ArgTypes_t" further above. Retrieves an
            // individual arg type from "ArgTypes_t" by passing its zero-based
            // index. Called as per the following example but see IMPORTANT section
            // below for a cleaner way (and also see function template
            // "ForEachArg()" if you need to iterate all arg types instead of just
            // accessing an individual arg type as the following does):
            //
            //  Example
            //  -------
            //       ////////////////////////////////////////////////////////
            //       // Function (we'll detect the type of its 3rd arg just
            //       // below, a "float *"):
            //       ////////////////////////////////////////////////////////
            //       int MyFunction(int, double, float *, const std::string &)
            //       {
            //       }
            //
            //       ///////////////////////////////////////////////////////////////////
            //       // Type of the 3rd arg of above function (a "float *"). Passing 2
            //       // here since template arg "I" is zero-based. Note that if "I" is
            //       // out of bounds (>= the number of args in the target function),
            //       // then template arg "ReturnVoidIfInvalidIndex" controls the
            //       // behavior. If "false" (the default), then a compiler error
            //       // occurs as a constraints (concept) violation in C++20 or
            //       // later (but see USE_CONCEPTS in "CompilerVersions.h" for
            //       // details), or a "static_assert" occurs in C++17 instead. Pass
            //       // true for the "ReturnVoidIfInvalidIndex" template arg instead
            //       // if you wish to suppress the compiler error and return "void"
            //       // instead (since "void" isn't a legal parameter type in C++ so
            //       // if returned it guarantees that "I" isn't valid). Note that
            //       // this is sometimes useful for SFINAE situations for instance,
            //       // where you don't a compiler error to trigger when "I" is
            //       // invalid (targets a non-existent arg), but for "void" to
            //       // simply be returned instead (which you can even test for
            //       // if required using "std::is_void").
            //       //
            //       // Lastly, note that the following syntax is (obviously) verbose
            //       // so see IMPORTANT section just below for a cleaner way (that
            //       // wraps the following so you should normally rely on it instead).
            //       ///////////////////////////////////////////////////////////////////
            //       using TypeOf3rdArg = typename FunctionTraits<decltype(MyFunction)>::template ArgType_t<2>;
            //
            // IMPORTANT:
            // ---------
            // Note that the syntax above is verbose so you should normally rely on
            // the helper alias templates "FunctionTraitsArgType_t" or "ArgType_t"
            // instead (usually the latter - both declared later in this file). The
            // latter just defers to the former but in either case the syntax is
            // much cleaner than using the above technique directly (since the helper
            // aliases wrap it all up for you). See these templates for details. The
            // above example for instance can be replaced with this (using the helper
            // aliases):
            //
            //       //////////////////////////////////////////////////////////
            //       // Still verbose but better than the original call above
            //       // (but read on - it gets even easier)
            //       //////////////////////////////////////////////////////////
            //       using TypeOf3rdArg = FunctionTraitsArgType_t<FunctionTraits<decltype(MyFunction)>, 2>;
            //
            // or simplifying the above a bit (by breaking it into 2 lines, making
            // it longer but still arguably more digestible):
            //
            //       using MyFunctionTraits = FunctionTraits<decltype(MyFunction)>;
            //       using TypeOf3rdArg = FunctionTraitsArgType_t<MyFunctionTraits, 2>;
            //
            // or better yet, you can rely on the following helper instead (so the
            // syntax is now about as clean as it gets):
            //
            //       using TypeOf3rdArg = ArgType_t<decltype(MyFunction), 2>;
            //
            // or lastly, you can break the line above into the following two lines
            // instead if you wish (though the above line isn't that verbose so
            // still arguably better):
            //
            //       using MyFunctionType = decltype(MyFunction);
            //       using TypeOf3rdArg = ArgType_t<MyFunctionType, 2>;
            ////////////////////////////////////////////////////////////////////////
            template <std::size_t I /* Zero-based */,
                      bool ReturnVoidIfInvalidIndex = false>
            #if defined(USE_CONCEPTS)
                requires IndexLessThanArgCountOrReturnVoid_v<I, ReturnVoidIfInvalidIndex>
            #endif
            using ArgType_t = typename ArgTypeImpl<I
                                               #if !defined(USE_CONCEPTS)
                                                   , ReturnVoidIfInvalidIndex
                                               #endif
                                                  >::Type;

            //////////////////////////////////////////////
            // For "requires" clause in "ArgTypeName_v"
            // just below (as well as the library's
            // helper templates for the latter, declared
            // at namespace scope later on but which also
            // ultimately rely on it)
            //////////////////////////////////////////////
            template <std::size_t I /* Zero-based */,
                      bool ReturnEmptyStringIfInvalidIndex>
            static constexpr bool IndexLessThanArgCountOrReturnEmptyString_v = IndexLessThanArgCountOrReturnVoid_v<I, ReturnEmptyStringIfInvalidIndex>;

            //////////////////////////////////////////////////
            // Helper variable for "ArgType_t above (returns
            // it as a WYSIWYG "tstring_view")
            //////////////////////////////////////////////////
            template <std::size_t I /* Zero-based */,
                      bool ReturnEmptyStringIfInvalidIndex = false>
            #if defined(USE_CONCEPTS)
                requires IndexLessThanArgCountOrReturnEmptyString_v<I, ReturnEmptyStringIfInvalidIndex>
            #endif
            static constexpr tstring_view ArgTypeName_v = IndexLessThanArgCount_v<I> || !ReturnEmptyStringIfInvalidIndex ? TypeName_v<ArgType_t<I, ReturnEmptyStringIfInvalidIndex>>
                                                                                                                         : tstring_view(_T(""));

            ////////////////////////////////////////////////
            // For "requires" clause in "IsArgTypeSame_v"
            // just below (as well as the library's helper
            // templates for the latter, declared at
            // namespace scope later on but which also
            // ultimately rely on it)
            ////////////////////////////////////////////////
            template <std::size_t I /* Zero-based */,
                      bool CompareWithVoidIfInvalidIndex>
            static constexpr bool IndexLessThanArgCountOrCompareWithVoid_v = IndexLessThanArgCountOrReturnVoid_v<I, CompareWithVoidIfInvalidIndex>;

            template <std::size_t I /* Zero-based */,
                      typename T,
                      bool CompareWithVoidIfInvalidIndex = false>
            #if defined(USE_CONCEPTS)
                requires IndexLessThanArgCountOrCompareWithVoid_v<I, CompareWithVoidIfInvalidIndex>
            #endif
            static constexpr bool IsArgTypeSame_v = std::is_same_v< ArgType_t<I, CompareWithVoidIfInvalidIndex>,
                                                                    std::conditional_t<std::is_void_v<T>, // If "T" is void then it might be cv-qualified
                                                                                                          // (though rare in practice) but we want to ignore
                                                                                                          // these (and just compare with plain "void")
                                                                                       void, // Remove the cv-qualifiers if any (usually not). If
                                                                                             // call to "ArgType_t" above returns "void" then we'll
                                                                                             // be comparing it with "void", hence returning "true",
                                                                                             // not a cv-qualified "void" (such as "const void"), which
                                                                                             // would erroneously return "false" (rare as this scenario
                                                                                             // normally would be). Note that "void" will only be
                                                                                             // returned by "ArgType_t" above if the function has no
                                                                                             // "Ith" arg and "CompareWithVoidIfInvalidIndex" is true.
                                                                                       T
                                                                                      >
                                                                  >;
            ////////////////////////////////////////////////
            // For use by "IsArgTypeMatch_v" just below (as
            // well as the library's helper templates for
            // the latter, declared at namespace scope
            // later on but which also ultimately rely on
            // it). Used in a "requires" clause if C++20
            // or later, or a "static_assert" in C++17.
            ////////////////////////////////////////////////
            template <FUNCTION_TRAITS_BASE_C FunctionTraitsBaseT,
                      std::size_t I /* Zero-based */,
                      bool ReturnFalseIfOneHasArgOtherDoesnt>
            static constexpr bool IndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v = (IndexLessThanArgCount_v<I> && FunctionTraitsBaseT::template IndexLessThanArgCount_v<I>) || // True if arg "I" exists in *both* functions
                                                                                          ((IndexLessThanArgCount_v<I> || FunctionTraitsBaseT::template IndexLessThanArgCount_v<I>) && ReturnFalseIfOneHasArgOtherDoesnt); // "True" if arg "I" exists in one function only (not both)
                                                                                                                                                                                                                           // AND "ReturnFalseIfOneHasArgOtherDoesnt" is true

            ///////////////////////////////////////////////////////
            // "IsArgTypeMatch_v" implementation declared further
            // below (following #defined here so we can apply
            // this line in both the concepts or "static_assert"
            // implementations just below)
            ///////////////////////////////////////////////////////
            #define IS_ARG_TYPE_MATCH IsArgTypeSame_v<I, \
                                                      typename FunctionTraitsBaseT::template ArgType_t<I, \
                                                                                                       true /* ReturnVoidIfInvalidIndex */>, \
                                                      true /* CompareWithVoidIfInvalidIndex */ \
                                                     >

    #if defined(USE_CONCEPTS)
            #define IS_ARG_TYPE_MATCH_V_IMPL IS_ARG_TYPE_MATCH
    #else
        ////////////////////////////////////////////////////////
        // "IsArgTypeMatch_v" (private) implementation. Latter
        // variable declared just below this implementation.
        ////////////////////////////////////////////////////////
        private:
            template <typename FunctionTraitsBaseT,
                      std::size_t I /* Zero-based */,
                      bool ReturnFalseIfOneHasArgOtherDoesnt,
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct IsArgTypeMatchImpl : StaticAssertInvalidFunctionTraitsBase<FunctionTraitsBaseT>
            {
            };

            template <typename FunctionTraitsBaseT,
                      std::size_t I /* Zero-based */,
                      bool ReturnFalseIfOneHasArgOtherDoesnt>
            struct IsArgTypeMatchImpl<FunctionTraitsBaseT,
                                      I /* Zero-based */,
                                      ReturnFalseIfOneHasArgOtherDoesnt,
                                      std::enable_if_t< IsFunctionTraitsBase_v<FunctionTraitsBaseT> >
                                     >
                                     : std::bool_constant<IS_ARG_TYPE_MATCH>
            {
                static_assert(IndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v<FunctionTraitsBaseT,
                                                                                    I,
                                                                                    ReturnFalseIfOneHasArgOtherDoesnt>,
                              "An invalid value for index \"I\" was passed to the \"FunctionTraits\" "
                              "template \"IsArgTypeMatch_v\". \"I\" targets a non-existent arg "
                              "in either or both functions (i.e., \"I\" is greater than or equal "
                              "to the number of args in either or both functions that were passed "
                              "to this template). If both then the call is nonsensical so this "
                              "\"static_assert\" always occurs. If \"I\" targets a non-existent "
                              "arg in one function only however (not both), then this is "
                              "supported if the \"ReturnFalseIfOneHasArgOtherDoesnt\" template "
                              "arg is true (the default), as it will effectively compare a "
                              "non-existent arg to an existing one, always resulting in a false "
                              "return value. If \"ReturnFalseIfOneHasArgOtherDoesnt\" is false "
                              "however then this \"static_assert\" will occur instead. You've "
                              "therefore arrived here either because \"I\" is greater than or "
                              "equal to the number of args in both functions (the arg doesn't "
                              "exist in either function so this \"static_assert\" always occurs "
                              "as noted), or just one function only, but in the latter case the "
                              "\"ReturnFalseIfOneHasArgOtherDoesnt\" template arg is false. "
                              "Pass true in the latter case (the default arg) if you wish to "
                              "suppress this \"static_assert\" and have \"IsArgTypeMatch_v\" "
                              "return false instead (the result of comparing an arg that doesn't "
                              "exist to one that does).");
            };

            #define IS_ARG_TYPE_MATCH_V_IMPL IsArgTypeMatchImpl<FunctionTraitsBaseT, I, ReturnFalseIfOneHasArgOtherDoesnt>::value
        public:
    #endif // #if defined(USE_CONCEPTS)

            template <FUNCTION_TRAITS_BASE_C FunctionTraitsBaseT,
                      std::size_t I, // Zero-based
                      bool ReturnFalseIfOneHasArgOtherDoesnt = true>
            #if defined(USE_CONCEPTS)
                requires IndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v<FunctionTraitsBaseT,
                                                                               I,
                                                                               ReturnFalseIfOneHasArgOtherDoesnt>
            #endif
            static constexpr bool IsArgTypeMatch_v = IS_ARG_TYPE_MATCH_V_IMPL;
            // Done with these (for private use of above template only) ...
            #undef IS_ARG_TYPE_MATCH_V_IMPL
            #undef IS_ARG_TYPE_MATCH

            template <TUPLE_C ArgTypesToCompareTupleT,
                      AndVariadic AndVariadicT = AndVariadic::IsFalse>
            static constexpr bool IsArgTypesSameTuple_v = STATIC_ASSERT_V(STATIC_ASSERT_IS_TUPLE_F(ArgTypesToCompareTupleT),
                                                                          (std::is_same_v<ArgTypes_t,
                                                                                          std::remove_cv_t<ArgTypesToCompareTupleT>
                                                                                         > &&
                                                                           (AndVariadicT == AndVariadic::Ignore ||
                                                                            static_cast<AndVariadic>(IsVariadic_v) == AndVariadicT) // "static_cast" here is dirty but convenient
                                                                                                                                     // so we'll live with it (it's safe)
                                                                          )
                                                                         );

            // Defers to "IsArgTypesSameTuple_v" just above ...
            template <AndVariadic AndVariadicT,
                      typename... ArgTypesToCompareT>
            static constexpr bool IsArgTypesSame_v = IsArgTypesSameTuple_v<std::tuple<ArgTypesToCompareT...>,
                                                                           AndVariadicT>;

            ////////////////////////////////////////////////////////
            // "IsArgTypesMatch_v" implementation declared further
            // below (following #defined here so we can apply this
            // line in both the concepts or "static_assert"
            // implementations just below)
            ////////////////////////////////////////////////////////
            #define IS_ARG_TYPES_MATCH IsArgTypesSameTuple_v<typename FunctionTraitsBaseT::ArgTypes_t, \
                                                             IgnoreVariadic ? AndVariadic::Ignore \
                                                                            : static_cast<AndVariadic>(FunctionTraitsBaseT::IsVariadic_v)> // "static_cast" here is dirty but convenient
                                                                                                                                         // so we'll live with it (it's safe)
    #if defined(USE_CONCEPTS)
            #define IS_ARG_TYPES_MATCH_V_IMPL IS_ARG_TYPES_MATCH
    #else
        /////////////////////////////////////////////////////////
        // "IsArgTypesMatch_v" (private) implementation. Latter
        // variable declared just below this implementation.
        /////////////////////////////////////////////////////////
        private:
            template <typename FunctionTraitsBaseT,
                      bool IgnoreVariadic,
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct IsArgTypesMatchImpl : StaticAssertInvalidFunctionTraitsBase<FunctionTraitsBaseT>
            {
            };

            template <typename FunctionTraitsBaseT,
                      bool IgnoreVariadic>
            struct IsArgTypesMatchImpl<FunctionTraitsBaseT,
                                       IgnoreVariadic,
                                       std::enable_if_t< IsFunctionTraitsBase_v<FunctionTraitsBaseT> >
                                      >
                                      : std::bool_constant<IS_ARG_TYPES_MATCH>
            {
            };

            #define IS_ARG_TYPES_MATCH_V_IMPL IsArgTypesMatchImpl<FunctionTraitsBaseT, IgnoreVariadic>::value
        public:
    #endif // #if defined(USE_CONCEPTS)

            template <FUNCTION_TRAITS_BASE_C FunctionTraitsBaseT,
                      bool IgnoreVariadic = false>
            // Done with this just above (for internal use only)
            #undef FUNCTION_TRAITS_BASE_C
            static constexpr bool IsArgTypesMatch_v = IS_ARG_TYPES_MATCH_V_IMPL;
            // Done with these (for private use of above template only) ...
            #undef IS_ARG_TYPES_MATCH_V_IMPL
            #undef IS_ARG_TYPES_MATCH

            template <FOR_EACH_ARG_FUNCTOR_C ForEachArgFunctorT>
            static constexpr bool ForEachArg(ForEachArgFunctorT&& forEachArgFunctor)
            {
                //////////////////////////////////////////////////////////
                // Defer to function template "ForEachTupleType()",
                // specializing it on our "ArgTypes_t" member (i.e., the
                // the "std::tuple" containing all non-variadic arg types
                // for the function). All types in "ArgTypes_t" are
                // therefore iterated, invoking "forEachArgFunctor" for
                // each (we pass the latter to "ForEachTupleType()").
                //////////////////////////////////////////////////////////
                return ForEachTupleType<ArgTypes_t>(std::forward<ForEachArgFunctorT>(forEachArgFunctor));
            }

            ///////////////////////////////////////////////////////////////////
            // Note: At this writing "FunctionClassification::Abominable" can
            // never occur by design but we check for it here anyway (since
            // it's cleaner to do so). Abominable functions are handled by our
            // "AbominableFunctionTraits" derivative later in the file however
            // which ultimately derives from the struct you're now reading and
            // provides an override for the following variable (setting it to
            // "FunctionClassification::Abominable"). The following code will
            // therefore never directly encounter an abominable function at
            // this writing and we "static_assert" on it just below.
            ///////////////////////////////////////////////////////////////////
            static constexpr FunctionClassification FunctionClassification_v = std::is_void_v<MemberFunctionClass_t> ? (!IsConst_v && !IsVolatile_v && FunctionReference_v == FunctionReference::None ? FunctionClassification::Free
                                                                                                                                                                                                      : FunctionClassification::Abominable)
                                                                                                                     : FunctionClassification::NonStaticMember;
            // See comments just above
            static_assert(FunctionClassification_v != FunctionClassification::Abominable);

            //////////////////////////////////////////////////////////
            // Overridden (hidden) where required in derived classes
            // such as "FunctorTraits" (which sets this to
            // "FunctionOrigin::Functor"). At this point however it's
            // always "FunctionOrigin::None" (unless overridden in a
            // derived class as just described).
            //////////////////////////////////////////////////////////
            static constexpr FunctionOrigin FunctionOrigin_v = FunctionOrigin::None;

            /////////////////////////////////////////////////////////
            // Default constructor. Deleted to prevent struct (and
            // its derivatives) from being constructed since it's a
            // traits class so no instance of the class is normally
            // ever required. Deleting it here prevents accidental
            // creation (including derivatives) and to some extent
            // helps enforce the struct's intent (as a traits class
            // so there's no reason to instantiate it normally and
            // the following even prevents accidental instantiation
            // - the struct's entire interface consists of "using"
            // statements and "static constexpr" declarations only
            // so no instance is therefore normally required). Some
            // might argue that it's not a good idea however (for
            // one reason or another) but for now we'll live with
            // it (if an instance needs to be created it can be
            // done via a call to "std::declval" but that's all).
            /////////////////////////////////////////////////////////
            FunctionTraitsBase() = delete;
        }; // struct FunctionTraitsBase

        //////////////////////////////////////////////////////////////////
        // See this constant for details. Following code is preprocessed
        // out if function write traits not supported (since code is only
        // required to support function write traits at this writing,
        // never function read traits). Function write traits are
        // normally supported however so the following normally tests
        // true (the only exception at this writing is Microsoft Visual
        // C++ from Visual Studio 2017, which has a bug that prevents us
        // from including this code)
        //////////////////////////////////////////////////////////////////
        #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
            //////////////////////////////////////////////////////////////////
            // FunctionWriteTraitsHelper. Base class of "FunctionTraitsBase"
            // which all "FunctionTraits" derivatives (specializations)
            // ultimately inherit from. The following class is for internal
            // use only so it contains no public members. Contains helper
            // aliases that "FunctionTraits" ultimately relies on for its
            // implementation (specifically for handling function write
            // traits - function read traits never rely on this class).
            //////////////////////////////////////////////////////////////////
            class FunctionWriteTraitsHelper
            {
            private:
                template <typename F1,
                          typename F2>
                using MigrateConst = std::conditional_t<std::is_const_v<F1>,
                                                        std::add_const_t<F2>,
                                                        F2
                                                       >;

                template <typename F1,
                            typename F2>
                using MigrateVolatile = std::conditional_t<std::is_volatile_v<F1>,
                                                           std::add_volatile_t<F2>,
                                                           F2
                                                          >;

            protected:
                template <typename F1,
                            typename F2>
                using MigrateCv = MigrateVolatile<F1, MigrateConst<F1, F2>>;

                template <typename F1,
                            typename F2>
                using MigrateRef = std::conditional_t< std::is_lvalue_reference_v<F1>,
                                                       std::add_lvalue_reference_t<F2>,
                                                       std::conditional_t<std::is_rvalue_reference_v<F1>,
                                                                          std::add_rvalue_reference_t<F2>,
                                                                          F2
                                                                         >
                                                     >;
            private:
                ////////////////////////////////////////////////////////////////////////////////
                // ReplaceArgsTupleImpl (primary template). Implements the "ReplaceArgsTuple_t"
                // alias seen in all "FunctionTraits" derivatives (specializations). That is,
                // "ReplaceArgsTuple_t" which is publicly declared in
                // "StdExt::Private::FreeFunctionTraits" and
                // "StdExt::Private::MemberFunctionTraits" ("FunctionTraits" always ultimately
                // derives from either), defers to the struct you're now reading to carry out
                // its work. The following primary template never kicks in however unless
                // template arg "NewArgsTupleT" isn't a "std::tuple" which is only possible in
                // C++17 (for C++20 and later, our "StdExt::ValidFunctionArgTypesTuple_c"
                // concept kicks in instead, before we ever arrive here, so the primary
                // template *never* kicks in). The partial specialization just below kicks in
                // when "NewArgsTupleT" *is* a "std::tuple" which will normally be the case
                // (unless an invalid type was passed for "NewArgsTupleT").
                //
                // Given the following example for instance (end-user code):
                //
                //     using F = int (Whatever::* const volatile &&)(float, double)>;
                //     using NewArgsTupleT = std::tuple<char>;
                //     using FuncWithNewArgs = ReplaceArgsTuple_t<F, NewArgsTupleT>;
                //
                // The call to "ReplaceArgsTuple_t" just above (the helper template most
                // users will rely on), ultimately defers to the following struct with the
                // following template args (and because template arg "T" is always a
                // "std::tuple", the partial specialization just below will always kick in
                // to handle it):
                //
                //     ReplaceArgsT = StdExt::Private::MemberFunctionTraits<int (Whatever::* const volatile &&)(float, double),
                //                                                          int (Whatever::*)(float, double),
                //                                                          void
                //                                                         >::ReplaceArgs_t,
                //     T = std::tuple<char>
                //
                // The partial specialization below simply invokes "ReplaceArgsT" seen just
                // above, passing it the types that tuple "T" is specialized on.
                ////////////////////////////////////////////////////////////////////////////////
                template <template<typename... NewArgsT> class ReplaceArgsTemplate,
                          VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT
                         >
                struct ReplaceArgsTupleImpl
            #if defined(USE_CONCEPTS)
                ////////////////////////////////////////////////////////
                // Concepts in effect so no need to define the primary
                // template, only declare it (since the partial
                // specialization of this template further below always
                // kicks in by design when concepts are in effect).
                // Following semicolon therefore facilitates this (it
                // terminates the primary template but we've provided
                // no definition for it - ok).
                ////////////////////////////////////////////////////////
                ;
            #else
                {
                    /////////////////////////////////////////////////////////
                    // Always triggers at this point! (since specialization
                    // just below didn't kick in). Will only come through
                    // here in C++17 normally (in C++20 or later we rely on
                    // concepts so will never arrive here unless
                    // USE_CONCEPTS is explicitly #undefined - in C++20 or
                    // later it's #defined by default)
                    /////////////////////////////////////////////////////////
                    static_assert(AlwaysFalse_v<NewArgsTupleT>,
                                  STATIC_ASSERT_IS_TUPLE_MSG(NewArgsTupleT));

                    /////////////////////////////////////////////////////////
                    // Above "static_assert" always triggers so the
                    // following "Type" can be omitted. If omitted however
                    // then our call to access this type further below might
                    // result in an "undeclared type" error of some kind
                    // (since the type hasn't been declared). This will only
                    // lead to confusion since the above "static_assert" is
                    // the actual source of the problem (so it's the only
                    // error message users need to see). We therefore
                    // declare the following to eliminate any extraneous
                    // "undeclared type" error that might occur if it
                    // weren't declared (calling "ReplaceArgsTemplate" here
                    // with no template args which is fine for our purposes,
                    // it goes unused anyway due to the "static_assert").
                    /////////////////////////////////////////////////////////
                    using Type = ReplaceArgsTemplate<>;
                };
            #endif // #if defined(USE_CONCEPTS)

                ///////////////////////////////////////////////////////////////////
                // ReplaceArgsTupleImpl (partial specialization when 2nd arg is a
                // "std::tuple" - it always is by design - see primary template
                // just above for details)
                ///////////////////////////////////////////////////////////////////
                template <template<typename...> class ReplaceArgsTemplate,
                          VALID_FUNCTION_ARG_TYPE_C... NewArgsT>
                struct ReplaceArgsTupleImpl<ReplaceArgsTemplate,
                                            std::tuple<NewArgsT...>
                                           >
                {
                    using Type = ReplaceArgsTemplate<NewArgsT...>;
                };

            protected:
                ///////////////////////////////////////////////////////////
                // Helper alias for "ReplaceArgsTupleImpl" template above.
                // Implements the public "ReplaceArgsTuple_t" alias seen
                // in all "FunctionTraits" derivatives (specializations).
                // Latter alias just defers to this one which defers to
                // "ReplaceArgsTupleImpl" as seen. See latter template
                // above for details.
                ///////////////////////////////////////////////////////////
                template <template<typename... NewArgsT> class ReplaceArgsTemplate,
                          VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT>
                using ReplaceArgsTupleImpl_t = typename ReplaceArgsTupleImpl<ReplaceArgsTemplate,
                                                                             std::remove_cv_t<NewArgsTupleT>>::Type;
            }; // class FunctionWriteTraitsHelper

            #if defined(USE_CONCEPTS)
                // For internal use only (we #undef it later)
                #define DECLARE_REPLACE_RETURN_TYPE_IMPL(...)
                #define REPLACE_RETURN_TYPE_IMPL(REPLACE_RETURN_TYPE_MACRO) REPLACE_RETURN_TYPE_MACRO
            #else
                template <typename NewReturnTypeT>
                struct StaticAssertInvalidReturnType
                {
                    // Always triggers at this point!
                    static_assert(AlwaysFalse_v<NewReturnTypeT>,
                                  STATIC_ASSERT_IS_VALID_RETURN_TYPE_MSG(NewReturnTypeT));
                };

                // For internal use only (we #undef it later)
                #define DECLARE_REPLACE_RETURN_TYPE_IMPL(REPLACE_RETURN_TYPE_MACRO) \
                    private: \
                        template <typename NewReturnTypeT, \
                                  typename = void> \
                        struct ReplaceReturnTypeImpl : StaticAssertInvalidReturnType<NewReturnTypeT> \
                        { \
                            /*/////////////////////////////////////////////////////// \
                            // "static_assert" always triggers at this point so the   \
                            // following "Type" can be omitted. If omitted however    \
                            // then our call to access this type might result in an   \
                            // "undeclared type" error of some kind (since the type   \
                            // hasn't been declared). This will only lead to          \
                            // confusion since the "static_assert" is the actual      \
                            // source of the problem (so it's the only error message  \
                            // users need to see). We therefore declare the following \
                            // to eliminate any extraneous "undeclared type" error    \
                            // that might occur if it weren't declared (just          \
                            // assigning "SpecializedF" here which is declared in the \
                            // "FreeFunctionTraits" or "MemberFunctionTraits" class   \
                            // that all "FunctionTraits" specializations ultimately   \
                            // derive from, though not actually relevant here due to  \
                            // the "static_assert" that triggers, i.e., "Type" below  \
                            // goes unused).                                          \
                            ///////////////////////////////////////////////////////*/ \
                            using Type = SpecializedF; \
                        }; \
\
                        template <typename NewReturnTypeT> \
                        struct ReplaceReturnTypeImpl<NewReturnTypeT, \
                                                     std::enable_if_t< IsValidReturnType_v<NewReturnTypeT> > \
                                                    > \
                        { \
                            using Type = REPLACE_RETURN_TYPE_MACRO; \
                        }; \
                    public:

                // For internal use only (we #undef it later)
                #define REPLACE_RETURN_TYPE_IMPL(...) typename ReplaceReturnTypeImpl<NewReturnTypeT>::Type
            #endif // #if defined(USE_CONCEPTS)

            /////////////////////////////////////////////////
            // Is "vectorcall" calling convention supported
            // by this compiler?
            /////////////////////////////////////////////////
            #if defined(STDEXT_CC_VECTORCALL)
                // For internal use only (we #undef it later).
                #define STDEXT_CC_VECTORCALL_REPLACE(CC) STDEXT_CC_VECTORCALL
            #else
                // For internal use only (we #undef it later).
                #define STDEXT_CC_VECTORCALL_REPLACE(CC) CC
            #endif

            //////////////////////////////////////////////
            // Is "regcall" calling convention supported
            // by this compiler?
            //////////////////////////////////////////////
            #if defined(STDEXT_CC_REGCALL)
                // For internal use only (we #undef it later).
                #define STDEXT_CC_REGCALL_REPLACE(CC) STDEXT_CC_REGCALL
            #else
                // For internal use only (we #undef it later).
                #define STDEXT_CC_REGCALL_REPLACE(CC) CC
            #endif

            #if defined(USE_CONCEPTS)
                // For internal use only (we #undef it later)
                #define DECLARE_REPLACE_ARGS_IMPL(...)
                #define REPLACE_ARGS_IMPL(REPLACE_ARGS_MACRO) REPLACE_ARGS_MACRO
            #else
                template <typename NewArgsTupleT>
                struct StaticAssertInvalidArgTypesTuple
                {
                    // Always triggers at this point!
                    static_assert(AlwaysFalse_v<NewArgsTupleT>,
                                  STATIC_ASSERT_IS_VALID_FUNCTION_ARG_TYPES_MSG(NewArgsTupleT));
                };

                // For internal use only (we #undef it later)
                #define DECLARE_REPLACE_ARGS_IMPL(REPLACE_ARGS_MACRO) \
                    private: \
                        template <typename NewArgsTupleT, \
                                  typename = void> \
                        struct ReplaceArgsImpl : StaticAssertInvalidArgTypesTuple<NewArgsTupleT> \
                        { \
                            /*/////////////////////////////////////////////////////// \
                            // "static_assert" always triggers at this point so the   \
                            // following "Type" can be omitted. If omitted however    \
                            // then our call to access this type might result in an   \
                            // "undeclared type" error of some kind (since the type   \
                            // hasn't been declared). This will only lead to          \
                            // confusion since the "static_assert" is the actual      \
                            // source of the problem (so it's the only error message  \
                            // users need to see). We therefore declare the following \
                            // to eliminate any extraneous "undeclared type" error    \
                            // that might occur if it weren't declared (just          \
                            // assigning "SpecializedF" here which is declared in the \
                            // "FreeFunctionTraits" or "MemberFunctionTraits" class   \
                            // that all "FunctionTraits" specializations ultimately   \
                            // derive from, though not actually relevant here due to  \
                            // the "static_assert" that triggers, i.e., "Type" below  \
                            // goes unused).                                          \
                            ///////////////////////////////////////////////////////*/ \
                            using Type = SpecializedF; \
                        }; \
\
                        template <typename... NewArgsT> \
                        struct ReplaceArgsImpl<std::tuple<NewArgsT...>, \
                                               std::enable_if_t< IsValidFunctionArgTypes_v<NewArgsT...> > \
                                              > \
                        { \
                            using Type = REPLACE_ARGS_MACRO; \
                        }; \
                    public:

                // For internal use only (we #undef it later)
                #define REPLACE_ARGS_IMPL(...) typename ReplaceArgsImpl< std::tuple<NewArgsT...> >::Type
            #endif // #if defined(USE_CONCEPTS)
        #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)

        template <TRAITS_FREE_FUNCTION_C F,
                  FREE_FUNCTION_C FreeFunctionT = RemovePtrRef_t<F>, // For "std::enable_if_t" purposes in our specializations. Just "F" above after
                                                                     // removing any reference from "F" if present and then any pointer if present.
                                                                     // "F" itself may be a raw function or a pointer to a function or a reference
                                                                     // to a function or a reference to a pointer to a function. "FreeFunctionT" is
                                                                     // therefore always just the resulting raw function type of "F" (i.e., not a
                                                                     // pointer to a function or a reference to a function or a reference to a
                                                                     // pointer to a function but the actual raw function type). It's also never an
                                                                     // "abominable" function type however since "F" is never passed one by design
                                                                     // (note that the FREE_FUNCTION_C concepts rejects them). We don't support them
                                                                     // since abominable functions (those with cv-qualifiers and/or ref-qualifiers)
                                                                     // are always (ultimately) processed as member function pointers by design so
                                                                     // they never come through here.
                  typename = void> // Arg for internal use only (users should never explicitly pass it)
        struct FreeFunctionTraits
        {
            // See this constant for details
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concepts kick in in the template
                // declaration above instead (macros for these
                // above resolve to the "typename" keyword when
                // concepts aren't supported and the following
                // "static_asserts" are then used instead)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_TRAITS_FREE_FUNCTION(F)
                STATIC_ASSERT_IS_FREE_FUNCTION(FreeFunctionT)
            #endif

            /////////////////////////////////////////////////////////////
            // By design (mandatory and always the case when relying on
            // the default arg for "FreeFunctionT" which we always do)
            /////////////////////////////////////////////////////////////
            static_assert(std::is_same_v<RemovePtrRef_t<F>, FreeFunctionT>);

            static_assert(AlwaysFalse_v<F>,
                          "No expected partial specialization exists for \"FreeFunctionTraits\", "
                          "usually because its \"FreeFunctionT\" template arg has an unsupported "
                          "calling convention (though other rare reasons may be possible). Note "
                          "that \"FreeFunctionT\" is just template arg \"F\" stripped down to the "
                          "free function type it refers to (the result of removing any reference "
                          "from \"F\" if present, and then any function pointer if present). \"F\" "
                          "itself is therefore an unsupported type for use with this template, and "
                          "more generally for \"FunctionTraits\" or any of its helper templates.");
        }; // struct FreeFunctionTraits

        //////////////////////////////////////////////////////////////////
        // See this constant for details. Following code is preprocessed
        // out if function write traits not supported (since code is only
        // required to support function write traits at this writing,
        // never function read traits). Function write traits are
        // normally supported however so the following normally tests
        // true (the only exception at this writing is Microsoft Visual
        // C++ from Visual Studio 2017, which has a bug that prevents us
        // from including this code)
        //////////////////////////////////////////////////////////////////
        #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
            /////////////////////////////////////////////////////////////
            // FreeOrAbominableFunctionWriteTraitsHelper. Base class of
            // "FreeFunctionTraitsBase" which all "FreeFunctionTraits"
            // derivatives (specializations) ultimately inherit from.
            // The following class is for internal use only so it
            // contains no public members. Contains helper aliases that
            // "FreeFunctionTraits" ultimately relies on for its
            // implementation (specifically for handling function write
            // traits - function read traits never rely on this class).
            /////////////////////////////////////////////////////////////
            class FreeOrAbominableFunctionWriteTraitsHelper : public FunctionWriteTraitsHelper
            {
            private:
                // Primary template (kicks in when "NewClassT" is void)
                template <IS_CLASS_OR_VOID_C NewClassT,
                          TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION_C F,
                          IS_FUNCTION_C RawTypeF,
                          typename = void> // Arg for internal use only (users should never explicitly pass it)
                struct ReplaceMemberFunctionClassImpl
                {
                    //////////////////////////////////////////////////
                    // Kicks in if concepts not supported, otherwise
                    // corresponding concepts kick in in the template
                    // declaration above instead (macros for these
                    // above resolve to the "typename" keyword when
                    // concepts aren't supported and the following
                    // "static_asserts" are then used instead)
                    //////////////////////////////////////////////////
                    #if !defined(USE_CONCEPTS)
                        STATIC_ASSERT_IS_CLASS_OR_VOID(NewClassT)
                        STATIC_ASSERT_IS_TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION(F)
                        STATIC_ASSERT_IS_FUNCTION(RawTypeF)
                    #endif

                    using Type = F;
                };

                ///////////////////////////////////////////////////
                // Specialization of above template (kicks in when
                // "NewClassT" is a class or struct)
                ///////////////////////////////////////////////////
                template <typename NewClassT,
                          TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION_C F,
                          IS_FUNCTION_C RawTypeF>
                struct ReplaceMemberFunctionClassImpl<NewClassT,
                                                      F,
                                                      RawTypeF,
                                                      std::enable_if_t< std::is_class_v<NewClassT> >
                                                     >
                {
                    //////////////////////////////////////////////////
                    // Kicks in if concepts not supported, otherwise
                    // corresponding concepts kick in in the template
                    // declaration above instead (macros for these
                    // above resolve to the "typename" keyword when
                    // concepts aren't supported and the following
                    // "static_asserts" are then used instead)
                    //////////////////////////////////////////////////
                    #if !defined(USE_CONCEPTS)
                        STATIC_ASSERT_IS_TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION(F)
                        #undef STATIC_ASSERT_IS_TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION  // Done with this just above (for internal use only)
                        STATIC_ASSERT_IS_FUNCTION(RawTypeF)
                    #endif

                    ////////////////////////////////////////////
                    // Note: "NewClassT" may be "const" and/or
                    // "volatile" but it's effectively ignored
                    // (doesn't appear in the resulting type
                    // so harmless)
                    /////////////////////////////////////////////
                    using Type = RawTypeF NewClassT::*;
                };

            protected:
                template <IS_CLASS_OR_VOID_C NewClassT,
                          TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION_C F,
                          IS_FUNCTION_C RawTypeF>
                #undef TRAITS_FREE_FUNCTION_OR_ABOMINABLE_FUNCTION_C // Done with this just above (for internal use only)
                using ReplaceMemberFunctionClassImpl_t = typename ReplaceMemberFunctionClassImpl<NewClassT, F, RawTypeF>::Type;
            }; // class FreeOrAbominableFunctionWriteTraitsHelper

            /////////////////////////////////////////////////////////////////////////
            // FreeFunctionWriteTraitsHelper. Base class of "FreeFunctionTraitsBase"
            // which all "FreeFunctionTraits" derivatives (specializations)
            // ultimately inherit from. The following class is for internal use only
            // so it contains no public members. Contains helper aliases that
            // "FreeFunctionTraits" ultimately relies on for its implementation
            // (specifically for handling function write traits - function read
            // traits never rely on this class).
            /////////////////////////////////////////////////////////////////////////
            class FreeFunctionWriteTraitsHelper : public FreeOrAbominableFunctionWriteTraitsHelper
            {
                using BaseClass = FreeOrAbominableFunctionWriteTraitsHelper;

            protected:
                ////////////////////////////////////////////////
                // Migrate (copy) the optional pointer and/or
                // optional reference from "F" to "NewF". "F"
                // is always one of the following:
                //
                // 1) Raw free function type, e.g., "void ()",
                //    so never an abominable function since
                //    it's free
                // 2) Reference to 1 above, e.g., "void (&)()"
                // 3) Optionally cv-qualified pointer to 1
                //    above, e.g., "void (* const volatile)()"
                // 4) Reference to 3 above, e.g.,
                //    "void (* const volatile &)()"
                //
                // "NewF" itself is always a raw free function
                // type so never 2, 3 or 4 above, the reason we
                // call this template, to make it conform to 2,
                // 3 or 4 above in order to match "F" itself
                // (by copying the reference in 2 above or
                // pointer in 3 above or reference to pointer
                // in 4 above from "F" to "NewF", assuming
                // "F" is in fact 2, 3 or 4 above, otherwise
                // there's nothing to copy in which case this
                // template just returns "NewF" unaltered)
                //
                // E.g., Copy these to "NewF" (case 4 above)
                //                 | |     |        |
                //                 V V     V        V
                // using F = void (* const volatile &)();
                // using NewF = double (int);
                //
                // //////////////////////////////////////////////
                // // Yields:
                // //
                // //   double (* const volatile &)(int)
                // //////////////////////////////////////////////
                // using NewFWithPtrAndRef = MigratePointerAndRef<F, NewF>;
                ////////////////////////////////////////////////
                template <TRAITS_FREE_FUNCTION_C F,
                          FREE_FUNCTION_C NewF>
                using MigratePointerAndRef = BaseClass::MigrateRef< F,
                                                                    std::conditional_t< std::is_pointer_v< std::remove_reference_t<F> >, // Is "F" a function pointer or reference to one?
                                                                                                                                         // (if not then it's a function or reference to one)
                                                                                        BaseClass::MigrateCv< std::remove_reference_t<F>,
                                                                                                              std::add_pointer_t<NewF>
                                                                                                            >,
                                                                                        NewF
                                                                                      >
                                                                  >;
            }; // class FreeFunctionWriteTraitsHelper

            // For internal use only (we #undef it later)
            #define FREE_FUNCTION_REPLACE_RETURN_TYPE(CC, ELLIPSIS, IS_NOEXCEPT) NewReturnTypeT CC (ArgsT... ELLIPSIS) noexcept(IS_NOEXCEPT)

            // For internal use only (we #undef it later)
            #define FREE_FUNCTION_REPLACE_ARGS(CC, ELLIPSIS, IS_NOEXCEPT) R CC (NewArgsT... ELLIPSIS) noexcept(IS_NOEXCEPT)

            //////////////////////////////////////////////////////////////////////
            // For internal use only (we #undef it later). Used to implement
            // "FreeFunctionTraits::ReplaceCallingConvention_t" for non-variadic
            // free functions. Note that the (zero-based) 4th entry in the tuple
            // we're creating below is for handling STDEXT_CC_THISCALL. It's not
            // supported for free functions (it's for non-static member functions
            // only) so if someone calls
            // "FreeFunctionTraits::ReplaceCallingConvention_t" and passes
            // "CallingConvention::Thiscall" for its template arg, the
            // (zero-based) 4th entry below will kick in to handle it, thus
            // ensuring the calling convention remains unchanged ("CC" in the
            // (zero-based) 4th entry below will be applied to the function the
            // user is targeting so it remains unchanged).
            //////////////////////////////////////////////////////////////////////
            #define FREE_FUNCTION_REPLACE_CALLING_CONVENTION(CC, IS_NOEXCEPT) \
                MigratePointerAndRef_t< std::tuple_element_t<static_cast<std::size_t>(NewCallingConventionT), \
                                                             std::tuple<R STDEXT_CC_CDECL (ArgsT...) noexcept(IS_NOEXCEPT), \
                                                                        R STDEXT_CC_STDCALL (ArgsT...) noexcept(IS_NOEXCEPT), \
                                                                        R STDEXT_CC_FASTCALL (ArgsT...) noexcept(IS_NOEXCEPT), \
                                                                        R STDEXT_CC_VECTORCALL_REPLACE(CC) (ArgsT...) noexcept(IS_NOEXCEPT), \
                                                                        R CC (ArgsT...) noexcept(IS_NOEXCEPT), /* STDEXT_CC_THISCALL (not applicable to free functions) */ \
                                                                        R STDEXT_CC_REGCALL_REPLACE(CC) (ArgsT...) noexcept(IS_NOEXCEPT) \
                                                                       > \
                                                            > \
                                      >
            ////////////////////////////////////////////////////////
            // For internal use only (we #undef it later). Adds
            // function write traits to "struct FreeFunctionTraits"
            // below.
            ////////////////////////////////////////////////////////
            #define DECLARE_FREE_FUNCTION_WRITE_TRAITS(CC, ELLIPSIS, IS_NOEXCEPT) \
                private: \
                    template <FREE_FUNCTION_C NewF> \
                    using MigratePointerAndRef_t = typename BaseClass::template MigratePointerAndRef<F, NewF>; \
\
                public: \
                    /*/////////////////////////////////////////////////// \
                    // Private implementation for "ReplaceReturnType_t" / \
                    // just below (applicable to C++17 only - resolves  / \
                    // to whitespace in C++20 or later)                 / \
                    ///////////////////////////////////////////////////*/ \
                    DECLARE_REPLACE_RETURN_TYPE_IMPL(FREE_FUNCTION_REPLACE_RETURN_TYPE(CC, ELLIPSIS, IS_NOEXCEPT)) \
\
                    template <VALID_RETURN_TYPE_C NewReturnTypeT> \
                    using ReplaceReturnType_t = MigratePointerAndRef_t< REPLACE_RETURN_TYPE_IMPL(FREE_FUNCTION_REPLACE_RETURN_TYPE(CC, ELLIPSIS, IS_NOEXCEPT)) >; \
\
                    template <StdExt::CallingConvention NewCallingConventionT> \
                    using ReplaceCallingConvention_t = FREE_FUNCTION_REPLACE_CALLING_CONVENTION(CC, IS_NOEXCEPT); \
\
                    template <IS_CLASS_OR_VOID_C NewClassT> \
                    using ReplaceMemberFunctionClass_t = typename BaseClass::template ReplaceMemberFunctionClassImpl_t<NewClassT, F, typename BaseClass::RawType_t>; \
\
                    /*/////////////////////////////////////////////////// \
                    // Private implementation for "ReplaceArgs_t" just  / \
                    // below (applicable to C++17 only - resolves to    / \
                    // whitespace in C++20 or later)                    / \
                    ///////////////////////////////////////////////////*/ \
                    DECLARE_REPLACE_ARGS_IMPL(FREE_FUNCTION_REPLACE_ARGS(CC, ELLIPSIS, IS_NOEXCEPT)) \
\
                    template <VALID_FUNCTION_ARG_TYPE_C... NewArgsT> \
                    using ReplaceArgs_t = MigratePointerAndRef_t< REPLACE_ARGS_IMPL(FREE_FUNCTION_REPLACE_ARGS(CC, ELLIPSIS, IS_NOEXCEPT)) >; \
\
                    template <VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT> \
                    /* Ultimately defers to template just above (we're passing it as the 1st arg here) */ \
                    using ReplaceArgsTuple_t = typename BaseClass::template ReplaceArgsTupleImpl_t<ReplaceArgs_t, NewArgsTupleT>; \
\
                    template<std::size_t I /* Zero-based */, std::size_t Count, VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    /* Defers to template just above, passing the modified args (as a "std::tuple") which replaces the current args */ \
                    using ArgsModifyTuple_t = ReplaceArgsTuple_t< TupleModifyTuple_t<typename BaseClass::ArgTypes_t, I, Count, NewArgsTupleT> >; \
\
                    template<std::size_t I /* Zero-based */, std::size_t Count, VALID_FUNCTION_ARG_TYPE_C... NewArgsT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    /* Defers to template just above */ \
                    using ArgsModify_t = ArgsModifyTuple_t<I, Count, std::tuple<NewArgsT...>>; \
\
                    template <std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPE_C NewArgT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_ARG_COUNT(I) \
                    /* Defers to template just above */ \
                    using ReplaceArg_t = STATIC_ASSERT_T(STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE_F(typename BaseClass::ArgTypes_t, I), \
                                                         (ArgsModify_t<I, 1, NewArgT>)); \
\
                    template<std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToInsertTupleT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    using ArgsInsertTuple_t = ReplaceArgsTuple_t< TupleInsertTuple_t<typename BaseClass::ArgTypes_t, I, ArgsToInsertTupleT> >; \
\
                    template<std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPE_C... ArgsToInsertT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    /* Defers to template just above */ \
                    using ArgsInsert_t = ArgsInsertTuple_t<I, std::tuple<ArgsToInsertT...> >; \
\
                    template<VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToAppendTupleT> \
                    using ArgsAppendTuple_t = ReplaceArgsTuple_t< TupleAppendTuple_t<typename BaseClass::ArgTypes_t, ArgsToAppendTupleT> >; \
\
                    template<VALID_FUNCTION_ARG_TYPE_C... ArgsToAppendT> \
                    /* Defers to template just above */ \
                    using ArgsAppend_t = ArgsAppendTuple_t< std::tuple<ArgsToAppendT...> >; \
\
                    template<std::size_t I /* Zero-based */ = 0, std::size_t Count = size_t_npos> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    using ArgsDelete_t = ReplaceArgsTuple_t< TupleDelete_t<typename BaseClass::ArgTypes_t, I, Count> >; \
\
                    using ArgsMakeVoid_t = MigratePointerAndRef_t<R CC () noexcept(IS_NOEXCEPT)>; \
\
                    using AddVariadicArgs_t = MigratePointerAndRef_t<R STDEXT_CC_VARIADIC (ArgsT... ...) noexcept(IS_NOEXCEPT)>; \
                    using RemoveVariadicArgs_t = MigratePointerAndRef_t<R CC (ArgsT...) noexcept(IS_NOEXCEPT)>; \
\
                    /*//////////////////////////////////////////////////////////////// \
                    // Note that we don't call "MigratePointerAndRef_t" here. Adding   \
                    // "const" turns it into an abominable function which can't have   \
                    // pointers or references (not legal in C++).                      \
                    ////////////////////////////////////////////////////////////////*/ \
                    using AddConst_t = R CC (ArgsT... ELLIPSIS) const noexcept(IS_NOEXCEPT); \
                    using RemoveConst_t = F; /* Nothing to remove (free functions are never const) */ \
\
                    /*//////////////////////////////////////////////////////////////// \
                    // Note that we don't call "MigratePointerAndRef_t" here. Adding   \
                    // "volatile" turns it into an abominable function which can't     \
                    // have pointers or references (not legal in C++).                 \
                    ////////////////////////////////////////////////////////////////*/ \
                    using AddVolatile_t = R CC (ArgsT... ELLIPSIS) volatile noexcept(IS_NOEXCEPT); \
                    using RemoveVolatile_t = F; /* Nothing to remove (free functions are never volatile) */ \
\
                    /*//////////////////////////////////////////////////////////////// \
                    // Note that we don't call "MigratePointerAndRef_t" here. Adding   \
                    // "const" and "volatile" turns it into an abominable function     \
                    // which can't have pointers or references (not legal in C++).     \
                    ////////////////////////////////////////////////////////////////*/ \
                    using AddCV_t = R CC (ArgsT... ELLIPSIS) const volatile noexcept(IS_NOEXCEPT); \
                    using RemoveCV_t = F; /* Nothing to remove (free functions never have cv-qualifiers) */ \
\
                    /*//////////////////////////////////////////////////////////////// \
                    // Note that we don't call "MigratePointerAndRef_t" here. Adding   \
                    // "&" or "&&: turns it into an abominable function which can't    \
                    // have pointers or references (not legal in C++).                 \
                    ////////////////////////////////////////////////////////////////*/ \
                    using AddLValueReference_t = R CC (ArgsT... ELLIPSIS) & noexcept(IS_NOEXCEPT); \
                    using AddRValueReference_t = R CC (ArgsT... ELLIPSIS) && noexcept(IS_NOEXCEPT); \
                    using RemoveReference_t = F; /* Nothing to remove (free functions never have ref-qualifiers) */ \
\
                    using AddNoexcept_t = MigratePointerAndRef_t<R CC (ArgsT... ELLIPSIS) noexcept>; \
                    using RemoveNoexcept_t = MigratePointerAndRef_t<R CC (ArgsT... ELLIPSIS)>;
        #else
            /////////////////////////////////////////////////
            // For internal use only (we #undef it later).
            // Does nothing however. Function write traits
            // simply won't be added to "FreeFunctionTraits"
            // struct further below (since, when called,
            // the following just resolves to whitespace)
            /////////////////////////////////////////////////
            #define DECLARE_FREE_FUNCTION_WRITE_TRAITS(...)
        #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)

        template <TRAITS_FREE_FUNCTION_C F, // Function's full type
                  FREE_FUNCTION_C RawTypeT, // Function's raw (native) C++ type (raw version of "F" above)
                  typename ReturnTypeT, // Function's return type
                  CallingConvention CallingConventionT, // Function's calling convention (implicitly or explicitly declared in the function)
                  bool IsVariadicT, // "true" if function is variadic (last arg of function is "...") or "false" otherwise
                  bool IsNoexceptT, // "true" if the function is declared "noexcept" or "false" otherwise
                  typename... ArgsT> // Function's arguments (types) in left-to-right order of declaration (as would be expected)
        struct FreeFunctionTraitsBase : public FunctionTraitsBase<F, // Function's full type
                                                                  RawTypeT, // Function's raw (native) C++ type (raw version of "F" above)
                                                                  ReturnTypeT, // Function's return type
                                                                  CallingConventionT, // Function's calling convention (see enum class "CallingConvention")
                                                                                      // implicitly or explicitly declared in the function (enum corresponds
                                                                                      // to our #defined STDEXT_CC_? constants, whichever the function uses)
                                                                  void, // MemberFunctionClassT (we're a free function so not applicable - applies to non-static member functions only)
                                                                  IsVariadicT, // "true" if function is variadic (last arg of function is "...") or "false" otherwise
                                                                  false, // IsConstT (we're a free function so not applicable - applies to non-static member functions or abominable functions only)
                                                                  false, // IsVolatileT (we're a free function so not applicable - applies to non-static member functions or abominable functions only)
                                                                  FunctionReference::None, // FunctionReferenceT (we're a free function so not applicable - applies to non-static member functions or abominable functions only)
                                                                  IsNoexceptT, // "true" if the function is declared "noexcept" or "false" otherwise
                                                                  ArgsT...> // Function's arguments (types) in left-to-right order of declaration (as would be expected)
                                  #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
                                      , public FreeFunctionWriteTraitsHelper
                                  #endif
        {
        };

        ////////////////////////////////////////////////////////////////
        // For internal use only (we #undef it later). Base class for
        // "FreeFunctionTraits" specialization further below. All
        // "FunctionTraits" specializations for free functions
        // ultimately derive from this.
        ////////////////////////////////////////////////////////////////
        #define FREE_FUNCTION_TRAITS_BASE_CLASS(CC, CALLING_CONVENTION, ELLIPSIS, IS_NOEXCEPT) \
            FreeFunctionTraitsBase<F, /* F */ \
                                   R CC (ArgsT... ELLIPSIS) noexcept(IS_NOEXCEPT), /* RawTypeT */ \
                                   R, /* ReturnTypeT */ \
                                   CALLING_CONVENTION, /* CallingConventionT (value from "enum class CallingConvention") */ \
                                   #ELLIPSIS[0] != '\0', /* IsVariadicT (#ELLIPSIS always resolves to "" if function isn't variadic or "..." otherwise) */ \
                                   IS_NOEXCEPT, /* IsNoexceptT (true or false) */ \
                                   ArgsT... /* ArgsT */>

        //////////////////////////////////////////////////////////////////////////////
        // For internal use only (we #undef it later). Creates a "FreeFunctionTraits"
        // partial specialization to handle both free (non-member) functions and
        // static member functions (non-static member functions are handled later).
        // Called multiple times just below for each unique combination of macro
        // parameters (in succession by the numeric suffix at the end of each macro,
        // starting at "1" and ending with this one, "2"). A partial specialization
        // will therefore exist for all permutations of free functions and static
        // member functions (i.e., those with different calling conventions, those
        // with and without "noexcept", and variadic functions). Anytime someone uses
        // "FunctionTraits" to retrieve the traits for either a free (non-member)
        // function or static member function, the appropriate partial specialization
        // will therefore kick in (since "FunctionTraits" derives from
        // "FreeFunctionTraits" in this case). See Raymond Chen's 7 articles (links)
        // posted in the large header block further above (the following is based on
        // his work though I've modified his code for our needs including adding
        // support for non-static member functions further below)
        //////////////////////////////////////////////////////////////////////////////
        #define MAKE_FREE_FUNC_TRAITS_2(CC, CALLING_CONVENTION, ELLIPSIS, IS_NOEXCEPT) \
            template <TRAITS_FREE_FUNCTION_C F, \
                      typename R, \
                      typename... ArgsT> \
            struct FreeFunctionTraits<F, \
                                      R CC (ArgsT... ELLIPSIS) noexcept(IS_NOEXCEPT), \
                                      /*////////////////////////////////////////////////////////////////// \
                                      // The following call to "CallingConventionReplacedWithCdecl"        \
                                      // returns true if the calling convention of this partial            \
                                      // specialization, such as the one for STDEXT_CC_STDCALL, is         \
                                      // replaced with STDEXT_CC_CDECL by the compiler. This only occurs   \
                                      // in 64 bit builds of all our target compilers at this writing      \
                                      // (I'm not aware of any other time it might occur - it never        \
                                      // occurs in 32 bit builds), and takes place for all calling         \
                                      // conventions except STDEXT_CC_VECTORCALL and STDEXT_CC_REGCALL     \
                                      // (on those compilers that support them - they aren't replaced by   \
                                      // STDEXT_CC_CDECL in 64 bit builds). For those that are replaced,   \
                                      // i.e., for those where "CallingConventionReplacedWithCdecl"        \
                                      // returns true, then this "std::enable_if_t" will fail SFINAE       \
                                      // since the condition we're checking for then evaluates to false    \
                                      // (and so this partial specialization will be thrown away). The     \
                                      // partial specialization for STDEXT_CC_CDECL will therefore kick    \
                                      // in to handle things instead. It will handle STDEXT_CC_STDCALL     \
                                      // functions for instance since they're now identical to             \
                                      // STDEXT_CC_CDECL because the compiler changes them to              \
                                      // STDEXT_CC_CDECL. Everything therefore works correctly. Note that  \
                                      // the call below to "AlwaysTrue_v<F>" (which *always* returns true  \
                                      // - that's its purpose), is required simply to ensure the above     \
                                      // behaviour occurs during the 2nd phase of C++ template lookup      \
                                      // (not the 1st phase). Read up on C++'s two-phase template lookup   \
                                      // rules for details. We require evaluation of the above behaviour   \
                                      // in the 2nd phase. Otherwise, if it occurs in the 1st phase and    \
                                      // "CallingConventionReplacedWithCdecl above returns true as         \
                                      // described (so the "std_enable_if_t" condition fails as            \
                                      // described), then a redefinition error of the partial              \
                                      // specialization for STDEXT_CC_STDCALL (or whatever) will occur     \
                                      // since it's now a duplicate of the STDEXT_CC_CDECL partial         \
                                      // specialization (again, because the compiler changes it to         \
                                      // STDEXT_CC_CDECL). Our call to "AlwaysTrue_v<F>" here prevents     \
                                      // that redefinition error by forcing evaluation into the 2nd phase  \
                                      // of template lookup (because the latter call to "AlwaysTrue_v<F>"  \
                                      // is now dependent on a template arg, "F", unlike                   \
                                      // "CallingConventionReplacedWithCdecl" which has no dependencies    \
                                      // on any template args and is therefore evaluated in the 1st        \
                                      // phase). Note that even though the call to                         \
                                      // "CallingConventionReplacedWithCdecl" will still return true       \
                                      // during the 2nd phase, causing SFINAE failure of this              \
                                      // "std_enable_if_t", the partial specialization will simply be      \
                                      // thrown away in favor of the one for STDEXT_CC_CDECl (as           \
                                      // described above). No redefinition error therefore occurs unlike   \
                                      // when the above behavior occurs during the 1st phase (it now       \
                                      // occurs during the 2nd phase instead due to the call to            \
                                      // "AlwaysTrue_v<F>" - problem solved)                               \
                                      //////////////////////////////////////////////////////////////////*/ \
                                      std::enable_if_t<!CallingConventionReplacedWithCdecl<CALLING_CONVENTION, true>() && \
                                                       AlwaysTrue_v<F> \
                                                      > \
                                     > \
                : public FREE_FUNCTION_TRAITS_BASE_CLASS(CC, CALLING_CONVENTION, ELLIPSIS, IS_NOEXCEPT) \
            { \
                static_assert(std::is_same_v<RemovePtrRef_t<F>, R CC (ArgsT... ELLIPSIS) noexcept(IS_NOEXCEPT)>); \
\
            protected: \
                using SpecializedF = R CC (ArgsT... ELLIPSIS) noexcept(IS_NOEXCEPT); /* 2nd template arg */ \
                using BaseClass = FREE_FUNCTION_TRAITS_BASE_CLASS(CC, CALLING_CONVENTION, ELLIPSIS, IS_NOEXCEPT); \
\
                DECLARE_FREE_FUNCTION_WRITE_TRAITS(CC, ELLIPSIS, IS_NOEXCEPT) \
            };

        // noexcept (for internal use only, we #undef it later - invokes macro just above)
        #define MAKE_FREE_FUNC_TRAITS_1(CC, CALLING_CONVENTION, ELLIPSIS) \
            MAKE_FREE_FUNC_TRAITS_2(CC, CALLING_CONVENTION, ELLIPSIS, false) \
            MAKE_FREE_FUNC_TRAITS_2(CC, CALLING_CONVENTION, ELLIPSIS, true)

        ///////////////////////////////////////////////////////////////////
        // For internal use only (we #undef it later). Invokes macro just
        // above. Launching point (first macro to be called) to create
        // partial specializations of "FreeFunctionTraits" for handling
        // every permutation of non-variadic functions for the passed
        // calling convention (variadic functions are handled just after).
        ///////////////////////////////////////////////////////////////////
        #define MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(CC, CALLING_CONVENTION) \
            MAKE_FREE_FUNC_TRAITS_1(CC, CALLING_CONVENTION, )

        ///////////////////////////////////////////////////////////
        // Call above macro once for each calling convention (for
        // internal use only - invokes macro just above).
        ///////////////////////////////////////////////////////////
        MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_CDECL,    CallingConvention::Cdecl)
        MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_STDCALL,  CallingConvention::Stdcall)
        MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_FASTCALL, CallingConvention::Fastcall)
        #if defined(STDEXT_CC_VECTORCALL)
            MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_VECTORCALL, CallingConvention::Vectorcall)
        #endif
        #if defined(STDEXT_CC_REGCALL)
            MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_REGCALL, CallingConvention::Regcall)
        #endif

        // Done with this (for internal use only)
        #undef MAKE_FREE_FUNC_TRAITS_NON_VARIADIC

        ////////////////////////////////////////////////
        // Constants just below not #defined otherwise
        // (applicable only when function write traits
        // supported - usually the case)
        ////////////////////////////////////////////////
        #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
            /////////////////////////////////////////////
            // Done with this (redefining it just below
            // for variadic function specializations)
            /////////////////////////////////////////////
            #undef FREE_FUNCTION_REPLACE_CALLING_CONVENTION

            ////////////////////////////////////////////////////////////////
            // For internal use only (we #undef it later). Used to
            // implement "FreeFunctionTraits::ReplaceCallingConvention_t"
            // for variadic free functions. However, variadic functions
            // must always have the STDEXT_CC_VARIADIC calling convention
            // for all compilers we support at this writing (note that
            // STDEXT_CC_VARIADIC just resolves to STDEXT_CC_CDECL), so the
            // calling convention therefore can't be changed to anything
            // else. Hence the following just resolves to template arg "F"
            // in "FreeFunctionTraits". IOW, if the caller invokes its
            // "ReplaceCallingConvention_t" alias it always returns "F" so
            // the calling convention remains unchanged (always
            // STDEXT_CC_VARIADIC).
            ////////////////////////////////////////////////////////////////
            #define FREE_FUNCTION_REPLACE_CALLING_CONVENTION(...) F
        #endif

        /////////////////////////////////////////////////////////////////
        // For internal use only (we #undef it later). Creates partial
        // specializations of "MemberFunctionTraits" to handle variadic
        // functions (those whose last arg is "..."). Simply launches
        // MAKE_FREE_FUNC_TRAITS_1 as seen which creates partial
        // specializations for handling every permutation of variadic
        // function. These are always assumed to be STDEXT_CC_CDECL in
        // this release (what STDEXT_CC_VARIADIC just below resolves to)
        // since it's the only calling convention that supports variadic
        // functions among all the platforms we support at this writing
        // (but in the general case it's normally the calling convention
        // used for variadic functions on most platforms anyway, though
        // some platforms may have other calling conventions that
        // support variadic functions but it would be rare and we don't
        // currently support them anyway).
        /////////////////////////////////////////////////////////////////
        MAKE_FREE_FUNC_TRAITS_1(STDEXT_CC_VARIADIC, CallingConvention::Variadic, ...)

        // Done with these (for internal use only)
        #undef MAKE_FREE_FUNC_TRAITS_1
        #undef MAKE_FREE_FUNC_TRAITS_2
        #undef FREE_FUNCTION_TRAITS_BASE_CLASS
        #undef DECLARE_FREE_FUNCTION_WRITE_TRAITS

        ///////////////////////////////////////////////////////////////
        // Following only #defined if FUNCTION_WRITE_TRAITS_SUPPORTED
        // is #defined (usually the case)
        ///////////////////////////////////////////////////////////////
        #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
            // Done with these (for internal use only)
            #undef FREE_FUNCTION_REPLACE_CALLING_CONVENTION
            #undef FREE_FUNCTION_REPLACE_ARGS
            #undef FREE_FUNCTION_REPLACE_RETURN_TYPE
        #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)

        template <TRAITS_MEMBER_FUNCTION_C F,
                  MEMBER_FUNCTION_NON_CV_POINTER_C MemberFunctionNonCvPtrT = RemoveCvRef_t<F>, // For "std::enable_if_t" purposes in our specializations. Just
                                                                                               // "F" above after removing any reference from "F" if present,
                                                                                               // and then any cv-qualifiers from the resulting member function
                                                                                               // pointer. "F" itself may be a cv-qualified pointer to a function
                                                                                               // or a reference to such a pointer so "MemberFunctionNonCvPtrT"
                                                                                               // is therefore always a non-cv-qualified pointer to a non-static
                                                                                               // member function.
                  #undef MEMBER_FUNCTION_NON_CV_POINTER_C // Done with this just above (for internal use only)
                  typename = void> // Arg for internal use only (users should never explicitly pass it)
        struct MemberFunctionTraits
        {
            // See this constant for details
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concepts kick in in the template
                // declaration above instead (macros for these
                // above resolve to the "typename" keyword when
                // concepts aren't supported and the following
                // "static_asserts" are then used instead)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_TRAITS_MEMBER_FUNCTION(F)
                STATIC_ASSERT_IS_MEMBER_FUNCTION_NON_CV_POINTER(MemberFunctionNonCvPtrT)

                // Done with this just above (for internal use only)
                #undef STATIC_ASSERT_IS_MEMBER_FUNCTION_NON_CV_POINTER
            #endif

            /////////////////////////////////////////////////////////////
            // By design (mandatory and always the case when relying on
            // the default arg for "MemberFunctionNonCvPtrT" which we
            // always do)
            /////////////////////////////////////////////////////////////
            static_assert(std::is_same_v<RemoveCvRef_t<F>, MemberFunctionNonCvPtrT>);

            static_assert(AlwaysFalse_v<F>,
                          "No expected partial specialization exists for \"MemberFunctionTraits\", "
                          "usually because its \"MemberFunctionNonCvPtrT\" template arg has an "
                          "unsupported calling convention (though other rare reasons may be possible). "
                          "Note that \"MemberFunctionNonCvPtrT\" is just template arg \"F\" stripped "
                          "down to the non-cv-qualified, non-static member function pointer it refers "
                          "to (the result of removing any reference from \"F\" if present, and then "
                          "any cv-qualifiers from the resulting non-static member function pointer). "
                          "\"F\" itself is therefore an unsupported type for use with this template, "
                          "and more generally for \"FunctionTraits\" or any of its helper templates.");
        }; // struct MemberFunctionTraits

        ////////////////////////////////////////////////////////////////////////
        // MemberFunctionRawType. Commented out for now due to a compiler
        // bug in MSVC. See use of this template in "MemberFunctionTraitsBase"
        // for details (commented out for now - relying on an alternate
        // technique instead but following is preferable)
        ////////////////////////////////////////////////////////////////////////
        /*
            template <typename F>
            struct MemberFunctionRawType;

            template <typename T, typename U>
            struct MemberFunctionRawType<U T::*>
            {
                using Type = U;
            };

            template <typename F>
            using MemberFunctionRawType_t = typename MemberFunctionRawType<F>::Type;
        */

        //////////////////////////////////////////////////////////////////
        // See this constant for details. Following code is preprocessed
        // out if function write traits not supported (since code is only
        // required to support function write traits at this writing,
        // never function read traits). Function write traits are
        // normally supported however so the following normally tests
        // true (the only exception at this writing is Microsoft Visual
        // C++ from Visual Studio 2017, which has a bug that prevents us
        // from including this code)
        //////////////////////////////////////////////////////////////////
        #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
            /////////////////////////////////////////////////////////////////////////
            // MemberFunctionWriteTraitsHelper. Base class of
            // "MemberFunctionTraitsBase" which all "MemberFunctionTraits"
            // derivatives (specializations) ultimately inherit from. The following
            // class is for internal use only so it contains no public members.
            // Contains helper aliases that "MemberFunctionTraits" ultimately relies
            // on for its implementation (specifically for handling function write
            // traits - function read traits never rely on this class).
            /////////////////////////////////////////////////////////////////////////
            class MemberFunctionWriteTraitsHelper : public FunctionWriteTraitsHelper
            {
                using BaseClass = FunctionWriteTraitsHelper;

            protected:
                ////////////////////////////////////////////////
                // Migrate (copy) the cv-qualifiers from "F",
                // an optionally cv-qualified non-static member
                // function pointer or reference to one, to
                // "NewF" (if "const" and/or" "volatile" is
                // present in "F"), and then the ref-qualifier
                // from "F" to "NewF" (if & or && is present in
                // "F"). "NewF" itself is always a non-static
                // member function pointer without any
                // qualifiers (by design), the reason we call
                // this template, to copy them from "F" to
                // "NewF.
                //
                // E.g.,
                //                         Copy these to "NewF"
                //                         |     |        |
                //                         V     V        V
                // using F = void (Test::* const volatile &)();
                // using NewF = double (Test::*)(int);
                //
                // //////////////////////////////////////////////
                // // Yields:
                // //
                // //   double (* const volatile &)(int)
                // //////////////////////////////////////////////
                // using NewFWithCVAndRefQualifiers = MigrateCvAndRef<F, NewF>;
                ////////////////////////////////////////////////
                template <TRAITS_MEMBER_FUNCTION_C F, // Optionally cv-qualified non-static member
                                                      // function pointer or reference to one
                          MEMBER_FUNCTION_POINTER_C NewF> // Non-static member function pointer
                                                          // (without any qualifiers by design)
                using MigrateCvAndRef = BaseClass::MigrateRef<F,
                                                                BaseClass::MigrateCv< std::remove_reference_t<F>,
                                                                                      NewF
                                                                                    >
                                                             >;
            }; // class MemberFunctionWriteTraitsHelper

            // For internal use only (we #undef it later)
            #define MEMBER_FUNCTION_REPLACE_RETURN_TYPE(CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT) NewReturnTypeT (CC C::*)(ArgsT... ELLIPSIS) CONST VOLATILE REF noexcept(IS_NOEXCEPT)

            // For internal use only (we #undef it later)
            #define MEMBER_FUNCTION_REPLACE_ARGS(CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT) R (CC C::*)(NewArgsT... ELLIPSIS) CONST VOLATILE REF noexcept(IS_NOEXCEPT)

            //////////////////////////////////////////////////////////////////
            // For internal use only (we #undef it later). Used to implement
            // "MemberFunctionTraits::ReplaceCallingConvention_t" for
            // non-variadic, non-static member functions.
            //////////////////////////////////////////////////////////////////
            #define MEMBER_FUNCTION_REPLACE_CALLING_CONVENTION(CC, CONST, VOLATILE, REF, IS_NOEXCEPT) \
                MigrateCvAndRef_t< std::tuple_element_t<static_cast<std::size_t>(NewCallingConventionT), \
                                                        std::tuple<R (STDEXT_CC_CDECL C::*)(ArgsT...) CONST VOLATILE REF noexcept(IS_NOEXCEPT), \
                                                                   R (STDEXT_CC_STDCALL C::*)(ArgsT...) CONST VOLATILE REF noexcept(IS_NOEXCEPT), \
                                                                   R (STDEXT_CC_FASTCALL C::*)(ArgsT...) CONST VOLATILE REF noexcept(IS_NOEXCEPT), \
                                                                   R (STDEXT_CC_VECTORCALL_REPLACE(CC) C::*)(ArgsT...) CONST VOLATILE REF noexcept(IS_NOEXCEPT), \
                                                                   R (STDEXT_CC_THISCALL C::*)(ArgsT...) CONST VOLATILE REF noexcept(IS_NOEXCEPT), \
                                                                   R (STDEXT_CC_REGCALL_REPLACE(CC) C::*)(ArgsT...) CONST VOLATILE REF noexcept(IS_NOEXCEPT) \
                                                                  > \
                                                       > \
                                 >

            //////////////////////////////////////////////////////////
            // For internal use only (we #undef it later). Adds
            // function write traits to "struct MemberFunctionTraits"
            // below.
            //////////////////////////////////////////////////////////
            #define DECLARE_MEMBER_FUNCTION_WRITE_TRAITS(CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT) \
                private: \
                    template <MEMBER_FUNCTION_POINTER_C NewF> \
                    using MigrateCvAndRef_t = typename BaseClass::template MigrateCvAndRef<F, NewF>; \
\
                    /* Primary template (kicks in when "NewClassT" is void) */ \
                    template <IS_CLASS_OR_VOID_C NewClassT, \
                              typename = void> \
                    struct ReplaceMemberFunctionClassImpl \
                    { \
                        /*//////////////////////////////////////////////// \
                        // Kicks in if concepts not supported, otherwise   \
                        // the following resolves to whitespace and the    \
                        // corresponding concept kicks in in the template  \
                        // declaration above instead (macro for this       \
                        // above resolves to the "typename" keyword when   \
                        // concepts aren't supported and the following     \
                        // "static_assert" is then used instead).          \
                        ////////////////////////////////////////////////*/ \
                        STATIC_ASSERT_IS_CLASS_OR_VOID(NewClassT) \
\
                        using Type = typename BaseClass::RawType_t; \
                    }; \
\
                    /* Specialization of above template (kicks in when "NewClassT" is a class or struct) */ \
                    template <typename NewClassT>  \
                    struct ReplaceMemberFunctionClassImpl<NewClassT, \
                                                          std::enable_if_t< std::is_class_v<NewClassT> > \
                                                         > \
                    { \
                        /*////////////////////////////////////////// \
                        // Note: "NewClassT" may be "const" and/or   \
                        // "volatile" but it's effectively ignored   \
                        // (doesn't appear in the resulting type     \
                        // so harmless)                              \
                        //////////////////////////////////////////*/ \
                        using Type = MigrateCvAndRef_t<R (CC NewClassT::*)(ArgsT... ELLIPSIS) CONST VOLATILE REF noexcept(IS_NOEXCEPT)>; \
                    }; \
\
                    template <IS_CLASS_OR_VOID_C NewClassT>  \
                    using ReplaceMemberFunctionClassImpl_t = typename ReplaceMemberFunctionClassImpl<NewClassT>::Type; \
\
                public: \
                    /*/////////////////////////////////////////////////// \
                    // Private implementation for "ReplaceReturnType_t" / \
                    // just below (applicable to C++17 only - resolves  / \
                    // to whitespace in C++20 or later)                 / \
                    ///////////////////////////////////////////////////*/ \
                    DECLARE_REPLACE_RETURN_TYPE_IMPL(MEMBER_FUNCTION_REPLACE_RETURN_TYPE(CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT)) \
\
                    template <VALID_RETURN_TYPE_C NewReturnTypeT> \
                    using ReplaceReturnType_t = MigrateCvAndRef_t< REPLACE_RETURN_TYPE_IMPL(MEMBER_FUNCTION_REPLACE_RETURN_TYPE(CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT)) >; \
\
                    template <StdExt::CallingConvention NewCallingConventionT> \
                    using ReplaceCallingConvention_t = MEMBER_FUNCTION_REPLACE_CALLING_CONVENTION(CC, CONST, VOLATILE, REF, IS_NOEXCEPT); \
\
                    template <IS_CLASS_OR_VOID_C NewClassT> \
                    using ReplaceMemberFunctionClass_t = ReplaceMemberFunctionClassImpl_t<NewClassT>; \
\
                    /*/////////////////////////////////////////////////// \
                    // Private implementation for "ReplaceArgs_t" just  / \
                    // below (applicable to C++17 only - resolves to    / \
                    // whitespace in C++20 or later)                    / \
                    ///////////////////////////////////////////////////*/ \
                    DECLARE_REPLACE_ARGS_IMPL(MEMBER_FUNCTION_REPLACE_ARGS(CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT)) \
\
                    template <VALID_FUNCTION_ARG_TYPE_C... NewArgsT> \
                    using ReplaceArgs_t = MigrateCvAndRef_t< REPLACE_ARGS_IMPL(MEMBER_FUNCTION_REPLACE_ARGS(CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT))>; \
\
                    template <VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT> \
                    /* Ultimately defers to template just above (we're passing it as the 1st arg here) */ \
                    using ReplaceArgsTuple_t = typename BaseClass::template ReplaceArgsTupleImpl_t<ReplaceArgs_t, NewArgsTupleT>; \
\
                    template<std::size_t I /* Zero-based */, std::size_t Count, VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    /* Defers to template just above, passing the modified args (as a "std::tuple") which replaces the current args */ \
                    using ArgsModifyTuple_t = ReplaceArgsTuple_t< TupleModifyTuple_t<typename BaseClass::ArgTypes_t, I, Count, NewArgsTupleT> >; \
\
                    template<std::size_t I /* Zero-based */, std::size_t Count, VALID_FUNCTION_ARG_TYPE_C... NewArgsT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    /* Defers to template just above */ \
                    using ArgsModify_t = ArgsModifyTuple_t<I, Count, std::tuple<NewArgsT...>>; \
\
                    template <std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPE_C NewArgT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_ARG_COUNT(I) \
                    /* Defers to template just above */ \
                    using ReplaceArg_t = STATIC_ASSERT_T(STATIC_ASSERT_I_LESS_THAN_TUPLE_SIZE_F(typename BaseClass::ArgTypes_t, I), \
                                                         (ArgsModify_t<I, 1, NewArgT>)); \
\
                    template<std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToInsertTupleT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    using ArgsInsertTuple_t = ReplaceArgsTuple_t< TupleInsertTuple_t<typename BaseClass::ArgTypes_t, I, ArgsToInsertTupleT> >; \
\
                    template<std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPE_C... ArgsToInsertT> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    /* Defers to template just above */ \
                    using ArgsInsert_t = ArgsInsertTuple_t<I, std::tuple<ArgsToInsertT...> >; \
\
                    template<VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToAppendTupleT> \
                    using ArgsAppendTuple_t = ReplaceArgsTuple_t< TupleAppendTuple_t<typename BaseClass::ArgTypes_t, ArgsToAppendTupleT> >; \
\
                    template<VALID_FUNCTION_ARG_TYPE_C... ArgsToAppendT> \
                    /* Defers to template just above */ \
                    using ArgsAppend_t = ArgsAppendTuple_t< std::tuple<ArgsToAppendT...> >; \
\
                    template<std::size_t I /* Zero-based */ = 0, std::size_t Count = size_t_npos> \
                    REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I) \
                    using ArgsDelete_t = ReplaceArgsTuple_t< TupleDelete_t<typename BaseClass::ArgTypes_t, I, Count> >; \
\
                    using ArgsMakeVoid_t = MigrateCvAndRef_t<R (CC C::*)() CONST VOLATILE REF noexcept(IS_NOEXCEPT)>; \
\
                    using AddVariadicArgs_t = MigrateCvAndRef_t<R (STDEXT_CC_VARIADIC C::*)(ArgsT... ...) CONST VOLATILE REF noexcept(IS_NOEXCEPT)>; \
                    using RemoveVariadicArgs_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT...) CONST VOLATILE REF noexcept(IS_NOEXCEPT)>; \
\
                    using AddConst_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) const VOLATILE REF noexcept(IS_NOEXCEPT)>; \
                    using RemoveConst_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) VOLATILE REF noexcept(IS_NOEXCEPT)>; \
\
                    using AddVolatile_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) CONST volatile REF noexcept(IS_NOEXCEPT)>; \
                    using RemoveVolatile_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) CONST REF noexcept(IS_NOEXCEPT)>; \
\
                    using AddCV_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) const volatile REF noexcept(IS_NOEXCEPT)>; \
                    using RemoveCV_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) REF noexcept(IS_NOEXCEPT)>; \
\
                    using AddLValueReference_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) CONST VOLATILE & noexcept(IS_NOEXCEPT)>; \
                    using AddRValueReference_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) CONST VOLATILE && noexcept(IS_NOEXCEPT)>; \
                    using RemoveReference_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) CONST VOLATILE noexcept(IS_NOEXCEPT)>; \
\
                    using AddNoexcept_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) CONST VOLATILE REF noexcept>; \
                    using RemoveNoexcept_t = MigrateCvAndRef_t<R (CC C::*)(ArgsT... ELLIPSIS) CONST VOLATILE REF>;
        #else
            ///////////////////////////////////////////////////
            // For internal use only (we #undef it later).
            // Does nothing however. Function write traits
            // simply won't be added to "MemberFunctionTraits"
            // struct further below (since, when called, the
            // following just resolves to whitespace)
            ///////////////////////////////////////////////////
            #define DECLARE_MEMBER_FUNCTION_WRITE_TRAITS(...)
        #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)

        template <TRAITS_MEMBER_FUNCTION_C F, // Function's full type
                  IS_FUNCTION_C RawTypeT, // Function's raw (native) C++ type (raw version of "F" above)
                  typename ReturnTypeT, // Function's return type
                  CallingConvention CallingConventionT, // Function's calling convention (see enum class "CallingConvention")
                                                        // implicitly or explicitly declared in the function (enum corresponds
                                                        // to our #defined STDEXT_CC_? constants, whichever the function uses)
                  IS_CLASS_C MemberFunctionClassT, // Function's class (we're a non-static member function)
                  bool IsVariadicT, // "true" if function is variadic (last arg is "...") or "false" otherwise
                  bool IsConstT, // "true" if the function is declared "const" or "false" otherwise
                  bool IsVolatileT, // "true" if the function is declared "volatile" or "false" otherwise
                  FunctionReference FunctionReferenceT, // Function's reference qualifier (see enum class "FunctionReference")
                  bool IsNoexceptT, // "true" if the function is declared "noexcept" or "false" otherwise
                  typename... ArgsT> // Function's arguments (types) in left-to-right order of declaration (as would be expected)
        struct MemberFunctionTraitsBase : public FunctionTraitsBase<F, // Function's full type
                                                                    RawTypeT, // Function's raw (native) C++ type (raw version of "F" above)
                                                                    ReturnTypeT, // Function's return type
                                                                    CallingConventionT, // Function's calling convention (see enum class "CallingConvention")
                                                                                        // implicitly or explicitly declared in the function (enum corresponds
                                                                                        // to our #defined STDEXT_CC_? constants, whichever the function uses)
                                                                    MemberFunctionClassT, // Function's class (we're a non-static member function)
                                                                    IsVariadicT, // "true" if function is variadic (last arg of function is "...") or "false" otherwise
                                                                    IsConstT, // "true" if the function is declared "const" or "false" otherwise
                                                                    IsVolatileT, // "true" if the function is declared "volatile" or "false" otherwise
                                                                    FunctionReferenceT, // Function's reference qualifier (see enum class "FunctionReference")
                                                                    IsNoexceptT, // "true" if the function is declared "noexcept" or "false" otherwise
                                                                    ArgsT...> // Function's arguments (types) in left-to-right order of declaration (as would be expected)
                                    #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
                                        , public MemberFunctionWriteTraitsHelper
                                    #endif
        {
        };

        // For internal use only ("pRef" is always "", "&" or "&&")
        constexpr FunctionReference RefStrToFunctionReference(const char * const pRef) noexcept
        {
            return (pRef[0] == '\0' ? FunctionReference::None
                                    : (pRef[1] == '\0' ? FunctionReference::LValue
                                                       : FunctionReference::RValue));
        }

        ///////////////////////////////////////////////////////////////
        // For internal use only (we #undef it later). Base class for
        // "MemberFunctionTraits" specialization further below. All
        // "FunctionTraits" specializations for non-static member
        // functions ultimately derive from this.
        ///////////////////////////////////////////////////////////////
        #define MEMBER_FUNCTION_TRAITS_BASE_CLASS(CALLING_CONVENTION, RAW_CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT) \
            MemberFunctionTraitsBase<F, /* F */ \
                                     R RAW_CC (ArgsT... ELLIPSIS) CONST VOLATILE REF noexcept(IS_NOEXCEPT), /* RawTypeT */ \
                                     /*//////////////////////////////////////////////////////////////// \
                                     // Following commented out for now. Previously attempted to rely   \
                                     // on this instead of line just above, but a bug exists in MSVC    \
                                     // at this writing that causes occasional compilation errors.      \
                                     // Issue was never fully investigated, but occurs for instance if  \
                                     // "DisplayAllFunctionTraits" is specialized on a variadic         \
                                     // function after changing the calling convention using the MSVC   \
                                     // calling convention options /Gd (__cdecl), /Gr (__fastcall),     \
                                     // /Gv (__vectorcall) or /Gz (__stdcall). Numerous errors          \
                                     // surrounding "MemberFunctionRawType_t" then occur. Would be      \
                                     // better to rely on the following however if a work-around can be \
                                     // found. Entire issue needs a wider investigation so line just    \
                                     // above currently circumvents the issue for now (doesn't rely on  \
                                     // "MemberFunctionRawType_t").                                     \
                                     ////////////////////////////////////////////////////////////////*/ \
                                     /* MemberFunctionRawType_t<R (CC C::*)(ArgsT... ELLIPSIS) CONST VOLATILE REF noexcept(IS_NOEXCEPT)>, */ \
                                     R, /* ReturnTypeT */ \
                                     CALLING_CONVENTION, /* CallingConventionT (value from "enum class CallingConvention") */ \
                                     C, /* MemberFunctionClassT */ \
                                     #ELLIPSIS[0] != '\0', /* IsVariadicT (#ELLIPSIS always resolves to "" if function isn't variadic or "..." otherwise) */ \
                                     #CONST[0] != '\0', /* IsConstT (#CONST always resolves to "" if function isn't const or "const" otherwise) */ \
                                     #VOLATILE[0] != '\0', /* IsVolatileT (#VOLATILE always resolves to "" if function isn't volatile or "volatile" otherwise) */ \
                                     RefStrToFunctionReference(#REF), /* FunctionReferenceT (#REF resolves to "" if function has no ref-qualifier, or "&" or "&&" otherwise) */ \
                                     IS_NOEXCEPT, /* IsNoexceptT (true or false) */ \
                                     ArgsT... /* ArgsT */>

        //////////////////////////////////////////////////////////////////
        // Required to circumvent a bug in Visual Studio 2019 releases of
        // MSVC or earlier (V16.11 or earlier which target MSVC 19.29 or
        // earlier). We apply this in the 2nd template arg of the
        // #defined (macro) struct just below, which we call shortly to
        // create our partial specializations of "MemberFunctionTraits".
        // Without it, compilation fails in the latter versions of VS
        // when the ELLIPSIS arg in the macro just below is later passed
        // "..." to handle variadic functions. The MSFT compiler chokes
        // on the 2nd template arg being passed to the "MemberFunctionTraits"
        // specialization just below unless a comma is used to separate
        // the "ArgsT..." template arg seen below (applicable to the
        // "ArgsT" parameter pack) from the ELLIPSIS (3 dots) that
        // immediately follow it (applicable to variadic functions). When
        // the macro just below is called later on to target variadic
        // functions, MSFT_BUG_FIX_COMMA will be redefined to an actual
        // comma which corrects the error. A comma is normally optional
        // however in C++ when separating non-variadic function args from
        // the "..." that follows them in variadic functions (so applying
        // it to all compilers, not just the buggy MSVC versions above,
        // is harmless). If the bug didn't exist we could just eliminate
        // the following constant altogether since no comma separator is
        // normally required (if not for the bug)
        //////////////////////////////////////////////////////////////////
        #define MSFT_BUG_FIX_COMMA

        ////////////////////////////////////////////////////////////////////////////////
        // For internal use only (we #undef it later). Creates a "MemberFunctionTraits"
        // partial specialization to handle non-static member functions. Called
        // multiple times from the other macros just below for each unique combination
        // of macro parameters (in succession by the numeric suffix at the end of each
        // macro, starting at "1" and ending with this one, "5"). A partial
        // specialization will therefore exist for all permutations of non-static
        // member functions, e.g., those with different calling conventions, "const" vs
        // non-const member functions, member functions with and without "noexcept",
        // variadic functions, etc. Anytime someone uses this class to retrieve the
        // function traits for a non-static member function, the appropriate partial
        // specialization will therefore kick in (since "FunctionTraits" derives from
        // "MemberFunctionTraits" in this case)
        //
        // IMPORTANT: Note that these specializations handle pointers to non-member
        // functions only, and these pointers never have cv-qualifiers since we remove
        // them if present before arriving here (referring to cv-qualifiers on the
        // pointer itself, not cv-qualifiers on the actual function itself, which may
        // be present). References to pointers are also removed so the bottom line is
        // that these specializations handle pointers to non-static member functions
        // only (not references to such pointers), and only pointers without
        // cv-qualifiers as noted. To this end, it's the 2nd template arg that we're
        // effectively specializing on here as seen, and it handles non-cv-qualified
        // pointers only. Note that the 2nd template arg is always just template arg
        // "F" itself after removing any reference from "F" (if present), and then any
        // cv-qualifiers from what remains (so if what remains is a pointer to a
        // non-static member function, its cv-qualifiers are removed from the pointer
        // if any and the specialization then kicks in to handle that non-cv-qualified
        // pointer). Note that template arg "F" is just the original type passed to
        // "FunctionTraits" before removing the reference if present and/or any
        // cv-qualifiers on the resulting pointer (as just described), but it's not
        // involved in the specializations. We just pass it as-is to the
        // "FunctionTraitsBase" class where its stored in that class' "Type" alias so
        // users (or us) can access it if required (i.e., the original "F" they passed
        // to this class or any of its helper template declared later on).
        ////////////////////////////////////////////////////////////////////////////////
        #define MAKE_MEMBER_FUNC_TRAITS_5(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT) \
            template <TRAITS_MEMBER_FUNCTION_C F, \
                      typename R, \
                      IS_CLASS_C C, \
                      typename... ArgsT> \
            struct MemberFunctionTraits<F, \
                                        R (CC C::*)(ArgsT... MSFT_BUG_FIX_COMMA ELLIPSIS) CONST VOLATILE REF noexcept(IS_NOEXCEPT), \
                                        /*////////////////////////////////////////////////////// \
                                        // See same call in "struct FreeFunctionTraits" partial  \
                                        // specialization earlier (consult long comments there)  \
                                        //////////////////////////////////////////////////////*/ \
                                        std::enable_if_t<!CallingConventionReplacedWithCdecl<CALLING_CONVENTION, false>() && \
                                                         AlwaysTrue_v<F> \
                                                        > \
                                       > \
                : public MEMBER_FUNCTION_TRAITS_BASE_CLASS(CALLING_CONVENTION, RAW_CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT) \
            { \
                static_assert(std::is_same_v<RemoveCvRef_t<F>, R (CC C::*)(ArgsT... ELLIPSIS) CONST VOLATILE REF noexcept(IS_NOEXCEPT)>); \
\
            protected: \
                using SpecializedF = R (CC C::*)(ArgsT... ELLIPSIS) CONST VOLATILE REF noexcept(IS_NOEXCEPT); /* 2nd template arg */ \
                using BaseClass = MEMBER_FUNCTION_TRAITS_BASE_CLASS(CALLING_CONVENTION, RAW_CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT); \
\
                DECLARE_MEMBER_FUNCTION_WRITE_TRAITS(CC, ELLIPSIS, CONST, VOLATILE, REF, IS_NOEXCEPT) \
\
            /*///////////////////////////////////////////////////////// \
            // Following commented out for now due to a compiler bug    \
            // in MSVC. See declaration of "MemberFunctionRawType_t"    \
            // for details (can't rely it for now due to this bug  -    \
            // currently using an alternate technique as a work-around  \
            // but may revisit this again later, if MSFT ever fixes     \
            // the problem)                                             \
            /////////////////////////////////////////////////////////*/ \
            /* protected: */ \
                /*////////////////////////////////////////////////////////// \
                // Commented out for now - see comments just above.          \
                //                                                           \
                // Required by the "CallingConvention" override in our       \
                // "AbominableFunctionTraits" derivative. Ugly technique     \
                // but does the job (another way that would allow us to      \
                // eliminate the following alias is shown in the comments    \
                // preceding the use of this alias by "CallingConvention_v"  \
                // - see this in "AbominableFunctionTraits" for details).    \
                //////////////////////////////////////////////////////////*/ \
                /* using FreeFunctionRawType = MemberFunctionRawType_t<R (CC C::*)ARGS noexcept(IS_NOEXCEPT)>; */ \
\
            };

        //////////////////////////////////////////////////////////////////////////////
        // IMPORTANT: The following macros declare all specializations of non-static
        // member function pointers. As an optimization however (premature or not),
        // though it's unclear if it's actually of any benefit (very likely
        // negligible), the specializations are declared in the order that they're
        // mostly likely to be encountered when applied by end-users, with calling
        // convention preference first given to STDEXT_CC_CDECL and then
        // STDEXT_CC_THISCALL (read on). The first 4 specializations in particular
        // are as follows:
        //
        //    R (STDEXT_CC_CDECL C::*)(ArgsT...) const noexcept
        //    R (STDEXT_CC_CDECL C::*)(ArgsT...) const
        //    R (STDEXT_CC_CDECL C::*)(ArgsT...) noexcept
        //    R (STDEXT_CC_CDECL C::*)(ArgsT...)
        //
        // In practice these are by far the most likely functions users will be
        // passing on non-Microsoft platforms (read on for Microsoft). There are just
        // 4 of them as seen so their own order won't make much of a difference but
        // I've put them first in the order (before all others) in case a given
        // compiler searches for matching specializations in the order of declaration
        // (so it will find a matching declaration for any of the above reasonably
        // quickly - no need to search through less frequently used specializations
        // first, i.e., those with less frequently used calling conventions, and
        // those with the "volatile" and/or "&" or "&&" qualifiers). I don't know how
        // a given compiler might actually search through the specializations however
        // (whether declaration order necessarily matters), and there's also the
        // issue of the calling convention itself, since STDEXT_CC_THISCALL is
        // normally preferable over STDEXT_CC_CDECL when compiling for 32 bits on
        // Microsoft platforms (i.e., when compiling for 32 bits in Microsoft Visual
        // C++, non-static member functions normally default to STDEXT_CC_THISCALL
        // instead of STDEXT_CC_CDECL so ideally we'd want to check
        // STDEXT_CC_THISCALL first instead). However, when compiling for 64 bits on
        // Microsoft platforms, STDEXT_CC_CDECL will kick in instead (even on
        // non-Microsoft platforms for most calling conventions that aren't already
        // STDEXT_CC_CDECL).
        //
        // The upshot is that we should ideally check the calling convention actually
        // in effect first if possible, regardless of what it is, so that the order
        // of specialization reflects the calling conventions most likely to be in
        // effect depending on the platform and target build (32 vs 64 bit normally).
        // For now however we only (very) crudely order the specializations to try
        // and put some of the most common specializations first, and STDEXT_CC_CDECL
        // is given priority over STDEXT_CC_THISCALL (so things might be a tiny bit
        // slower on Microsoft platforms, since all 24 STDEXT_CC_CDECL
        // specializations occur before the first STDEXT_CC_THISCALL specialization
        // is encountered). A future release may try to do better assuming it's even
        // doable (and can be done cleanly), but any performance boost will likely
        // (usually) be negligible anyway, no matter what the order. With 144
        // specializations of non-static member functions we're creating at this
        // writing however (168 on Clang and Intel due to an extra calling convention
        // they support, "regcall"), for now I'm ordering things to potentially
        // improve the performance even if just a bit (again, the attempt is crude),
        // assuming it has any appreciable effect at all (but harmless if not).
        //////////////////////////////////////////////////////////////////////////////

        // noexcept (for internal use only, we #undef it later - invokes macro just above)
        #define MAKE_MEMBER_FUNC_TRAITS_4(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, REF, VOLATILE, CONST) \
            MAKE_MEMBER_FUNC_TRAITS_5(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, CONST, VOLATILE, REF, true) \
            MAKE_MEMBER_FUNC_TRAITS_5(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, CONST, VOLATILE, REF, false)

        // const (for internal use only, we #undef it later - invokes macro just above)
        #define MAKE_MEMBER_FUNC_TRAITS_3(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, REF, VOLATILE) \
            MAKE_MEMBER_FUNC_TRAITS_4(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, REF, VOLATILE, const) \
            MAKE_MEMBER_FUNC_TRAITS_4(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, REF, VOLATILE, )

        // volatile (for internal use only, we #undef it later - invokes macro just above)
        #define MAKE_MEMBER_FUNC_TRAITS_2(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, REF) \
            MAKE_MEMBER_FUNC_TRAITS_3(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, REF, ) \
            MAKE_MEMBER_FUNC_TRAITS_3(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, REF, volatile)

        // && and && (for internal use only, we #undef it later - invokes macro just above)
        #define MAKE_MEMBER_FUNC_TRAITS_1(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS) \
            MAKE_MEMBER_FUNC_TRAITS_2(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, ) \
            MAKE_MEMBER_FUNC_TRAITS_2(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, &) \
            MAKE_MEMBER_FUNC_TRAITS_2(CC, CALLING_CONVENTION, RAW_CC, ELLIPSIS, &&)

        ///////////////////////////////////////////////////////////////////
        // For internal use only (we #undef it later). Invokes macro just
        // above. Launching point (first macro to be called) to create
        // partial specializations of "MemberFunctionTraits" for handling
        // every permutation of non-variadic functions for the passed
        // calling convention (variadic functions are handled just after).
        ///////////////////////////////////////////////////////////////////
        #define MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(CC, CALLING_CONVENTION, RAW_CC) \
            MAKE_MEMBER_FUNC_TRAITS_1(CC, CALLING_CONVENTION, RAW_CC, )

        ////////////////////////////////////////////////////////////
        // Call above macro once for each calling convention (for
        // internal use only - invokes macro just above). Note that
        // am ordering these calls by their (likely) frequency of
        // occurrence in order to potentially speed things up a bit
        // (based on partial guesswork though but see IMPORTANT
        // comments preceding MAKE_MEMBER_FUNC_TRAITS_4 above for
        // further details)
        ////////////////////////////////////////////////////////////
        MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_CDECL,    CallingConvention::Cdecl,    STDEXT_CC_CDECL)
        MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_THISCALL, CallingConvention::Thiscall, )
        MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_STDCALL,  CallingConvention::Stdcall,  STDEXT_CC_STDCALL)
        MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_FASTCALL, CallingConvention::Fastcall, STDEXT_CC_FASTCALL)
        #if defined(STDEXT_CC_VECTORCALL)
            MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_VECTORCALL, CallingConvention::Vectorcall, STDEXT_CC_VECTORCALL)
        #endif
        #if defined(STDEXT_CC_REGCALL)
            MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_REGCALL, CallingConvention::Regcall, STDEXT_CC_REGCALL)
        #endif

        // Done with this (for internal use only)
        #undef MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC

        ////////////////////////////////////////////////
        // Constants just below not #defined otherwise
        // (applicable only when function write traits
        // supported - usually the case)
        ////////////////////////////////////////////////
        #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
            /////////////////////////////////////////////
            // Done with this (redefining it just below
            // for variadic function specializations)
            /////////////////////////////////////////////
            #undef MEMBER_FUNCTION_REPLACE_CALLING_CONVENTION

            ////////////////////////////////////////////////////////////////
            // For internal use only (we #undef it later). Used to
            // implement "MemberFunctionTraits::ReplaceCallingConvention_t"
            // for variadic non-static member functions. However, variadic
            // functions must always have the STDEXT_CC_VARIADIC calling
            // convention for all compilers we support at this writing
            // (note that STDEXT_CC_VARIADIC just resolves to
            // STDEXT_CC_CDECL), so the calling convention therefore can't
            // be changed to anything else. Hence the following just
            // resolves to template arg "F" in "MemberFunctionTraits". IOW,
            // if the caller invokes its "ReplaceCallingConvention_t" alias
            // it always returns "F" so the calling convention remains
            // unchanged (always STDEXT_CC_VARIADIC).
            ////////////////////////////////////////////////////////////////
            #define MEMBER_FUNCTION_REPLACE_CALLING_CONVENTION(...) F
        #endif

        ////////////////////////////////////////////////////////////////
        // See this private constant (#defined earlier) for details.
        // Required here since we're about to handle variadic functions
        // in macro call just below so following constant required to
        // circumvent the MSFT bug described in the constant's comments
        // earlier.
        ////////////////////////////////////////////////////////////////
        #undef MSFT_BUG_FIX_COMMA
        #define MSFT_BUG_FIX_COMMA ,

        /////////////////////////////////////////////////////////////////
        // For internal use only (we #undef it later). Creates partial
        // specializations of "MemberFunctionTraits" to handle variadic
        // functions (those whose last arg is "..."). Simply launches
        // MAKE_MEMBER_FUNC_TRAITS_1 as seen which creates partial
        // specializations for handling every permutation of variadic
        // function. These are always assumed to be STDEXT_CC_CDECL in
        // this release (what STDEXT_CC_VARIADIC just below resolves to)
        // since it's the only calling convention that supports variadic
        // functions among all the platforms we support at this writing
        // (but in the general case it's normally the calling convention
        // used for variadic functions on most platforms anyway, though
        // some platforms may have other calling conventions that
        // support variadic functions but it would be rare and we don't
        // currently support them anyway).
        /////////////////////////////////////////////////////////////////
        MAKE_MEMBER_FUNC_TRAITS_1(STDEXT_CC_VARIADIC, CallingConvention::Variadic, STDEXT_CC_VARIADIC, ...)

        // Done with these (for internal use only)
        #undef MSFT_BUG_FIX_COMMA
        #undef MAKE_MEMBER_FUNC_TRAITS_1
        #undef MAKE_MEMBER_FUNC_TRAITS_2
        #undef MAKE_MEMBER_FUNC_TRAITS_3
        #undef MAKE_MEMBER_FUNC_TRAITS_4
        #undef MAKE_MEMBER_FUNC_TRAITS_5
        #undef MEMBER_FUNCTION_TRAITS_BASE_CLASS
        #undef DECLARE_MEMBER_FUNCTION_WRITE_TRAITS

        ///////////////////////////////////////////////////////////////
        // Following only #defined if FUNCTION_WRITE_TRAITS_SUPPORTED
        // is #defined (usually the case)
        ///////////////////////////////////////////////////////////////
        #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
            // Done with these (for internal use only)
            #undef MEMBER_FUNCTION_REPLACE_CALLING_CONVENTION
            #undef MEMBER_FUNCTION_REPLACE_ARGS
            #undef MEMBER_FUNCTION_REPLACE_RETURN_TYPE
        #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)

        ///////////////////////////////////////////////////////////////
        // Following only #defined if FUNCTION_WRITE_TRAITS_SUPPORTED
        // is #defined (usually the case)
        ///////////////////////////////////////////////////////////////
        #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
            // Done with these (for internal use only)
            #undef REPLACE_ARGS_IMPL
            #undef DECLARE_REPLACE_ARGS_IMPL
            #undef STDEXT_CC_REGCALL_REPLACE
            #undef STDEXT_CC_VECTORCALL_REPLACE
            #undef REPLACE_RETURN_TYPE_IMPL
            #undef DECLARE_REPLACE_RETURN_TYPE_IMPL
        #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
    } // namespace Private

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraits (primary template). Template that all users rely on to
    // retrieve the traits of any function, though users will normally rely
    // on the (easier) helper templates for "FunctionTraits" instead
    // (declared later on), which ultimately rely on "FunctionTraits" (but
    // are normally cleaner and less verbose than using "FunctionTraits"
    // directly). Note that the following declaration is the primary template
    // for "FunctionTraits" but all uses of this template always rely on a
    // particular partial specialization declared just below, one for each
    // permutation of function type we support. If template arg "F" of
    // "FunctionTraits" isn't a function type we support however (so not
    // suitable for passing to this template - see comments preceding
    // "IsTraitsFunction_v" for details), then the following primary template
    // kicks in. A compiler error therefore always occurs by design since the
    // template is declared only. It has no definition since "F" must be
    // illegal by definition (not a function type supported by
    // "FunctionTraits" - a specialization would have kicked in otherwise).
    // In this case the TRAITS_FUNCTION_C concept will trigger for C++20 or
    // later so a constraints violation will occur. For C++17 however, a
    // compiler-specific error of some kind will occur instead, indicating
    // that the template isn't defined or specialized. Users should then fix
    // whatever invalid type they're passing since it's not a function type
    // supported by this library (but all mainstream functions types are
    // supported so the type isn't a function at all). Note that no
    // "static_assert" is used in the latter case by design (though the class
    // could be defined with one), since the class would no longer be SFINAE
    // friendly in C++17 (even though a "static_assert" could display a more
    // user-friendly message than what the compiler puts out when an invalid
    // type is passed for "F" - a "not declared" error of some kind for the
    // primary template of "FunctionTraits" isn't exactly the cleanest
    // message). However, the "static_assert" would then trigger even in
    // SFINAE contexts (unlike the TRAITS_FUNCTION_C concept in C++20 or
    // later - concepts are always SFINAE friendly, "static_asserts" aren't),
    // so the class couldn't be used during SFINAE (since if passed an
    // illegal value of "F" then the "static_assert" would kick in even
    // during valid SFINAE situations - omitting the "static_assert" allows
    // SFINAE to work as usual). Normally "F" will be valid though (why would
    // anyone pass something invalid unless it's for SFINAE purposes), so "F"
    // must be one of the following types (a partial specialization to handle
    // each case will then always kick in or the primary template itself if
    // "F" is illegal - again, see comments preceding "IsTraitsFunction_v"
    // for details). Note that all pointer types in this list may contain
    // optional cv-qualifiers:
    //
    //    1) Free function (i.e., any function satisfying "std::is_function"
    //       EXcluding abominable functions which are still supported via
    //       via 8 below but aren't considered "free" functions by this
    //       library - see "IsFreeFunction_v()" for details)
    //    2) Pointer to free function
    //    3) Reference to free function
    //    4) Reference to pointer to free function
    //    5) Pointer to non-static member function
    //    6) Reference to pointer to non-static member function (references
    //       to non-static member functions not supported by C++ itself, only
    //       references to pointers are).
    //    7) Non-overloaded functor (or reference to functor) including lambdas
    //    8) Abominable functions
    //    9) "std::function" specializations (or references to them)
    //
    // IMPORTANT:
    // ---------
    // Given the types we support as seen above, including optional
    // cv-qualifiers on any of the pointer to function types, and also
    // factoring in calling conventions on the target function, as well as
    // "noexcept" and, for non-static member functions or functors, cv and
    // ref qualifiers, we're dealing with 1878 permutations (required
    // specializations on all platforms we currently support - a bit more for
    // Clang and Intel compilers which have an extra calling convention,
    // "regcall"). Although this can easily be handled in a relatively small
    // amount of actual code due to judicious use of the macros seen further
    // above (used to create these specializations - they handled all 1878
    // specializations in earlier releases), we've since adjusted these
    // macros to handle a much smaller number of specializations (154). We're
    // still able to target all 1878 permutations of function types however
    // by specializing "F" *after* removing the optional reference and/or
    // pointer to "F" if it targets a free function (resulting in the actual
    // free function type itself), or the optional reference and/or
    // cv-qualifiers if "F" targets a non-static member function (resulting
    // in a non-cv-qualified, non-static member function pointer - functors
    // are routed through here as well). We remove the optional reference
    // and/or pointer on "F" in order to specialize "FunctionTraits" on "F"
    // *after* these removals, not on "F" itself, which therefore reduces the
    // number of specializations from 1878 to 154 as noted (big improvement).
    // After these removals on "F" that is, we only require specializations
    // for either raw free function types (never pointers to free functions
    // or references to free functions or references to pointers to free
    // functions), or (non-cv-qualified) pointers to non-static member
    // functions (never references to such pointers). This greatly reduces
    // the number of specializations we need to handle as noted, again, from
    // 1878 to 154, but we still capture the same information for all 1878
    // permutations because these removals don't impact our ability to
    // collect this information. We can capture the same info we used to
    // capture in 1878 specializations but now using only 154
    // specializations.
    //
    // The upshot is that these removals are designed solely to reduce the
    // large number of specializations of "FunctionTraits" that would
    // otherwise be required (1878), without affecting our ability to capture
    // the exact same info in all these specializations (which we now do
    // using only 154 specializations).
    //
    // Finally, please be aware that when "F" is in fact a supported function
    // type, its calling convention may be changed by the compiler to the
    // "cdecl" calling convention. This normally occurs when compiling for 64
    // bits opposed to 32 bits, or possibly other scenarios depending on the
    // compiler options in effect (platform specific), in which case the
    // compiler ignores the calling convention on "F" and automatically
    // switches to the "cdecl" calling convention. Note that those
    // "FreeFunctionTraits" and "MemberFunctionTraits" specializations
    // further above which handle calling conventions are designed to detect
    // this situation and when it occurs, the specializations for all calling
    // conventions besides "cdecl" itself therefore won't kick in (such as
    // when compiling for 64 bits as noted - the compiler always switches to
    // the "cdecl" calling convention in this case unless a given calling
    // convention is supported in that environment, normally the "vectorcall"
    // calling convention only at this writing, which unlike the other
    // non-cdecl calling conventions is still applied in 64 bit builds). The
    // primary template for "FreeFunctionTraits" and "MemberFunctionTraits"
    // themselves would therefore normally kick in instead in this situation
    // (since no specializations now exist for these calling conventions - we
    // removed them), but it will never happen because these calling
    // conventions will never be encountered in "F". The "cdecl" calling
    // convention always will be and a specialization always exists for it.
    // For instance, if function "F" is declared with the "stdcall" calling
    // convention (whatever its syntax is for a given compiler), and you
    // compile for 64 bits opposed to 32 bits, the compiler will ignore the
    // "stdcall" calling convention on "F" and use "cdecl" instead (for all
    // compilers we currently support). No specialization is therefore
    // required for "stdcall" so the specialization that handles "stdcall"
    // won't kick in (based on our design), since the "cdecl" specialization
    // will handle it. IOW, even though there's no specialization for
    // functions with the "stdcall" calling convention, it doesn't matter
    // since the "stdcall" in function "F" has been replaced with "cdecl" by
    // the compiler. The primary template therefore won't kick in for
    // functions declared with "stdcall", the specialization that handles
    // "cdecl" will. The primary template only kicks in if "F" isn't a
    // supported function type (someone passes an "int" for instance which
    // isn't even a function), in which case either the TRAITS_FUNCTION_C
    // concept (C++20 or later) seen in the following struct will trigger
    // (see below), or a compiler-specific error will occur in C++17 (that
    // the template isn't defined or specialized). Specializations whose
    // calling convention is replaced by "cdecl" therefore never wind up here
    // as described, so no error will ever be flagged for them (they're still
    // supported functions that is, only that the "cdecl" specialization will
    // now kick in to handle them instead of the usual specialization that
    // normally targets that specific calling convention).
    ////////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F,
              typename = void> // Arg for internal use only (users should never explicitly pass it)
    struct FunctionTraits; // Not defined by design in order to support SFINAE.
                           // See large comments above.

    ////////////////////////////////////////////////////////////////////////
    // "FunctionTraits" partial specialization for handling free functions
    // including static member functions. "F" can be any of the following:
    //
    //   1) A free function including static member functions
    //   2) An (optionally cv-qualified) pointer to a free function
    //   3) A reference to a free function
    //   4) A reference to an (optionally cv-qualified) pointer to a free
    //      function
    ////////////////////////////////////////////////////////////////////////
    template <typename F>
    struct FunctionTraits<F,
                          std::enable_if_t< IsTraitsFreeFunction_v<F> > // True if "F" is a free function or a
                                                                        // pointer to one or a reference to one
                                                                        // or a reference to a pointer to one
                         >
                         : Private::FreeFunctionTraits<F>
    {
    };

    /////////////////////////////////////////////////////////////////////////////
    // "FunctionTraits" partial specialization for handling non-static member
    // functions. "F" can be either of the following:
    //
    //   1) An (optionally cv-qualified) pointer to a non-static member function
    //   2) A reference to an (optionally cv-qualified) pointer to a non-static
    //      member function
    /////////////////////////////////////////////////////////////////////////////
    template <typename F>
    struct FunctionTraits<F,
                          std::enable_if_t< IsTraitsMemberFunction_v<F> > // True if "F" is a pointer to a
                                                                          // non-static member function or
                                                                          // a reference to such a pointer
                         >
                         : Private::MemberFunctionTraits<F>
    {
    };

    // For internal use only
    namespace Private
    {
        /////////////////////////////////////////////////////////////////
        // struct FunctorTraitsBase (base class of both "FunctorTraits"
        // and "StaticFunctorTraits"
        /////////////////////////////////////////////////////////////////
        template <TRAITS_NON_STATIC_OR_STATIC_FUNCTOR_C F>
        struct FunctorTraitsBase
        {
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concept kicks in in the template
                // declaration above instead (macro for this
                // above resolves to the "typename" keyword when
                // concepts aren't supported and the following
                // "static_assert" is then used instead)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_TRAITS_NON_STATIC_OR_STATIC_FUNCTOR(F);
            #endif

            using FunctorType = F;

            ////////////////////////////////////////////////////
            // For internal use only for now (not intended for
            // end users but should be made available to them
            // in a future release)
            ////////////////////////////////////////////////////
            using RawFunctorType_t = RemoveCvRef_t<F>;
            static constexpr tstring_view RawFunctorTypeName_v = TypeName_v<RawFunctorType_t>;
        }; // struct FunctorTraitsBase

        template <TRAITS_FUNCTOR_C F>
        struct FunctorTraits : MemberFunctionTraits< decltype(&std::remove_reference_t<F>::operator()) >,
                               FunctorTraitsBase<F>
        {
        private:
            using BaseClass = MemberFunctionTraits< decltype(&std::remove_reference_t<F>::operator()) >;

            // See this constant for details
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concept kicks in in the template
                // declaration above instead (macro for this
                // above resolves to the "typename" keyword when
                // concepts aren't supported and the following
                // "static_assert" is then used instead)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_TRAITS_FUNCTOR(F)
            #endif

        public:
            //////////////////////////////////////////////////////////////////
            // Overrides (hides) the same member in the "FunctionTraitsBase"
            // class (which is always "FunctionOrigin::None"). See this for
            // details.
            //////////////////////////////////////////////////////////////////
            static constexpr FunctionOrigin FunctionOrigin_v = FunctionOrigin::Functor;
        }; // struct FunctorTraits
    } // namespace Private

    //////////////////////////////////////////////////////////////////////////
    // "FunctionTraits" partial specialization for handling non-overloaded
    // function objects (AKA functors). "F" can be either a functor or a
    // reference to one. In either case this includes lambdas which are just
    // syntactic sugar for creating function objects on-the-fly (the compiler
    // creates a hidden functor behind the scenes - note that generic lambdas
    // can't be targeted using functor syntax but can be targeted using
    // pointer-to-member syntax as seen in example 3 below). Template arg "F"
    // therefore isn't a function type in this specialization (unlike the
    // other specializations which handle function types and pointers and
    // references to such types), but a class or struct type with a single
    // "F::operator()" member function (the class or struct type itself is
    // known as the lambda's "closure type"). The traits created by this
    // specialization are therefore for that function. Note that overloads of
    // "F::operator()" aren't supported however since it would be ambiguous
    // (which "F::operator()" to use). In this case the following template
    // will fail SFINAE (due to the ambiguity) so the primary template will
    // kick in instead (which isn't defined resulting in a TRAITS_FUNCTION_C
    // constraint error in C++20 or later, or a "template not defined" error
    // of some kind in C++17 - both are SFINAE friendly but the bottom line
    // is that "F" isn't a legal function type).
    //
    //     Example 1 (functor)
    //     -------------------
    //     class MyFunctor
    //     {
    //     public:
    //          int operator()(float, const std::string &);
    //     };
    //
    //     ////////////////////////////////////////////////////////////
    //     // Resolves to "int". Following helper template relies on
    //     // "FunctionTraits" to determine this (passing a functor
    //     // here). See it for details.
    //     ////////////////////////////////////////////////////////////
    //     using MyClassReturnType_t = ReturnType_t<MyFunctor>
    //
    //     ////////////////////////////////////////////////////////////
    //     // Resolves to "const std::string &" (passing 1 here which
    //     // is the zero-based index of the arg we want). Following
    //     // helper template relies on "FunctionTraits" to determine
    //     // this. See it for details.
    //     ////////////////////////////////////////////////////////////
    //     using Arg2Type_t = ArgType_t<MyFunctor, 1>
    //
    //     Example 2 (lambda - note that lambdas are just functors)
    //     --------------------------------------------------------
    //     const auto myLambda = [](float val, const std::string &str)
    //                             {
    //                                 int rc;
    //                                 // ...
    //                                 return rc;
    //                             };
    //
    //     ///////////////////////////////////////////////////////////////
    //     // Resolves to "int" (lambda's return type). Following helper
    //     // template relies on "FunctionTraits" to determine this. See
    //     // it for details.
    //     ///////////////////////////////////////////////////////////////
    //     using MyLambdaReturnType_t = ReturnType_t<decltype(myLambda)>;
    //
    //     ////////////////////////////////////////////////////////////
    //     // Resolves to "const std::string &" (passing 1 here which
    //     // is the zero-based index of the arg we want). Following
    //     // helper template relies on "FunctionTraits" to determine
    //     // this. See it for details.
    //     ////////////////////////////////////////////////////////////
    //     using Arg2Type_t = ArgType_t<decltype(myLambda), 1>;
    //
    //     Example 3 (generic lambda). See comments below.
    //     -----------------------------------------------
    //     const auto myLambda = [](auto val, const std::string &str)
    //                             {
    //                                 int rc;
    //                                 // ...
    //                                 return rc;
    //                             };
    //
    //     //////////////////////////////////////////////////////////////
    //     // Generic lambdas, similar to lambda templates in C++20,
    //     // can't be handled using functor syntax, i.e., by passing
    //     // the lambda's type like in "Example 2" above (to
    //     // "FunctionTraits" or any of its helper templates). That's
    //     // because for all lambdas, generic or not, C++ creates a
    //     // hidden functor behind the scenes whose "operator()" member
    //     // has the same args as the lambda itself, but when a generic
    //     // lambda is created, such as the one in "Example 3" above,
    //     // a template version of "operator()" is created instead (to
    //     // handle the lambda's "auto" parameters). The functor's
    //     // class (i.e., it's "closure type") is therefore not really
    //     // a functor in the conventional sense since "operator()" is
    //     // actually a function template (unlike for non-generic
    //     // lambdas where "operator()" isn't a template). In "Example
    //     // 3" just above for instance, C++ will create a class behind
    //     // the scenes that looks something like the following (which
    //     // we're calling "Lambda" here but whatever internal name the
    //     // compiler actually assigns it - it's an implementation
    //     // detail we don't care about):
    //     //
    //     //     class Lambda
    //     //     {
    //     //     public:
    //     //         template <class T>
    //     //         inline auto operator()(T val, const std::string &str) const
    //     //         {
    //     //            int rc;
    //     //            // ...
    //     //            return rc;
    //     //         }
    //     //     };
    //     //
    //     // You therefore can't simply pass "decltype(myLambda)" seen
    //     // in example 3 above to "FunctionTraits" or any of its helper
    //     // templates like you could in "Example 2" because "operator()"
    //     // is now a template. It therefore needs to be specialized on
    //     // template arg "T" first, unlike "Example 2" whose own
    //     // "operator()" member isn't a template (it's a non-template
    //     // function). Dealing with "myLambda" in "Example 3" is
    //     // therefore different than for "Example 2" because you can't
    //     // pass "decltype(myLambda)" to "FunctionTraits" or any of
    //     // its helper templates (since the compiler doesn't know the
    //     // function template arguments for "operator()" until you
    //     // explicitly tell it). To do so you need to instantiate the
    //     // underlying "Lambda::operator()" template in "Example 3"
    //     // and access it using pointer-to-member function syntax
    //     // instead of functor syntax. The syntax for doing so is
    //     // ugly compared to normal functor syntax but you can always
    //     // create a helper alias to clean it up a bit if you wish
    //     // (though we don't do that here). The following example
    //     // demonstrates this by creating alias "F" which specializes
    //     // "operator()" on type "float" and then takes its address
    //     // as seen (details to follow). "F" in the following call
    //     // therefore effectively resolves to the following type:
    //     //
    //     //     int (Lambda::*)(float, const std::string &) const
    //     //
    //     // We therefore have an ordinary member function pointer type
    //     // that we can pass to "FunctionTraits" or any of its helper
    //     // templates. From this point on everything works the same as
    //     // "Example 2" above except "F" is the functor's class type
    //     // in that example and a pointer to a non-static member
    //     // function in the following example (but the 1st arg in each
    //     // example is still a "float" - the only difference for our
    //     // purposes is how we pass the lambda to "FunctionTraits" or
    //     // any of its helper functions, by using functor syntax in
    //     // "Example 2" and pointer-to-member function syntax in
    //     // "Example 3")
    //     //
    //     // Lastly, note that in the following (cryptic) call, the
    //     // call to "decltype(myLambda)" simply returns the type of
    //     // "myLambda" (i.e., the compiler-generated class for this
    //     // which is just "class Lambda" seen above), and we then
    //     // specialize its "operator()" member on type "float". By
    //     // applying the "&" operator to the result, we therefore
    //     // get back a pointer to "operator()" which is now just an
    //     // ordinary member function specialized on "float" (so we
    //     // have a pointer to this function). The outer "decltype"
    //     // then returns the type of this pointer as seen above.
    //     // Again, the syntax is nasty but a helper alias can always
    //     // be written to clean it up a bit (and may be made available
    //     // in a future release of this library). Note that creating a
    //     // specialization of "FunctionTraits" itself to handle this
    //     // syntax instead isn't viable for reasons beyond the scope
    //     // of this documentation (such a specialization would require
    //     // the caller to pass the type(s) that "operator()" needs to
    //     // be specialized on which introduces unwieldy complications).
    //     //////////////////////////////////////////////////////////////
    //     using F = decltype(&decltype(myLambda)::template operator()<float>); // See above for details. A helper alias
    //                                                                          // to make the syntax cleaner will likely
    //                                                                          // be made available in a future release
    //                                                                         //  of this header.
    //
    //     ///////////////////////////////////////////////////////
    //     // Resolves to "int" (lambda's return type) just like
    //     // "Example 2".
    //     ///////////////////////////////////////////////////////
    //     using MyLambdaReturnType_t = ReturnType_t<F>;
    //
    //     ////////////////////////////////////////////////////////
    //     // Resolves to "const std::string &", again, just like
    //     // "Example 2" (and again, passing 1 here which is the
    //     // zero-based index of the arg we want).
    //     ////////////////////////////////////////////////////////
    //     using Arg2Type_t = ArgType_t<F, 1>;
    //////////////////////////////////////////////////////////////////////////
    template <typename F>
    struct FunctionTraits<F,
                          std::enable_if_t< IsTraitsFunctor_v<F> > // True if "F" is a non-overloaded, non-static
                                                                   // functor or a reference to one but not a
                                                                   // "std::function" (even though the latter is
                                                                   // technically a functor - it's explicitly bypassed
                                                                   // here since we have a separate "FunctionTraits"
                                                                   // specialization for it further below)
                         >
                         : Private::FunctorTraits<F>
    {
    };

    // For internal use only
    namespace Private
    {
        template <TRAITS_STATIC_FUNCTOR_C F>
        struct StaticFunctorTraits : FreeFunctionTraits< decltype(std::remove_reference_t<F>::operator()) >,
                                     FunctorTraitsBase<F>
        {
        private:
            using BaseClass = FreeFunctionTraits< decltype(std::remove_reference_t<F>::operator()) >;

            // See this constant for details
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concept kicks in in the template
                // declaration above instead (macro for this
                // above resolves to the "typename" keyword when
                // concepts aren't supported and the following
                // "static_assert" is then used instead)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_TRAITS_STATIC_FUNCTOR(F)
            #endif

        public:
            //////////////////////////////////////////////////////////////////
            // Overrides (hides) the same member in the "FunctionTraitsBase"
            // class (which is always "FunctionOrigin::None"). See this for
            // details.
            //////////////////////////////////////////////////////////////////
            static constexpr FunctionOrigin FunctionOrigin_v = FunctionOrigin::StaticFunctor;
        }; // struct StaticFunctorTraits
    } // namespace Private

    //////////////////////////////////////////////////////////////////////////
    // "FunctionTraits" partial specialization for handling non-overloaded
    // *static* functors (opposed to non-static functors which have their own
    // "FunctionTraits" specialization further above). Note that static
    // functors were legalized in C++23 (i.e., functors whose "operator()"
    // member is *static*, though they'll normally be rare in practice). Note
    // that "F" can be either a static functor or a reference to one. See the
    // "FunctionTraits" specialization further above for handling non-static
    // functors (the usual functors that will be encountered in practice
    // normally). The following is identical except it deals with the
    // *static* "operator()" member (so dealing with lambdas not an issue
    // here - they're always based on the non-static "operator()")
    //////////////////////////////////////////////////////////////////////////
    template <typename F>
    struct FunctionTraits<F,
                          std::enable_if_t< IsTraitsStaticFunctor_v<F> > // True if "F" is a non-overloaded static
                                                                         // functor or a reference to one
                         >
                         : Private::StaticFunctorTraits<F>
    {
    };

    namespace Private
    {
        template <TRAITS_STD_FUNCTION_C F>
        /////////////////////////////////////////////////////////
        // Note: In theory we should derive the following from
        // "FreeFunctionTraits" instead of "FunctionTraits" as
        // we're doing since the template arg of "std::function"
        // can only legally be a free function in C++ (and with
        // some restrictions too - see comments in template
        // "StdFunctionTemplateArg_t"). Should a user specialize
        // "std::function" on non-free function however, which
        // isn't legal, (again, "std::function" only legally
        // takes free functions), the following will handle it
        // even though it's illegal in C++ itself. A compiler
        // error might therefore occur or undefined behavior but
        // it has nothing to do with us (since we purposely
        // derive the following from "FunctionTraits" instead of
        // "FreeFunctionTraits" so it can handle any function
        // type the user specializes "std::function" on even if
        // it's not a legal type for "std::function" itself).
        // Only if it's not an actual function type handled by
        // this library will we have an issue, since no
        // "FunctionTraits" specialization will then exist for
        // it so the following will normally result in an
        // undefined "FunctionTraits" error (it's declared but
        // not defined). Why someone would pass an (obviously
        // illegal) non-function type to "std::function" however
        // (unless it's occurring in an SFINAE context) is
        // anyone's guess. Otherwise it's guaranteed to cause a
        // compiler error in the caller's own code regardless of
        // our library.
        /////////////////////////////////////////////////////////
        struct StdFunctionTraits : public FunctionTraits< StdFunctionTemplateArg_t< std::remove_reference_t<F> > >
        {
            // See this constant for details
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concept kicks in in the template
                // declaration above instead (macros for this
                // above resolves to the "typename" keyword when
                // concepts aren't supported and the following
                // "static_assert" is then used instead)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_TRAITS_STD_FUNCTION(F)
            #endif

            //////////////////////////////////////////////////////////////////
            // Overrides (hides) the same member in the "FunctionTraitsBase"
            // class (which is always "FunctionOrigin::None). See this for
            // details.
            //////////////////////////////////////////////////////////////////
            static constexpr FunctionOrigin FunctionOrigin_v = FunctionOrigin::StdFunction;
        }; // struct StdFunctionTraits
    } // namespace Private

    //////////////////////////////////////////////////////////////////////////
    // "FunctionTraits" partial specialization for handling "std::function"
    // types. Returns the traits for the template arg that "std::function"
    // itself is specialized on. This should always be a free function in
    // theory and never noexcept either, since in both cases "std::function"
    // must be specialized on a noexcept function type as per C++ rules for
    // this template. In practice though it might be specialized on something
    // else (a noexcept function, an abominable function, or even a
    // non-function type) but it's not legal to do. Different compilers will
    // handle it their own way however, some allowing certain illegal types,
    // others don't and will fail immediately compilation (others might not
    // fail compilation or may fail only when the "std::function" is actually
    // used). So long as the type that "std::function" is specialized on is
    // an actual function type handled by this library however (assuming the
    // compiler itself doesn't complain when the user specializes their
    // "std::function" type on something illegal), then the following will
    // successfully kick in (though the base class will recursively call this
    // specialization again if "F" is specialized on another "std::function"
    // type - that would be extremely unlikely in practice though so we won't
    // worry about it - it's illegal anyway).
    //////////////////////////////////////////////////////////////////////////
    template <typename F>
    struct FunctionTraits<F,
                          std::enable_if_t< IsTraitsStdFunction_v<F> > // True if "F" is a "std::function" specialization or reference to one
                         >
                         : Private::StdFunctionTraits<F>
    {
    };

    ////////////////////////////////////////////////////////////
    // Returns the default calling convention for the target
    // compiler. Pass "true" for the "IsFreeFunc" template arg
    // if you require this for a free function (including
    // static member functions), or false for a non-static
    // member function. They might differ depending on the
    // compiler. MSVC in particular defaults to "__thiscall"
    // for non-static member functions when targeting x86 or
    // for free functions, the /Gd (__cdecl), /Gr (__fastcall),
    // /Gv (__vectorcall) or /Gz (__stdcall) calling convention
    // option that's in effect (defaults to /Gd = cdecl). See:
    //
    //     https://learn.microsoft.com/en-us/cpp/build/reference/gd-gr-gv-gz-calling-convention?view=msvc-170
    //
    // When targeting x64 however MSVC defaults to "__cdecl"
    // for both free and non-static member functions. All other
    // compilers we support always default to "cdecl" for both
    // free and non-static member functions regardless of the
    // architecture (32 or 64 bits).
    ////////////////////////////////////////////////////////////
    template <bool IsFreeFunc>
    inline constexpr CallingConvention DefaultCallingConvention_v = FunctionTraits< std::conditional_t<IsFreeFunc,
                                                                                                       void(),
                                                                                                       void (Private::AnyClass::*)()
                                                                                                      >
                                                                                  >::CallingConvention_v;

    /////////////////////////////////////////////////////////////////////
    // DefaultCallingConventionName_v. See "DefaultCallingConvention_v"
    // just above. Simply converts that to a string suitable for display
    // purposes and returns it as a "tstring_view".
    /////////////////////////////////////////////////////////////////////
    template <bool IsFreeFunc>
    inline constexpr tstring_view DefaultCallingConventionName_v = CallingConventionToString(DefaultCallingConvention_v<IsFreeFunc>);

    // For internal use only
    namespace Private
    {
        template <ABOMINABLE_FUNCTION_C F>
        struct AbominableFunctionTraits : public MemberFunctionTraits<F AnyClass::*>
                                    #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
                                        , public FreeOrAbominableFunctionWriteTraitsHelper
                                    #endif
        {
            // See this constant for details
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concept kicks in in the template
                // declaration above instead (macro for this
                // above resolves to the "typename" keyword when
                // concepts aren't supported and the following
                // "static_assert" is then used instead)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_ABOMINABLE_FUNCTION(F)
            #endif

        private:
            using BaseClass = MemberFunctionTraits<F AnyClass::*>;

            template <MEMBER_FUNCTION_POINTER_C NewF>
            using ToRawType_t = typename MemberFunctionTraits<NewF>::RawType_t;

        public:
            //////////////////////////////////////////////////////
            // Note that *all* declarations below are base class
            // overrides (they hide the base class declarations)
            //////////////////////////////////////////////////////

            //////////////////////////////////////////////////////////////////////////////////
            // "FunctionType_t". Hides the base class version. Not setting it to template arg
            // "F" itself just in case the user explicitly set the calling convention of "F"
            // to STDEXT_CC_THISCALL. It's very unlikely but we don't support it.
            // STDEXT_CC_THISCALL applies to non-static member functions only and even MSFT
            // will reject it at least under some circumstances (with MSVC Compiler Error
            // C3865 - strange they permit it in certain other contexts but it's not legal).
            // The upshot is that if STDEXT_CC_THISCALL is in fact present in "F" for
            // unknown reasons (why would a user ever add it to an abominable function
            // though), then the following simply removes it (so "FunctionType_t" will
            // normally be the same as "F" but with STDEXT_CC_THISCALL now removed - the
            // default calling convention for free functions therefore kicks in instead,
            // which on MSFT platforms will be the /Gd (__cdecl), /Gr (__fastcall), /Gv
            // (__vectorcall) or /Gz (__stdcall) compiler option, assuming it's not ignored
            // on a given platform (in particular on x64, where calling conventions other
            // than "vectorcall" and "regcall" are ignored by all compilers we support,
            // though "regcall" isn't available on MSVC). Note that on MSVC this option
            // defaults to /Gd (__cdecl) but see
            // https://learn.microsoft.com/en-us/cpp/build/reference/gd-gr-gv-gz-calling-convention?view=msvc-170
            // for details at this writing - link liable to go dead though at some point
            // since MSFT is notorious for this). For all other compilers we currently
            // support the default will normally be "cdecl" instead. If STDEXT_CC_THISCALL
            // is not present in "F" however as is normally the case (so either "F" was
            // declared with no explicit calling convention or it was but not
            // STDEXT_CC_THISCALL, which will normally the case), then the following will
            // effectively just resolve back to "F" (for all intents and purposes).
            //
            // The following summarizes the situation:
            //
            //    Calling convention in "F"   Calling convention in "F AnyClass::*"         "FunctionType_t" (same as "RawType_t" in "F AnyClass::*")  CallingConvention_v
            //    -------------------------   ------------------------------------          ------------------------------------------------------     -------------------
            //    Unspecified                 Thiscall (MSFT), Cdecl (Non-MSFT)             Unspecified (MSFT), Cdecl (Non-MSFT)                       DefaultCallingConvention_v<true> (MSFT), Cdecl (Non-MSFT)
            //    Thiscall                    Thiscall (MSFT), Thiscall (Non-MSFT)          Unspecified (MSFT), Unspecified (Non-MSFT)                 DefaultCallingConvention_v<true> (MSFT), DefaultCallingConvention_v<true> (Non-MSFT)
            //    Non-Thiscall                Non-Thiscall (MSFT), Non-Thiscall (Non-MSFT)  Non-Thiscall (MSFT), Non-Thiscall (Non-MSFT)               Non-Thiscall (MSFT), Non-Thiscall (Non-MSFT)
            //////////////////////////////////////////////////////////////////////////////////
            using FunctionType_t = typename BaseClass::RawType_t;

            ////////////////////////////////////////////////////////
            // Helper variable for above variable (returns it as a
            // WYSIWYG "tstring_view"). Hides base class version.
            ////////////////////////////////////////////////////////
            static constexpr tstring_view FunctionTypeName_v = TypeName_v<FunctionType_t>;

            /////////////////////////////////////////////////////////////////////////////////
            // Calling convention of this abominable function (hides base class version).
            // Note: Testing shows (and research including lengthy discussions with
            // various AI engines FWIW, given their often unreliable accuracy), seems to
            // confirm that given a raw C++ function type "F", then if you do this:
            //
            //     using T = F AnyClass::*;
            //
            // Then on MSFT platforms targeting x86 (not x64 - read on), if the calling
            // convention of "F" matches the compiler's default calling convention currently
            // in effect via the /Gd (__cdecl), /Gr (__fastcall), /Gv (__vectorcall) or /Gz
            // (__stdcall) compiler options (defaults to /Gd), then the calling convention
            // of "T" will be changed to __thiscall, assuming the function isn't variadic
            // since the calling convention is then always __cdecl (even on non-static
            // member functions). This assumes the above compiler options are supported on
            // the target platform being compiled for, in particular on x64 as noted above
            // (though there are others), where the default calling convention for
            // non-static member functions isn't __thiscall but __cdecl so that's what "T"
            // will end up with instead).
            //
            // For instance, if /Gd (__cdecl) is currently in effect (the default), then
            // given this on x86:
            //
            //     using F = int (float);
            //     using T = F AnyClass::*;
            //
            // The calling convention of "F" will be __cdecl based on the /Gd compiler
            // option and since it matches the /Gd calling convention currently in effect,
            // the calling convention of "T" will be changed to __thiscall (and it wouldn't
            // matter if we explicitly declared "F" above with __cdecl instead of relying on
            // the /Gd default, it still matches the /Gd (__cdecl) option in effect so "T"
            // will still be declared with __thiscall unless we're targeting x64 and some
            // others where it will be __cdecl instead).
            //
            // If the calling convention is changed to, say, /Gz (__stdcall) instead
            // however, then the calling convention of "F" will now default to __stdcall
            // instead (again, assuming x86 where it's supported, not x64 or others), and
            // since it matches the default calling convention currently in effect (/Gz),
            // the calling convention of "T" will still be changed to __thiscall (and again,
            // it wouldn't matter if we explicitly declared "F" above with __stdcall instead
            // of relying on the /Gz (__stdcall) option currently in effect, it still
            // matches the /Gz (__stdcall ) setting so "T" will still be declared with
            // __thiscall).
            //
            // While still to set to /Gz (__stdcall) however, if we now do this (explicitly
            // add __cdecl to "F")
            //
            //     using F = int __cdecl (float);
            //     using T = F AnyClass::*;
            //
            // Then the calling convention of "T" will no longer change to __thiscall, but
            // will pick up the explicitly declared __cdecl calling convention in "F",
            // simply because it doesn't match the /Gz (__stdcall) option currently in
            // effect.
            //
            // The upshot is that on MSFT (when targeting x86), when the compiler sees calls
            // like this:
            //
            //    using T = F AnyClass::*;
            //
            // It always changes the calling convention in "F" to __thiscall when creating
            // "T" if the calling convention in "F" matches the calling convention compiler
            // option currently in effect (/Gd, /Gr, /Gz or /Gv). Otherwise, if they don't
            // match then the calling convention of "T" simply picks up the calling
            // convention of "F" itself, as would (intuitively) be expected.
            //
            // Lastly, note that when compiling for x64 instead of x86 (and possibly for
            // other platforms besides x64), only __vectorcall is supported on MSFT
            // platforms (via the /Gv option or when explicitly set on a function), and the
            // other calling conventions have no effect (so in the latter case they
            // effectively just default back to __cdecl). Note that the same applies to
            // "regcall" on Clang and Intel (not available on MSFT or GCC), if explicitly
            // set on a function (noting there's no analogue for /Gd, /Gr, /Gz or /Gv on
            // other compilers - only MSFT has options to control the default calling
            // convention). In these cases __cdecl is the default for non-static member
            // functions, not __thiscall. Note that __thiscall is ignored by the compiler in
            // this case which replaces it with a register-based calling convention as
            // mentioned by MSFT itself at the following link (but for our purposes __cdecl
            // becomes part of the function's raw type unless explicitly overridden with
            // another calling convention instead, but only __vectorcall is supported on
            // MSFT in x64, the others are effectively replaced with __cdecl if you
            // explicitly apply them):
            //
            //    // __thiscall
            //    https://learn.microsoft.com/en-us/cpp/cpp/thiscall?view=msvc-160
            //
            // See MSFT's documentation on calling conventions here:
            //
            //    Argument Passing and Naming Conventions
            //    https://learn.microsoft.com/en-us/cpp/cpp/argument-passing-and-naming-conventions?view=msvc-160
            //
            //    // Calling conventions
            //    https://learn.microsoft.com/en-us/cpp/cpp/calling-conventions?view=msvc-170
            //
            //    // /Gd, /Gr, /Gv, /Gz (Calling Convention)
            //    https://learn.microsoft.com/en-us/cpp/build/reference/gd-gr-gv-gz-calling-convention?view=msvc-170
            //
            // Finally, note that this above behaviour isn't 100% confirmed since it's not
            // officially documented by MSFT anywhere I can find (after extensive
            // searching), but testing and research show it always seems to be the case (and
            // very likely is the case so it should hopefully be stable in theory).
            /////////////////////////////////////////////////////////////////////////////////
            static constexpr CallingConvention CallingConvention_v = (BaseClass::CallingConvention_v != CallingConvention::Thiscall ? BaseClass::CallingConvention_v
                                                                                                                                    : DefaultCallingConvention_v<true>); // Default calling convention for a free function;

            //////////////////////////////////////////////////////////////
            // *** Commented out for now. Can't rely on it due to a
            //     compiler bug in MSVC at this writing (see declaration
            //     of "FreeFunctionRawType" seen in the following call in
            //     "struct MemberFunctionTraits" - ultimately the bug has
            //     something to do with "MemberFunctionRawType_t"). Using
            //     the alternate declaration just above for now.
            //
            // Calling convention of this abominable function (base class
            // override) but commented out for now due to the bug
            // mentioned just above (so again, the alternate declaration
            // just above is used instead as seen).
            //
            // The following technique is arguably more sound however
            // (IMHO) for reasons I won't get into here, but does require
            // the "FreeFunctionRawType" alias from the base class as
            // seen in the following call. That alias exists for this
            // purpose only and is therefore ugly (having to declare it
            // in the base class to support the following) but we'll live
            // with it for now (TODO - revisit this again at some point,
            // there must be a cleaner way).
            //////////////////////////////////////////////////////////////
            //static constexpr auto CallingConvention_v = StdExt::CallingConvention_v<typename BaseClass::FreeFunctionRawType>;

            ////////////////////////////////////////////////////////
            // Helper variable for above variable (returns it as a
            // WYSIWYG "tstring_view"). Hides base class version.
            ////////////////////////////////////////////////////////
            static constexpr tstring_view CallingConventionName_v = CallingConventionToString(CallingConvention_v);

            ///////////////////////////////////////////////////////////
            // Hides base class version (we're an abominable function
            // so no class associated with it, hence we set it to
            // "void")
            ///////////////////////////////////////////////////////////
            using MemberFunctionClass_t = void;

            ////////////////////////////////////////////////////////
            // Helper variable for above variable (returns it as a
            // WYSIWYG "tstring_view") but this is an abominable
            // function so it has no class (only non-static member
            // functions do). We therefore always return an empty
            // string. Note that this member hides the base class
            // version.
            ////////////////////////////////////////////////////////
            static constexpr tstring_view MemberFunctionClassName_v = tstring_view(_T(""));

            // Hides base class version
            static constexpr FunctionClassification FunctionClassification_v = FunctionClassification::Abominable;

            #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
                template <VALID_RETURN_TYPE_C NewReturnTypeT>
                using ReplaceReturnType_t = ToRawType_t<typename BaseClass::template ReplaceReturnType_t<NewReturnTypeT>>;

                template <StdExt::CallingConvention NewCallingConventionT>
                using ReplaceCallingConvention_t = std::conditional_t< NewCallingConventionT != CallingConvention::Thiscall, // Only applies to non-static member functions (we're an abominable function)
                                                                       ToRawType_t< typename BaseClass::template ReplaceCallingConvention_t<NewCallingConventionT> >,
                                                                       FunctionType_t // No change (we don't allow the calling convention of an abominable function to change to STDEXT_CC_THISCALL)
                                                                     >;

                template <IS_CLASS_OR_VOID_C NewClassT>
                using ReplaceMemberFunctionClass_t = typename FreeOrAbominableFunctionWriteTraitsHelper::template ReplaceMemberFunctionClassImpl_t<NewClassT,
                                                                                                                                                   FunctionType_t,
                                                                                                                                                   FunctionType_t // Always same as the raw type (by design)
                                                                                                                                                  >;

                template <VALID_FUNCTION_ARG_TYPE_C... NewArgsT>
                using ReplaceArgs_t = ToRawType_t< typename BaseClass::template ReplaceArgs_t<NewArgsT...> >;

                template <VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT>
                using ReplaceArgsTuple_t = ToRawType_t<typename BaseClass::template ReplaceArgsTuple_t<NewArgsTupleT> >;

                template<std::size_t I /* Zero-based */, std::size_t Count, VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT>
                REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I)
                using ArgsModifyTuple_t = ToRawType_t< typename BaseClass::template ArgsModifyTuple_t<I, Count, NewArgsTupleT> >;

                template<std::size_t I /* Zero-based */, std::size_t Count, VALID_FUNCTION_ARG_TYPE_C... NewArgsT>
                REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I)
                using ArgsModify_t = ToRawType_t< typename BaseClass::template ArgsModify_t<I, Count, NewArgsT...> >;

                template <std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPE_C NewArgT>
                REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_ARG_COUNT(I)
                // Done with this just above (for internal use only) ...
                #undef REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_ARG_COUNT
                using ReplaceArg_t = ToRawType_t< typename BaseClass::template ReplaceArg_t<I, NewArgT> >;

                template<std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToInsertTupleT>
                REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I)
                using ArgsInsertTuple_t = ToRawType_t< typename BaseClass::template ArgsInsertTuple_t<I, ArgsToInsertTupleT> >;

                template<std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPE_C... ArgsToInsertT>
                REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I)
                using ArgsInsert_t = ToRawType_t< typename BaseClass::template ArgsInsert_t<I, ArgsToInsertT...> >;

                template<VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToAppendTupleT>
                using ArgsAppendTuple_t = ToRawType_t< typename BaseClass::template ArgsAppendTuple_t<ArgsToAppendTupleT> >;

                template<VALID_FUNCTION_ARG_TYPE_C... ArgsToAppendT>
                using ArgsAppend_t = ToRawType_t< typename BaseClass::template ArgsAppend_t<ArgsToAppendT...> >;

                template<std::size_t I = 0 /* Zero-based */, std::size_t Count = size_t_npos>
                REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(I)
                // Done with this just above (for internal use only) ...
                #undef REQUIRES_FUNCTION_TRAITS_BASE_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT
                using ArgsDelete_t = ToRawType_t< typename BaseClass::template ArgsDelete_t<I, Count> >;

                using ArgsMakeVoid_t = ToRawType_t<typename BaseClass::ArgsMakeVoid_t>;

                using AddVariadicArgs_t = ToRawType_t<typename BaseClass::AddVariadicArgs_t>;
                using RemoveVariadicArgs_t = ToRawType_t<typename BaseClass::RemoveVariadicArgs_t>;

                using AddConst_t = ToRawType_t<typename BaseClass::AddConst_t>;
                using RemoveConst_t = ToRawType_t<typename BaseClass::RemoveConst_t>;

                using AddVolatile_t = ToRawType_t<typename BaseClass::AddVolatile_t>;
                using RemoveVolatile_t = ToRawType_t<typename BaseClass::RemoveVolatile_t>;

                using AddCV_t = ToRawType_t<typename BaseClass::AddCV_t>;
                using RemoveCV_t = ToRawType_t<typename BaseClass::RemoveCV_t>;

                using AddLValueReference_t = ToRawType_t<typename BaseClass::AddLValueReference_t>;
                using AddRValueReference_t = ToRawType_t<typename BaseClass::AddRValueReference_t>;
                using RemoveReference_t = ToRawType_t<typename BaseClass::RemoveReference_t>;

                using AddNoexcept_t = ToRawType_t<typename BaseClass::AddNoexcept_t>;
                using RemoveNoexcept_t = ToRawType_t<typename BaseClass::RemoveNoexcept_t>;
            #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
        }; // struct AbominableFunctionTraits
    } // namespace Private

    // "FunctionTraits" partial specialization for abominable functions
    template <typename F>
    struct FunctionTraits<F,
                          std::enable_if_t< IsAbominableFunction_v<F> > // True if "F" is an abominable function
                         > :
        Private::AbominableFunctionTraits<F>
    {
    };

    //////////////////////////////////////////////////////
    // Restore warnings we disabled earlier (for GCC see
    // earlier call to "#pragma GCC diagnostic push")
    //////////////////////////////////////////////////////
    #if defined(GCC_COMPILER)
        #pragma GCC diagnostic pop
    //////////////////////////////////////////////////////////
    // Again, restore warnings we disabled earlier (for both
    // Clang and Intel see earlier call to "#pragma clang
    // diagnostic push", noting that Intel (DPC++/C++) is
    // based on Clang so same calls handle both)
    //////////////////////////////////////////////////////////
    #elif defined(CLANG_COMPILER) || \
          defined(INTEL_COMPILER)
        #pragma clang diagnostic pop
    #endif

    ///////////////////////////////////////////////////////////////////
    // IsFunctionTraitsSpecialization_v. Returns true if "T" is a
    // specialization of struct "FunctionTraits" or false otherwise.
    // Note that "FunctionTraits" derivatives always return false
    // since in the general case, types derived from a specialization
    // don't qualify as the specialization itself, they only inherit
    // from it (subtle but meaningful difference). "FunctionTraits"
    // derivatives aren't normally valid in C++ anyway, since the
    // "FunctionTraits" destructor isn't virtual so developers
    // shouldn't normally derive from it (this library never does).
    ///////////////////////////////////////////////////////////////////
    template <typename T>
    inline constexpr bool IsFunctionTraitsSpecialization_v = IsSpecialization_v<T, FunctionTraits>;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    //////////////////////////////////////////////
    // Concept for above template (see following
    // #defined constant for details)
    //////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T>
            concept FunctionTraits_c = IsFunctionTraitsSpecialization_v<T>;
        #endif

        #define FUNCTION_TRAITS_C StdExt::FunctionTraits_c
        #define STATIC_ASSERT_IS_FUNCTION_TRAITS(T)
    #else
        #define FUNCTION_TRAITS_C typename
        #define STATIC_ASSERT_IS_FUNCTION_TRAITS(T) static_assert(StdExt::IsFunctionTraitsSpecialization_v<T>, \
                                                                  QUOTE_STRINGIFY(T) " must be a " \
                                                                  "\"FunctionTraits\" specialization");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsArgCount_v. Thin wrapper around
    // "FunctionTraits::ArgCount_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#argcount_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr std::size_t FunctionTraitsArgCount_v = FunctionTraitsT::ArgCount_v;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

//////////////////////////////////////////////////////////////////////////
// REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_ARG_COUNT
//////////////////////////////////////////////////////////////////////////
#if defined(USE_CONCEPTS)
    #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
        template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */>
        inline constexpr bool FunctionTraitsIndexLessThanArgCount_v = FunctionTraitsT::template IndexLessThanArgCount_v<I>;

        template <typename FunctionTraitsT, std::size_t I /* Zero-based */>
        concept FunctionTraitsIndexLessThanArgCount_c = FunctionTraitsIndexLessThanArgCount_v<FunctionTraitsT, I>;
    #endif

    #define REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_ARG_COUNT(FUNCTION_TRAITS_T, I) requires StdExt::FunctionTraitsIndexLessThanArgCount_c<FUNCTION_TRAITS_T, (I)>
#else
    #define REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_ARG_COUNT(FUNCTION_TRAITS_T, I)
#endif // #if defined(USE_CONCEPTS)

//////////////////////////////////////////////////////////////////////////
// REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT
//////////////////////////////////////////////////////////////////////////
#if defined(USE_CONCEPTS)
    #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
        template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */>
        inline constexpr bool FunctionTraitsIndexLessThanOrEqualToArgCount_v = FunctionTraitsT::template IndexLessThanOrEqualToArgCount_v<I>;

        template <typename FunctionTraitsT, std::size_t I /* Zero-based */>
        concept FunctionTraitsIndexLessThanOrEqualToArgCount_c = FunctionTraitsIndexLessThanOrEqualToArgCount_v<FunctionTraitsT, I>;
    #endif

    #define REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(FUNCTION_TRAITS_T, I) requires StdExt::FunctionTraitsIndexLessThanOrEqualToArgCount_c<FUNCTION_TRAITS_T, (I)>
#else
    #define REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(FUNCTION_TRAITS_T, I)
#endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIndexLessThanArgCountOrReturnVoid_v
    //////////////////////////////////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, bool ReturnVoidIfInvalidIndex>
        inline constexpr bool FunctionTraitsIndexLessThanArgCountOrReturnVoid_v = FunctionTraitsT::template IndexLessThanArgCountOrReturnVoid_v<I, ReturnVoidIfInvalidIndex>;
    #endif // #if defined(USE_CONCEPTS)

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsArgType_t. Thin wrapper around
    // "FunctionTraits::ArgType_t".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#argtype_t
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, bool ReturnVoidIfInvalidIndex = false>
    #if defined(USE_CONCEPTS)
        requires FunctionTraitsIndexLessThanArgCountOrReturnVoid_v<FunctionTraitsT, I, ReturnVoidIfInvalidIndex>
    #endif
    using FunctionTraitsArgType_t = typename FunctionTraitsT::template ArgType_t<I, ReturnVoidIfInvalidIndex>;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIndexLessThanArgCountOrReturnEmptyString_v
    //////////////////////////////////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, bool ReturnEmptyStringIfInvalidIndex>
        inline constexpr bool FunctionTraitsIndexLessThanArgCountOrReturnEmptyString_v = FunctionTraitsT::template IndexLessThanArgCountOrReturnEmptyString_v<I, ReturnEmptyStringIfInvalidIndex>;
    #endif // #if defined(USE_CONCEPTS)

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsArgTypeName_v. Thin wrapper around
    // "FunctionTraits::ArgTypeName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#argtypename_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, bool ReturnEmptyStringIfInvalidIndex = false>
    #if defined(USE_CONCEPTS)
        requires FunctionTraitsIndexLessThanArgCountOrReturnEmptyString_v<FunctionTraitsT, I, ReturnEmptyStringIfInvalidIndex>
    #endif
    inline constexpr tstring_view FunctionTraitsArgTypeName_v = FunctionTraitsT::template ArgTypeName_v<I, ReturnEmptyStringIfInvalidIndex>;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsArgTypes_t. Thin wrapper around
    // "FunctionTraits::ArgTypes_t".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#argtypes_t
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    using FunctionTraitsArgTypes_t = typename FunctionTraitsT::ArgTypes_t;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsCallingConvention_v. Thin wrapper around
    // "FunctionTraits::CallingConvention_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#callingconvention_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr CallingConvention FunctionTraitsCallingConvention_v = FunctionTraitsT::CallingConvention_v;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsCallingConventionName_v. Thin wrapper around
    // "FunctionTraits::CallingConventionName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#callingconventionname_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr tstring_view FunctionTraitsCallingConventionName_v = FunctionTraitsT::CallingConventionName_v;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsForEachArg. Thin wrapper around
    // "FunctionTraits::ForEachArg".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#foreacharg
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT,
              FOR_EACH_ARG_FUNCTOR_C ForEachArgFunctorT>
    inline constexpr bool FunctionTraitsForEachArg(ForEachArgFunctorT&& forEachArgFunctor)
    {
        return FunctionTraitsT::ForEachArg(std::forward<ForEachArgFunctorT>(forEachArgFunctor));
    }

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsFunctionClassification_v. Thin wrapper around
    // "FunctionTraits::FunctionClassification_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionclassification_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr FunctionClassification FunctionTraitsFunctionClassification_v = FunctionTraitsT::FunctionClassification_v;

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    [[deprecated("This template was replaced by \"FunctionTraitsFunctionClassification_v\". The old name continues to be supported for now (it just defers to the new name), but may be removed in a future release (users should update their code to invoke the new name, checking the result for \"FunctionClassification::NonStaticMember\")")]]
    inline constexpr bool FunctionTraitsIsMemberFunction_v = (FunctionTraitsFunctionClassification_v<FunctionTraitsT> == FunctionClassification::NonStaticMember); // Just defer to new name above
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsFunctionOrigin_v. Thin wrapper around
    // "FunctionTraits::FunctionOrigin_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionorigin_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr FunctionOrigin FunctionTraitsFunctionOrigin_v = FunctionTraitsT::FunctionOrigin_v;

#if defined(STDEXT_SUPPORT_DEPRECATED)
    ///////////////////////////////////////////////////////////
    // Replaced by above template (just defers to it), and we
    // warn user below
    ///////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    [[deprecated("This template was replaced by \"FunctionTraitsFunctionOrigin_v\". The old name continues to be supported for now (it just defers to the new name), but may be removed in a future release (users should update their code to invoke the new name, checking the result for \"FunctionOrigin::Functor\" or \"FunctionOrigin::StaticFunctor\", usually the former)")]]
    inline constexpr bool FunctionTraitsIsFunctor_v = (FunctionTraitsFunctionOrigin_v<FunctionTraitsT> == FunctionOrigin::Functor);
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsFunctionRawType_t. Thin wrapper around
    // "FunctionTraits::RawType_t".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionrawtype_t
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    using FunctionTraitsFunctionRawType_t = typename FunctionTraitsT::RawType_t;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsFunctionRawTypeName_v. Thin wrapper around
    // "FunctionTraits::RawTypeName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionrawtypename_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr tstring_view FunctionTraitsFunctionRawTypeName_v = FunctionTraitsT::RawTypeName_v;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsFunctionType_t. Thin wrapper around
    // "FunctionTraits::FunctionType_t".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functiontype_t
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    using FunctionTraitsFunctionType_t = typename FunctionTraitsT::FunctionType_t;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsFunctionTypeName_v. Thin wrapper around
    // "FunctionTraits::FunctionTypeName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functiontypename_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr tstring_view FunctionTraitsFunctionTypeName_v = FunctionTraitsT::FunctionTypeName_v;

    ////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v
    ////////////////////////////////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        template <FUNCTION_TRAITS_C FunctionTraits1,
                  FUNCTION_TRAITS_C FunctionTraits2,
                  std::size_t I /* Zero-based */,
                  bool ReturnFalseIfOneHasArgOtherDoesnt>
        inline constexpr bool FunctionTraitsIndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v = FunctionTraits1::template IndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v<FunctionTraits2,
                                                                                                                                                                                    I,
                                                                                                                                                                                    ReturnFalseIfOneHasArgOtherDoesnt>;
    #endif // #if defined(USE_CONCEPTS)

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsArgListEmpty_v. Thin wrapper around
    // "FunctionTraits::IsArgListEmpty_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isarglistempty_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr bool FunctionTraitsIsArgListEmpty_v = FunctionTraitsT::IsArgListEmpty_v;

    #if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsIsArgListEmpty_v)
        inline constexpr bool FunctionTraitsIsEmptyArgList_v = FunctionTraitsIsArgListEmpty_v<FunctionTraitsT>;
    #endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsArgTypeMatch_v. Thin wrapper around
    // "FunctionTraits::IsArgTypeMatch_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isargtypematch_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraits1, FUNCTION_TRAITS_C FunctionTraits2, std::size_t I /* Zero-based */, bool ReturnFalseIfOneHasArgOtherDoesnt = true>
    #if defined(USE_CONCEPTS)
        requires FunctionTraitsIndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v<FunctionTraits1, FunctionTraits2, I, ReturnFalseIfOneHasArgOtherDoesnt>
    #endif
    inline constexpr bool FunctionTraitsIsArgTypeMatch_v = FunctionTraits1::template IsArgTypeMatch_v<FunctionTraits2, I, ReturnFalseIfOneHasArgOtherDoesnt>;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsArgTypesMatch_v. Thin wrapper around
    // "FunctionTraits::IsArgTypesMatch_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isargtypesmatch_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraits1, FUNCTION_TRAITS_C FunctionTraits2, bool IgnoreVariadic = false>
    inline constexpr bool FunctionTraitsIsArgTypesMatch_v = FunctionTraits1::template IsArgTypesMatch_v<FunctionTraits2, IgnoreVariadic>;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIndexLessThanArgCountOrCompareWithVoid_v
    //////////////////////////////////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, bool CompareWithVoidIfInvalidIndex>
        inline constexpr bool FunctionTraitsIndexLessThanArgCountOrCompareWithVoid_v = FunctionTraitsT::template IndexLessThanArgCountOrCompareWithVoid_v<I, CompareWithVoidIfInvalidIndex>;
    #endif // #if defined(USE_CONCEPTS)

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsArgTypeSame_v. Thin wrapper around
    // "FunctionTraits::IsArgTypeSame_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isargtypesame_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, typename T, bool CompareWithVoidIfInvalidIndex = false>
    #if defined(USE_CONCEPTS)
        requires FunctionTraitsIndexLessThanArgCountOrCompareWithVoid_v<FunctionTraitsT, I, CompareWithVoidIfInvalidIndex>
    #endif
    inline constexpr bool FunctionTraitsIsArgTypeSame_v = FunctionTraitsT::template IsArgTypeSame_v<I, T, CompareWithVoidIfInvalidIndex>;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsArgTypesSame_v. Thin wrapper around
    // "FunctionTraits::IsArgTypesSame_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isargtypessame_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT, AndVariadic AndVariadicT, typename... ArgTypesToCompareT>
    inline constexpr bool FunctionTraitsIsArgTypesSame_v = FunctionTraitsT::template IsArgTypesSame_v<AndVariadicT, ArgTypesToCompareT...>;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsArgTypesSameTuple_v. Thin wrapper around
    // "FunctionTraits::IsArgTypesSameTuple_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isargtypessametuple_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT, TUPLE_C ArgTypesToCompareTupleT, AndVariadic AndVariadicT = AndVariadic::IsFalse>
    inline constexpr bool FunctionTraitsIsArgTypesSameTuple_v = FunctionTraitsT::template IsArgTypesSameTuple_v<ArgTypesToCompareTupleT, AndVariadicT>;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsFunctionConst_v. Thin wrapper around
    // "FunctionTraits::IsConst_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isfunctionconst_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr bool FunctionTraitsIsFunctionConst_v = FunctionTraitsT::IsConst_v;

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsIsFunctionConst_v)
    inline constexpr bool FunctionTraitsIsMemberFunctionConst_v = FunctionTraitsIsFunctionConst_v<FunctionTraitsT>; // Just defer to new name above
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsFunctionVolatile_v. Thin wrapper around
    // "FunctionTraits::IsVolatile_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isfunctionvolatile_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr bool FunctionTraitsIsFunctionVolatile_v = FunctionTraitsT::IsVolatile_v;

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsIsFunctionVolatile_v)
    inline constexpr bool FunctionTraitsIsMemberFunctionVolatile_v = FunctionTraitsIsFunctionVolatile_v<FunctionTraitsT>; // Just defer to new name above
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsFunctionReference_v. Thin wrapper around
    // "FunctionTraits::FunctionReference_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionreference_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr FunctionReference FunctionTraitsFunctionReference_v = FunctionTraitsT::FunctionReference_v;

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    [[deprecated("Template was renamed to \"FunctionTraitsFunctionReference_v\" (and its return type renamed to \"FunctionReference\"). The old names continue to be supported for now (they just defers to the new names), but may be removed in a future release (users should update their code to invoke the new names)")]]
    inline constexpr RefQualifier FunctionTraitsMemberFunctionRefQualifier_v = static_cast<RefQualifier>(FunctionTraitsFunctionReference_v<FunctionTraitsT>); // Just defer to new name above (and cast return value to the new name as well)
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsFunctionReferenceName_v. Thin wrapper around
    // "FunctionTraits::FunctionReferenceName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionreferencename_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT, bool UseAmpersands = true>
    inline constexpr tstring_view FunctionTraitsFunctionReferenceName_v = FunctionTraitsT::template FunctionReferenceName_v<UseAmpersands>;

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT, bool UseAmpersands = true>
    TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionReferenceName_v)
    inline constexpr tstring_view FunctionTraitsMemberFunctionRefQualifierName_v = FunctionTraitsFunctionReferenceName_v<FunctionTraitsT, UseAmpersands>; // Just defer to new name above
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsNoexcept_v. Thin wrapper around
    // "FunctionTraits::IsNoexcept_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isnoexcept_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr bool FunctionTraitsIsNoexcept_v = FunctionTraitsT::IsNoexcept_v;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsReturnTypeMatch_v. Thin wrapper around
    // "FunctionTraits::IsReturnTypeMatch_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isreturntypematch_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraits1, FUNCTION_TRAITS_C FunctionTraits2>
    inline constexpr bool FunctionTraitsIsReturnTypeMatch_v = FunctionTraits1::template IsReturnTypeMatch_v<FunctionTraits2>;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsReturnTypeSame_v. Thin wrapper around
    // "FunctionTraits::IsReturnTypeSame_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isreturntypesame_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT, typename T>
    inline constexpr bool FunctionTraitsIsReturnTypeSame_v = FunctionTraitsT::template IsReturnTypeSame_v<T>;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsReturnTypeVoid_v. Thin wrapper around
    // "FunctionTraits::IsReturnTypeVoid_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isreturntypevoid_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr bool FunctionTraitsIsReturnTypeVoid_v = FunctionTraitsT::IsReturnTypeVoid_v;

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsIsReturnTypeVoid_v)
    inline constexpr bool FunctionTraitsIsVoidReturnType_v = FunctionTraitsIsReturnTypeVoid_v<FunctionTraitsT>;
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsIsVariadic_v. Thin wrapper around
    // "FunctionTraits::IsVariadic_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isvariadic_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr bool FunctionTraitsIsVariadic_v = FunctionTraitsT::IsVariadic_v;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsMemberFunctionClass_t. Thin wrapper around
    // "FunctionTraits::MemberFunctionClass_t".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#memberfunctionclass_t
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    using FunctionTraitsMemberFunctionClass_t = typename FunctionTraitsT::MemberFunctionClass_t;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsMemberFunctionClassName_v. Thin wrapper around
    // "FunctionTraits::MemberFunctionClassName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#memberfunctionclassname_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr tstring_view FunctionTraitsMemberFunctionClassName_v = FunctionTraitsT::MemberFunctionClassName_v;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsReturnType_t. Thin wrapper around
    // "FunctionTraits::ReturnType_t".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#returntype_t
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    using FunctionTraitsReturnType_t = typename FunctionTraitsT::ReturnType_t;

    //////////////////////////////////////////////////////////////////////////
    // FunctionTraitsReturnTypeName_v. Thin wrapper around
    // "FunctionTraits::ReturnTypeName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#returntypename_v
    //////////////////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    inline constexpr tstring_view FunctionTraitsReturnTypeName_v = FunctionTraitsT::ReturnTypeName_v;

    namespace Private
    {
        ////////////////////////////////////////////////////////////
        // TODO - Declared in our private namespace for now so not
        // intended for end users but should be made available to
        // them in a future release. If so then a
        // NON_STATIC_OR_STATIC_FUNCTOR_TRAITS_C concept (name TBD)
        // should be created and used to replace FUNCTION_TRAITS_C
        // just below.
        ////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraits>
        using FunctionTraitsRawFunctorType_t = typename FunctionTraits::RawFunctorType_t;

        // TODO - See comments just above
        template <FUNCTION_TRAITS_C FunctionTraits>
        inline constexpr tstring_view FunctionTraitsRawFunctorTypeName_v = FunctionTraits::RawFunctorTypeName_v;
    } // namespace Private

    /////////////////////////////////////////////////////
    // See this constant for details. Following helper
    // templates for function write traits are declared
    // only if function write traits are supported.
    // They're preprocessed out otherwise (so function
    // write traits simply won't be available).
    /////////////////////////////////////////////////////
    #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsAddNoexcept_t. Thin wrapper around
        // "FunctionTraits::AddNoexcept_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#addnoexcept_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsAddNoexcept_t = typename FunctionTraitsT::AddNoexcept_t;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsAddVariadicArgs_t. Thin wrapper around
        // "FunctionTraits::AddVariadicArgs_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#addvariadicargs_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsAddVariadicArgs_t = typename FunctionTraitsT::AddVariadicArgs_t;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsFunctionAddConst_t. Thin wrapper around
        // "FunctionTraits::AddConst_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionaddconst_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsFunctionAddConst_t = typename FunctionTraitsT::AddConst_t;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMemberFunctionAddConst_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionAddConst_t) = FunctionTraitsFunctionAddConst_t<FunctionTraitsT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsFunctionRemoveConst_t. Thin wrapper around
        // "FunctionTraits::RemoveConst_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionremoveconst_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsFunctionRemoveConst_t = typename FunctionTraitsT::RemoveConst_t;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMemberFunctionRemoveConst_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionRemoveConst_t) = FunctionTraitsFunctionRemoveConst_t<FunctionTraitsT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsFunctionAddVolatile_t. Thin wrapper around
        // "FunctionTraits::AddVolatile_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionaddvolatile_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsFunctionAddVolatile_t = typename FunctionTraitsT::AddVolatile_t;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMemberFunctionAddVolatile_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionAddVolatile_t) = FunctionTraitsFunctionAddVolatile_t<FunctionTraitsT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsFunctionRemoveVolatile_t. Thin wrapper around
        // "FunctionTraits::RemoveVolatile_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionremovevolatile_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsFunctionRemoveVolatile_t = typename FunctionTraitsT::RemoveVolatile_t;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMemberFunctionRemoveVolatile_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionRemoveVolatile_t) = FunctionTraitsFunctionRemoveVolatile_t<FunctionTraitsT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsFunctionAddCV_t. Thin wrapper around
        // "FunctionTraits::AddCV_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionaddcv_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsFunctionAddCV_t = typename FunctionTraitsT::AddCV_t;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMemberFunctionAddCV_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionAddCV_t) = FunctionTraitsFunctionAddCV_t<FunctionTraitsT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsFunctionRemoveCV_t. Thin wrapper around
        // "FunctionTraits::RemoveCV_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionremovecv_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsFunctionRemoveCV_t = typename FunctionTraitsT::RemoveCV_t;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMemberFunctionRemoveCV_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionRemoveCV_t) = FunctionTraitsFunctionRemoveCV_t<FunctionTraitsT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsFunctionAddLValueReference_t. Thin wrapper around
        // "FunctionTraits::AddLValueReference_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionaddlvaluereference_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsFunctionAddLValueReference_t = typename FunctionTraitsT::AddLValueReference_t;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMemberFunctionAddLValueReference_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionAddLValueReference_t) = FunctionTraitsFunctionAddLValueReference_t<FunctionTraitsT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsFunctionAddRValueReference_t. Thin wrapper around
        // "FunctionTraits::AddRValueReference_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionaddrvaluereference_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsFunctionAddRValueReference_t = typename FunctionTraitsT::AddRValueReference_t;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMemberFunctionAddRValueReference_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionAddRValueReference_t) = FunctionTraitsFunctionAddRValueReference_t<FunctionTraitsT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsFunctionRemoveReference_t. Thin wrapper around
        // "FunctionTraits::RemoveReference_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionremovereference_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsFunctionRemoveReference_t = typename FunctionTraitsT::RemoveReference_t;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMemberFunctionRemoveReference_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsFunctionRemoveReference_t) = FunctionTraitsFunctionRemoveReference_t<FunctionTraitsT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsReplaceMemberFunctionClass_t. Thin wrapper around
        // "FunctionTraits::ReplaceMemberFunctionClass_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replacememberfunctionclass_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT, IS_CLASS_OR_VOID_C NewClassT>
        using FunctionTraitsReplaceMemberFunctionClass_t = typename FunctionTraitsT::template ReplaceMemberFunctionClass_t<NewClassT>;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT, IS_CLASS_OR_VOID_C NewClassT>
        using FunctionTraitsMemberFunctionReplaceClass_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsReplaceMemberFunctionClass_t) = FunctionTraitsReplaceMemberFunctionClass_t<FunctionTraitsT, NewClassT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsRemoveNoexcept_t. Thin wrapper around
        // "FunctionTraits::RemoveNoexcept_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#removenoexcept_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsRemoveNoexcept_t = typename FunctionTraitsT::RemoveNoexcept_t;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsRemoveVariadicArgs_t. Thin wrapper around
        // "FunctionTraits::RemoveVariadicArgs_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#removevariadicargs_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsRemoveVariadicArgs_t = typename FunctionTraitsT::RemoveVariadicArgs_t;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsReplaceArgs_t. Thin wrapper around
        // "FunctionTraits::ReplaceArgs_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replaceargs_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT, VALID_FUNCTION_ARG_TYPE_C... NewArgsT>
        using FunctionTraitsReplaceArgs_t = typename FunctionTraitsT::template ReplaceArgs_t<NewArgsT...>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsReplaceArgsTuple_t. Thin wrapper around
        // "FunctionTraits::ReplaceArgsTuple_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replaceargstuple_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT, VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT>
        using FunctionTraitsReplaceArgsTuple_t = typename FunctionTraitsT::template ReplaceArgsTuple_t<NewArgsTupleT>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsReplaceCallingConvention_t. Thin wrapper around
        // "FunctionTraits::ReplaceCallingConvention_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replacecallingconvention_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT, CallingConvention NewCallingConventionT>
        using FunctionTraitsReplaceCallingConvention_t = typename FunctionTraitsT::template ReplaceCallingConvention_t<NewCallingConventionT>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsArgsModify_t. Thin wrapper around
        // "FunctionTraits::ArgsModify_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsmodify_t
        //////////////////////////////////////////////////////////////////////////
        template<FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, std::size_t Count, VALID_FUNCTION_ARG_TYPE_C... NewArgsT>
        REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(FunctionTraitsT, I)
        using FunctionTraitsArgsModify_t = typename FunctionTraitsT::template ArgsModify_t<I, Count, NewArgsT...>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsArgsModifyTuple_t. Thin wrapper around
        // "FunctionTraits::ArgsModifyTuple_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsmodifytuple_t
        //////////////////////////////////////////////////////////////////////////
        template<FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, std::size_t Count, VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT>
        REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(FunctionTraitsT, I)
        using FunctionTraitsArgsModifyTuple_t = typename FunctionTraitsT::template ArgsModifyTuple_t<I, Count, NewArgsTupleT>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsArgsInsert_t. Thin wrapper around
        // "FunctionTraits::ArgsInsert_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsinsert_t
        //////////////////////////////////////////////////////////////////////////
        template<FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPE_C... ArgsToInsertT>
        REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(FunctionTraitsT, I)
        using FunctionTraitsArgsInsert_t = typename FunctionTraitsT::template ArgsInsert_t<I, ArgsToInsertT...>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsArgsInsertTuple_t. Thin wrapper around
        // "FunctionTraits::ArgsInsertTuple_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsinserttuple_t
        //////////////////////////////////////////////////////////////////////////
        template<FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I, VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToInsertTupleT>
        REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(FunctionTraitsT, I)
        using FunctionTraitsArgsInsertTuple_t = typename FunctionTraitsT::template ArgsInsertTuple_t<I, ArgsToInsertTupleT>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsArgsMakeVoid_t. Thin wrapper around
        // "FunctionTraits::ArgsMakeVoid_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsmakevoid_t
        //////////////////////////////////////////////////////////////////////////
        template<FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsArgsMakeVoid_t = typename FunctionTraitsT::ArgsMakeVoid_t;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsArgsAppend_t. Thin wrapper around
        // "FunctionTraits::ArgsAppend_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsappend_t
        //////////////////////////////////////////////////////////////////////////
        template<FUNCTION_TRAITS_C FunctionTraitsT, VALID_FUNCTION_ARG_TYPE_C... ArgsToAppendT>
        using FunctionTraitsArgsAppend_t = typename FunctionTraitsT::template ArgsAppend_t<ArgsToAppendT...>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsArgsAppendTuple_t. Thin wrapper around
        // "FunctionTraits::ArgsAppendTuple_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsappendtuple_t
        //////////////////////////////////////////////////////////////////////////
        template<FUNCTION_TRAITS_C FunctionTraitsT, VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToAppendTupleT>
        using FunctionTraitsArgsAppendTuple_t = typename FunctionTraitsT::template ArgsAppendTuple_t<ArgsToAppendTupleT>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsArgsDelete_t. Thin wrapper around
        // "FunctionTraits::ArgsDelete_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsdelete_t
        //////////////////////////////////////////////////////////////////////////
        template<FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I = 0 /* Zero-based */, std::size_t Count = size_t_npos>
        REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(FunctionTraitsT, I)
        using FunctionTraitsArgsDelete_t = typename FunctionTraitsT::template ArgsDelete_t<I, Count>;

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsReplaceArg_t. Thin wrapper around
        // "FunctionTraits::ReplaceArg_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replacearg_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, VALID_FUNCTION_ARG_TYPE_C NewArgT>
        REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_ARG_COUNT(FunctionTraitsT, I)
        using FunctionTraitsReplaceArg_t = typename FunctionTraitsT::template ReplaceArg_t<I, NewArgT>;

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */, typename NewArgT>
        REQUIRES_FUNCTION_TRAITS_I_LESS_THAN_ARG_COUNT(FunctionTraitsT, I)
        using FunctionTraitsReplaceNthArg_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionTraitsReplaceArg_t) = FunctionTraitsReplaceArg_t<FunctionTraitsT, I, NewArgT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionTraitsReplaceReturnType_t. Thin wrapper around
        // "FunctionTraits::ReplaceReturnType_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replacereturntype_t
        //////////////////////////////////////////////////////////////////////////
        template <FUNCTION_TRAITS_C FunctionTraitsT, VALID_RETURN_TYPE_C NewReturnTypeT>
        using FunctionTraitsReplaceReturnType_t = typename FunctionTraitsT::template ReplaceReturnType_t<NewReturnTypeT>;

        template <IS_CLASS_C T, FUNCTION_TRAITS_C FunctionTraitsT>
        using FunctionTraitsMakeMemberFunctionPtr_t = FunctionTraitsReplaceMemberFunctionClass_t<FunctionTraitsT, T>;
    #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)

    //////////////////////////////////////////////////////////////////////////
    // ArgCount_v. Thin wrapper around
    // "FunctionTraits::ArgCount_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#argcount_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr std::size_t ArgCount_v = FunctionTraitsArgCount_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

//////////////////////////////////////////////////////////////////////////
// REQUIRES_I_LESS_THAN_ARG_COUNT
//////////////////////////////////////////////////////////////////////////
#if defined(USE_CONCEPTS)
    #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
        template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */>
        inline constexpr bool IndexLessThanArgCount_v = FunctionTraitsIndexLessThanArgCount_v<FunctionTraits<F>, I>; // Defers to the "FunctionTraits" version further above

        template <typename F, std::size_t I /* Zero-based */>
        concept IndexLessThanArgCount_c = IndexLessThanArgCount_v<F, I>;
    #endif

    #define REQUIRES_I_LESS_THAN_ARG_COUNT(F, I) requires StdExt::IndexLessThanArgCount_c<F, (I)>
#else
    #define REQUIRES_I_LESS_THAN_ARG_COUNT(F, I)
#endif // #if defined(USE_CONCEPTS)

//////////////////////////////////////////////////////////////////////////
// REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT
//////////////////////////////////////////////////////////////////////////
#if defined(USE_CONCEPTS)
    #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
        template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */>
        inline constexpr bool IndexLessThanOrEqualToArgCount_v = FunctionTraitsIndexLessThanOrEqualToArgCount_v<FunctionTraits<F>, I>; // Defers to the "FunctionTraits" version further above

        template <typename F, std::size_t I /* Zero-based */>
        concept IndexLessThanOrEqualToArgCount_c = IndexLessThanOrEqualToArgCount_v<F, I>;
    #endif

    #define REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I) requires StdExt::IndexLessThanOrEqualToArgCount_c<F, (I)>
#else
    #define REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
#endif // #if defined(USE_CONCEPTS)

//////////////////////////////////////////////////////////////////////////
// REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_VOID
//////////////////////////////////////////////////////////////////////////
#if defined(USE_CONCEPTS)
    #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
        template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */, bool ReturnVoidIfInvalidIndex>
        inline constexpr bool IndexLessThanArgCountOrReturnVoid_v = FunctionTraitsIndexLessThanArgCountOrReturnVoid_v<FunctionTraits<F>, I, ReturnVoidIfInvalidIndex>; // Defers to the "FunctionTraits" version further above
    #endif

    #define REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_VOID(F, I, RETURN_VOID_IF_INVALID_INDEX) requires StdExt::IndexLessThanArgCountOrReturnVoid_v<F, (I), (RETURN_VOID_IF_INVALID_INDEX)>
#else
    #define REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_VOID(F, I, RETURN_VOID_IF_INVALID_INDEX)
#endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    //////////////////////////////////////////////////////////////////////////
    // ArgType_t. Thin wrapper around
    // "FunctionTraits::ArgType_t".
    // See here for details:
    // https://github.com/HexadigmSystems/Functiontraits/#argtype_t
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */, bool ReturnVoidIfInvalidIndex = false>
    REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_VOID(F, I, ReturnVoidIfInvalidIndex)
    using ArgType_t = FunctionTraitsArgType_t<FunctionTraits<F>, I, ReturnVoidIfInvalidIndex>; // Defers to the "FunctionTraits" helper further above
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

//////////////////////////////////////////////////////////////////////////
// REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_EMPTY_STRING
//////////////////////////////////////////////////////////////////////////
#if defined(USE_CONCEPTS)
    #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
        template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */, bool ReturnEmptyStringIfInvalidIndex>
        inline constexpr bool IndexLessThanArgCountOrReturnEmptyString_v = FunctionTraitsIndexLessThanArgCountOrReturnEmptyString_v<FunctionTraits<F>, I, ReturnEmptyStringIfInvalidIndex>; // Defers to the "FunctionTraits" version further above
    #endif

    #define REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_EMPTY_STRING(F, I, RETURN_EMPTY_STRING_IF_INVALID_INDEX) requires StdExt::IndexLessThanArgCountOrReturnEmptyString_v<F, (I), (ReturnEmptyStringIfInvalidIndex)>
#else
    #define REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_EMPTY_STRING(F, I, RETURN_EMPTY_STRING_IF_INVALID_INDEX)
#endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    //////////////////////////////////////////////////////////////////////////
    // ArgTypeName_v. Thin wrapper around
    // "FunctionTraits::ArgTypeName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/Functiontraits/#argtypename_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */, bool ReturnEmptyStringIfInvalidIndex = false>
    REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_EMPTY_STRING(F, I, ReturnEmptyStringIfInvalidIndex)
    inline constexpr tstring_view ArgTypeName_v = FunctionTraitsArgTypeName_v<FunctionTraits<F>, I, ReturnEmptyStringIfInvalidIndex>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // ArgTypes_t. Thin wrapper around
    // "FunctionTraits::ArgTypes_t".
    // See here for details:
    // https://github.com/HexadigmSystems/Functiontraits/#argtypes_t
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    using ArgTypes_t = FunctionTraitsArgTypes_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // CallingConvention_v. Thin wrapper around
    // "FunctionTraits::CallingConvention_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#callingconvention
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr CallingConvention CallingConvention_v = FunctionTraitsCallingConvention_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // CallingConventionName_v. Thin wrapper around
    // "FunctionTraits::CallingConventionName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#callingconventionname_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr tstring_view CallingConventionName_v = FunctionTraitsCallingConventionName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // ForEachArg. Thin wrapper around
    // "FunctionTraits::ForEachArg".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#callingconventionname_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F,
              FOR_EACH_ARG_FUNCTOR_C ForEachArgFunctorT>
    inline constexpr bool ForEachArg(ForEachArgFunctorT&& forEachArgFunctor)
    {
        return FunctionTraitsForEachArg<FunctionTraits<F>>(std::forward<ForEachArgFunctorT>(forEachArgFunctor)); // Defers to the "FunctionTraits" helper further above
    }

    //////////////////////////////////////////////////////////////////////////
    // FunctionClassification_v. Thin wrapper around
    // "FunctionTraits::FunctionClassification_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionclassification_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr FunctionClassification FunctionClassification_v = FunctionTraitsFunctionClassification_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

/////////////////////////////////////////////////////////////////
// Following is deprecated and no longer supported (which we'll
// live with - can't defer to its "FunctionClassification_v"
// replacement above since the old name, "IsFreeFunction_v",
// has been reassigned for other use - code below therefore
// just commented out for - will eventually be removed)
/////////////////////////////////////////////////////////////////
/*
#if defined(STDEXT_SUPPORT_DEPRECATED)
    ///////////////////////////////////////////////////////////////
    // Replaced by "FunctionClassification_v" just above (just
    // defers to it), but we've reused the name "IsFreeFunction_v"
    // for another purpose (albeit similar) so no longer support
    // it here (existing users *must* update their code to use
    // "FunctionClassification_v", hence the following is
    // commented out)
    ///////////////////////////////////////////////////////////////
    template <FUNCTION_TRAITS_C FunctionTraitsT>
    [[deprecated("This template was replaced by \"FunctionClassification_v\". The old name continues to be supported for now (it just defers to the new name), but may be removed in a future release (users should update their code to invoke the new name, checking the result for \"FunctionClassification::Free\")")]]
    inline constexpr bool IsFreeFunction_v = (FunctionClassification_v<F> == FunctionClassification::Free);
#endif
*/

#if defined(STDEXT_SUPPORT_DEPRECATED)
    ///////////////////////////////////////////////////////////
    // Replaced by "FunctionClassification_v" (just defers to
    // it), but we warn user below
    ///////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    [[deprecated("This template was replaced by \"FunctionClassification_v\". The old name continues to be supported for now (it just defers to the new name), but may be removed in a future release (users should update their code to invoke the new name, checking the result for \"FunctionClassification::NonStaticMember\")")]]
    inline constexpr bool IsMemberFunction_v = (FunctionClassification_v<F> == FunctionClassification::NonStaticMember); // Just defer to new name above
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionOrigin_v. Thin wrapper around
    // "FunctionTraits::FunctionOrigin_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionorigin_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr FunctionOrigin FunctionOrigin_v = FunctionTraitsFunctionOrigin_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
    ///////////////////////////////////////////////////////////
    // Replaced by above template (just defers to it), and we
    // warn user below
    ///////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    [[deprecated("This template was replaced by \"FunctionOrigin_v\". The old name continues to be supported for now (it just defers to the new name), but may be removed in a future release (users should update their code to invoke the new name, checking the result for \"FunctionOrigin::Functor\" or \"FunctionOrigin::StaticFunctor\", usually the former)")]]
    inline constexpr bool IsFunctor_v = (FunctionOrigin_v<F> == FunctionOrigin::Functor); // Just defer to new name above
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionRawType_t. Thin wrapper around
    // "FunctionTraits::FunctionRawType".
    // See here for details:
    // https://github.com/HexadigmSystems/Functiontraits/#functionrawtype_t
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    using FunctionRawType_t = FunctionTraitsFunctionRawType_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // FunctionRawTypeName_v. Thin wrapper around
    // "FunctionTraits::FunctionRawTypeName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionrawtypename_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr tstring_view FunctionRawTypeName_v = FunctionTraitsFunctionRawTypeName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // FunctionType_t. Thin wrapper around
    // "FunctionTraits::FunctionType_t".
    // See here for details:
    // https://github.com/HexadigmSystems/Functiontraits/#functiontype_t
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    using FunctionType_t = FunctionTraitsFunctionType_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // FunctionTypeName_v. Thin wrapper around
    // "FunctionTraits::FunctionTypeName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functiontypename_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr tstring_view FunctionTypeName_v = FunctionTraitsFunctionTypeName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

//////////////////////////////////////////////////////////////////////////
// REQUIRES_I_VALID_IN_BOTH_OR_RETURN_FALSE_IF_ONE_HAS_ARG_OTHER_DOESNT
//////////////////////////////////////////////////////////////////////////
#if defined(USE_CONCEPTS)
    #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
        template <TRAITS_FUNCTION_C F1,
                  TRAITS_FUNCTION_C F2,
                  std::size_t I /* Zero-based */,
                  bool ReturnFalseIfOneHasArgOtherDoesnt>
        inline constexpr bool IndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v = FunctionTraitsIndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v<FunctionTraits<F1>,
                                                                                                                                                          FunctionTraits<F2>,
                                                                                                                                                          I,
                                                                                                                                                          ReturnFalseIfOneHasArgOtherDoesnt>;
    #endif // if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    #define REQUIRES_I_VALID_IN_BOTH_OR_RETURN_FALSE_IF_ONE_HAS_ARG_OTHER_DOESNT(F1, \
                                                                                 F2, \
                                                                                 I, \
                                                                                 RETURN_FALSE_IF_ONE_HAS_ARG_OTHER_DOESNT) requires StdExt::IndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v<F1, \
                                                                                                                                                                                                  F2, \
                                                                                                                                                                                                  I, \
                                                                                                                                                                                                  RETURN_FALSE_IF_ONE_HAS_ARG_OTHER_DOESNT>
#else
    #define REQUIRES_I_VALID_IN_BOTH_OR_RETURN_FALSE_IF_ONE_HAS_ARG_OTHER_DOESNT(F1, \
                                                                                 F2, \
                                                                                 I, \
                                                                                 RETURN_FALSE_IF_ONE_HAS_ARG_OTHER_DOESNT)
#endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    //////////////////////////////////////////////////////////////////////////
    // IsArgListEmpty_v. Thin wrapper around
    // "FunctionTraits::IsArgListEmpty_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isarglistempty_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr bool IsArgListEmpty_v = FunctionTraitsIsArgListEmpty_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    #if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(IsArgListEmpty_v)
        inline constexpr bool IsEmptyArgList_v = IsArgListEmpty_v<F>; // Just defer to new name above
    #endif

    //////////////////////////////////////////////////////////////////////////
    // IsArgTypeMatch_v. Thin wrapper around
    // "FunctionTraits::IsArgTypeMatch_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isargtypematch_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F1, TRAITS_FUNCTION_C F2, std::size_t I /* Zero-based */, bool ReturnFalseIfOneHasArgOtherDoesnt = true>
    REQUIRES_I_VALID_IN_BOTH_OR_RETURN_FALSE_IF_ONE_HAS_ARG_OTHER_DOESNT(F1, F2, I, ReturnFalseIfOneHasArgOtherDoesnt)
    inline constexpr bool IsArgTypeMatch_v = FunctionTraitsIsArgTypeMatch_v<FunctionTraits<F1>, FunctionTraits<F2>, I, ReturnFalseIfOneHasArgOtherDoesnt>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // IsArgTypesMatch_v. Thin wrapper around
    // "FunctionTraits::IsArgTypesMatch_v".
    // See here for details:
    // https://github.com/HexadigmSystems.FuntionTraits/#isargtypesmatch_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F1, TRAITS_FUNCTION_C F2, bool IgnoreVariadic = false>
    inline constexpr bool IsArgTypesMatch_v = FunctionTraitsIsArgTypesMatch_v<FunctionTraits<F1>, FunctionTraits<F2>, IgnoreVariadic>; // Defers to the "FunctionTraits" helper further above
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

//////////////////////////////////////////////////////////////////////////
// REQUIRES_I_LESS_THAN_ARG_COUNT_OR_COMPARE_WITH_VOID
//////////////////////////////////////////////////////////////////////////
#if defined(USE_CONCEPTS)
    #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
        template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */, bool CompareWithVoidIfInvalidIndex>
        inline constexpr bool IndexLessThanArgCountOrCompareWithVoid_v = FunctionTraitsIndexLessThanArgCountOrCompareWithVoid_v<FunctionTraits<F>, I, CompareWithVoidIfInvalidIndex>; // Defers to the "FunctionTraits" version further above
    #endif

    #define REQUIRES_I_LESS_THAN_ARG_COUNT_OR_COMPARE_WITH_VOID(F, I, COMPARE_WITH_VOID_IF_INVALID_INDEX) requires StdExt::IndexLessThanArgCountOrCompareWithVoid_v<F, (I), (COMPARE_WITH_VOID_IF_INVALID_INDEX)>
#else
    #define REQUIRES_I_LESS_THAN_ARG_COUNT_OR_COMPARE_WITH_VOID(F, I, COMPARE_WITH_VOID_IF_INVALID_INDEX)
#endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    //////////////////////////////////////////////////////////////////////////
    // IsArgTypeSame_v. Thin wrapper around
    // "FunctionTraits::IsArgTypeSame_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isargtypesame_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */, typename T, bool CompareWithVoidIfInvalidIndex = false>
    REQUIRES_I_LESS_THAN_ARG_COUNT_OR_COMPARE_WITH_VOID(F, I, CompareWithVoidIfInvalidIndex)
    inline constexpr bool IsArgTypeSame_v = FunctionTraitsIsArgTypeSame_v<FunctionTraits<F>, I, T, CompareWithVoidIfInvalidIndex>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // IsArgTypesSame_v. Thin wrapper around
    // "FunctionTraits::IsArgTypesSame_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isargtypessame_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F, AndVariadic AndVariadicT, typename... ArgTypesToCompareT>
    inline constexpr bool IsArgTypesSame_v = FunctionTraitsIsArgTypesSame_v<FunctionTraits<F>, AndVariadicT, ArgTypesToCompareT...>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // IsArgTypesSameTuple_v. Thin wrapper around
    // "FunctionTraits::IsArgTypesSameTuple_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isargtypessametuple_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F, TUPLE_C ArgTypesToCompareTupleT, AndVariadic AndVariadicT = AndVariadic::IsFalse>
    inline constexpr bool IsArgTypesSameTuple_v = FunctionTraitsIsArgTypesSameTuple_v<FunctionTraits<F>, ArgTypesToCompareTupleT, AndVariadicT>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // IsFunctionConst_v. Thin wrapper around
    // "FunctionTraits::IsFunctionConst_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isfunctionconst_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr bool IsFunctionConst_v = FunctionTraitsIsFunctionConst_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(IsFunctionConst_v)
    inline constexpr bool IsMemberFunctionConst_v = IsFunctionConst_v<F>; // Just defer to new name above
#endif

    //////////////////////////////////////////////////////////////////////////
    // IsFunctionVolatile_v. Thin wrapper around
    // "FunctionTraits::IsFunctionVolatile_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isfunctionvolatile_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr bool IsFunctionVolatile_v = FunctionTraitsIsFunctionVolatile_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(IsFunctionVolatile_v)
    inline constexpr bool IsMemberFunctionVolatile_v = IsFunctionVolatile_v<F>; // Just defer to new name above
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionReference_v. Thin wrapper around
    // "FunctionTraits::FunctionReference_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionreference_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr FunctionReference FunctionReference_v = FunctionTraitsFunctionReference_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    [[deprecated("Template was renamed to \"FunctionReference_v\" (and its return type renamed to \"FunctionReference\"). The old names continue to be supported for now (they just defers to the new names), but may be removed in a future release (users should update their code to invoke the new names)")]]
    inline constexpr RefQualifier MemberFunctionRefQualifier_v = static_cast<RefQualifier>(FunctionReference_v<F>); // Just defer to new name above (and cast return value to the new name as well)
#endif

    //////////////////////////////////////////////////////////////////////////
    // FunctionReferenceName_v. Thin wrapper around
    // "FunctionTraits::FunctionReferenceName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#functionreferencename_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F, bool UseAmpersands = true>
    inline constexpr tstring_view FunctionReferenceName_v = FunctionTraitsFunctionReferenceName_v<FunctionTraits<F>, UseAmpersands>;  // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F, bool UseAmpersands = true>
    TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionReferenceName_v)
    inline constexpr tstring_view MemberFunctionRefQualifierName_v = FunctionReferenceName_v<F, UseAmpersands>; // Just defer to new name above
#endif

    //////////////////////////////////////////////////////////////////////////
    // IsNoexcept_v. Thin wrapper around
    // "FunctionTraits::IsNoexcept_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isnoexcept_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr bool IsNoexcept_v = FunctionTraitsIsNoexcept_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // IsReturnTypeMatch_v. Thin wrapper around
    // "FunctionTraits::IsReturnTypeMatch_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isreturntypematch_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F1, TRAITS_FUNCTION_C F2>
    inline constexpr bool IsReturnTypeMatch_v = FunctionTraitsIsReturnTypeMatch_v<FunctionTraits<F1>, FunctionTraits<F2>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // IsReturnTypeSame_v. Thin wrapper around
    // "FunctionTraits::IsReturnTypeSame_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isreturntypesame_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F, typename T>
    inline constexpr bool IsReturnTypeSame_v = FunctionTraitsIsReturnTypeSame_v<FunctionTraits<F>, T>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // IsReturnTypeVoid_v. Thin wrapper around
    // "FunctionTraits::IsReturnTypeVoid_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isreturntypevoid_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr bool IsReturnTypeVoid_v = FunctionTraitsIsReturnTypeVoid_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
    //////////////////////////////////////////////////////////////////
    // Same as above template (just defers to it) but was renamed to
    // the above so following name is deprecated (we warn user below)
    //////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(IsReturnTypeVoid_v)
    inline constexpr bool IsVoidReturnType_v = IsReturnTypeVoid_v<F>;
#endif

    //////////////////////////////////////////////////////////////////////////
    // IsVariadic_v. Thin wrapper around
    // "FunctionTraits::IsVariadic_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#isvariadic_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr bool IsVariadic_v = FunctionTraitsIsVariadic_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // MemberFunctionClass_t. Thin wrapper around
    // "FunctionTraits::MemberFunctionClass_t".
    // See here for details:
    // https://github.com/HexadigmSystems/Functiontraits/#memberfunctionclass_t
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    using MemberFunctionClass_t = FunctionTraitsMemberFunctionClass_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // MemberFunctionClassName_v. Thin wrapper around
    // "FunctionTraits::MemberFunctionClassName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#memberfunctionclassname_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr tstring_view MemberFunctionClassName_v = FunctionTraitsMemberFunctionClassName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // ReturnType_t. Thin wrapper around
    // "FunctionTraits::ReturnType_t".
    // See here for details:
    // https://github.com/HexadigmSystems/Functiontraits/#returntype_t
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    using ReturnType_t = FunctionTraitsReturnType_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    //////////////////////////////////////////////////////////////////////////
    // ReturnTypeName_v. Thin wrapper around
    // "FunctionTraits::ReturnTypeName_v".
    // See here for details:
    // https://github.com/HexadigmSystems/FunctionTraits/#returntypename_v
    //////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    inline constexpr tstring_view ReturnTypeName_v = FunctionTraitsReturnTypeName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

    // For internal use only
    namespace Private
    {
        ////////////////////////////////////////////////////////////
        // TODO - Declared in our private namespace for now so not
        // intended for end users but should be made available to
        // them in a future release.
        ////////////////////////////////////////////////////////////
        template <TRAITS_NON_STATIC_OR_STATIC_FUNCTOR_C F>
        using RawFunctorType_t = FunctionTraitsRawFunctorType_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

        // TODO - See comments just above
        template <TRAITS_NON_STATIC_OR_STATIC_FUNCTOR_C F>
        inline constexpr tstring_view RawFunctorTypeName_v = FunctionTraitsRawFunctorTypeName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above
    } // namespace Private

    /////////////////////////////////////////////////////
    // See this constant for details. Following helper
    // templates for function write traits are declared
    // only if function write traits are supported.
    // They're preprocessed out otherwise (so function
    // write traits simply won't be available).
    /////////////////////////////////////////////////////
    #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
        //////////////////////////////////////////////////////////////////////
        // AddNoexcept_t. Thin wrapper around
        // "FunctionTraits::AddNoexcept_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#addnoexcept_t
        //////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using AddNoexcept_t = FunctionTraitsAddNoexcept_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////////
        // AddVariadicArgs_t. Thin wrapper around
        // "FunctionTraits::AddVariadicArgs_t.
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#addvariadicargs_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using AddVariadicArgs_t = FunctionTraitsAddVariadicArgs_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////////
        // FunctionAddConst_t. Thin wrapper around
        // "FunctionTraits::FunctionAddConst".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits//#functionaddconst_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using FunctionAddConst_t = FunctionTraitsFunctionAddConst_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using MemberFunctionAddConst_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionAddConst_t) = FunctionAddConst_t<F>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionRemoveConst_t. Thin wrapper around
        // "FunctionTraits::FunctionRemoveConst".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionremoveconst_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using FunctionRemoveConst_t = FunctionTraitsFunctionRemoveConst_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using MemberFunctionRemoveConst_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionRemoveConst_t) = FunctionRemoveConst_t<F>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionAddVolatile_t. Thin wrapper around
        // "FunctionTraits::FunctionAddVolatile".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionaddvolatile_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using FunctionAddVolatile_t = FunctionTraitsFunctionAddVolatile_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using MemberFunctionAddVolatile_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionAddVolatile_t) = FunctionAddVolatile_t<F>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionRemoveVolatile_t. Thin wrapper around
        // "FunctionTraits::FunctionRemoveVolatile".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionremovevolatile_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using FunctionRemoveVolatile_t = FunctionTraitsFunctionRemoveVolatile_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using MemberFunctionRemoveVolatile_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionRemoveVolatile_t) = FunctionRemoveVolatile_t<F>; // Just defer to new name above
#endif

        ////////////////////////////////////////////////////////////////////////////
        // FunctionAddCV_t. Thin wrapper around
        // "FunctionTraits::FunctionAddCV".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionaddcv_t
        ////////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using FunctionAddCV_t = FunctionTraitsFunctionAddCV_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using MemberFunctionAddCV_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionAddCV_t) = FunctionAddCV_t<F>; // Just defer to new name above
#endif

        /////////////////////////////////////////////////////////////////////////////////
        // FunctionRemoveCV_t. Thin wrapper around
        // "FunctionTraits::FunctionRemoveCV".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionremovecv_t
        /////////////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using FunctionRemoveCV_t = FunctionTraitsFunctionRemoveCV_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using MemberFunctionRemoveCV_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionRemoveCV_t) = FunctionRemoveCV_t<F>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionAddLValueReference_t. Thin wrapper around
        // "FunctionTraits::FunctionAddLValueReference.
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionaddlvaluereference_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using FunctionAddLValueReference_t = FunctionTraitsFunctionAddLValueReference_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using MemberFunctionAddLValueReference_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionAddLValueReference_t) = FunctionAddLValueReference_t<F>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionAddRValueReference_t. Thin wrapper around
        // "FunctionTraits::FunctionAddRValueReference.
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionaddrvaluereference_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using FunctionAddRValueReference_t = FunctionTraitsFunctionAddRValueReference_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using MemberFunctionAddRValueReference_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionAddRValueReference_t) = FunctionAddRValueReference_t<F>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // FunctionRemoveReference_t. Thin wrapper around
        // "FunctionTraits::FunctionRemoveReference.
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#functionremovereference_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using FunctionRemoveReference_t = FunctionTraitsFunctionRemoveReference_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using MemberFunctionRemoveReference_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(FunctionRemoveReference_t) = FunctionRemoveReference_t<F>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // ReplaceMemberFunctionClass_t. Thin wrapper around
        // "FunctionTraits::ReplaceMemberFunctionClass_t.
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replacememberfunctionclass_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F, IS_CLASS_OR_VOID_C NewClassT>
        using ReplaceMemberFunctionClass_t = FunctionTraitsReplaceMemberFunctionClass_t<FunctionTraits<F>, NewClassT>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F, IS_CLASS_OR_VOID_C NewClassT>
        using MemberFunctionReplaceClass_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(ReplaceMemberFunctionClass_t) = ReplaceMemberFunctionClass_t<F, NewClassT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // RemoveNoexcept_t. Thin wrapper around
        // "FunctionTraits::RemoveNoexcept_t.
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#removenoexcept_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F>
        using RemoveNoexcept_t = FunctionTraitsRemoveNoexcept_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////////
        // RemoveVariadicArgs_t. Thin wrapper around
        // "FunctionTraits::RemoveVariadicArgs_t.
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#removevariadicargs_t
        template <TRAITS_FUNCTION_C F>
        using RemoveVariadicArgs_t = FunctionTraitsRemoveVariadicArgs_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////////
        // ReplaceArgs_t. Thin wrapper around
        // "FunctionTraits::ReplaceArgs_t"
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replaceargs_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F, VALID_FUNCTION_ARG_TYPE_C... NewArgsT>
        using ReplaceArgs_t = FunctionTraitsReplaceArgs_t<FunctionTraits<F>, NewArgsT...>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////////
        // ReplaceArgsTuple_t. Thin wrapper around
        // "FunctionTraits::ReplaceArgsTuple_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replaceargstuple_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F, VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT>
        using ReplaceArgsTuple_t = FunctionTraitsReplaceArgsTuple_t<FunctionTraits<F>, NewArgsTupleT>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////////
        // ReplaceCallingConvention_t. Thin wrapper around
        // "FunctionTraits::ReplaceCallingConvention_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replacecallingconvention_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F, CallingConvention NewCallingConventionT>
        using ReplaceCallingConvention_t = FunctionTraitsReplaceCallingConvention_t<FunctionTraits<F>, NewCallingConventionT>; // Defers to the "FunctionTraits" helper further above

        /////////////////////////////////////////////////////////////////////////////
        // "ArgsModify_t". Thin wrapper around
        // "FunctionTraits::ArgsModify_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsmodify_t
        /////////////////////////////////////////////////////////////////////////////
        template<TRAITS_FUNCTION_C F, std::size_t I, std::size_t Count, VALID_FUNCTION_ARG_TYPE_C... NewArgsT>
        REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
        using ArgsModify_t = FunctionTraitsArgsModify_t<FunctionTraits<F>, I, Count, NewArgsT...>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////////////
        // "ArgsModifyTuple_t". Thin wrapper around
        // "FunctionTraits::ArgsModifyTuple_t"
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsmodifytuple_t
        //////////////////////////////////////////////////////////////////////////////
        template<TRAITS_FUNCTION_C F, std::size_t I, std::size_t Count, VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT>
        REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
        using ArgsModifyTuple_t = FunctionTraitsArgsModifyTuple_t<FunctionTraits<F>, I, Count, NewArgsTupleT>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////
        // "ArgsInsert_t". Thin wrapper around
        // "FunctionTraits::ArgsInsert_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsinsert_t
        //////////////////////////////////////////////////////////////////////
        template<TRAITS_FUNCTION_C F, std::size_t I, VALID_FUNCTION_ARG_TYPE_C... ArgsToInsertT>
        REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
        using ArgsInsert_t = FunctionTraitsArgsInsert_t<FunctionTraits<F>, I, ArgsToInsertT...>; // Defers to the "FunctionTraits" helper further above

        ///////////////////////////////////////////////////////////////////////////////
        // "ArgsInsertTuple_t". Thin wrapper around
        // "FunctionTraits::ArgsInsertTuple_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsinserttuple_t
        ///////////////////////////////////////////////////////////////////////////////
        template<TRAITS_FUNCTION_C F, std::size_t I, VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToInsertTupleT>
        REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
        using ArgsInsertTuple_t = FunctionTraitsArgsInsertTuple_t<FunctionTraits<F>, I, ArgsToInsertTupleT>; // Defers to the "FunctionTraits" helper further above

        ///////////////////////////////////////////////////////////////////////////////
        // "ArgsMakeVoid_t". Thin wrapper around
        // "FunctionTraits::ArgsMakeVoid_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsmakevoid_t
        ///////////////////////////////////////////////////////////////////////////////
        template<TRAITS_FUNCTION_C F>
        using ArgsMakeVoid_t = FunctionTraitsArgsMakeVoid_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////
        // "ArgsAppend_t". Thin wrapper around
        // "FunctionTraits::ArgsAppend_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsappend_t
        //////////////////////////////////////////////////////////////////////
        template<TRAITS_FUNCTION_C F, VALID_FUNCTION_ARG_TYPE_C... ArgsToAppendT>
        using ArgsAppend_t = FunctionTraitsArgsAppend_t<FunctionTraits<F>, ArgsToAppendT...>; // Defers to the "FunctionTraits" helper further above

        ///////////////////////////////////////////////////////////////////////////////
        // "ArgsAppendTuple_t". Thin wrapper around
        // "FunctionTraits::ArgsAppendTuple_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsappendtuple_t
        ///////////////////////////////////////////////////////////////////////////////
        template<TRAITS_FUNCTION_C F, VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToAppendTupleT>
        using ArgsAppendTuple_t = FunctionTraitsArgsAppendTuple_t<FunctionTraits<F>, ArgsToAppendTupleT>; // Defers to the "FunctionTraits" helper further above

        ///////////////////////////////////////////////////////////////////////
        // "ArgsDelete_t". Thin wrapper around
        // "FunctionTraits::ArgsDelete_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#argsdelete_t
        ///////////////////////////////////////////////////////////////////////
        template<TRAITS_FUNCTION_C F, std::size_t I = 0, std::size_t Count = size_t_npos>
        REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
        using ArgsDelete_t = FunctionTraitsArgsDelete_t<FunctionTraits<F>, I, Count>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////////
        // ReplaceArg_t. Thin wrapper around
        // "FunctionTraits::ReplaceArg_t".
        // See here for details:
        // https://github.com/HexadigmSystems/FunctionTraits/#replacearg_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F, std::size_t I, VALID_FUNCTION_ARG_TYPE_C NewArgT>
        REQUIRES_I_LESS_THAN_ARG_COUNT(F, I)
        using ReplaceArg_t = FunctionTraitsReplaceArg_t<FunctionTraits<F>, I, NewArgT>; // Defers to the "FunctionTraits" helper further above

#if defined(STDEXT_SUPPORT_DEPRECATED)
        //////////////////////////////////////////////////////////////////
        // Same as above template (just defers to it) but was renamed to
        // the above so following name is deprecated (we warn user below)
        //////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F, std::size_t N, typename NewArgT>
        REQUIRES_I_LESS_THAN_ARG_COUNT(F, N)
        using ReplaceNthArg_t TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE(ReplaceArg_t) = ReplaceArg_t<F, N, NewArgT>; // Just defer to new name above
#endif

        //////////////////////////////////////////////////////////////////////////
        // ReplaceReturnType_t. Thin wrapper around
        // "FunctionTraits::ReplaceReturnType_t".
        // See here for details:
        // https://github.com/HexadigmSystems/Functiontraits/#replacereturntype_t
        //////////////////////////////////////////////////////////////////////////
        template <TRAITS_FUNCTION_C F, VALID_RETURN_TYPE_C NewReturnTypeT>
        using ReplaceReturnType_t = FunctionTraitsReplaceReturnType_t<FunctionTraits<F>, NewReturnTypeT>; // Defers to the "FunctionTraits" helper further above

        //////////////////////////////////////////////////////////////////////////
        // MakeMemberFunctionPtr_t. A type alias for "F T::*" but ultimately
        // constructed via a call to
        // "FunctionTraits::ReplaceMemberFunctionClass_t". If template arg "F" is
        // a free or abominable function then the latter is ultimately
        // constructed as follows (but read on for a potential issue that exists
        // doing it this way for MSFT x86 apps):
        //
        //     FunctionRawType_t<F> T::*
        //
        // If template arg "F" is a non-static member function however (or a
        // functor so referring to "F::operator()") then its existing class is
        // simply replaced with "T" using an inline technique (so not by using
        // the call above). While the same call above for free or abominable
        // functions could also be used for non-static member functions,
        // synthesizing a non-static member function pointer this way can cause a
        // potential calling convention issue when compiling for x86 on MSFT (but
        // not when compiling for x64, nor on any other compiler since it affects
        // MSFT only). If so and the calling convention of "F" isn't explicitly
        // set (so it implicitly picks up the default calling convention), or it
        // is explicitly set but to the default calling convention (see the /Gd
        // (__cdecl), /Gr (__fastcall), /Gz (__stdcall) or /Gv (__vectorcall)
        // MSVC compiler options - defaults to /Gd (__cdecl)), then the default
        // calling convention in "F" is converted to "__thiscall" by the above
        // call. The default calling convention in "F" is therefore lost (changed
        // to "__thiscall") in the resulting non-static member function pointer
        // (so the function this pointer points to doesn't precisely match "F"
        // anymore - their calling conventions now differ which can be a problem
        // depending on what the resulting "__thiscall" function returned by this
        // template is used for). When template arg "F" is already a non-static
        // member function pointer however (so not a free or abominable function)
        // then the issue doesn't occur since the code ultimately just replaces
        // F's existing class with template arg "T" using an inline technique (so
        // not by using the call seen above). The existing default calling
        // convention in "F" is therefore properly maintained (it's no longer
        // converted to "_thiscall").
        //
        // The upshot is that if targeting x86 then MSFT users of this template
        // may want to pass a non-static member function to this template instead
        // of a free or abominable function, if changing the calling convention
        // to "__thiscall" as just described is going to be a problem (depending
        // on what they're doing with the type returned by this template).
        //////////////////////////////////////////////////////////////////////////
        template <IS_CLASS_C T, TRAITS_FUNCTION_C F>
        using MakeMemberFunctionPtr_t = FunctionTraitsMakeMemberFunctionPtr_t<T, FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above
    #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)

    ///////////////////////////////////////////////////////////////////////
    // Private namespace (for internal use only) used to implement
    // function template "DisplayAllFunctionTraits()" declared just after
    // this namespace. Allows you to display (stream) all traits for a
    // given function to any stream. See "DisplayAllFunctionTraits()"
    // just after this namespace for full details (it just defers to
    // "DisplayAllFunctionTraitsImpl::Process()" in the following
    // namespace).
    ///////////////////////////////////////////////////////////////////////
    namespace Private
    {
        template <typename CharT, typename CharTraitsT>
        class DisplayAllFunctionTraitsImpl
        {
        public:
            // Constructor
            DisplayAllFunctionTraitsImpl(std::basic_ostream<CharT, CharTraitsT>& stream) noexcept
                : m_Stream(stream)
            {
            }

            ////////////////////////////////////////////////////////////////////
            // Implementation function for "StdExt::DisplayAllFunctionTraits()"
            // declared just after this class. See this function for details
            // (it just defers to the following implementation function).
            ////////////////////////////////////////////////////////////////////
            template <typename F>
            std::basic_ostream<CharT, CharTraitsT>& Process() const
            {
                /////////////////////////////////////////////////////////////
                // Lambda for displaying the "#)" seen at the start of each
                // output line (where "#" is the argument # starting at "1)",
                // but not the "#)" seen for each arg in the "Arguments"
                // section of the output, which are later handled by a call
                // to (private member function) "StreamArgTypes()".
                /////////////////////////////////////////////////////////////
                auto streamNumBracket = [this, num = 1]() mutable -> auto&
                {
                    return this->m_Stream << num++ << _T(") ");
                };

                ///////////////////////////////
                // #) Function
                ///////////////////////////////
                streamNumBracket() << _T("Function: ") << FunctionTypeName_v<F> << _T("\n");

                ///////////////////////////////
                // #) Classification
                ///////////////////////////////
                streamNumBracket() << _T("Classification: ");

                //////////////////////////////////////////////////////////
                // Free function which includes static member functions
                // and "std::function" specializations (since the latter
                // are specialized on free functions), but not abominable
                // functions which are handled in the final "else" clause
                // below
                //////////////////////////////////////////////////////////
                if constexpr (FunctionClassification_v<F> == FunctionClassification::Free)
                {
                    m_Stream << _T("Free function");
                    if constexpr (FunctionOrigin_v<F> == FunctionOrigin::StaticFunctor)
                    {
                        m_Stream << _T(" (static \"operator()\" in functor just below)");
                        m_Stream << _T("\n");
                        streamNumBracket() << _T("Class (static functor): \"") <<
                                              RawFunctorTypeName_v<F> <<
                                              _T("\"");
                    }
                    else if constexpr (FunctionOrigin_v<F> == FunctionOrigin::StdFunction)
                    {
                        DisplayFromStdFunctionMsg();
                    }
                    else
                    {
                         // Only other possibility by design at this stage
                        static_assert(FunctionOrigin_v<F> == FunctionOrigin::None);

                        m_Stream << _T(" (includes static member functions)");
                    }
                }
                // Non-static member function? (includes functors)
                else if constexpr (FunctionClassification_v<F> == FunctionClassification::NonStaticMember)
                {
                    m_Stream << _T("Non-static member function");
                    if constexpr (FunctionOrigin_v<F> == FunctionOrigin::Functor)
                    {
                        m_Stream << _T(" (\"operator()\" in functor just below)");
                    }
                    ////////////////////////////////////////////////////////////
                    // Should always be false in theory since "std::function"
                    // can only be legally specialized on a free function which
                    // we handled further above (we're now processing a
                    // non-static member function), so we should never come
                    // through here normally. If a given compiler supports it
                    // though (doesn't choke on a "std::function" specialized
                    // on a non-free function) then we *will* come through here
                    // but it's not legal in C++ (but it's the user's own
                    // doing, not ours, so we'll allow it through - testing
                    // shows some compilers will allow "std::function" to be
                    // specialized on non-free functions but we're simply
                    // displaying what the compiler allows through, not our job
                    // to validate it - in practice it's very unlikely to occur
                    // normally however, i.e., why would a user specialize
                    // "std::function" on something illegal outside of an
                    // SFINAE context anyway, unless by accident).
                    ////////////////////////////////////////////////////////////
                    else if constexpr (FunctionOrigin_v<F> == FunctionOrigin::StdFunction)
                    {
                        DisplayFromStdFunctionMsg();
                    }

                    m_Stream << _T("\n");

                    streamNumBracket() << _T("Class");
                    if constexpr (FunctionOrigin_v<F> == FunctionOrigin::Functor)
                    {
                        m_Stream << _T(" (functor)");
                    }

                    m_Stream << _T(": ") << MemberFunctionClassName_v<F>;
                }
                else
                {
                    // Only other possibility by design at this stage
                    static_assert(FunctionClassification_v<F> == FunctionClassification::Abominable);

                    m_Stream << _T("Abominable function");

                    ////////////////////////////////////////////////////////////
                    // Should always be false in theory since "std::function"
                    // can only be legally specialized on a free function
                    // (we're now processing an abominable function!) so we
                    // should never come through here. If a given compiler
                    // supports it though (doesn't choke on it) then we *will*
                    // come through here but it's not legal (not our doing but
                    // we'll allow it through)
                    ////////////////////////////////////////////////////////////
                    if constexpr (FunctionOrigin_v<F> == FunctionOrigin::StdFunction)
                    {
                        DisplayFromStdFunctionMsg();
                    }
                }

                m_Stream << _T("\n");

                ///////////////////////////////
                // #) Calling convention
                ///////////////////////////////
                streamNumBracket() << _T("Calling convention: ") << CallingConventionName_v<F> << _T("\n");

                ///////////////////////////////
                // #) Return
                ///////////////////////////////
                streamNumBracket() << _T("Return: ") << ReturnTypeName_v<F> << _T("\n");

                ///////////////////////////////
                // #) Arguments
                ///////////////////////////////
                streamNumBracket() << _T("Arguments (") <<
                                      ArgCount_v<F> <<
                                      (IsVariadic_v<F> ? _T(" + variadic") : _T("")) <<
                                      _T("):");

                //////////////////////////////////////////////
                // Function's arg list not empty, i.e., not
                // "Func()" or (equivalently) "Func(void)"?
                // (so it has at least 1 non-variadic arg
                // and/or is variadic)
                //////////////////////////////////////////////
                if constexpr (!IsArgListEmpty_v<F>)
                {
                    m_Stream << _T("\n");

                    //////////////////////////////////////////////
                    // Stream all args as per this example (note
                    // that a tab character precedes each number
                    // and a linefeed ("\n") follows the last
                    // line - the "..." line only exists if the
                    // function is variadic):
                    //
                    //    1) const std::basic_string<wchar_t> &
                    //    2) const char*
                    //    3) short
                    //    4) ...
                    //////////////////////////////////////////////
                    StreamArgTypes<F>();
                }
                else
                {
                    ////////////////////////////////////////
                    // Function's arg list is "(void)" (it
                    // has no args and is not variadic)
                    ////////////////////////////////////////
                    m_Stream << _T(" None\n");
                }

                /////////////////////////////////////////////////////////
                // If not a free function then display traits applicable
                // to those only (i.e., cv and ref qualifiers since they
                // aren't applicable to free functions - they're only
                // applicable to non-static member functions and
                // "abominable" functions)
                /////////////////////////////////////////////////////////
                if constexpr (FunctionClassification_v<F> != FunctionClassification::Free)
                {
                    ///////////////////////////////
                    // #) const
                    ///////////////////////////////
                    streamNumBracket() << _T("const: ") << std::boolalpha << IsFunctionConst_v<F> << _T("\n");

                    ///////////////////////////////
                    // #) volatile
                    ///////////////////////////////
                    streamNumBracket() << _T("volatile: ") << std::boolalpha << IsFunctionVolatile_v<F> << _T("\n");

                    ///////////////////////////////
                    // #) Ref-qualifier
                    ///////////////////////////////
                    streamNumBracket() << _T("Reference: ") << (FunctionReference_v<F> == FunctionReference::None ? _T("None") : FunctionReferenceName_v<F>) << _T("\n");
                }

                ///////////////////////////////
                // #) noexcept
                ///////////////////////////////
                streamNumBracket() << _T("noexcept: ") << std::boolalpha << IsNoexcept_v<F>;

                return m_Stream;
            }

        private:
            ///////////////////////////////////////////////////////////////
            // OutputArgI(). Streams the "Ith" arg to "m_Stream" (formatted
            // as seen below). Note that "I" refers to the (zero-based)
            // "Ith" arg in the function we're being called for (in
            // left-to-right order), and "argTypeAsStr" is its type
            // (converted to a WYSIWYG string).
            ///////////////////////////////////////////////////////////////
            void OutputArgI(const std::size_t i, // "Ith" arg (zero-based)
                            tstring_view argTypeAsStr) const // Type of the "Ith" arg just above (as a string)
            {
                ////////////////////////////////////////
                // E.g., (using zero-based i = 2 as an
                // example, i.e., the 3rd arg of the
                // function we're being called for)
                //
                //     "\t3) double\n"
                ////////////////////////////////////////
                m_Stream << _T("\t") <<
                            (i + 1) << // "Ith" arg ("i" is zero-based so we add 1 to display
                                        // it as 1-based - more natural for display purposes)
                            _T(") ") <<
                            argTypeAsStr <<
                            _T("\n");
            }

            /////////////////////////////////////////////////////////////////
            // OutputArgI(). Converts "ArgTypeT" template arg to a "pretty"
            // (WYSIWYG) string and passes it to above (non-template)
            // overload where it's streamed to "m_Stream" (formatted as
            // seen in the above overload). Note that "I" refers to the
            // (zero-based) "Ith" arg in the function we're being called for
            // (in left-to-right order) and "ArgTypeT" is its type.
            /////////////////////////////////////////////////////////////////
            template <std::size_t I, typename ArgTypeT>
            void OutputArgI() const // "Ith" arg (zero-based)
            {
                ////////////////////////////////////////
                // Convert "ArgTypeT" to a string (its
                // user-friendly name) and pass it to
                // the overload just above
                ////////////////////////////////////////
                OutputArgI(I, TypeName_v<ArgTypeT>);
            }

            ////////////////////////////////////////////////////////////
            // Lambda templates not supported? (they're only supported
            // in C++20 or later). Note that we don't support earlier
            // versions and everything would have already been
            // preprocessed out if compiling for versions prior to C++17
            // (see check for CPP17_OR_LATER near to top of file). For
            // C++20 or later however we replace the following class
            // (StreamArgType) with its lambda template equivalent at
            // point of use later on (in "StreamArgTypes()"). Both just
            // invoke "OutputArgI()" above so are identical but the
            // lambda is more convenient.
            ////////////////////////////////////////////////////////////
            #if !defined(LAMBDA_TEMPLATES_SUPPORTED)
                class StreamArgType
                {
                public:
                    StreamArgType(const DisplayAllFunctionTraitsImpl &displayAllFunctionTraitsImpl) noexcept
                        : m_DisplayAllFunctionTraitsImpl(displayAllFunctionTraitsImpl)
                    {
                    }

                    ///////////////////////////////////////////////////
                    // Functor we'll be invoking once for each arg in
                    // the function whose traits we're displaying
                    // (where "I" is the zero-based number of the arg,
                    // and "ArgTypeT" is its type). Following is a
                    // template so it effectively gets stamped out
                    // once for each argument in the function whose
                    // traits we're displaying. See call to
                    // "ForEachArg()" in "StreamArgTypes()" (which
                    // processes this class in C++ 17 only).
                    ///////////////////////////////////////////////////
                    template <std::size_t I, typename ArgTypeT>
                    bool operator()() const
                    {
                        /////////////////////////////////////////////
                        // Defer to this function (template) which
                        // streams (zero-based) arg "I" (whose type
                        // is "ArgTypeT") to "m_Stream". Both "I"
                        // (actually "I+1" to make it one-based for
                        // display purposes) and the type name of
                        // "ArgType_t"T are therefore displayed.
                        /////////////////////////////////////////////
                        m_DisplayAllFunctionTraitsImpl.OutputArgI<I, ArgTypeT>();

                        //////////////////////////////////////////////
                        // Return true to continue iterating so this
                        // function gets called again with "I + 1"
                        // (until all args in the function whose
                        // traits are being displayed are processed).
                        // Note that returning false would stop
                        // iterating, equivalent to a "break"
                        // statement in a regular"for" loop.
                        //////////////////////////////////////////////
                        return true;
                    }

                private:
                    const DisplayAllFunctionTraitsImpl& m_DisplayAllFunctionTraitsImpl;
                };
            #endif

            ///////////////////////////////////////////////////////////////////////////
            // "StreamArgTypes()". Displays all arguments for function "F" to
            // "m_Stream" as per the following example:
            //
            // If "F" is a function of the following type:
            //
            //       int (const std::string &, const char *, short, ...) noexcept;
            //
            // Then displays (outputs to "m_Stream") the following (note that the
            // format of the type names below are compiler-dependent but are usually
            // identical or very similar):
            //
            //       1) const std::basic_string<char> &
            //       2) const char *
            //	     3) short
            //	     4) ...
            //
            // Note that a tab character precedes each line above, and a linefeed
            // ("\n") is output after the final line. Note that if "F" has no args
            // however (including variadic args), then the function does nothing
            // (nothing gets streamed to "m_Stream")
            ///////////////////////////////////////////////////////////////////////////
            template <TRAITS_FUNCTION_C F>
            void StreamArgTypes() const
            {
                // Lambda templates not supported until C++20
                #if defined(LAMBDA_TEMPLATES_SUPPORTED)
                    ///////////////////////////////////////////////////
                    // Lambda we'll be invoking once for each arg in
                    // function "F" (where "I" is the zero-based
                    // number of the arg, and "ArgTypeT" is its type).
                    // Following is a template so it effectively gets
                    // stamped out once for each argument in function
                    // "F" (in the call to "ForEachArg()" below).
                    ///////////////////////////////////////////////////
                    const auto streamArgType = [&]<std::size_t I, typename ArgTypeT>()
                    {
                        /////////////////////////////////////////////
                        // Defer to this function (template) which
                        // streams (zero-based) arg "I" (whose type
                        // is "ArgTypeT") to "m_Stream". Both "I"
                        // (actually "I+1" to make it one-based for
                        // display purposes) and the type name of
                        // "ArgTypeT are therefore displayed.
                        /////////////////////////////////////////////
                        OutputArgI<I, ArgTypeT>();

                        ////////////////////////////////////////////////
                        // Return true to continue iterating so this
                        // function gets called again with "I + 1"
                        // (until all args in function "F" are
                        // processed). Note that returning false would
                        // stop iterating, equivalent to a "break"
                        // statement in a regular "for" loop.
                        ////////////////////////////////////////////////
                        return true;
                    };

                    ////////////////////////////////////////////////////
                    // Roll our own functor for C++17 (identical to
                    // lambda above). Note that we don't support C++
                    // versions < C++17 so if we drop through here we
                    // must be targeting C++17 (since CPP17_OR_LATER
                    // was checked at top of file and is now in effect)
                    ////////////////////////////////////////////////////
                #else
                    const StreamArgType streamArgType(*this);
                #endif

                ////////////////////////////////////////////////////////
                // Invoke the above functor ("streamArgType") once for
                // each arg in the function whose traits we're
                // processing ("F"). Note that the following generic
                // function (template) "ForEachArg()" will effectively
                // stamp out one copy of member function (template)
                // "streamArgType.operator()<I, ArgTypeT>" for each
                // argument in function "F". IOW, we wind up with "N"
                // versions of "streamArgType.operator()", where "N" is
                // the number of non-variadic args in function "F" and
                // the template args of each "streamArgType.operator()"
                // reflect the "Ith" arg in function "F" (where "I" is
                // in the range 0 to N - 1)
                ////////////////////////////////////////////////////////
                ForEachArg<F>(streamArgType);

                // Is function variadic? (i.e., last arg is "...")
                if (IsVariadic_v<F>)
                {
                    OutputArgI(ArgCount_v<F>, // i
                               _T("...")); // argTypeAsStr
                }
            }

            void DisplayFromStdFunctionMsg() const
            {
                m_Stream << _T(" (from \"std::function\" specialization)");
            }

            std::basic_ostream<CharT, CharTraitsT>& m_Stream;
        }; // class DisplayAllFunctionTraitsImpl
    } // namespace Private

    /////////////////////////////////////////////////////////////////////////
    // DisplayAllFunctionTraits(). Displays (streams) all function traits
    // for template arg (function) "F" to the given "stream" and returns
    // "stream" (officially documented here:
    // https://github.com/HexadigmSystems/FunctionTraits/#displayallfunctiontraits).
    // The general format of the streamed traits is shown in the
    // example below and will vary somewhat depending on "F", but generally
    // remains largely the same (consult the code below itself for precise
    // details). Also note that the format of all displayed types in the
    // output (return type, argument types, etc.) can also vary slightly
    // depending on the target compiler itself (since the compiler itself is
    // responsible for them).
    //
    //      // Standard C++ headers
    //      #include <iostream>
    //      #include <string>
    //
    //      ////////////////////////////////////////////////////////
    //      // Only file in this repository you need to explicitly
    //      // #include
    //      ////////////////////////////////////////////////////////
    //      #include "FunctionTraits.h"
    //
    //      /////////////////////////////////////////////////////////
    //      // Namespace with sample function whose traits you wish
    //      // to display (all of them)
    //      /////////////////////////////////////////////////////////
    //      namespace Test
    //      {
    //          class SomeClass
    //          {
    //          public:
    //              int DoSomething(const std::basic_string<wchar_t>&,
    //                              const char*,
    //                              short int,
    //                              int,
    //                              float,
    //                              long int,
    //                              double,
    //                              ...) const volatile && noexcept;
    //          };
    //      }
    //
    //      int main()
    //      {
    //          // Everything in the library declared in this namespace
    //          using namespace StdExt;
    //
    //          // Above member function whose traits you wish to display
    //          using F = decltype(&Test::SomeClass::DoSomething);
    //
    //          //////////////////////////////////////////
    //          // Display all traits for "F" to "tcout"
    //          // (or pass any other required stream)
    //          //////////////////////////////////////////
    //          DisplayAllFunctionTraits<F>(tcout);
    //
    //          return 0;
    //      }
    //
    // This will stream the traits for function "Test::SomeClass::DoSomething()"
    // above to "tcout" in the following format (again, the format may vary
    // slightly depending on "F" and the target compiler):
    //
    //      1) Function: int (Test::SomeClass::*)(const std::basic_string<wchar_t>&, const char*, short int, int, float, long int, double, ...) const volatile && noexcept
    //      2) Classification: Non-static member
    //      3) Calling convention: cdecl
    //      4) Class: Test::SomeClass
    //      5) Return: int
    //      6) Arguments (7 + variadic):
    //              1) const std::basic_string<wchar_t> &
    //              2) const char*
    //              3) short int
    //              4) int
    //              5) float
    //              6) long int
    //              7) double
    //              8) ...
    //      7) const: true
    //      8) volatile: true
    //      9) Reference: &&
    //      10) noexcept: true
    //
    // Note that for free functions (including static member functions),
    // which don't support "const", "volatile" or ref-qualifiers, items 7,
    // 8 and 9 above will therefore be removed. Instead, anything that
    // follows them ("noexcept" only at this writing), will simply be
    // renumbered.
    //
    // Lastly, as emphasized, the exact format can vary depending on "F" and
    // the target compiler itself. Additional info is sometimes displayed
    // for instance to identify when "F" is a functor, an abominable
    // function, a "std::function" specialization, etc., but the format
    // remains very similar to the example above. You'll need to examine the
    // code itself for precise details but since the output is intended for
    // display purposes only, most won't need to. That is, the exact format
    // isn't normally relevant for most users since the traits of "F" are
    // simply displayed in a human-readable, WYISWYG format that's subject
    // to possible change in future releases. Unless you need to know the
    // exact details or wish to see how the function does its job (the
    // latter is a good learning exercise in fact since the following
    // function is designed as a client of the library itself), most will
    // never need to trace into the code (but for end-users wishing to do so
    // as a learning exercise, note that you normally don't need to trace
    // into any of the library's other code besides class
    // "Private::DisplayAllFunctionTraitsImpl" just above, which implements
    // the following function, since the latter class is effectively just a
    // self-contained client of the library no different than your own code
    // is - it just relies on the library's public templates to do its work
    // so tracing into any other parts of the library beyond
    // "Private::DisplayAllFunctionTraitsImpl" itself is normally
    // unnecessary and would be an arduous task).
    /////////////////////////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F,
              typename CharT,
              typename CharTraitsT = std::char_traits<CharT>>
    std::basic_ostream<CharT, CharTraitsT>& DisplayAllFunctionTraits(std::basic_ostream<CharT, CharTraitsT> &stream)
    {
        ////////////////////////////////////////////////
        // Implementation class that does all the work
        // (for internal use only so declared in
        // namespace "Private")
        ////////////////////////////////////////////////
        const Private::DisplayAllFunctionTraitsImpl<CharT, CharTraitsT> displayAllFunctionTraitsImpl(stream);

        return displayAllFunctionTraitsImpl.template Process<F>();
    }
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    ////////////////////////////////////////////////////////////////
    // For internal use only (but must remain permanently #defined
    // since the function detection macros further below depend on
    // them - they need to remain alive in the user's environment)
    ////////////////////////////////////////////////////////////////
    #define DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) static constexpr StdExt::tstring_view Name = StdExt::tstring_view(STDEXT_TSTRINGIFY(STDEXT_UNBRACKET BRACKETED_CPP_NAME));

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
        // For internal use only
        namespace Private
        {
            ///////////////////////////////////////////////////////////////
            // Note: When "F" is any supported function type other than a
            // reference to a non-static member function pointer then the
            // call to "RemoveCvRef_t" seen below isn't required though
            // harmless for our purposes (since the result of calling
            // "MakeMemberFunctionPtr_t" below always results in a
            // non-static member function pointer in this case, not a
            // reference to one so the call to "RemoveCvRef_t" has no
            // impact on the outcome of what we're calling the following
            // template for). If "F" *is* a reference to a non-static
            // member function pointer however, though it would be rare in
            // practice, then "MakeMemberFunctionPtr_t" will also return a
            // reference to a non-static member function pointer, since
            // the reference remains intact, but we need to remove this
            // reference or the callers of this template will be trying to
            // "static_cast" a non-static member function pointer to a
            // *reference* to one, which will fail because of the
            // reference (so removing the reference here prevents that and
            // it's only possible if "F" is already a reference to a
            // non-static member function pointer as noted - again, the
            // call to "RemoveCvRef_t" isn't necessary when "F" is any
            // other function type we support but doing so is harmless).
            // Note that cv-qualifiers in non-static member function
            // pointers if any are immaterial however (they don't affect
            // the "static_cast" this alias is used for) so we don't
            // really need to remove them (and could therefore just call
            // "std::remove_reference_t" here instead of "RemoveCvRef_t"),
            // but we'll remove them anyway (to prevent any possible
            // surprises if we kept them just in case we ever run afoul of
            // some arcane rule in the language in some untested context -
            // it's highly unlikely so removing the cv-qualifiers isn't
            // necessary at this writing but harmless for our purposes).
            ///////////////////////////////////////////////////////////////
            template <IS_CLASS_C T, TRAITS_FUNCTION_C F>
            using ClassHasFunctionStaticCastTarget_t = RemoveCvRef_t< MakeMemberFunctionPtr_t<T, F> >;
        }
    #endif // #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

/////////////////////////////////////////////////////////////////////////////
// Member function detection templates follow below. Note however that some
// of these templates depend on write traits to do their work (by necessity)
// so the following must be #defined in order to support them (but it
// usually is - read on). If not #defined then write traits were
// preprocessed out earlier in this header so these function detection
// templates won't be available (since they depend on these write traits).
// Note that the following constant is always #defined however unless the
// user explicitly #defines REMOVE_FUNCTION_WRITE_TRAITS if they wish to
// preprocess them out (because they don't rely on them and want to
// eliminate the overhead though it's negligible - doing so however means
// they can't use those function detection that depend on write traits).
// Note that if we're now targeting a Microsoft Visual Studio 2017 release
// of MSVC then the following will always be undefined since we don't
// support write traits in that version (due to bugs in that version so the
// function detection templates that depend on write traits won't be
// available there either).
/////////////////////////////////////////////////////////////////////////////
#if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
    #define WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED
#endif

////////////////////////////////////////////////////////
// Usually true (see comments just above) but if false
// then the DECLARE_CLASS_HAS_FUNCTION and
// DECLARE_CLASS_HAS_STATIC_FUNCTION macros (and the
// templates they create) are preprocessed out below.
////////////////////////////////////////////////////////
#if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)
    ////////////////////////////////////////////////////////////
    // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=116167.
    // Once fixed we can #undef this constant and remove the
    // code that checks for it just below. The GCC code below
    // that works around this bug can then be removed. Note
    // that the same code also works in non-GCC environments as
    // well so either implementation below will do, but the
    // code targeting the bug fix is much longer though as
    // seen. Note that the bug exists in the call to:
    //
    //    static_cast< StdExt::Private::ClassHasFunctionStaticCastTarget_t<T, StdExt::AddNoexcept_t<F>> >(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME)
    //
    // If "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME" (for brevity
    // we'll just call this "T::CPP_NAME" from here on) is not
    // "noexcept" but "F" is "noexcept" then the cast is
    // illegal so it should always fail (and our SFINAE code
    // below handles the failure correctly). On GCC however, if
    // "T::CPP_NAME" is not overloaded and not inherited then
    // it erroneously succeeds (due to the bug cited above)
    // when it should fail so our SFINAE code won't work
    // correctly (when "T::CPP_NAME" is not overloaded). If it
    // is overloaded then it correctly fails so our
    // "static_cast" will work. Our work-around for GCC below
    // therefore checks if "T::CPP_NAME" is overloaded and if
    // so, relies on the cast above since it works correctly
    // (so the same "static_cast" is used on all platforms
    // including GCC). If not overloaded however then we're
    // potentially dealing with the bug (if "CPP_NAME" isn't
    // inherited) since the "static_cast" will erroneously
    // succeed (again, if "CPP_NAME" isn't inherited) so we
    // resort to "StdExt::IsNoexcept_v" instead of the
    // "static_cast" (to check if "T::CPP_NAME" is noexcept -
    // since it's not overloaded at this point we can avoid the
    // "static_cast" altogether by simply calling
    // "decltype(&T::CPP_NAME)" which, because it's not
    // overloaded, won't fail due to overload ambiguity and we
    // can then simply pass it to "StdExt::IsNoexcept_v" - the
    // latter call therefore acts as an alternative to the
    // "static_cast" to determine if "T::CPP_NAME" is noexcept
    // or not - we can't and don't use "StdExt::IsNoexcept_v"
    // in the overloaded case however because
    // "decltype(&T::CPP_NAME)" will fail due to overload
    // ambiguity)
    ////////////////////////////////////////////////////////////
    #if defined(GCC_COMPILER)
        #define GCC_BUG_116167_IN_EFFECT
    #endif

    ////////////////////////////////////////////////////////////
    // See GCC_BUG_116167_IN_EFFECT just above. The #else code
    // below works around it until fixed (but that code will
    // also work on non-GCC platforms as well).
    ////////////////////////////////////////////////////////////
    #if !defined(GCC_BUG_116167_IN_EFFECT)
        ///////////////////////////////////////////////////
        // For internal use only (both templates declared
        // by this macro below)
        ///////////////////////////////////////////////////
        #define DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_IS_NOEXCEPT(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
            namespace StdExt_Private_ClassHas##FUNCTION_OR_OPERATOR##_##NAME \
            { \
                /* Primary template */ \
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F, \
                          typename = void> \
                struct IsNoexcept : std::false_type \
                { \
                }; \
\
                /* Partial specialization of above template */ \
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F> \
                struct IsNoexcept<T, \
                                  F, \
                                  /*//////////////////////////////////////////////////////// \
                                  // By design we only arrive here if static member          \
                                  // function "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME" (for  \
                                  // brevity we'll just call this "T::CPP_NAME" from here    \
                                  // on) exists and is either identical to function "F" or   \
                                  // identical except for "noexcept" in "T::CPP_NAME" but    \
                                  // not in "F" (in the latter case they're identical in     \
                                  // all other respects). To determine if "noexcept" is      \
                                  // present in "T::CPP_NAME" or not (why we're here), we    \
                                  // first add "noexcept" to "T::CPP_NAME" via the call to   \
                                  // "StdExt::AddNoexcept_t" seen below. We're calling the   \
                                  // latter on "F" however since it's identical to           \
                                  // "T::CPP_NAME" at this stage except for possibly its     \
                                  // "noexcept" specifier as noted (again, by design). The   \
                                  // call to "StdExt::AddNoexcept_t" therefore just yields   \
                                  // a noexcept version of "T::CPP_NAME" itself. We then     \
                                  // simply cast "T::CPP_NAME" to this "noexcept" version    \
                                  // of itself but if "T::CPP_NAME" isn't actually           \
                                  // "noexcept" then the cast is illegal in C++ since you    \
                                  // can't cast a non-noexcept function to a noexcept        \
                                  // function (though the reverse situation is legal - you   \
                                  // can cast a noexcept function to a non-noexcept          \
                                  // function but we're not doing that here). The upshot is  \
                                  // that if the "static_cast" fails due to the illegal      \
                                  // cast of non-noexcept to noexcept as just described      \
                                  // then we know that "T::CPP_NAME" must not be noexcept    \
                                  // (because if it's noexcept then the "static_cast" will   \
                                  // succeed). SFINAE therefore fails and our primary        \
                                  // template kicks in (meaning that "T::CPP_NAME" isn't     \
                                  // noexcept). If the "static_cast" succeeds however then   \
                                  // "T::CPP_NAME" must be noexcept since we're simply       \
                                  // casting "T::CPP_NAME" back to itself in this case (and  \
                                  // it's also a noexcept to noexcept cast which is legal    \
                                  // but not really relevant here). The following            \
                                  // specialization therefore kicks in meaning that          \
                                  // "T::CPP_NAME" must be noexcept.                         \
                                  ////////////////////////////////////////////////////////*/ \
                                  std::void_t< decltype( static_cast< StdExt::Private::ClassHasFunctionStaticCastTarget_t<T, StdExt::AddNoexcept_t<F>> >(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME) ) > \
                                 > \
                                 : std::true_type \
                { \
                }; \
\
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F> \
                inline constexpr bool IsNoexcept_v = IsNoexcept<T, F>::value; \
            }
    #else
        //////////////////////////////////////////////////
        // For internal use only (all templates declared
        // by this macro below)
        //////////////////////////////////////////////////
        #define DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_IS_NOEXCEPT(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
            namespace StdExt_Private_ClassHas##FUNCTION_OR_OPERATOR##_##NAME \
            { \
                /* Primary template */ \
                template <IS_CLASS_C T, \
                          typename = void> \
                struct IsOverloaded : std::true_type \
                { \
                }; \
\
                /* Partial specialization of above template */ \
                template <IS_CLASS_C T> \
                struct IsOverloaded<T, \
                                    /*//////////////////////////////////////////////////// \
                                    // By design we only arrive here if non-static member  \
                                    // function "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME"   \
                                    // exists. Following therefore succeeds SFINAE only    \
                                    // if it's not overloaded (fails if it is overloaded)  \
                                    ////////////////////////////////////////////////////*/ \
                                    std::void_t< decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME) > \
                                   > \
                                   : std::false_type \
                { \
                }; \
\
                template <IS_CLASS_C T> \
                inline constexpr bool IsOverloaded_v = IsOverloaded<T>::value; \
\
                /* Partial specialization of above template */ \
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F, \
                          typename = void> \
                struct IsOverloadedNoexcept : std::false_type \
                { \
                }; \
\
                /* Primary template */ \
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F> \
                struct IsOverloadedNoexcept<T, \
                                            F, \
                                            /*//////////////////////////////////////////////////////// \
                                            // By design we only arrive here if static member          \
                                            // function "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME" (for  \
                                            // brevity we'll just call this "T::CPP_NAME" from here    \
                                            // on) exists and is either identical to function "F" or   \
                                            // identical except for "noexcept" in "T::CPP_NAME" but    \
                                            // not in "F" (in the latter case they're identical in     \
                                            // all other respects). To determine if "noexcept" is      \
                                            // present in "T::CPP_NAME" or not (why we're here), we    \
                                            // first add "noexcept" to "T::CPP_NAME" via the call to   \
                                            // "StdExt::AddNoexcept_t" seen below. We're calling the   \
                                            // latter on "F" however since it's identical to           \
                                            // "T::CPP_NAME" at this stage except for possibly its     \
                                            // "noexcept" specifier as noted (again, by design). The   \
                                            // call to "StdExt::AddNoexcept_t" therefore just yields   \
                                            // a noexcept version of "T::CPP_NAME" itself. We then     \
                                            // simply cast "T::CPP_NAME" to this "noexcept" version    \
                                            // of itself but if "T::CPP_NAME" isn't actually           \
                                            // "noexcept" then the cast is illegal in C++ since you    \
                                            // can't cast a non-noexcept function to a noexcept        \
                                            // function (though the reverse situation is legal - you   \
                                            // can cast a noexcept function to a non-noexcept          \
                                            // function but we're not doing that here). The upshot is  \
                                            // that if the "static_cast" fails due to the illegal      \
                                            // cast of non-noexcept to noexcept as just described      \
                                            // then we know that "T::CPP_NAME" must not be noexcept    \
                                            // (because if it's noexcept then the "static_cast" will   \
                                            // succeed). SFINAE therefore fails and our primary        \
                                            // template kicks in (meaning that "T::CPP_NAME" isn't     \
                                            // noexcept). If the "static_cast" succeeds however then   \
                                            // "T::CPP_NAME" must be noexcept since we're simply       \
                                            // casting "T::CPP_NAME" back to itself in this case (and  \
                                            // it's also a noexcept to noexcept cast which is legal    \
                                            // but not really relevant here). The following            \
                                            // specialization therefore kicks in meaning that          \
                                            // "T::CPP_NAME" must be noexcept.                         \
                                            ////////////////////////////////////////////////////////*/ \
                                            std::void_t< decltype( static_cast< StdExt::Private::ClassHasFunctionStaticCastTarget_t<T, StdExt::AddNoexcept_t<F>> >(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME) ) > \
                                           > \
                                           : std::true_type \
                { \
                }; \
\
                /* Partial specialization of above template */ \
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F> \
                inline constexpr bool IsOverloadedNoexcept_v = IsOverloadedNoexcept<T, F>::value; \
\
                /*//////////////////////////////////////////////////////  \
                // Primary template (not defined since specializations    \
                // below always guaranteed to kick in)                    \
                ///////////////////////////////////////////////////////*/ \
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F, \
                          typename = void> \
                struct IsNoexcept; \
\
                /* Partial specialization of above template */ \
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F> \
                struct IsNoexcept<T, \
                                  F, \
                                  std::enable_if_t< !IsOverloaded_v<T> > \
                                 > \
                                 : std::bool_constant< StdExt::IsNoexcept_v<decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME)> > \
                { \
                }; \
\
                /* Partial specialization of above template */ \
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F> \
                struct IsNoexcept<T, \
                                  F, \
                                  std::enable_if_t< IsOverloaded_v<T> > \
                                  > \
                                 : std::bool_constant< IsOverloadedNoexcept_v<T, F> > \
                { \
                }; \
\
                /* Helper variable for above template */ \
                template <IS_CLASS_C T, \
                          TRAITS_FUNCTION_C F> \
                inline constexpr bool IsNoexcept_v = IsNoexcept<T, F>::value; \
            }

            // Done with this (for internal use only)
            #undef GCC_BUG_116167_IN_EFFECT
    #endif // #if !defined(GCC_BUG_116167_IN_EFFECT)

    ////////////////////////////////////////////////////////////////////////////
    // DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR (for internal use only). Macro
    // used to declare helper template "ClassHasFunction_? (read on) used to
    // determine if any class or struct has a non-static member function with a
    // given name and specific (exact) function type. Used as per the following
    // example. Just pass the name of the non-static member function you're
    // interested in via the macro's NAME arg ("Whatever" in the following
    // case):
    //
    //     DECLARE_CLASS_HAS_FUNCTION(Whatever)
    //
    // Doing so then declares a template struct where the name of the struct
    // always starts with "ClassHasFunction_" and ends with the NAME arg you
    // passed (representing the name of the function you're targeting,
    // "Whatever" in this case):
    //
    //     template <IS_CLASS_C T, IS_FUNCTION_C F>
    //     struct ClassHasFunction_Whatever;
    //
    // A helper variable template with the usual "_v" suffix is also created
    // that just defers to the above in the usual C++ way (read on):
    //
    //     template <IS_CLASS_C T, IS_FUNCTION_C F>
    //     constexpr bool ClassHasFunction_Whatever_v = ClassHasFunction_Whatever<T, F>::value;
    //
    // These templates (usually the "_v" version just above) can then be used
    // to determine if any class or struct has a member function called
    // "Whatever" with the exact function type you're interested in. The
    // struct inherits from "std::true_type" if so or "std::false_type"
    // otherwise. For instance, after calling this macro as seen above
    // (passing "Whatever" in this case), you can then do the following.
    // Given any class or struct like so:
    //
    //
    //     class Test
    //     {
    //     public:
    //         int Whatever(float) const noexcept;
    //     };
    //
    // You can determine if it has a non-static member function with the
    // exact type seen above. Just pass the class or struct via the 1st
    // template arg ("Test" in this case), and the function's type via the
    // 2nd template arg ("int (float) const noexcept" in this case):
    //
    //     constexpr bool hasMemberFunction_Whatever = HasMemberFunction_Whatever_v<Test, int (float) const noexcept);
    //
    // This will be set to true because class "Test" (the 1st template arg
    // passed to the call above) has a non-static member function called
    // "Whatever" (the NAME arg passed to this macro), whose type exactly
    // matches the 2nd template arg passed to the above. Note that the 2nd
    // template arg, "F", is the native C++ function type you're interested
    // in, so "int (float) const noexcept" in this case (exactly matching the
    // type of function "Test::Whatever"). It's therefore never a function
    // pointer but the actual C++ function type itself (meeting the criteria
    // of "std::function" - read on). Note that "true" is returned only if
    // the class or struct you pass for the 1st template arg (T) has a
    // non-static member function with this *exact* function type, including
    // its return type, calling convention, etc. To this end, note that
    // function types in C++ are always of the following form, where items in
    // square brackets are optional (and again, this is what "std::function"
    // in the C++ standard itself looks for to determine a function):
    //
    //      ReturnType [CallingConvention] (Args [...]) [const] [volatile] [&|&&] [noexcept[(true|false)]]
    //
    // Note however that the function's "CallingConvention" ("cdecl",
    // "thiscall", etc.), isn't part of the C++ standard at this writing but
    // it's normally considered part of a function's type regardless (by all
    // C++ implementations I'm aware of). The above templates therefore
    // expect a function in this specific format (for type "F") and will only
    // return true if (struct or class) type "T" has a member function called
    // "Whatever" that precisely matches type "F". This means that every
    // component of "F" seen above is tested for in function "Whatever" and
    // must match exactly for these templates to return true. In fact, the
    // function does its job by simply calling "std::is_same_v", passing the
    // type of "T::Whatever" as its 1st template arg and "F" as the 2nd
    // template arg. The comparison therefore looks for an exact match so
    // caution advised. Everything must precisely match for equality to
    // occur. In the above example for instance, even if the return type of
    // "Test::Whatever" is changed from "int" to, say, "char", the call to
    // "ClassHasFunction_Whatever_v" will no longer return true. It will now
    // return false unless you also change "int" to "char" in its 2nd
    // template arg (so that the return types match again).
    //////////////////////////////////////////////////////////////////////////
    #define DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
        /*//////////////////////// \
        // For internal use only   \
        ////////////////////////*/ \
        DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_IS_NOEXCEPT(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
\
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F, \
                  typename = void> \
        struct ClassHas##FUNCTION_OR_OPERATOR##_##NAME : std::false_type \
        { \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F> \
        struct ClassHas##FUNCTION_OR_OPERATOR##_##NAME<T, \
                                                       F, \
                                                       /*//////////////////////////////////////////////////////// \
                                                       // The "static_cast" below will succeed SFINAE only if     \
                                                       // "T" has a non-static member function called             \
                                                       // STDEXT_UNBRACKET BRACKETED_CPP_NAME (for brevity we'll  \
                                                       // just call this "CPP_NAME" from here on) with the same   \
                                                       // type as "F" except for possible differences in their    \
                                                       // "noexcept" specifier (but see IMPORTANT comment         \
                                                       // further below). If so then here are the "noexcept"      \
                                                       // possibilities:                                          \
                                                       //                                                         \
                                                       //    F         T::CPP_NAME   SFINAE succeeds              \
                                                       //    -         -----------   ---------------              \
                                                       //                            ✓                            \
                                                       //              noexcept      ✓                            \
                                                       //    noexcept                X (illegal cast)             \
                                                       //    noexcept  noexcept      ✓                            \
                                                       //                                                         \
                                                       // If the "static_cast" succeeds (SFINAE fails             \
                                                       // otherwise), then as per the above table it's            \
                                                       // guaranteed that "F" and "T::CPP_NAME" are identical     \
                                                       // non-static member function types unless we're dealing   \
                                                       // with the "noexcept" mismatch on the 2nd row above (if   \
                                                       // dealing with the mismatch on the 3rd row then the       \
                                                       // "static_cast" itself is guaranteed to fail since a      \
                                                       // non-noexcept function can't be cast to one that's       \
                                                       // noexcept - SFINAE is therefore guaranteed to fail).     \
                                                       // The call to "bool_constant" in the base class below is  \
                                                       // therefore used to detect the "noexcept" mismatch on     \
                                                       // the 2nd row in the table. It simply compares the        \
                                                       // "noexcept" for "F" and "T::CPP_NAME", returning true    \
                                                       // if they're equal (meaning we must be dealing with the   \
                                                       // 1st or 4th row above), or false otherwise (meaning we   \
                                                       // must be dealing with the 2nd row above).                \
                                                       //                                                         \
                                                       // The upshot is that SFINAE will succeed and the class    \
                                                       // will inherit from "std::true_type" only if "T" has a    \
                                                       // non-static member function called CPP_NAME with the     \
                                                       // same type as "F" including their noexcept specifiers    \
                                                       // (their types exactly match IOW). We're therefore        \
                                                       // dealing with row 1 and 4 above only. Any other          \
                                                       // scenario, i.e., "T::CPP_NAME" doesn't exist             \
                                                       // whatsoever, or it's an alias, or it's a non-function,   \
                                                       // or it's a static function, or it's a non-static         \
                                                       // function but doesn't match "F" exactly (everything      \
                                                       // must match meaning return type, calling convention,     \
                                                       // arg types, variadic, const, volatile, & or &&           \
                                                       // qualifiers, noexcept), in which case the class will     \
                                                       // inherit from "std::false_type" (either via the primary  \
                                                       // template if SFINAE fails or via this specialization's   \
                                                       // base class but only if row 2 of the table above         \
                                                       // occurs).                                                \
                                                       //                                                         \
                                                       // IMPORTANT                                               \
                                                       // ---------                                               \
                                                       // The above assumes that "T" is a class or struct type    \
                                                       // and "F" is any traits function type supported by this   \
                                                       // library. This is enforced however by the template's     \
                                                       // IS_CLASS_C and TRAITS_FUNCTION_C concepts in C++20 or   \
                                                       // later, or, for C++17, the corresponding static_asserts  \
                                                       // in the primary template (in the latter case SFINAE is   \
                                                       // guaranteed to fail so this specialization never kicks   \
                                                       // in)                                                     \
                                                       ////////////////////////////////////////////////////////*/ \
                                                       std::void_t< decltype( static_cast< StdExt::Private::ClassHasFunctionStaticCastTarget_t<T, F> >(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME) ) > \
                                                      > \
                                                      : std::bool_constant< StdExt::IsNoexcept_v<F> == StdExt_Private_ClassHas##FUNCTION_OR_OPERATOR##_##NAME::IsNoexcept_v<T, F> > \
        { \
            /*///////////////////////////////////////////////////// \
            // Note: Neither of these "static_asserts" will ever    \
            // kick in in C++20 or later since the macros simply    \
            // resolve to whitespace in C++20 or later. The         \
            // corresponding concepts above kick in instead. Even   \
            // in C++17 however where they do resolve to actual     \
            // "static_asserts", the "static_asserts" still won't   \
            // ever kick in since specialization of this class      \
            // won't kick in unless the "static_asserts" hold (due  \
            // to what we're testing in the "std::enable_if_t" call \
            // above). They're therefore just for documentation     \
            // purposes only and consistency with the same          \
            // "static_asserts" in the primary template (which      \
            // *will* kick in in C++17 if T and/or F is illegal -   \
            // again, concepts above will trap things in C++20 and  \
            // later).                                              \
            /////////////////////////////////////////////////////*/ \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, TRAITS_FUNCTION_C F> \
        inline constexpr bool ClassHas##FUNCTION_OR_OPERATOR##_##NAME##_v = ClassHas##FUNCTION_OR_OPERATOR##_##NAME<T, F>::value;

    #define DECLARE_CLASS_HAS_FUNCTION(NAME) DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR(Function, NAME, (NAME))

    //////////////////////////////////////////////////
    // For internal use only (all templates declared
    // by this macro below)
    //////////////////////////////////////////////////
    #define DECLARE_CLASS_HAS_STATIC_FUNCTION_IS_NOEXCEPT(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
        namespace StdExt_Private_ClassHasStatic##FUNCTION_OR_OPERATOR##_##NAME \
        { \
            /* Primary template (for internal use only) */ \
            template <IS_CLASS_C T, \
                      TRAITS_FUNCTION_C F, \
                      typename = void> \
            struct IsNoexcept: std::false_type \
            { \
            }; \
\
            /*//////////////////////////////////////////////// \
            // Partial specialization of above template (for   \
            // internal use only                               \
            ////////////////////////////////////////////////*/ \
            template <IS_CLASS_C T, \
                      TRAITS_FUNCTION_C F> \
            struct IsNoexcept<T, \
                              F, \
                              /*//////////////////////////////////////////////////////// \
                              // By design we only arrive here if static member          \
                              // function "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME" (for  \
                              // brevity we'll just call this "T::CPP_NAME" from here    \
                              // on) exists and is either identical to function "F" or   \
                              // identical except for "noexcept" in "T::CPP_NAME" but    \
                              // not in "F" (in the latter case they're identical in     \
                              // all other respects). To determine if "noexcept" is      \
                              // present in "T::CPP_NAME" or not (why we're here), we    \
                              // first add "noexcept" to "T::CPP_NAME" via the call to   \
                              // "StdExt::AddNoexcept_t" seen below. We're calling the   \
                              // latter on "F" however since it's identical to           \
                              // "T::CPP_NAME" at this stage except for possibly its     \
                              // "noexcept" specifier as noted (again, by design). The   \
                              // call to "StdExt::AddNoexcept_t" therefore just yields   \
                              // a noexcept version of "T::CPP_NAME" itself. We then     \
                              // simply cast "T::CPP_NAME" to this "noexcept" version    \
                              // of itself but if "T::CPP_NAME" isn't actually           \
                              // "noexcept" then the cast is illegal in C++ since you    \
                              // can't cast a non-noexcept function to a noexcept        \
                              // function (though the reverse situation is legal - you   \
                              // can cast a noexcept function to a non-noexcept          \
                              // function but we're not doing that here). The upshot is  \
                              // that if the "static_cast" fails due to the illegal      \
                              // cast of non-noexcept to noexcept as just described      \
                              // then we know that "T::CPP_NAME" must not be noexcept    \
                              // (because if it's noexcept then the "static_cast" will   \
                              // succeed). SFINAE therefore fails and our primary        \
                              // template kicks in (meaning that "T::CPP_NAME" isn't     \
                              // noexcept). If the "static_cast" succeeds however then   \
                              // "T::CPP_NAME" must be noexcept since we're simply       \
                              // casting "T::CPP_NAME" back to itself in this case (and  \
                              // it's also a noexcept to noexcept cast which is legal    \
                              // but not really relevant here). The following            \
                              // specialization therefore kicks in meaning that          \
                              // "T::CPP_NAME" must be noexcept.                         \
                              ////////////////////////////////////////////////////////*/ \
                              std::void_t< decltype( static_cast< StdExt::FunctionRawType_t<StdExt::AddNoexcept_t<F>> *>(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME) ) > \
                             > \
                             : std::true_type \
            { \
            }; \
\
            /* For internal use only */ \
            template <IS_CLASS_C T, \
                      TRAITS_FUNCTION_C F> \
            inline constexpr bool IsNoexcept_v = IsNoexcept<T, F>::value; \
        } // namespace StdExt_Private_ClassHasStatic##FUNCTION_OR_OPERATOR##_##NAME

    ////////////////////////////////////////////////////////////////////////////
    // DECLARE_CLASS_HAS_STATIC_FUNCTION_OR_OPERATOR (for internal use only).
    // Macro used to declare helper template "ClassHasStaticFunction_? (read
    // on) used to determine if any class or struct has a non-static member
    // function with a given name and specific (exact) function type. Used as
    // per the following example. Just pass the name of the non-static member
    // function you're interested in via the macro's NAME arg ("Whatever" in
    // the following case):
    //
    //     DECLARE_CLASS_HAS_STATIC_FUNCTION(Whatever)
    //
    // Doing so then declares a template struct where the name of the struct
    // always starts with "ClassHasStaticFunction_" and ends with the NAME arg you
    // passed (representing the name of the function you're targeting,
    // "Whatever" in this case):
    //
    //     template <IS_CLASS_C T, TRAITS_FUNCTION_C F>
    //     struct ClassHasStaticFunction_Whatever;
    //
    // A helper variable template with the usual "_v" suffix is also created
    // that just defers to the above in the usual C++ way (read on):
    //
    //     template <IS_CLASS_C T, TRAITS_FUNCTION_C F>
    //     constexpr bool ClassHasStaticFunction_Whatever_v = ClassHasStaticFunction_Whatever<T, F>::value;
    //
    // These templates (usually the "_v" version just above) can then be used
    // to determine if any class or struct has a member function
    // called "Whatever" with the exact function type you're interested in. The
    // struct inherits from "std::true_type" if so or "std::false_type"
    // otherwise. For instance, after calling this macro as seen above (passing
    // "Whatever" in this case), you can then do the following. Given any class
    // or struct like so:
    //
    //     class Test
    //     {
    //     public:
    //         int Whatever(float) const noexcept;
    //     };
    //
    // You can determine if it has a non-static member function with the exact
    // type seen above. Just pass the class or struct via the 1st template
    // arg ("Test" in this case), and the function's type via the 2nd template
    // arg ("int (float) const noexcept" in this case):
    //
    //     constexpr bool hasStaticFunction_Whatever = ClassHasStaticFunction_Whatever_v<Test, int (float) const noexcept);
    //
    // This will be set to true because class "Test" (the 1st template arg
    // passed to the call above) has a non-static member function called
    // "Whatever" (the NAME arg passed to this macro), whose type exactly
    // matches the 2nd template arg passed to the above. Note that the 2nd
    // template arg, "F", is the native C++ function type you're interested in,
    // so "int (float) const noexcept" in this case (exactly matching the type
    // of function "Test::Whatever"). It's therefore never a function pointer
    // but the actual C++ function type itself (meeting the criteria of
    // "std::function" - read on). Note that "true" is returned only if the
    // class or struct you pass for the 1st template arg (T) has a non-static
    // member function with this *exact* function type, including its return
    // type, calling convention, etc. To this end, note that function types in
    // C++ are always of the following form, where items in square brackets are
    // optional (and again, this is what "std::function" in the C++ standard
    // itself looks for to determine a function):
    //
    //      ReturnType [CallingConvention] (Args [...]) [const] [volatile] [&|&&] [noexcept[(true|false)]]
    //
    // Note however that the function's "CallingConvention" ("cdecl",
    // "thiscall", etc.), isn't part of the C++ standard at this writing but
    // it's normally considered part of a function's type regardless (by all
    // C++ implementations I'm aware of). The above templates therefore
    // expect a function in this specific format (for type "F") and will only
    // return true if (struct or class) type "T" has a member function called
    // "Whatever" that precisely matches type "F". This means that every
    // component of "F" seen above is tested for in function "Whatever" and
    // must match exactly for these templates to return true. In fact, the
    // function does its job by simply calling "std::is_same_v", passing the
    // type of "T::Whatever" as its 1st template arg and "F" as the 2nd
    // template arg. The comparison therefore looks for an exact match so
    // caution advised. Everything must precisely match for equality to
    // occur. In the above example for instance, even if the return type of
    // "Test::Whatever" is changed from "int" to, say, "char", the call to
    // "ClassHasStaticFunction_Whatever_v" will no longer return
    // true. It will now return false unless you also change "int" to "char"
    // in its 2nd template arg (so that the return types match again).
    //////////////////////////////////////////////////////////////////////////
    #define DECLARE_CLASS_HAS_STATIC_FUNCTION_OR_OPERATOR(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
        /*//////////////////////// \
        // For internal use only   \
        ////////////////////////*/ \
        DECLARE_CLASS_HAS_STATIC_FUNCTION_IS_NOEXCEPT(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
\
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F, \
                  typename = void> \
        struct ClassHasStatic##FUNCTION_OR_OPERATOR##_##NAME : std::false_type \
        { \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F> \
        struct ClassHasStatic##FUNCTION_OR_OPERATOR##_##NAME<T, \
                                                             F, \
                                                             /*///////////////////////////////////////////////////////// \
                                                             // The 1st condition below (the call to "static_cast")      \
                                                             // will succeed SFINAE only if "T" has a static member      \
                                                             // function called STDEXT_UNBRACKET BRACKETED_CPP_NAME (for \
                                                             // brevity we'll just call this "CPP_NAME" from here on)    \
                                                             // with the same type as "F" except for possible            \
                                                             // differences in their "noexcept" specifier (but see       \
                                                             // IMPORTANT comment further below). If so then here are    \
                                                             // the "noexcept" possibilities:                            \
                                                             //                                                          \
                                                             //    F         T::CPP_NAME   SFINAE succeeds               \
                                                             //    -         -----------   ---------------               \
                                                             //                            ✓                             \
                                                             //              noexcept      ✓                             \
                                                             //    noexcept                X (illegal cast)              \
                                                             //    noexcept  noexcept      ✓                             \
                                                             //                                                          \
                                                             // If the "static_cast" succeeds (SFINAE fails otherwise)   \
                                                             // then as per the above table it's guaranteed that "F"     \
                                                             // and "T::CPP_NAME" are identical static member function   \
                                                             // types unless we're dealing with the "noexcept"           \
                                                             // mismatch on the 2nd row above (if dealing with the       \
                                                             // mismatch on the 3rd row then the "static_cast" itself    \
                                                             // is guaranteed to fail since a non-noexcept function      \
                                                             // can't be cast to one that's noexcept - SFINAE is         \
                                                             // therefore guaranteed to fail). The 2nd condition below   \
                                                             // is therefore used to catch the "noexcept" mismatch on    \
                                                             // the 2nd row in the table. It simply compares the         \
                                                             // "noexcept" for "F" and "T::CPP_NAME", returning true if  \
                                                             // they're equal (meaning we must be dealing with the 1st   \
                                                             // or 4th row above), or false otherwise (meaning we must   \
                                                             // be dealing with the 2nd row above).                      \
                                                             //                                                          \
                                                             // The upshot is that SFINAE will succeed only if "T" has   \
                                                             // a static member function called CPP_NAME with the same   \
                                                             // type as "F" including their noexcept specifiers (their   \
                                                             // types exactly match IOW). We're therefore dealing with   \
                                                             // row 1 and 4 above only. SFINAE always fails for row 2    \
                                                             // and 3 ("F" and "T::F" identical except for "noexcept" so \
                                                             // they don't precisely match).                             \
                                                             //                                                          \
                                                             // IMPORTANT                                                \
                                                             // ---------                                                \
                                                             // The above assumes that "T" is a class or struct type     \
                                                             // and "F" is a function type. This is enforced however     \
                                                             // by the template's IS_CLASS_C and IS_FUNCTION_C concepts  \
                                                             // in C++20 or later or, for C++17, the corresponding       \
                                                             // static_asserts in the primary template (in the latter    \
                                                             // case SFINAE is guaranteed to fail below).                \
                                                             /////////////////////////////////////////////////////////*/ \
                                                             std::void_t< decltype( static_cast< StdExt::FunctionRawType_t<F> *>(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME) ) > \
                                                            > \
                                                            : std::bool_constant< StdExt::IsNoexcept_v<F> == StdExt_Private_ClassHasStatic##FUNCTION_OR_OPERATOR##_##NAME::IsNoexcept_v<T, F> >  \
        { \
            /*///////////////////////////////////////////////////// \
            // Note: Neither of these "static_asserts" will ever    \
            // kick in in C++20 or later since the macros simply    \
            // resolve to whitespace in C++20 or later. The         \
            // corresponding concepts above kick in instead. Even   \
            // in C++17 however where they do resolve to actual     \
            // "static_asserts", the "static_asserts" still won't   \
            // ever kick in since specialization of this class      \
            // won't kick in unless the "static_asserts" hold (due  \
            // to what we're testing in the "std::enable_if_t" call \
            // above). They're therefore just for documentation     \
            // purposes only and consistency with the same          \
            // "static_asserts" in the primary template (which      \
            // *will* kick in in C++17 if T and/or F is illegal -   \
            // again, concepts above will trap things in C++20 and  \
            // later).                                              \
            /////////////////////////////////////////////////////*/ \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, TRAITS_FUNCTION_C F> \
        inline constexpr bool ClassHasStatic##FUNCTION_OR_OPERATOR##_##NAME##_v = ClassHasStatic##FUNCTION_OR_OPERATOR##_##NAME<T, F>::value;

    #define DECLARE_CLASS_HAS_STATIC_FUNCTION(NAME) DECLARE_CLASS_HAS_STATIC_FUNCTION_OR_OPERATOR(Function, NAME, (NAME))

    ////////////////////////////////////////////////////////////////////////
    // DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR (for internal
    // use only). Functionally equivalent to the DECLARE_CLASS_HAS_FUNCTION
    // macro just above but targets non-overloaded functions only. If the
    // function you're targeting, "T::NAME", is overloaded, then the
    // following templates will always return false (unlike the templates
    // created by DECLARE_CLASS_HAS_FUNCTION), even if one of the overloads
    // match the 2nd template arg you're passing to these templates. IOW,
    // the templates are designed to target functions that are not
    // overloaded. If they are overloaded then these templates will always
    // return false.
    ////////////////////////////////////////////////////////////////////////
    #define DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F, \
                  typename = void> \
        struct ClassHasNonOverloaded##FUNCTION_OR_OPERATOR##_##NAME : std::false_type \
        { \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F> \
        struct ClassHasNonOverloaded##FUNCTION_OR_OPERATOR##_##NAME<T, \
                                                                    F, \
                                                                    /*///////////////////////////////////////////////////////////////// \
                                                                    // For the 1st check below (the "static_cast"), see the identical   \
                                                                    // check in the "struct ClassHas##FUNCTION_OR_OPERATOR##_##NAME"    \
                                                                    // specialization (see the comments there). The 2nd check below     \
                                                                    // simply ensures "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME" (for     \
                                                                    // brevity we'll just call this "CPP_NAME" from here on) isn't      \
                                                                    // overloaded either (the purpose of the following specialization   \
                                                                    // - if SFINAE fails on the 1st check however then the 2nd check    \
                                                                    // isn't relevant). This entire specialization is therefore         \
                                                                    // effectively identical to the                                     \
                                                                    // "struct ClassHas##FUNCTION_OR_OPERATOR##_##NAME" specialization  \
                                                                    // except for the 2nd check below (to ensure "T::CPP_NAME" isn't    \
                                                                    // overloaded as noted). See the latter struct for details.         \
                                                                    /////////////////////////////////////////////////////////////////*/ \
                                                                    std::void_t< decltype( static_cast< StdExt::Private::ClassHasFunctionStaticCastTarget_t<T, F> >(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME) ), \
                                                                                 decltype( &T::STDEXT_UNBRACKET BRACKETED_CPP_NAME ) \
                                                                               > \
                                                                   > \
                                                                   /*/////////////////////////////////////////////////////////////////// \
                                                                   // Don't move this to "std::enable_if_t" above (though we normally    \
                                                                   // could) since GCC has a bug in C++17 only whereby if                \
                                                                   // "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME" below resolves to a       \
                                                                   // non-function member then a hard-compiler error occurs since        \
                                                                   // "StdExt::IsNoexcept_v" will fail (since it's not being passed an   \
                                                                   // actual function). SFINAE should trigger instead but it doesn't     \
                                                                   // (an actual compiler error occurs). This is broken GCC behaviour    \
                                                                   // that was fixed in C++20 but moving this code below as we're doing  \
                                                                   // circumvents the problem in C++17. It would be rare for this to     \
                                                                   // occur anyway however.                                              \
                                                                   ///////////////////////////////////////////////////////////////////*/ \
                                                                   : std::bool_constant< StdExt::IsNoexcept_v<F> == StdExt::IsNoexcept_v<decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME)> > \
        { \
            /*///////////////////////////////////////////////////// \
            // Note: Neither of these "static_asserts" will ever    \
            // kick in in C++20 or later since the macros simply    \
            // resolve to whitespace in C++20 or later. The         \
            // corresponding concepts above kick in instead. Even   \
            // in C++17 however where they do resolve to actual     \
            // "static_asserts", the "static_asserts" still won't   \
            // ever kick in since specialization of this class      \
            // won't kick in unless the "static_asserts" hold (due  \
            // to what we're testing in the "std::enable_if_t" call \
            // above). They're therefore just for documentation     \
            // purposes only and consistency with the same          \
            // "static_asserts" in the primary template (which      \
            // *will* kick in in C++17 if T and/or F is illegal -   \
            // again, concepts above will trap things in C++20 and  \
            // later).                                              \
            /////////////////////////////////////////////////////*/ \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F> \
        inline constexpr bool ClassHasNonOverloaded##FUNCTION_OR_OPERATOR##_##NAME##_v = ClassHasNonOverloaded##FUNCTION_OR_OPERATOR##_##NAME<T, F>::value;

    #define DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION(NAME) DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR(Function, NAME, (NAME))
#endif // if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)

    ////////////////////////////////////////////////////////////////////////
    // DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR (for
    // internal use only). Functionally equivalent to the
    // DECLARE_CLASS_HAS_STATIC_FUNCTION macro just above but targets
    // non-overloaded functions only. If the function you're targeting,
    // "T::NAME", is overloaded, then the following templates will always
    // return false (unlike the templates created by
    // DECLARE_CLASS_HAS_STATIC_FUNCTION), even if one of the overloads
    // match the 2nd template arg you're passing to these templates. IOW,
    // the templates are designed to handle functions that are not
    // overloaded. If they are overloaded these templates will always
    // return false.
    ////////////////////////////////////////////////////////////////////////
    #define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F, \
                  typename = void> \
        struct ClassHasNonOverloadedStatic##FUNCTION_OR_OPERATOR##_##NAME : std::false_type \
        { \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F> \
        struct ClassHasNonOverloadedStatic##FUNCTION_OR_OPERATOR##_##NAME<T, \
                                                                          F, \
                                                                          /*//////////////////////////////////////////////////////////////////////// \
                                                                          // For the 1st check below (the "static_cast"), see the identical check    \
                                                                          // in the "struct ClassHasStatic##FUNCTION_OR_OPERATOR##_##NAME"           \
                                                                          // specialization (see the comments there). The 2nd check below simply     \
                                                                          // ensures "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME" (for brevity we'll     \
                                                                          // just call this "CPP_NAME" from here on) isn't overloaded either (the    \
                                                                          // purpose of the following specialization - if SFINAE fails on the 1st    \
                                                                          // check however then the 2nd check isn't relevant). This entire           \
                                                                          // specialization is identical to the                                      \
                                                                          // "struct ClassHasStatic##FUNCTION_OR_OPERATOR##_##NAME"                  \
                                                                          // specialization except for the 2nd check below (to ensure "T::CPP_NAME"  \
                                                                          // isn't overloaded as noted). See the latter struct for details.          \
                                                                          ////////////////////////////////////////////////////////////////////////*/ \
                                                                          std::void_t< decltype( static_cast< StdExt::FunctionRawType_t<F> *>(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME) ), \
                                                                                       decltype( &T::STDEXT_UNBRACKET BRACKETED_CPP_NAME ) \
                                                                                     > \
                                                                          /* Alternate way (but using above to keep things consistent with the other structs above) */ \
                                                                          /* \
                                                                          std::enable_if_t< StdExt::IsFreeFunctionPointer_v< decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME) > && \
                                                                                            std::is_same_v<StdExt::FunctionRawType_t<decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME)>, \
                                                                                                           StdExt::FunctionRawType_t<F> \
                                                                                                          > \
                                                                                          > \
                                                                          */ \
\
                                                                         > \
                                                                         /*////////////////////////////////////////////////////////////////////////// \
                                                                         // Don't move this to "std::enable_if_t" above (though we normally could).   \
                                                                         // GCC has a bug in C++17 only whereby if "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME"  \
                                                                         // below resolves to a non-function member then a hard-compiler error occurs \
                                                                         // since "StdExt::IsNoexcept_v" will fail (since it's not being passed an    \
                                                                         // actual function). SFINAE should trigger instead but it doesn't (an actual \
                                                                         // compiler error occurs). This is broken GCC behaviour that was fixed in    \
                                                                         // C++20 but moving this code below as we're doing circumvents the problem   \
                                                                         // in C++17. It would be rare for this to occur anyway however.              \
                                                                         //////////////////////////////////////////////////////////////////////////*/ \
                                                                         : std::bool_constant< StdExt::IsNoexcept_v<F> == StdExt::IsNoexcept_v<decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME)> > \
        { \
            /*///////////////////////////////////////////////////// \
            // Note: Neither of these "static_asserts" will ever    \
            // kick in in C++20 or later since the macros simply    \
            // resolve to whitespace in C++20 or later. The         \
            // corresponding concepts above kick in instead. Even   \
            // in C++17 however where they do resolve to actual     \
            // "static_asserts", the "static_asserts" still won't   \
            // ever kick in since specialization of this class      \
            // won't kick in unless the "static_asserts" hold (due  \
            // to what we're testing in the "std::enable_if_t" call \
            // above). They're therefore just for documentation     \
            // purposes only and consistency with the same          \
            // "static_asserts" in the primary template (which      \
            // *will* kick in in C++17 if T and/or F is illegal -   \
            // again, concepts above will trap things in C++20 and  \
            // later).                                              \
            /////////////////////////////////////////////////////*/ \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  TRAITS_FUNCTION_C F> \
        inline constexpr bool ClassHasNonOverloadedStatic##FUNCTION_OR_OPERATOR##_##NAME##_v = ClassHasNonOverloadedStatic##FUNCTION_OR_OPERATOR##_##NAME<T, F>::value;

    #define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION(NAME) DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR(Function, NAME, (NAME))

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    // For internal use only
    namespace Private
    {
        // Primary template
        template <typename T,
                  typename = void>
        struct HasFunctionCallOperatorWithOneTemplateArg : std::false_type
        {
        };

        // Partial specialization of above template
        template <typename T>
        struct HasFunctionCallOperatorWithOneTemplateArg<T,
                                                         std::enable_if_t< std::is_same_v< decltype(T().template operator()<void (AnyClass::*)()>()),
                                                                                           bool
                                                                                         >
                                                                         >
                                                        >
                                                        : std::true_type
        {
        };

        // Helper variable for above templates
        template <typename T>
        inline constexpr bool HasFunctionCallOperatorWithOneTemplateArg_v = HasFunctionCallOperatorWithOneTemplateArg<T>::value;

        // Primary template
        template <typename T,
                  typename UserTypeT,
                  typename = void>
        struct HasFunctionCallOperatorWithTwoTemplateArgs : std::false_type
        {
        };

        // Partial specialization of above template
        template <typename T,
                  typename UserTypeT>
        struct HasFunctionCallOperatorWithTwoTemplateArgs<T,
                                                          UserTypeT,
                                                          std::enable_if_t< std::is_same_v< decltype(T().template operator()<void (AnyClass::*)(), UserTypeT>()),
                                                                                            bool
                                                                                          >
                                                                          >
                                                         >
                                                         : std::true_type
        {
        };

        // Helper variable for above templates
        template <typename T,
                  typename UserTypeT>
        inline constexpr bool HasFunctionCallOperatorWithTwoTemplateArgs_v = HasFunctionCallOperatorWithTwoTemplateArgs<T, UserTypeT>::value;

        /////////////////////////////////////////////////
        // Primary template (undefined since our partial
        // specializations below will always kick in)
        /////////////////////////////////////////////////
        template <typename T,
                  typename UserTypeT,
                  typename = void>
        struct IsHasFunctionTraits;

        // Partial specialization of above template
        template <typename T,
                  typename UserTypeT>
        struct IsHasFunctionTraits<T,
                                   UserTypeT,
                                   std::enable_if_t< std::is_void_v<UserTypeT> >
                                  >
                                  : std::bool_constant< HasFunctionCallOperatorWithOneTemplateArg_v<T> >
        {
        };

        // Partial specialization of above template
        template <typename T,
                  typename UserTypeT>
        struct IsHasFunctionTraits<T,
                                   UserTypeT,
                                   std::enable_if_t< !std::is_void_v<UserTypeT> >
                                  >
                                  : std::bool_constant< HasFunctionCallOperatorWithTwoTemplateArgs_v<T, UserTypeT> >
        {
        };
    } // namespace Private

    template <typename T,
              typename UserTypeT>
    inline constexpr bool IsHasFunctionTraits_v = Private::IsHasFunctionTraits<T, UserTypeT>::value;
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    /////////////////////////////////////////////////////////////////////////
    // IsHasFunctionTraits_c concept. Used by macros
    // DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS and
    // DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_TRAITS in their
    // declarations of templates "ClassHasNonOverloadedFunctionTraits_##NAME"
    // and "ClassHasNonOverloadedStaticFunctionTraits_##NAME" respectively
    // (see macros for details). The following concept is applied to the
    // latter templates to constrain the template's "HasFunctionTraitsT"
    // template arg.
    /////////////////////////////////////////////////////////////////////////
    #if defined(USE_CONCEPTS)
        #if !defined(DECLARE_PUBLIC_MACROS_ONLY)
            template <typename T,
                      typename UserTypeT>
            concept IsHasFunctionTraits_c = IsHasFunctionTraits_v<T, UserTypeT>;
        #endif

        #define REQUIRES_IS_HAS_FUNCTION_TRAITS_C(T, UserTypeT) requires StdExt::IsHasFunctionTraits_c<T, UserTypeT>
        #define STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(T, UserTypeT)
    #else
        #define REQUIRES_IS_HAS_FUNCTION_TRAITS_C(T, UserTypeT)
        #define STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(T, UserTypeT) static_assert(StdExt::IsHasFunctionTraits_v<T, UserTypeT>, \
                                                                                 "An invalid type was passed for the " QUOTE_STRINGIFY(T) \
                                                                                 " template arg of a member function detection template " \
                                                                                 "created by the library's " \
                                                                                 "DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS or " \
                                                                                 "DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_TRAITS " \
                                                                                 "macros. " QUOTE_STRINGIFY(T) " must be a class/struct " \
                                                                                 "with a non-static, template-based function call operator " \
                                                                                 "as follows (both templates below identical except the " \
                                                                                 "2nd takes an optional user-defined (pass-through) " \
                                                                                 "template arg which you can pass via the \"UserTypeT\" " \
                                                                                 "template arg described under " \
                                                                                 "https://github.com/HexadigmSystems/FunctionTraits/#declare_class_has_non_overloaded_function_traits):\n" \
                                                                                 "\n" \
                                                                                 "template <TRAITS_FUNCTION_C F>\n" \
                                                                                 "constexpr bool operator()() const noexcept;\n" \
                                                                                 "\n" \
                                                                                 "template <TRAITS_FUNCTION_C F, typename UserTypeT>\n" \
                                                                                 "constexpr bool operator()() const noexcept;\n" \
                                                                                 "\n" \
                                                                                 "The above function call operators should return true if " \
                                                                                 "the template arg \"F\" meets your search criteria (usually " \
                                                                                 "by invoking the templates in this library), or false  " \
                                                                                 "otherwise. See link in this message above for details.");
    #endif // #if defined(USE_CONCEPTS)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    // For internal use only
    namespace Private
    {
        /////////////////////////////////////////////////
        // Primary template (undefined since our partial
        // specializations below will always kick in)
        /////////////////////////////////////////////////
        template <IS_CLASS_C HasFunctionTraitsT,
                  FREE_OR_MEMBER_FUNCTION_POINTER_C,
                  typename UserTypeT, typename = void>
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
        struct InvokeHasFunctionTraits;

        // // Partial specialization of (primary) template just above
        template <IS_CLASS_C HasFunctionTraitsT,
                  FREE_OR_MEMBER_FUNCTION_POINTER_C F,
                  typename UserTypeT>
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
        struct InvokeHasFunctionTraits<HasFunctionTraitsT,
                                       F,
                                       UserTypeT,
                                       std::enable_if_t< std::is_void_v<UserTypeT> >
                                      >
        {
            // See this constant for details
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concept kicks in in the template
                // declaration above instead (macro for these
                // above resolves to the "typename" keyword when
                // concepts aren't supported and
                // REQUIRES_IS_HAS_FUNCTION_TRAITS_C resolves to
                // whitespace, and the following "static_asserts"
                // are then used instead - note that
                // REQUIRES_IS_HAS_FUNCTION_TRAITS_C ensures that
                // "HasFunctionTraitsT" is also a class so no
                // need to call STATIC_ASSERT_IS_CLASS on it
                // below - it would be redundant)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT)
                STATIC_ASSERT_IS_FREE_OR_MEMBER_FUNCTION_POINTER(F)
            #endif

            ///////////////////////////////////////////////////////////
            // IMPORTANT: The following would be more cleanly handled
            // by inheriting this class from "std::bool_constant" and
            // specializing the latter on the following condition but
            // it leads to the MSVC compiler bug described in the
            // "InvokeHasFunctionTraits_v" comments further below (so
            // we handle the condition using the following variable
            // instead - it circumvents the bug)
            ///////////////////////////////////////////////////////////
            static constexpr bool Value = HasFunctionTraitsT().template operator()<F>();
        }; // struct InvokeHasFunctionTraits

        // Partial specialization of (primary) template above
        template <IS_CLASS_C HasFunctionTraitsT,
                  FREE_OR_MEMBER_FUNCTION_POINTER_C F,
                  typename UserTypeT>
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
        struct InvokeHasFunctionTraits<HasFunctionTraitsT,
                                       F,
                                       UserTypeT,
                                       std::enable_if_t< !std::is_void_v<UserTypeT> >
                                      >
        {
            // See this constant for details
            #if !defined(USE_CONCEPTS)
                //////////////////////////////////////////////////
                // Kicks in if concepts not supported, otherwise
                // corresponding concept kicks in in the template
                // declaration above instead (macro for these
                // above resolves to the "typename" keyword when
                // concepts aren't supported and
                // REQUIRES_IS_HAS_FUNCTION_TRAITS_C resolves to
                // whitespace, and the following "static_asserts"
                // are then used instead - note that
                // REQUIRES_IS_HAS_FUNCTION_TRAITS_C ensures that
                // "HasFunctionTraitsT" is also a class so no
                // need to call STATIC_ASSERT_IS_CLASS on it
                // below - it would be redundant)
                //////////////////////////////////////////////////
                STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT)
                STATIC_ASSERT_IS_FREE_OR_MEMBER_FUNCTION_POINTER(F)
            #endif

            ///////////////////////////////////////////////////////////
            // IMPORTANT: The following would be more cleanly handled
            // by inheriting this class from "std::bool_constant" and
            // specializing the latter on the following condition but
            // it leads to the MSVC compiler bug described in the
            // "InvokeHasFunctionTraits_v" comments just below (so we
            // handle the condition using the following variable
            // instead - it circumvents the bug)
            ///////////////////////////////////////////////////////////
            static constexpr bool Value = HasFunctionTraitsT().template operator()<F, UserTypeT>();
        }; // struct InvokeHasFunctionTraits

        ////////////////////////////////////////////////////////
        // Note: We rely on this not just because it's cleaner
        // than manually invoking it on-the-fly each time we
        // require it (though it's only called twice at this
        // writing), but because it also circumvents a MSFT bug
        // at this writing (when targeting C++17 - works in
        // C++20). See bug report I posted here (on Aug. 26,
        // 2024 but will likely be removed by MSFT at some
        // point):
        //
        //   Template bug invoking any C++ template-based
        //   "operator" in partial specialization
        //   https://developercommunity.visualstudio.com/t/Template-bug-invoking-any-C-template-b/10731541
        //
        // Basic issue demo'd here (affects operators only -
        // normal, non-operator functions are fine):
        //
        //   https://godbolt.org/z/cYKxb3f3a
        ////////////////////////////////////////////////////////
        template <IS_CLASS_C HasFunctionTraitsT,
                  FREE_OR_MEMBER_FUNCTION_POINTER_C F,
                  typename UserTypeT>
        #undef FREE_OR_MEMBER_FUNCTION_POINTER_C // Done with this just above (for internal use only)
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
        inline constexpr bool InvokeHasFunctionTraits_v = InvokeHasFunctionTraits<HasFunctionTraitsT, F, UserTypeT>::Value;
         // Done with this (for internal use only)
        #undef STATIC_ASSERT_IS_FREE_OR_MEMBER_FUNCTION_POINTER
    } // namespace Private
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

    /////////////////////////////////////////////////////////////////
    // DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR_TRAITS
    // (for internal use only)
    /////////////////////////////////////////////////////////////////
    #define DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR_TRAITS(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
        template <IS_CLASS_C T, \
                  IS_CLASS_C HasFunctionTraitsT, \
                  typename UserTypeT, \
                  typename = void> \
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT) \
        struct ClassHasNonOverloaded##FUNCTION_OR_OPERATOR##Traits_##NAME : std::false_type \
        { \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  IS_CLASS_C HasFunctionTraitsT, \
                  typename UserTypeT> \
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT) \
        struct ClassHasNonOverloaded##FUNCTION_OR_OPERATOR##Traits_##NAME<T, \
                                                                          HasFunctionTraitsT, \
                                                                          UserTypeT, \
                                                                          /*/////////////////////////////////////////// \
                                                                          // SFINAE succeeds only if                  / \
                                                                          // "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME" / \
                                                                          // is a non-overloaded, non-static member   / \
                                                                          // function                                 /
                                                                          ///////////////////////////////////////////*/ \
                                                                          std::enable_if_t< std::is_member_function_pointer_v<decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME)> > \
                                                                         > \
                                                                         : std::bool_constant< StdExt::Private::InvokeHasFunctionTraits_v<HasFunctionTraitsT, decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME), UserTypeT> > \
        { \
            /*////////////////////////////////////////////////////// \
            // Note: In C++17 only (since the following macros       \
            // resolve to nothing in C++20 - the corresponding       \
            // concepts above kick in instead), the first            \
            // "static_assert" below will never kick in since        \
            // specialization of this class won't kick in unless     \
            // the "static_assert" holds (due to what we're testing  \
            // in the "std::enable_if_t" call above). The first      \
            // "static_assert" is therefore just for documentation   \
            // purposes only and consistency with the same           \
            // "static_assert" in the primary template which can     \
            // kick in. The 2nd "static_assert" below *will* kick in \
            // however if the "std::enable_if_t" above succeeds but  \
            // "HasFunctionTraitsT" is illegal (again, in C++17      \
            // only)                                                 \
            //////////////////////////////////////////////////////*/ \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  IS_CLASS_C HasFunctionTraitsT, \
                  typename UserTypeT = void> \
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT) \
        inline constexpr bool ClassHasNonOverloaded##FUNCTION_OR_OPERATOR##Traits_##NAME##_v = ClassHasNonOverloaded##FUNCTION_OR_OPERATOR##Traits_##NAME<T, HasFunctionTraitsT, UserTypeT>::value;

    #define DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS(NAME) DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR_TRAITS(Function, NAME, (NAME))

    ////////////////////////////////////////////////////////////////////////
    // DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR_TRAITS
    // (for internal use only)
    ////////////////////////////////////////////////////////////////////////
    #define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR_TRAITS(FUNCTION_OR_OPERATOR, NAME, BRACKETED_CPP_NAME) \
        template <IS_CLASS_C T, \
                  IS_CLASS_C HasFunctionTraitsT, \
                  typename UserTypeT, \
                  typename = void> \
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT) \
        struct ClassHasNonOverloadedStatic##FUNCTION_OR_OPERATOR##Traits_##NAME : std::false_type \
        { \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  IS_CLASS_C HasFunctionTraitsT, \
                  typename UserTypeT> \
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT) \
        struct ClassHasNonOverloadedStatic##FUNCTION_OR_OPERATOR##Traits_##NAME<T, \
                                                                                HasFunctionTraitsT, \
                                                                                UserTypeT, \
                                                                                /*////////////////////////////////////////////// \
                                                                                // SFINAE succeeds only if                     / \
                                                                                // "T::STDEXT_UNBRACKET BRACKETED_CPP_NAME)"   / \
                                                                                // is a non-overloaded, static member function / \
                                                                                //////////////////////////////////////////////*/ \
                                                                                std::enable_if_t< StdExt::IsFreeFunctionPointer_v<decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME)> > \
                                                                               > \
                                                                               : std::bool_constant< StdExt::Private::InvokeHasFunctionTraits_v<HasFunctionTraitsT, decltype(&T::STDEXT_UNBRACKET BRACKETED_CPP_NAME), UserTypeT> > \
        { \
            /*////////////////////////////////////////////////////// \
            // Note: In C++17 only (since the following macros       \
            // resolve to nothing in C++20 - the corresponding       \
            // concepts above kick in instead), the first            \
            // "static_assert" below will never kick in since        \
            // specialization of this class won't kick in unless     \
            // the "static_assert" holds (due to what we're testing  \
            // in the "std::enable_if_t" call above). The first      \
            // "static_assert" is therefore just for documentation   \
            // purposes only and consistency with the same           \
            // "static_assert" in the primary template which can     \
            // kick in. The 2nd "static_assert" below *will* kick in \
            // however if the "std::enable_if_t" above succeeds but  \
            // "HasFunctionTraitsT" is illegal (again, in C++17      \
            // only)                                                 \
            //////////////////////////////////////////////////////*/ \
            STATIC_ASSERT_IS_CLASS(T) \
            STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT) \
\
            /*/////////////////////////////////////////////////////////////////////// \
            // Declare "Name" (tstring_view) member storing the target function       \
            // name (though probably rarely used by most)                             \
            //                                                                        \
            //    E.g.,                                                               \
            //    static constexpr auto Name = StdExt::tstring_view(_T("Whatever"));  \
            ///////////////////////////////////////////////////////////////////////*/ \
            DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR_NAME_VARIABLE(BRACKETED_CPP_NAME) \
        }; \
\
        template <IS_CLASS_C T, \
                  IS_CLASS_C HasFunctionTraitsT, \
                  typename UserTypeT = void> \
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT) \
        inline constexpr bool ClassHasNonOverloadedStatic##FUNCTION_OR_OPERATOR##Traits_##NAME##_v = ClassHasNonOverloadedStatic##FUNCTION_OR_OPERATOR##Traits_##NAME<T, HasFunctionTraitsT, UserTypeT>::value;

    #define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_TRAITS(NAME) DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR_TRAITS(Function, NAME, (NAME))

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)
    //////////////////////////////////////////////////////////////
    // For future use only so always true (undefined) at this
    // writing (for now). May be #defined (or removed) in a
    // future release however (still a work-in-progress for now
    // but deferred until reflection is available in C++26 - will
    // hopefully resolve several issues required to cleanly
    // implement operator detection).
    //////////////////////////////////////////////////////////////
    #if !defined(STDEXT_SUPPORTS_OPERATOR_DETECTION_TEMPLATES)
        // See this constant's definition for details
        #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)
            // For internal use only (we #undef it later)
            #define DECLARE_CLASS_HAS_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
                DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME)

            // For internal use only (we #undef it later)
            #define DECLARE_CLASS_HAS_STATIC_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
                DECLARE_CLASS_HAS_STATIC_FUNCTION_OR_OPERATOR(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME)

            // For internal use only (we #undef it later)
            #define DECLARE_CLASS_HAS_NON_OVERLOADED_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
                DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME)
        #else
            #define DECLARE_CLASS_HAS_OPERATOR(...)
            #define DECLARE_CLASS_HAS_STATIC_OPERATOR(...)
            #define DECLARE_CLASS_HAS_NON_OVERLOADED_OPERATOR(...)
        #endif // #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)

        // For internal use only (we #undef it later)
        #define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
            DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME)

        // For internal use only (we #undef it later)
        #define DECLARE_CLASS_HAS_NON_OVERLOADED_OPERATOR_TRAITS(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
            DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR_TRAITS(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME)

        // For internal use only (we #undef it later)
        #define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_OPERATOR_TRAITS(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
            DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR_TRAITS(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME)

    /////////////////////////////////////////////////
    // We never come through here at this writing
    // (for possible future use only - see matching
    // #if above for details)
    /////////////////////////////////////////////////
    #else
        ///////////////////////////////////////////////////////
        // For internal use only at this writing (enum of all
        // overloaded operators in C++). May be made publicly
        // available in a future release but design of the
        // operator detection templates that currently rely
        // on this enum still a work-in-progress (so not yet
        // intended for end-users)
        ///////////////////////////////////////////////////////
        enum class OperatorFunctions
        {
            // Arithmetic operators
            Add,                      // operator +
            Subtract,                 // operator -
            Multiply,                 // operator *
            Divide,                   // operator /
            Modulus,                  // operator %

            // Bitwise operators
            BitwiseXor,               // operator ^
            BitwiseAnd,               // operator &
            BitwiseOr,                // operator |
            BitwiseNot,               // operator ~
            LeftShift,                // operator <<
            RightShift,               // operator >>

            // Logical operators      // operator !
            LogicalNot,               // operator &&
            LogicalAnd,               // operator ||
            LogicalOr,

            // Assignment operators
            Assignment,               // operator =
            AddAssignment,            // operator +=
            SubtractAssignment,       // operator -=
            MultiplyAssignment,       // operator *=
            DivideAssignment,         // operator /=
            ModulusAssignment,        // operator %=
            BitwiseXorAssignment,     // operator ^=
            BitwiseAndAssignment,     // operator &=
            BitwiseOrAssignment,      // operator |=
            LeftShiftAssignment,      // operator <<=
            RightShiftAssignment,     // operator >>=

            // Comparison operators
            LessThan,                 // operator <
            GreaterThan,              // operator >
            EqualTo,                  // operator ==
            NotEqualTo,               // operator !=
            LessThanOrEqualTo,        // operator <=
            GreaterThanOrEqualTo,     // operator >=

            // Increment and Decrement operators
            Increment,                // operator ++
            Decrement,                // operator --

            // Other operators
            FunctionCall,             // operator ()
            ClassMemberAccess,        // operator ->
            PointerToMember,          // operator ->*
            Dereference,              // operator *
            Subscript,                // operator []
            AddressOf,                // operator &
            Comma,                    // operator ,

            // Memory allocation/deallocation operators
            New,                      // operator new
            NewArray,                 // operator new[]
            Delete,                   // operator delete
            DeleteArray,              // operator delete[]

            #if CPP23_OR_LATER
                CoAwait               // operator CoAwait
            #endif
        };

        // See this constant's definition for details
        #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)
            ////////////////////////////////////////////////////////////
            // For internal use only for now (still a work-in-progress
            // for possible future release)
            ////////////////////////////////////////////////////////////
            template <IS_CLASS_C T,
                      OperatorFunctions OperatorFunctionT,
                      TRAITS_FUNCTION_C F,
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct ClassHasOperator : std::false_type
            {
                STATIC_ASSERT_IS_CLASS(T)
                STATIC_ASSERT_IS_TRAITS_FUNCTION(F)
            };

            /////////////////////////////////////////////////////////////////////
            // For internal use only (the following macro, not the templates it
            // creates - we #undef the macro later). IMPORTANT: For now the
            // "ClassHasOperator" template declared below is not intended for
            // use by end-users. It's still a work-in-progress for possible
            // future release.
            /////////////////////////////////////////////////////////////////////
            #define DECLARE_CLASS_HAS_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
                DECLARE_CLASS_HAS_FUNCTION_OR_OPERATOR(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
\
                template <IS_CLASS_C T, TRAITS_FUNCTION_C F> \
                struct ClassHasOperator<T, \
                                        OperatorFunctions::OPERATOR_ENUM_NAME, \
                                        F, \
                                        std::enable_if_t< ClassHasOperator_##OPERATOR_ENUM_NAME##_v<T, F> > \
                                       > \
                                       : std::true_type \
                { \
                    /*///////////////////////////////////////////////////// \
                    // Note: Neither of these "static_asserts" will ever    \
                    // kick in in C++20 or later since the macros simply    \
                    // resolve to whitespace in C++20 or later. The         \
                    // corresponding concepts above kick in instead. Even   \
                    // in C++17 however where they do resolve to actual     \
                    // "static_asserts", the "static_asserts" still won't   \
                    // ever kick in since specialization of this class      \
                    // won't kick in unless the "static_asserts" hold (due  \
                    // to what we're testing in the "std::enable_if_t" call \
                    // above). They're therefore just for documentation     \
                    // purposes only and consistency with the same          \
                    // "static_asserts" in the primary template (which      \
                    // *will* kick in in C++17 if T and/or F is illegal -   \
                    // again, concepts above will trap things in C++20 and  \
                    // later).                                              \
                    /////////////////////////////////////////////////////*/ \
                    STATIC_ASSERT_IS_CLASS(T) \
                    STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
                };

            ////////////////////////////////////////////////////////////
            // For internal use only for now (still a work-in-progress
            // for possible future release)
            ////////////////////////////////////////////////////////////
            template <IS_CLASS_C T,
                      OperatorFunctions OperatorFunctionT,
                      TRAITS_FUNCTION_C F>
            inline constexpr bool ClassHasOperator_v = ClassHasOperator<T, OperatorFunctionT, F>::value;

            ////////////////////////////////////////////////////////////
            // For internal use only for now (still a work-in-progress
            // for possible future release)
            ////////////////////////////////////////////////////////////
            template <IS_CLASS_C T,
                      OperatorFunctions OperatorFunctionT,
                      TRAITS_FUNCTION_C F,
                      typename = void> // Arg for internal use only (users should never explicitly pass it)
            struct ClassHasStaticOperator : std::false_type
            {
                STATIC_ASSERT_IS_CLASS(T)
                STATIC_ASSERT_IS_TRAITS_FUNCTION(F)
            };

            ///////////////////////////////////////////////////////////////////////
            // For internal use only (the following macro, not the templates it
            // creates). IMPORTANT: For now the "ClassHasStaticOperator" template
            // declared below is not intended for use by end-users. It's still a
            // work-in-progress for possible future release.
            ///////////////////////////////////////////////////////////////////////
            #define DECLARE_CLASS_HAS_STATIC_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
                DECLARE_CLASS_HAS_STATIC_FUNCTION_OR_OPERATOR(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
\
                template <IS_CLASS_C T, TRAITS_FUNCTION_C F> \
                struct ClassHasStaticOperator<T, \
                                              OperatorFunctions::OPERATOR_ENUM_NAME, \
                                              F, \
                                              /*//////////////////////////////////////////////////////// \
                                              // The 1st condition below (the call to "static_cast")     \
                                              // will succeed SFINAE only if "T" has a static member     \
                                              // function called NAME with the same type as "F"          \
                                              // except for possible differences in their "noexcept"     \
                                              // specifier (but see IMPORTANT comment further below).    \
                                              // If so then here are the "noexcept" possibilities:       \
                                              //                                                         \
                                              //    F         T::NAME     SFINAE succeeds                \
                                              //    -         -------     ---------------                \
                                              //                          ?                              \
                                              //              noexcept    ?                              \
                                              //    noexcept              X (illegal cast)               \
                                              //    noexcept  noexcept    ?                              \
                                              //                                                         \
                                              //                                                         \
                                              // If the "static_cast" succeeds (SFINAE fails otherwise)  \
                                              // then as per the above table it's guaranteed that "F"    \
                                              // and "T::NAME" are identical static member function      \
                                              // types unless we're dealing with the "noexcept"          \
                                              // mismatch on the 2nd row above (if dealing with the      \
                                              // mismatch on the 3rd row then the "static_cast" itself   \
                                              // is guaranteed to fail since a non-noexcept function     \
                                              // can't be cast to one that's noexcept - SFINAE is        \
                                              // therefore guaranteed to fail). The 2nd condition below  \
                                              // is therefore used to catch the "noexcept" mismatch on   \
                                              // the 2nd row in the table. It simply compares the        \
                                              // "noexcept" for "F" and "T::NAME", returning true if     \
                                              // they're equal (meaning we must be dealing with the 1st  \
                                              // or 4th row above), or false otherwise (meaning we must  \
                                              // be dealing with the 2nd row above).                     \
                                              //                                                         \
                                              // The upshot is that SFINAE will succeed only if "T" has  \
                                              // a static member function called NAME with the same      \
                                              // type as "F" including their noexcept specifiers (their  \
                                              // types exactly match IOW). We're therefore dealing with  \
                                              // row 1 and 4 above only. SFINAE always fails for row 2   \
                                              // and 3 ("F" and "T::F" identical except for "noexcept"   \
                                              // so they don't precisely match).                         \
                                              //                                                         \
                                              // IMPORTANT                                               \
                                              // ---------                                               \
                                              // The above assumes that "T" is a class or struct type    \
                                              // and "F" is a free function type. This is enforced       \
                                              // however by the template's IS_CLASS_C and                \
                                              // TRAITS_FUNCTION_C concepts in C++20 or later or, for    \
                                              // C++17, the corresponding static_asserts seen in the     \
                                              // body of the class below (and its primary template       \
                                              // class). The calls in "std::enable_if_t" below can       \
                                              // therefore assume that "T" is a class or struct type     \
                                              // and "F" is a free function type. Compilation is         \
                                              // guaranteed to fail otherwise (via our concept in C++20  \
                                              // or later or our "static_assert" otherwise)              \
                                              ////////////////////////////////////////////////////////*/ \
                                              std::enable_if_t< ClassHasStaticOperator_##OPERATOR_ENUM_NAME##_v<T, F > > \
                                             > \
                                             : std::true_type \
                { \
                    /*///////////////////////////////////////////////////// \
                    // Note: Neither of these "static_asserts" will ever    \
                    // kick in in C++20 or later since the macros simply    \
                    // resolve to whitespace in C++20 or later. The         \
                    // corresponding concepts above kick in instead. Even   \
                    // in C++17 however where they do resolve to actual     \
                    // "static_asserts", the "static_asserts" still won't   \
                    // ever kick in since specialization of this class      \
                    // won't kick in unless the "static_asserts" hold (due  \
                    // to what we're testing in the "std::enable_if_t" call \
                    // above). They're therefore just for documentation     \
                    // purposes only and consistency with the same          \
                    // "static_asserts" in the primary template (which      \
                    // *will* kick in in C++17 if T and/or F is illegal -   \
                    // again, concepts above will trap things in C++20 and  \
                    // later).                                              \
                    /////////////////////////////////////////////////////*/ \
                    STATIC_ASSERT_IS_CLASS(T) \
                    STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
                };

            ////////////////////////////////////////////////////////////
            // For internal use only for now (still a work-in-progress
            // for possible future release)
            ////////////////////////////////////////////////////////////
            template <IS_CLASS_C T,
                      OperatorFunctions OperatorFunctionT,
                      TRAITS_FUNCTION_C F>
            inline constexpr bool ClassHasStaticOperator_v = ClassHasStaticOperator<T, OperatorFunctionT, F>::value;

            ////////////////////////////////////////////////////////////
            // For internal use only for now (still a work-in-progress
            // for possible future release)
            ////////////////////////////////////////////////////////////
            template <IS_CLASS_C T,
                      OperatorFunctions OperatorFunctionT,
                      TRAITS_FUNCTION_C F,
                      typename = void>
            struct ClassHasNonOverloadedOperator : std::false_type
            {
                STATIC_ASSERT_IS_CLASS(T)
                STATIC_ASSERT_IS_TRAITS_FUNCTION(F)
            };

            /////////////////////////////////////////////////////////////////////
            // For internal use only (the following macro, not the templates it
            // creates - we #undef the macro later). IMPORTANT: For now the
            // "ClassHasNonOverloadedOperator" template declared below is not
            // intended for use by end-users. It's still a work-in-progress for
            // possible future release.
            /////////////////////////////////////////////////////////////////////
            #define DECLARE_CLASS_HAS_NON_OVERLOADED_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
                DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
\
                template <IS_CLASS_C T, TRAITS_FUNCTION_C F> \
                struct ClassHasNonOverloadedOperator<T, \
                                                     OperatorFunctions::OPERATOR_ENUM_NAME, \
                                                     F, \
                                                     std::enable_if_t< ClassHasNonOverloadedOperator_##OPERATOR_ENUM_NAME##_v<T, F> > \
                                                    > : std::true_type \
                { \
                    /*///////////////////////////////////////////////////// \
                    // Note: Neither of these "static_asserts" will ever    \
                    // kick in in C++20 or later since the macros simply    \
                    // resolve to whitespace in C++20 or later. The         \
                    // corresponding concepts above kick in instead. Even   \
                    // in C++17 however where they do resolve to actual     \
                    // "static_asserts", the "static_asserts" still won't   \
                    // ever kick in since specialization of this class      \
                    // won't kick in unless the "static_asserts" hold (due  \
                    // to what we're testing in the "std::enable_if_t" call \
                    // above). They're therefore just for documentation     \
                    // purposes only and consistency with the same          \
                    // "static_asserts" in the primary template (which      \
                    // *will* kick in in C++17 if T and/or F is illegal -   \
                    // again, concepts above will trap things in C++20 and  \
                    // later).                                              \
                    /////////////////////////////////////////////////////*/ \
                    STATIC_ASSERT_IS_CLASS(T) \
                    STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
                };

            ////////////////////////////////////////////////////////////
            // For internal use only for now (still a work-in-progress
            // for possible future release)
            ////////////////////////////////////////////////////////////
            template <IS_CLASS_C T,
                      OperatorFunctions OperatorFunctionT,
                      TRAITS_FUNCTION_C F>
            inline constexpr bool ClassHasNonOverloadedOperator_v = ClassHasNonOverloadedOperator<T, OperatorFunctionT, F>::value;
        #endif // #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)

        ////////////////////////////////////////////////////////////
        // For internal use only for now (still a work-in-progress
        // for possible future release)
        ////////////////////////////////////////////////////////////
        template <IS_CLASS_C T,
                  OperatorFunctions OperatorFunctionT,
                  TRAITS_FUNCTION_C F,
                  typename = void> // Arg for internal use only (users should never explicitly pass it)
        struct ClassHasNonOverloadedStaticOperator : std::false_type
        {
            STATIC_ASSERT_IS_CLASS(T)
            STATIC_ASSERT_IS_TRAITS_FUNCTION(F)
        };

        /////////////////////////////////////////////////////////////////////
        // For internal use only (the following macro, not the templates it
        // creates - we #undef the macro later). IMPORTANT: For now the
        // "ClassHasNonOverloadedStaticOperator" template declared below is
        // not intended for use by end-users. It's still a work-in-progress
        // for possible future release.
        /////////////////////////////////////////////////////////////////////
        #define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
            DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
\
            template <IS_CLASS_C T, TRAITS_FUNCTION_C F> \
            struct ClassHasNonOverloadedStaticOperator<T, \
                                                       OperatorFunctions::OPERATOR_ENUM_NAME, \
                                                       F, \
                                                       std::enable_if_t< ClassHasNonOverloadedStaticOperator_##OPERATOR_ENUM_NAME##_v<T, F> > \
                                                      > : std::true_type \
            { \
                /*///////////////////////////////////////////////////// \
                // Note: Neither of these "static_asserts" will ever    \
                // kick in in C++20 or later since the macros simply    \
                // resolve to whitespace in C++20 or later. The         \
                // corresponding concepts above kick in instead. Even   \
                // in C++17 however where they do resolve to actual     \
                // "static_asserts", the "static_asserts" still won't   \
                // ever kick in since specialization of this class      \
                // won't kick in unless the "static_asserts" hold (due  \
                // to what we're testing in the "std::enable_if_t" call \
                // above). They're therefore just for documentation     \
                // purposes only and consistency with the same          \
                // "static_asserts" in the primary template (which      \
                // *will* kick in in C++17 if T and/or F is illegal -   \
                // again, concepts above will trap things in C++20 and  \
                // later).                                              \
                /////////////////////////////////////////////////////*/ \
                STATIC_ASSERT_IS_CLASS(T) \
                STATIC_ASSERT_IS_TRAITS_FUNCTION(F) \
            };

        ////////////////////////////////////////////////////////////
        // For internal use only for now (still a work-in-progress
        // for possible future release)
        ////////////////////////////////////////////////////////////
        template <IS_CLASS_C T,
                  OperatorFunctions OperatorFunctionT,
                  TRAITS_FUNCTION_C F>
        inline constexpr bool ClassHasNonOverloadedStaticOperator_v = ClassHasNonOverloadedStaticOperator<T, OperatorFunctionT, F>::value;

        template <IS_CLASS_C T,
                  OperatorFunctions OperatorFunctionT,
                  IS_CLASS_C HasFunctionTraitsT,
                  typename UserTypeT,
                  typename = void> // Arg for internal use only (users should never explicitly pass it)
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
        struct ClassHasNonOverloadedOperatorTraits : std::false_type
        {
            STATIC_ASSERT_IS_CLASS(T)
            STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT)
        };

        /////////////////////////////////////////////////////////////////////
        // For internal use only (the following macro, not the templates it
        // creates - we #undef the macro later). IMPORTANT: For now the
        // "ClassHasNonOverloadedOperatorTraits" template declared below is
        // not intended for use by end-users. It's still a work-in-progress
        // for possible future release.
        /////////////////////////////////////////////////////////////////////
        #define DECLARE_CLASS_HAS_NON_OVERLOADED_OPERATOR_TRAITS(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
            DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_OR_OPERATOR_TRAITS(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
\
            template <IS_CLASS_C T,
                      IS_CLASS_C HasFunctionTraitsT,
                      typename UserTypeT> \
            REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT) \
            struct ClassHasNonOverloadedOperatorTraits<T, \
                                                        OperatorFunctions::OPERATOR_ENUM_NAME, \
                                                        HasFunctionTraitsT, \
                                                        UserTypeT, \
                                                        std::enable_if_t< ClassHasNonOverloadedOperatorTraits_##OPERATOR_ENUM_NAME##_v<T, HasFunctionTraitsT, UserTypeT> > \
                                                       > \
                                                       : std::true_type \
            { \
                /*///////////////////////////////////////////////////// \
                // Note: Neither of these "static_asserts" will ever    \
                // kick in in C++20 or later since the macros simply    \
                // resolve to whitespace in C++20 or later. The         \
                // corresponding concepts above kick in instead. Even   \
                // in C++17 however where they do resolve to actual     \
                // "static_asserts", the "static_asserts" still won't   \
                // ever kick in since specialization of this class      \
                // won't kick in unless the "static_asserts" hold (due  \
                // to what we're testing in the "std::enable_if_t" call \
                // above). They're therefore just for documentation     \
                // purposes only and consistency with the same          \
                // "static_asserts" in the primary template (which      \
                // *will* kick in in C++17 if T and/or                  \
                // "HasFunctionTraitsT" is illegal -  again, concepts   \
                // above will trap things in C++20 and later).          \                                   \
                /////////////////////////////////////////////////////*/ \
                STATIC_ASSERT_IS_CLASS(T) \
                STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT) \
            };

        ////////////////////////////////////////////////////////////
        // For internal use only for now (still a work-in-progress
        // for possible future release)
        ////////////////////////////////////////////////////////////
        template <IS_CLASS_C T,
                  OperatorFunctions OperatorFunctionT,
                  IS_CLASS_C HasFunctionTraitsT,
                  typename UserTypeT = void> // Arg for internal use only (users should never explicitly pass it)
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
        inline constexpr bool ClassHasNonOverloadedOperatorTraits_v = ClassHasNonOverloadedOperatorTraits<T, OperatorFunctionT, HasFunctionTraitsT, UserTypeT>::value;

        // For internal use only
        template <IS_CLASS_C T,
                  OperatorFunctions OperatorFunctionT,
                  IS_CLASS_C HasFunctionTraitsT,
                  typename UserTypeT,
                  typename = void> // Arg for internal use only (users should never explicitly pass it)
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
        struct ClassHasNonOverloadedStaticOperatorTraits : std::false_type
        {
            STATIC_ASSERT_IS_CLASS(T)
            STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT)
        };

        /////////////////////////////////////////////////////////////////////
        // For internal use only (the following macro, not the templates it
        // creates - we #undef the macro later). IMPORTANT: For now the
        // "ClassHasNonOverloadedStaticOperatorTraits" template declared
        // below is not intended for use by end-users. It's still a
        // work-in-progress for possible future release.
        ////////////////////////////////////////////////////////////////////
        #define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_OPERATOR_TRAITS(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
            DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_OR_OPERATOR_TRAITS(Operator, OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
\
            template <IS_CLASS_C T, \
                      IS_CLASS_C HasFunctionTraitsT, \
                      typename UserTypeT> \
            REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT) \
            struct ClassHasNonOverloadedStaticOperatorTraits<T, \
                                                              OperatorFunctions::OPERATOR_ENUM_NAME, \
                                                              HasFunctionTraitsT, \
                                                              UserTypeT, \
                                                              std::enable_if_t< ClassHasNonOverloadedStaticOperatorTraits_##OPERATOR_ENUM_NAME##_v<T, HasFunctionTraitsT, UserTypeT> > \
                                                             > \
                                                             : std::true_type \
            { \
                /*///////////////////////////////////////////////////// \
                // Note: Neither of these "static_asserts" will ever    \
                // kick in in C++20 or later since the macros simply    \
                // resolve to whitespace in C++20 or later. The         \
                // corresponding concepts above kick in instead. Even   \
                // in C++17 however where they do resolve to actual     \
                // "static_asserts", the "static_asserts" still won't   \
                // ever kick in since specialization of this class      \
                // won't kick in unless the "static_asserts" hold (due  \
                // to what we're testing in the "std::enable_if_t" call \
                // above). They're therefore just for documentation     \
                // purposes only and consistency with the same          \
                // "static_asserts" in the primary template (which      \
                // *will* kick in in C++17 if T and/or                  \
                // "HasFunctionTraitsT" is illegal -  again, concepts   \
                // above will trap things in C++20 and later).          \                                   \
                /////////////////////////////////////////////////////*/ \
                STATIC_ASSERT_IS_CLASS(T) \
                STATIC_ASSERT_IS_HAS_FUNCTION_TRAITS(HasFunctionTraitsT, UserTypeT) \
            };

        ////////////////////////////////////////////////////////////
        // For internal use only for now (still a work-in-progress
        // for possible future release)
        ////////////////////////////////////////////////////////////
        template <IS_CLASS_C T,
                  OperatorFunctions OperatorFunctionT,
                  IS_CLASS_C HasFunctionTraitsT,
                  typename UserTypeT = void>
        REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
        inline constexpr bool ClassHasNonOverloadedStaticOperatorTraits_v = ClassHasNonOverloadedStaticOperatorTraits<T, OperatorFunctionT, HasFunctionTraitsT, UserTypeT>::value;
    #endif // #if !defined(STDEXT_SUPPORTS_OPERATOR_DETECTION_TEMPLATES)

    // For internal use only (we #undef it later)
    #define DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
        DECLARE_CLASS_HAS_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
        DECLARE_CLASS_HAS_NON_OVERLOADED_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
        DECLARE_CLASS_HAS_NON_OVERLOADED_OPERATOR_TRAITS(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME)

    // For internal use only (we #undef it later)
    #define DECLARE_CLASS_HAS_STATIC_OPERATORS(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
        DECLARE_CLASS_HAS_STATIC_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
        DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_OPERATOR(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME) \
        DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_OPERATOR_TRAITS(OPERATOR_ENUM_NAME, BRACKETED_OPERATOR_CPP_NAME)

    // Can also be a static member starting in C++23 (see just below)
    DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(FunctionCall, (operator()))

    // https://wg21.link/P1169R4 (C++23 or later)
    #if __cpp_static_call_operator
        // Can be an (optionally) static member starting in C++23
        DECLARE_CLASS_HAS_STATIC_OPERATORS(FunctionCall, (operator()))
    #endif

    //////////////////////////////////////////////////////////////
    // Always false (undefined) at this writing (for internal use
    // only). May be #defined (or removed) in a future release
    // (still a work-in-progress for now but deferred until
    // reflection is available in C++26 - will hopefully resolve
    // several issues required to cleanly implement operator
    // detection).
    //////////////////////////////////////////////////////////////
    #if defined(STDEXT_SUPPORTS_OPERATOR_DETECTION_TEMPLATES)
        /////////////////////////////////////////
        // Arithmetic operators
        /////////////////////////////////////////
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Add, (operator+))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Subtract, (operator-))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Multiply, (operator*))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Divide, (operator/))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Modulus, (operator%))

        /////////////////////////////////////////
        // Bitwise operators
        /////////////////////////////////////////
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(BitwiseXor, (operator^))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(BitwiseAnd, (operator&))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(BitwiseOr, (operator|))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(BitwiseNot, (operator~))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(LeftShift, (operator<<))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(RightShift, (operator>>))

        /////////////////////////////////////////
        // Logical operators
        /////////////////////////////////////////
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(LogicalNot, (operator!))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(LogicalAnd, (operator&&))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(LogicalOr, (operator||))

        /////////////////////////////////////////
        // Assignment operators
        /////////////////////////////////////////
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Assignment, (operator=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(AddAssignment, (operator+=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(SubtractAssignment, (operator-=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(MultiplyAssignment, (operator*=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(DivideAssignment, (operator/=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(ModulusAssignment, (operator%=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(BitwiseXorAssignment, (operator^=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(BitwiseAndAssignment, (operator&=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(BitwiseOrAssignment, (operator|=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(LeftShiftAssignment, (operator<<=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(RightShiftAssignment, (operator>>=))

        /////////////////////////////////////////
        // Comparison operators
        /////////////////////////////////////////
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(LessThan, (operator<))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(GreaterThan, (operator>))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(EqualTo, (operator==))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(NotEqualTo, (operator!=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(LessThanOrEqualTo, (operator<=))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(GreaterThanOrEqualTo, (operator>=))

        /////////////////////////////////////////
        // Increment and Decrement operators
        /////////////////////////////////////////
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Increment, (operator++))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Decrement, (operator--))

        /////////////////////////////////////////
        // Other operators
        /////////////////////////////////////////
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(ClassMemberAccess, (operator->)) // See "Class member access" in the C++ standard
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(PointerToMember, (operator->)) // See "Pointer-to-member operators" in the C++ standard
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Dereference, (operator*))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Subscript, (operator[])) // Can also be a static member starting in C++23 (see below)
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(AddressOf, (operator&))
        DECLARE_CLASS_HAS_NON_STATIC_OPERATORS(Comma, (operator,))

        #if CPP23_OR_LATER
            DECLARE_CLASS_HAS_STATIC_OPERATORS(CoAwait, (operatorCoAwait)) // Not available until C++23
        #endif

        //////////////////////////////////////////////////////////////////
        // Memory allocation/deallocation operators
        //
        // IMPORTANT
        // ---------
        // "operator delete" below (both of them) are always static even
        // if declared without the "static" keyword. C++ always
        // implicitly adds the "static" keyword if not already
        // (explicitly) declared that way! They can never be non-static
        // IOW. Also, if the "noexcept" keyword isn't explicitly applied
        // when the "delete" operators are declared, then they *may* be
        // explicitly declared "noexcept" depending on the compiler. This
        // is because destructors themselves are implicitly "noexcept"
        // when not explicitly declared, so even if a destructor *is*
        // explicitly declared, the compiler may still implicitly declare
        // it as "noexcept" if the "noexcept" keyword isn't explicitly
        // applied. MSVC, Clang and Intel compilers do this for instance
        // (automatically add the "noexcept" keyword if not explicitly
        // applied), but GCC doesn't! (at this writing). Therefore, when
        // users use the following templates declared by the two "delete"
        // macros below:
        //
        //   1) ClassHasOperator_Delete_v
        //   2) ClassHasNonOverloadedOperator_Delete_v
        //   3) ClassHasNonOverloadedOperatorFunctionTraits_Delete_v
        //   4) ClassHasOperator_DeleteArray_v
        //   5) ClassHasNonOverloadedOperator_Delete_v
        //   6) ClassHasNonOverloadedOperatorFunctionTraits_Delete_v
        //
        // The "operator delete" and "operator delete[]" method in the
        // class passed via the 1st template arg of the above templates
        // may or may not be declared as "noexcept" depending on whether
        // these destructors were explicitly declared in the class itself
        // (if they were then they're always implicitly "noexcept") and
        // (assuming they are explicitly declared in the class), whether
        // the "noexcept" keyword is explicitly declared for them. If not
        // explicitly declared (in a destructor that is explicitly
        // declared), then it's implementation defined as to whether
        // these "delete" methods are "noexcept" or not (again, in MSVC,
        // Clang and Intel they will be "noexcept", but in GCC they won't
        // be - the behavior is therefore inconsistent so caution
        // advised). The upshot is that the above templates can
        // potentially return true in one compiler (MSVC, Clang and
        // Intel) and false in another (GCC). Users who write their own
        // "operator delete" and "operator delete[]" functions should
        // therefore always explicitly add the "noexcept" keyword even
        // if setting it to "noexcept(false)" (though destructors should
        // normally be "noexcept(true) - exceptions shouldn't be thrown
        // from destructors). Doing so eliminates any inconsistencies
        // among compilers as described, and makes the behavior clear for
        // the reader (who might mistakingly interpret the absence of the
        // "noexcept" keyword as meaning the destructor can potentially
        // throw when in fact it won't thrown in some compilers since
        // they'll implicitly declare it as "noexcept")
        //////////////////////////////////////////////////////////////////
        DECLARE_CLASS_HAS_STATIC_OPERATORS(New, (operator new))
        DECLARE_CLASS_HAS_STATIC_OPERATORS(NewArray, (operator new[]))
        DECLARE_CLASS_HAS_STATIC_OPERATORS(Delete, (operator delete))
        DECLARE_CLASS_HAS_STATIC_OPERATORS(DeleteArray, (operator delete[]))

        // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2589r1.pdf (C++23 or later)
        #if __cpp_multidimensional_subscript
            // Can be (optionally) static starting in C++23
            DECLARE_CLASS_HAS_STATIC_OPERATORS(Subscript, (operator[]))
        #endif
    #endif // #if defined(STDEXT_SUPPORTS_OPERATOR_DETECTION_TEMPLATES)

    // Done with these (for internal use only)
    #undef DECLARE_CLASS_HAS_STATIC_OPERATORS
    #undef DECLARE_CLASS_HAS_NON_STATIC_OPERATORS
    #undef DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_OPERATOR_TRAITS
    #undef DECLARE_CLASS_HAS_NON_OVERLOADED_OPERATOR_TRAITS
    #undef DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_OPERATOR
    #undef DECLARE_CLASS_HAS_NON_OVERLOADED_OPERATOR
    #undef DECLARE_CLASS_HAS_STATIC_OPERATOR
    #undef DECLARE_CLASS_HAS_OPERATOR
#endif // #if !defined(DECLARE_PUBLIC_MACROS_ONLY)

#if !defined(DECLARE_PUBLIC_MACROS_ONLY)

////////////////////////////////////////////////////////////////
// Done with these (#defined only if STDEXT_SUPPORT_DEPRECATED
// is #defined - for internal use only)
////////////////////////////////////////////////////////////////
#undef ENUM_DEPRECATED_DUE_TO_NAME_CHANGE
#undef TEMPLATE_DEPRECATED_DUE_TO_NAME_CHANGE
#undef ELEMENT_DEPRECATED_DUE_TO_NAME_CHANGE
} // namespace StdExt
#else
    // Done with this (for internal use only)
    #undef DECLARE_PUBLIC_MACROS_ONLY
#endif

// Done with this (if currently #defined - for internal use only)
#if defined(MSVC_FROM_VISUAL_STUDIO_2017)
    #undef MSVC_FROM_VISUAL_STUDIO_2017
#endif

#endif // #if CPP17_OR_LATER

#endif // #ifndef FUNCTION_TRAITS (#include guard)