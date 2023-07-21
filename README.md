# FunctionTraits
## C++ function traits library (single header-only) for retrieving info about any function (arg types, arg count, return type, etc.). Clean and easy-to-use, the most "complete" implementation on the web.

See [here](https://godbolt.org/z/fx8MWGv99) for a complete working example (demo for those who want to get right to the code - the same code in this repository).

"FunctionTraits" is a lightweight C++ traits struct (template) that allows you to quickly and easily determine the traits of any function at compile-time, such as argument types, number of arguments, return type, etc. (for C++17 and later). It's a "complete" implementation in the sense that it handles (detects) all mainstream function syntax unlike almost any other implementation you'll often find, most of which fail to address one or more issues (including the Boost implementation - read on). Many (actually most) of these implementations are just hacks or solutions quickly developed on-the-fly to answer someone's question on "stackoverflow.com" for instance. Only a small handful I'm aware of are more complete and far fewer are reasonably complete (though still missing at least one or two features, in particular calling convention support - again, read on). Some have even written articles on the subject but still don't address various issues. Some of these issues are obvious (or should be), such as failing to detect functions declared as "noexcept" (part of a function's type since C++17), or variadic functions, or volatile non-static member functions, or function pointers with cv-qualifiers, among other things (often resulting in a lot of cryptic compiler errors). Some are less obvious, like failing to detect functions with calling conventions other than the default (usually "cdecl"), which isn't addressed in the C++ standard itself (and a bit more tricky to implement than it first appears as far as creating a functions traits class is concerned). With the exception of the one from Boost ("boost::callable_traits"), whose own support for calling conventions is limited and even discouraged by the author's internal comments (and not on by default or even officially documented - more on this later), I have yet to see any other implementation address calling conventions at all, even in the (very) few other (mostly) "complete" solutions for handling function traits that are out there (that I've ever come across). Without (mainstream) calling convention support however, all functions in the entire Microsoft Windows API would fail to be detected for instance, since it's almost entirely based on the "stdcall" calling convention. "FunctionTraits" does address all these issues however. It provides traits info about every (mainstream) aspect of any function within the limitations of what C++ currently supports (or easily supports), and allows you to change every trait as well, where applicable (including function parameter types, which will be rarely used by most but no other implementation I've seen at this writing supports it, including the one from Boost). You can currently obtain and/or change (where applicable) the following information:

1. `Return type` - Function's return type and/or its name as a WYSIWYG string (std::basic_string_view)
2. `Argument count` - Number of (non-variadic) arguments in the function (formally called "arity")
3. `Arguments` - The type of any (non-variadic) function argument (based on its zero-based index in the parameter list), and/or its name as a WYSIWYG string (std::basic_string_view). You can also obtain the "std::tuple" containing all arguments and even iterate them using the [ForEachArg](#ForEachArg) function template (though rarely required by most in practice - targeting specific function arguments by their index is far more common). See [Looping through all function arguments](#LoopingThroughAllFunctionArguments) for details.
4. `variadic` - Whether a function has variadic args (last arg is "...")
5. `cv-qualifiers` - Whether a non-static member function is declared with the "const" and/or "volatile" cv-qualifiers
6. `ref-qualifiers` - Whether a non-static member function is declared with the "&" or "&&" ref-qualifier
7. `noexcept` - Whether a function is declared with "noexcept"
8. `Calling convention` - The function's calling convention ("cdecl", "stdcall", etc.). As previously mentioned, all other implementations I've seen can detect functions with the default calling convention only (usually "cdecl" or possibly "thiscall" for non-static member functions depending on the platform). Only "callable_traits" from Boost supports calling conventions as previously noted, but again, its support is limited, unofficial, not on by default, and its use is discouraged by the author's internal comments (see [Why choose this library](#WhyChooseThisLibrary)). Note that calling conventions aren't addressed in the C++ standard itself and are platform specific. As a result, there are numerous calling conventions in the wild that are specific to one platform only and it's therefore inherently difficult for any function traits library to support them all. For now "FunctionTraits" currently supports the most mainstream calling conventions which will usually meet the needs of most users ("cdecl", "stdcall", "fastcall", "vectorcall", though GCC itself doesn't support "vectorcall", "thiscall", and on Intel platforms only, "regcall").
9. `class/struct type` - The class/struct associated with a non-static member function (including functors), and/or its name as a WYSIWYG string (std::basic_string_view)
10. `Function's classification` - Whether the function is a free function (which for our purposes includes static member functions), a non-static member function, or a functor

The above covers almost all traits most programmers will ever be interested in. Other possible traits such as whether a function is a static member function, whether a non-static member function is virtual (and/or the "override" keyword is in effect), etc., are not available in this release, normally due to limitations in the language itself (either impossible or difficult/unwieldy to implement in current releases of C++). They may be available in a future version however if C++ better supports it.

## Usage (GCC or compatible, Clang, Microsoft and Intel compilers only - C++17 and later)
To use "FunctionTraits", simply add both "TypeTraits.h" and "CompilerVersions.h" to your code and then #include "TypeTraits.h" wherever you require it (all code is declared in namespace "StdExt"). Note that you need not explicitly #include "CompilerVersions.h" unless you wish to use it independently of "TypeTraits.h", since "TypeTraits.h" itself #includes it as a dependency ("CompilerVersions.h" simply declares various #defined constants used to identify the version of C++ you're using, and a few other compiler-related declarations - you're free to use these in your own code as well if you wish). The struct (template) "FunctionTraits" is then immediately available for use (see [Technique 1 of 2](#Technique1Of2) below), though you'll normally rely on its [Helper templates](#HelperTemplates) instead (see [Technique 2 of 2](#Technique2Of2) below). Note that both files above have no platform-specific dependencies, except when targeting Microsoft, where the native Microsoft header <tchar.h> is expected to be in the usual #include search path (and it normally will be on Microsoft platforms). Otherwise they rely on the C++ standard headers only which are therefore (also) expected to be in the usual search path on your platform.

<a name="SupportedTypesForF"></a>Note that template arg "F" is the first (and usually only) template arg of "FunctionTraits" and all its [Helper templates](#HelperTemplates), and refers to the function's type which can be any of the following:

1. Free functions which also includes static member functions, in either case referring to the function's actual C++ type (which are always considered to be free functions - non-static member functions are always handled by 4 or 5 below).
2. Pointers and references to free functions
3. References to pointers to free functions
4. Pointers to non-static member functions
5. References to pointers to non-static member functions (note that C++ doesn't support references to non-static member functions, only pointers)
6. Non-overloaded functors (i.e., functors or references to functors with a single "operator()" member only, otherwise which overload to target becomes ambiguous - if present then you'll need to target the specific overload you're interested in using 4 or 5 above instead). Includes lambdas as well (just syntactic sugar for creating functors on-the-fly). Simply apply "decltype" to your lambda to retrieve its compiler-generated class type, which you can then pass to "FunctionTraits" or any of its [Helper templates](#HelperTemplates). Please note however that generic lambdas are _not_ supported using this technique for technical reasons beyond the scope of this documentation. They can still be accessed via 4 and 5 above however using some unconventional syntax described in "TypeTraits.h" itself. Search the latter file for "_Example 3 (generic lambda)_" (quotes not included), which you'll find in the comments preceding the "FunctionTraits" specialization for functors (the comments for this example also provides additional technical details on this issue).

Note that the code also incorporates concepts when targeting C++20 or later or "static_assert" for C++17 (again, earlier versions aren't supported). In either case this will trap invalid function types with cleaner error messages at compile-time (if anything other than the above is passed for "F").

Once you've #included "TypeTraits.h", there are two ways to use the "FunctionTraits" class. You can either use "FunctionTraits" directly like so (but with verbose syntax that [Technique 2 Of 2](#Technique2Of2) below eliminates):

<a name="Technique1Of2"></a>
## Technique 1 of 2 - Using "FunctionTraits" directly (not usually recommended)

```C++
// Only file you need to explicitly #include (see "Usage" section just above)
#include "TypeTraits.h"

// Everything declared in this namespace
using namespace StdExt;

////////////////////////////////////////////////////////////////////////////
// Free function whose traits you wish to retrieve (note that static member
// functions are also considered "free" functions). Pointers and references
// to free functions are also supported (including references to such
// pointers), pointers to non-static member functions (including references
// to such pointers, though references to non-static member functions aren't
// supported in C++ itself), and functors (including lambdas).
////////////////////////////////////////////////////////////////////////////
float SomeFunc(const std::string &, double, int);

// Type of the above function (but see alternate syntax just below)
using F = decltype(SomeFunc);

///////////////////////////////////////////////
// Same as above but using a function pointer
// (function references also supported)
///////////////////////////////////////////////
// using F = decltype(&SomeFunc);

///////////////////////////////////////////////////////////
// Also works but using a reference to a function pointer
// (though the following syntax is rare in practice but
// more mainstream syntax may exist in your own code,
// though references to pointers are usually rare outside
// of function templates taking reference args to arbitrary
// types, so if passed a function pointer then the function
// template winds up taking a reference arg to a function
// pointer)
///////////////////////////////////////////////////////////
// using F = decltype(&SomeFunc) &;

////////////////////////////////////////////////////////
// And this works too (manually passing the type, just
// to show we can)
////////////////////////////////////////////////////////
// using F = float (const std::string &, double, int);

// Apply "FunctionTraits" to above function
using SomeFuncTraits = FunctionTraits<F>;

////////////////////////////////////////////////////
// Retrieve the function's return type (a "float").
// Note that this (mandatory C++) syntax is ugly
// however. A cleaner alias for this exists as
// described in the next section.
////////////////////////////////////////////////////
using SomeFuncReturnType_t = typename SomeFuncTraits::ReturnType;

// Number of arguments (3)
constexpr std::size_t SomeFuncArgCount = SomeFuncTraits::ArgCount;

/////////////////////////////////////////////////////////
// Retrieve type of the function's 3rd arg (an "int").
// Arg's index is zero-based, so passing 2 here (to
// target the 3rd arg). Note that this (mandatory C++)
// syntax is ugly however. A much cleaner alias for this
// exists as described in the next section.
/////////////////////////////////////////////////////////
using SomeFuncArg3Type_t = typename SomeFuncTraits::template Args<2>::Type;

////////////////////////////////////////////////////////////////
// Type of the function's 3rd arg (same as above) but returned
// as a "std::basic_string_view" (so literally returns "int",
// quotes not included). Again, a cleaner template exists for
// this as described in the next section. Note that the
// following variable template, "TypeName_v", can be used to
// get the (WYSIWYG) name for any type, not just types
// associated with "FunctionTraits". See its entry in the
// "Helper templates" section later.
////////////////////////////////////////////////////////////////
constexpr auto SomeFuncArg3TypeName = TypeName_v<SomeFuncArg3Type_t>; 

/////////////////////////////////////////////////////////////////
// Though few will rarely need to update a function trait (most
// need to read them only), you can also modify them as well
// (every available trait is supported). The following adds
// "noexcept" to the function (see its string representation
// just below):
/////////////////////////////////////////////////////////////////
using SomeFuncWithNoexceptAdded_t = typename SomeFuncTraits::AddNoexcept;

/////////////////////////////////////////////////////////////////////////
// Above type as a string so displays the following (same as "SomeFunc"
// but with "noexcept" added - note the format of the following may
// change slightly depending on the target compiler):
//
//     float(const std::basic_string<char>&, double, int) noexcept
/////////////////////////////////////////////////////////////////////////
constexpr auto SomeFuncWithNoexceptAdded_v = TypeName_v<SomeFuncWithNoexceptAdded_t>;

/////////////////////////////////////////////////////////
// And yet a different change, replacing the function's
// 3rd arg (an "int") with a "char", passing 2 here as
// the zero-based index of the arg we're targeting (see
// its string representation just below):
/////////////////////////////////////////////////////////
using SomeFuncReplace3rdArgWithChar_t = typename SomeFuncTraits::template ReplaceNthArg<2, char>;

////////////////////////////////////////////////////////////
// Above type as a string so displays the following (same
// as function "SomeFunc" but the 3rd arg has now been
// changed to a "char" - note the format of the following
// may change slightly depending on the target compiler):
//
//    float (const std::basic_string<char>&, double, char)
///////////////////////////////////////////////////////////
constexpr auto SomeFuncReplace3rdArgWithChar_v = TypeName_v<SomeFuncReplace3rdArgWithChar_t>;

// Etc. (see "Helper templates" further below for the complete list)

```

<a name="Technique2Of2"></a>
## Technique 2 of 2 - Using the helper templates instead of "FunctionTraits" directly (recommended)

Alternatively, instead of using "FunctionTraits" directly ([Technique 1 of 2](#Technique1Of2) above), you can rely on the second technique just below instead, which is normally much cleaner (and you should normally use it). As seen in the first technique above, relying on "FunctionTraits" directly can result in verbose syntax. For instance, due to the syntax of C++ itself, accessing the type of a given arg is ugly because you have to apply both the "typename" and "template" keywords (see "SomeFuncArg3Type_t" alias in the [Technique 1 of 2](#Technique1Of2) example). A helper template therefore exists not only for this, but for every member of "FunctionTraits". Therefore, instead of relying on "FunctionTraits" directly as seen in the above examples, you can rely on the [Helper templates](#HelperTemplates) instead. They're easier and cleaner, making the job of extracting or modifying a function's traits a breeze.

Note that two sets of helper templates exist, one taking a "FunctionTraits" template arg, and a corresponding template taking a function template arg (just pass your function's type as described [here](#SupportedTypesForF)). The latter templates simply create a "FunctionTraits" for you from the function type you pass and then defer to the corresponding "FunctionTraits" helper template. The "FunctionTraits" helper templates are rarely used directly in practice however so they're not documented here. Only the [Helper templates](#HelperTemplates) taking a function template arg are therefore documented further below. Most users will rely on these exclusively, though you can always quickly inspect the code itself if you ever want to use the "FunctionTraits" helper templates directly (they're just thin wrappers around the members of "FunctionTraits" which inherits all its members from its base classes). Here's the same example seen in "Technique 1" above but using these [Helper templates](#HelperTemplates) instead (though without repeating most of the comments seen in "Technique 1"):

```C++
// Only file you need to explicitly #include (see "Usage" section earlier)
#include "TypeTraits.h"

// Everything declared in this namespace
using namespace StdExt;

/////////////////////////////////////////////////////////
// Free function whose traits you wish to retrieve (see
// comments in corresponding example in "Technique 1"
// above)
/////////////////////////////////////////////////////////
float SomeFunc(const std::string &, double, int);

///////////////////////////////////////////////////
// Type of the above function (also see alternate
// syntax in "Technique 1" above).
///////////////////////////////////////////////////
using F = decltype(SomeFunc);

/////////////////////////////////////////////////////////
// Retrieve the function's return type (a "float") but
// using the cleaner helper alias this time (instead of
// "FunctionTraits" directly)
/////////////////////////////////////////////////////////
using SomeFuncReturnType_t = ReturnType_t<F>;

// Number of arguments (3)
constexpr std::size_t SomeFuncArgCount = ArgCount_v<F>;

//////////////////////////////////////////////////////////////
// Retrieve type of the function's 3rd arg (an "int") but
// again, using the cleaner helper alias this time (instead
// of "FunctionTraits" directly)
//////////////////////////////////////////////////////////////
using SomeFuncArg3Type_t = ArgType_t<F, 2>;

////////////////////////////////////////////////////////////////
// Type of the function's 3rd arg (same as above) but returned
// as a "std::basic_string_view" (so literally returns "int",
// quotes not included). Again, uses the easier helper template
// this time instead of "FunctionTraits" directly.
////////////////////////////////////////////////////////////////
constexpr auto SomeFuncArg3TypeName = ArgTypeName_v<F, 2>;

/////////////////////////////////////////////////////////////////
// Though few will rarely need to update a function trait (most
// need to read them only), you can also modify them as well
// (every available trait is supported). The following adds
// "noexcept" to the function (see its string representation
// just below). Again, uses the easier helper template this
// time instead of "FunctionTraits" directly:
/////////////////////////////////////////////////////////////////
using SomeFuncWithNoexceptAdded_t = AddNoexcept_t<F>;

/////////////////////////////////////////////////////////////////////////
// Above type as a string so displays the following (same as "SomeFunc"
// but with "noexcept" added - note the format of the following may
// change slightly depending on the target compiler):
//
//     float(const std::basic_string<char>&, double, int) noexcept
/////////////////////////////////////////////////////////////////////////
constexpr auto SomeFuncWithNoexceptAdded_v = TypeName_v<SomeFuncWithNoexceptAdded_t>;

/////////////////////////////////////////////////////////
// And yet a different change, replacing the function's
// 3rd arg (an "int") with a "char", passing 2 here as
// the zero-based index of the arg we're targeting (see
// its string representation just below). Again, uses
// the easier helper template this time instead of
// "FunctionTraits" directly.
/////////////////////////////////////////////////////////
using SomeFuncReplace3rdArgWithChar_t = ReplaceNthArg_t<F, 2, char>;

////////////////////////////////////////////////////////////
// Above type as a string so displays the following (same
// as function "SomeFunc" but the 3rd arg has now been
// changed to a "char" - note the format of the following
// may change slightly depending on the target compiler):
//
//    float (const std::basic_string<char>&, double, char)
///////////////////////////////////////////////////////////
constexpr auto SomeFuncReplace3rdArgWithChar_v = TypeName_v<SomeFuncReplace3rdArgWithChar_t>;

// Etc. (see "Helper templates" further below for the complete list)

```

<a name="LoopingThroughAllFunctionArguments"></a>
## Looping through all function arguments

You can even loop through all arguments using the helper function template [ForEachArg](#ForEachArg). The following example assumes C++20 or later for the lambda template seen below (lambda templates aren't available until C++20 or later), though if targeting C++17 you can easily replace it with your own functor instead (the "operator()" member in your functor needs to be a template however, with the same template args seen in the lambda below and the same code). See [ForEachArg](#ForEachArg) for further details.
```C++
// Only file you need to explicitly #include (see "Usage" section earlier)
#include "TypeTraits.h"

// Everything declared in this namespace
using namespace StdExt;

// Free function whose arg types you wish to iterate
float SomeFunc(const std::string &, double, int);

// Type of the above function
using F = decltype(SomeFunc);

//////////////////////////////////////////////////////////////
// Lambda that will be invoked just below, once for each arg
// in function "F" (where template arg "I" is the zero-based
// "Ith" argument in function "F", and "ArgTypeT" is its type).
// Note that lambda templates are supported in C++20 and later
// only. For C++17 (earlier versions aren't supported), you
// need to roll your own functor instead (with a template
// version of "operator()" equivalent to this lambda)
//////////////////////////////////////////////////////////////
const auto displayArgType = []<std::size_t I, typename ArgTypeT>()
                            {
                                //////////////////////////////////////////////////////////
                                // Display the type of the (zero-based) "Ith" arg in
                                // function "F" (where template arg "I" just above stores
                                // this index though we don't use it for anything in this
                                // example, and "ArgTypeT" is that arg's type). Note that
                                // we never come through here if function "F" has no args
                                // (or just variadic args only - variadic args aren't
                                // processed). Also note that the following call to
                                // "TypeName_v" simply returns "ArgTypeT" as a compile-time
                                // string ("std::basic_string_view"). See "TypeName_v" in
                                // the "Helper templates" section just below (consult its
                                // entry there - it can be passed any C++ type).
                                //
                                // Finally, note that on non-Microsoft platforms, "tcout"
                                // and "_T" (both automatically declared in namespace
                                // "StdExt" when you #include "TypeTraits.h"), always
                                // resolve to "std::cout" and (for _T) simply the arg
                                // you pass it (_T effectively does nothing). On
                                // Microsoft platforms however they resolve to
                                // "std::wcout" and L##x (for the given macro arg "x")
                                // respectively when compiling for Unicode (normally the
                                // case), or "std::cout" and (for _T) the arg you pass
                                // it otherwise (_T effectively does nothing in the
                                // latter case). For further details in general, see the
                                // comments preceding the definitions of "_T", "tcout"
                                // and "TCHAR" in "CompilerVersions.h" (for non-Microsoft
                                // platforms but those targeting Microsoft may wish to
                                // review this as well).
                                ////////////////////////////////////////////////////////// 
                                tcout << TypeName_v<ArgTypeT> << _T("\n");

                                //////////////////////////////////////////////
                                // Return true to continue iterating (false
                                // would stop iterating, equivalent to a
                                // "break" statement in a regular "for" loop)
                                //////////////////////////////////////////////
                                return true;
                             };

////////////////////////////////////////////////
// Iterate all argument types in function "F",
// invoking "displayArgType" above for each.
////////////////////////////////////////////////
ForEachArg<F>(displayArgType);
```

<a name="HelperTemplates"></a>
## Helper templates (complete, alphabetical list)

The following provides a complete (alphabetical) list of all helper templates. Two separate sections exist, the first for [Read traits](#ReadTraits), allowing you to read any part up a function's type, and the second for [Write traits](#WriteTraits), allowing you to update any part of a function's type. Note that the first template arg of every template is the function you're targeting, whose name is always "F" (see [here](#SupportedTypesForF) for details). IMPORTANT: When "F" is a functor, note that all traits implicitly refer (apply) to the non-static member function "F::operator()" unless noted otherwise (so if "F" is the type of a lambda for instance then the traits target "operator()" of the compiler-generated class for that lambda). It should therefore be understood that whenever "F" is a functor and it's cited in the description of each template, the description is normally referring to member function "F::operator()", not class "F" itself.

Please note that for all traits, a TRAITS_FUNCTION_C concept (declared in "TypeTraits.h") will kick in for illegal values of "F" in C++20 or later ("F" is declared a TRAITS_FUNCTION_C in all templates below), or a "static_assert" in C++17 otherwise (again, earlier versions aren't supported). For most of the traits, "F" is the only template arg. Only a small handful of templates take additional (template) args which are described on a case-by-case basis below. Lastly, note that the "=" sign is omitted after the template name in each template declaration below. The "=" sign is removed since the implementation isn't normally relevant for most users. You can inspect the actual declaration in "TypeTraits.h" if you wish to see it.

Finally, please note that each template below simply wraps the corresponding member of the "FunctionTraits" struct itself
(or indirectly targets it). As previously described, you can access "FunctionTraits" directly (see this class and its various base classes - you can access all public members), or using any of its own helper templates _not_ documented here (each takes a "FunctionTraits" template arg). The following helper templates however taking a function template arg "F" instead are normally much easier (see these in [Technique 2 of 2](#Technique2Of2) earlier). The syntax for accessing "FunctionTraits" directly (or using any of its own helper templates) is therefore not shown in this document (just the earlier examples in [Technique 1 of 2](#Technique1Of2) only). You can simply review the public members of "FunctionTraits" inherited from its base classes in "TypeTraits.h" in the unlikely event you ever need to work with them directly (or alternatively review the helper templates in "TypeTraits.h" taking a "FunctionTraits" template arg). However, every public member of "FunctionTraits" inherited from its base classes has a helper template taking a function template arg "F" as seen in each template declaration below, including some additional helper templates not available when accessing "FunctionTraits" directly (or its base classes). These helper templates should therefore normally be relied on since they're easier and cleaner, so the following documentation is normally all you ever need to consult.

<a name="ReadTraits"></a>
### _Read traits_
  
<a name="ArgCount_v"></a><details><summary>ArgCount_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr std::size_t ArgCount_v;
```
"std::size_t" variable storing the number of arguments in "F" *not* including variadic arguments if any. Will be zero if function "F" has no (non-variadic) args. Note that this count is formally called "arity" but this variable is given a more user-friendly name.<br /><br /><ins>IMPORTANT</ins>:<br />Please note that if you wish to check if a function's argument list is completely empty, then inspecting this helper template for zero (0) is not sufficient, since it may return zero but still contain variadic args. To check for a completely empty argument list, call [IsEmptyArgList_v](#IsEmptyArgList_v) instead.</details>

<a name="ArgType_t"></a><details><summary>ArgType_t</summary>
```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I>
using ArgType_t;
```
Type alias for the type of the "Ith" arg in function "F", where "I" is in the range 0 to the number of (non-variadic) arguments in "F" minus 1 (see "ArgCount_v" variable just above). Pass "I" as the (zero-based) 2nd template arg (see earlier examples). Note that if "I" is greater than or equal to the number of args in "F" (again, see "ArgCount_v" just above), then a compiler error will occur (so if "F" has no non-variadic args whatsoever, a compiler error will always occur, even if passing zero).</details>

<a name="ArgTypeName_v"></a><details><summary>ArgTypeName_v</summary>
```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I>
inline constexpr tstring_view ArgTypeName_v;
```
Same as "ArgType_t" just above but returns this as a (WYSIWYG) string (of type "tstring_view" - see [TypeName_v](#TypeName_v) for details). A float would therefore be (literally) returned as "float" for instance (quotes not included).</details>

<a name="ArgTypes_t"></a><details><summary>ArgTypes_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using ArgTypes_t;
```
Type alias for a "std::tuple" representing all non-variadic argument types in "F". Rarely required in practice however since you'll usually rely on "ArgType_t" or "ArgTypeName_v" to retrieve the type of a specific argument (see these above). If you require the "std::tuple" that stores all (non-variadic) argument types, then it's typically (usually) because you want to iterate all of them (say, to process the type of every argument in a loop). If you require this, then you can use the "ForEachArg()" helper function (template) further below. See this for details.</details>

<a name="CallingConvention_v"></a><details><summary>CallingConvention_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr CallingConvention CallingConvention_v;
```
Calling convention of "F" returned as a "CallingConvention" enumerator (declared in "TypeTraits.h"). Calling conventions include "Cdecl", "Stdcall", etc. (note that functions with calling conventions not seen in this enumerator are not supported, but all mainstream calling conventions are). Also please note that compilers will sometimes change the calling convention declared on your functions to the "Cdecl" calling convention depending on the compiler options in effect at the time (in particular when compiling for 64 bits opposed to 32 bits, though the "Vectorcall" calling convention *is* supported on 64 bits but not on GCC since it doesn't support this particular calling convention at all). In this case the calling convention on your function is ignored and "CallingConvention_v" will correctly return the "Cdecl" calling convention (if that's what the compiler actually used).</details>

<a name="CallingConventionName_v"></a><details><summary>CallingConventionName_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view CallingConventionName_v;
```
Same as "CallingConvention_v" just above but returns this as a (WYSIWYG) string (of type "tstring_view" - see [TypeName_v](#TypeName_v) for details).</details>

<a name="ForEachArg"></a><details><summary>ForEachArg</summary>
```C++
template <TRAITS_FUNCTION_C F,>
          FOR_EACH_TUPLE_FUNCTOR_C ForEachTupleFunctorT>
inline constexpr bool ForEachArg(ForEachTupleFunctorT &&);
```
Not a traits template (unlike all other read traits), but a helper function template you can use to iterate all arguments for function "F" if required (though rare in practice since you'll usually rely on "ArgType_t" or "ArgTypeName_v" to retrieve the type of a specific argument - see these above). See [Looping through all function arguments](#LoopingThroughAllFunctionArguments) earlier for an example, as well as the declaration of "ForEachArg()" in "TypeTraits.h" for full details (or for a complete program that also uses it, see the [demo](https://godbolt.org/z/fx8MWGv99) program, also available in the repository itself).</details>

<a name="FunctionType_t"></a><details><summary>FunctionType_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using FunctionType_t;
```
Type alias identical to "F" itself unless "F" is a functor (i.e., "IsFunctor_v" returns true), in which case it's a type alias for the "operator()" member of the functor (to retrieve the functor type itself in this case, see [MemberFunctionClass_t](#MemberFunctionClass_t)).</details>

<a name="FunctionTypeName_v"></a><details><summary>FunctionTypeName_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool FunctionTypeName_v;
```
Same as "FunctionType_t" just above but returns this as a (WYSIWYG) string (of type "tstring_view" - see [TypeName_v](#TypeName_v) for details).</details>

<a name="IsEmptyArgList_v"></a><details><summary>IsEmptyArgList_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsEmptyArgList_v;
```
"bool" variable set to "true" if the function represented by "F" has an empty arg list (it has no args whatsoever including variadic args), or "false" otherwise. If true then note that "ArgCount_v" is guaranteed to return zero (0), and "IsVariadic_v" is guaranteed to return false.<br /><br /><ins>IMPORTANT</ins>:<br />Note that you should rely on this helper to determine if a function's argument list is completely empty opposed to checking the "ArgCount_v" helper for zero (0), since the latter returns zero only if "F" has no non-variadic args. If it has variadic args but no others, i.e., its argument list is "(...)", then the argument list isn't empty even though "ArgCount_v" returns zero (since it still has variadic args). Caution advised.</details>

<a name="IsFreeFunction_v"></a><details><summary>IsFreeFunction_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsFreeFunction_v;
```
"bool" variable set to "true" if "F" is a free function (including static member functions), or "false" otherwise.</details>

<a name="IsFunctor_v"></a><details><summary>IsFunctor_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsFunctor_v;
```
"bool" variable set to "true" if "F" is a functor (the functor's class/struct was passed for "F") or "false" otherwise. Note that when true, [IsMemberFunction_v](#IsMemberFunction_v) is also guaranteed to be true.</details>

<a name="IsMemberFunction_v"></a><details><summary>IsMemberFunction_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsMemberFunction_v;
```
"bool" variable set to "true" if "F" is a non-static member function (including when "F" is a functor), or "false" otherwise (if you need to specifically check for functors only then see "IsFunctor_v" just above). Note that you may need to invoke this before accessing the following helper templates. Since the following are applicable to non-static member functions only, if you don't know whether "F" is a non-static member function ahead of time (or a functor), then you should normally call "IsMemberFunction_v" to determine this first. If it's "false" then "F" is a free function (which includes static member functions), so a call to any of the following will result in default values being returned that aren't applicable to free functions (so you shouldn't normally invoke them unless you're ok with the default values they return for free functions):<br /><br />- IsMemberFunctionConst_v<br />- IsMemberFunctionVolatile_v<br />- MemberFunctionClass_t<br />- MemberFunctionClassName_v<br />- MemberFunctionRefQualifier_v<br />- MemberFunctionRefQualifierName_v</details>

<a name="IsMemberFunctionConst_v"></a><details><summary>IsMemberFunctionConst_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsMemberFunctionConst_v;
```
"bool" variable applicable only if "F" is a non-static member function (or a functor). Set to "true" if the function has the "const" cv-qualifier (it's declared with the "const" keyword) or "false" otherwise. Always "false" for free functions including static member functions (not applicable to either). You may therefore wish to invoke [IsMemberFunction_v](#IsMemberFunction_v) to detect if "F" is in fact a non-static member function (or functor) before using this trait.</details>

<a name="IsMemberFunctionVolatile_v"></a><details><summary>IsMemberFunctionVolatile_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsMemberFunctionVolatile_v;
```
"bool" variable applicable only if "F" is a non-static member function  (or a functor). Set to "true" if the function has the "volatile" cv-qualifier (its declared with the "volatile" keyword) or "false" otherwise. Always "false" for free functions including static member functions (not applicable to either). You may therefore wish to invoke [IsMemberFunction_v](#IsMemberFunction_v) to detect if "F" is in fact a non-static member function (or functor) before using this trait.</details>

<a name="IsNoexcept_v"></a><details><summary>IsNoexcept_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsNoexcept_v;
```
"bool" variable set to "true" if the function is declared as "noexcept" or "false" otherwise (always false if the "noexcept" specifier is absent in the function, otherwise, if present then it evaluates to "true" if no bool expression is present in the "noexcept" specifier (the expression has been omitted), or the result of the bool expression otherwise - WYSIWYG).</details>

<a name="IsVariadic_v"></a><details><summary>IsVariadic_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsVariadic_v;
```
"bool" variable set to true if "F" is a variadic function (last arg of "F" is "...") or false otherwise.</details>

<a name="MemberFunctionClass_t"></a><details><summary>MemberFunctionClass_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionClass_t;
```
If "F" is a non-static member function (or a functor), a type alias for the type of the class (or struct) that declares that function (same as "F" itself if "F" is a functor). Always "void" otherwise (for free functions including static member functions). You may therefore wish to invoke [IsMemberFunction_v](#IsMemberFunction_v) to detect if "F" is in fact a non-static member function (or functor) before applying this trait.</details>

<a name="MemberFunctionClassName_v"></a><details><summary>MemberFunctionClassName_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view MemberFunctionClassName_v;
```
Same as "MemberFunctionClass_t" just above but returns this as a (WYSIWYG) string (of type "tstring_view" - see [TypeName_v](#TypeName_v) for details).</details>

<a name="MemberFunctionRefQualifier_v"></a><details><summary>MemberFunctionRefQualifier_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr RefQualifier MemberFunctionRefQualifier_v;
```
"RefQualifier" variable, a proprietary enumerator in "TypeTraits.h" applicable only if "F" is a non-static member function (or a functor). Set to "RefQualifier::None" if the function isn't declared with any reference qualifiers (usually the case for non-static member functions in practice, and always the case for free functions since it's not applicable), "RefQualifier::LValue" if the function is declared with the "&" reference qualifier, or "RefQualifier::RValue" if the function is declared with the "&&" reference qualifier. Note that you may wish to invoke [IsMemberFunction_v](#IsMemberFunction_v) to detect if "F" is in fact a non-static member function (or functor) before applying this trait.</details>

<a name="MemberFunctionRefQualifierName_v"></a><details><summary>MemberFunctionRefQualifierName_v</summary>
```C++
template <TRAITS_FUNCTION_C F,
          bool UseAmpersands = true>
inline constexpr tstring_view MemberFunctionRefQualifierName_v;
```
Same as "MemberFunctionRefQualifier_v" just above but returns this as a (WYSIWYG) string (of type "tstring_view" - see [TypeName_v](#TypeName_v) for details). Note that this template also takes an extra template arg besides function "F", a "bool" called "UseAmpersands", indicating whether the returned string should be returned as "&" or "&&" (if the function is declared with an "&" or "&&" reference qualifier respectively), or "LValue" or "RValue" otherwise. Defaults to "true" if not specified (returns "&" or "&&" by default). Not applicable however if no reference qualifiers are present ("None" is always returned).</details>

<a name="ReturnType_t"></a><details><summary>ReturnType_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using ReturnType_t;
```
Type alias for the return type of function "F".</details>

<a name="ReturnTypeName_v"></a><details><summary>ReturnTypeName_v</summary>
```C++
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view ReturnTypeName_v;
```
Same as "ReturnType_t" just above but returns this as a (WYSIWYG) string (of type "tstring_view" - see [TypeName_v](#TypeName_v) for details). A float would therefore be (literally) returned as "float" for instance (quotes not included).</details>

<a name="TypeName_v"></a><details><summary>TypeName_v</summary>
```C++
template <typename T>
inline constexpr tstring_view TypeName_v;
```
Not a template associated with "FunctionTraits" per se, but a helper template you can use to return the user-friendly name of any C++ type as a "tstring_view" (more on this shortly). Just pass the type you're interested in as the template's only template arg. Note however that all helper aliases above such as "ArgType_t" have a corresponding helper "Name" template ("ArgTypeName_v" in the latter case) that simply rely on "TypeName_v" to return the type's user-friendly name (by simply passing the alias itself to "TypeName_v"). You therefore don't have to call "TypeName_v" directly for any of the type aliases in this library since a helper variable template already exists that does this for you (again, one for every alias template above, where the name of the variable template returning the type's name is the same as the name of the alias template itself but with the "_t" suffix in the alias' name replaced with "Name_v", e.g., "ArgType_t" and "ArgTypeName_v"). The only time you may need to call "TypeName_v" directly when using "FunctionTraits" is when you use "ForEachArg()" as seen in the [Looping through all function arguments](#LoopingThroughAllFunctionArguments) section above. See the sample code in that section for an example (specifically the call to "TypeName_v" in the "displayArgType" lambda of the example).<br/><br/>Note that "TypeName_v" can be passed any C++ type however, not just types associated with "FunctionTraits". You can therefore use it for your own purposes whenever you need the user-friendly name of a C++ type as a compile-time string. Note that "TypeName_v" returns a "tstring_view" (in the "StdExt" namespace) which always resolves to "std::string_view" on non-Microsoft platforms, and on Microsoft platforms, to "std::wstring_view" when compiling for Unicode (usually the case - strings are normally stored in UTF-16 in modern-day Windows), or "std::string_view" otherwise (when compiling for ANSI but this is very rare these days).</details>

---
<a name="WriteTraits"></a>
### _Write traits_

<a name="AddVariadicArgs_t"></a><details><summary>AddVariadicArgs_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using AddVariadicArgs_t;
```
Type alias for "F" after adding "..." to the end of its argument list if not already present. Note that the calling convention is also changed to the "Cdecl" calling convention for the given compiler. This is the only supported calling convention for variadic functions in this release but most platforms require this calling convention for variadic functions. It ensures that the calling function (opposed to the called function) pops the stack of arguments after the function is called, which is required by variadic functions. Other calling conventions that also do this are possible though none are currently supported in this release (since none of the currently supported compilers support this - such calling conventions are rare in practice).</details>

<a name="AddNoexcept_t"></a><details><summary>AddNoexcept_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using AddNoexcept_t;
```
Type alias for "F" after adding "noexcept" to "F" if not already present.</details>

<a name="MemberFunctionAddConst_t"></a><details><summary>MemberFunctionAddConst_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionAddConst_t;
```
If "F" is a non-static member function, yields a type alias for "F" after adding the "const" cv-qualifier to the function if not already present. If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since "const" applies to non-static member functions only).</details>

<a name="MemberFunctionAddCV_t"></a><details><summary>MemberFunctionAddCV_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionAddCV_t;
```
If "F" is a non-static member function, yields a type alias for "F" after adding both the "const" AND "volatile" cv-qualifiers to the function if not already present. If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since "const" and "volatile" apply to non-static member functions only).</details>

<a name="MemberFunctionAddLValueReference_t"></a><details><summary>MemberFunctionAddLValueReference_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionAddLValueReference_t;
```
If "F" is a non-static member function, yields a type alias for "F" after adding the "&" reference-qualifier to the function if not already present (replacing the "&&" reference-qualifier if present). If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since reference-qualifiers apply to non-static member functions only).</details>

<a name="MemberFunctionAddRValueReference_t"></a><details><summary>MemberFunctionAddRValueReference_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionAddRValueReference_t;
```
If "F" is a non-static member function, yields a type alias for "F" after adding the "&&" reference-qualifier to the function (replacing the "&" reference-qualifier if present). If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since reference-qualifiers apply to non-static member functions only).</details>

<a name="MemberFunctionAddVolatile_t"></a><details><summary>MemberFunctionAddVolatile_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionAddVolatile_t;
```
If "F" is a non-static member function, yields a type alias for "F" after adding the "volatile" cv-qualifier to the function if not already present. If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since "volatile" applies to non-static member functions only).</details>

<a name="MemberFunctionRemoveConst_t"></a><details><summary>MemberFunctionRemoveConst_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionRemoveConst_t;
```
If "F" is a non-static member function, yields a type alias for "F" after removing the "const" cv-qualifier from the function if present. If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since "const" applies to non-static member functions only so will never be present otherwise).</details>

<a name="MemberFunctionRemoveCV_t"></a><details><summary>MemberFunctionRemoveCV_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionRemoveCV_t;
```
If "F" is a non-static member function, yields a type alias for "F" after removing both the "const" AND "volatile" cv-qualifiers from the function if present. If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since "const" and "volatile" apply to non-static member functions only so will never be present otherwise).</details>

<a name="MemberFunctionRemoveReference_t"></a><details><summary>MemberFunctionRemoveReference_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionRemoveReference_t;
```
If "F" is a non-static member function, yields a type alias for "F" after removing the "&" or "&&" reference-qualifier from the function if present. If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since reference-qualifiers to non-static member functions only so will never be present otherwise).</details>

<a name="MemberFunctionRemoveVolatile_t"></a><details><summary>MemberFunctionRemoveVolatile_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionRemoveVolatile_t;
```
If "F" is a non-static member function, yields a type alias for "F" after removing the "volatile" cv-qualifier from the function if present. If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since "volatile" applies to non-static member functions only so will never be present otherwise).</details>

<a name="MemberFunctionReplaceClass_t"></a><details><summary>MemberFunctionReplaceClass_t</summary>
```C++
template <TRAITS_FUNCTION_C F,
          typename NewClassT>
using MemberFunctionReplaceClass_t;
```
If "F" is a non-static member function, yields a type alias for "F" after replacing the class this function belongs to with "NewClassT". If "F" is a free function including static member functions, yields "F" itself (effectively does nothing since a "class" applies to non-static member functions only so will never be present otherwise - note that due to limitations in C++ itself, replacing the class for static member functions is not supported).</details>

<a name="RemoveVariadicArgs_t"></a><details><summary>RemoveVariadicArgs_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using RemoveVariadicArgs_t;
```
If "F" is a variadic function (its last arg is "..."), yields a type alias for "F" after removing the "..." from the argument list. All non-variadic arguments (if any) remain intact (only the "..." is removed).</details>

<a name="RemoveNoexcept_t"></a><details><summary>RemoveNoexcept_t</summary>
```C++
template <TRAITS_FUNCTION_C F>
using RemoveNoexcept_t;
```
Type alias for "F" after removing "noexcept" from "F" if present.</details>

<a name="ReplaceArgs_t"></a><details><summary>ReplaceArgs_t</summary>
```C++
template <TRAITS_FUNCTION_C F,
          typename... NewArgsT>
using ReplaceArgs_t;
```
Type alias for "F" after replacing all its existing non-variadic arguments with the args given by "NewArgsT" (a parameter pack of the types that become the new argument list). If none are passed then an empty argument list results instead, though if variadic args are present in "F" then they still remain intact (the "..." remains - read on). The resulting alias is identical to "F" itself except that the non-variadic arguments in "F" are completely replaced with "NewArgsT". Note that if "F" is a variadic function (its last parameter is "..."), then it remains a variadiac function after the call (the "..." remains in place). If you wish to explicitly add or remove the "..." as well then pass the resulting type to "AddVariadicArgs_t" or "RemoveVariadicArgs_t" respectively (either before or after the call to "ReplaceArgs_t"). Note that if you wish to replace specific arguments instead of all of them, then call "ReplaceNthArg_t" instead. Lastly, you can alternatively use "ReplaceArgsTuple_t" instead of "ReplaceArgs_t" if you have a "std::tuple" of types you wish to use for the argument list instead of a parameter pack. "ReplaceArgsTuple_t" is identical to "ReplaceArgs_t" otherwise (it ultimately defers to it).</details>

<a name="ReplaceArgsTuple_t"></a><details><summary>ReplaceArgsTuple_t</summary>
```C++
template <TRAITS_FUNCTION_C F,
          TUPLE_C NewArgsTupleT>
using ReplaceArgsTuple_t;
```
Identical to "ReplaceArgs_t" just above except the argument list is passed as a "std::tuple" instead of a parameter pack (via the 2nd template arg). The types in the "std::tuple" are therefore used for the resulting argument list. "ReplaceArgsTuple_t" is otherwise identical to "ReplaceArgs_t" (it ultimately defers to it).</details>

<a name="ReplaceCallingConvention_t"></a><details><summary>ReplaceCallingConvention_t</summary>
```C++
template <TRAITS_FUNCTION_C F,
          CallingConvention NewCallingConventionT>
using ReplaceCallingConvention_t;
```
Type alias for "F" after replacing its calling convention with the platform-specific calling convention corresponding to "NewCallingConventionT" (a "CallingConvention" enumerator declared in "TypeTraits.h"). For instance, if you pass  "CallingConvention::FastCall" then the calling convention on "F" is replaced with "\_\_attribute\_\_((cdecl))" on GCC and Clang, but "\_\_cdecl" on Microsoft platforms. Note however that the calling convention for variadic functions (those whose last arg is "...") can't be changed in this release. Variadic functions require that the calling function pop the stack to clean up passed arguments and only the "Cdecl" calling convention supports that in this release (on all supported compilers at this writing). Attempts to change it are therefore ignored. Note that you also can't change the calling convention of free functions to "CallingConvention::Thiscall" (including for static member functions since they're considered "free" functions). Attempts to do so are ignored since the latter calling convention applies to non-static member functions only. Lastly, please note that compilers will sometimes change the calling convention declared on your functions to the "Cdecl" calling convention depending on the compiler options in effect at the time (in particular when compiling for 64 bits opposed to 32 bits, though the "Vectorcall" calling convention *is* supported on 64 bits on compilers that support this calling convention). Therefore, if you specify a calling convention that the compiler changes to "Cdecl" based on the compiler options currently in effect, then "ReplaceCallingConvention_t" will also ignore your calling convention and apply "Cdecl" instead (since that's what the compiler actually uses).</details>

<a name="ReplaceNthArg_t"></a><details><summary>ReplaceNthArg_t</summary>
```C++
template <TRAITS_FUNCTION_C F,
          std::size_t N,
          typename NewArgT>
using ReplaceNthArg_t;
```
Type alias for "F" after replacing its (zero-based) "Nth" argument with "NewArgT". Pass "N" via the 2nd template arg (i.e., the zero-based index of the arg you're targeting), and the type you wish to replace it with via the 3rd template arg ("NewArgT"). The resulting alias is therefore identical to "F" except its "Nth" argument is replaced by "NewArgT" (so passing, say, zero-based "2" for "N" and "int" for "NewArgT" would replace the 3rd function argument with an "int"). Note that "N" must be less than the number of arguments in the function or a "static_assert" will occur (new argument types can't be added using this trait, only existing argument types replaced). If you need to replace multiple arguments then recursively call "ReplaceNthArg_t" again, passing the result as the "F" template arg of "ReplaceNthArg_t" as many times as you need to (each time specifying a new "N" and "NewArgT"). If you wish to replace all arguments at once then call "ReplaceArgs_t" or "ReplaceArgsTuple_t" instead. Lastly, note that if "F" has variadic arguments (it ends with "..."), then these remain intact. If you need to remove them then call "RemoveVariadicArgs_t" before or after the call to "ReplaceNthArg_t".</details>

<a name="ReplaceReturnType_t"></a><details><summary>ReplaceReturnType_t</summary>
```C++
template <TRAITS_FUNCTION_C F,
          typename NewReturnTypeT>
using ReplaceReturnType_t;
```
Type alias for "F" after replacing its return type with "NewReturnTypeT".
</details>

<a name="WhyChooseThisLibrary"></a>
## Why choose this library
In a nutshell, because it's extremely easy to use, with syntax that's consistently very clean (when relying on [Technique 2 of 2](#Technique2Of2) as most normally will), has a very small footprint (once you ignore the many comments in "TypeTraits.h"), and it may be the most complete function traits library available on the web at this writing (based on my attempt to find an equivalent library with calling convention support in particular). It's also significantly smaller than the Boost version ("boost::callable_traits"), which consists of a bloated number of files and at least twice the amount of code (largely due to a needlessly complex design, no disrespect intended). "FunctionTraits" still provides the same features for all intents and purposes however (and a few extra), as well as support for (mainstream) calling conventions as emphasized, which only has limited support in "boost::callable_traits" (but again, it's not enabled by default and the author's own internal comments about it are negative and discourage its use). Note that even when activated, calling convention support in "boost::callable_traits" isn't designed to work in 64 bit builds (it won't compile), while "FunctionTraits" does support it. Note that "boost::callable_traits" does support the experimental "transaction_safe" keyword however (unrelated to calling conventions), but "FunctionTraits" doesn't by design. Since this keyword isn't in the official C++ standard (most have never likely heard of it), and it's questionable if it ever will be (it was first floated in 2015), I've deferred its inclusion until it's actually implemented, if ever. Very few users will be impacted by its absence and including it in "FunctionTraits" can likely be done in less than a day based on my review of the situation.

The upshot is that "FunctionTraits" is generally more complete than other similar libraries I've seen, in particular due to its (mainstream) calling convention support (noting that very few other libraries even exist that can be considered "complete"). Very little could be added at this stage that would benefit most users and would usually require improvements to the C++ standard itself to accommodate. However, even wider support for calling conventions may be added later, to target less frequently used calling conventions that are platform specific (if there's a demand for it). For now its mainstream calling convention support will usually meet the needs of most users.

<a name="DesignConsiderations"></a>
## "FunctionTraits" design considerations (for those who care - not required reading)
Note that "FunctionTraits" is not a canonical C++ "traits" class that would likely be considered for inclusion in the C++ standard itself. While it could be with some fairly easy-to-implement changes and (rarely-used) additions (read on), it wasn't designed for this. It was designed for practical, real-world use instead. For instance, it supports most mainstream calling conventions as emphasized earlier which isn't currently addressed in the C++ standard itself. Also note that template arg "F" of "FunctionTraits" and its helper templates isn't confined to pure C++ function types. As described earlier in this document, you can also pass pointers to functions, references to functions, references to pointers to functions, and functors (in all cases referring to their actual C++ types, not any actual instances). Note that references to non-static member function pointers aren't supported however because they're not supported by C++ itself.

While I'm not a member of the C++ committee, it seems very likely that a "canonical" implementation for inclusion in the standard itself likely wouldn't (or might not) address certain features, like calling conventions since they don't currently exist in the C++ standard as described earlier (unless it's ever added at some point). It's also unlikely to support all the variations of pointers and reference types (and/or possibly functors) that "FunctionTraits" handles. In the real world however function types include calling conventions so they need to be detected by a function traits library (not just the default calling convention), and programmers are also often working with raw function types or pointers to functions or functors (or references to functions or references to pointers to functions though these are typically encountered less frequently). For non-static member functions in particular pointers to member functions are the norm. In fact, for non-static member functions, almost nobody in the real world ever works with their actual (non-pointer) function type. I've been programming in C++ for decades and don't recall a single time I've ever worked with the actual type of one. You work with pointers to non-static member functions instead when you need to, such as **void (YourClass::*)()**. Even for free functions (which for our purposes also includes static member functions), since the name of a free function decays into a pointer to the function in most expressions, you often find yourself working with a pointer to such functions, not the actual function type (though you do work with the actual function type sometimes, unlike for non-static member functions which is extremely rare in the real world). Supporting pointers and references to functions and even references to pointers to functions therefore just make things easier even if some consider it unclean (I don't). A practical traits struct should just make it very easy to use without having to manually remove things using "std::remove_pointer" and/or "std::remove_reference" first (which would be required if such a traits struct was designed to handle pure C++ function types only, not pointers and references to function types or even references to pointers). It's just easier and more flexible to use it this way (syntatically cleaner). Some would argue it's not a clean approach but I disagree. It may not be consistent with how things are normally done in the standard itself (again, I suspect it might only handle raw function types only), but it better deals with the real-world needs of most developers IMHO (so it's "clean" because its syntax is designed to cleanly support that).

Lastly, note that "FunctionTraits" doesn't support the so-called [abominable function types](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html). These are simply non-static member function types (not pointers to such types but the actual raw C++ type) with cv ("const" and/or "volatile") and/or reference qualifiers (& and &&). For full details, see the comments preceding "Private::IsFreeFunction" in the "TypeTraits.h" header. Abominable functions are arguably a flawed feature of the C++ type system, and a wider discussion of them isn't warranted here. The bottom line is that most programmers will never encounter them throughout their entire career, and for practical reasons (not discussed here), their lack of support by "FunctionTraits" makes its design cleaner and easier to use. "FunctionTraits" will therefore reject such functions via a concept in C++20 or later (possibly depending on the context), or a "static_assert" otherwise (always in C++17). Instead, non-static member functions are always handled using mainstream pointer-to-member function syntax. Raw C++ function types are always treated as free functions by "FunctionTraits" (which includes static member functions for our purposes), so if you pass a raw C++ function type (i.e., an actual function type and not a pointer or reference to such a type), no cv or ref qualifiers are allowed on that type (i.e., abominable functions aren't supported), since free functions don't have such qualifiers. They're only legal in C++ for non-static member functions. Passing an abominable function is therefore considered illegal by "FunctionTraits" even though it's a valid C++ function type (but they're "abominable" nevertheless so we don't support them because the design is cleaner and easier to use when we don't - in the real-world almost nobody will care).
