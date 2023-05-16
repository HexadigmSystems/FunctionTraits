#ifndef TYPETRAITS
#define TYPETRAITS

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
// Contains miscellaneous type traits similar to those in the native C++
// header <type_traits>. Supported in Microsoft, GCC, Clang and Intel
// compilers only at this writing (C++17 and later - the check for
// CPP17_OR_LATER just below causes all code to be preprocessed out
// otherwise). Note that this file depends on (#includes) "CompilerVersions.h"
// as well. All other dependencies are native C++ headers with the exception
// of the native Windows header "tchar.h" on MSFT platforms, which must also
// be in the #include search path (and normally will be). Note that all
// declarations in this file are in namespace "StdExt". Everything is
// available for public use except items declared in (nested) namespace
// "Private" (reserved for internal use), and in some (limited) cases,
// certain macros outside of namespace "Private" (but labeled as private in
// their comments).
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Our header containing mostly #defined C++ version constants
// (indicating which version of C++ is in effect which we can
// test for below as required). A few other compiler-related
// declarations also exist however. Note that we #include this
// first so we can immediately start using these version
// constants (in particular CPP17_OR_LATER seen just below - we
// don't support earlier versions)
////////////////////////////////////////////////////////////////
#include "CompilerVersions.h"

//////////////////////////////////////////////////////////////
// This header supports C++17 and later only. All code below
// ignored (preprocessed out) otherwise.
//////////////////////////////////////////////////////////////
#if CPP17_OR_LATER

// Standard C/C++ headers
#include <algorithm>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

// Standard C++ header
#include <string_view>

/////////////////////////////////////////////////////////////
// MSFT compiler? Note that Intel also #defines _MSC_VER on
// Windows, as well as _MSC_FULL_VER and _MSC_EXTENSIONS.
// See the following:
//
// https://github.com/cpredef/predef/blob/master/Compilers.md#user-content-microsoft-visual-c
// https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-8/additional-predefined-macros.html
/////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
    #include <tchar.h>
#endif

namespace StdExt
{

/////////////////////////////////////////////////////
// For use in any "static_assert" where the 1st arg
// is always false. Can't use "false" directly or
// it will always trigger the "static_assert" (even
// if that code is never called or instantiated).
// Making it a (template) dependent name here
// eliminates the problem (use the following IOW
// instead of "false" directly). See:
// 
//   https://artificial-mind.net/blog/2020/10/03/always-false
/////////////////////////////////////////////////////
template <typename...>
inline constexpr bool AlwaysFalse = false;

/////////////////////////////////////////////////////////////////////
// Private namespace (for internal use only) used to implement
// "TypeName_v" declared just after this namespace. Allows you to
// retrieve the type name for any type as a compile-time string
// (std::basic_string_view). See "TypeName_v" for full details (it
// just defers to "TypeNameImpl::Get()" in the following namespace).
/////////////////////////////////////////////////////////////////////
namespace Private
{
    //////////////////////////////////////////////////////////////////////
    // Implementation class for variable template "TypeName_v" (declared
    // just after this class but outside of this "Private" namespace so
    // for public use). The latter variable just invokes static member
    // "Get()" below which carries out all the work. See "TypeName_v" for
    // complete details        
    //
    // IMPORTANT:
    //  ---------
    // Note that the implementation below relies on the predefined string
    // __PRETTY_FUNCTION_ or (for MSFT only) __FUNCSIG__ (Google these
    // for details). All implementations you can find on the web normally
    // rely on these as does our own "Get()" member below, but unlike
    // most other implementations I've seen, ours doesn't require any
    // changes should you modify any part of the latter function's
    // fully-qualified name or signature (affecting the value of the
    // above predefined strings). Most other implementations I've seen
    // would require changes, even though they should normally be very
    // simple changes (trivial usually but changes nevertheless). Our
    // implementation doesn't require any so users can move the following
    // code to another namespace if they wish, change the function's
    // class name or any part its signature without breaking anything
    // (well, except for its "noexcept" specifier on MSFT platforms only,
    // but nobody will ever need to change this regardless of platform).
    // Note that the code is fairly small and clean notwithstanding first
    // impressions, lengthy only because of the many comments (the code
    // itself is fairly short and digestible however). It will only break
    // normally if a compiler vendor makes a breaking change to
    // __PRETTY_FUNCTION__ or (for MSFT only) __FUNCSIG__, but this will
    // normally be caught by judicious use of "static_asserts" in the
    // implementation and just after the following class itself (where we
    // arbitrarily test it with a float to make sure it returns "float",
    // a quick and dirty test but normally reliable)
    //////////////////////////////////////////////////////////////////////
    class TypeNameImpl
    {
    public:
            
        ////////////////////////////////////////////////////////////////////
        // Implementation function called by variable template "TypeName_v"
        // (just after the "Private" namespace this class is declared in).
        // The following function does all the work. See "TypeName_v" for
        // details.
        ////////////////////////////////////////////////////////////////////
        template <typename T>
        static constexpr tstring_view Get() noexcept
        {
            //////////////////////////////////////////////////////////
            // Extract template arg "T" (whatever string it resolves
            // to) from __PRETTY_FUNCTION__ or (for MSFT only)
            // __FUNCSIG__. Returns it as a "tstring_view" which
            // remains alive for the life of the app (since it's just
            // a view into the latter string).
            //////////////////////////////////////////////////////////
            return GetPrettyFunction<T>().substr(GetTypeNameOffset(), // Offset of the type in __PRETTY_FUNCTION__
                                                                      // or (for MSFT only) __FUNCSIG__
                                                 GetTypeNameLen<T>()); // Length of the type in __PRETTY_FUNCTION__
                                                                       // or (for MSFT only) __FUNCSIG__
        }

    private:
        /////////////////////////////////////////////////////////////////////////////////
        // GetPrettyFunction(). Returns the predefined string constant __PRETTY_FUNCTION__
        // or (for MSFT only) __FUNCSIG__. Returns these as a "tstring_view" which lives
        // for the life of the app (since it's just a view into the latter strings which
        // are always static). Assuming template arg "T" is a float for instance, each
        // resolves to the following (where the first three rows show the offsets for
        // your guidance only, and the rows just after show the actual value of the
        // above strings for the compilers we currently support):
        //                                                                                                                                 1         1         1         1         1
        //                                       1         2         3         4         5         6         7         8         9         0         1         2         3         4
        //                             0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901
        //   Clang:                    static auto StdExt::Private::TypeNameImpl::GetPrettyFunction() [T = float]
        //   GCC (1 of 2 - see below): static constexpr auto StdExt::Private::TypeNameImpl::GetPrettyFunction() [with T = float]
        //   GCC (2 of 2 - see below): static constexpr tstring_view StdExt::Private::TypeNameImpl::GetPrettyFunction() [with T = float; tstring_view = std::basic_string_view<char>]
        //   Intel:                    static constexpr auto StdExt::Private::TypeNameImpl::GetPrettyFunction() noexcept [with T = float]
        //   Microsoft:                auto __cdecl StdExt::Private::TypeNameImpl::GetPrettyFunction<float>(void) noexcept
        //
        // GCC
        // ---
        // Note that unlike the other compilers, two possible formats exist for GCC as
        // seen above (the others have one), where the one used depends on the return
        // type of "GetPrettyFunction()" itself. GCC format "1 of 2" above is the one
        // currently in effect at this writing since "GetPrettyFunction()" currently
        // returns "auto", which just resolves to "std::basic_string_view<TCHAR>"
        // (unless someone changed the return type since this writing which is safely
        // handled but read on). Because "GetPrettyFunction()" is *not* returning an
        // alias for another type in this case (it's returning "auto" which is not
        // treated as an alias by GCC), GCC uses format "1 of 2". If the return type of
        // "GetPrettyFunction()" is ever changed however so that it returns an alias
        // instead, such as "tstring_view" seen in "2 of 2" above (which is an alias for
        // "std::basic_string_view<TCHAR>" - note that TCHAR always resolves to "char"
        // on GCC), then GCC uses format "2 of 2" above instead. In this case it displays
        // (resolves) this alias at the end of __PRETTY_FUNCTION__ as seen. This refers
        // to the "tstring_view = std::basic_string_view<char>" portion of the "2 of 2"
        // string above. It removes this portion of the string in "1 of 2" however since
        // the return value of "GetPrettyFunction()" is no longer returning an alias but
        // "auto" (or you can change "auto" directly to "std::basic_string_view<TCHAR>"
        // if you wish since it's still not an alias). The "tstring_view =
        // std::basic_string_view<char>" portion of the "2 of 2" string is no longer
        // required IOW since there's no "tstring_view" alias anymore, so GCC removes
        // it. The upshot is that the offset to the type we're trying to extract from
        // __PRETTY_FUNCTION__, a "float" in the above example but whatever the type is,
        // can change depending on the return type of "GetPrettyFunction()" itself.
        // Therefore, unlike the case for all other compilers we currently support,
        // whose offset to the type is always at the same consistent location from the
        // start of __PRETTY_FUNCTION__ or (for MSFT only) __FUNCSIG__, we need to check
        // which format is in effect for GCC since we need (want) to protect against
        // possible changes to the return type of "GetPrettyFunction()" itself.
        // "GetTypeNameOffset()", which calculates the offset, therefore takes this
        // situation into account for GCC only (checking which of these two formats is
        // in effect).
        //
        // Reliance on "float"
        // -------------------
        // Note that float isn't just used for the examples above, it's also used by
        // members "GetTypeNameOffset()" and "GetTypeNameLen()" below to determine the
        // offset into each string of the type to extract, and the type's length, both
        // of which are called by member "Get()" above to extract the type name from the
        // string (for its template arg "T"). Since type float always returns "float"
        // for all supported compilers (though any type whose string is the same for
        // each compiler would do - read on), we can leverage this knowledge to easily
        // determine the offset of type "T" and its length, regardless of what "T" is,
        // without any complicated parsing of the above strings (in order to locate "T"
        // within the string and determine its length). Note that parsing would be more
        // complicated because type "T" itself can potentially contain any character
        // including angled brackets, square brackets, equal signs, etc., each of which
        // makes it harder to distinguish between those particular characters in "T"
        // itself from their use as delimiters in the above strings (where they aren't
        // part of "T"). It would usually be rare in practice but can happen. For
        // instance, "T" might be a template type (class) called, say, "Bracket", with a
        // non-template "char" arg so it can be instantiated (and therefore appear in
        // __PRETTY_FUNCTION__ and __FUNCSIG__) like so (ignoring the class' namespace to
        // keep things simple):
        //
        //      Bracket<'>'>
        //      Bracket<']'>
        //      Bracket<'='>
        //      Bracket<';'>
        //
        // This makes it more difficult to parse __PRETTY_FUNCTION__ and __FUNCSIG__
        // looking for the above type and determining its length because the above
        // strings contain the same characters used as delimiters elsewhere in
        // __PRETTY_FUNCTION__ and __FUNCSIG__ (where applicable), so any parsing code
        // will have to deal with this where required.
        //
        // To circumvent having to do this (parse the string and deal with this issue),
        // there's a much easier alternative. For the offset of "T" itself, note that
        // for each supported compiler it's always the same regardless of "T" (within
        // that particular compiler). We can therefore simply rely on a known type like
        // "float" to determine it, not "T" itself (since it will be the same for any
        // "T" so we can arbitrarily use "float" - more on why "float" was chosen
        // later). For the length of "T" however, we know that for any two different
        // values of "T", say "int" and "float" (any two types will do), the "pretty"
        // string containing "int" will be identical to the "pretty" string containing
        // "float" except for the difference between "int" and "float" themselves.
        // That's the only difference between these pretty strings, i.e., one contains
        // "int" and one contains "float", but the remainder of the pretty strings are
        // identical. Therefore, since the string "int" is 3 characters long and the
        // string "float" is 5 characters long (2 characters longer than "int"), then
        // the length of the pretty string above containing "int" must be shorter than
        // the pretty string containing "float" by 2 characters, since the pretty
        // strings themselves are identical except for the presence of "int" and "float"
        // (within their respective pretty strings). So by simply subtracting the length
        // of the pretty string for a "float" from the length of the pretty string for
        // any type "T" (i.e., by simply computing this delta), we know how much longer
        // (positive delta) or shorter (negative delta) the length of "T" must be
        // compared to a "float", since the latter is always 5 characters long. We
        // therefore just add this (positive or negative) delta to 5 to arrive at the
        // length of "T" itself. For an "int" for instance, the delta is -2 (the length
        // of its pretty string minus the length of the pretty string for "float" is
        // always -2) so 5 - 2 = 3 is the length of an "int". For a type longer than "T"
        // it works the same way only the delta is positive in this case. If an
        // "unsigned int" for instance then the delta is 7 (length of its pretty string
        // minus the length of the pretty string for "float" is always 7) so 5 + 7 = 12
        // is the length of "unsigned int". We can do this for any arbitrary "T" of
        // course to get its own length, by simply computing the delta for its pretty
        // string in relation to the pretty string for a "float" as described.
        //
        // Note that float was chosen over other types we could have used instead since
        // the name it generates in its own pretty string is always "float" for all our
        // supported compilers. It's therefore consistent among all supported compilers
        // and its length is always 5, both situations making it a reliable type to work
        // with in the code below. In practice however all the fundamental types or even
        // a particular user-defined type could have been used (each of which normally
        // generates the same consistent string as well), but going forward "float"
        // seemed (potentially) less vulnerable to issues like signedness among integral
        // types, or other potential issues. If an integral type like "int" was chosen
        // instead for instance (or "char", or "long", etc.), some future compiler (or
        // compiler option) might display it as "int" within its pretty string, or maybe
        // "signed int" or "unsigned int" depending on the default signedness in effect
        // (so not always consistent among all compilers or possibly even within a given
        // compiler depending on which compiler options are in effect at the time). Or
        // perhaps a "double" might be displayed as "double" or "long double" based on
        // some obscure compiler option so potentially not consistent either. Or perhaps
        // a "bool" might appear as an "unsigned char" if some backwards compatibility
        // option is turned on for some future compiler (since bools may have been
        // internally declared that way once-upon-a-time and someone may turn the option
        // on for backwards compatibility reasons if required). In reality it doesn't
        // seem likely this is actually going to happen for any of the fundamental types
        // however, and even "float" itself could potentially become a "double" under
        // some unknown circumstance but for now it seems to be potentially more stable
        // than the other fundamental types so I chose it for that reason only (even if
        // these reasons are a bit flimsy).
        /////////////////////////////////////////////////////////////////////////////////
        template <typename T>
        static constexpr auto GetPrettyFunction() noexcept
        {
            #if defined(_MSC_VER)
                return tstring_view(_T(__FUNCSIG__));
            #elif defined(GCC) || defined(__clang__) || defined(__INTEL_COMPILER)
                return tstring_view(__PRETTY_FUNCTION__);
            #else
                static_assert(false, "Unknown compiler in use. Only Clang, GCC, Intel and Microsoft are currently supported");
                return tstring_view(); // Empty but need to return something to shut compiler up
                                       // (but we always "static_assert" just above anyway)
            #endif
        }

        //////////////////////////////////////////////////////////////////////
        // GetTypeNameOffset(). Returns the offset within __PRETTY_FUNCTION__
        // or (for MSFT only) __FUNCSIG__ to the start of the type we need to
        // extract from the latter strings (i.e., the offset to template arg
        // "T" in the string returned by "GetPrettyFunction()" above). Note
        // that as explained in the comments preceding "GetPrettyFunction()",
        // (see this for details), the offset to the type's name within the
        // latter strings is always identical regardless of the type so we
        // can easily calculate it using any type. No need to do it for
        // template arg "T" that is but we create the following function as a
        // template anyway (with template arg "T"). We always pass "float"
        // however (the default arg) but "T" is still required so we can
        // "static_assert" in the code below without any "static_asserts"
        // triggering when we don't want them to (where applicable). Because
        // of how "static_assert" works in C++, if it's used outside a
        // template-based context it will always trigger if its first arg is
        // false. For instance, see the "if constexpr" clause in the GCC code
        // below. When that condition is true, the "static_assert" in the
        // "else" clause would trigger if this function wasn't a template
        // because the arg being passed to "static_assert"" is false whenever
        // the "if constexpr" condition itself is true. If not for the
        // dependence on template arg "T" itself in the "static_assert" (its
        // condition indirectly but ultimately depends on "T"), the
        // "static_assert" would always trigger even when the "else" clause
        // isn't in effect. Because of the function's dependence on a
        // template arg however (that the "static_assert" itself depends on),
        // it won't trigger, which is what we require. It's similar in nature
        // to the reason behind the "AlwaysFalse" alias earlier in this
        // header (see its comments). Ideally the following function
        // shouldn't be a template since "T" will always be a "float" but in
        // order to prevent the "static_assert" from always triggering as
        // described we make it a template anyway. Note that we arbitrarily
        // choose "float" as explained in the "GetPrettyFunction()" comments.
        //////////////////////////////////////////////////////////////////////
        template <typename T = float>
        static constexpr tstring_view::size_type GetTypeNameOffset() noexcept
        {
            // See comments above
            static_assert(std::is_same_v<T, float>);

            //////////////////////////////////////////////////////////////
            // "T" is always float here (see "static_assert" just above
            // and function comments above). Must pass "T" here instead
            // of "float" directly however so that "prettyFunctionFloat"
            // becomes dependent on a template arg (so any applicable
            // "static_asserts" that depend on it below, either directly
            // or indirectly, won't erroneously kick in when we don't
            // want them to - see function comments above)
            //////////////////////////////////////////////////////////////
            constexpr tstring_view prettyFunctionFloat = GetPrettyFunction<T>();

            // "basic_string_view::ends_with()" not available until C++20
            #if CPP20_OR_LATER
                #define PRETTY_FUNCTION_FLOAT_ENDS_WITH(END_STR) prettyFunctionFloat.ends_with(END_STR)
            #else
                ///////////////////////////////////////////////////////////////
                // Quick and dirty equivalent of the C++20 code above but for
                // our specific needs below only (so certain assumptions in
                // effect, like the length of END_STR always being less than
                // the length of "prettyFunctionFloat" so we don't safeguard
                // against it being longer - it will always be shorter for our
                // uses below unless something is seriously wrong, but this
                // will result in a compiler error anyway - will never
                // realistically happen though)
                ///////////////////////////////////////////////////////////////
                #define PRETTY_FUNCTION_FLOAT_ENDS_WITH(END_STR) prettyFunctionFloat.substr(prettyFunctionFloat.size() - tstring_view(END_STR).size()) == END_STR
            #endif

            #if defined(_MSC_VER)
                ///////////////////////////////////////////////////////////////////
                // See format of __FUNCSIG__ string for "MSFT" near the top of the
                // comments in "GetPrettyFunction()". Offset to the type in this
                // string is the same regardless of the type so we arbitrarily
                // use "float" here (see comments preceding latter function for
                // details). The __FUNCSIG__ string with type "float" always ends
                // with "<float>(void) noexcept" so the offset to the "f" in this
                // string (i.e., the offset to the type we're after) is always 21
                // characters from the end (unless MSFT makes a breaking change
                // to the string). The offset will be the same no matter what the
                // type so we're good (i.e., we can just rely on the one for float
                // and immediately return this).
                ///////////////////////////////////////////////////////////////////
                static_assert(PRETTY_FUNCTION_FLOAT_ENDS_WITH(_T("<float>(void) noexcept")));

                //////////////////////////////////////////////////
                // Offset to the "f" in "<float>(void) noexcept"
                // (i.e., the 1st character of the type we're
                // after). Always the same regardless of the type
                // so our use of "float" to calculate it will
                // work no matter what the type.
                //////////////////////////////////////////////////
                return prettyFunctionFloat.size() - 21;
            #elif defined(GCC)
                //////////////////////////////////////////////////////////////////
                // See format of __PRETTY_FUNCTION__ string for "GCC" near the
                // top of the comments in "GetPrettyFunction()". There are 2
                // possible formats labeled "1 of 2" and "2 of 2" in the comments
                // (with an explanation of when each kicks in - see comments for
                // details). The offset to the type in this string is the same
                // regardless of these two formats however (for whatever format
                // is in use), or the type itself, so we arbitrarily use float in
                // the code below (again, see comments in "GetPrettyFunction()"
                // for details). For format "1 of 2" the string always ends with
                // "= float]" so the offset to the "f" in this string (i.e., the
                // offset to the type we're after) is always 6 characters from
                // the end (unless the compiler vendor makes a breaking change to
                // the string but the "static_assert" below will trap this). This
                // is identical to the "Clang" and "Intel" case further below so
                // we do the same check here for GCC. If true then the offset
                // will be the same no matter what the type so we're good (i.e.,
                // we can just rely on the one for float and immediately return
                // this). Unlike "Clang" and "Intel" however, where this check
                // must always be true (or we "static_assert" for those compilers
                // if not), if it's not true for GCC then we must be dealing with
                // format "2 of 2" instead. In this case we need to locate "=
                // float;" within the string and the offset to the type itself is
                // therefore 2 characters after the '=' sign. Again, the offset
                // to the type will be the same no matter what the type so we're
                // good (we can just rely on the one for float and immediately
                // return this).
                //////////////////////////////////////////////////////////////////

                // GCC format 1 of 2 (see "GetPrettyFunction()" for details)
                if constexpr (PRETTY_FUNCTION_FLOAT_ENDS_WITH(_T("= float]")))
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
                    static_assert(offsetOfEqualSign != tstring_view::npos);

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
            #elif defined(__clang__) || defined(__INTEL_COMPILER)
                ///////////////////////////////////////////////////////////////////
                // See format of __PRETTY_FUNCTION__ string for Clang and Intel
                // near the top of the comments in "GetPrettyFunction()". Offset
                // to the type in this string is the same regardless of type so we
                // arbitrarily use "float" here (see latter comments for details).
                // The __PRETTY_FUNCTION__ string with type "float" always ends
                // with "= float]" so the offset to the "f" in this string (i.e.,
                // the offset to the type we're after) is always 6 characters from
                // the end (unless the compiler vendors makes a breaking change to
                // the string). The offset will be the same no matter what the
                // type so we're good (i.e., we can just rely on the one for float
                // and immediately return this).
                ///////////////////////////////////////////////////////////////////
                static_assert(PRETTY_FUNCTION_FLOAT_ENDS_WITH(_T("= float]")));

                ///////////////////////////////////////////
                // Offset to the "f" in "= float]" (i.e.,
                // the 1st character of the type we're
                // after). Always the same regardless of
                // the type so our use of "float" to
                // calculate it will work no matter what
                // the type.
                ///////////////////////////////////////////
                return prettyFunctionFloat.size() - 6;
            #else
                static_assert(false, "Unknown compiler in use. Only Clang, GCC, Intel and Microsoft are currently supported");
                return tstring_view(); // Empty but need to return something to shut compiler up
                                       // (but we always "static_assert" just above anyway)
            #endif
        }

        //////////////////////////////////////////////////////////////////////
        // GetTypeNameLen(). Returns the length of template arg "T" within
        // __PRETTY_FUNCTION__ or (for MSFT only) __FUNCSIG__. If "T" is an
        // int for instance and it actually appears in the latter string as
        // "int" (always the case at this writing for all supported compilers
        // but not guaranteed - "signed int" or "unsigned int" is always
        // possible for future compilers), then it returns 3 (if it is in
        // fact "int" - if it resolves to "signed int" for some reason then
        // 10 would be returned of course, or if "unsigned int" then 12 would
        // be returned). Note that as explained in the comments preceding
        // function "GetPrettyFunction()" above, we leverage our knowledge of
        // type float to help us calculate the length of "T". See
        // "GetPrettyFunction()" for details.
        //////////////////////////////////////////////////////////////////////
        template <typename T>
        static constexpr tstring_view::size_type GetTypeNameLen() noexcept
        {
            constexpr tstring_view prettyFunctionT = GetPrettyFunction<T>();
            constexpr tstring_view prettyFunctionFloat = GetPrettyFunction<float>();

            ///////////////////////////////////////////////////////////////
            // See full explanation in "GetPrettyFunction()". The size of
            // type "T" is determined by simply taking the difference in
            // the length of the pretty function containing "float" and
            // the length of the pretty function containg "T" itself
            // (positive if length of "T" is greater than the length of
            // "float" which is always 5, negative if less, zero if
            // equal), and adding this to the length of "float" (again,
            // always 5). "float" was chosen for this purpose but any type
            // with a consistent size among all supported compilers will
            // do. Again, see comments in "GetPrettyFunction()" for
            // details.
            //
            // Lastly, note that since the following is dealing with
            // unsigned numbers, I've coded things to avoid the possibility
            // of a negative number when the length of the pretty strings
            // are subtracted from each other. It's a confusing area for
            // many, gets into the possibility of undefined behavior (won't
            // talk about that here), etc. Therefore simpler to just code
            // things as seen (whole situation avoided).
            ///////////////////////////////////////////////////////////////

            // Size of type "T" greater than "float" (i.e., > 5)
            if constexpr (prettyFunctionT.size() > prettyFunctionFloat.size())
            {
                return 5 + (prettyFunctionT.size() - prettyFunctionFloat.size());
            }
            // Size of type "T" less than or equal to "float" (i.e., <= 5)
            else
            {
                return 5 - (prettyFunctionFloat.size() - prettyFunctionT.size());
            }
        }
    };

    ////////////////////////////////////////////////////////////
    // MSFT? (though Intel will also #define this when running
    // on Windows). Just a Visual Studio 2017 bug fix ...
    ////////////////////////////////////////////////////////////
    #if defined(_MSC_VER)
        /////////////////////////////////////////////////
        // Visual Studio 2017? (can't be earlier since
        // CPP17_OR_LATER currently in effect from our
        // check for it earlier - C++17 or later must
        // therefore be in effect which wasn't supported
        // in Visual Studio 2015 or earlier so if the
        // following is true it must be VS 2017)
        /////////////////////////////////////////////////
        #if _MSC_VER < MSC_VER_2019
            /////////////////////////////////////////////////////////
            // The following code is only required in VS2017 due to
            // a MSFT bug in that version (which I won't get into
            // here). The bug was fixed in VS2019. If we ever stop
            // supporting VS2017 then we can kill this code (and
            // the code that relies on it later on)
            /////////////////////////////////////////////////////////
            static inline constexpr bool IsEqualTo(tstring_view typeName,
                                                   tstring_view expectedTypeName) noexcept
            {
                if (typeName.size() == expectedTypeName.size())
                {
                    for (tstring_view::size_type i = 0; i < typeName.size(); ++i)
                    {
                        if (typeName[i] != expectedTypeName[i])
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
            };
        #endif // _MSC_VER < MSC_VER_2019
    #endif // defined(_MSC_VER)

    //////////////////////////////////////////////////////////////////////
    // Sanity check only. The template, "TypeName_v" defined below, is
    // brittle should any compiler vendor change the format of
    // __PRETTY_FUNCTION__ or __FUNCSIG__ (MSFT) so that our assumption
    // about its existing format no longer holds (or we ever encounter
    // something different than what we expected given that its format is
    // undocumented). "TypeName_v" could then potentially return an
    // erroneous value, leading to potentially serious (hard-to-find)
    // runtime bugs (including possible crashes). The following
    // compile-time check therefore traps the situation by checking if
    // the template (and hence its implementation function) works for
    // type "float". It does this by simply checking that it returns
    // "float" which I'm arbitrarily choosing since any type whose name
    // is consistent among all our supported compilers will do (such as
    // "float"). It doesn't have to be "float" IOW since other types will
    // also work (see "TypeNameImpl::GetPrettyFunction()" above for
    // further details).
    //
    // In any case, if "TypeName_v<float>" doesn't return "float" then
    // the following "static_assert" will trigger. It normally means that
    // the compiler vendor changed the format of __PRETTY_FUNCTION__ or
    // (for MSFT only) __FUNCSIG__, assuming no errors in our
    // understanding of this string's undocumented format. Note that it's
    // not a fullproof check though (longer story), the implementation
    // above will normally "static_assert" anyway (making the following
    // one superfluous usually), and it's really just a hack, but in
    // reality it will normally be reliable. The upshot is that if
    // "TypeName_v" ever starts returning a different name for "float"
    // than it did at the time "TypeName_v" was written, and our
    // implementation above doesn't already "static_assert" as a result,
    // then the following "static_assert" will almost certainly be
    // triggered (by itself or in addition to those in the implementation
    // above). You'll then have to review the implementation above to
    // correct the situation.
    //////////////////////////////////////////////////////////////////////
    static_assert(
                    ////////////////////////////////////////////////////
                    // True for all compilers we support except Visual
                    // Studio 2017. See #else comments below
                    ////////////////////////////////////////////////////
                    #if !defined(_MSC_VER) || _MSC_VER >= MSC_VER_2019 
                        TypeNameImpl::Get<float>() == _T("float"), // Make sure "TypeName_v()" returns "float" (literally, though
                                                                   // it's returned as a "tstring_view"). See long comments above.
    
                    //////////////////////////////////////////////////////
                    // Visual Studio 2017 only. Call just above should
                    // also work but fails due to a MSFT bug (in VS2017).
                    // The following is a work-around ...
                    //////////////////////////////////////////////////////
                    #else
                        IsEqualTo(TypeName_v<float>, _T("float")),
                    #endif
                    "A breaking change was detected in template \"TypeNameImpl::Get()\". The format of the "
                    "predefined string __PRETTY_FUNCTION__ or (for MSFT) __FUNCSIG__ was likely changed by "
                    "the compiler vendor (though would be very rare). \"TypeNameImpl::Get()\" was (arbitrarily) "
                    "tested with type float but the returned string isn't \"float\" and normally should be. "
                    "The format of __PRETTY_FUNCTION__ (or __FUNCSIG__) was therefore (likely) changed since "
                    "\"TypeNameImpl::Get()\"was written, so its implementation should be reviewed and corrected.");
}

////////////////////////////////////////////////////////////////////////
// TypeName_v. Variable template that returns the literal name of the
// given template arg T as a compile-time string, suitable for display
// purposes (WYSIWYG). You can pass any type for T that is to return
// its name as a "tstring_view". This is always a "std::string_view" on
// non-MSFT platforms in this release ("tstring_view" always resolves
// to "std::string_view"), and normally "std::wstring" on MSFT
// platforms. Note that MSFT platforms are normally compiled for UTF-16
// based on the #defined constants UNICODE and _UNICODE (see these if
// you're not already familiar), but if your code is in fact compiled
// for ANSI instead (on MSFT platforms), though it would be very rare
// these days, then "tstring_view" will correctly resolve to
// "std::string_view" which we still support. It *always* resolves to
// "std::string_view" on non-MSFT platforms however (in this release as
// noted), since supporting anything else means we'd have to deal with
// the overhead of character conversions (if "wchar_t" or some other
// character type had to be supported - easier on MSFT platforms only
// for now, at least for Unicode vs non-Unicode builds, though in
// reality Unicode builds are almost always the norm there). We can
// always review the situation for other platforms in a future release
// if ever required.
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
//     ////////////////////////////////////////////////////
//     // A type like "std::wstring" produces a more
//     // complicated name than the "float" example above
//     // of course. For example (for 3 compilers shown -
//     // note that the surrounding quotes below are just
//     // for legibility and not actually returned):
//     //
//     //   Microsoft:
//     //   ---------
//     //   "class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t> >"
//     //
//     //   GCC
//     //   ---
//     //   "std::basic_string<wchar_t>" if "-D _GLIBCXX_USE_CXX11_ABI=0" macro is in effect (see https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html)
//     //   "std::__cxx11::basic_string<wchar_t>" if above macro is set to 1 instead (again, see above link)
//     //
//     //   Clang
//     //   -----
//     //   "std::basic_string<wchar_t>"
//     ////////////////////////////////////////////////////
//     constexpr tstring_view typeName = TypeName_v<std::wstring>;
//
// Note that this template simply extracts the type from the predefined
// strings __PRETTY_FUNCTION__ (for non-MSFT compilers), or __FUNCSIG__
// (Microsoft compilers), noting that these may or may not be defined
// as a macro or even a string literal depending on the compiler
// (__FUNCSIG__ is always officially a string literal however but for
// __PRETTY_FUNCTION__, see here for GCC for instance
// https://gcc.gnu.org/onlinedocs/gcc/Function-Names.html). It doesn't
// matter for our purposes however since we correctly extract the type
// name portion from the string regardless (corresponding to template
// arg "T"), returning this as a "tstring_view". Note that there are
// also no lifetime worries since these predefined strings are always
// statically defined (i.e., available for the life of the app), so the
// "tstring_view" returned is just a view into this static string.
//
// Lastly, note that the techniques used to extract the type are based
// on the undocumented format of __PRETTY__FUNCTION and __FUNCSIG__ so
// there's always the potential they may fail one day (if either a
// compiler vendor ever changes the format or we encounter some
// previously unknown situation with the format, again, since they're
// not officially documented). However, in practice they're stable, and
// the techniques we rely on are generally used by many others as well
// (using similar approaches though unlike many others, our own
// technique isn't vulnerable to changes in the name of the
// implementation function the following variable relies on - it can be
// changed to anything without breaking things but read on). Moreover,
// a "static_assert" just after the implementation class itself
// ("Private::TypeNameImpl") checks if it actually works for type float
// as a quick and dirty (but normally reliable) test, i.e., the
// function should return "float" if passed float as its template arg
// (arbitrarily chosen for testing but any type whose name is the same
// on all supported compilers will do - see
// "Private::TypeNameImpl::GetPrettyFunction" above for further
// details). If not then the "static_assert" kicks in with an
// appropriate message (also in the implementation itself). Note that
// I've also coded things not to rely on anything in the
// fully-qualified name (or return type) of __PRETTY_FUNCTION__ and
// __FUNCSIG___ themselves (beyond what's necessary), as some
// implementations of this (general) technique do. Doing so is brittle
// should you change anything in the function's signature, such as its
// name and/or return type (as some users might do). This would break
// many implementations even though most will (hopefully) be easy to
// fix. Ours isn't sensitive to the function's signature however,
// again, beyond the minimal requirements necessary for the code to
// work (given that we're dealing with undocumented strings). Changing
// the function's fully-qualified name for instance (the actual
// function name, its member class name, or namespace), and/or its
// return type (to "tstring_view" or "std::basic_string<TCHAR>" for
// instance) won't break the function. Some users may not like the
// "auto" return type for example and may want to change it to
// "tstring_view" (declared in "CompilerVersions.h" though this is a
// more MSFT-centric technique due to their historical use of TCHAR
// which you can read up on if not familiar), or
// "std::basic_string_view<char>" (or replacing "char" with TCHAR or on
// MSFT platforms only at this writing "wchar_t") or "std::string_view"
// or "std::wstring_view" (latter only on MSFT platforms at this
// writing)
//
// The bottom line is that you can change our implementation function's
// signature if you wish and it will still work correctly. Note that if a
// given compiler vendor ever changes the format of __PRETTY_FUNCTION__
// itself however (or __FUNCSIG__ for MSFT only), then that could
// potentially break the function as noted but again, the "static_asserts"
// in the implementation will normall trigger if this occurs, and you'll
// then have to fix the implementation function for this template to deal
// with it ("Private::TypeNameImpl::Get()" as it's currently called at
// this writing). It seems very unlikely a compiler vendor will ever
// change the format however, let alone in a way that will break this
// function, unless it's done to deal with some new C++ feature perhaps
// but who knows.
//
// Long story short, our implementation function should normally be very
// stable but compilation will fail with an appropriate error if something
// goes wrong (again, via the the "static_asserts" in the private
// implementation function called by this template).
///////////////////////////////////////////////////////////////////////////
template <typename T>
inline constexpr tstring_view TypeName_v = Private::TypeNameImpl::Get<T>();

/////////////////////////////////////////////////////////////////////////////
// IsSpecialization. Primary template. See partial specialization just below
// for details.
/////////////////////////////////////////////////////////////////////////////
template <typename, template<typename...> class>
struct IsSpecialization : public std::false_type
{
};

/////////////////////////////////////////////////////////////////////////////
// Partial specialization of (primary) template just above. The following
// kicks in when the 1st template arg in the primary template above (a type)
// is a specialization of the 2nd template arg (a template). IOW, this partial
// specialization kicks in if the 1st template arg (a type) is a type created
// from a template given by the 2nd template arg (a template). If not then the
// primary template kicks in above instead (i.e., when the 1st template arg (a
// type) isn't a type created from the template given by the 2nd template arg
// (a template), meaning it's not a specialization of that template).
//
//    Example
//    -------
//    template <class T>
//    class Whatever
//    {
//       // Make sure type "T" is a "std::vector"
//       static_assert(IsSpecialization<T, std::vector>::value,
//                     "Invalid template arg T. Must be a \"std::vector\"");
//    };
// 
//    Whatever<std::vector<int>> whatever1; // "static_assert" above succeeds (2nd template arg is a "std::vector")
//    Whatever<std::list<int>> whatever2; // "static_assert" above fails (2nd template arg is *not* a "std::vector")
/////////////////////////////////////////////////////////////////////////////
template <template<typename...> class Template, typename... TemplateArgs>
struct IsSpecialization<Template<TemplateArgs...>, Template> : public std::true_type
{
};

//////////////////////////////////////////////////////////
// Helper variable template for "IsSpecialization" above
// (with the usual "_v" suffix). Set to true if "Type"
// is a specialization of the given "Template" or false
// otherwise. See "IsSpecialization" above for details.
//////////////////////////////////////////////////////////
template <typename Type, template<typename...> class Template>
inline constexpr bool IsSpecialization_v = IsSpecialization<Type, Template>::value;

// Concept for above template (for C++20 and later)
#if CPP20_OR_LATER
    template <typename Type, template<typename...> class Template>
    concept Specialization_c = IsSpecialization_v<Type, Template>;
#endif

template <typename T>
using RemoveRefAndPtr = std::remove_pointer_t<std::remove_reference_t<T>>;

//////////////////////////////////////////////////////////////
// "IsTuple" (primary template). Determines if "T" is a
// "std::tuple" specialization. Note that the primary template
// kicks in only if "T" is *not* a "std::tuple" in which case
// the primary template inherits from "std::false_type".
// Otherwise, if it is a "std::tuple" then the specialization
// below kicks in instead, which inherits from "std::true_type".
// The specialization does the actual work of checking if "T"
// is a "std::tuple"
//////////////////////////////////////////////////////////////
template <typename T, typename = void>
struct IsTuple : std::false_type
{
};

////////////////////////////////////////////////////////////////
// Specialization of "IsTuple" (primary) template just above.
// This specialization does all the work. See primary template
// above for details.
////////////////////////////////////////////////////////////////
template <typename T>
struct IsTuple<T,
               std::enable_if_t<IsSpecialization_v<T, std::tuple>>
              > : std::true_type
{
};

///////////////////////////////////////////////////////
// Helper variable template for "IsTuple" above (with
// the usual "_v" suffix).
///////////////////////////////////////////////////////
template <typename T>
inline constexpr bool IsTuple_v = IsTuple<T>::value;

// Concept for above template (for C++20 and later)
#if CPP20_OR_LATER
    template <typename T>
    concept Tuple_c = IsTuple_v<T>;

    // See "#else" comments just below
    #define TUPLE_C Tuple_c
#else // C++17 (earlier compilers not supported)
    //////////////////////////////////////////////////////////////////
    // Useful macro that can be used in any template where you want
    // to check a template arg to ensure it's a "std::tuple"
    // (specialization). The following will "static_assert" if it's
    // not. See primary template "IsTuple" above for details (what
    // we're asserting on just below). Note that this macro is
    // #defined in C++17 and earlier only. In C++20 or later you can
    // rely on concepts instead so this macro should only be applied
    // if CPP17_OR_EARLIER is #defined. When not #defined then the
    // #defined constant TUPLE_C above can be used instead (to apply
    // the "IsTuple_c" concept). All code should therefore be written
    // similar to the following example (which correctly targets
    // whichever version of C++ is in effect, applying the following
    // macro if it's C++17 or earlier, or the concept if C++20 or
    // later):    
    // 
    //      template<TUPLE_C TupleT>
    //      void SomeFunction()
    //      {
    //          #if CPP17_OR_EARLIER
    //              /////////////////////////////////////////////
    //              // Kicks in if C++ 17 or earlier, otherwise
    //              // TUPLE_C concept kicks in just above
    //              // instead (latter simply resolves to the
    //              // "typename" keyword in C++17 or earlier)
    //              /////////////////////////////////////////////
    //              STATIC_ASSERT_IS_TUPLE(TupleT);
    //          #endif
    //          
    //          // ...
    //      }
    //////////////////////////////////////////////////////////////////
    #define STATIC_ASSERT_IS_TUPLE(T) static_assert(IsTuple<T>::value, "\"" #T "\" must be a \"std::tuple\"");

    // See comment block above
    #define TUPLE_C typename
#endif

///////////////////////////////////////////////////////////////////////////
// Declarations mostly associated with struct "FunctionTraits" declared
// further below (the latter struct itself plus additional support
// declarations including helper templates mostly associated with
// "FunctionTraits" - all code that follows for the remainder of this
// file). "FunctionTraits" itself is used to retrieve information about
// any function including (most notably) its return type and the type of
// any of its arguments. Other less used info is also avalable (see
// "FunctionTraitsBase" members). Simply pass the function's type as the
// template's only argument (see next paragraph), but see the helper alias
// templates "ReturnType_t" and "ArgType_t" declared later on for an even
// easier (less verbose) way to access the return type and arg types of
// any function (and other helper templates also exist). They take the
// same template arg and simply wrap the appropriate aliases of
// "FunctionTraits" demo'd in the example below. They're easier to use
// than "FunctionTraits" directly however, and should normally be relied
// on. The example below shows how to do it using "FunctionTraits" but the
// helper aliases should normally be used instead.
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
// exists for every permutation of the above, noting that the if primary
// template itself kicks in then no specialization exists for template
// argument "F". It means that "F" is not a function type so a compiler
// error will occur (our own concept will trigger in C++20 or greater, or
// our own "static_assert" for C++17 or earlier.
//
// Lastly, please note that calling conventions like "stdcall", etc. may
// be replaced by compilers with "cdecl" under certain circumstances, in
// particular when compiling for 64 bit environments (opposed to 32 bit).
// That is, if you declare a function with a particular calling convention
// other than "cdecl" itself, your compiler might change it to "cdecl"
// depending on your compile-time environment. "FunctionTraits" will
// therefore reflect this so its "CallingConvention" member will correctly
// indicate "cdecl", not the calling convention your function is actually
// declared with (since the compiler changed it). Note that a given
// compiler may or may not produce warnings about this situation when it
// quietly replaces your calling convention with "cdecl".
//
//     Example
//     -------
//     //////////////////////////////////////////////////////////////////
//     // Free function (member functions and functors also supported -
//     // see comments above)
//     //////////////////////////////////////////////////////////////////
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
//     using ReturnType_t = typename SomeFuncTraits::ReturnType; // Function's return type (float)
//     using Arg3Type_t = typename SomeFuncTraits::template Args<2>::Type; // Type of "arg3" (int)
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
// (erogonomcally correct) any way to do it using either the language
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

// For internal use only ...
namespace Private
{
    ///////////////////////////////////////////////////////////////////
    // "IsFunctor" (primary template). Primary template inherits from
    // "std::false_type" if "T" is *not* a functor or reference to a
    // functor. Otherwise, if it is a functor or reference to a
    // functor then the specialization just below kicks in instead
    // (inheriting from "std::true_type"). If true then by definition
    // "T" must be a class or struct (or reference to one) with a
    // member function called "operator()". Note that if
    // "T::operator()" is overloaded then the primary template will
    // kick in since which "operator()" to use becomes ambiguous. In
    // this case you'll have to target the specific "operator()"
    // overload you're interested by taking its address and casting it
    // to the exact signature you're interested in (in order to
    // disambiguate it). You can't rely on this trait IOW (since it
    // will fail to identify "T" as a functor due to the ambiguity).
    ///////////////////////////////////////////////////////////////////
    template <typename T, typename = void>
    struct IsFunctor : std::false_type
    {
    };

    /////////////////////////////////////////////////////////////////
    // Specialization of "IsFunctor" (primary) template just above.
    // This specialization does all the work. See primary template
    // above for details.
    /////////////////////////////////////////////////////////////////
    template <typename T>
    struct IsFunctor<T,
                     std::void_t<decltype(&std::remove_reference_t<T>::operator())>
                    > : std::true_type
    {
    };

    // Usual "_v" helper variable for above template
    template <typename T>
    inline constexpr bool IsFunctor_v = IsFunctor<T>::value;

    /////////////////////////////////////////////////////////////////////
    // "IsFreeFunction" (primary template). Primary template inherits
    // from "std::false_type" if "T" is *not* a free function, which
    // also includes static member functions ("free" will refer to both
    // from here on). Otherwise, if it is a free function then the
    // specialization just below kicks in instead (inheriting from
    // "std::true_type"). In order to understand how "IsFreeFunction"
    // works (which isn't as simple as it sounds), you first need to
    // understand C++ function type syntax. Note that function types in
    // C++ are structured as follows (square brackets indicate optional
    // items - note that this syntax isn't 100% precise but close
    // enough for this discussion):
    //
    //   ReturnType [CallingConvention] ([Args]) [const] [volatile] [& | &&] [noexcept];
    //
    // All function types including non-static member functions are
    // typed this way. For non-static member functions however, when
    // dealing with situations involving their type, normally
    // developers target them using pointer-to-member syntax but the
    // function's underlying type is still structured as seen above (no
    // different than for free functions). This may surprise many not
    // used to seeing the actual type of a non-static member function
    // since it's rarely encountered this way in most real-world code.
    // If you have the following for instance:
    //
    //     class Whatever
    //     {
    //     public:
    //         int SomeFunc(float) const;
    //     };
    //
    // And you need to deal with "SomeFunc" based on its type, you
    // would normally do this or similar:
    //
    //     using SomeFuncType = decltype(&Whatever::SomeFunc);
    //
    // "SomeFuncType" therefore resolves to the following:
    //
    //     int (Whatever::*)(float) const;
    //
    // You can then invoke "SomeFunc" using this pointer, the usual
    // reason you need to work with its type. However, this is a
    // pointer type (to a member of "Whatever" that happens to be a
    // function), not the actual type of "SomeFunc" itself, which is
    // actually this:
    //
    //     int (float) const;
    //
    // The above syntax follows the general structure of all functions
    // described earlier. It's also what "std::is_function" takes as
    // its template arg and it will return true, i.e., you can do this:
    //
    //     using F = int (float) const;
    //     constexpr bool isFunction = std::is_function_v<F>; // true
    //    
    // However, the above likely appears to many (maybe most) that it's
    // a free function but it can't be since a free function can't be
    // "const". However, if you remove the "const":
    //    
    //     using F = int (float);
    //     constexpr bool isFunction = std::is_function_v<F>; // true
    //
    // Now it really appears to be a free function type even though it
    // was a non-static member function type before removing the
    // "const", so how can you tell the difference. In fact you can't
    // distinguish between this non-static member function type (which
    // it was just before removing the "const" so in theory still is),
    // and a free function that has the same type. They're both the
    // same, in this case a function taking a "float" and returning an
    // "int". Without knowing if it came from a class or not it's just a
    // function type and that's all. You can therefore have a free
    // function with this type and a non-static member function with the
    // same type. The (major) difference is that a non-static member
    // function must be wired to the class it belongs to before you can
    // invoke it, and the usual pointer-to-member syntax allows
    // developers to do so. Nevertheless, the function's actual type
    // (syntax) is still that of a normal (non-static) function, as
    // emphasized. In the wild it's rarely ever encountered this way
    // however as previously noted (for a non-static member function).
    // The context in which the actual (non-pointer) type of a
    // non-static member function needs to be dealt with rarely ever
    // comes up for most developers, though there are some rare
    // circumstances when it can (but I won't bother showing an example
    // here - few will ever come across it). In any case, for purposes
    // of "IsFreeFunction" we're creating here, we want to return "true"
    // when template arg "T" refers to a "free" function but not a
    // non-static member function. Since there's no way to distinguish
    // between the two however when targeting a function like so:        
    //
    //     using F = int (float);
    //
    // Then for purposes of "IsFreeFunction" we simply always assume
    // that it is a free function (but read on). It normally will be
    // in the real world because this syntax is almost always used
    // exclusively to deal with free functions, opposed a non-static
    // member functions, where pointer-to-member syntax is normally
    // used instead. Nevertheless, there are ways of getting hold of a
    // non-static member function's type though it usually takes some
    // extra effort. If you have such a type however and pass it for
    // template arg "T", "IsFreeFunction" will appear to erroneously
    // return "true", at least for the case just above. It would
    // therefore (seemingly) misidentify it as a free function but it
    // should be understood that the intention of "IsFreeFunction"
    // isn't to identify whether "T" originated from a free function or
    // not. Its purpose is to determine if it *qualifies* as a "free"
    // function even if "T" originated from a non-static member
    // function. So long as it represents a valid free function type,
    // then "IsFreeFunction" will return true, even if "T" actually
    // came from a non-static member function (with the same type).
    // "IsFreeFunction" doesn't care where it originates from IOW, only
    // that it represents a valid free function. Whether it actually
    // came from one or not is immaterial since the origin of "T"
    // doesn't matter.
    //
    // One might therefore ask, why not just rely on "std::is_function"    
    // instead. The difference is that "std::is_function" will return
    // true for *any* function type, even if the type you pass has a
    // cv-qualifier and/or reference-qualifier (again, see the general
    // syntax of functions at the outset of these comments). The
    // cv-qualifiers ("const" or "volatile") and reference-qualifiers
    // (& and &&) aren't legal qualifiers on a free function so if you
    // have any of these ...
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
    // ... then none of them qualify as a free function.
    // "std::is_function" will still return true for these however since
    // they're still valid functions. "IsFreeFunction" won't however,
    // and that's the difference between the two. It will return "false"
    // instead since they don't qualify as free functions. It's illegal
    // in C++ to use them in any context where they might be identified
    // as free functions. That's the only difference between
    // "std::is_function" and "IsFreeFunction". For all other function
    // types, namely, those that don't include the above qualifiers (cv
    // and reference), "std::is_function" and "IsFreeFunction" will both
    // return the same true or false value. You should therefore call
    // "IsFreeFunction" when you need to filter out functions with the
    // above qualifiers. If "IsFreeFunction" returns true, it not only
    // means that "T" is a function type (passing a non-function type
    // like "int" for instance would obviously return false, as would
    // pointers/references to functions which are pointers/reference
    // types, not actual function types), but that it also qualifies as
    // a free function as well, meaning it has none of the above
    // qualifiers. If "IsFreeFunction" returns false however, it either
    // means "T" isn't a function whatsoever (again, if you pass a
    // non-function type like "int" or even a function pointer or
    // reference for instance), or "T" *is* a function but it has one of
    // the above qualifiers so therefore isn't a "free" function
    // (doesn't qualify as one). Note that function types with cv and
    // reference qualifiers are informally known as "abominable function
    // types. They are described here at this writing (note that the
    // author, Alisdair Meredith, is the C++ Standard Committee Library
    // Working Group chair at this writing):
    //
    //     Abominable Function Types
    //     https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html
    //
    // "IsFreeFunction" therefore simply filters out abominable function
    // types, inheriting from "std::false_type" if "T" is in fact an
    // abominable function (or a non-function type altogether, including
    // pointers and references to functions). For all other functions
    // (actual function types only, again, not pointer and references to
    // functions), it inherits from "std::true_type" which is the
    // definition of a "free" function in this context (a function type
    // that's not an abominable function).
    //
    // Lastly, see the specialization just below for details on how to
    // actually determine if the above qualifiers are present or not
    // (i.e., how we filter out abominable functions)
    /////////////////////////////////////////////////////////////////////
    template <typename T, typename = void>
    struct IsFreeFunction : std::false_type
    {
    };

    //////////////////////////////////////////////////////////////////
    // Specialization of "IsFreeFunction" (primary) template just
    // above. This specialization does all the work. See primary
    // template above for details. Note that this specialization
    // kicks in if "T" is a valid function type but not an abominable
    // function (see primary template above for full details). IOW,
    // this specialization kicks in only if the following holds:
    //
    // 1) "T" is a function type (pointers or references to functions
    //    don't qualify - must be an actual function type described in
    //    the primary template above)
    // 2) The function "T" has no cv-qualifiers or reference-qualifiers
    //    meaning it qualifies as a free function (which includes
    //    static member functions). It's not an abominable function
    //    IOW. Again, see primary template above for details. As
    //    described there, the following specialization kicks in only
    //    if "T" is a function type AND does not have any cv-qualifiers
    //    or reference-qualifiers as per the following examples (every
    //    possible permutation of these qualifiers shown - these are
    //    the abominable function types by definition):
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
    //
    // If none of these qualifiers are present then this specialization
    // kicks in as described (inheriting from "std::true_type"). If
    // any of them are present however than the primary template kicks
    // in instead (inheriting from "std::false_type" type)
    //
    // Lastly, note that the code below works by simply attempting to
    // add a pointer to "T", resulting in "T *", and then immediately
    // removing the pointer and checking if the resulting type is a
    // function (by passing it to "std::is_function_v"). If "T" isn't a
    // function at all, such as an "int", or even a pointer or reference
    // to a function (or even a reference type like "int &" which will
    // fail when we attempt to convert it to "int & *" - pointers to
    // references are illegal in C++ so SFINAE will always fail), then
    // it's guaranteed that the primary template above will kick in
    // (i.e., "T" is not a function at all in this case). If "T" is a
    // function however, then the attempt to add a pointer to it, i.e.,
    // convert it to "T *", will only work if "T" does not have a
    // cv-qualifier or reference-qualifier (i.e. it's not an abominable
    // function). IOW, it's a function type but has none of the
    // qualifiers seen above. "T *" is legal in this case so we wind up
    // with "T *", then immediately remove the pointer using
    // "std::remove_pointer_t", resulting in "T" again, which we pass to
    // "std::is_function_v" and it's guaraneed to return true. "T" is
    // therefore a function type but has none of the above qualifiers so
    // it's a free function type. OTOH, if any of the above qualifiers
    // are present in "T", meaning that "T" must be a non-static member
    // function (since free functions can't have these qualifiers), then
    // the attempt to add a pointer to "T", resulting in "T *", will
    // always fail because it's illegal to do so. See section 2.2 here:
    // 
    //     Abominable Function Types (by Alisdair Meredith)
    //     https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html
    //
    // You can't create a pointer to a function that has any of these
    // qualifiers (we're not with dealing pointer-to-member syntax
    // here, but the actual raw function type), so this SFINAE context
    // fails and the primary template above therefore kicks in instead
    // (inheriting from "std::false_type" meaning "T" does not qualify
    // as a free function even though it's a valid function type,
    // because "T" contains one of the above qualifiers - it must be a
    // non-static member function type as well as an abominable
    // function).
    ///////////////////////////////////////////////////////////////////
    template <typename T>
    struct IsFreeFunction<T,
                          std::enable_if_t<std::is_function_v<std::remove_pointer_t<T *>>>
                         > : std::true_type
    {
    };

    // Usual "_v" helper variable for above template
    template <typename T>
    inline constexpr bool IsFreeFunction_v = IsFreeFunction<T>::value;
}

////////////////////////////////////////////////////////////////
// "IsTraitsFunction" (primary template). Primary template
// inherits from "std::false_type" if "T" is *not* a function
// type suitable for passing as the template arg to struct
// "FunctionTraits". Otherwise, if it is a suitable function
// then the specialization just below kicks in instead
// (inheriting from "std::true_type"). Note that for use with
// "FunctionTraits", a suitable function refers to any of the
// following types:
//
// 1) Free functions (which includes static member functions).
//    See comments preceding "Private::IsFreeFunction" for
//    complete details.
// 2) Pointers and references to free functions
// 3) References to pointers to free functions
// 4) Pointers to non-static member functions
// 5) References to pointers to non-static member functions
// 6) Functors (classes with a single non-static "operator()"
//    member so long as it's not overloaded - doing so would
//    create an ambiguous situation so if you need to deal
//    with this you'll normally need to target the specific
//    "operator()" overload you're interested by taking its
//    address and casting it to the exact signature you need
//    to target - item 4 or 5 above will then handle it)
//
// Note that raw function types containing cv-qualifiers and/or
// reference-qualifiers (AKA "abominable" functions) are NOT
// supported, so "IsTraitsFunction" will return false for them
// (see details in comments preceding "Private::IsFreeFunction"
// for further details). These are very rare in reality though.
// They apply to non-static member functions only since the
// presence of these qualifiers in free functions isn't legal
// in C++. However, non-static member functions are only
// supported using pointers to these functions (or references
// to such pointers), as seen in items 4 and 5 above, not the
// actual non-static member function type itself. Again, for
// complete details see the comments preceding
// "Private::IsFreeFunction". The upshot is that if "T" is a
// function type (not a pointer or reference to a function but
// the actual function type), and it has cv-qualifiers and/or
// reference-qualifiers as per the following examples, then
// "IsTraitsFunction" will return false by design. Note that
// all permutations of these qualifiers are accounted for in
// the examples below. So long as those qualifiers aren't
// present then "T" constitutes a legal free function and is
// interpreted that way (not as a non-static member function
// which "FunctionTraits" only supports via normal non-static
// member function pointer syntax or references to such
// pointers - the actual member function's non-pointer type
// isn't supported even though the following types are legal
// C++ functions):    
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
////////////////////////////////////////////////////////////////
template <typename T, typename = void>
struct IsTraitsFunction : std::false_type
{
};

/////////////////////////////////////////////////////////////////
// Specialization of "IsTraitsFunction" (primary) template just
// above. This specialization does all the work. See primary
// template above for details.
/////////////////////////////////////////////////////////////////
template <typename T>
struct IsTraitsFunction<T,
                        std::enable_if_t<Private::IsFreeFunction_v<RemoveRefAndPtr<T>> || // 1) Is "T" a free function (includes static member functions).
                                                                                          //    See (long) comments just above "Private::IsFreeFunction"
                                                                                          //    for details. Note that this item (1) excludes pointers and
                                                                                          //    references to free functions which are handled by items 2-4
                                                                                          //    below. This item (1) only targets the function's actual C++
                                                                                          //    type which is never a pointer or a reference. The actual C++
                                                                                          //    type for functions isn't used that often in practice however
                                                                                          //    (typically), since free function names decay to a pointer to
                                                                                          //    the function in most expressions so item 3 below is usually
                                                                                          //    encountered more frequently (though item 1 does occur sometimes
                                                                                          //    so it's correctly handled). In any case, if "T" is *not* an
                                                                                          //    actual function type then ...
                                                                                          // 2) Is "T" a reference to a free function (rare in practice) or ...
                                                                                          // 3) Is "T" a (possibly cv-qualified) pointer to a free function
                                                                                          //    (the most common case in practice since free function names
                                                                                          //    decay to a pointer to the function in most expressions) or ...
                                                                                          // 4) Is "T" a reference to a (possibly cv-qualified) pointer to
                                                                                          //    a free function (rare in practice)
                                                                                          // *) If none of the above hold then "T" isn't a free function (or
                                                                                          //    a reference to one or a pointer to one or a reference to a
                                                                                          //    pointer to one) so we go on to check if it's a pointer to a
                                                                                          //    non-static member function ...
                                         std::is_member_function_pointer_v<std::remove_reference_t<T>> || // 1) Is T a (possibly cv-qualified) pointer to a
                                                                                                          //    non-static member function (the most common
                                                                                                          //    case for non-static member functions in
                                                                                                          //    practice) or ...
                                                                                                          // 2) Is T a reference to a (possibly cv-qualified)
                                                                                                          //    pointer to a non-static member function (rare
                                                                                                          //    in practice) but ...
                                                                                                          // X) Note that "T" *cannot* be a reference to a
                                                                                                          //    member function (not supported in C++ itself
                                                                                                          //    so we don't check for it) though 2) just
                                                                                                          //    above does support references to pointers to
                                                                                                          //    non-static member functions
                                         Private::IsFunctor_v<T> // Is "T" a functor (i.e., does it have a non-static member
                                                                 // function named "operator()" that's not overloaded since
                                                                 // overloads would be ambiguous)
                                        >
                       > : std::true_type
{
};

/////////////////////////////////////////////////////////
// Helper variable template for "IsTraitsFunction" just
// above (with the usual "_v" suffix).
/////////////////////////////////////////////////////////
template <typename T>
inline constexpr bool IsTraitsFunction_v = IsTraitsFunction<T>::value;

#if CPP20_OR_LATER
    template <typename T>
    concept TraitsFunction_c = IsTraitsFunction_v<T>;

    // See "#else" comments just below
    #define TRAITS_FUNCTION_C TraitsFunction_c
#else
    /////////////////////////////////////////////////////////////////
    // Useful macro that can be used in any template where you want
    // to check a template arg to ensure it qualifies as a
    // "FunctionTraits" function (i.e., suitable for passing as the
    // template arg to struct "FunctionTraits"). The following will
    // "static_assert" if it's not. See primary template for
    // "IsTraitsFunction" above for details (what we're asserting on
    // just below). Note that this macro is #defined in C++17 only.
    // In C++20 or later you can rely on concepts instead so this
    // macro should only be applied if CPP17_OR_EARLIER is #defined.
    // When not #defined then the #defined constant TRAITS_FUNCTION_C
    // above can be used instead (to apply the "TraitsFunction_c"
    // concept). All code should therefore be written similar to the
    // following example (which correctly targets whichever version
    // of C++ is in effect, applying the following macro if it's
    // C++17 or earlier, or the concept if C++20 or later):        
    // 
    //      template <TRAITS_FUNCTION_C F>
    //      void SomeFunction()
    //      {
    //          #if CPP17_OR_EARLIER
    //              ///////////////////////////////////////////////
    //              // Kicks in if C++ 17 or earlier, otherwise
    //              // TRAITS_FUNCTION_C concept kicks in just
    //              // above instead (latter simply resolves to
    //              // the "typename" keyword in C++17 or earlier)
    //              ///////////////////////////////////////////////
    //              STATIC_ASSERT_IS_TRAITS_FUNCTION(F);
    //          #endif
    //          
    //          // ...
    //      }
    /////////////////////////////////////////////////////////////////
    #define STATIC_ASSERT_IS_TRAITS_FUNCTION(T) static_assert(IsTraitsFunction_v<T>, "\"" #T "\" isn't a function type suitable for passing to \"FunctionTraits\". " \
                                                                                     "See comments preceding \"IsTraitsFunction\" for details but for all intents " \
                                                                                     "and purpose any legal type identifying a function will normally do (i.e., free " \
                                                                                     "functions which include static members functions, pointers and references to " \
                                                                                     "free functions, references to pointers to free functions, pointers to non-static " \
                                                                                     "member functions, references to pointers to member functions, and functors");

    // See comment block above
    #define TRAITS_FUNCTION_C typename
#endif

/////////////////////////////////////////////////////////////////
// IMPORTANT: Identifying compilers using #defined constants is
// somewhat tricky business since some of them #define the same
// identifying constants as other compilers. See here for instance:
//
//    https://stackoverflow.com/a/55926503
//  
// See the following for each compiler we currently support
// (MSFT, GCC, Clang and Intel)
//
//  MSFT (see macro _MSC_VER):
//  https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
//  https://sourceforge.net/p/predef/wiki/Compilers/#microsoft-visual-c
// 
//  GCC (see macro __GNUC__):
//  https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html#Common-Predefined-Macros
//  https://sourceforge.net/p/predef/wiki/Compilers/#gcc-cc
// 
//  Clang (see macro __clang__):
//  https://clang.llvm.org/docs/LanguageExtensions.html#builtin-macros
//  https://sourceforge.net/p/predef/wiki/Compilers/#clang
// 
//  Intel (see macro __INTEL_COMPILER):
//  https://www.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/macros/additional-predefined-macros.html
//  https://sourceforge.net/p/predef/wiki/Compilers/#intel-cc
/////////////////////////////////////////////////////////////////

// MSFT? (though Intel also #defines _MSC_VER on Windows - ok)
#if defined(_MSC_VER) // For Intel: https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/c-c-calling-conventions.html
    #define STDEXT_CC_CDECL __cdecl
    #define STDEXT_CC_STDCALL __stdcall
    #define STDEXT_CC_FASTCALL __fastcall
    #define STDEXT_CC_VECTORCALL __vectorcall
    #define STDEXT_CC_THISCALL __thiscall // Supported by Intel compiler as well (see https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/c-c-calling-conventions.html)
        
    // Intel compiler (not Microsoft)
    #if defined(__INTEL_COMPILER)
        // "regcall" only available on Intel compiler (among the ones we currently support)
        // https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/c-c-calling-conventions.html
        #define STDEXT_CC_REGCALL __regcall
    #endif
/////////////////////////////////////////////////////////////////////////
// 1) GCC (or any other compiler that implements the __GNUC__ compiler
//    extensions and therefore #defines __GNUC__)
// 2) Clang (note that __GNUC__ above might be #defined for this as well
//    if the __GNUC__ compiler extensions are in effect - ok)
// 3) Intel (again, __GNUC__ above might be #defined for this as well if
//    the __GNUC__ compiler extensions are in effect - ok)
/////////////////////////////////////////////////////////////////////////
#elif defined(__GNUC__) || \
        defined(__clang__) || \
        defined(__INTEL_COMPILER) 
    // See Raymond Chen's articles (links) in comments further above (I modified the following however to also support __thiscall)
    // For GCC: https://gcc.gnu.org/onlinedocs/gcc/x86-Function-Attributes.html
    // For Clang: https://clang.llvm.org/docs/AttributeReference.html#calling-conventions
    // For Intel: https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/c-c-calling-conventions.html 
    
    #define STDEXT_CC_CDECL __attribute__((cdecl))
    #define STDEXT_CC_STDCALL __attribute__((stdcall))
    #define STDEXT_CC_FASTCALL __attribute__((fastcall))
    #define STDEXT_CC_VECTORCALL __attribute__((vectorcall))
    #define STDEXT_CC_THISCALL __attribute__((thiscall))
    #if defined(__INTEL_COMPILER)
        // "regcall" only available on Intel compiler (among the ones we currently support)
        // See https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/c-c-calling-conventions.html
        #define STDEXT_CC_REGCALL __attribute__((regcall))
    #endif

    ///////////////////////////////////////////////////////////////
    // GCC (or compatible but not including Clang and Intel - see
    // GCC for details)
    ///////////////////////////////////////////////////////////////
    #if defined(GCC) // See https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Diagnostic-Pragmas.html#Diagnostic-Pragmas
        /////////////////////////////////////////////////////////
        // When targeting 64 bit builds the above calling
        // conventions are ignored (automatically replaced by
        // the compiler with STDEXT_CC_CDECL) except for
        // STDEXT_CC_CDECL itself (always supported).
        // STDEXT_CC_VECTORCALL may also be supported in a 64
        // bit build where applicable (when other calling
        // conventions are not). Some calling conventions may
        // also not be supported depending on other compiler
        // options (again, except for STDEXT_CC_CDECL which is
        // always supported). The bottom line is that this can
        // result in many annoying warnings so we'll temporarily
        // turn these warnings off here and back on later (only
        // if they were already on). Note that the following
        // (GCC) attributes also normally work on Clang (and
        // probably other compilers that #define __GNUC__), but
        // we won't rely on them. We'll use the compiler specific
        // #pragmas instead (in the "#elif" statements that
        // follow)
        /////////////////////////////////////////////////////////
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wattributes" // E.g., "warning: 'stdcall' attribute ignored [-Wattributes]".
    #elif defined(__clang__) // See: https://clang.llvm.org/docs/UsersManual.html#controlling-diagnostics-via-pragmas
        // See GCC comments just above (same things applies here)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wignored-attributes" // E.g., warning: 'stdcall' calling convention is not supported for this target [-Wignored-attributes].
                                                                // See https://clang.llvm.org/docs/DiagnosticsReference.html#wignored-attributes
        #pragma clang diagnostic ignored "-Wunknown-attributes" // E.g., warning: unknown attribute 'stdcall' ignored [-Wunknown-attributes] (not normally encountered
                                                                // however - all calling conventions above are legal in Clang but ignoring this warning just in case)
                                                                // See https://clang.llvm.org/docs/DiagnosticsReference.html#id886
    #elif defined (__INTEL_COMPILER)
        // See GCC comments above (same things applies here)
        #pragma warning (push)
        #pragma warning (disable:1292) // E.g. warning #1292: unknown attribute "vectorcall". Note that testing shows this warning might erroneously appear anyway for "cdecl" in a 64 bit build (go figure - Intel compiler I tested with ignores the pragma)
        #pragma warning (disable:3706) // E.g., warning #3706: attribute is not supported in 64-bit x86 configurations
    #endif
#else
    #error "Unsupported compiler (Microsoft, GCC, Clang and Intel are the only ones supported at this writing)"
#endif

enum class CallingConvention
{
    /////////////////////////////////////////////////////////
    // IMPORTANT: Don't change the order OR value of these,
    // our design depends on it (brittle only if someone
    // changes it so don't). Dirty but we'll live with it
    // (normally shouldn't do this but we have unique
    // requirements)
    /////////////////////////////////////////////////////////
    Cdecl, 
    Stdcall,
    Fastcall,
    Vectorcall,
#ifdef STDEXT_CC_REGCALL
    Regcall,
#endif
    Thiscall
};

///////////////////////////////////////////////////////////////////////////
// CallingConventionToString(). WYSIWYG
///////////////////////////////////////////////////////////////////////////
inline constexpr tstring_view CallingConventionToString(const CallingConvention callingConvention) noexcept
{
    tstring_view str;

    switch (callingConvention)
    {
        case CallingConvention::Cdecl:
            str = _T("Cdecl");
            break;
        case CallingConvention::Stdcall:
            str = _T("Stdcall");
            break;
        case CallingConvention::Fastcall:
            str = _T("Fastcall");
            break;
        case CallingConvention::Vectorcall:
            str = _T("Vectorcall");
        break;
    #ifdef STDEXT_CC_REGCALL
        case CallingConvention::Regcall:
            str = _T("Regcall");
            break;
    #endif
        case CallingConvention::Thiscall:
            str = _T("Thiscall");
            break;
    }

    return str;
}

////////////////////////////////////////////////////////////////////////////
// CallingConventionReplacedWithCdecl(). For the given "callingConvention"
// arg, returns true if the compiler will change a function with that calling
// convention to the "cdecl" calling convention or false otherwise. Note that
// compilers will sometimes change a function's (implicitly or explicitly)
// declared calling convention to cdecl depending on your compiler options,
// in particular when compiling for 64 bit versus 32 bit. The following tests
// for this by simply comparing an (arbitrary) function with the calling
// convention you pass ("callingConvention" arg), to the same (arbitrary)
// function but with the cdecl calling convention. If they're equal then the
// compiler isn't respecting the given "callingConvention" since it changed
// it to cdecl, so the function returns true (meaning functions declared with
// the calling convention you pass are replaced with the cdecl calling
// convention by the compiler). Note however that false is always returned if
// you pass "CallingConvention::Cdecl" itself for the "callingConvention"
// arg, since for the intended purpose of this function, the compiler never
// replaces cdecl with itself (it makes no sense since it's already cdecl and
// AFAIK cdecl is always supported no matter what compiler options are in
// effect)
/////////////////////////////////////////////////////////////////////////////
template <bool IsFreeFuncT>
inline constexpr bool CallingConventionReplacedWithCdecl(const CallingConvention callingConvention) noexcept
{
    // Free or static member function ...
    if constexpr (IsFreeFuncT)
    {
        #define IS_REPLACED_WITH_CDECL(CC) std::is_same_v<void CC (), void STDEXT_CC_CDECL ()>
        constexpr bool ReplacedWithCdecl[] = { false, // STDEXT_CC_CDECL never replaced with itself (for purposes of this
                                                      // function), and it's always supported regardless (so it never gets
                                                      // replaced, let alone with itself)
                                               IS_REPLACED_WITH_CDECL(STDEXT_CC_STDCALL),
                                               IS_REPLACED_WITH_CDECL(STDEXT_CC_FASTCALL),
                                               IS_REPLACED_WITH_CDECL(STDEXT_CC_VECTORCALL), // Comma required if STDEXT_CC_REGCALL
                                                                                             // #defined just below (harmless if not)
                                               #ifdef STDEXT_CC_REGCALL
                                                   IS_REPLACED_WITH_CDECL(STDEXT_CC_REGCALL)
                                               #endif
                                             };
        #undef IS_REPLACED_WITH_CDECL // Done with this

        return ReplacedWithCdecl[static_cast<std::size_t>(callingConvention)];
    }
    // Non-static member function ...
    else   
    {
        /////////////////////////////////////////////////////////
        // For use by macro just below. Note that testing shows
        // the calling convention isn't impacted by the scope
        // of this class, at least in the compilers we support.
        // The same calling convention results whether the
        // class is declared at function scope or outside the
        // function (at namespace scope), though it's a fuzzy
        // area so always possible a given compiler or compiler
        // option could affect it (though it seems unlikely at
        // this point - compilers sometimes change the calling
        // convention to Cdecl for instance, even when declared
        // with another calling convention, though it's normally
        // based on whether the declared calling convention is
        // supported depending on the compiler and compiler
        // options in effect, *not* the scope of the function
        // itself - we're likely safe IOW).
        /////////////////////////////////////////////////////////
        class AnyClass
        {
        };        

        #define IS_REPLACED_WITH_CDECL(CC) std::is_same_v<void (CC AnyClass::*)(), void (STDEXT_CC_CDECL AnyClass::*)()>
        constexpr bool ReplacedWithCdecl[] = { false, // STDEXT_CC_CDECL never replaced with itself (for purposes of this
                                                      // function), and it's always supported regardless (so it never gets
                                                      // replaced, let alone with itself)
                                               IS_REPLACED_WITH_CDECL(STDEXT_CC_STDCALL),
                                               IS_REPLACED_WITH_CDECL(STDEXT_CC_FASTCALL),
                                               IS_REPLACED_WITH_CDECL(STDEXT_CC_VECTORCALL),
                                               #ifdef STDEXT_CC_REGCALL
                                                   IS_REPLACED_WITH_CDECL(STDEXT_CC_REGCALL),
                                               #endif
                                               IS_REPLACED_WITH_CDECL(STDEXT_CC_THISCALL)
                                             };
        #undef IS_REPLACED_WITH_CDECL// Done with this

        return ReplacedWithCdecl[static_cast<std::size_t>(callingConvention)];
    }
}

//////////////////////////////////////////////////////////
// Refers to the optional "&" or "&&" ref-qualifiers that
// can be added just after the argument list of
// non-static member functions (similar to adding
// optional cv-qualifiers though the purpose different of
// course). These are rarely used in practice though so
// the "None" enum is usually in effect for most member
// functions. Read up on "ref-qualifiers" for further
// details.
//////////////////////////////////////////////////////////
enum class RefQualifier
{
    None,
    LValue,
    RValue
};

///////////////////////////////////////////////////////////////////////////
// RefQualifierToString(). WYSIWYG
///////////////////////////////////////////////////////////////////////////
inline constexpr tstring_view RefQualifierToString(const RefQualifier refQualifier,
                                                   const bool useAmpersands = true) noexcept
{
    tstring_view str;

    switch (refQualifier)
    {
        case RefQualifier::None:
            str = _T("None");
            break;
        case RefQualifier::LValue:
            str = useAmpersands ? _T("&") : _T("LValue");
            break;
        case RefQualifier::RValue:
            str = useAmpersands ? _T("&&") : _T("RValue");
            break;
    }

    return str;
}

////////////////////////////////////////////////////////////////////////////
// struct FunctionTraitsBase. Base class of all "FunctionTraits"
// specializations declared just below (the latter template is what you'll
// actually use in your code, not "FunctionTraitsBase" directly). Note that
// "FunctionTraits" has a primary template that's empty (no members), and
// one partial specialization for the many permutations of functions that
// can be handled (e.g., free functions, pointers to free functions,
// references to free functions, references to pointers to free functions,
// pointers to non-static member functions, references to pointers to
// non-static member functions, functions with or without the "noexcept"
// keyword, functions with or without the "const" and/or "volatile"
// keyword, variadic functions, etc.). Note that not including variadic
// functions whose calling convention is always cdecl, each partial
// specialization is also repeated for every calling convention (cdecl,
// stdcall, etc), since functions with different calling conventions are
// treated as different function types by all compilers we support but
// presumably/likely all compilers in general. The bottom line is that no
// matter how a function is declared, a partial specializaton exists below
// to handle it. All partial specializations inherit from the following
// struct (template) however, which contains all the function's actual
// traits. Note that the primary template itself never kicks in unless the
// template is specialized with a non-function type (see "IsTraitsFunction"
// for details). A "static_assert" will therefore occur by design in the
// primary template if template arg "F" isn't a function (i.e., it's an
// invalid type for use with "FunctionTraits")
////////////////////////////////////////////////////////////////////////////
template <typename F, // Function's full type
          typename ReturnTypeT, // Function's return type
          CallingConvention CallingConventionT, // Function's calling convention (implicitly or explicitly declared in the function)
          bool IsVariadicT, // "true" if function is variadic (last arg of function is "...") or "false" otherwise
          typename ClassT, // Applicable to non-static member functions only (use the "IsMemberFunction" constant below to check this).
                           // Stores the class (type) this non-static member function belongs to. Always "void" if not a member
                           // function.
          bool IsConstT, // Applicable to non-static member functions only (use the "IsMemberFunction" constant below to check this).
                         // Set to "true" if a "const" member function or "false" otherwise. Always false if not a member function.
          bool IsVolatileT, // Applicable to non-static member functions only (use the "IsMemberFunction" constant below to check this).
                            // Set to "true" if a "volatile" member function or "false" otherwise. Always false if not a member function.
          RefQualifier RefQualifierT, // Applicable to non-static member functions only (use the "IsMemberFunction" constant below to check
                                      // this). Set to the given "RefQualifier" enumerator if one is present in the function (& or &&).
                                      // Always "RefQualifier::None" otherwise (therefore always the case if not a member function)
          bool IsNoexceptT, // "true" if the function is declared "noexcept" or "false" otherwise
          typename... ArgsT> // Function's arguments (types) in left-to-right order of declaration (as would be expected)
struct FunctionTraitsBase
{
    ///////////////////////////////////////////////////////////////////////
    // Function's full type. Same as template arg "F" used to instantiate
    // each "FunctionTraits" specialization (which all derive from
    // "FunctionTraitsBase"). If the template arg "F" passed to
    // "FunctionTraits" is a functor however (a class/struct with an
    // "operator()" member), then the "F" we receive here isn't that
    // functor (its class/struct), but the type of "operator()" in the
    // functor. The member "IsFunctor" below will return true in this case.
    // For a detailed explanation of acceptable values of "F" (that can be
    // passed to "FunctionTraits"), see "IsTraitsFunction" (comments just
    // before the primary template).
    ///////////////////////////////////////////////////////////////////////
    using Type = F; 

    ////////////////////////////////////////////////////////////////////////
    // Function's return type. The syntax for accessing this directly is a
    // bit verbose however since you have to apply the "typename" keyword.
    // You should therefore normally rely on the helper alias templates
    // "ReturnType" or "FunctionTraitsReturnType_t" instead (declared later
    // in this file). The former just defers to the latter and should
    // usually be used (it's easier). See these for details (plus examples).
    ////////////////////////////////////////////////////////////////////////
    using ReturnType = ReturnTypeT; 

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
    // "cdecl" itself is always (realisitically) supported by all compilers
    // AFAIK (those we support at least but very likely all others as well) 
    /////////////////////////////////////////////////////////////////////////
    constexpr static enum CallingConvention CallingConvention = CallingConventionT; 

    ///////////////////////////////////////////////////////////////////////
    // "true" if the function is variadic (last arg of function is "...")
    // or "false" otherwise. Note that "..." refers to the old-school
    // definition in C, so nothing to do with variadic template arguments
    // in C++.
    ///////////////////////////////////////////////////////////////////////
    constexpr static bool IsVariadic = IsVariadicT;

    ////////////////////////////////////////////////////////////////////
    // Applicable to non-static member functions only. Stores the class
    // (type) this non-static member function belongs to. Always "void"
    // otherwise. Use the "IsMemberFunction" member below to check
    // this. Note that at this writing, the following isn't supported
    // for static member functions (it's always void for them) since
    // C++ doesn't provide any simple or clean way to detect them in
    // the context that "FunctionTraits" is used.
    ////////////////////////////////////////////////////////////////////
    using Class = ClassT; 

    /////////////////////////////////////////////////////////////////////
    // Is non-static member function declared with the "const" keyword.
    // Applicable to non-static member functions only (use the
    // "IsMemberFunction" member below to check this). Always false
    // otherwise (N/A in this case)
    /////////////////////////////////////////////////////////////////////
    constexpr static bool IsConst = IsConstT; 

    ////////////////////////////////////////////////////////////////////////
    // Is non-static member function declared with the "volatile" keyword.
    // Applicable to non-static member functions only (use the
    // "IsMemberFunction" member below to check this). Always false
    // otherwise (N/A in this case)
    ////////////////////////////////////////////////////////////////////////
    constexpr static bool IsVolatile = IsVolatileT;

    //////////////////////////////////////////////////////////////////////
    // Is non-static member function declared with a reference-qualifier
    // (& or &&). Applicable to non-static member functions only (use the
    // "IsMemberFunction" member below to check this). Always
    // "RefQualifier::None" otherwise (N/A in this case).
    //////////////////////////////////////////////////////////////////////
    constexpr static enum RefQualifier RefQualifier = RefQualifierT;

    // "true" if the function is declared "noexcept" or "false" otherwise
    constexpr static bool IsNoexcept = IsNoexceptT; 

    ///////////////////////////////////////////////////////////////////////
    // Function's arguments (types) in left-to-right order of declaration
    // (as would be expected). The syntax for accessing this directly is
    // verbose however (using the "Args" struct below), so you should
    // normally rely on the helper alias templates "ArgType_t" (usually)
    // or less frequently "FunctionTraitsArgType_t" instead (both declared
    // later in this file). The former just defers to the latter. See
    // these for details (plus examples).
    ///////////////////////////////////////////////////////////////////////
    using ArgTypes = std::tuple<ArgsT...>; 

    ///////////////////////////////////////////////////////////////////
    // Number of arguments in the function. This is officially called
    // "arity" but the term is obscure so we'll stick with a name
    // everyone can relate to.
    ///////////////////////////////////////////////////////////////////
    static constexpr std::size_t ArgCount = sizeof...(ArgsT);

    ////////////////////////////////////////////////////////////////////////
    // Function's arguments (types). Called as per the following example
    // (index "I" is zero-based) but see IMPORTANT section below for an
    // easier way (and also see helper function template "ForEachArg()"
    // if you need to iterate all arg types instead of just accessing
    // an individual arg type as the following does):
    //
    //  Example
    //  -------
    //       // Function (we'll detect the type of its 3rd arg just below)
    //       int MyFunction(int, double, float *, const std::string &)
    //       {
    //       }
    //    
    //       ///////////////////////////////////////////////////////////////////
    //       // Type of the 3rd arg of above function ("float *"). Passing 2
    //       // here since template arg I is zero-based. Note that the syntax
    //       // is (obviously) verbose however so see IMPORTANT section just
    //       // below for an easier way (that wraps the following so you should
    //       // normally rely on it instead).
    //       ///////////////////////////////////////////////////////////////////
    //       using TypeOf3rdArg = typename FunctionTraits<decltype(MyFunction)>::template Args<2>::Type;
    // 
    // IMPORTANT:
    // ---------
    //
    // Note that the syntax above is verbose so you should normally rely on
    // the helper alias templates "FunctionTraitsArgType_t" or "ArgType_t"
    // instead (usually the latter, and both declared later in this file).
    // The latter just defers to the former but in either case the syntax
    // is much easier than using the above technique directly (since the
    // aliases wrap it all up for you). See these templates for details. In
    // the above example for instance, you can replace it with this (using
    // the helper aliases):
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
    // syntax is now about as clean as we can get it):
    // 
    //       using TypeOf3rdArg = ArgType_t<decltype(MyFunction), 2>;
    // 
    // or lastly, you can break the line above into the following two lines
    // instead if you wish (though the above line isn't that verbose IMHO so
    // still arguably better):
    // 
    //       using MyFunctionType = decltype(MyFunction);
    //       using TypeOf3rdArg = ArgType_t<MyFunctionType, 2>;
    ////////////////////////////////////////////////////////////////////////
    template <std::size_t I> // I is 0-based
    struct Args
    {
        static_assert(I < ArgCount, "Invalid index I. Must be less than the number of types in (variadic) template arg \"ArgsT\" "
                                    "(i.e., the number of arguments in the function this struct is being specialized on)");
        using Type = std::tuple_element_t<I, ArgTypes>;
    };

    //////////////////////////////////////////////////////////////
    // Returns true if the function represented by this traits
    // struct is a non-static member function (including
    // functors), or false otherwise (in the latter case it's
    // either a free function or a static member function). If
    // true then the "Class", "IsConst", "IsVolatile" and
    // "RefQualifier" members above can be inspected. They're
    // always "void", "false", "false" and "RefQualifier::None"
    // respectively otherwise. Please note that at this writing,
    // the following isn't supported for *static* member
    // functions (it's always false for them), since C++ doesn't
    // provide any simple or clean (ergonomically satisfying) way
    // to detect them that I could find (in the context of using
    // a traits class like "FunctionTraits"). Therefore, even
    // though a static member function is a member function, the
    // following always returns false for it (hopefully a future
    // C++ release will allow us to revisit the situation)
    //////////////////////////////////////////////////////////////
    constexpr static bool IsMemberFunction = !std::is_void_v<Class>;

    //////////////////////////////////////////////////////////////
    // Returns true if the function represented by this traits
    // struct is a free function (including static member
    // functions) or false otherwise (in the latter case it must
    // be a member function including functors).
    //////////////////////////////////////////////////////////////
    constexpr static bool IsFreeFunction = !IsMemberFunction;

    //////////////////////////////////////////////////////////////////////////
    // Was this class instantiated from a functor? Always "false" here as
    // seen but overridden in the "FunctorTraits" partial specialization for
    // functors later on, which ultimately inherits from the class you're now
    // reading. In that derived class we redefine the following and set it to
    // true, which then "overrides" (hides) the following when accessing it
    // from that particular specialization only (and no other specialization
    // - hiding base class members admittedly not the cleanest design but
    // we'll live with it for now). Note that when true (in that partial
    // specialization only), then it means "FunctionTraits" was instantiated
    // on the functor given by the "Class" alias member seen further above.
    // The "Type" alias further above therefore stores the type of
    // "Class::operator()". Note that when true, member "IsMemberFunction"
    // is also guaranteed to be true.
    //
    // IMPORTANT: Note that if someone instantiates "FunctionTraits" using
    // the actual type of "operator()" for its template arg, opposed to the
    // functor itself (the class containing "operator()"), then "IsFunctor"
    // will always be false, even though "FunctionTraits" targets the same
    // "operator()" in this case (again, because someone passed its type
    // directly, opposed to the functor class it belongs to). This is because
    // the partial specialization taking a non-static member function pointer
    // kicks in opposed to the specialization taking the (functor) class
    // type. It's by design IOW but given a non-static member function's type
    // only, there's no way to determine if it represents "operator()" anyway
    // (since another non-static member function with the exact same type
    // might exist in a class, so we wouldn't know if the function we're
    // working with is "operator()" or some other member that happens to have
    // the exact same type).
    //////////////////////////////////////////////////////////////////////////
    constexpr static bool IsFunctor = false;

protected:
    /////////////////////////////////////////////////////////
    // Default constructor. Made protected to prevent class
    // from being publicly constructed (since it serves as
    // a base class for "FunctionTraits" only - a traits
    // class in any case so for our purposes no instance
    // is normally ever required).
    /////////////////////////////////////////////////////////
    FunctionTraitsBase() = default;
};

//////////////////////////////////////////////////////////////////////////
// FunctionTraits. Primary template but all uses of this template always
// rely on a particular partial specialization defined by the macros just
// below (one for each permutation of function signature we may encounter).
// If template arg "F" isn't a function type suitable for passing to this
// template ("F" can effectively be any function type however, including
// pointers and references to functions, references to pointers to
// functions, and functors - see comments preceding "IsTraitsFunction" for
// details), then the following primary template kicks in. A compiler error
// therefore always occurs by design (the "static_assert" seen in the
// following struct triggers since "F" must be illegal by definition - no
// specialization kicked in to handle it).
////////////////////////////////////////////////////////////////////////////
template <typename F, int = 0, typename = void> // IMPORTANT: 2nd template arg *must* default to zero as seen (our
                                                // design depends on it - brittle only if someone changes it so don't).
                                                // Note that this is for internal use only anyway (design is based on
                                                // Raymond Chen's work as discussed earlier - it's a bit unwieldy but
                                                // given its purpose, better designs are hard to come by at this writing)
struct FunctionTraits
{
    // Always triggers! (illegal to come through here - see main comments above)
    static_assert(AlwaysFalse<F>, "Invalid template arg \"F\". Not a function type (or in very rare cases \"F\" "
                                  "is a non-static member function type with cv or reference qualifiers, AKA an " 
                                  "\"abominable\" function, which aren't supported by \"FunctionTraits\" - cv "
                                  "or ref qualified non-static member functions must be handled using "
                                  "pointer-to-member syntax). See \"IsTraitsFunction\" and in particular, "
                                  "\"Private::IsFreeFunction\" which it relies on for full details).");
};

///////////////////////////////////////////////////////////////////////////
// MAKE_FREE_FUNC_TRAITS_3 (macro for internal use only - #undefined when
// we're done with it). Creates a "FunctionTraits" partial specialization
// to handle both free (non-member) functions and static member functions
// (non-static member functions are handled later). Called multiple times
// just below for each unique combination of macro parameters (in
// succession by the numeric suffix at the end of each macro, starting
// at "1" and ending with this one, "3"). A partial specialization will
// therefore exist for all permutations of free functions and static
// member functions (i.e., those with different calling conventions,
// those with and without "noexcept", variadic functions, etc.). Anytime
// someone uses this class to retrieve the function traits for either a
// free (non-class) function or static member function, the appropriate
// partial specialization will therefore kick in. See Raymond Chen's 7
// articles (links) posted in the large header block further above (the
// following is based on his work though I've modified his code for our
// needs including adding support for non-static member functions)
///////////////////////////////////////////////////////////////////////////
#define MAKE_FREE_FUNC_TRAITS_3(CC, CALLING_CONVENTION, ARGS, IS_NOEXCEPT) \
        template <typename R, typename... Args> \
        struct FunctionTraits<R CC ARGS noexcept(IS_NOEXCEPT), \
                              !CallingConventionReplacedWithCdecl<true>(CALLING_CONVENTION) \
                              ? 0 : static_cast<int>(CALLING_CONVENTION)> \
            : FunctionTraitsBase<R CC ARGS noexcept(IS_NOEXCEPT), \
                                 R, \
                                 CALLING_CONVENTION, \
                                 #ARGS[9] != '\0', \
                                 void, \
                                 false, \
                                 false, \
                                 RefQualifier::None, \
                                 IS_NOEXCEPT, \
                                 Args...> \
        { \
        };

////////////////////////////////////////////////////////////
// Macro for internal use only (invokes macro just above).
// Handles "noexcept".
////////////////////////////////////////////////////////////
#define MAKE_FREE_FUNC_TRAITS_2(CC, CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_3(CC, CALLING_CONVENTION, ARGS, false) \
    MAKE_FREE_FUNC_TRAITS_3(CC, CALLING_CONVENTION, ARGS, true)
    
/////////////////////////////////////////////////////////////
// Macro for internal use only (invokes macro just above).
// Handles all combinations of CC, references to functions,
// cv-qualified pointers to functions, and references to
// cv-qualified pointers.
/////////////////////////////////////////////////////////////
#define MAKE_FREE_FUNC_TRAITS_1(CC, CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2(CC,                       CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC &),                   CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC &&),                  CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC *),                   CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * &),                 CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * &&),                CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * const),             CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * const &),           CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * const &&),          CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * volatile),          CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * volatile &),        CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * volatile &&),       CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * const volatile),    CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * const volatile &),  CALLING_CONVENTION, ARGS) \
    MAKE_FREE_FUNC_TRAITS_2((CC * const volatile &&), CALLING_CONVENTION, ARGS)

/////////////////////////////////////////////////////////////////////////
// Macro for internal use only (invokes macro just above). Creates
// partial specializations of "FunctionTraits" to handle non-variadic
// functions. Handles free functions and non-static member functions
// only (non-variadic in either case - variadic are handled just after,
// and non-static member functions are handled just after that).
/////////////////////////////////////////////////////////////////////////
#define MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(CC, CALLING_CONVENTION) \
    MAKE_FREE_FUNC_TRAITS_1(CC, CALLING_CONVENTION, (Args...))

//////////////////////////////////////////////////////////////////
// Macro calls for internal use only (invokes macro just above).
// Calls above macro once for each calling convention.
//////////////////////////////////////////////////////////////////
MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_CDECL,      CallingConvention::Cdecl)
MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_STDCALL,    CallingConvention::Stdcall)
MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_FASTCALL,   CallingConvention::Fastcall)
MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_VECTORCALL, CallingConvention::Vectorcall)
#ifdef STDEXT_CC_REGCALL
    MAKE_FREE_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_REGCALL, CallingConvention::Regcall)
#endif
#undef MAKE_FREE_FUNC_TRAITS_NON_VARIADIC // Done with this

////////////////////////////////////////////////////////////////////////
// Macro call for internal use only. Creates partial specializations of
// "FunctionTraits" to handle variadic functions (those whose last arg
// is "..."). Handles free functions and static member functions only
// (non-variadic functions were handled just above, and non-static
// member functions are handled later). Note that variadic functions
// always use the "cdecl" calling convention even for functions
// declared with a different calling convention (compiler ignores
// them). See the following by Raymond Chen (from MSFT) for instance
// (testing shows it applies to non-MSFT compilers as well, at least
// the ones we support but likely all others as well):
//
//     "If you try to declare a variadic function with an incompatible
//     calling convention, the compiler secretly converts it to cdecl"
//     https://devblogs.microsoft.com/oldnewthing/20131128-00/?p=2543
////////////////////////////////////////////////////////////////////////
MAKE_FREE_FUNC_TRAITS_1(STDEXT_CC_CDECL, CallingConvention::Cdecl, (Args..., ...))

// Done with these
#undef MAKE_FREE_FUNC_TRAITS_1
#undef MAKE_FREE_FUNC_TRAITS_2
#undef MAKE_FREE_FUNC_TRAITS_3

/////////////////////////////////////////////////////////////////////////
// Macro for internal use only. Creates a "FunctionTraits" partial
// specialization to handle non-static member functions. Called multiple
// times from the other macros just below for each unique combination of
// macro parameters (in succession by the numeric suffix at the end of
// each macro, starting at "1" and ending with this one, "8"). A partial
// specialization will therefore exist for all permutations of
// non-static member functions, e.g., those with different calling
// conventions, "const" vs non-const member functions, member functions
// with and without "noexcept", variadic functions, etc. Anytime someone
// uses this class to retrieve the function traits for a non-static
// member function, the appropriate partial specialization will
// therefore kick in.        
//
// IMPORTANT: Unlike free functions just above, the following creates
// partial specializations for pointers to member functions only
// (including references to such pointers), but NOT references to
// member functions, which aren't supported in C++ (nor the direct type
// of a member function itself - read on). From the C++ standard (e.g.,
// see the "Note" at the end 11.3.3 [3] in C++17):
//
//     "There is no “reference-to-member” type in C++"
// 
// Also see the following where the author even goes on to cite an email
// he sent to Stroustrup himself (who confirms it):
//
//    Why doesn't reference-to-member exist in C++?
//    https://stackoverflow.com/a/21952738
//
// As mentioned above, also note that only *pointers* to non-static
// member functions are supported (or references to such pointers), not
// the function's actual type when the pointer is removed (unlike for
// free functions including static member functions, where a
// specialization does exist for the function's actual type, in
// addition to pointers to that type, references to that type, and
// references to pointers to that type). See the comments preceding the
// primary "FuntionTraits" template for details.
/////////////////////////////////////////////////////////////////////////
#define MAKE_MEMBER_FUNC_TRAITS_8(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, VOLATILE, REF, IS_NOEXCEPT) \
    template <typename R, class C, typename... Args> \
    struct FunctionTraits<R (CC C::* CONST_PTR VOLATILE_PTR REF_PTR) ARGS CONST VOLATILE REF noexcept(IS_NOEXCEPT), \
                          !CallingConventionReplacedWithCdecl<false>(CALLING_CONVENTION) \
                          ? 0 : static_cast<int>(CALLING_CONVENTION)> \
        : FunctionTraitsBase<R (CC C::* CONST_PTR VOLATILE_PTR REF_PTR) ARGS CONST VOLATILE REF noexcept(IS_NOEXCEPT), \
                             R, \
                             CALLING_CONVENTION, \
                             #ARGS[9] != '\0', \
                             C, \
                             #CONST[0] != '\0', \
                             #VOLATILE[0] != '\0', \
                             #REF[0] == '\0' ? RefQualifier::None \
                                             : (#REF[1] == '\0' ? RefQualifier::LValue \
                                                                : RefQualifier::RValue), \
                             IS_NOEXCEPT, \
                             Args...> \
    { \
    };

// noexcept (macro for internal use only - invokes macro just above)
#define MAKE_MEMBER_FUNC_TRAITS_7(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, VOLATILE, REF) \
    MAKE_MEMBER_FUNC_TRAITS_8(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, VOLATILE, REF, false) \
    MAKE_MEMBER_FUNC_TRAITS_8(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, VOLATILE, REF, true)

// && and && (ref-qualifier on the function - macro for internal use only - invokes macro just above)
#define MAKE_MEMBER_FUNC_TRAITS_6(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, VOLATILE) \
    MAKE_MEMBER_FUNC_TRAITS_7(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, VOLATILE, ) \
    MAKE_MEMBER_FUNC_TRAITS_7(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, VOLATILE, &) \
    MAKE_MEMBER_FUNC_TRAITS_7(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, VOLATILE, &&)

// volatile (cv-qualifier on the function - macro for internal use only - invokes macro just above)
#define MAKE_MEMBER_FUNC_TRAITS_5(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST) \
    MAKE_MEMBER_FUNC_TRAITS_6(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, ) \
    MAKE_MEMBER_FUNC_TRAITS_6(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, CONST, volatile)
    
// const (cv-qualifier on the function - macro for internal use only - invokes macro just above)
#define MAKE_MEMBER_FUNC_TRAITS_4(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS) \
    MAKE_MEMBER_FUNC_TRAITS_5(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, ) \
    MAKE_MEMBER_FUNC_TRAITS_5(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, REF_PTR, ARGS, const)

// & and && (ref-qualifier on the pointer - macro for internal use only - invokes macro just above)
#define MAKE_MEMBER_FUNC_TRAITS_3(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, ARGS) \
    MAKE_MEMBER_FUNC_TRAITS_4(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR,   , ARGS) \
    MAKE_MEMBER_FUNC_TRAITS_4(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR,  &, ARGS) \
    MAKE_MEMBER_FUNC_TRAITS_4(CC, CALLING_CONVENTION, CONST_PTR, VOLATILE_PTR, &&, ARGS)

// volatile (cv-qualifier on the pointer - macro for internal use only - invokes macro just above)
#define MAKE_MEMBER_FUNC_TRAITS_2(CC, CALLING_CONVENTION, CONST_PTR, ARGS) \
    MAKE_MEMBER_FUNC_TRAITS_3(CC, CALLING_CONVENTION, CONST_PTR,         , ARGS) \
    MAKE_MEMBER_FUNC_TRAITS_3(CC, CALLING_CONVENTION, CONST_PTR, volatile, ARGS)

// const (cv-qualifier on the pointer - macro for internal use only - invokes macro just above)
#define MAKE_MEMBER_FUNC_TRAITS_1(CC, CALLING_CONVENTION, ARGS) \
    MAKE_MEMBER_FUNC_TRAITS_2(CC, CALLING_CONVENTION,      , ARGS) \
    MAKE_MEMBER_FUNC_TRAITS_2(CC, CALLING_CONVENTION, const, ARGS)

/////////////////////////////////////////////////////////////
// Non-variadic member functions (macro for internal use
// only - invokes macro just above). Launching point (first
// macro to be called) to create partial specializations for
// handling every permutation of non-variadic functions for
// the passed calling convention.
/////////////////////////////////////////////////////////////
#define MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(CC, CALLING_CONVENTION) \
    MAKE_MEMBER_FUNC_TRAITS_1(CC, CALLING_CONVENTION, (Args...))

//////////////////////////////////////////////////////
// Call above macro once for each calling convention
// (for internal use only - invokes macro just above
///////////////////////////////////////////////////////
MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_CDECL,      CallingConvention::Cdecl)
MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_STDCALL,    CallingConvention::Stdcall)
MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_FASTCALL,   CallingConvention::Fastcall)
MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_VECTORCALL, CallingConvention::Vectorcall)
#ifdef STDEXT_CC_REGCALL
    MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_REGCALL, CallingConvention::Regcall)
#endif
MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC(STDEXT_CC_THISCALL, CallingConvention::Thiscall)
#undef MAKE_MEMBER_FUNC_TRAITS_NON_VARIADIC // Done with this

///////////////////////////////////////////////////////
// Variadic member functions (for internal use only).
// Simply launches MAKE_MEMBER_FUNC_TRAITS_1 as seen
// which creates partial specializations for handling
// every permutation of variadic function. These are
// always STDEXT_CC_CDECL even for functions declared
// with a different calling convention (compiler
// ignores it for variadic functions - cdecl always
// used).
///////////////////////////////////////////////////////
MAKE_MEMBER_FUNC_TRAITS_1(STDEXT_CC_CDECL, CallingConvention::Cdecl, (Args..., ...))

// Done with these
#undef MAKE_MEMBER_FUNC_TRAITS_1
#undef MAKE_MEMBER_FUNC_TRAITS_2
#undef MAKE_MEMBER_FUNC_TRAITS_3
#undef MAKE_MEMBER_FUNC_TRAITS_4
#undef MAKE_MEMBER_FUNC_TRAITS_5
#undef MAKE_MEMBER_FUNC_TRAITS_6
#undef MAKE_MEMBER_FUNC_TRAITS_7
#undef MAKE_MEMBER_FUNC_TRAITS_8

//////////////////////////////////////////////////////////////////////////
// "FunctionTraits" partial specialization for handling function objects
// (AKA functors). This includes lambdas which are just syntactic sugar
// for creating function objects on the fly (the compiler creates a
// hidden functor behind the scenes). Template arg "T" therefore isn't a
// function type in this specialization (unlike all other specializations
// which handle function types and pointers and references to such
// types), but a class or struct type with a single "T::operator()"
// member function. The traits created by this specialization are
// therefore for that function. Note that overloads of "T::operator()"
// aren't supported however since it would be ambiguous (which
// "T::operator()" to use). In this case the following template will fail
// SFINAE (due to the ambiguity) so the primary template will kick in
// instead (which results in a "static_assert", i.e., the alias template
// can't be specialized).    
// 
//       Example 1 (functor)
//       -------------------
//       class MyFunctor
//       {
//       public:
//            int operator()(float, const std::string &);
//       };
// 
//       ////////////////////////////////////////////////////////////
//       // Resolves to "int". Following helper template relies on
//       // "FunctionTraits" to determine this (passing a functor
//       // here). See it for details.
//       ////////////////////////////////////////////////////////////
//       using MyClassReturnType_t = ReturnType_t<MyFunctor>
// 
//       ////////////////////////////////////////////////////////////
//       // Resolves to "const std::string &" (passing 1 here which
//       // is the zero-based index of the arg we want). Following
//       // helper template relies on "FunctionTraits" to determine
//       // this. See it for details.
//       ////////////////////////////////////////////////////////////
//       using Arg2Type_t = ArgType_t<MyFunctor, 1>
// 
//       Example 2 (lambda - note that lambdas are just functors)
//       --------------------------------------------------------
//       const auto myLambda = [](float val, const std::string &str)
//                               {
//                                   int rc;
//                                   // ...
//                                   return rc;
//                               };
// 
//       ///////////////////////////////////////////////////////////////
//       // Resolves to "int" (lambda's return type). Following helper
//       // template relies on "FunctionTraits" to determine this. See
//       // it for details.
//       ///////////////////////////////////////////////////////////////
//       using MyLambdaReturnType_t = ReturnType_t<decltype(myLambda)>;
// 
//       ////////////////////////////////////////////////////////////
//       // Resolves to "const std::string &" (passing 1 here which
//       // is the zero-based index of the arg we want). Following
//       // helper template relies on "FunctionTraits" to determine
//       // this. See it for details.
//       ////////////////////////////////////////////////////////////
//       using Arg2Type_t = ArgType_t<decltype(myLambda), 1>;
//////////////////////////////////////////////////////////////////////////
template <typename T>
struct FunctionTraits<T,
                      0, // Always zero (don't change it or this specialization won't kick in)
                      std::enable_if_t<Private::IsFunctor_v<T>>> : FunctionTraits<decltype(&std::remove_reference_t<T>::operator())>
{
    //////////////////////////////////////////////////////////
    // Overrides the same member in the "FunctionTraitsBase"
    // class (which is always false). See this for details.
    //////////////////////////////////////////////////////////
    constexpr static bool IsFunctor = true;
};

// Restore warnings we disabled earlier (see these for details)
#if defined(GCC)
    #pragma GCC diagnostic pop
#elif defined (__clang__)
    #pragma clang diagnostic pop
#elif defined(__INTEL_COMPILER)
    #pragma warning (pop)
#endif

//////////////////////////////////////////////////////////////////////////
// "IsFunctionTraits" (primary template). Primary template inherits from
// "std::false_type" if the template arg is *not* a specialization of
// struct "FunctionTraits". Otherwise, if it is a specialization of
// "FunctionTraits" then the specialization just below kicks in instead
// (inheriting from "std::true_type")
//
//    Example (see FUNCTION_TRAITS_C declaration further below)
//    ---------------------------------------------------------
//    template <FUNCTION_TRAITS_C T>
//    class Whatever
//    {
//       #if CPP17_OR_EARLIER
//            STATIC_ASSERT_IS_FUNCTION_TRAITS(T);
//       #endif
//     };
// 
//    ///////////////////////////////////////////////////////////////
//    // "static_assert" (C++17 or earlier) or concept (C++20 or
//    // later) succeeds above (template arg is a "FunctionTraits")
//    ///////////////////////////////////////////////////////////////
//    Whatever<FunctionTraits<int ()> whatever1; 
// 
//    //////////////////////////////////////////////////////////////////
//    // "static_assert" (C++17 or earlier) or concept (C++20 or later)
//    //  fails above (template arg is *not* a "FunctionTraits")
//    //////////////////////////////////////////////////////////////////
//    Whatever<int> whatever2; 
//////////////////////////////////////////////////////////////////////////
template <typename>
struct IsFunctionTraits : public std::false_type
{
};
    
//////////////////////////////////////////////////////////////////
// Specialization of "IsFunctionTraits" (primary) template just
// above. This specialization does all the work. See primary
// template above for details.
//////////////////////////////////////////////////////////////////
template <typename T, int ZeroIfCallingConventionNotReplaced, typename EnableIf>
struct IsFunctionTraits<FunctionTraits<T, ZeroIfCallingConventionNotReplaced, EnableIf>> : public std::true_type
{
};

//////////////////////////////////////////////////////////////////
// Helper variable template for "IsFunctionTraits" above (with
// the usual "_v" suffix). Set to true if "T" is a specialization
// of "FunctionTraits" or false otherwise.
//////////////////////////////////////////////////////////////////
template <typename T>
inline constexpr bool IsFunctionTraits_v = IsFunctionTraits<T>::value;

#if CPP20_OR_LATER
    template <typename T>
    concept FunctionTraits_c = IsFunctionTraits_v<T>;

    // See "#else" comments just below
    #define FUNCTION_TRAITS_C FunctionTraits_c
#else
    /////////////////////////////////////////////////////////////////
    // Useful macro that can be used in any template where you want
    // to check a template arg to ensure it's a "FunctionTraits"
    // specialization. The following will "static_assert" if it's
    // not. See "IsFunctionTraits" above for details (what we're
    // asserting on just below). Note that this macro is #defined in
    // C++17 only. In C++20 or later you can rely on concepts
    // instead so this macro should only be applied if
    // CPP17_OR_EARLIER is #defined. When not #defined then the
    // #defined constant FUNCTION_TRAITS_C above can be used instead
    // to apply the "FunctionTraits_c" concept. All code should
    // therefore be written similar to the following example (which
    // correctly targets whichever version of C++ is in effect,
    // applying the following macro if it's C++17 or earlier, or the
    // concept if C++20 or later):
    // 
    //      template <FUNCTION_TRAITS_C T>
    //      void SomeFunction()
    //      {
    //          #if CPP17_OR_EARLIER
    //              ///////////////////////////////////////////////
    //              // Kicks in if C++ 17 or earlier, otherwise
    //              // FUNCTION_TRAITS_C concept kicks in just
    //              // above instead (latter simply resolves to
    //              // the "typename" keyword in C++17 or earlier)
    //              ///////////////////////////////////////////////
    //              STATIC_ASSERT_IS_FUNCTION_TRAITS(T);
    //          #endif
    //          
    //          // ...
    //      }
    /////////////////////////////////////////////////////////////////
    #define STATIC_ASSERT_IS_FUNCTION_TRAITS(T) static_assert(IsFunctionTraits_v<T>, "\"" #T "\" must be a \"FunctionTraits\" specialization");

    // See comment block above
    #define FUNCTION_TRAITS_C typename
#endif

/////////////////////////////////////////////////////////////////////////
// FunctionTraitsArgCount_v. Helper template yielding
// "FunctionTraitsT::ArgCount" (number of args in the function
// represented by "FunctionTraits" not including variadic args if any),
// where "FunctionTraitsT" is a "FunctionTraits" specialization. Note
// that there's usually no benefit to this particular template however
// compared to calling "FunctionTraitsT::ArgCount" directly (there's no
// reduction in verbosity), but it's provided for consistency anyway (to
// ensure a helper template is available for all members
// of"FunctionTraits"). In most cases you should rely on the
// "ArgCount_v" helper template instead however, which just defers to
// "FunctionTraitsArgCount_v" and is easier to use.
//
// IMPORTANT:
// ---------
// Please note that if you wish to check if a function's argument list is
// completely empty, then inspecting this helper template for zero (0) is
// not sufficient, since it may return zero but still contain variadic
// args. To check for a completely empty argument list, call
// "IsFunctionTraitsEmptyArgList_v" instead. See this for further details.
//
// Lastly, note that the "FunctionTraitsT" template arg must be a
// "FunctionTraits" specialization or compilation will normally fail
// (concept kicks in in C++20 or later so failure is guaranteed - in C++17
// or earlier however there is no guarantee compilation will fail but
// usually will for other reasons - longer story but since C++20 or later
// is fast becoming the norm we won't worry about it - pass the expected
// "FunctionTraits" template arg and everything will be fine).
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr std::size_t FunctionTraitsArgCount_v = FunctionTraitsT::ArgCount;

/////////////////////////////////////////////////////////////////////////
// FunctionTraitsArgType_t. Helper alias template yielding
// "FunctionTraitsT::Args" (the type of the zero-based "Ith" arg of the
// function represented by "FunctionTraitsT"), where "FunctionTraitsT"
// is a "FunctionTraits" specialization. Note that it's less verbose
// than accessing the "Args" alias in "FunctionTraits" directly so you
// should normally rely on the following instead (or preferably the
// "ArgType_t" helper alias instead, which just defers to
// "FunctionTraitsArgType_t" and is even easier to use).  Note that "I"
// must be less than the number args in the function represented by
// "FunctionTraits" or a compiler errror occurs. If this function has
// no args or variadic args only (if it's variadic, i.e., the last arg
// is "..."), then a compiler error will always occur so this function
// should never be called in that case (since no args exist). You can
// check "FunctionTraitsArgCount_v" for this and if zero then it's not
// legal to use "FunctionTraitsArgType_t". Note that variadic args are
// never included so "I" can't be used to target them (they're
// effectively ignored as far as "I" is concerned). Lastly, note that
// the "FunctionTraitsT" template arg must be a "FunctionTraits"
// specialization or compilation will normally fail (concept kicks in
// in C++20 or later so failure is guaranteed - in C++17 or earlier
// however there is no guarantee compilation will fail but usually will
// for other reasons - longer story but since C++20 or later is fast
// becoming the norm we won't worry about it - pass the expected
// "FunctionTraits" template arg and everything will be fine).
//
// E.g.,
//
//     void MyFunction(const std::string &str, int val)
//     {
//     }
//
//     // "FunctionTraits" for above function
//     using MyFunctionTraits = FunctionTraits<decltype(MyFunction)>;
//     // using MyFunctionTraits = FunctionTraits<void (const std::string &, int)>; // This will also work (same as above but passing function's type on-the-fly)
//     // using MyFunctionTraits = FunctionTraits<void (*)(const std::string &, int)>; // ... and this too (but now using a pointer to the function - references
//                                                                                     // to the function will also work, and references to pointers to the
//                                                                                     // function as well)
//
//     ////////////////////////////////////////////////////////
//     // Type of second arg of "MyFunction" above (an "int").
//     // Passing 1 here since index is zero-based.
//     ////////////////////////////////////////////////////////
//     using MyFunctionArg2Type = FunctionTraitsArgType_t<MyFunctionTraits, 1>;
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */>
using FunctionTraitsArgType_t = typename FunctionTraitsT::template Args<I>::Type;

//////////////////////////////////////////////////////////////////////////
// FunctionTraitsArgTypeName(). See "FunctionTraitsArgType_t" just above.
// Simply converts that to a string suitable for display purposes and
// returns it as a "tstring_view"
//////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT, std::size_t I /* Zero-based */>
inline constexpr tstring_view FunctionTraitsArgTypeName_v = TypeName_v<FunctionTraitsArgType_t<FunctionTraitsT, I>>;

////////////////////////////////////////////////////////////////////////////
// FunctionTraitsArgTypes_t. Helper alias yielding
// "FunctionTraitsT::ArgTypes" (a "std::tuple" storing all arg types
// for the function represented by "FunctionTraits"), where
// "FunctionTraitsT" is a "FunctionTraits" specialization. Note that
// it's less verbose than accessing "FunctionTraits::ArgTypes" directly
// so you should normally rely on the following instead (or preferably
// the "ArgTypes_t" helper alias instead, which just defers to
// "FunctionTraitsArgTypes_t" and is even easier to use). Note that using
// this alias is rarely ever required in practice however since arg
// types can be individually accessed using the "FunctionTraitsArgType_t"
// helper instead (see this for details). Normally "FunctionTraitsArgTypes_t"
// only needs to be accessed if you need to iterate all the arg types
// for some reason but if so then you can just invoke helper function
// (template) "ForEachArg()" which exists for this purpose. See this
// for details. Lastly, note that the "FunctionTraitsT" template arg
// must be a "FunctionTraits" specialization or compilation will
// normally fail (concept kicks in in C++20 or later so failure is
// guaranteed - in C++17 or earlier however there is no guarantee
// compilation will fail but usually will for other reasons - longer
// story but since C++20 or later is fast becoming the norm we won't
// worry about it - pass the expected "FunctionTraits" template arg and
// everything will be fine).
// 
// E.g.,
// 
//       void MyFunction(const std::string &str, int val)
//       {
//       }
// 
//       using MyFunctionTraits = FunctionTraits<decltype(MyFunction)>;
//       // using MyFunctionTraits = FunctionTraits<void (const std::string &, int)>; // This will also work (same as above but passing function's type on-the-fly)
//       // using MyFunctionTraits = FunctionTraits<void (*)(const std::string &, int)>; // ... and this too (but now using a pointer to the function so the type
//                                                                                       // will now reflect that - references to the function will also work, and
//                                                                                       // references to pointers to the function as well)
// 
//       ////////////////////////////////////////////////////////////////
//       // "ArgTypes" for "MyFunction" above. A std::tuple" containing
//       // all args in the function so its size will be 2 in this case
//       // (first type a "const std::string &" and 2nd type an "int",
//       // as seen in the function above). Note that you can use
//       // "std::is_same" to compare this with the "ArgTypes" in another
//       // "FunctionTraits" to determine if they contain the exact same
//       // argument types.
//       ////////////////////////////////////////////////////////////////
//       using MyFunctionArgTypes = FunctionTraitsArgTypes_t<MyFunctionTraits>;
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
using FunctionTraitsArgTypes_t = typename FunctionTraitsT::ArgTypes;

//////////////////////////////////////////////////////////////////////
// FunctionTraitsCallingConvention_v. Helper template template
// yielding "FunctionTraitsT::CallingConvention" (the calling
// convention of the function repesented by "FunctionTraitsT"), where
// "FunctionTraitsT" is a "FunctionTraits" specialization. Note that
// there's usually no benefit to this particular helper template
// however compared to calling "FunctionTraitsT::CallingConvention"
// directly (there's no reduction in verbosity) but it's provided for
// consistency anyway (to ensure a helper template is available for
// all members of "FunctionTraits"). In most cases you should rely on
// "CallingConvention_v" helper template instead however, which just
// defers to "FunctionTraitsCallingConvention_v" and is easier to
// use.
//
// Please note that compilers will sometimes change the explicitly
// declared calling convention of a function to "cdecl" (whatever the
// syntax is for this on the given platform) depending on the
// compiler options in effect, in particular when compiling for 64
// bits. In these cases the following will always return
// "CallingConvention::Cdecl" regardless of what the calling
// convention is explicitly set to on the function (since "cdecl" is
// the actual calling convention applied by the compiler). Lastly,
// note that the "FunctionTraitsT" template arg must be a
// "FunctionTraits" specialization or compilation will normally fail
// (concept kicks in in C++20 or later so failure is guaranteed - in
// C++17 or earlier however there is no guarantee compilation will
// fail but usually will for other reasons - longer story but since
// C++20 or later is fast becoming the norm we won't worry about it
// - pass the expected "FunctionTraits" template arg and everything
//   will be fine).
//////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr CallingConvention FunctionTraitsCallingConvention_v = FunctionTraitsT::CallingConvention;

///////////////////////////////////////////////////////////////////////////////////
// FunctionTraitsCallingConventionName_v. See "FunctionTraitsCallingConvention_v"
// just above. Simply converts that to a string suitable for display purposes and
// returns it as a "tstring_view"
///////////////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr tstring_view FunctionTraitsCallingConventionName_v = CallingConventionToString(FunctionTraitsCallingConvention_v<FunctionTraitsT>);

/////////////////////////////////////////////////////////////////////////
// FunctionTraitsFunctionType_t. Helper alias yielding "FunctionTraitsT::Type"
// (the type of the function repesented by "FunctionTraitsT"), where
// "FunctionTraitsT" is a "FunctionTraits" specialization. Note that
// it's less verbose than accessing "FunctionTraits::Type" directly so
// you should normally rely on the following instead (or preferably the
// "FunctionType_t" helper alias instead, which just defers to
// "FunctionTraitsFunctionType_t" and is even easier to use). Note that the
// "FunctionTraitsT" template arg must be a "FunctionTraits"
// specialization or compilation will normally fail (concept kicks in
// in C++20 or later so failure is guaranteed - in C++17 or earlier
// however there is no guarantee compilation will fail but usually will
// for other reasons - longer story but since C++20 or later is fast
// becoming the norm we won't worry about it - pass the expected
// "FunctionTraits" template arg and everything will be fine).    
// 
// E.g.,
// 
//       void MyFunction(const std::string &str, int val)
//       {
//       }
// 
//       // "FunctionTraits" for above function
//       using MyFunctionTraits = FunctionTraits<decltype(MyFunction)>;
//       // using MyFunctionTraits = FunctionTraits<void (const std::string &, int)>; // This will also work (same as above but passing function's type on-the-fly)
//       // using MyFunctionTraits = FunctionTraits<void (*)(const std::string &, int)>; // ... and this too (but now using a pointer to the function so the type
//                                                                                       // will now reflect that - references to the function will also work, and
//                                                                                       // references to pointers to the function as well)
// 
//       //////////////////////////////////////////////////////////////
//       // Type of "MyFunction" above. Yields the following (though
//       // it might include the calling convention as well depending
//       // on the platform):
//       //
//       //     void (const std::string &, int)
//       //////////////////////////////////////////////////////////////
//       using MyFunctionType = FunctionTraitsFunctionType_t<MyFunctionTraits>;
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
using FunctionTraitsFunctionType_t = typename FunctionTraitsT::Type;

////////////////////////////////////////////////////////////////////
// FunctionTraitsTypeName. See "FunctionTraitsFunctionType_t" just
// above. Simply converts that to a string suitable for display
// purposes and returns it as a "tstring_view"
////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr tstring_view FunctionTraitsTypeName_v = TypeName_v<FunctionTraitsFunctionType_t<FunctionTraitsT>>;

///////////////////////////////////////////////////////////////////////////
// IsFunctionTraitsFreeFunction_v. Helper template yielding
// "FunctionTraitsT::IsFreeFunction" (true or false to indicate if the
// function repesented by "FunctionTraitsT" is a free function including
// static member functions), where "FunctionTraitsT" is a "FunctionTraits"
// specialization. Note that there's usually no benefit to this particular
// template however compared to calling "FunctionTraitsT::IsFreeFunction"
// directly (there's no reduction in verbosity), but it's provided for
// consistency anyway (to ensure a template is available for all members
// of "FunctionTraits"). In most cases you should rely on the
// "IsFreeFunction_v" helper template instead however, which just defers
// to "IsFunctionTraitsFreeFunction_v" and is easier to use.
//
// Lastly, note that the "FunctionTraitsT" template arg must be a
// "FunctionTraits" specialization or compilation will normally fail
// (concept kicks in in C++20 or later so failure is guaranteed - in C++17
// or earlier however there is no guarantee compilation will fail but
// usually will for other reasons - longer story but since C++20 or later
// is fast becoming the norm we won't worry about it - pass the expected
// "FunctionTraits" template arg and everything will be fine).
///////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr bool IsFunctionTraitsFreeFunction_v = FunctionTraitsT::IsFreeFunction;

/////////////////////////////////////////////////////////////////////////
// IsFunctionTraitsFunctor_v. Helper template yielding
// "FunctionTraitsT::IsFunctor" (true or false to indicate if the
// function repesented by "FunctionTraitsT" is a functor), where
// "FunctionTraitsT" is a "FunctionTraits" specialization. If "true"
// then note that "IsFunctionTraitsMemberFunction_v" is also guaranteed
// to return true. Please note that there's usually no benefit to this
// particular helper template however compared to calling
// "FunctionTraitsT::IsFunctor" directly (there's no reduction in
// verbosity), but it's provided for consistency anyway (to ensure a
// helper template is available for all members of "FunctionTraits"). In
// most cases you should rely on the "IsFunctor_v" helper template
// instead however, which just defers to "IsFunctionTraitsFunctor_v" and
// is easier to use.
//
// Lastly, note that the "FunctionTraitsT" template arg must be a
// "FunctionTraits" specialization or compilation will normally fail
// (concept kicks in in C++20 or later so failure is guaranteed - in
// C++17 or earlier however there is no guarantee compilation will fail
// but usually will for other reasons - longer story but since C++20 or
// later is fast becoming the norm we won't worry about it - pass the
// expected "FunctionTraits" template arg and everything will be fine).
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr bool IsFunctionTraitsFunctor_v = FunctionTraitsT::IsFunctor;

/////////////////////////////////////////////////////////////////////////
// IsFunctionTraitsMemberFunction_v. Helper template yielding
// "FunctionTraitsT::IsMemberFunction" (which stores "true" if
// "FunctionTraitsT" represents a non-static member function including
// functors), where "FunctionTraitsT" is a "FunctionTraits"
// specialization. Note that there's usually no benefit to this
// particular helper template however compared to calling
// "FunctionTraitsT::IsMemberFunction" directly (there's no reduction in
// verbosity), but it's provided for consistency anyway (to ensure a
// helper template is available for all members of"FunctionTraits"). In
// most cases you should rely on the "IsMemberFunction_v" helper
// template instead however, which just defers to
// "IsFunctionTraitsMemberFunction_v" and is easier to use.
//
// Note that you might want to invoke the following to determine if
// "FunctionTraits" does in fact target a non-static member function
// before invoking any other helper template specific to non-static member
// functions only (if you don't know this ahead of time). The following
// templates are affected:
//
//     1) FunctionTraitsMemberFunctionClass_t
//     2) MemberFunctionTraitsClassName
//     3) IsFunctionTraitsMemberFunctionConst_v
//     4) IsFunctionTraitsMemberFunctionVolatile_v
//     4) FunctionTraitsMemberFunctionRefQualifier_v
//     6) FunctionTraitsMemberFunctionRefQualifierName_v
//
// Lastly, note that the "FunctionTraitsT" template arg must be a
// "FunctionTraits" specialization or compilation will normally fail
// (concept kicks in in C++20 or later so failure is guaranteed - in
// C++17 or earlier however there is no guarantee compilation will fail
// but usually will for other reasons - longer story but since C++20 or
// later is fast becoming the norm we won't worry about it - pass the
// expected "FunctionTraits" template arg and everything will be fine).
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr bool IsFunctionTraitsMemberFunction_v = FunctionTraitsT::IsMemberFunction;

/////////////////////////////////////////////////////////////////////////
// IsFunctionTraitsMemberFunctionConst_v. Helper template yielding
// "FunctionTraitsT::IsConst" (storing true or false to indicate if the
// non-static function repesented by "FunctionTraitsT" is declared with
// the "const" qualifier), where "FunctionTraitsT" is a "FunctionTraits"    
// specialization. Note that there's usually no benefit to this
// particular helper template however compared to calling
// "FunctionTraitsT::IsConst" directly (there's no reduction in
// verbosity), but it's provided for consistency anyway (to ensure a
// helper template is available for all members of "FunctionTraits"). In
// most cases you should rely on the "IsMemberFunctionConst_v" helper
// template instead however, which just defers to
// "IsFunctionTraitsMemberFunctionConst_v" and is easier to use.
//
// Note that this member is always false for free functions and static
// member functions (not applicable to them). You may therefore want to
// call "IsFunctionTraitsMemberFunction_v" to determine if
// "FunctionTraitsT" targets a non-static member function before
// invoking the following (if required). Lastly, note that the
// "FunctionTraitsT" template arg must be a "FunctionTraits"
// specialization or compilation will normally fail (concept kicks in in
// C++20 or later so failure is guaranteed - in C++17 or earlier however
// there is no guarantee compilation will fail but usually will for
// other reasons - longer story but since C++20 or later is fast
// becoming the norm we won't worry about it - pass the expected
// "FunctionTraits" template arg and everything will be fine).
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr bool IsFunctionTraitsMemberFunctionConst_v = FunctionTraitsT::IsConst;

/////////////////////////////////////////////////////////////////////////
// IsFunctionTraitsMemberFunctionVolatile_v. Helper template yielding
// "FunctionTraitsT::IsVolatile" (storing true or false to indicate if
// the non-static function repesented by "FunctionTraitsT" is declared
// with the "volatile" qualifier), where "FunctionTraitsT" is a
// "FunctionTraits" specialization. Note that there's usually no benefit
// to this particular helper template however compared to calling
// "FunctionTraitsT::IsVolatile" directly (there's no reduction in
// verbosity), but it's provided for consistency anyway (to ensure a
// helper template is available for all members of "FunctionTraits"). In
// most cases you should rely on the "IsMemberFunctionVolatile_v" helper
// template instead however, which just defers to
// "IsFunctionTraitsMemberFunctionVolatile_v" and is easier to use.
//
// Note that this member is always false for free functions and static
// member functions (not applicable to them). You may therefore want to
// call "IsFunctionTraitsMemberFunction_v" to determine if
// "FunctionTraitsT" targets a non-static member function before
// invoking the following (if required). Lastly, note that the
// "FunctionTraitsT" template arg must be a "FunctionTraits"
// specialization or compilation will normally fail (concept kicks in in
// C++20 or later so failure is guaranteed - in C++17 or earlier however
// there is no guarantee compilation will fail but usually will for
// other reasons - longer story but since C++20 or later is fast
// becoming the norm we won't worry about it - pass the expected
// "FunctionTraits" template arg and everything will be fine).
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr bool IsFunctionTraitsMemberFunctionVolatile_v = FunctionTraitsT::IsVolatile;

/////////////////////////////////////////////////////////////////////////
// IsFunctionTraitsNoexcept_v. Helper template yielding
// "FunctionTraitsT::IsNoexcept" (true or false to indicate if the
// function repesented by "FunctionTraitsT" is declared as "noexcept"),
// where "FunctionTraitsT" is a "FunctionTraits" specialization. Note
// that there's usually no benefit to this particular helper template
// however compared to calling "FunctionTraitsT::IsNoexcept" directly
// (there's no reduction in verbosity), but it's provided for
// consistency anyway (to ensure a helper template is available for all
// members of "FunctionTraits"). In most cases you should rely on the
// "IsNoexcept_v" helper template instead however, which just defers to
// "IsFunctionTraitsNoexcept_v" and is easier to use.
//
// Lastly, note that the "FunctionTraitsT" template arg must be a
// "FunctionTraits" specialization or compilation will normally fail
// (concept kicks in in C++20 or later so failure is guaranteed - in
// C++17 or earlier however there is no guarantee compilation will fail
// but usually will for other reasons - longer story but since C++20 or
// later is fast becoming the norm we won't worry about it - pass the
// expected "FunctionTraits" template arg and everything will be fine).
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr bool IsFunctionTraitsNoexcept_v = FunctionTraitsT::IsNoexcept;

//////////////////////////////////////////////////////////////////////
// IsFunctionTraitsVariadic_v. Helper template yielding
// "FunctionTraitsT::IsVariadic" (the type of the function repesented
// by "FunctionTraitsT"), where "FunctionTraitsT" is a
// "FunctionTraits" specialization. Note that there's usually no
// benefit to this particular helper template however compared to
// calling "FunctionTraitsT::IsVariadic" directly (there's no
// reduction in verbosity), but it's provided for consistency anyway
// (to ensure a helper template is available for all members of
// "FunctionTraits"). In most cases you should rely on the
// "IsVariadic_v" helper template instead however, which just defers
// to "IsFunctionTraitsVariadic_v" and is easier to use.
//
// Note that the "FunctionTraitsT" template arg must be a
// "FunctionTraits" specialization or compilation will normally fail
// (concept kicks in in C++20 or later so failure is guaranteed - in
// C++17 or earlier however there is no guarantee compilation will
// fail but usually will for other reasons - longer story but since
// C++20 or later is fast becoming the norm we won't worry about it -
// pass the expected "FunctionTraits" template arg and everything
// will be fine).
//////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr bool IsFunctionTraitsVariadic_v = FunctionTraitsT::IsVariadic;

/////////////////////////////////////////////////////////////////////////
// FunctionTraitsMemberFunctionClass_t. Helper alias template yielding
// "FunctionTraitsT::Class" (the class for a non-static member function
// repesented by "FunctionTraitsT"), where "FunctionTraitsT" is a
// "FunctionTraits" specialization. Note that it's less verbose than
// accessing the "Class" alias in "FunctionTraits" directly so you
// should normally rely on the following instead (or preferably the
// "MemberFunctionClass_t" helper alias instead, which just defers to
// "FunctionTraitsMemberFunctionClass_t" and is even easier to use).
// Note that this helper applies to non-static member functions only
// (yielding the class type of non-static member functions). For free
// functions including static member functions this type is always
// "void" (not applicable to them noting that the class for static
// member functions can't be retrieved in this release due to language
// limitations). Note that you might want to call
// "IsFunctionTraitsMemberFunction_v" to determine if "FunctionTraitsT"
// targets a non-static member function before invoking the following
// (if required). Lastly, the "FunctionTraitsT" template arg must be a
// "FunctionTraits" specialization or compilation will normally fail
// (concept kicks in in C++20 or later so failure is guaranteed - in
// C++17 or earlier however there is no guarantee compilation will fail
// but usually will for other reasons - longer story but since C++20 or
// later is fast becoming the norm we won't worry about it - pass the
// expected "FunctionTraits" template arg and everything will be fine).
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
using FunctionTraitsMemberFunctionClass_t = typename FunctionTraitsT::Class;

//////////////////////////////////////////////////////////////////////////////////
// MemberFunctionTraitsClassName. See "FunctionTraitsMemberFunctionClass_t" just
// above. Simply converts that to a string suitable for display purposes and
// returns it as a "tstring_view"
//////////////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr tstring_view FunctionTraitsMemberFunctionClassName_v = TypeName_v<FunctionTraitsMemberFunctionClass_t<FunctionTraitsT>>;

/////////////////////////////////////////////////////////////////////////
// FunctionTraitsMemberFunctionRefQualifier_v. Helper template yielding
// "FunctionTraitsT::RefQualifier" (which stores the reference qualifier
// "&" or "&&" if the non-static function repesented by
// "FunctionTraitsT" is declared with either), where "FunctionTraitsT"
// is a "FunctionTraits" specialization. Note that there's usually no
// benefit to this particular helper template however compared to
// calling "FunctionTraitsT::RefQualifier" directly (there's no
// reduction in verbosity), but it's provided for consistency anyway (to
// ensure a helper template is available for all members of
// "FunctionTraits"). In most cases you should rely on the
// "IsMemberFunctionRefQualifier_v" helper template instead however,
// which just defers to "IsMemberFunctionTraitsRefQualifier_v" and is
// easier to use.
//
// Note that this member is always "RefQualifier::None" for free
// functions and static member functions (not applicable to them). You
// may therefore want to call "IsFunctionTraitsMemberFunction_v" to
// determine if "FunctionTraitsT" targets a non-static member function
// before invoking the following (if required). Lastly, note that the
// "FunctionTraitsT" template arg must be a "FunctionTraits"
// specialization or compilation will normally fail (concept kicks in in
// C++20 or later so failure is guaranteed - in C++17 or earlier however
// there is no guarantee compilation will fail but usually will for
// other reasons - longer story but since C++20 or later is fast
// becoming the norm we won't worry about it - pass the expected
// "FunctionTraits" template arg and everything will be fine).
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr RefQualifier FunctionTraitsMemberFunctionRefQualifier_v = FunctionTraitsT::RefQualifier;

////////////////////////////////////////////////////////////////////////////////////////////////////
// FunctionTraitsMemberFunctionRefQualifierName_v. See "FunctionTraitsMemberFunctionRefQualifier_v"
// just above. Simply converts that to a string suitable for display purposes and returns it as a
// "tstring_view". Pass "true" (the default) for the "UseAmpersands" template arg if you wish to
// return this as "&" or "&&" or "false" to return it as "LValue" or "RValue" instead (but in eiter
// case however "None" is always returned if "F" has no reference-qualifier which is usually the
// case for most functions in practice.
////////////////////////////////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT, bool UseAmpersands = true>
inline constexpr tstring_view FunctionTraitsMemberFunctionRefQualifierName_v = RefQualifierToString(FunctionTraitsMemberFunctionRefQualifier_v<FunctionTraitsT>, // refQualifier
                                                                                                    UseAmpersands);

/////////////////////////////////////////////////////////////////////////
// FunctionTraitsReturnType_t. Helper alias template yielding
// "FunctionTraitsT::ReturnType" (the return type of the function
// repesented by "FunctionTraitsT"), where "FunctionTraitsT" is a
// "FunctionTraits" specialization. Note that it's less verbose than
// accessing the "ReturnType" alias in "FunctionTraits" directly so you
// should normally rely on the following instead (or preferably the
// "ReturnType_t" helper alias instead, which just defers to
// "FunctionTraitsReturnType_t" and is even easier to use). Note that
// the "FunctionTraitsT" template arg must be a "FunctionTraits"
// specialization or compilation will normally fail (concept kicks in
// in C++20 or later so failure is guaranteed - in C++17 or earlier
// however there is no guarantee compilation will fail but usually will
// for other reasons - longer story but since C++20 or later is fast
// becoming the norm we won't worry about it - pass the expected
// "FunctionTraits" template arg and everything will be fine).
// 
// E.g.,
// 
//       int MyFunction(const std::string &str, int val)
//       {
//       }
// 
//       // "FunctionTraits" for above function
//       using MyFunctionTraits = FunctionTraits<decltype(MyFunction)>;
//       // using MyFunctionTraits = FunctionTraits<int (const std::string &, int)>; // This will also work (same as above but passing function's type on-the-fly)
//       // using MyFunctionTraits = FunctionTraits<int (*)(const std::string &, int)>; // ... and this too (but now using a pointer to the function so the type
//                                                                                      // will now reflect that - references to the function will also work, and
//                                                                                      // references to pointers to the function as well)
// 
//       // Return type for "MyFunction" above (int)
//       using MyFunctionReturnType = FunctionTraitsReturnType_t<MyFunctionTraits>;
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
using FunctionTraitsReturnType_t = typename FunctionTraitsT::ReturnType;

///////////////////////////////////////////////////////////////////////
// FunctionTraitsReturnTypeName. See "FunctionTraitsReturnType_t" just
// above. Simply converts that to a string suitable for display
// purposes and returns it as a "tstring_view" (so a return type like,
// say, "int" will literally be returned as the string "int", quotes
// not included)
///////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr tstring_view FunctionTraitsReturnTypeName_v = TypeName_v<FunctionTraitsReturnType_t<FunctionTraitsT>>;

////////////////////////////////////////////////////////////////////////////
// IsFunctionTraitsEmptyArgList_v. Helper template returning "true" if the
// function represented by "FunctionTraitsT" has an empty arg list (it has
// no args whatsoever including variadic args), or "false" otherwise (where
// "FunctionTraitsT" is a "FunctionTraits" specialization). If true then
// note that the "FunctionTraitsArgCount_v" helper is guaranteed to return
// zero (0), and the "IsFunctionTraitsVariadic_v" helper is guaranteed to
// return false. Please note that in most cases you should rely on the
// "IsEmptyArgList_v" helper template instead however, which just defers to
// "IsFunctionTraitsEmptyArgList_v" and is easier to use.
//
// IMPORTANT:
// ----------
// Note that you should rely on this helper to determine if a function's
// argument list is completely empty opposed to checking the
// "FunctionTraitsArgCount_v" helper for zero (0), since the latter returns
// zero only if the function represented by "FunctionTraitsT" has no
// non-variadic args. If it has variadic args but no others, i.e., its
// argument list is "(...)", then the argument list isn't completely empty
// even though "FunctionTraitsArgCount_v" returns zero (since it still has
// variadic args). Caution advised.
//
// Lastly, note that the "FunctionTraitsT" template arg must be a
// "FunctionTraits" specialization or compilation will normally fail
// (concept kicks in in C++20 or later so failure is guaranteed - in C++17
// or earlier however there is no guarantee compilation will fail but
// usually will for other reasons - longer story but since C++20 or later
// is fast becoming the norm we won't worry about it - pass the expected
// "FunctionTraits" template arg and everything will be fine).
////////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT>
inline constexpr bool IsFunctionTraitsEmptyArgList_v = FunctionTraitsArgCount_v<FunctionTraitsT> == 0 && // Zero (non-variadic) args
                                                       !IsFunctionTraitsVariadic_v<FunctionTraitsT>; // Not variadic

/////////////////////////////////////////////////////////////////////////
// ArgCount_v. Helper template for "FunctionTraits::ArgCount" which
// yields the number of args in function "F" not including variadic args
// if any (i.e., where the last arg is "..."). Less verbose however than
// creating a "FunctionTraits" directly from "F" and accessing its
// "ArgCount" member. The following provides a convenient wrappper.
//
// IMPORTANT:
// ---------
// Please note that if you wish to check if a function's argument list
// is completely empty, then inspecting this helper template for zero
// (0) is not sufficient, since it may return zero but still contain
// variadic args. To check for a completely empty argument list, call
// "IsEmptyArgList_v" instead. See this for further details.
/////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr std::size_t ArgCount_v = FunctionTraitsArgCount_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

////////////////////////////////////////////////////////////////////////////
// ArgType_t. Helper alias for "FunctionTraits::Args" but less verbose than
// creating a "FunctionTraits" directly from "F" and accessing its "Args"
// alias (allowing you to retrieve the type of any non-variadic arg in
// the function). The following provides a convenient wrappper. Yields
// the type of the (zero-based) "Ith" arg of function "F" where "I" must
// be less than the number args in function "F" or a compiler errror
// occurs. If "F" has no args or variadic args only (if it's variadic),
// then a compiler error will always occur so this function should never
// be called in that case (since no args exist). You can check
// "ArgCount_v" for this and if zero then it's not legal to use
// "ArgType_t". Note that variadic args are never included so "I" can't be
// used to target them (they're effectively ignored as far as "I" is
// concerned).    
// 
// E.g.,
// 
//       void MyFunction(const std::string &str, int val)
//       {
//       }
// 
//       ////////////////////////////////////////////////////////
//       // Type of second arg of "MyFunction" above (an "int").
//       // Passing 1 here since index is zero-based.
//       ////////////////////////////////////////////////////////
//       using Arg2Type = ArgType_t<decltype(MyFunc), 1>;
//       // using Arg2Type = ArgType_t<void (const std::string &, int)>; // This will also work (same as above but passing function's type on-the-fly)
//       // using Arg2Type = ArgType_t<void (*)(const std::string &, int)>; // ... and this too (but now using a pointer to the function - references
//                                                                          // to the function will also work, and references to pointers to the
//                                                                          // function as well)
////////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */>
using ArgType_t = FunctionTraitsArgType_t<FunctionTraits<F>, I>; // Defers to the "FunctionTraits" helper further above

//////////////////////////////////////////////////////////////////////
// ArgTypeName(). See "ArgType_t()" just above. Simply converts that
// to a string suitable for display purposes and returns it as a
// "tstring_view"
//////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F, std::size_t I /* Zero-based */>
inline constexpr tstring_view ArgTypeName_v = FunctionTraitsArgTypeName_v<FunctionTraits<F>, I>; // Defers to the "FunctionTraits" helper further above

/////////////////////////////////////////////////////////////////////////
// ArgTypes_t. Helper alias for "FunctionTraits::ArgTypes" which yields
// a "std::tuple" storing all arg types for function "F" but less
// verbose than creating a "FunctionTraits" directly from "F" and
// accessing its "ArgTypes" alias. The following provides a convenient
// wrappper. Note that using this alias is rarely ever required in
// practice however since arg types can be individually accessed using
// the "ArgType_t" helper alias instead (see this for details). Normally
// "ArgTypes_t" only needs to be accessed if you need to iterate all the
// arg types for some reason but if so then you can just invoke helper
// function (template) "ForEachArg()" which exists for this purpose.
// See this for details.
//
// E.g.,
//
//     void MyFunction(const std::string &str, int val)
//     {
//     }
//
//     ////////////////////////////////////////////////////////////////
//     // "ArgTypes" for "MyFunction" above. A std::tuple" containing
//     // all args in the function so its size will be 2 in this case
//     // (first type a "const std::string &" and 2nd type an "int",
//     // as seen in the function above). Note that you can use
//     // "std::is_same" to compare this with the "ArgTypes" in another
//     // "FunctionTraits" to determine if they contain the exact same
//     // argument types. You can also iterate these types using helper
//     // "ForEachArg()". See this for details.
//     ////////////////////////////////////////////////////////////////
//     using MyFunctionArgTypes = ArgTypes_t<decltype(MyFunction)>;
//     // using MyFunctionArgTypes = ArgTypes_t<void (const std::string &, int)>; // This will also work (same as above but passing function's type on-the-fly)
//     // using MyFunctionArgTypes = ArgTypes_t<void (*)(const std::string &, int)>; // ... and this too (but now using a pointer to the function - references
//                                                                                   // to the function will also work, and references to pointers to the
//                                                                                   // function as well)
/////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
using ArgTypes_t = FunctionTraitsArgTypes_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

///////////////////////////////////////////////////////////////////////////////
// CallingConvention_v. Helper template for "FunctionTraits::CallingConvention"
// which yields the calling convention of function "F" but less verbose than
// creating a "FunctionTraits" directly from "F" and accessing its
// "CallingConvention" member. The following provides a convenient wrappper.
// Note however that compilers will sometimes change the explicitly declared
// calling convention of a function to "cdecl" (whatever the syntax is for
// this on the given platform) depending on the compiler options in effect,
// in particular when compiling for 64 bits. In these cases the following
// will always return "CallingConvention::Cdecl" regardless of what the
// calling convention is explicitly set to on the function (since "cdecl" is
// the actual calling convention applied by the compiler)
///////////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr CallingConvention CallingConvention_v = FunctionTraitsCallingConvention_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

///////////////////////////////////////////////////////////////////
// CallingConventionName_v. See "CallingConvention_v" just above.
// Simply converts that to a string suitable for display purposes
// and returns it as a "tstring_view"
///////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view CallingConventionName_v = FunctionTraitsCallingConventionName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

//////////////////////////////////////////////////////////////////////
// FunctionType_t. Helper alias for "FunctionTraits::Type" which
// yields the type of function "F" but less verbose than creating a
// "FunctionTraits" directly and accessing its "Type" alias. The
// following provides a convenient wrappper.
// 
// E.g.,
// 
//       void MyFunction(const std::string &str, int val)
//       {
//       }
// 
//       //////////////////////////////////////////////////////////////
//       // Type of "MyFunction" above. Yields the following (though
//       // it might include the calling convention as well depending
//       // on the platform):
//       //
//       //     void (const std::string &, int)
//       //////////////////////////////////////////////////////////////
//       using MyFunctionType = FunctionType_t<decltype(MyFunction)>;
//       // using MyFunctionType = FunctionType_t<void (const std::string &, int)>; // This will also work (same as above but passing function's type on-the-fly)
//       // using MyFunctionType = FunctionType_t<void (*)(const std::string &, int)>; // ... and this too (but now using a pointer to the function so the type
//                                                                                     // will now reflect that - references to the function will also work, and
//                                                                                     // references to pointers to the function as well)
////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
using FunctionType_t = FunctionTraitsFunctionType_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

/////////////////////////////////////////////////////////////////////////
// FunctionTypeName_v. See "FunctionType_t" just above. Simply converts
// that to a string suitable for display purposes and returns it as a
// "tstring_view"
/////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view FunctionTypeName_v = FunctionTraitsTypeName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

///////////////////////////////////////////////////////////////////////////
// IsEmptyArgList_v. Helper template yielding "true" if the function
// represented by "F" has an empty arg list (it has no args whatsoever
// including variadic args), or "false" otherwise. If true then note that
// the "ArgCount_v" helper is guaranteed to return zero (0), and the
// "IsVariadic_v" helper is guaranteed to return false. Note that this
// helper template is less verbose than creating a "FunctionTraits"
// directly from "F" and calling its own "IsFunctionTraitsEmptyArgList_v"
// helper. The follow wraps the latter call for you.
//
// IMPORTANT:
// ----------
// Note that you should rely on this helper to determine if a function's
// argument list is completely empty opposed to checking the "ArgCount_v"
// helper for zero (0), since the latter returns zero only if "F" has no
// non-variadic args. If it has variadic args but no others, i.e., its
// argument list is "(...)", then the argument list isn't completely empty
// even though "ArgCount_v" returns zero (since it still has variadic
// args). Caution advised.
///////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsEmptyArgList_v = IsFunctionTraitsEmptyArgList_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

//////////////////////////////////////////////////////////////////////////////
// IsFreeFunction_v. Helper template for "FunctionTraits::IsFreeFunction"
// which stores "true" if "F" is a free function (including static member
// functions) or false otherwise. This helper template is less verbose
// however than creating a "FunctionTraits" directly from "F" and accessing
// its "IsFreeFunction" member. The following provides a convenient wrappper.
//////////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsFreeFunction_v = IsFunctionTraitsFreeFunction_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

///////////////////////////////////////////////////////////////////////
// IsFunctor_v. Helper template for "FunctionTraits::IsFunctor" which
// stores "true" if "F" is a functor or false otherwise (if "true"
// then note that "IsMemberFunction_v" is also guaranteed to return
// true). This helper template is less verbose however than creating a
// "FunctionTraits" directly from "F" and accessing its "IsFunctor"
// member. The following provides a convenient wrappper.
///////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsFunctor_v = IsFunctionTraitsFunctor_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

///////////////////////////////////////////////////////////////////////////
// IsMemberFunction_v. Helper template for "FunctionTraits::IsMemberFunction"
// which stores "true" if "F" is a non-static member function (or a
// functor) or "false" otherwise (in the latter case "F" will always be
// either a free function or static member function - see
// "IsFreeFunction_v"). This helper template is less verbose however than
// creating a "FunctionTraits" directly from "F" and accessing its
// "IsMemberFunction" member. Note that you might want to invoke the
// following to determine if "F" is in fact a non-static member function
// before invoking any other helper template specific to non-static member
// functions only (if you don't know this ahead of time). The following
// templates are affected:
//
//     1) MemberFunctionClass_t
//     2) MemberFunctionClassName_v
//     3) IsMemberFunctionConst_v
//     4) IsMemberFunctionVolatile_v
//     4) MemberFunctionRefQualifier_v
//     6) MemberFunctionRefQualifierName_v
///////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsMemberFunction_v = IsFunctionTraitsMemberFunction_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

//////////////////////////////////////////////////////////////////////////
// IsMemberFunctionConst_v. Helper template for "FunctionTraits::IsConst"
// which stores "true" if "F" is a non-static member function with a
// "const" cv-qualifier or false otherwise. This helper template is less
// verbose however than creating a "FunctionTraits" directly from "F"
// and accessing its "IsConst" member. The following provides a
// convenient wrappper. Note that this member is always false for free
// functions and static member functions (not applicable to them). You
// may therefore want to call "IsMemberFunction_v" to determine if "F"
// is a non-static member function before invoking the following (if
// required).
//////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsMemberFunctionConst_v = IsFunctionTraitsMemberFunctionConst_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

/////////////////////////////////////////////////////////////////////////////////
// IsMemberFunctionVolatile_v. Helper template for "FunctionTraits::IsVolatile"
// which stores "true" if "F" is a non-static member function with a "volatile"
// cv-qualifier or false otherwise. This helper template is less verbose however
// than creating a "FunctionTraits" directly from "F" and accessing its
// "IsVolatile" member. The following provides a convenient wrappper. Note that
// this member is always false for free functions and static member functions
// (not applicable to them). You may therefore want to call
// "IsMemberFunction_v" to determine if "F" is a non-static member function
// before invoking the following (if required).
/////////////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsMemberFunctionVolatile_v = IsFunctionTraitsMemberFunctionVolatile_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

/////////////////////////////////////////////////////////////////////////
// IsNoexcept_v. Helper template for "FunctionTraits::IsNoexcept" which
// stores "true" if "F" is declared as "noexcept" or false otherwise.
// This helper template is less verbose however than creating a
// "FunctionTraits" directly from "F" and accessing its "IsNoexcept"
// member. The following provides a convenient wrappper.
/////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsNoexcept_v = IsFunctionTraitsNoexcept_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

/////////////////////////////////////////////////////////////////////////
// IsVariadic_v. Helper template for "FunctionTraits::IsVariadic" which
// stores "true" if "F" is a variadic function (last arg is "...") or
// false otherwise. This helper template is less verbose however than
// creating a "FunctionTraits" directly from "F" and accessing its
// "IsVariadic" member. The following provides a convenient wrappper.
/////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsVariadic_v = IsFunctionTraitsVariadic_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

///////////////////////////////////////////////////////////////////////////
// MemberFunctionClass_t. Helper alias for "FunctionTraits::Class" which
// yields the class for a non-static member "F" but less verbose than
// creating a "FunctionTraits" directly from "F" and accessing its "Class"
// alias. The following provides a convenient wrappper. Note that this
// helper applies to non-static member functions only (yielding the class
// type of non-static member functions). For free functions including
// static member functions this type is always "void" (not applicable to
// them noting that the class for static member functions can't be retrieved
// in this release due to language limitations). Note that you might want
// to call "IsMemberFunction_v" to determine if you're dealing with a
// non-static member function before invoking the following (if required).
///////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
using MemberFunctionClass_t = FunctionTraitsMemberFunctionClass_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

/////////////////////////////////////////////////////////////////////////
// MemberFunctionClassName_v. See "MemberFunctionClass_t()" just above.
// Simply converts that to a string suitable for display purposes and
// returns it as a "tstring_view"
/////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view MemberFunctionClassName_v = FunctionTraitsMemberFunctionClassName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

//////////////////////////////////////////////////////////////////////////
// MemberFunctionRefQualifier_v. Helper template for
// "FunctionTraits::RefQualifier" which stores the reference qualifier
// for "F" if it's a non-static member function with a "&" or "&&"
// reference-qualifier or "RefQualifier::None" otherwise. This helper
// template is less verbose however than creating a "FunctionTraits"
// directly from "F" and accessing its "RefQualifier" member. The
// following provides a convenient wrappper. Note that this member is
// always "RefQualifier::None" for free functions and static member
// functions (not applicable to them). You may therefore want to call
// "IsMemberFunction_v" to determine if "F" is a non-static member
// function before invoking the following (if required).
//////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr RefQualifier MemberFunctionRefQualifier_v = FunctionTraitsMemberFunctionRefQualifier_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

/////////////////////////////////////////////////////////////////////////
// MemberFunctionRefQualifierName_v. See "MemberFunctionRefQualifier_v"
// just above. Simply converts that to a string suitable for display
// purposes and returns it as a "tstring_view". Pass "true" (the
// default) for the "UseAmpersands" template arg arg if you wish to
// return this as "&" or "&&" or "false" to return it as "LValue" or
// "RValue" instead. In eiter case however "None" is always returned if
// "F" has no reference-qualifier which is usually the case for most
// functions in practice.
/////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F, bool UseAmpersands = true>
inline constexpr tstring_view MemberFunctionRefQualifierName_v = FunctionTraitsMemberFunctionRefQualifierName_v<FunctionTraits<F>, UseAmpersands>;  // Defers to the "FunctionTraits" helper further above

///////////////////////////////////////////////////////////////////////////
// ReturnType_t. Helper alias for "FunctionTraits::ReturnType" which
// yields the return type of function "F" but less verbose than creating
// a "FunctionTraits" directly from "F" and accessing its "ReturnType"
// alias. The following provides a convenient wrappper.
// 
// E.g.,
// 
//       void MyFunction(const std::string &str, int val)
//       {
//       }
// 
//       // Return type for "MyFunction" above (void)
//       using MyFunctionReturnType = ReturnType_t<decltype(MyFunction)>;
//       // using MyFunctionReturnType = ReturnType_t<void (const std::string &, int)>; // This will also work (same as above but passing function's type on-the-fly)
//       // using MyFunctionReturnType = ReturnType_t<void (*)(const std::string &, int)>; // ... and this too (but now using a pointer to the function - references
//                                                                                         // to the function will also work, and references to pointers to the
//                                                                                         // function as well)
///////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
using ReturnType_t = FunctionTraitsReturnType_t<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

//////////////////////////////////////////////////////////////////////////
// ReturnTypeName(). See "ReturnType_t()" just above. Simply converts
// that to a string suitable for display purposes and returns it as a
// "tstring_view" (so a return type like, say, "int" will literally be
// returned as the string "int", quotes not included)
//////////////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view ReturnTypeName_v = FunctionTraitsReturnTypeName_v<FunctionTraits<F>>; // Defers to the "FunctionTraits" helper further above

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
// behaviour of what qualifies as an lvalue and rvalue for purposes of
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
template <typename T, typename = void>
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
                        std::enable_if_t<std::is_same_v<decltype(std::declval<T>().template operator()<std::size_t(0)>()), bool>>
                       > : std::true_type
{
};

/////////////////////////////////////////////////////////
// Helper variable template for "IsForEachFunctor" just
// above (with the usual "_v" suffix).
/////////////////////////////////////////////////////////
template <typename T>
inline constexpr bool IsForEachFunctor_v = IsForEachFunctor<T>::value;

#if CPP20_OR_LATER
    template <typename T>
    concept ForEachFunctor_c = IsForEachFunctor_v<T>;

    // See "#else" comments just below
    #define FOR_EACH_FUNCTOR_C ForEachFunctor_c
#else
    //////////////////////////////////////////////////////////////////////
    // Useful macro that can be used in any template where you want to
    // check a template arg to ensure it's compatible with function
    // template "ForEach()". The following will "static_assert" if it's
    // not compatible. See primary template "IsForEachFunctor" above for
    // details (what we're asserting on just below). Note that this macro
    // is #defined in C++17 only. In C++20 or later you can rely on
    // concepts instead so this macro should only be applied if
    // CPP17_OR_EARLIER is #defined. When not #defined then the #defined
    // constant FOR_EACH_FUNCTOR_C above can be used instead (to apply
    // the "ForEachFunctor_c" concept). All code should therefore be
    // written similar to the following example (which correctly targets
    // whichever version of C++ is in effect, applying the following
    // macro if its C++17 or earlier, or the concept if C++20 or later):               
    // 
    //      template <FOR_EACH_FUNCTOR_C F>
    //      void SomeFunction()
    //      {
    //          #if CPP17_OR_EARLIER
    //              ///////////////////////////////////////////////
    //              // Kicks in if C++ 17 or earlier, otherwise
    //              // FOR_EACH_FUNCTOR_C concept kicks in just
    //              // above instead (latter simply resolves to
    //              // the "typename" keyword in C++17 or earlier)
    //              ///////////////////////////////////////////////
    //              STATIC_ASSERT_IS_FOR_EACH_FUNCTOR(F);
    //          #endif
    //          
    //          // ...
    //      }
    /////////////////////////////////////////////////////////////////
    #define STATIC_ASSERT_IS_FOR_EACH_FUNCTOR(T) static_assert(IsForEachFunctor_v<T>);

    // See comment block above
    #define FOR_EACH_FUNCTOR_C typename
#endif

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
            STATIC_ASSERT_IS_FOR_EACH_FUNCTOR(ForEachFunctorT);

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
    }
#endif // #if CPP17_OR_EARLIER

///////////////////////////////////////////////////////////////////
// ForEach(). Generic function template effectively equivalent to
// a "for" loop but used at compile time to execute the given
// "functor" "N" times (unless the "functor" arg returns false on
// any of those iterations in which case processing immediately
// stops and "false" is immediately returned - read on).    
//
// Note that "operator()" in your functor must be declared as follows
// (and will typically be "const" as seen though it can be non-const
// as well if required, and/or noexcept as well)
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
//                                         // insteaad as we're doing here means all
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
//     // syntactic sugar for a compiler-generated functor). The upshot
//     // is that "operator()" in this functor (the above lambda) is
//     // effectively stamped out 3 times and invoked for each (for I=0,
//     // I=1 and I=2)
//     ///////////////////////////////////////////////////////////////////
//     ForEach<std::tuple_size_v<TupleT>>(displayTupleElement);
///////////////////////////////////////////////////////////////////
template <std::size_t N, FOR_EACH_FUNCTOR_C ForEachFunctorT>
inline constexpr bool ForEach(ForEachFunctorT&& functor)
{
    // Lambda templates not supported until C++20
    #if CPP20_OR_LATER
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
        STATIC_ASSERT_IS_FOR_EACH_FUNCTOR(ForEachFunctorT);

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
        // "Private::ForEachImpl "means that the latter's
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

////////////////////////////////////////////////////////////////////////////
// "IsForEachTupleFunctor" (primary template). Determines if template arg
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
// purposes of "IsForEachTupleFunctor", "T" must be an lvalue reference
// such as "int &". To qualify as an rvalue, "T" must either be a
// non-reference type such as a plain "int" OR an rvalue reference such as
// "int &&". The behaviour of what qualifies as an lvalue and rvalue for
// purposes of using "IsForEachTupleFunctor" therefore follows the usual
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
// "IsForEachTupleFunctor" therefore inherits from "std::true_type"
// (meaning "T" is a suitable functor type for passing as the 2nd template
// arg of function template "ForEachTupleType()").
////////////////////////////////////////////////////////////////////////////
template <typename T, typename = void>
struct IsForEachTupleFunctor : std::false_type
{
};

/////////////////////////////////////////////////////////////////
// Specialization of "IsForEachTupleFunctor" (primary) template
// just above. This specialization does all the work. See
// primary template above for details. Note that calls to
// "declval(T)" return "T &&" (unless "T" is void - see
// "declval()" docs) so the following call to this works
// according to the usual perfect forwarding rules in C++. That
// is, if "T" is an lvalue reference then "declval()" returns an
// lvalue reference (due to the C++ reference collapsing rules),
// otherwise "T" must be an a non-reference or an rvalue
// reference so "declval" returns an rvalue reference (in either
// case). In all cases "declval()" therefore returns the same
// function argument type used in a perfect forwarding function.
// Such arguments are always of the type "T &&" which resolves
// to an lvalue reference only if "T" is an lvalue reference
// (again, due to the C++ reference collapsing rules), or an
// rvalue reference if "T" is a non-reference or rvalue
// reference. "declval()" therefore returns the exact same type
// as a perfectly forwarded argument after plugging in "T" which
// is what we require here (we're invoking "operator()" on that
// argument in the following call in an unevaluated context
// simply to make sure it can be invoked - the primary template
// kicks in via SFINAE otherwise).
/////////////////////////////////////////////////////////////////
template <typename T>
struct IsForEachTupleFunctor<T,
                             // See explanation of "declval()" in comments above
                             std::enable_if_t<std::is_same_v<decltype(std::declval<T>().template operator()<std::size_t(0), void>()), bool>>
                            > : std::true_type
{
};

/////////////////////////////////////////////////////////
// Helper variable template for "IsForEachTupleFunctor"
// just above (with the usual "_v" suffix).
/////////////////////////////////////////////////////////
template <typename T>
inline constexpr bool IsForEachTupleFunctor_v = IsForEachTupleFunctor<T>::value;

#if CPP20_OR_LATER
    template <typename T>
    concept ForEachTupleFunctor_c = IsForEachTupleFunctor_v<T>;

    // See "#else" comments just below
    #define FOR_EACH_TUPLE_FUNCTOR_C ForEachTupleFunctor_c
#else
    //////////////////////////////////////////////////////////////////
    // Useful macro that can be used in any template where you want
    // to check a template arg to ensure it's compatible with
    // function template "ForEachTupleType()". The following will
    // "static_assert" if it's not compatible. See primary template
    // "IsForEachTupleFunctor" above for details (what we're
    // asserting on just below). Note that this macro is #defined in
    // C++17 only. In C++20 or later you can rely on concepts
    // instead so this macro should only be applied if
    // CPP17_OR_EARLIER is #defined. When not #defined then the
    // #defined constant FOR_EACH_TUPLE_FUNCTOR_C above can be used
    // instead (to apply the "ForEachTupleFunctor_c" concept). All
    // code should therefore be written similar to the following
    // example (which correctly targets whichever version of C++ is
    // in effect, applying the following macro if it's C++17 or
    // earlier, or the concept if C++20 or later):        
    // 
    //      template <FOR_EACH_TUPLE_FUNCTOR_C F>
    //      void SomeFunction()
    //      {
    //          #if CPP17_OR_EARLIER
    //              /////////////////////////////////////////////
    //              // Kicks in if C++ 17 or earlier, otherwise
    //              // FOR_EACH_TUPLE_FUNCTOR_C concept kicks
    //              // in above instead (latter simply resolves
    //              // to the "typename" keyword in C++17 or
    //              // earlier)
    //              /////////////////////////////////////////////
    //              STATIC_ASSERT_IS_FOR_EACH_TUPLE_FUNCTOR(F);
    //          #endif
    //          
    //          // ...
    //      }
    //////////////////////////////////////////////////////////////////
    #define STATIC_ASSERT_IS_FOR_EACH_TUPLE_FUNCTOR(T) static_assert(IsForEachTupleFunctor_v<T>);

    // See comment block above
    #define FOR_EACH_TUPLE_FUNCTOR_C typename
#endif

///////////////////////////////////////////////////////////////////////////
// For C++17 only we'll rely on the "ProcessTupleType" functor declared
// just below (used exclusively by function template "ForEachTupleType()"
// declared just after). For C++20 and later we'll rely on a lambda
// template instead (the equivalent of the struct just below but locally
// declared in function template "ForEachTupleType()" so it's cleaner).
// Note that lambda templates aren't supported until C++20. Lastly, note
// that we're currently inside a "#if CPP17_OR_LATER" preprocessor block
// (we're processing code for C++17 or later) so if the #defined constant
// CPP17_OR_EARLIER we're testing just below is false (0), then we're
// guaranteed to be processing C++20 or later (again, because we know that
// the #defined constant CPP17_OR_LATER is currently true).
// /////////////////////////////////////////////////////////////////////////
#if CPP17_OR_EARLIER
    ///////////////////////////////////////////////////
    // For internal use only (by "ForEachTupleType()"
    // just after this namespace)
    ///////////////////////////////////////////////////
    namespace Private
    {   
        /////////////////////////////////////////////////////////////
        // class ProcessTupleType. Private implementation class
        // used by function template "ForEachTupleType()" declared
        // just after this private namespace. See this for
        // details.  Note that in the following class,
        // "ForEachTupleFunctorT" is always the same template type
        // passed to "ForEachTupleType()", so either "T &" for the
        // lvalue case or just plain "T" or "T &&" for the rvalue
        // case (for the "rvalue" case however usually just plain
        // "T" as per the usual perfect forwarding rules when
        // invoking such functions via implicit type deduction).
        ///////////////////////////////////////////////////////////
        template <typename TupleT, typename ForEachTupleFunctorT>
        class ProcessTupleType
        {
            STATIC_ASSERT_IS_TUPLE(TupleT);
            STATIC_ASSERT_IS_FOR_EACH_TUPLE_FUNCTOR(ForEachTupleFunctorT);

        public:
            /////////////////////////////////////////////////////////
            // Constructor. Note that we always pass "functor" by a
            // "std::forward" reference so "functor" resolves to "&"
            // or "&&" accordingly
            /////////////////////////////////////////////////////////
            constexpr ProcessTupleType(ForEachTupleFunctorT &&functor) noexcept
                ///////////////////////////////////////////////////////////
                // "std::forward" required when "ForEachTupleFunctorT" is
                // "&&". Both "functor" and "m_Functor" are therefore "&&"
                // but "functor" is still an lvalue because it's named
                // (rvalue references that are named are lvalues!).
                // Therefore illegal to bind an lvalue (functor) to an
                // rvalue reference (m_Functor) so "std::forward" is
                // required to circumvent this (for the "&&" case it
                // returns an "&&" reference but it's now unnamed so an
                // rvalue and therefore suitable for initializing
                // "m_Functor")
                ///////////////////////////////////////////////////////////
                : m_Functor(std::forward<ForEachTupleFunctorT>(functor))
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
                return std::forward<ForEachTupleFunctorT>(m_Functor).template operator()<I, TupleElement_t>();
            }

        private:
            /////////////////////////////////////////////////////
            // Note: Resolves to "&" in the lvalue case or "&&"
            // otherwise (by the usual C++ reference collapsing
            // rules - "ForEachTupleFunctorT" is always "T &"
            // in the former case or either plain "T" or "T &&"
            // otherwise)
            /////////////////////////////////////////////////////
            ForEachTupleFunctorT &&m_Functor;
        };
    }
#endif

/////////////////////////////////////////////////////////////////////////
// ForEachTupleType(). Iterates all tuple elements in "TupleT" where
// "TupleT" is a "std::tuple" specialization, and invokes the given
// "functor" for each. The effect of this function is that all types in
// "TupleT" are iterated or none if "TupleT" is empty. For each type in
// "TupleT", the given "functor" is invoked as noted. "functor" must be
// a functor with the following functor (template) signature (where
// items in square brackets are optional - note that an invalid functor
// will trigger the FOR_EACH_TUPLE_FUNCTOR_C concept in C++20 or later,
// or a "static_assert" in C++17, since we don't support earlier
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
template <TUPLE_C TupleT, FOR_EACH_TUPLE_FUNCTOR_C ForEachTupleFunctorT>
inline constexpr bool ForEachTupleType(ForEachTupleFunctorT&& functor)
{
    #if CPP17_OR_EARLIER
        /////////////////////////////////////////////
        // Kicks in if C++ 17 or earlier, otherwise
        // TUPLE_C concept kicks in above instead
        // (latter simply resolves to the "typename"
        // keyword in C++17 or earlier)
        /////////////////////////////////////////////
        STATIC_ASSERT_IS_TUPLE(TupleT)

        ///////////////////////////////////////////////
        // Kicks in if C++ 17 or earlier, otherwise
        // FOR_EACH_TUPLE_FUNCTOR_C concept kicks in
        // above instead (latter simply resolves to
        // the "typename" keyword in C++17 or earlier)
        ///////////////////////////////////////////////
        STATIC_ASSERT_IS_FOR_EACH_TUPLE_FUNCTOR(ForEachTupleFunctorT)
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
    // passing to "ForEach" below will be specialized on zero, (its
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
    // noted (so there will beno iterations), "IsForEachFunctor_v"
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
    // described). It's likely (presumably) not being stampled out
    // in this case. Testing shows however that if we allowed the
    // same code to be compiled in C++20, instead of the lambda
    // below (that we actually rely on in C++20 or later), it will
    // also fail. IOW, this whole situation surrounding
    // "std::tuple_element_t<I, TupleT>" only starts surfacing in
    // C++20 so something clearly has changed between C++17 and
    // C++20 (since what compiled without error in C++17 now fails
    // in C++20). This should be investigated but it's a moot point
    // for now since the following "if constexpr" statemement
    // prevents the issue altogether
    /////////////////////////////////////////////////////////////////
    if constexpr (std::tuple_size_v<TupleT> != 0)
    {
        // Lambda templates not supported until C++20
        #if CPP20_OR_LATER
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
                                              return std::forward<ForEachTupleFunctorT>(functor).template operator()<I, TupleElement_t>();
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
            // Note: The "ForEachTupleFunctorT" template arg we're
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
            const Private::ProcessTupleType<TupleT, ForEachTupleFunctorT> processTupleType(std::forward<ForEachTupleFunctorT>(functor));
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

/////////////////////////////////////////////////////////////////////////
// ForEachFunctionTraitsArg(). Iterates all tuple elements in member
// "FunctionTraitsT::ArgTypes", where "FunctionTraitsT" is a
// "FunctionTraits" specialization, and invokes the given "functor" for
// each. Also see helper function "ForEachArg()" just below which just
// defers to the function you're now reading (and easier to use so you
// should normally use it instead - the function you're now reading
// should normally be used only if you already have an existing
// "FunctionTraits" you're working with, otherwise "ForEachArg()"
// creates a "FunctionTraits" for you so it's less verbose than calling
// the following function directly).
//
// The effect of this function is that all (non-variadic) args in the
// function that the given "FunctionTraits" was specialized on are
// iterated or none if that function has no args. For each argument
// type, the given "functor" is invoked as noted. "functor" must be a
// functor with the following functor (template) signature (where items
// in square brackets are optional - note that an invalid functor will
// trigger the FOR_EACH_TUPLE_FUNCTOR_C concept in C++20 or later, or a
// "static_assert" in C++17, since we don't support earlier versions -
// compile fails either way):
//
//     template <std::size_t I, typename ArgTypeT>
//     bool operator()() [const] [volatile] [ref-qualifier] [noexcept];
//
// Note that lambdas with this signature are also (therefore) supported
// in C++20 or later (since lambda templates aren't supported in earlier
// versions of C++ so you'll have to roll your own functor template if
// targeting C++17). Note that free functions or other non-static member
// functions besides "operator()" are not currently supported (but may
// be in a future release):
//
//
//     Example
//     -------
//     ////////////////////////////////////////////////////
//     // Prototype of some function whose arg types you
//     // wish to iterate (display in this example - see
//     // "displayArgType" lambda below). Note that we're
//     // using a raw C++ function type in this example
//     // (just a free-function prototype), but pointers
//     // and references to functions are also supported
//     // (though references to non-static member functions
//     // are illegal in C++), as well as references to
//     // pointers, and functors.
//     ////////////////////////////////////////////////////
//     int SomeFunc(int arg1, float arg2, double arg3);
//
//     ///////////////////////////////////////////////////////////////
//     // "FunctionTraits" specialization for the above function.
//     // Note that you can pass any function type for its template
//     // arg (free functions, member functions, pointers to
//     // functions, references to functions, references to pointers
//     // to functions and functors). The resulting "FunctionTraits"
//     // specialization therefore captures all (most) info about
//     // whatever function you pass (its return type, argument types
//     // and other info). See "FunctionTraits" for details (we'll
//     // be iterating its "ArgTypes" member below).
//     ///////////////////////////////////////////////////////////////
//     using SomeFuncTraits = FunctionTraits<decltype(SomeFunc)>;
//
//     /////////////////////////////////////////////////////////////////
//     // Lambda that will be invoked just below, once for each arg in
//     // "SomeFunc()" above (where template arg "I" is the zero-based
//     // "Ith" argument of "SomeFunc()", and "ArgTypeT" is its type).
//     // Note that lambda templates are supported in C++20 and later
//     // only. For C++17 (this header doesn't support earlier versions),
//     // you need to roll your own functor instead (with a template
//     // version of "operator()" equivalent to this lambda)
//     /////////////////////////////////////////////////////////////////
//     const auto displayArgType = []<std::size_t I, typename ArgTypeT>()
//                                 {
//                                     /////////////////////////////////////////////////////////
//                                     // Display the type of the (zero-based) "Ith" arg in
//                                     // "SomeFunc" using the following format (shown here for
//                                     // "I" equals zero so "ArgTypeT" is therefore an "int",
//                                     // but whatever "I" and "ArgTypeT" are on each iteration
//                                     // - note that we increment zero-based "I" by 1 however
//                                     // to display it as 1-based since it's more natural for
//                                     // display purposes):
//                                     //
//                                     //    1) int
//                                     //
//                                     // Note that we never come through here if the function
//                                     // represented by "FunctionTraitsT" has no "args (or
//                                     // variadic args only - variadic args aren't processed)
//                                     /////////////////////////////////////////////////////////
//                                     tcout << (I + 1) << _T(") ") << TypeName_v<ArgTypeT> << _T("\n");
//
//                                     //////////////////////////////////////////////
//                                     // Return true to continue iterating (false
//                                     // would stop iterating, equivalent to a
//                                     // "break" statement in a regular "for" loop)
//                                     //////////////////////////////////////////////
//                                     return true;
//                                 };
//
//     /////////////////////////////////////////////////////////////
//     // Iterate all tuple elements in "SomeFuncTraits::ArgTypes"
//     // meaning once for each argument in "SomeFunc()" above.
//     // Invokes "displayArgType" above for each. Outputs the
//     // following:
//     //
//     //     1) int
//     //     2) float
//     //     3) double
//     //
//     // Note that "ForEachFunctionTraitsArg()" returns true if
//     // the functor you pass ("displayArgType" in this example)
//     // returns true or false otherwise (useful if your functor
//     // needs to "break" like a normal "for" loop for any
//     // reason - iteration immediately stops if it returns false
//     // and the following therefore returns false). Always
//     // returns true in the above example (and typically in most
//     // real-world code as well)
//     /////////////////////////////////////////////////////////////
//     ForEachFunctionTraitsArg<SomeFuncTraits>(displayArgType);
/////////////////////////////////////////////////////////////////////////
template <FUNCTION_TRAITS_C FunctionTraitsT, FOR_EACH_TUPLE_FUNCTOR_C ForEachTupleFunctorT>
inline constexpr bool ForEachFunctionTraitsArg(ForEachTupleFunctorT&& functor)
{
    #if CPP17_OR_EARLIER
        ///////////////////////////////////////////////
        // Kicks in if C++ 17 or earlier, otherwise
        // FUNCTION_TRAITS_C concept kicks in above
        // instead (latter simply resolves to the
        // "typename" keyword in C++17 or earlier)
        ///////////////////////////////////////////////
        STATIC_ASSERT_IS_FUNCTION_TRAITS(FunctionTraitsT);

        ///////////////////////////////////////////////
        // Kicks in if C++ 17 or earlier, otherwise
        // FOR_EACH_TUPLE_FUNCTOR_C concept kicks in
        // above instead (latter simply resolves to
        // the "typename" keyword in C++17 or earlier)
        ///////////////////////////////////////////////
        STATIC_ASSERT_IS_FOR_EACH_TUPLE_FUNCTOR(ForEachTupleFunctorT)
    #endif

    /////////////////////////////////////////////////////////
    // Defer to function template "ForEachTupleType()" just
    // above, specializing it on the tuple type member of
    // "FunctionTraitsT" (i.e., the "std::tuple" containing
    // all non-variadic arg types for the function
    // represented by "FunctionTraitsT"). All tuple types
    // (i.e., all arg types in the "FunctionTraitsT"
    // function) are therefore iterated, invoking "functor"
    // for each (we pass the latter).
    /////////////////////////////////////////////////////////
    return ForEachTupleType<FunctionTraitsArgTypes_t<FunctionTraitsT>>(std::forward<ForEachTupleFunctorT>(functor));
}

///////////////////////////////////////////////////////////////////
// ForEachArg(). Thin wrapper around "ForEachFunctionTraitsArg()"
// just above. Simply creates a "FunctionTraits" from the function
// given by template arg "F" and then invokes the latter function.
// Therefore iterates all tuple elements corresponding to each
// (non-variadic) arg in function "F" (or none if "F" has no
// non-variadic args) and invokes "functor" for each. See above
// function for further details.
// 
// Note that you should normally rely on the following function
// instead of "ForEachFunctionTraitsArg()" above since it's easier
// (though you're free to use the latter function if you've already
// created a "FunctionTraits" for other purposes). The following
// provides the same example seen in "ForEachFunctionTraitsArg()"
// to demonstrate this.
//
//     Example
//     -------
//     ///////////////////////////////////////////////////
//     // Prototype of some function whose arg types you
//     // wish to iterate (display in this example - see
//     // "displayArgType" lambda below). Note that we're
//     // using a raw C++ function type in this example
//     // (just a free-function prototype), but pointers
//     // and references to functions are also supported
//     // (though references to non-static member functions
//     // are illegal in C++), as well as references to
//     // pointers, and functors.
//     ///////////////////////////////////////////////////
//     int SomeFunc(int arg1, float arg2, double arg3);
//
//     /////////////////////////////////////////////////////////////////
//     // Lambda that will be invoked just below, once for each arg in
//     // "SomeFunc()" above (where template arg "I" is the zero-based
//     // "Ith" argument of "SomeFunc()", and "ArgTypeT" is its type).
//     // Note that lambda templates are supported in C++20 and later
//     // only. For C++17 (this header doesn't support earlier versions),
//     // you need to roll your own functor instead (with a template
//     // version of "operator()" equivalent to this lambda)
//     /////////////////////////////////////////////////////////////////
//     const auto displayArgType = []<std::size_t I, typename ArgTypeT>()
//                                 {
//                                     //////////////////////////////////////////////////////
//                                     // Display the type of the (zero-based) "Ith" arg in
//                                     // "SomeFunc" using the following format (shown here
//                                     // for "I" equals zero so "ArgTypeT" is therefore an
//                                     // "int", but whatever "I" and "ArgTypeT" are on each
//                                     // iteration - note that we increment zero-based "I"
//                                     // by 1 however to display it as 1-based since it's
//                                     // more natural for display purposes):
//                                     //
//                                     //    1) int
//                                     //
//                                     // Note that we never come through here if function
//                                     // "F" has no args (or variadic args only - variadic
//                                     // args aren't processed)
//                                     //////////////////////////////////////////////////////
//                                     tcout << (I + 1) << _T(") ") << TypeName_v<ArgTypeT> << _T("\n");
//
//                                     //////////////////////////////////////////////
//                                     // Return true to continue iterating (false
//                                     // would stop iterating, equivalent to a
//                                     // "break" statement in a regular "for" loop)
//                                     //////////////////////////////////////////////
//                                     return true;
//                                 };
//
//     ///////////////////////////////////////////////////////////////
//     // Iterate all argument types in "SomeFunc", invoking
//     // "displayArgType" above for each. Outputs the following:
//     //
//     //     1) int
//     //     2) float
//     //     3) double
//     //
//     // Note that "ForEachArg" returns true if the functor you pass
//     // ("displayArgType" in this example) returns true or false
//     // otherwise (useful if your functor needs to "break" like a
//     // normal "for" loop for some reason - iteration immediately
//     // stops if it returns false and the following therefore
//     // returns false). Always returns true in the above example
//     // (and typically in most real-world code as well)
//     ///////////////////////////////////////////////////////////////
//     using F = decltype(SomeFunc);
//     ForEachArg<F>(displayArgType);
///////////////////////////////////////////////////////////////////
template <TRAITS_FUNCTION_C F, FOR_EACH_TUPLE_FUNCTOR_C ForEachTupleFunctorT>
inline constexpr bool ForEachArg(ForEachTupleFunctorT&& functor)
{
    #if CPP17_OR_EARLIER
        /////////////////////////////////////////////
        // Kicks in if C++ 17 or earlier, otherwise
        // TRAITS_FUNCTION_C concept kicks in above
        // instead (latter simply resolves to the
        // "typename" keyword in C++17 or earlier)
        /////////////////////////////////////////////
        STATIC_ASSERT_IS_TRAITS_FUNCTION(F);

        ///////////////////////////////////////////////
        // Kicks in if C++ 17 or earlier, otherwise
        // FOR_EACH_TUPLE_FUNCTOR_C concept kicks in
        // above instead (latter simply resolves to
        // the "typename" keyword in C++17 or earlier)
        ///////////////////////////////////////////////
        STATIC_ASSERT_IS_FOR_EACH_TUPLE_FUNCTOR(ForEachTupleFunctorT)
    #endif

    ///////////////////////////////////////////////////////////
    // Create (specialize) a "FunctionTraits" from "F" and
    // defer to function template "ForEachFunctionTraitsArg()"
    // just above (the "FunctionTraits" version of this
    // function). All non-variadic arg types for the function
    // represented by "FunctionTraitsT" are therefore iterated,
    // invoking "functor" for each (we pass the latter).
    ///////////////////////////////////////////////////////////
    return ForEachFunctionTraitsArg<FunctionTraits<F>>(std::forward<ForEachTupleFunctorT>(functor));
}

} // namespace StdExt

#endif // CPP17_OR_LATER
#endif // TYPETRAITS