# FunctionTraits
## C++ traits struct (template) to retrieve traits of any function (a "complete" implementation)

See [here](https://godbolt.org/z/PPPshYrh7) for a complete working example (demo for those who want to get right to the code - the same code in this repository).

"FunctionTraits" is a lightweight C++ traits struct (template) that allows you to quickly and easily determine the traits of any function at compile-time (for C++17 and later). It's a "complete" implementation in the sense that it detects functions with difference calling conventions as well, which all other implementations I've seen fail to do at this writing (respectfully). Without calling convention support, functions with calling conventions other than the default (normally "cdecl") won't be detected, because calling conventions are normally treated as part of a function's signature (so if not handled and they normally aren't by other implementations, such functions will be ignored or rejected). While most developers rely on the default calling convention for their own functions (typically), when targeting functions in 3rd-party code such as the Windows API for instance (which is almost entirely based on "stdcall", though "FunctionTraits" isn't Windows specific), these functions won't be detected by other implementations. "FunctionTraits" however detects all the mainstream calling conventions. It provides traits info about every (mainstream) aspect of any function within the limitations of what C++ currently supports (or easily supports). You can currently obtain the following information:

1. `Return type` - Function's return type and/or its name as a WYSIWYG string (std::basic_string_view)
2. `Argument count` - Number of (non-variadic) arguments in the function (formally called "arity")
3. `Arguments` - The type of any (non-variadic) function argument (based on its zero-based index in the parameter list), and/or its name as a WYSIWYG string (std::basic_string_view). You can also obtain the "std::tuple" containing all arguments and iterate them using the "ForEachArg" function template (though rarely required by most in practice - targeting specific function arguments by their index is far more common). See "Looping through all function arguments" later on for an example (the [demo](https://godbolt.org/z/PPPshYrh7) link cited at the top of this document also uses it).
4. `cv-qualifers` - Whether a non-static member function is declared with the "const" and/or "volatile" cv-qualifiers
5. `ref-qualifiers` - Whether a non-static member function is declared with the "&" or "&&" ref-qualifier
6. `noexcept` - Whether a function is declared with "noexcept"
7. `Calling convention` - The function's calling convention (cdecl, stdcall, etc.). As previously emphasized, all other implementations I've seen can detect functions with the default calling convention only (cdecl for free functions for instance).
10. `class/struct type` - The class/struct associated with a non-static member function (including functors), and/or its name as a WYSIWYG string (std::basic_string_view)
11. `Function's classification` - Whether the function is a free function (which for our purposes includes static member functions), a non-static member function, or a functor

The above covers almost all traits most programmers will ever be interested in. Other possible traits such as whether a function is a static member function, whether a non-static member function is virtual (and/or the "override" keyword is in effect), etc., are not available in this release, normally due to limitations in the language itself (either impossible or difficult/unwieldly to implement in current releases of C++). They may be available in a future version however if C++ better supports it.

Note that this implementation is well researched and very easy to use. It supports all mainstream function types you'll likely ever want to work with normally. This includes the following (just pass one of these types as the "F" template parameter of "FunctionTraits" or any of its helper aliases - details to follow):

1. Free functions which for our purposes also includes static member functions (in either case referring to the function's actual C++ type - pointers and references to free functions are also supported via 2 and 3 below)
2. Pointers and references to free functions
3. References to pointers to free functions
4. Pointers to non-static member functions
5. References to pointers to non-static member functions (note that C++ doesn't support references to non-static member functions, only pointers),
6. Non-overloaded functors (i.e., functors with a single "operator()" member only, otherwise which overload to target becomes ambiguous - if present then you'll need to target the specific overload you're interested in using 4 or 5 above instead).

Note that the code also incorporates concepts when targeting C++20 or later or "static_assert" for C++17 (again, earlier versions aren't supported). In either case this will trap invalid function types with cleaner error messages at compile-time.

Given the following example for instance, you can quickly determine the traits for "MyClass::SomeFunc" seen below using the easier helper aliases for "FunctionTraits" in this example (opposed to using "FunctionTraits" directly - more on this later, including the complete list of all aliases since only the ones most are typically interested in are shown below):

``` C++
// Everything declared in this namespace
using namespace StdExt;

class MyClass
{
public:
    ////////////////////////////////////////////////////
    // Function we'll be targeting below (note that it
    // need not be defined to use "FunctionTraits")
    ////////////////////////////////////////////////////
    int SomeFunc(const std::string &, float);
};

// Function we're targeting (passed to all aliases below)
using F = decltype(&MyClass::SomeFunc);

// Function's return type (int)
using SomeFuncReturnType_t = ReturnType_t<F>;

// Number of arguments (2)
constexpr std::size_t argCount = ArgCount_v<F>;

// Type of the function's (zero-based) 2nd arg (float)
using Arg2Type_t = ArgType_t<F, 1>;

///////////////////////////////////////////////////////////////////
// Name of type just above returned as a "std::basic_string_view"
// (so literally "float", quotes not included)
///////////////////////////////////////////////////////////////////
constexpr auto Arg2TypeName = ArgTypeName_v<F, 1>;

// Etc. (see "Helper aliases" further below for the complete list)
```

### Usage (GCC or compatible, Clang, Microsoft and Intel compilers only - C++17 and later)
To use "FunctionTraits", simply add both "TypeTraits.h" and "CompilerVersions.h" to your code and then #include "TypeTraits.h" wherever you require it (all code is declared in namespace "StdExt"). Note that you need not explicitly #include "CompilerVersions.h" unless you wish to use it independently of "TypeTraits.h", since "TypeTraits.h" itself #includes it as a dependency ("CompilerVersions.h" simply declares various #defined constants used to identify the version of C++ you're using, and a few other compiler-related declarations - you're free to use these in your own code as well if you wish). The struct (template) "FunctionTraits" is then immediately available for use (see "Technique 1 of 2" below), though you'll usually rely on its helper aliases instead (see "Technique 2 of 2" below). Note that both files above have no platform-specific dependencies, except when targeting MSFT, where the native MSFT header <tchar.h> is expected to be in the usual #include search path (and it normally will be on MSFT platforms). Otherwise they rely on the C++ standard headers only which are therefore (also) expected to be in the usual search path on your platform.

Once you've #included "TypeTraits.h", there are two ways to use the "FunctionTraits" class. You can either use "FunctionTraits" directly like so (but with verbose syntax that technique 2 below eliminates):

### Technique 1 of 2 - Using "FunctionTraits" directly (not usually recommended)

``` C++
////////////////////////////////////////////////////////////////////////////
// Free function whose traits you wish to retrieve (note that static class
// functions are also considered "free" functions). Pointers and references
// to free functions are also supported (including references to such
// pointers though not references to references which C++ itself doesn't
// support). Pointers to non-static member functions are also suported,
// including references to such pointers (though references to non-static
// member functions aren't supported in C++ itself). Functors are also
// supported.
////////////////////////////////////////////////////////////////////////////
float SomeFunction(const std::string &, double, int);

// Type of the above function (but see alternate syntax just below)
using F = decltype(SomeFunction);

///////////////////////////////////////////////
// Same as above but using a function pointer
// (function references also supported)
///////////////////////////////////////////////
// using F = decltype(&SomeFunction);

///////////////////////////////////////////////////////////
// Also works but using a reference to a function pointer
// (though the following syntax is rare in practice but
// more mainstream syntax may exist in your own code,
// though references to pointers are usually rare outside
// of templates taking references to arbitrary types, so
// if passed a function pointer then the template winds up
// taking a reference to a pointer to a function)
///////////////////////////////////////////////////////////
//using F = decltype(&SomeFunction) &;

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

/////////////////////////////////////////////////////////
// Retrieve type of the function's 3rd arg (an "int").
// Arg's index is zero-based, so passing 2 here (to
// target the 3rd arg)). Note that this (mandatory C++)
// syntax is ugly however. A much cleaner alias for this
// exists as described in the next section.
/////////////////////////////////////////////////////////
using Arg3Type_t = typename SomeFuncTraits::template Args<2>::Type;

////////////////////////////////////////////////////////////////
// Type of the function's 3rd arg (same as above) but returned
// as a "std::basic_string_view" (so literally returns "int",
// quotes not included). Again, a cleaner alias exists for this
// as described in the next section. Note that the following
// variable template, "TypeName_v", can be used to get the
// (WYSIWYG) name for any type, not just types associated with
// "FunctionTraits".
////////////////////////////////////////////////////////////////
constexpr auto arg3TypeName = TypeName_v<Arg3Type_t>; 

```

### Technique 2 of 2 - Using the helper aliases instead of "FunctionTraits" directly (recommended)

Alternatively, instead of using "FunctionTraits" directly (technique 1 of 2 above), you can rely on the second technique just below instead, which is normally much cleaner (and you should usually use it). As seen in the first technique above, relying on "FunctionTraits" directly can result in verbose syntax. For instance, due to the syntax of C++ itself, accessing the type of a given arg is ugly because you have to apply both the "typename" and "template" keywords (see example above). A helper alias therefore exists not only for this, but for every member of "FunctionTraits". Therefore, instead of relying on "FunctionTraits" directly as seen in the above examples, you can rely on the helper aliases instead. They're easier and cleaner, making the job of extracting a function's traits a breeze. 

Note that two sets of aliases exist, one taking a "FunctionTraits" template arg, and a corresponding alias taking a function template arg (just pass your function's type). The latter aliases simply create a "FunctionTraits" for you from the function type you pass and then defer to the corresponding "FunctionTraits" alias. The "FunctionTraits" aliases are rarely used directly in practice however so they're not documented here. Only the aliases taking a function template arg are therefore documented below. Most users will rely on these almost exclusively, though you can always quickly inspect the code itself if you ever want to use the "FunctionTraits" aliases directly (they're just thin wrappers around the members of "FunctionTraitsBase" which all "FunctionTraits" specializations inherit from). Here's the same example seen in "Technique 1" above but using these aliases instead (though without repeating most of the comments seen in "Technique 1"):

``` C++
/////////////////////////////////////////////////////////
// Free function whose traits you wish to retrieve (see
// comments in corresponding example in "Technique 1"
// above)
/////////////////////////////////////////////////////////
float SomeFunction(const std::string &, double, int);

///////////////////////////////////////////////////
// Type of the above function (also see alternate
// syntax in "Technique 1" above).
///////////////////////////////////////////////////
using F = decltype(SomeFunction);

/////////////////////////////////////////////////////////
// Retrieve the function's return type (a "float") but
// using the cleaner helper alias this time (instead of
// "FunctionTraits" directly)
/////////////////////////////////////////////////////////
using SomeFuncReturnType_t = ReturnType_t<F>;

//////////////////////////////////////////////////////////////
// Retrieve type of the function's 3rd arg (an "int") but
// again, using the cleaner helper alias this time (instead
// of "FunctionTraits" directly)
//////////////////////////////////////////////////////////////
using Arg3Type_t = ArgType_t<F, 2>;

////////////////////////////////////////////////////////////////
// Type of the function's 3rd arg (same as above) but returned
// as a "std::basic_string_view" (so literally returns "int",
// quotes not included). Again, uses the easier alias this time
// instead of "FunctionTraits" directly.
////////////////////////////////////////////////////////////////
constexpr auto arg3TypeName = ArgTypeName_v<F, 2>;

```
### Looping through all function arguments
You can even loop through all arguments using the helper function template "ForEachArg". The following
example assumes C++20 or later for the lambda template seen below (lambda templates aren't available
until C++20 or later), though if targeting C++17 you can easily replace it with your own functor instead
(the "operator()" member in your functor needs to be a template however, with the same template args
seen in the lambda below and the same code). For complete details on "ForEachArg()", see the comments
preceding its declaration in "TypeTraits.h".
``` C++
//////////////////////////////////////////////////////////////
// Lambda that will be invoked just below, once for each arg
// in function "F" (where template arg "I" is the zero-based
// "Ith" argument in function "F"). Note that lambda templates
// are supported in C++20 and later only. For C++17 (earlier
// versions aren't supported), you need to roll your own
// functor template instead (with a template version of
// "operator()" equivalent to this lambda)
//////////////////////////////////////////////////////////////
const auto displayArgType = []<std::size_t I, typename ArgTypeT>()
                            {
                                //////////////////////////////////////////////////
                                // Display the type of the (zero-based) "Ith" arg
                                // in function "F" (where template arg "I" just
                                // above stores this index though we don't use it
                                // for anything in this example). Note that we
                                // never come through here if function "F" has no
                                // args (or just variadic args only - variadic
                                // args aren't processed).
                                //////////////////////////////////////////////////
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
// invoking "displayArg" above for each.
////////////////////////////////////////////////
ForEachArg<F>(displayArgType);
```

### Helper aliases (complete, alphabetical list)
The following provides a complete (alphabetical) list of all helper aliases. Note that each is a template taking a single template arg called "F" as the 1st (and normally only) template arg, representing the function's type you're targeting (again, supports the actual C++ function type, pointers and references to functions, references to pointers to functions,
and functors). Only the "ArgType_t" and "ArgTypeName_v" templates take an extra template arg (of type "std::size_t"), indicating the zero-based index of the function argument you're targeting in "F" (and "MemberFunctionRefQualifierName_v" also takes an extra template arg as described below). Note that for brevity, template arg "F" isn't shown below, just the template's name. 

Lastly, please note that each template below simply wraps the corresponding member of the "FunctionTraits" struct itself
(or indirectly targets it). As previously described, you can access "FunctionTraits" directly (see its "FunctionTraitsBase" base class - all members are stored there), or using any of its helper aliases (all taking a "FunctionTraitsT" template arg not documented here), but the following aliases taking a function template arg "F" are normally easier (see these in "Technique 2 of 2" earlier). The syntax for acessing "FunctionTraits" directly (or using any of its helper aliases) is therefore not shown in detail in this document (just the earlier examples only). You can simply review its "FunctionTraitsBase" base class in "TypeTraits.h" if required (or its own aliases). Every member of the latter struct is public and the corresponding alias taking a function template arg "F" is described below, including some additional helper aliases not available when accessing "FunctionTraits" directly. These aliases should normally be relied on since they're cleaner.

Name                              | Description                                                                                              |
----------------------------------|----------------------------------------------------------------------------------------------------------|
`ArgCount_v`                      | "std::size_t" variable storing the number of arguments in "F" *not* including variadic arguments if any. Will be zero if function "F" has no (non-variadic) args. Note that this count is formally called "arity" but this alias is given a more user-friendly name.|
`ArgType_t`                       | Yields the type of the "Ith" arg in function "F", where "I" is in the range 0 to the number of (non-variadic) arguments in "F" minus 1 (see "ArgCount_v" variable just above). Pass "I" as the (zero-based) 2nd template arg (see earlier examples). Note that if "I" is greater than or equal to the number of args in "F" (see "ArgCount_v" just above), then a compiler error will occur (so if "F" has no non-variadic args whatsoever, a compiler error will always occur, even if passing zero).|
`ArgTypeName_v`                   | Same as "ArgType_t" just above but returns this as a (WYSIWYG) string (of type "std::basic_string_view"). A float would therefore be (literally) returned as "float" for instance (quotes not included).|
`ArgTypes_t`                      | Returns a "std::tuple" representing all non-variadic argument types in "F". Rarely required in practice however since you'll usually rely on the "ArgType_t" or "ArgTypeName_v" alias to retrieve the type of a specific argument (see these above). If you require the "std::tuple" that stores all (non-variadic) argument types, then it's typically (usually) because you want to iterate all of them (say, to process the type of every argument in a loop). If you require this, then you can use the "ForEachArg" helper function (template) further below. See this in "TypeTraits.h" for a demo (or for a complete program, see the [demo ](https://godbolt.org/z/PPPshYrh7) link cited at the top of this document).|
`CallingConvention_v`             | Calling convention of "F" returned as a "CallingConvention" enumerator (declared in "TypeTraits.h"). Calling conventions include "Cdecl", "Stdcall", etc. (note that functions with calling conventions not seen in this enumerator are not supported, but all mainstream calling conventions are). Also please note that compilers will sometimes change the calling convention declared on your functions to the "Cdecl" calling convention depending on the compiler options in effect at the time (in particular when compiling for 64 bits opposed to 32 bits). In this case the calling convention on your function is ignored and "CallingConvention_v" will correctly return the "Cdecl" calling convention (since that's what the compiler actually used).|
`CallingConventionName_v`         | Same as "CallingConvention_v" just above but returns this as a (WYSIWYG) string (of type "std::basic_string_view").|
`ForEachArg`                      | Not a traits alias (unlike all others in this table), but a helper function template you can use to iterate all arguments for function "F" if required (though rare in practice since you'll usually rely on the "ArgType_t" or "ArgTypeName_v" alias to retrieve the type of a specific argument - see these above). See "Looping through all function arguments" earlier for an example, as well as the declaration of "ForEachArg" in "TypeTraits.h" for full details (or for a complete program that also uses it, see the [demo](https://godbolt.org/z/PPPshYrh7) link cited at this top of this document).
`FunctionType_t`                  | Type of the full signature of the function (its complete type which is normally identical to template arg "F" itself, or for functors the signature of its "operator() member").|
`FunctionTypeName_v`              | Same as "FunctionType_t" just above but returns this as a (WYSIWYG) string (of type "std::basic_string_view").|
`IsFunctor_v`                     | "bool" variable set to "true" if "F" is a functor (the functor's class/struct was passed for "F") or "false" otherwise|
`IsMemberFunction_v`              | "bool" variable set to "true" if "F" is a non-static member function (including when "F" is a functor), or "false" otherwise. Note that you may need to invoke this before accessing the following aliases. Since the following are applicable to non-static member functions only, if you don't know whether "F" is a non-static member function ahead of time (or a functor), then you should normally call "IsMemberFunction_v" to determine this first. If it's "false" then "F" is a free function (which includes static member functions), so a call to any of the following will result in default values being returned that aren't applicable to free functions (so you shouldn't normally invoke them unless you're ok with the default values they return for free functions):<br /><br />- MemberFunctionClass_t<br />- MemberFunctionClassName_v<br />- IsMemberFunctionConst_v<br />- IsMemberFunctionVolatile_v<br />- MemberFunctionRefQualifier_v<br />- MemberFunctionRefQualifierName_v|
`IsMemberFunctionConst_v`         | "bool" variable applicable only if "F" is a non-static member function (or a functor). Set to "true" if the function is "const" (declared with the "const" keyword) or "false" otherwise. Always "false" for free functions including static member functions (not applicable to either). You may therefore wish to invoke "IsMemberFunction_v" to detect if "F" is in fact a non-static member function (or functor) before using this trait (see "IsMemberFunction_v" above).|
`IsMemberFunctionVolatile_v`      | "bool" variable applicable only if "F" is a non-static member function  (or a functor). Set to "true" if the function is "volatile" (declared with the "volatile" keyword) or "false" otherwise. Always "false" for free functions including static member functions (not applicable to either). You may therefore wish to invoke "IsMemberFunction_v" to detect if "F" is in fact a non-static member function (or functor) before using this trait (see "IsMemberFunction_v" above).|
`IsNoexcept_v`                    | "bool" variable set to "true" if the function is declared as "noexcept" or "false" otherwise (always false if "noexcept" specifier is absent in the function, otherwise, if present then it evaluates to "true" if no bool expression is present in the "noexcept" specifier, or the result of the bool expression otherwise - WYSIWYG).|
`IsVariadic_v`                    | "bool" variable set to true if "F" is a variadic function (last arg of "F" is "...") or false otherwise.|
`MemberFunctionClass_t`           | If "F" is a non-static member function (or a functor), yields the type of the class (or struct) that declares that function (same as "F" itself if "F" is a functor). Always "void" otherwise (for free functions including static member functions). You may therefore wish to invoke "IsMemberFunction_v" to detect if "F" is in fact a non-static member function (or functor) before applying this trait (see "IsMemberFunction_v" above).|
`MemberFunctionClassName_v`       | Same as "MemberFunctionClass_t" just above but returns this as a (WYSIWYG) string (of type "std::basic_string_view").|
`MemberFunctionRefQualifier_v`    | "RefQualifier" variable, a proprietary enumerator in "TypeTraits.h" applicable only if "F" is a non-static member function (or a functor). Set to "RefQualifier::LValue" if the function is declared with the "&" reference qualifier, "RefQualifier::RValue" if the function is declared with the "&&" reference qualifier, or "RefQualifier::None" if neither is present (usually the case for most non-static member functions in practice, and always the case for free functions including static member functions since it doesn't apply to either). You may therefore wish to invoke "IsMemberFunction_v" to detect if "F" is in fact a non-static member function (or functor) before applying this trait (see "IsMemberFunction_v" above).|
`MemberFunctionRefQualifierName_v`| Same as "MemberFunctionRefQualifier_v" just above but returns this as a (WYSIWYG) string (of type "std::basic_string_view"). Note that this template also takes an extra template arg besides function "F", a "bool" called "UseAmpersands", indicating whether the returned string should be returned as "&" or "&&" (if the function is declared with an "&" or "&&" reference qualifier respectively), or "LValue" or "RValue" otherwise. Defaults to "true" if not specified (returns "&" or "&&" by default). Not applicable however if no reference qualifers are present ("UseAmpersands" template arg is ignored).|
`ReturnType_t`                    | Type of the return type for  function "F".|
`ReturnTypeName_v`                | Same as "ReturnType_t" just above but returns this as a (WYSIWYG) string (of type "std::basic_string_view").|

### "FunctionTraits" design considerations (for those who care - not required reading)
Note that "FunctionTraits" is not a canonical C++ "traits" class that would likely be considered for inclusion in the C++ standard itself. While it could be with some fairly easy-to-implement changes and (rarely-used) additions (read on), it wasn't designed for this. It was designed for practical, real-world usage instead. For instance, template arg "F" of "FunctionTraits" and its helper aliases isn't confined to pure C++ function types. As described in the docs above, you can also pass pointers to functions, references to functions, and references to pointers to functions (in all cases referring to their types). Note that reference types to non-static member function pointers aren't supported however because they're not supported by C++ itself.

While I'm not a member of the C++ committee, it seems very likely that a "canonical" implementation for inclusion in the standard itself likely wouldn't support pointer and reference types however. It would likely support raw function types only (not pointers or references to functions). In the real world however programmers are often working with pointers to functions, not the actual function type itself, especially for non-static member functions when calling them that way. In fact, for non-static member functions, almost nobody in the real world ever works with the actual function type of a non-static member function. I've been programming in C++ for decades and don't recall a single time I've ever worked with the actual type of one. You work with pointers to non-static member functions instead when you need to, such as **void (YourClass::*)()**. Even for free functions (which for our purposes also includes static member functions), since the name of a free function decays into a pointer to the function in most expressions, you often find yourself working with a pointer to such functions, not the actual function type (though you do work with the actual function type sometimes, unlike for non-static member functions which is extremely rare in the real world). Supporting pointers and references to functions therefore just make things easier even if some consider it unclean (I don't).

Note that other practical issues also exist. For instance, there are currently no "add/remove" trait structs in this implementation, which would allow you to add and/or remove any part of a function, say, if you want to add the "const" cv-qualifier to a non-static member function's type for instance (yielding the same type but with the "const" cv-qualifier added if not already present). Such traits would in fact be very easy to add, but I've neglected to do so for now since again, very few people in the real world ever need to do this. In almost all real-world situations people need a function's traits to extract information about the function, not add and/or remove parts of it. Yes, some might, but it would be very rare in practice so as a free contribution to the programming community, I'm interested in targeting what most (almost all) developers normally need, not worrying about the tiny minority who might need to modify a function's type (via add/remove traits). If I was selling "FunctionTraits" or trying to get it included in the C++ standard itself then things would be different. The traits are designed for ease-of-use and practicality however (by most developers), not to enter a C++ standards contest (or meet the needs of a tiny minority of developers who require add/remove traits). In real life most developers will likely use these traits to retrieve a function's parameter types and/or the number of parameters and/or the return type in most cases, and/or whether it's declared as "noexcept" and for non-static member functions, whether it has the "const" cv-qualifier (or the few other remaining traits). A practical traits struct should therefore make this very easy, which also means allowing pointers and references IMHO, without having to manually remove them first (which would be required if such a traits struct was designed to handle pure C++ function types only, not pointers and references to function types). It's just easier and more flexible to use it this way (syntatically cleaner). Some would argue it's not a clean approach but I disagree. It may not be consistent with how things are normally done in the standard itself (again, it would likely handle raw function types only), but it better deals with the real-world needs of most developers IMHO (so it's "clean" because its syntax is cleanly designed to support that).

Lastly, note that "FunctionTraits" doesn't support the so-called "[abominable function types](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html)". These are simply non-static member function types (not pointers to such types but the actual raw C++ type) with cv ("const" and/or "volatile") and/or reference qualifiers (& and &&). For full details, see the comments preceding "Private::IsFreeFunction" in the "TypeTraits.h" header. Abominable functions are arguably a flawed feature of the C++ type system, and a wider discussion of them isn't warranted here. The bottom line is that most programmers will never encounter them throughout their entire career, and for practical reasons (not discussed here), their lack of support by "FunctionTraits" makes its design cleaner and easier to use. "FunctionTraits" will therefore reject such functions via a "static_assert". Instead, non-static member functions are always handled using mainstream pointer-to-member function syntax. Raw C++ function types are always treated as free functions by "FunctionTraits" (which includes static member functions for our purposes), so if you pass a raw C++ function type (i.e., an actual function type and not a pointer or reference to such a type), no cv or ref qualifiers are allowed on that type (i.e., abominable functions aren't supported), since free functions don't have such qualifiers. They're only legal in C++ for non-static member functions. Passing an abominable function is therefore consideered illegal by "FunctionTraits" even though it's a valid C++ function type (but they're "abominable" nevertheless so we don't support them because the design is cleaner and easier to use when we don't - in the real-world almost nobody will care).