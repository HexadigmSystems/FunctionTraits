# FunctionTraits
## C++ function traits library (single header-only) for retrieving info about any function (arg types, arg count, return type, etc.). Clean and easy-to-use, the most "complete" implementation on the web.

See [here](https://godbolt.org/z/PYsTYbafb) for a complete working example (demo that displays all available traits for various sample functions - for those who want to get right to the code). Also see usage examples further below.

"FunctionTraits" is a lightweight C++ traits struct (template) that allows you to quickly and easily determine the traits of any function at compile-time, such as argument types, number of arguments, return type, etc. (for C++17 and later). It's a natural extension to the C++ standard itself, whose [<type\_traits>](https://en.cppreference.com/w/cpp/header/type_traits) header offers almost no support for handling function traits, save for [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) and [std::is\_member\_function\_pointer](https://en.cppreference.com/w/cpp/types/is_member_function_pointer) (and one or two other borderline cases). It's also a "complete" implementation in the sense that it handles (detects) all mainstream function syntax unlike any other implementation you'll normally find at this writing, which usually fail to address one or more issues (including the Boost implementation - read on). Many (actually most) of these implementations are just hacks or solutions quickly developed on-the-fly to answer someone's question on [stackoverflow.com](https://stackoverflow.com/) for instance. Only a small handful are more complete and far fewer are reasonably complete, though still missing at least one or two features, in particular calling convention support (again, read on).

Some have even written articles on the subject but still don't address various issues. Some of these issues are obvious (or should be), such as failing to detect functions declared as "*noexcept*" (part of a function's type since C++17), or "*const*" and/or "*volatile*" non-static member functions (usually "*const*" in practice), or variadic functions (those whose arguments end with "..."), or function pointers with cv-qualifiers (the pointers themselves are "*const*" and/or "*volatile*"), among other things (often resulting in a lot of cryptic compiler errors).

Some are less obvious, like failing to detect non-static [Member functions with ref-qualifiers](https://en.cppreference.com/w/cpp/language/member_functions#Member_functions_with_ref-qualifier) (rarely used but still required for completeness), or much more prominently (and important usually), functions with calling conventions other than the default (usually "*cdecl*"), which isn't addressed in the C++ standard itself (and a bit more tricky to implement than it first appears as far as creating a functions traits class is concerned). Without (mainstream) calling convention support however, all functions in the entire Microsoft Windows API would fail to be detected for instance (when targeting 32 bit apps), since it's almost entirely based on the "*stdcall*" calling convention. Only the one from Boost ("*boost::callable\_traits*") made a half-hearted attempt to handle calling conventions but it's not officially active (more on this later), it targets the Microsoft C++ compiler only (and in a limited capacity only - again, more on this later), and the feature is even discouraged by the author's own internal comments (referring to it to as "*likely broken*" and "*too much work to maintain*"). For all intents and purposes it's therefore not supported and no other implementation I'm aware of addresses calling conventions at all, even in the (very) few other (mostly) "complete" solutions for handling function traits that are out there (that I've ever come across). Again, the result is usually a lot of cryptic compiler errors or possibly incorrect behavior (including in the Boost version since its limited support for calling conventions is officially inactive).

"*FunctionTraits*" does address all these issues however. It provides traits info about every (mainstream) aspect of any function within the limitations of what C++ currently supports (or easily supports), and allows you to change every trait as well, where applicable (including function parameter types, which will be rarely used by most but no other implementation I've seen at this writing supports it, including the one from Boost).

## Where to go from here
If you wish to immediately get started, proceed to the [Usage](#usage) section now. If you wish to read a very short summary of C++ function types, proceed to the section just below ([C++ function types at a glance](#cppfunctiontypesataglance)). If you wish to see a brief description of the library's main templates, grouped so you can see which templates target each component of a [C++ function type](#cppfunctiontypeformat) (from return type to noexcept, as described in the section just below), see [Summary of templates in this library](#summaryoftemplatesinthislibrary). If you wish to see the complete documentation of all templates in the library (in alphabetical order categorized by read traits and write traits), see [Helper templates](#helpertemplates). Lastly, if you need to determine if an arbitrary member function exists (by name) with a specific function type (or set of traits) in any arbitrary class, see [Determining if a member function exists](#determiningifamemberfunctionexists).

<a name="CppFunctionTypesAtAGlance"></a>
## C++ function types at a glance

<a name="CppFunctionTypeFormat"></a>
#### C++ Function Type Format:

<pre><a href="#returntype">ReturnType</a> [<a href="#callingconvention">CallingConvention</a>] ([<a href="#args">Args</a>] [[,] <a href="#variadic">...</a>]) [<a href="#const">const</a>] [<a href="#volatile">volatile</a>] [<a href="#refqualifiers">&amp;|&amp;&amp;</a>] [<a href="#noexcept">noexcept</a>[(true_or_false_expression)]]</pre>

Note that function types in C++ are always of the general format seen just above, where items in square brackets are optional. This is what [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) in the C++ standard itself looks for to determine if a type qualifies as a function (though the function's "*CallingConvention*" seen above isn't addressed by the C++ standard itself - it's a platform-specific extension in most C++ implementations but still part of a function's type in all mainstream C++ compilers). Note that the above format is somewhat imprecise but sufficient for most purposes (the precise legalese is beyond the scope of this documentation). Each part of a function's type seen above will informally be referred to as a function type "*component*" throughout this documentation. You may click each "*component*" above to see the available templates in this library for targeting that specific component (the main purpose of this library, to read or update any function "*component*"), but consult the [Usage](#usage) section later on for full details (and the [Helper templates](#helpertemplates) section for the complete, fully-documented list of all templates in the library).

The format above is what actually qualifies as a function type in C++. In particular, pointers to functions, which experienced developers will already be very familiar with (including *non-static* member function pointers), references to functions though they are rarely used by most in practice (noting that direct references to *non-static* member functions aren't legal in C++, only references to *non-static* member function *pointers* are legal), functors (classes with a function call operator "*operator()"* including lambdas which are just syntactic sugar for functors), and [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) (just a wrapper for a function), are *not* actual functions in their own right. They therefore don't qualify as functions and [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) will therefore return "*false*" for them (though the library itself handles them - see [Template arg "F"](#templateargF) later on). A pointer to a function for instance is still a pointer, not a function, and its type is therefore a pointer type in the C++ type system, not a function type (though pointers to functions are sometimes treated a bit differently than pointers to data objects - a *non-static* member function pointer for instance doesn't even satisfy [std::is\_pointer](https://en.cppreference.com/w/cpp/types/is_pointer) for example - it returns "*false*" for them so you need to rely on [std::is\_member\_function\_pointer](https://en.cppreference.com/w/cpp/types/is_member_function_pointer) instead). Actual C++ functions have their own type in the C++ type system and that type is the format seen above, not pointer types or any of the others mentioned above. Only types that satisfy [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) qualify as actual functions in C++ (though [abominable functions](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html), which most developers have likely never heard of, are a special breed).

<a name="RawFunctionTypes"></a>
#### Raw function types

The term "*raw*" will be applied throughout this documentation to refer to an actual C++ function type, i.e., a C++ type that conforms to the [C++ Function Type Format](#cppfunctiontypeformat) seen in the previous section (and therefore satisfies [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function)), opposed to any other "*function*" type supported by the library (function pointers and references to such pointers, function references, functors, and [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specializations - none of these qualify as "*raw*" function types in their own right but indirectly refer to a "*raw*" function so you're free to pass them as well). No matter which type you're working with though, you can obtain its "*raw*" function type (if required) using templates [FunctionRawType\_t](#functionrawtype_t) and even its string equivalent (for display purposes) using [FunctionRawTypeName\_v](#functionrawtypename_v) (no matter what supported type you pass even if it's already a "*raw*" function type - in the latter case it will just be returned as-is). The library's templates as summarized in the section just below are therefore available to target each component comprising a [C++ function type](#cppfunctiontypeformat) regardless of which type you pass (whether the actual "*raw*" function type itself, a pointer (type) to a function, a functor type, etc.). Unless noted otherwise, the templates always target the "*raw*" function type of whatever you pass however, so if you pass an actual "*raw*" function type then it's already a "*raw*" function type so it's directly targeted, but if you pass a function pointer type for instance then the "*raw*" function type is the function it points to (which is then targeted). If you pass a *non-overloaded* functor type (note that *overloaded* functor types can't be directly passed since the specific "*operator()*" to target would be ambiguous), then the type of its *non-overloaded* "*operator()*" member is the "*raw*" function type (which is then targeted). If you pass a [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specialization then the function type it was specialized on is the "*raw*" function type (which is then targeted), etc. No matter which type you pass however, the library's templates can then be used to read or update any of the components comprising the "*raw*" function type itself (again, any of the components seen under [C++ Function Type Format](#cppfunctiontypeformat) in the previous section).

<a name="summaryoftemplatesinthislibrary"></a>
## Summary of templates in this library

The following provides a brief summary of the library's main templates grouped by each component comprising a C++ function type, in left-to-right order as seen under [C++ Function Type Format](#cppfunctiontypeformat) (so you can quickly see which templates target each component of a C++ function type, from return type to noexcept). Note that several other useful templates also exist in the library, but those below specifically target the actual components comprising a C++ function type (unless otherwise noted). They comprise the bulk of the library's templates (though a few other miscellaneous templates also exist). For the library's complete (fully-documented) list (note that the descriptions below provide a brief summary of the main templates only), see [Helper templates](#helpertemplates) (though each template below is hyperlinked to its full documentation in the latter section). See [Usage](#usage) section to get started.

1. <a name="ReturnType"></a>***ReturnType***

    1. [ReturnType\_t](#returntype_t) - Type alias for a function's return type
    2. [ReturnTypeName\_v](#returntypename_v) - Same as template just above but returns this as a WYSIWYG string (so a float return type for instance will literally be returned as "float")
    3. [IsReturnTypeSame\_v](#isreturntypesame_v) - Compares the return type in a function with the type you pass, returning "*true*" if they're the same or "*false*" otherwise
    4. [IsReturnTypeVoid\_v](#isreturntypevoid_v) - Returns "*true*" if a function's return type is (optionally cv-qualified) *void*" or "*false*" otherwise
    5. [IsReturnTypeMatch\_v](#isreturntypematch_v) - Compares the return type of the two function types you pass, returning "*true*" if they're the same or "*false*" otherwise
    6. [ReplaceReturnType\_t](#replacereturntype_t) - Type alias for a function after replacing its return type with the new return type you pass

2. <a name="CallingConvention"></a>***CallingConvention***

    Note that as briefly described earlier, all other function traits libraries (at the time of this writing) can detect functions with the default calling convention only (usually "*cdecl*" or possibly "*thiscall*" for non-static member functions depending on the platform). Only "*callable\_traits*" from Boost was partially designed to handle calling conventions but for the Microsoft C++ compiler only, though again, it's officially unavailable (the documentation makes this clear and doesn't describe how to turn it on), its support is limited even if it is turned on (by intrepid developers who wish to inspect the code - it won't handle certain syntax, it won't compile for 64 bit builds, etc.), and its use is discouraged by the author's own internal comments (see [Why choose this library](#whychoosethislibrary)).

    Note that calling conventions aren't addressed in the C++ standard itself and are platform specific. As a result, there are numerous calling conventions in the wild that are specific to one platform only and it's therefore inherently difficult for any function traits library to support them all. For now "*FunctionTraits*" currently supports the most mainstream calling conventions which will usually meet the needs of most users ("*cdecl*", "*stdcall*", "*fastcall*", "*vectorcall*", though GCC itself doesn't support "*vectorcall*", "*thiscall*", and on Clang and Intel platforms only, "*regcall*"). The actual calling convention itself for these is platform specific however, such as ```__cdecl``` on Microsoft vs ```__attribute__((cdecl))``` on all other supported compilers at this writing. An enumerator in the library, "*CallingConvention*", is therefore used to handle them in all templates below (see each template below for details), but if you require the actual platform specific calling convention itself then the following macros may be used (e.g., STDEXT\_CC\_CDECL below resolves to ```__cdecl``` on Microsoft platforms and ```__attribute__((cdecl))``` on all other supported platforms but note that each macro is #defined only if the target platform supports it - if not then this is noted for each below):

    * STDEXT\_CC\_CDECL
    * STDEXT\_CC\_STDCALL
    * STDEXT\_CC\_FASTCALL
    * STDEXT\_CC\_VECTORCALL (not supported by GCC)
    * STDEXT\_CC\_THISCALL
    * STDEXT\_CC\_REGCALL (supported by Clang and Intel compilers only)

    The following templates are available for working with calling conventions:

    1. [CallingConvention\_v](#callingconvention_v) - Variable of enumerator type "*CallingConvention*". Returns the calling convention for any function ("*CallingConvention::Cdecl*", "*CallingConvention::Stdcall*", etc. corresponding to the *STDEXT\_CC\_\?* #defined constants described above).
    2. [CallingConventionName\_v](#callingconventionname_v) - Same as template just above but returns this as a WYSIWYG string  ("*cdecl*", "*stdcall*", etc. - always in lowercase, and the "*CallingConvention*" enumerator itself removed)
    3. [DefaultCallingConvention\_v](#defaultcallingconvention_v) - Variable of enumerator type "*CallingConvention*". Returns the default calling convention, i.e., the calling convention assigned by the compiler to an arbitrary function if none is explicitly specified in the function's type (see the *STDEXT\_CC\_\?* #defined constants described further above - the template returns the corresponding "*CallingConvention*" enumerator such as "*CallingConvention::Cdecl*", "*CallingConvention::Stdcall*", etc.).
    4. [DefaultCallingConventionName\_v](#defaultcallingconventionname_v) - Same as template just above but returns this as a WYSIWYG string ("*cdecl*", "*stdcall*", etc. - always in lowercase, and the "*CallingConvention*" enumerator itself removed)
    5. [ReplaceCallingConvention\_t](#replacecallingconvention_t) - Type alias for a function after replacing its calling convention with the calling convention you pass (as a "*CallingConvention*" enumerator). The function's calling convention is replaced with the actual platform specific calling convention corresponding to the enumerator you pass (such as ```__cdecl``` on Microsoft platforms or ```__attribute__((cdecl))``` on all other supported platforms at this writing - see *STDEXT\_CC\_\?* #defined constants described further above).

3. <a name="Args"></a>***Args***

    1. [ArgType\_t](#argtype_t) - Type of the (zero-based) "*I*th" argument in a function's argument list (pass "*I*" as required)
    2. [ArgTypeName\_v](#argtypename_v) - Same as template just above but returns this as a WYSIWYG string (so a *float* arg for instance will literally be returned as "*float*")
    3. [ArgTypes\_t](#argtypes_t) - Type alias for a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) specialized on all argument types in a function (so effectively the function's complete argument list). Rarely required in practice however since the library's other argument templates in this section will usually meet most needs (so accessing the [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) of all argument types returned by this template isn't typically required).
    4. [ArgCount\_v](#argcount_v) - Variable template returning the number of *non-variadic* arguments in a function (formally called "*arity*"), but to determine if the argument list is empty, don't check this for zero (0). Use [IsArgListEmpty\_v](#isarglistempty_v) instead (see this for details). To determine if a function has variadic args (ends with "..."), see [IsVariadic\_v](#isvariadic_v).
    5. [IsArgTypesSame\_v](#isargtypessame_v) - Compares the argument types in any function with the types (parameter pack) you pass, returning "*true*" if they're the same or "*false*" otherwise (i.e., allows you to check if a function's argument list consists entirely of the types you pass)
    6. [IsArgTypesSameTuple\_v](#isargtypessametuple_v) - Identical to template just above but allows you to pass the types to compare in a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of using a parameter pack
    7. [IsArgTypeSame\_v](#isargtypesame_v) - Compares the type of the (zero-based) "*I*th" argument in any function with the type you pass, returning "*true*" if they're the same or "*false*" otherwise (pass "*I*" as required)
    8. [IsArgTypesMatch\_v](#isargtypesmatch_v) - Compares the argument types of the two functions you pass, returning "*true*" if they're the same or "*false*" otherwise (i.e., allows you to check if a function's argument list matches the argument list of another function)
    9. [IsArgTypeMatch\_v](#isargtypematch_v) - Compares the type of the (zero-based) "*I*th" argument in one function with the corresponding argument in another, returning "*true*" if they're the same or "*false*" otherwise (i.e., allows you to check if a function's argument at index "*I*" matches the corresponding argument in another function - pass "*I*" as required)
    10. [IsArgListEmpty\_v](#isarglistempty_v) - Checks a function for an empty argument list (including no variadic args), returning "*true*" if so or "*false*" otherwise
    11. [ForEachArg](#foreacharg) - Iterates all function argument types and allows you to invoke a callback template functor for each
    12. [ArgsModify\_t](#argsmodify_t) - Type alias for a function after modifying (replacing) its argument types at the (zero-based) index you pass for the size you pass (the number of args to replace at that index), with new argument types you pass as a parameter pack (which replace the existing argument types you've targeted at the given index). Effectively allows you to replace any range of argument types with any other argument types. Note that all other argument modification templates in the library (those just below) are just special cases of this one (helper templates that ultimately defer to this one for common use cases). Note that while "*ArgsModify\_t*" itself is a one-stop template for performing any type of modification on a function's argument types, the others below should normally be relied on instead (when suitable for your needs - they are cleaner and easier than directly relying on "*ArgsModify\_t*" for the most common use cases but just defer back to "*ArgsModify\_t*" as noted).
    13. [ArgsModifyTuple\_t](#argsmodifytuple_t) - Identical to template just above but the new args are passed as a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of a parameter pack
    14. [ReplaceArgs\_t](#replaceargs_t) - Type alias for a function after replacing all its argument types with the new argument types you pass as a parameter pack (which becomes the new argument list). Just a special case however of [ArgsModify\_t](#argsmodify_t) and its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t) (see these for details).
    15. [ReplaceArgsTuple\_t](#replaceargstuple_t) - Identical to template just above but allows you to pass the args to replace using a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of a parameter pack
    16. [ReplaceArg\_t](#replacearg_t) - Type alias for a function after replacing its (zero-based) "*I*th" argument type with the new argument type you pass (effectively replacing the argument's existing type). Just a special case however of [ArgsModify\_t](#argsmodify_t) and its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t) (see these for details).
    17. [ArgsInsert\_t](#argsinsert_t) - Type alias for a function after inserting new argument types (passed as a parameter pack) into its existing arguments at the (zero-based) index you pass (so effectively inserts new arguments into the function). Just a special case however of [ArgsModify\_t](#argsmodify_t) and its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t) (see these for details). Also see [ArgsAppend\_t](#argsappend_t) and its cousin [ArgsAppendTuple\_t](#argsappendtuple_t) if you wish to append new arguments to the end of a function's existing arguments instead (just a special case of "*ArgsInsert\_t*" however, where the index to insert is always at the end).
    18. [ArgsInsertTuple\_t](#argsinserttuple_t) - Identical to template just above but the new args to be inserted are passed as a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of a parameter pack
    19. [ArgsAppend\_t](#argsappend_t) - Type alias for a function after appending new argument types (passed as a parameter pack) to the end of its existing arguments (so effectively appends new arguments to the end of the function's existing arguments). Just a special case however of [ArgsModify\_t](#argsmodify_t) and its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t) (see these for details).
    20. [ArgsAppendTuple\_t](#argsappendtuple_t) - Identical to template just above but the new args to be appended are passed as a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of a parameter pack
    21. [ArgsDelete\_t](#argsdelete_t) - Type alias for a function after deleting (removing) its arguments at the (zero-based) index you pass for the size you pass (i.e., the number of args to delete starting at the index you pass, so effectively removes those arguments from the function's existing arguments). Just a special case however of [ArgsModify\_t](#argsmodify_t) and its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t) (see these for details).
    22. [ArgsMakeVoid\_t](#argsmakevoid_t) - Type alias for a function after removing all its variadic and non-variadic args, if any, therefore setting its arg list to "*(void)*" (or identically just "*()*" which is more commonly used by most)

4. <a name="Variadic"></a>***Variadic args (...)***

    1. [IsVariadic\_v](#isvariadic_v) - "*bool*" variable set to "*true*" if a function has variadic args, i.e., its last arg is "...", or "*false*" otherwise
    2. [AddVariadicArgs\_t](#addvariadicargs_t) - Type alias for a function after adding "..." to the function, hence converting it to a variadic function (if it's not already)
    3. [RemoveVariadicArgs\_t](#removevariadicargs_t) - Type alias for a function after removing "..." from the function if present, hence reverting its type to a *non-variadic* function

5. <a name="Const"></a>***const***

    1. [IsFunctionConst\_v](#isfunctionconst_v) - "*bool*" variable set to "*true*" if a function is declared with the "*const*" cv-qualifier or "*false*" otherwise
    2. [FunctionAddConst\_t](#functionaddconst_t) - Type alias for a function after adding the "*const*" cv-qualifier to the function if not already present (also see [FunctionAddCV\_t](#functionaddcv_t) to add both cv-qualifiers)
    3. [FunctionRemoveConst\_t](#functionremoveconst_t) - Type alias for a function after removing the "*const*" cv-qualifier from the function if present (also see [FunctionRemoveCV\_t](#functionremovecv_t) to remove both cv-qualifiers)

6. <a name="Volatile"></a>***volatile***

    1. [IsFunctionVolatile\_v](#isfunctionvolatile_v) - "*bool*" variable set to "*true*" if a function is declared with the "*volatile*" cv-qualifier or "*false*" otherwise
    2. [FunctionAddVolatile\_t](#functionaddvolatile_t) - Type alias for a function after adding the "*volatile*" cv-qualifier to the function if not already present (also see [FunctionAddCV\_t](#functionaddcv_t) to add both cv-qualifiers)
    3. [FunctionRemoveVolatile\_t](#functionremovevolatile_t) - Type alias for a function after removing the "*volatile*" cv-qualifier from the function if present (also see [FunctionRemoveCV\_t](#functionremovecv_t) to remove both cv-qualifiers)

7. <a name="RefQualifiers"></a>***&|&& (ref-qualifiers)***

    1. [FunctionReference\_v](#functionreference_v) - Variable of enumerator type "*FunctionReference*" (declared in the library's header "*FunctionTraits.h*"). Returns a function's reference qualifier if any (&, && or none).
    2. [FunctionReferenceName\_v](#functionreferencename_v) - Same as template just above but returns this as a WYSIWYG string
    3. [FunctionAddLValueReference\_t](#functionaddlvaluereference_t) - Type alias for a function after adding the "*&*" ref-qualifier to the function if not already present
    4. [FunctionAddRValueReference\_t](#functionaddrvaluereference_t) - Type alias for a function after adding the "*&&*" ref-qualifier to the function if not already present
    5. [FunctionRemoveReference\_t](#functionremovereference_t) - Type alias for a function after removing the "&" or "&&" ref-qualifier from the function if present

8. <a name="Noexcept"></a>***noexcept***

    1. [IsNoExcept\_v](#isnoexcept_v) - "*bool*" variable set to "*true*" if a function is declared as "*noexcept*" or "*false*" otherwise
    2. [AddNoexcept\_t](#addnoexcept_t) - Type alias for a function after adding the "*noexcept*" specifier to the function if not already present
    3. [RemoveNoexcept\_t](#removenoexcept_t) - Type alias for a function after removing the "*noexcept*" specifier from the function if present

9. <a name="FunctionType"></a>***Function's type***

    The following templates return the type of the C++ function itself (corresponding to the function type you pass):

    1. [FunctionType\_t](#functiontype_t) - Type alias identical to the function type you pass (see [Template arg "F"](#templateargf)), unless it's a functor or a [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specialization, in which case it's a type alias for the function represented by the latter types instead (so in the case of a functor, the type of its "*operator()*" member is returned, and in the case of a [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specialization, the type of the function it was specialized on is returned). The alias therefore effectively just returns the function type corresponding to the type you pass (always [Template arg "F"](#templateargf) itself unless its a functor or [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specialization as noted).
    2. [FunctionTypeName\_v](#functiontypename_v) - Same as template just above but returns this as a WYSIWYG string
    3. [FunctionRawType\_t](#functionrawtype_t) - Type alias of the [raw](#rawfunctiontypes) (native C++) function corresponding to the function type you pass, assuming it's not already a [raw](#rawfunctiontypes) C++ function type (in which case it's just returned as-is). As described earlier in this document (see [C++ function types at a glance](#cppfunctiontypesataglance) for details), a [raw](#rawfunctiontypes) C++ function type is the (informal) term used by this library to refer to any type satisfying [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function). If you pass a pointer to a function for instance (its type), or a functor type, etc. (i.e., any supported function type that doesn't already satisfy [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function)), then the [raw](#rawfunctiontypes) type refers to the native C++ function that *does* satisfy [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) (so in the case of a function pointer for instance, the [raw](#rawfunctiontypes) type refers to the type of the function it points to after removing the pointer - in the case of a functor, the [raw](#rawfunctiontypes) type refers to the type of its "*operator()*" member, etc.). Again, see [C++ function types at a glance](#cppfunctiontypesataglance) for complete details on "raw" function types.
    4. [FunctionRawTypeName\_v](#functionrawtypename_v) - Same as template just above but returns this as a WYSIWYG string.

10. <a name="FunctionClassification"></a>***Function's classification***

    While not part of a C++ function type itself (see format [here](#cppfunctiontypeformat)), the following template returns the function's classification (not official in the C++ standard itself but as classified by this library):

    1. [FunctionClassification\_v](#functionclassification_v) - Variable of enumerator type "*FunctionClassification*" (see latter link) set to "*FunctionClassification::Free*" if the function type you pass is a free function (which includes static member functions), "*FunctionClassification::NonStaticMember*" if it's a non-static member function, or "*FunctionClassification::Abominable*" if it's an abominable function.

11. <a name="FunctionOrigin"></a>***Function's origin***

    While not part of a C++ function type itself (see format [here](#cppfunctiontypeformat)), the "*origin*" of a function is the term used by the library to refer to the origin of a function if the type you pass is not a [raw](#rawfunctiontypes) C++ function in its own right nor a pointer or reference to one (nor a reference to a pointer to one), but the function originates from the type you pass instead (read on):

    1. [FunctionOrigin\_v](#functionorigin_v) - Variable of enumerator type "*FunctionOrigin*" declared in the library's header ("*FunctionTraits.h*") indicating the origin of a function if the type you pass is not a [raw](#rawfunctiontypes) C++ function type nor a pointer or a reference to one (nor a reference to a pointer to one), but the *origin* of a function instead, such as when passing a functor type. A functor is not an actual function (nor a pointer or a reference to one nor a reference to a pointer to one), but the origin of a function instead, namely, its member function call "*operator()*" (the functor is its *origin*), so passing the functor's type to [FunctionOrigin\_v](#functionorigin_v) will therefore return "*FunctionOrigin::Functor*" (since the function itself originates from a functor). See template for full details.

12. ***Class type (applicable to non-static member functions)***

    While not part of a C++ function type itself (see format [here](#cppfunctiontypeformat)), the following templates target the class associated with a *non-static* member function including *non-static* functors (functors with a *non-static* "*operator()*" member) :
    1. [MemberFunctionClass\_t](#memberfunctionclass_t) - Returns the class type for any *non-static* member function pointer (or reference to such a pointer) or *non-static* functor (in the latter case, the actual class that declares its *non-static* "*operator()*" which might not be the actual class of the functor you pass if its "*operator()*" member is declared further up the class hierarchy - never applicable to functors with a *static* version of "*operator()*" however which were legalized in C++23). Always returns "*void*" otherwise (for all other supported function types in the library, none of which target *non-static* member functions so this template isn't applicable).
    2. [MemberFunctionClassName\_v](#memberfunctionclassname_v) - Same as template just above but returns this as a WYSIWYG string (always an empty string if if the function you pass isn't a *non-static* member function pointer or *non-static* functor)
    3. [ReplaceMemberFunctionClass\_t](#replacememberfunctionclass_t) - Type alias for a function after replacing its class with the one you pass, usually when targeting *non-static* member functions (though passing "*void*" for the class will remove its existing class instead, effectively converting it to a [raw](#rawfunctiontypes) function type). If passing a free function or [abominable function](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) however, converts it to a *non-static* member function pointer instead (as a member of the class you pass).

The above covers every function trait associated with a C++ function type (every component seen under [C++ Function Type Format](#cppfunctiontypeformat) may be read or modified). Other possible "traits" such as detecting whether a (named) *non-static* member function is declared "*virtual*", and/or whether the "*override*" keyword is in effect, etc., are not available in this release, normally due to limitations in the language itself (either impossible or difficult/unwieldy to implement in current releases of C++). Moreover, the library's function detection templates (see [Determining if a member function exists](#determiningifamemberfunctionexists)) therefore can't detect functions with the latter "traits". Such traits may be detectable in a future version however if C++ makes it possible to do so (in particular when reflection becomes available in C++26).

<a name="Usage"></a>
## Usage (C++17 and later: GCC[^1], Microsoft[^2], Clang[^3] and Intel[^4] compilers only)

### To use "*FunctionTraits*":

1. Add both "*FunctionTraits.h*" and "*CompilerVersions.h*" to your code and then *#include "FunctionTraits.h"* wherever you require it (an experimental module version is also now available - see [Module support in C++20 or later](#moduleusage)). All code is declared in namespace "*StdExt*". Note that you need not explicitly *#include "CompilerVersions.h"* unless you wish to use it independently of "*FunctionTraits.h*", since "*FunctionTraits.h*" itself #includes it as a dependency ("*CompilerVersions.h*" simply declares various #defined constants used to identify the version of C++ you're using, and a few other compiler-related declarations - they are not documented in this README.md file since they are used behind-the-scenes to support "*FunctionTraits*", but you're free to use them in your own code if you wish to inspect "*CompilerVersions.h*" for details). Note that both files above have no platform-specific dependencies except when targeting Microsoft, where the native Microsoft header *<tchar.h>* is expected to be in the usual #include search path (and it normally will be on Microsoft platforms). Otherwise they rely on the C++ standard headers only which are therefore (also) expected to be in the usual search path on your platform.
2. Simply use the template you're interested in. [Technique 1 of 2](#technique1of2) demonstrates this using the library's main template itself ("*FunctionTraits*", the struct template the entire library is built on), but you'll normally rely on its helper templates instead, which is demonstrated in [Technique 2 of 2](#technique2of2). The direct use of struct (template) "*FunctionTraits*" ([Technique 1 of 2](#technique1of2)) can therefore be ignored by most users (it's not required reading). See [Helper templates](#helpertemplates) for the library's complete documentation of all templates (alphabetical list of all templates grouped by [Read traits](#readtraits) and [Write traits](#writetraits) - each template is fully documented). See [Summary of templates in this library](#summaryoftemplatesinthislibrary) if you wish to see a brief summary of the library's main templates, grouped so you can quickly see which templates target each component of a [C++ function type](#cppfunctiontypeformat), from return type to noexcept (but each template there simply links to the [Helper templates](#helpertemplates) section which provides the complete documentation). For almost all templates, just pass the function type you're targeting as the first template arg, always called "*F*", and any other documented template args where applicable (often none). Template arg "*F*" can effectively be any function type in the C++ universe ([raw](#rawfunctiontypes) C++ function types, pointers and references to functions including references to function pointers, *non-overloaded* functors including lambdas, and [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specializations). The section just below, [Template arg "F"](#templateargf), describes "*F*" in detail but for most intents and purposes you can pass any C++ function type you normally deal with (so most can skim or even ignore the bullets in the section below unless you wish to review the details).

<a name="TemplateArgF"></a>
### Template arg "F"
Note that template arg "*F*" is the first (and often only) template arg of "*FunctionTraits*" and all its [Helper templates](#helpertemplates), and refers to the function's type which can be any of the following (effectively any type that refers to a function):

1. Any type satisfying [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function). Note that when such functions *don't* contain any function qualifiers ("*const*" and/or "*volatile*" and/or "*&*" or "*&&*"), they are classified as "*free*" functions by this library. If they do contain such qualifiers then they are classified as [abominable functions](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html). See [FunctionClassification\_v](#functionclassification_v) for details.
2. Optionally cv-qualified pointers to free functions (see 1 above for the definition of a "*free*" function), or references to such pointers (note that pointers and references to [abominable functions](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) aren't legal in C++)
3. References to free functions (see 1 above for the definition of a "*free*" function)
4. Optionally cv-qualified pointers to non-static member functions, or references to such pointers (note that direct references to non-static member functions aren't legal in C++, only references to non-static member function *pointers* are)
5. Optionally cv-qualified, *non-overloaded* functors or references to such functors (i.e., classes or references to classes with a single "*operator()*" member only, otherwise which overload to target becomes ambiguous - if overloaded then you'll need to rely on "*static\_cast*" to target the specific overload you're interested which will then be detected via 4 above instead, e.g., ```using F = decltype(static_cast<int (SomeFunctor::*)(double)>(&SomeFunctor::operator()));```). When passing functors for template arg "*F*", note that all templates in the library will target "*F::operator()*", not "*F*" itself (since "*F*" is just a class, not an actual function in its own right). Note that lambdas are also fully supported (just syntactic sugar for creating functors on-the-fly). Simply apply "*decltype*" to your lambda to retrieve its compiler-generated class type (officially known as its "*closure type*"), which you can then pass to "*FunctionTraits*" or any of its [Helper templates](#helpertemplates). Your lambda's function will then be targeted. Please note however that *generic* lambdas are *not* supported by passing their class type for technical reasons beyond the scope of this documentation (their class type can't be passed for "*F*"). They can still be accessed via 4 above however using some unconventional syntax described in "*FunctionTraits.h*" itself. Search the latter file for "*Example 3 (generic lambda)*" (quotes not included), which you'll find in the comments preceding the "*FunctionTraits*" specialization for functors (the comments for this example also provides additional technical details on this issue).
6. Optionally cv-qualified [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specializations or references to such specializations. While such specializations are also functors (they have an "*operator()*" member), they are *not* classified as functors by this library, hence the library's [FunctionOrigin\_v](#functionorigin_v) template will always return "*FunctionOrigin::StdFunction*" for them instead of "*FunctionOrigin::Functor*". Passing such specializations to any of the templates in the library therefore don't target the "*operator()*" member of [std::function](https://en.cppreference.com/w/cpp/utility/functional/function), but the template arg that was used to specialize it instead. This template arg is simply the function type that [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) is wrapping (what it expects for the callable functions you're using it for), which must always satisfy [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) *excluding* the following (as required by the language itself - the following aren't valid template args for passing to [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) even though some compilers might accept them):
    1. Functions declared "*noexcept*"
    2. [Abominable functions](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) (functions satisfying [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) but with at least one qualifier "*const*" and/or "*volatile*" and/or "*&*" or "*&&*")
    3. Variadic functions (those whose args include "...")

    Any other type for [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) is illegal, as mandated by the C++ standard itself (see "*StdFunction*" enumerator under [FunctionOrigin\_v](#functionorigin_v) for further details). Note that the "*operator()*" member of [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) is just a thin wrapper around the template arg used to specialize the class (the type of the function it will be used to invoke), so its "*operator()*" member always has the same arg types and return type as that template arg (the function used to specialize the class). It's normally the template arg itself that users need to target however, not the "*operator()*" member, which simply wraps the function passed to the template arg. Hence the library always targets the template arg of [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) when you pass such specializations for template arg "*F*", not the "*operator()*" member itself (of [std::function](https://en.cppreference.com/w/cpp/utility/functional/function)).

Note that the code also incorporates [concepts](https://en.cppreference.com/w/cpp/language/constraints) when targeting C++20 or later. This will trap invalid function types with cleaner error messages at compile-time (if anything other than the above is passed for "*F*"). In C++17 however (again, earlier versions aren't supported), if "*F"* is invalid then a compiler-specific error message indicating that "*FunctionTraits*" isn't defined will occur instead (since "*FunctionTraits*" is *declared* if "*F*" is illegal, but it's not actually *defined*). While a "*static\_assert*" could be employed to trap this in C++17, the use of "*static\_assert*" isn't [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae)-friendly. It doesn't respect [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) contexts so will trigger instead of quietly failing (even in [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) contexts). Omitting a "*static\_assert*" for illegal values of "*F*" therefore allows the "*FunctionTraits*" templates to properly support [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) in C++17, and invalid values of "*F*" even outside of [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) contexts are normally just programmer mistakes that rarely occur in practice anyway (so in C++17, supporting [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) for invalid template arg "*F"* types is favored over the use of "*static\_asserts*"). However, for templates that take other template args in addition to template arg "*F*", [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) is *not* supported for those particular args in C++17 (only in C++20 or later). Illegal template args besides template arg "*F*" itself (which always supports [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) as noted regardless of the version of C++), always result in a "*static\_assert*" in C++17 instead (since neglecting to trap such errors can potentially result in many compiler error messages that can be frustrating to diagnose - it was therefore decided to trap such errors with "*static_asserts*" instead in C++17). If you require [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) support in C++17 for template args other than "*F*" itself (i.e., you don't want a "*static_assert*" to trigger for such template args when illegal), then you will have to provide your own wrappers for the library's templates you need to support in an [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) context (ensuring the library's own templates won't kick in unless the particular template args you pass are in fact valid). This will normally be rare for most users however and the issue is confined to C++17 only as noted (again, in C++20 concepts are always used instead of "*static\_asserts*" and C++ itself will correctly handle these in [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) contexts).

Once you've #included "*FunctionTraits.h*", there are two ways to use the "*FunctionTraits*" struct (template) as noted earlier. You can either use the "*FunctionTraits*" struct (template) directly as described in the following section ([Technique 1 of 2](#technique1of2) just below), or use the library's [Helper templates](#helpertemplates) instead ([Technique 2 of 2](#technique2of2) further below), which are normally (strongly) recommended. Directly relying on the "*FunctionTraits*" struct (template) instead often results in verbose syntax due to C++ syntax rules, the reason users should normally rely on the library's helper templates instead as noted. These helper templates wrap the "*FunctionTraits*" struct (template) itself, cleanly eliminating the C++ clutter that often results when using "*FunctionTraits*" directly. Most will normally always rely on them so feel free to jump straight to [Technique 2 of 2](#technique2of2) now if you wish (again, strongly recommended).

<a name="Technique1Of2"></a>
## Technique 1 of 2 - Using "FunctionTraits" directly (not usually recommended)
```C++
// Only header you need to explicitly #include (see "Usage" section just above)
#include "FunctionTraits.h"

int main()
{
    // Everything declared in this namespace
    using namespace StdExt;

    ////////////////////////////////////////////////////////////////////
    // Free function whose traits you wish to retrieve (note that
    // static member functions are also considered "free" functions).
    // Pointers and references to free functions are also supported
    // (including references to such pointers), pointers to non-static
    // member functions (including references to such pointers, though
    // references to non-static member functions aren't supported in
    // C++ itself), functors (including lambdas), and "std::function"
    // specializations. All mainstream function syntax is therefore
    // supported.
    ////////////////////////////////////////////////////////////////////
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
    // template winds up taking a reference to a function
    // pointer)
    ///////////////////////////////////////////////////////////
    // using F = decltype(&SomeFunc) &;

    ///////////////////////////////////////////////////
    // And this works too (manually passing the type,
    // just to show we can)
    ///////////////////////////////////////////////////
    // using F = float (const std::string &, double, int);

    // Apply "FunctionTraits" to above function
    using SomeFuncTraits = FunctionTraits<F>;

    ////////////////////////////////////////////////////
    // Retrieve the function's return type (a "float").
    // Note that this (mandatory C++) syntax is ugly
    // however. A cleaner alias for this exists as
    // described in the next section.
    ////////////////////////////////////////////////////
    using SomeFuncReturnType_t = typename SomeFuncTraits::ReturnType_t;

    // Number of arguments (3)
    constexpr std::size_t SomeFuncArgCount = SomeFuncTraits::ArgCount_v;

    /////////////////////////////////////////////////////////
    // Retrieve type of the function's 3rd arg (an "int").
    // The arg's index is zero-based, so passing 2 here to
    // target the 3rd arg. Note that this (mandatory C++)
    // syntax is ugly however. A much cleaner alias for this
    // exists as described in the next section.
    /////////////////////////////////////////////////////////
    using SomeFuncArg3Type_t = typename SomeFuncTraits::template ArgType_t<2>;

    ////////////////////////////////////////////////////////////////
    // Type of the function's 3rd arg (same as above) but returned
    // as a "std::basic_string_view" (so literally returns "int",
    // quotes not included). Note that the following variable
    // template, "TypeName_v", can be used to get the name of any
    // type, not just types associated with "FunctionTraits". See
    // its entry in the "Helper templates" section later.
    ////////////////////////////////////////////////////////////////
    constexpr auto SomeFuncArg3TypeName = TypeName_v<SomeFuncArg3Type_t>;

    /////////////////////////////////////////////////////////////////
    // Check if the function's 3rd arg is an "int" (returns true in
    // this example). The arg's index is zero-based, so passing 2
    // here to target the 3rd arg.
    /////////////////////////////////////////////////////////////////
    constexpr bool IsArg3AnInt = SomeFuncTraits::IsArgTypeSame_v<2, int>;

    ///////////////////////////////////////////////////////////////
    // Though few will ever need to update a function trait (most
    // need to read them only), you can also modify them as well
    // (every available trait is supported). The following adds
    // "noexcept" to the function (see its string representation
    // just below).
    ///////////////////////////////////////////////////////////////
    using SomeFuncWithNoexceptAdded_t = typename SomeFuncTraits::AddNoexcept_t;

    /////////////////////////////////////////////////////////////////////
    // Above type as a "std::basic_string_view" so (literally) returns
    // the following (same as "SomeFunc" but with "noexcept" added -
    // note the format of the following may change slightly depending
    // on the target compiler).
    //
    //     float (const std::basic_string<char>&, double, int) noexcept
    /////////////////////////////////////////////////////////////////////
    constexpr auto SomeFuncWithNoexceptAdded_v = TypeName_v<SomeFuncWithNoexceptAdded_t>;

    /////////////////////////////////////////////////////////
    // And yet a different change, replacing the function's
    // 3rd arg (an "int") with a "char", passing 2 here as
    // the zero-based index of the arg we're targeting (see
    // its string representation just below). Note that
    // other templates also exist for conveniently modifying
    // a function's args anyway you wish (inserting,
    // deleting, etc.)..
    /////////////////////////////////////////////////////////
    using SomeFuncReplace3rdArgWithChar_t = typename SomeFuncTraits::template ReplaceArg_t<2, char>;

    ////////////////////////////////////////////////////////////
    // Above type as a "std::basic_string_view so (literally)
    // returns the following (same as function "SomeFunc" but
    // the 3rd arg has now been changed to a "char" - note the
    // format of the following may change slightly depending
    // on the target compiler).
    //
    //    float (const std::basic_string<char>&, double, char)
    ////////////////////////////////////////////////////////////
    constexpr auto SomeFuncReplace3rdArgWithChar_v = TypeName_v<SomeFuncReplace3rdArgWithChar_t>;

    // Etc. (see "Helper templates" further below for the complete list)

    return 0;
}
```

<a name="Technique2Of2"></a>
## Technique 2 of 2 - Using the helper templates instead of "FunctionTraits" directly (recommended)

Alternatively, instead of using the "*FunctionTraits*" struct (template) directly ([Technique 1 of 2](#technique1of2) above), you can rely on the second technique just below instead, which is much cleaner (and you should normally use it). As seen in [Technique 1 of 2](#technique1of2) above, relying on "*FunctionTraits*" directly can result in verbose syntax. For instance, due to the syntax of C++ itself, accessing the type of a given function arg using the "*ArgType\_t*" member (template) of struct "*FunctionTraits*" is ugly because you have to apply both the "*typename*" and "*template*" keywords (see "*SomeFuncArg3Type\_t*" alias in the [Technique 1 of 2](#technique1of2) example above). A *non-member* helper template to eliminate the clutter therefore exists not only for the latter example ([ArgType\_t](#argtype_t)), but for every member of "*FunctionTraits*". Therefore, instead of relying on struct "*FunctionTraits*" directly as seen in the [Technique 1 of 2](#technique1of2) examples, you can rely on the [Helper templates](#helpertemplates) instead. They're easier and cleaner, making the job of extracting or modifying a function's traits a breeze.

> [!NOTE]
> *(Optional reading): Two sets of helper templates actually exist, one where each template takes a "*FunctionTraits*" template arg, and a corresponding set taking a function type template arg instead (just pass your function's type "*F*" as described [here](#templateargf)). The latter (second) set of helper templates simply create a "*FunctionTraits*" for you from the function type you pass (by passing "*F*" itself as the template arg to "*FunctionTraits*"), and then defer to the corresponding "*FunctionTraits*" helper template in the first set. The "*FunctionTraits*" helper templates (the first set) are rarely used directly in practice however so they're not documented in this README file. Only the [Helper templates](#helpertemplates) taking a function template arg "*F*" (the second set) are therefore documented below. The two sets are functionally identical however other than their respective template args ("*FunctionTraits*" and "*F*"), and template names (the first set is normally named the same as the second but just adds the prefix "*FunctionTraits*" to the name), so the documentation below effectively applies to both sets. Most users will exclusively rely on the set taking template arg "*F*" however, so all remaining documentation below specifically targets it, though you can always quickly inspect the code itself if you ever want to use the "*FunctionTraits*" helper templates directly (since the second set taking template arg "*F*" just defers to them as described). Note that the "*FunctionTraits*" helper templates are just thin wrappers around the members of struct "*FunctionTraits*" itself, which inherits all its members from its base classes. Users normally never need to access these classes directly however, and are discouraged from doing so (so they are not documented here for that reason). The helper templates documented in this README file are all you normally ever need to rely on.*

Here's the same example seen in [Technique 1 of 2](#technique1of2) above but using the easier [Helper templates](#helpertemplates) instead:
```C++
// Only header you need to explicitly #include (see "Usage" section earlier)
#include "FunctionTraits.h"

int main()
{
    // Everything declared in this namespace
    using namespace StdExt;

    ////////////////////////////////////////////////////////////////////
    // Free function whose traits you wish to retrieve (note that
    // static member functions are also considered "free" functions).
    // Pointers and references to free functions are also supported
    // (including references to such pointers), pointers to non-static
    // member functions (including references to such pointers, though
    // references to non-static member functions aren't supported in
    // C++ itself), functors (including lambdas), and "std::function"
    // specializations. All mainstream function syntax is therefore
    // supported.
    ////////////////////////////////////////////////////////////////////
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
    // template winds up taking a reference to a function
    // pointer)
    ///////////////////////////////////////////////////////////
    // using F = decltype(&SomeFunc) &;

    ///////////////////////////////////////////////////
    // And this works too (manually passing the type,
    // just to show we can)
    ///////////////////////////////////////////////////
    // using F = float (const std::string &, double, int);

    ////////////////////////////////////////////////////
    // Retrieve the function's return type (a "float")
    // but using the cleaner helper alias this time
    // (instead of "FunctionTraits" directly)
    ////////////////////////////////////////////////////
    using SomeFuncReturnType_t = ReturnType_t<F>;

    ///////////////////////////////////////////////////////
    // Number of arguments (3) but using the helper alias
    // this time (instead of "FunctionTraits" directly)
    ///////////////////////////////////////////////////////
    constexpr std::size_t SomeFuncArgCount = ArgCount_v<F>;

    ////////////////////////////////////////////////////////
    // Retrieve type of the function's 3rd arg (an "int").
    // Arg's index is zero-based, so passing 2 here to
    // target the 3rd arg. Again, uses the cleaner helper
    // alias this time (instead of "FunctionTraits"
    // directly).
    ////////////////////////////////////////////////////////
    using SomeFuncArg3Type_t = ArgType_t<F, 2>;

    ////////////////////////////////////////////////////////////////
    // Type of the function's 3rd arg (same as above) but returned
    // as a "std::basic_string_view" (so literally returns "int",
    // quotes not included). Note that the following variable
    // template, "TypeName_v", can be used to get the name of any
    // type, not just types associated with "FunctionTraits". See
    // its entry in the "Helper templates" section later.
    ////////////////////////////////////////////////////////////////
    constexpr auto SomeFuncArg3TypeName = TypeName_v<SomeFuncArg3Type_t>;

    /////////////////////////////////////////////////////////////////
    // Check if the function's 3rd arg is an "int" (returns true in
    // this example). The arg's index is zero-based, so passing 2
    // here to target the 3rd arg.
    /////////////////////////////////////////////////////////////////
    constexpr bool IsArg3AnInt = IsArgTypeSame_v<F, 2, int>;

    ///////////////////////////////////////////////////////////////
    // Though few will ever need to update a function trait (most
    // need to read them only), you can also modify them as well
    // (every available trait is supported). The following adds
    // "noexcept" to the function (see its string representation
    // just below). Again, uses the easier helper template this
    // time instead of "FunctionTraits" directly.
    ///////////////////////////////////////////////////////////////
    using SomeFuncWithNoexceptAdded_t = AddNoexcept_t<F>;

    /////////////////////////////////////////////////////////////////////
    // Above type as a "std::string_view" so (literally) returns the
    // following (same as "SomeFunc" but with "noexcept" added - note
    // the format of the following may change slightly depending on
    // the target compiler).
    //
    //     float (const std::basic_string<char>&, double, int) noexcept
    /////////////////////////////////////////////////////////////////////
    constexpr auto SomeFuncWithNoexceptAdded_v = TypeName_v<SomeFuncWithNoexceptAdded_t>;

    /////////////////////////////////////////////////////////
    // And yet a different change, replacing the function's
    // 3rd arg (an "int") with a "char", passing 2 here as
    // the zero-based index of the arg we're targeting (see
    // its string representation just below). Again, uses
    // the cleaner helper template this time instead of
    // "FunctionTraits" directly. Note that other templates
    // also exist for conveniently modifying a function's
    // args anyway you wish (inserting, deleting, etc.).
    /////////////////////////////////////////////////////////
    using SomeFuncReplace3rdArgWithChar_t = ReplaceArg_t<F, 2, char>;

    ////////////////////////////////////////////////////////////
    // Above type as a "std::basic_string_view so (literally)
    // returns the following (same as function "SomeFunc" but
    // the 3rd arg has now been changed to a "char" - note the
    // format of the following may change slightly depending
    // on the target compiler).
    //
    //    float (const std::basic_string<char>&, double, char)
    ////////////////////////////////////////////////////////////
    constexpr auto SomeFuncReplace3rdArgWithChar_v = TypeName_v<SomeFuncReplace3rdArgWithChar_t>;

    // Etc. (see "Helper templates" further below for the complete list)

    return 0;
}
```

<a name="DeterminingIfAMemberFunctionExists"></a>
## Determining if a member function exists

> [!IMPORTANT]
> You may skip this (lengthy) section if you don't require it (the need to detect if a member function exists - you can proceed directly to the next section now instead, [Helper templates](#helpertemplates)), but if you rely on the member function detection templates in the following section then please see the following footnotes at the end of this document for issues that may affect you (though usually rare in practice):<br><br>***GCC and Clang users only:*** See footnote[^5]<br>***Microsoft users only:*** See footnote[^6]

Determining if a member function with a specific name and type (signature) exists in an arbitrary class is a requirement that's not usually required by most developers in practice. When it is required however most users can usually just rely on [std::is\_invocable](https://en.cppreference.com/w/cpp/types/is_invocable) and cousins in practice, and/or in C++20 and later its (concept) cousin [std::invocable](https://en.cppreference.com/w/cpp/concepts/invocable). However, those templates simply determine if a function can be called with a given set of arguments, and the arguments you pass don't need to exactly match the parameter types of the function you're checking (nor does the return type have to exactly match when using the "*std::is\_invocable\_r*" variant). The templates will still return "*true*" that is so long as the arguments you pass can be implicitly converted to the function's actual parameter types (say, if the function you're checking takes an "*int*" and you pass a "*double*" to [std::is\_invocable](https://en.cppreference.com/w/cpp/types/is_invocable) - it will return "*true*" since a "*double*" can be implicitly converted to an "*int*"). The details of how to use the latter templates to detect if a member function exists are beyond the scope of this documentation, but the point is that it's not sufficient if you need to detect if a function exists with *exact* parameter types (again, because of the implicit argument conversion issue). Moreover, you may need to check other function traits as well, not just its arguments and/or return type (though due to C++ language limitations, such granular checks are supported for *non-overloaded* functions only in this release - more on this shortly). You may want to check a particular parameter type only for instance (not all of them), and/or the function's return type (for an *exact* type which "*std::is\_invocable\_r*" doesn't do), and/or whether the function is "*const*", and/or any of the other possible components that make up a function (see [C++ Function Type Format](#cppfunctiontypeformat)).

The "*FunctionTraits*" library provides a set of templates to do this that will usually meet the needs of most users in practice. Unfortunately, due to lack of support in the language itself (which will hopefully be rectified when reflection becomes available in C++26 - read on), the library is currently restrained in both what it can offer, and how it's able to implement the templates it does offer. The implementation is therefore a (tiny) bit more verbose than it could be, primarily because function names can't be passed as template args in C++ (or as function args), although the library still relies on a reasonably clean approach given the situation. Moreover, if the function you wish to detect is overloaded, the library can detect if it exists based on its exact type but is restrained from detecting if it exists based on its individual traits, since the language itself doesn't support it. This is because when a function is overloaded, there is no way to isolate each specific overload so they can be checked for the traits you're interested in (and there may be more than one with those traits), since the language has no reflection capabilities (and no other way currently exists to do it). This will hopefully be rectified when reflection becomes available in C++26 as previously noted, and the situation will be revisited at that time. Until then, granular checks to detect if an *overloaded* function exists based on its individual traits isn't possible because there's no way to target the individual overloads for checking. An overloaded function only be detected based on an exact match of its entire type. If a function is *not* overloaded however, it *can* be targeted (detected) based on the precise traits you're interested in, since disambiguation among the function's overloads is no longer required. There *are* no overloads that is so the language does allow the one and only function with the given name you're interested in to be targeted (and therefore its individual traits can be checked).

Lastly, with the exception of the C++ function call operator, "*operator()*" (see [ClassHasOperator\_FunctionCall](#classhasoperator_functioncall) and cousins), no templates exist out-of-the-box to detect C++ operators. If you need to do so then you'll have to roll your own technique for the particular operator(s) you're interested in, and in practice for the specific class\[es\] as well (that you wish to inspect for a given operator). The detection of operators isn't supported in the library itself (out-of-the-box) because the lack of reflection in the language makes it impossible to provide anything other than a very messy and half-baked solution for reasons beyond the scope of this documentation. Fortunately, since most users normally (typically) need to check for *non-operator* functions only (based on the function's name), or the [function call operator](#classhasoperator_functioncall), it's unlikely most will be impacted by the situation (lack of support for the operator functions for now). Operator detection will likely be revisited in a future release however, once reflection becomes available in C++26 (where presumably a clean and full solution will then become possible). For now, the library provides templates to detect either named functions only (i.e., non-operator functions), or the [function call operator](#classhasoperator_functioncall).

> [!IMPORTANT]
> *Note that in all cases, whether overloaded or not, the normal C++ member "name hiding" rules remain in effect, so if a function exists in a derived class with the same name as a function in a base class, the base class functions are effectively hidden (they won't be detected when using the library's function detection templates on the derived class). This is often surprising to new C++ developers trying to call a member function. The short story is that the C++ name lookup rules first search the class a function is being called from and if not found there, will then search up the base class hierarchy until it's found. Searching stops as soon as the first base class with that function is encountered so if a function with the same name exists in another base class even further up the class hierarchy, it's effectively ignored (hidden by the one that was first found in a derived class - hidden functions don't participate in overloaded resolution). The library's function detection templates are subject to the same C++ rules which are enforced by the language itself (the templates will locate the first function in the class hierarchy matching the signature you're looking, ignoring functions with the same name that may be further up the class hierarchy).*

If you need to determine whether a function, say,```int Whatever(std::string_view, float) const noexcept``` exists in an arbitrary class (public functions only normally - again, Clang and GCC users should also consult footnote[^5] and Microsoft users footnote [^6]), and [std::is\_invocable](https://en.cppreference.com/w/cpp/types/is_invocable) and cousins aren't sufficient for your needs (again, if you need to check for *exact* parameter types or other traits, otherwise use [std::is\_invocable](https://en.cppreference.com/w/cpp/types/is_invocable) and cousins instead), you'll rely on one of six possible macros in the library to do so (but if targeting the function call operator "*operator()*" see [ClassHasOperator\_FunctionCall](#classhasoperator_functionCall) and cousins - they require no macros though you'll still need to read the following section to acquire an understanding). While relying on macros may seem a dirty technique to experienced developers (for named functions only, not "*operator()*"), and six of them to choose from no less, the language itself currently leaves no other choice but the situation isn't as bad as it first seems.

First, each macro merely declares a C++ (struct) template targeting the function you're interested in by name, where each struct is a [std::bool\_constant](https://en.cppreference.com/w/cpp/types/integral_constant#Helper_alias_templates) derivative that resolves to [std::true\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) if the function exists or [std::false\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) otherwise. A "*bool*" helper variable is also declared with the usual "*\_v*" suffix that just defers to the latter template. C++ provides no other way to do it besides using macros at this writing (read on), which do nothing more than provide the necessary boilerplate code to create the templates. You'll therefore be relying on the declared templates themselves to do the actual work, not on the macros themselves (which just declare the templates as noted). Again, no other way to do it currently exists. Until C++ supports reflection, again, which will be available in C++26 (and which will hopefully provide improved techniques), a function name can't be passed as a template argument in C++ (nor as a function argument), so dedicated templates are required to target each particular function name you're interested in. The six macros simply declare slightly different flavors of these templates targeting the particular function by name, such as "*Whatever*" in the earlier example, so the macros simply declare these templates with the hardcoded name. You'll normally just apply one of these macros for your particular needs however (the flavor you require), not all six of them, so the overhead isn't a significant issue.

Second, the six macros are really comprised of just two complementary sets of three macros each, where the first set targets (detects) *non-static* member functions and the second set targets (detects) *static* member functions. The two sets are effectively identical otherwise. There are therefore really just three macros to choose from, each serving slightly different needs, so you just need to choose the one you require (from the three), and then select either the *non-static* or *static* version based on the (*non-static* or *static*) function you wish to detect. The usage of the non-static and static macros is effectively the same otherwise. These macros are briefly described in the table just below with full examples in the last column (note that the table is somewhat lengthy but if using a mouse it can normally be horizontally scrolled via ```<Shift+MouseWheel>``` or less conveniently, the horizontal scrollbar beneath the table). For now the table provides a summary only but provides enough detail that most users can start using these macros and the templates they create after reading the table (again, with full examples in the last column). Consult the macros themselves documented later in this README.md file for complete details (see the links to these macros repeated in each cell of the table below). The macro documentation itself also includes some additional (useful) techniques in some cases (not shown in the table below), so it's recommended you ultimately read it (for the particular macro(s) you require).

### Member function detection templates (overview and comparative summary table)

*Using a mouse? Click table and use ```<Shift+MouseWheel>``` to scroll horizontally (easier than scrollbar at bottom of table).*

<table>
  <thead>
    <tr span style="font-size: x-large">
      <th>#</th>
      <th><i>Macro</i></th>
      <th><i>Templates created by this macro<br>(implementation details purposely omitted)</i></th>
      <!--Trailing spaces required in following to prevent
          column from being too narrow prior to user clicking
          our example <details> element to open them (creating
          an ergonomically bad experience). Dirty technique but
          couldn't find any work-around for GitHub ".md" files.
          Using arbitrary number of spaces here (workable for
          our needs - pinning down an accurate number a pain). -->
      <th style="text-align:left;"><i>Example</i>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</th>
    </tr>
  </thead>
  <tbody>
    <tr valign="top">
      <td>1</td>
      <td><b><a href="#declare_class_has_function">DECLARE_CLASS_HAS_FUNCTION(NAME)</a></b><br><br>Use when you need to check a class for an <i>exact</i> type match of a <i>non-static</i> (public) member function given by the <i>NAME</i> (macro) arg (among all overloads of <i>NAME</i> that may exist in the class, if any). Just pass the target class as the 1st arg to the templates created by this macro (see next column), and the function type itself as the 2nd arg (using any function type supported by this library - see <a href="#templateargf">Template arg &quot;F&quot;</a>). The templates then inherit from <a href="https://en.cppreference.com/w/cpp/types/integral_constant#Specializations">std::true_type</a> or <a href="https://en.cppreference.com/w/cpp/types/integral_constant#Specializations">std::false_type</a> (or return &quot;<i>true</i>&quot; or &quot;<i>false</i>&quot; depending on the template), based on whether the function you're targeting (<i>NAME</i>) exists in the target class (overloaded or not), <i>and</i> has the <i>exact</i> <a href="#cppfunctiontypeformat">function type</a> you pass. See adjacent columns for details.</td>
      <td><b><a href="#declare_class_has_function">DECLARE_CLASS_HAS_FUNCTION(NAME)</a> </b>declares:<pre><b>template &lt;IS_CLASS_C T,<br>          TRAITS_FUNCTION_C F&gt;<br>struct ClassHasFunction_##NAME : std::bool_constant&lt;&gt;<br>{<br>};<br><br>template &lt;IS_CLASS_C T,<br>          TRAITS_FUNCTION_C F&gt;<br>inline constexpr bool ClassHasFunction_##NAME##_v;</b></pre>First template above inherits from <a href="https://en.cppreference.com/w/cpp/types/integral_constant#Specializations">std::true_type</a> or <a href="https://en.cppreference.com/w/cpp/types/integral_constant#Specializations">std::false_type</a> based on whether the <i>non-static</i> (public) member function given by the macro's <i>NAME</i> arg exists in the class passed as the template's 1st arg (&quot;<i>T</i>&quot;, which must conform to <a href="https://en.cppreference.com/w/cpp/types/is_class">std::is_class</a>), and exactly matches the function type given by the 2nd arg (&quot;<i>F</i>&quot;, which can be any type supported by this library, such as <code style="white-space: pre;">int (std::string_view, float) const noexcept</code> - see <a href="#templateargf">Template arg &quot;F&quot;</a>). The second template above is just a bool helper variable you'll usually rely on instead (that just defers to the first template). Note that the <i>NAME</i> macro arg is embedded in each template's name, so if you pass, say, &quot;<i>Whatever</i>&quot; to the macro, the templates will be called &quot;<i>ClassHasFunction_Whatever</i>&quot; and &quot;<i>ClassHasFunction_Whatever_v</i>&quot;. The function &quot;<i>Whatever</i>&quot; will be detected whether overloaded or not.</td>
      <td><b><a href="#declare_class_has_function">DECLARE_CLASS_HAS_FUNCTION(NAME)</a></b><details><summary>Click here to open/close example 👀</summary><pre>// Standard C/C++ headers (only req'd for this example)<br>#include &lt;string_view&gt;<br><br>// Only file in this repository you need to explicitly #include<br>#include &quot;FunctionTraits.h&quot;<br><br>// Everything in the library declared in this namespace<br>using namespace StdExt;<br><br>/////////////////////////////////////////////////////////////<br>// Class you wish to check for <i>non-static</i> (public) function<br>// &quot;<i>Whatever</i>&quot; with the exact type of the first overload<br>// below (&quot;<i>ClassHasFunction_Whatever_v</i>&quot; called further below<br>// will detect it regardless of overloads).<br>/////////////////////////////////////////////////////////////<br>class Test<br>{<br>public:<br>    int Whatever(std::string_view, float) const noexcept;<br><br>    //////////////////////////////////////////////////////////////<br>    // Overload but above function will still be found. Template<br>    // &quot;<i>ClassHasFunction_Whatever_v</i>&quot; works even when &quot;<i>Whatever</i>&quot;<br>    // is overloaded.<br>    //////////////////////////////////////////////////////////////<br>    int Whatever(char) const noexcept;<br>};<br><br>/////////////////////////////////////////////////////////////////<br>// Declares template &quot;<i>ClassHasFunction_Whatever_v</i>&quot; called below<br>// which is set to true if the <i>non-static</i> (public) function<br>// &quot;<i>Whatever</i>&quot; exists in the class you pass it (via the 1st<br>// template arg), and exactly matches the <a href="#cppfunctiontypeformat">function type</a> you<br>// pass it (via the 2nd template arg). False returned otherwise.<br>/////////////////////////////////////////////////////////////////<br>DECLARE_CLASS_HAS_FUNCTION(Whatever)<br><br>int main()<br>{<br>    ///////////////////////////////////////////////////////////<br>    // Returns true since class &quot;<i>Test</i>&quot; (the 1st template arg)<br>    // has a <i>non-static</i> (public) function called &quot;<i>Whatever</i>&quot;<br>    // with the exact type being passed via the 2nd template<br>    // arg, &quot;<i>int (std::string_view, float) const noexcept</i>&quot; in<br>    // this case.<br>    ///////////////////////////////////////////////////////////<br>    constexpr bool classHasFunction_Whatever = ClassHasFunction_Whatever_v&lt;Test, int (std::string_view, float) const noexcept&gt;;<br><br>    return 0;<br>}</pre></details></td>
    </tr>
    <tr valign="top">
      <td>2</td>
      <td><b><a href="#declare_class_has_static_function">DECLARE_CLASS_HAS_STATIC_FUNCTION(NAME)</a></b><br><br>Identical to macro on row 1 of this table just above but detects a <i>static</i> function instead of <i>non-static</i>.</td>
      <td><b><a href="#declare_class_has_static_function">DECLARE_CLASS_HAS_STATIC_FUNCTION(NAME)</a> </b>declares:<pre><b>template &lt;IS_CLASS_C T,<br>          TRAITS_FUNCTION_C F&gt;<br>struct ClassHasStaticFunction_##NAME : std::bool_constant&lt;&gt;<br>{<br>};<br><br>template &lt;IS_CLASS_C T,<br>          TRAITS_FUNCTION_C F&gt;<br>inline constexpr bool ClassHasStaticFunction_##NAME##_v;</b></pre>Functionally equivalent to the templates created by the <i>DECLARE_CLASS_HAS_FUNCTION</i> macro on row 1 of this table just above but detects a <i>static</i> function instead of <i>non-static</i> (so if the function passed via the &quot;<i>F</i>&quot; template arg contains any function qualifiers, namely &quot;<i>const</i>&quot;, &quot;<i>volatile</i>&quot;, &quot;<i>&amp;</i>&quot; or &quot;<i>&amp;&amp;</i>&quot;, which apply to <i>non-static</i> member functions only, then false will always be returned). The templates will therefore return true only if the target function is <i>static</i> but are otherwise identical to the templates on row 1 of this table just above (see this for details). Note that the name of each template based on a <i>NAME</i> macro arg of, say, &quot;<i>Whatever</i>&quot;, will be &quot;<i>ClassHasStaticFunction_Whatever</i>&quot; and &quot;<i>ClassHasStaticFunction_Whatever_v</i>&quot;. The function &quot;<i>Whatever</i>&quot; will be detected whether overloaded or not.</td>
      <td><b><a href="#declare_class_has_static_function">DECLARE_CLASS_HAS_STATIC_FUNCTION(NAME)</a></b><details><summary>Click here to open/close example 👀</summary><pre>// Standard C/C++ headers (only req'd for this example)<br>#include &lt;string_view&gt;<br><br>// Only file in this repository you need to explicitly #include<br>#include &quot;FunctionTraits.h&quot;<br><br>// Everything in the library declared in this namespace<br>using namespace StdExt;<br><br>//////////////////////////////////////////////////////////<br>// Class you wish to check for <i>static</i> (public) function<br>// &quot;<i>Whatever</i>&quot; with the exact type of the first overload<br>// below (&quot;<i>ClassHasStaticFunction_Whatever_v</i>&quot; called<br>// further below will detect it regardless of overloads).<br>//////////////////////////////////////////////////////////<br>class Test<br>{<br>public:<br>    static int Whatever(std::string_view, float) noexcept;<br><br>    //////////////////////////////////////////////////////////////<br>    // Overload but above function will still be found. Template<br>    // &quot;<i>ClassHasStaticFunction_Whatever_v</i>&quot; works even when<br>    // &quot;<i>Whatever</i>&quot; is overloaded.<br>    //////////////////////////////////////////////////////////////<br>    static int Whatever(char) noexcept;<br>};<br><br>/////////////////////////////////////////////////////////////////<br>// Declares template &quot;<i>ClassHasStaticFunction_Whatever_v</i>&quot; called<br>// below which is set to true if the <i>static</i> (public) function<br>// &quot;<i>Whatever</i>&quot; exists in the class you pass it (via the 1st<br>// template arg), and exactly matches the <a href="#cppfunctiontypeformat">function type</a> you pass<br>// it (via the 2nd template arg). False returned otherwise.<br>/////////////////////////////////////////////////////////////////<br>DECLARE_CLASS_HAS_STATIC_FUNCTION(Whatever)<br><br>int main()<br>{<br>    ///////////////////////////////////////////////////////////<br>    // Returns true since class &quot;<i>Test</i>&quot; (the 1st template arg)<br>    // has a <i>static</i> (public) function called &quot;<i>Whatever</i>&quot; with<br>    // the exact type being passed via the 2nd template arg,<br>    // &quot;<i>int (std::string_view, float) noexcept</i>&quot; in this case.<br>    ///////////////////////////////////////////////////////////<br>    constexpr bool classHasStaticFunction_Whatever = ClassHasStaticFunction_Whatever_v&lt;Test, int (std::string_view, float) noexcept&gt;;<br><br>    return 0;<br>}</pre></details></td>
    </tr>
    <tr valign="top">
      <td>3</td>
      <td><b><a href="#declare_class_has_non_overloaded_function">DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION(NAME)</a></b><br><br>Functionally equivalent to macro <i>DECLARE_CLASS_HAS_FUNCTION</i> on row 1 of this table but used when you need to detect a <i>non-overloaded</i> member function instead. That is, the templates created by the latter macro will check for a (non-static) member function's existence whether it's overloaded or not, while the templates created by the following macro will only detect (non-static) member functions that are <i>not</i> overloaded (always returning false if they are overloaded). The templates are functionally identical otherwise. See adjacent column for details.</td>
      <td><b><a href="#declare_class_has_non_overloaded_function">DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION(NAME)</a> </b>declares:<pre><b>template &lt;IS_CLASS_C T,<br>          TRAITS_FUNCTION_C F&gt;<br>struct ClassHasNonOverloadedFunction_##NAME : std::bool_constant&lt;&gt;<br>{<br>};<br><br>template &lt;IS_CLASS_C T,<br>          TRAITS_FUNCTION_C F&gt;<br>inline constexpr bool ClassHasNonOverloadedFunction_##NAME##_v;</b></pre>Functionally equivalent to the templates created by the <i>DECLARE_CLASS_HAS_FUNCTION</i> macro on row 1 of this table but unlike those templates, the templates just above will only return true if the target function is <i>not</i> overloaded (unlike the templates created by the latter macro which detect the target function whether overloaded or not). See latter macro for further details. Note that the name of each template based on a <i>NAME</i> macro arg of, say, &quot;<i>Whatever</i>&quot;, will be &quot;<i>ClassHasNonOverloadedFunction_Whatever</i>&quot; and &quot;<i>ClassHasNonOverloadedFunction_Whatever_v</i>&quot;.</td>
      <td><b><a href="#declare_class_has_non_overloaded_function">DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION(NAME)</a></b><details><summary>Click here to open/close example 👀</summary><pre>// Standard C/C++ headers (only req'd for this example)<br>#include &lt;string_view&gt;<br><br>// Only file in this repository you need to explicitly #include<br>#include &quot;FunctionTraits.h&quot;<br><br>// Everything in the library declared in this namespace<br>using namespace StdExt;<br><br>//////////////////////////////////////////////////////////////<br>// Class you wish to check for <i>non-overloaded</i>, <i>non-static</i><br>// (public) function &quot;<i>Whatever</i>&quot; with the exact type seen<br>// below (&quot;<i>ClassHasNonOverloadedFunction_Whatever_v</i>&quot; called<br>// further below will detect it only if it's not overloaded)<br>//////////////////////////////////////////////////////////////<br>class Test<br>{<br>public:<br>    int Whatever(std::string_view, float) const noexcept;<br><br>    /////////////////////////////////////////////////////////////<br>    // Commented out. If uncommented then &quot;<i>Whatever</i>&quot; becomes<br>    // overloaded so &quot;<i>ClassHasNonOverloadedFunction_Whatever_v</i>&quot;<br>    // below will then return false instead (since it only<br>    // returns true for <i>non-overloaded</i> functions)<br>    /////////////////////////////////////////////////////////////<br>    // int Whatever(char) const noexcept;<br>};<br><br>/////////////////////////////////////////////////////////////////<br>// Declares template &quot;<i>ClassHasNonOverloadedFunction_Whatever_v</i>&quot;<br>// called below which is set to true if the <i>non-overloaded</i>,<br>// <i>non-static</i> (public) function &quot;<i>Whatever</i>&quot; exists in the class<br>// you pass it (via the 1st template arg), is not overloaded<br>// (unlike the example on row 1 of this table above which<br>// detects the target function even if overloaded), and exactly<br>// matches the <a href="#cppfunctiontypeformat">function type</a> you pass it (via the 2nd template<br>// arg). False returned otherwise.<br>/////////////////////////////////////////////////////////////////<br>DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION(Whatever)<br><br>int main()<br>{<br>    ////////////////////////////////////////////////////////////////<br>    // Returns true since class &quot;<i>Test</i>&quot; (the 1st template arg) has<br>    // a <i>non-overloaded</i>, <i>non-static</i> (public) function called<br>    // &quot;<i>Whatever</i>&quot; with the exact type being passed via the 2nd<br>    // template arg, &quot;<i>int (std::string_view, float) const noexcept</i>&quot;<br>    // in this case.<br>    ////////////////////////////////////////////////////////////////<br>    constexpr bool classHasNonOverloadedFunction_Whatever = ClassHasNonOverloadedFunction_Whatever_v&lt;Test, int (std::string_view, float) const noexcept&gt;;<br><br>    return 0;<br>}</pre></details></td>
    </tr>
    <tr valign="top">
      <td>4</td>
      <td><b><a href="#declare_class_has_non_overloaded_static_function">DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION(NAME)</a></b><br><br>Identical to macro on row 3 of this table just above but detects a <i>static</i> (<i>non-overloaded</i>) function instead of <i>non-static</i>.</td>
      <td><b><a href="#declare_class_has_non_overloaded_static_function">DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION(NAME)</a> </b>declares:<pre><b>template &lt;IS_CLASS_C T,<br>          TRAITS_FUNCTION_C F&gt;<br>struct ClassHasNonOverloadedStaticFunction_##NAME : std::bool_constant&lt;&gt;<br>{<br>};<br><br>template &lt;IS_CLASS_C T,<br>          TRAITS_FUNCTION_C F&gt;<br>inline constexpr bool ClassHasNonOverloadedStaticFunction_##NAME##_v;</b></pre>Functionally equivalent to the templates created by the <i>DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION</i> macro on row 3 of this table just above but detects a <i>static</i> (non-overloaded) function instead of <i>non-static</i> (so if the function passed via the &quot;<i>F</i>&quot; template arg contains any function qualifiers, namely &quot;<i>const</i>&quot;, &quot;<i>volatile</i>&quot;, &quot;<i>&amp;</i>&quot; or &quot;<i>&amp;&amp;</i>&quot;, which apply to <i>non-static</i> member functions only, then false will always be returned). The templates will therefore return true only if the target function is <i>static</i> but are otherwise identical to the templates on row 3 of this table just above (see this for details). Note that the name of each template based on a <i>NAME</i> macro arg of, say, &quot;<i>Whatever</i>&quot;, will be &quot;<i>ClassHasNonOverloadedStaticFunction_Whatever</i>&quot; and &quot;<i>ClassHasNonOverloadedStaticFunction_Whatever_v</i>&quot;.</td>
      <td><b><a href="#declare_class_has_non_overloaded_static_function">DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION(NAME)</a></b><details><summary>Click here to open/close example 👀</summary><pre>// Standard C/C++ headers (only req'd for this example)<br>#include &lt;string_view&gt;<br><br>// Only file in this repository you need to explicitly #include<br>#include &quot;FunctionTraits.h&quot;<br><br>// Everything in the library declared in this namespace<br>using namespace StdExt;<br><br>///////////////////////////////////////////////////////////<br>// Class you wish to check for <i>non-overloaded</i>, <i>static</i><br>// (public) function &quot;<i>Whatever</i>&quot; with the exact type seen<br>// below (&quot;<i>ClassHasNonOverloadedStaticFunction_Whatever_v</i>&quot;<br>// called further below will detect it only if it's not<br>// overloaded)<br>///////////////////////////////////////////////////////////<br>class Test<br>{<br>public:<br>    static int Whatever(std::string_view, float) noexcept;<br><br>    /////////////////////////////////////////////////////////////////////<br>    // Commented out. If uncommented then &quot;<i>Whatever</i>&quot; becomes overloaded<br>    // so &quot;<i>ClassHasNonOverloadedStaticFunction_Whatever_v</i>&quot; below will<br>    // then return false instead (since it only returns true for<br>    // <i>non-overloaded</i> functions)<br>    /////////////////////////////////////////////////////////////////////<br>    // static int Whatever(char) noexcept;<br>};<br><br>///////////////////////////////////////////////////////////////////////<br>// Declares template &quot;<i>ClassHasNonOverloadedStaticFunction_Whatever_v</i>&quot;<br>// called below which is set to true if the <i>non-overloaded</i>, <i>static</i><br>// (public) function &quot;<i>Whatever</i>&quot; exists in the class you pass it (via<br>// the 1st template arg), is not overloaded (unlike the example on<br>// row 2 of this table above which detects the target function even<br>// if overloaded), and exactly matches the <a href="#cppfunctiontypeformat">function type</a> you pass it<br>// (via the 2nd template arg). False returned otherwise.<br>///////////////////////////////////////////////////////////////////////<br>DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION(Whatever)<br><br>int main()<br>{<br>    ////////////////////////////////////////////////////////////<br>    // Returns true since class &quot;<i>Test</i>&quot; (the 1st template arg)<br>    // has a <i>non-overloaded</i>, <i>static</i> (public) function called<br>    // &quot;<i>Whatever</i>&quot; with the exact type being passed via the 2nd<br>    // template arg, &quot;<i>int (std::string_view, float) noexcept</i>&quot;<br>    // in this case.<br>    ////////////////////////////////////////////////////////////<br>    constexpr bool classHasNonOverloadedStaticFunction_Whatever = ClassHasNonOverloadedStaticFunction_Whatever_v&lt;Test, int (std::string_view, float) noexcept&gt;;<br><br>    return 0;<br>}</pre></details></td>
    </tr>
    <tr valign="top">
      <td>5</td>
      <td><b><a href="#declare_class_has_non_overloaded_function_traits">DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS(NAME)</a></b><br><br>Use instead of <i>DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION</i> (see row 3 of this table above) when the latter macro doesn't suffice because you need to do a more granular check for a (<i>non-overloaded</i>) function (say, if you want to ignore the function's return type, its <i>noexcept</i> specification, check a specific arg type only, etc.). You have complete control over what to check for opposed to the latter macro whose templates always look for an <i>exact</i> match of the function type you pass (so instead of passing a function type via the template's 2nd arg which the function you're checking for must <i>exactly</i> match, you'll effectively pass the specific function traits you wish to check for instead). See adjacent column for details.</td>
      <td><b><a href="#declare_class_has_non_overloaded_function_traits">DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS(NAME)</a> </b>declares:<pre><b>template &lt;IS_CLASS_C T,<br>          IS_CLASS_C HasFunctionTraitsT,<br>          typename UserTypeT = void&gt;<br>REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)<br>struct ClassHasNonOverloadedFunctionTraits_##NAME : std::bool_constant&lt;&gt;<br>{<br>};<br><br>template &lt;IS_CLASS_C T,<br>          IS_CLASS_C HasFunctionTraitsT,<br>          typename UserTypeT = void&gt;<br>REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)<br>inline constexpr bool ClassHasNonOverloadedFunctionTraits_##NAME##_v;</b></pre>Similar to the templates created by the <i>DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION</i> macro on row 3 of this table above except the 2nd template arg is no longer the function's type you wish to match, but a customized struct you&#39;ll write instead (template arg &quot;<i>HasFunctionTraitsT</i>&quot;), used to determine if the function exists based on the specific function traits you're interested in. Your struct should simply declare a template-based function call operator analogous to the one in the example below (the struct or class itself can be named anything you wish), or a variation of this operator taking a 2nd template arg of type &quot;<i>UserTypeT</i>&quot;, as seen in the template declaration above (for passing your own user-defined type but see <a href="#declare_class_has_non_overloaded_function_traits">DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS</a> for details). In C++20 or later however it's normally easier to just rely on a lambda template instead (which automatically creates the following template for you behind the scenes - see example in next column and latter link for complete details). Note that the <i>REQUIRES_IS_HAS_FUNCTION_TRAITS_C</i> clause in the above templates resolve to a concept in C++20 or later, ensuring that &quot;<i>HasFunctionTraitsT</i>&quot; is a conforming (valid) class.<br><pre>struct HasWhateverTraits<br>{<br>    template &lt;TRAITS_FUNCTION F&gt;<br>    constexpr bool operator()() const noexcept<br>    {<br>        /////////////////////////////////////////////////////<br>        // See explanation in next (example) column and<br>        // <a href="#declare_class_has_non_overloaded_function_traits">DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS</a><br>        // for complete details<br>        /////////////////////////////////////////////////////<br>        return (IsReturnTypeSame_v&lt;F, int&gt; &&<br>                CallingConvention_v&lt;F&gt; == DefaultCallingConvention_v&lt;false&gt; &&<br>                IsArgTypesSameTuple_v&lt;F, std::tuple&lt;std::string_view, float&gt; &gt; &&<br>                IsFunctionConst_v&lt;F&gt; &&<br>                !IsFunctionVolatile_v&lt;F&gt; &&<br>                FunctionReference_v&lt;F&gt; == FunctionReference::None &&<br>                IsNoexcept_v&lt;F&gt;);</pre>&quot;<i>operator()</i>&quot; above will be specialized by the library on the function type of <i>&amp;T::NAME</i> (assuming a <i>non-overloaded</i>, <i>non-static</i> member function called <i>NAME</i> exists in class &quot;<i>T</i>&quot;), and your &quot;<i>operator()</i>&quot; member will simply return &quot;<i>true</i>&quot; or &quot;<i>false</i>&quot; to indicate if the function (<a href="#templateargf">Template arg &quot;F&quot;</a>, a <i>non-static</i> member function pointer in this case), matches your search criteria (by testing the function's traits using the templates in this library). This gives you precise control over what to check for a match in the <a href="#cppfunctiontypeformat">function's type</a>, opposed to an <i>exact</i> match of the entire type that the <i>DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION</i> templates on row 3 of this table above require. Note that the name of each template based on a <i>NAME</i> macro arg of, say, &quot;<i>Whatever</i>&quot;, will be &quot;<i>ClassHasNonOverloadedFunctionTraits_Whatever</i>&quot; and &quot;<i>ClassHasNonOverloadedFunctionTraits_Whatever_v</i>&quot;.</td>
      <td><b><a href="#declare_class_has_non_overloaded_function_traits">DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS(NAME)</a></b><details><summary>Click here to open/close example 👀</summary><pre>// Standard C/C++ headers (only req'd for this example)<br>#include &lt;string_view&gt;<br><br>// Only file in this repository you need to explicitly #include<br>#include &quot;FunctionTraits.h&quot;<br><br>// Everything in the library declared in this namespace<br>using namespace StdExt;<br><br>/////////////////////////////////////////////////////////////<br>// Class you wish to check for <i>non-overloaded</i>, <i>non-static</i><br>// (public) function &quot;<i>Whatever</i>&quot; with the particular traits<br>// seen further below (class &quot;<i>HasWhateverTraits</i>&quot;).<br>// &quot;<i>ClassHasNonOverloadedFunctionTraits_Whatever_v</i>&quot; called<br>// further below will detect it only if it's not overloaded.<br>/////////////////////////////////////////////////////////////<br>class Test<br>{<br>public:<br>    int Whatever(std::string_view, float) const noexcept;<br><br>    /////////////////////////////////////////////////////////////////////<br>    // Commented out. If uncommented then &quot;<i>Whatever</i>&quot; becomes overloaded<br>    // so &quot;<i>ClassHasNonOverloadedFunctionTraits_Whatever_v</i>&quot; below will<br>    // then return false instead (since it only returns true for<br>    // non-overloaded functions)<br>    /////////////////////////////////////////////////////////////////////<br>    // int Whatever(char) const noexcept;<br>};<br><br>///////////////////////////////////////////////////////////////////////<br>// Declares template &quot;<i>ClassHasNonOverloadedFunctionTraits_Whatever_v</i>&quot;<br>// called below which is set to true if the <i>non-overloaded</i>, <i>non-static</i><br>// (public) function &quot;<i>Whatever</i>&quot; exists in the class you pass it (via<br>// the 1st template arg), is not overloaded, and has the traits passed <br>// via the 2nd template arg (&quot;<i>HasWhateverTraits</i>&quot; declared in function<br>// &quot;<i>main</i>&quot; below). False returned otherwise.<br>///////////////////////////////////////////////////////////////////////<br>DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS(Whatever)<br><br>int main()<br>{<br>    //////////////////////////////////////////////////////////////////////////////<br>    // This example assumes C++20 or later (checked for just below) which most<br>    // will likely be using at some point if not already, but see #else comments<br>    // below if you're targeting C++17 (earlier versions aren't supported by the<br>    // &quot;<i>FunctionTraits</i>&quot; library). The following simply creates a lambda template<br>    // to do the work (read on). However, you can even eliminate this &quot;<i>using</i>&quot;<br>    // statement if you wish, and simply pass the &quot;<i>decltype</i>&quot; statement below<br>    // directly to &quot;<i>ClassHasNonOverloadedFunctionTraits_Whatever_v</i>&quot; via its<br>    // 2nd arg. No additional overhead of this &quot;<i>using</i>&quot; statement is therefore<br>    // required, minimal as it is (if you prefer to just invoke the lambda<br>    // on-the-fly).<br>    //<br>    // Note that the lambda (template) itself creates the condition used to<br>    // determine if the <i>non-overloaded</i>, <i>non-static</i> (public) function<br>    // &quot;<i>Test::Whatever</i>&quot; exists in the call to<br>    // &quot;<i>ClassHasNonOverloadedFunctionTraits_Whatever_v</i>&quot; just below (by testing<br>    // the specific function traits you're interested in). The lambda (template)<br>    // will be specialized on the type of &quot;<i>Test::Whatever</i>&quot; in this particular<br>    // example (assuming a <i>non-static</i>, <i>non-overloaded</i> (public) function called<br>    // &quot;<i>Whatever</i>&quot; exists in class &quot;<i>Test</i>&quot;), so its &quot;<i>F</i>&quot; template arg will be<br>    // &quot;<i>int (Test::*)(std::string_view, float) const noexcept</i>&quot; in this example<br>    // (the type of &quot;&amp;<i>Test::Whatever</i>&quot;). The same lambda can be reused for other<br>    // functions you wish to check for the same traits however, in any class you<br>    // wish (if ever required). The lambda itself simply checks &quot;<i>F</i>&quot; for whatever<br>    // traits you require using the templates in this library normally (but you<br>    // can conduct any check you want), and returns true if a match is found or<br>    // false otherwise. This is then simply returned by<br>    // &quot;<i>ClassHasNonOverloadedFunctionTraits_Whatever_v</i>&quot; below.<br>    //<br>    // In this particular example however the following lambda effectively<br>    // performs the equivalent of the example on row 3 of this table above (see<br>    // <i>DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION(NAME)</i>), but does so by checking<br>    // the function's individual traits instead (using the templates in this<br>    // library). That is, every trait comprising a <a href="#cppfunctiontypeformat">C++ function type</a> is tested<br>    // for in the following lambda so it's equivalent to the example on row 3<br>    // above (but the individual traits are tested for here instead of the<br>    // function's actual C++ type). The lambda is therefore passed as the 2nd<br>    // template arg to &quot;<i>ClassHasNonOverloadedFunctionTraits_Whatever_v</i>&quot; below,<br>    // unlike the example on row 3 of this table where the function's actual C++<br>    // type is passed instead. The example on row 3 would therefore normally be<br>    // more suitable in this particular case since its interface is more natural<br>    // and less verbose. However, if you need to customize your check then the<br>    // example on row 3 can't be used since it always checks the entire<br>    // <a href="#cppfunctiontypeformat">function type</a> for a match (i.e., every possible component of a function<br>    // as described under <a href="#cppfunctiontypesataglance">C++ function types at a glance</a>). You'll therefore rely<br>    // on the following technique instead and just modify it for your particular<br>    // needs (say, if you don't want to check the calling convention for instance,<br>    // which most users normally don't care about - you can just remove the call<br>    // to the &quot;<i>CallingConvention_v</i>&quot; check seen in the lambda below and it will<br>    // then be ignored). The following technique therefore gives you complete<br>    // granular control over what to check compared to the technique used on row <br>    // 3 of this table. For complete details see<br>    // <a href="#declare_class_has_non_overloaded_function_traits">DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS</a>.<br>    //////////////////////////////////////////////////////////////////////////////<br>    &#35;if defined(LAMBDA_TEMPLATES_SUPPORTED) // Convenience macro in the library (lambda templates a C++20 feature)<br>        using HasWhateverTraits = decltype([]&lt;TRAITS_FUNCTION_C F&gt;() noexcept<br>                                           {<br>                                               return (IsReturnTypeSame_v&lt;F, int&gt; && // Must have &quot;<i>int</i>&quot; return type<br>                                                       CallingConvention_v&lt;F&gt; == DefaultCallingConvention_v&lt;false&gt; && // Must have the default calling convention for a <i>non-static</i> member function<br>                                                       IsArgTypesSameTuple_v&lt;F, std::tuple&lt;std::string_view, float&gt; &gt; && // Must have these 2 args (only): &quot;<i>std::string_view</i>&quot; and &quot;<i>float</i>&quot;<br>                                                       IsFunctionConst_v&lt;F&gt; && // Must be &quot;<i>const</i>&quot;<br>                                                       !IsFunctionVolatile_v&lt;F&gt; && // Must not be &quot;<i>volatile</i>&quot;<br>                                                       FunctionReference_v&lt;F&gt; == FunctionReference::None && // Must not have any ref-qualifiers (&quot;<i>&amp;</i>&quot; or &quot;<i>&amp;&amp;</i>&quot; though rare in practice)<br>                                                       IsNoexcept_v&lt;F&gt;); // Must be &quot;<i>noexcept</i>&quot;<br>                                           });<br>    &#35;else<br>        //////////////////////////////////////////////////////////////////////<br>        // If targeting C++17 which doesn't support lambda templates, or if<br>        // you simply don't wish to use lambda templates though there's<br>        // normally no reason not to (since they're less verbose), then<br>        // simply replace the &quot;<i>HasWhateverTraits</i>&quot; lambda template above with<br>        // your own equivalent template-based functor instead, like so<br>        // (declared at namespace scope since C++ doesn't support templates<br>        // at function scope - note that the function call operator below is<br>        // equivalent to the lambda template above):<br>        //<br>        //    struct HasWhateverTraits<br>        //    {<br>        //        template &lt;typename F&gt;<br>        //        constexpr bool operator()() const noexcept<br>        //        {<br>        //            return (IsReturnTypeSame_v&lt;F, int&gt; && // Must have &quot;<i>int</i>&quot; return type<br>        //                    CallingConvention_v&lt;F&gt; == DefaultCallingConvention_v&lt;false&gt; && // Must have the default calling convention for a <i>non-static</i> member function<br>        //                    IsArgTypesSameTuple_v&lt;F, std::tuple&lt;std::string_view, float&gt; &gt; && // Must have these 2 args (only): &quot;<i>std::string_view</i>&quot; and &quot;<i>float</i>&quot;<br>        //                    IsFunctionConst_v&lt;F&gt; && // Must be &quot;<i>const</i>&quot;<br>        //                    !IsFunctionVolatile_v&lt;F&gt; && // Must not be &quot;<i>volatile</i>&quot;<br>        //                    FunctionReference_v&lt;F&gt; == FunctionReference::None && // Must not have any ref-qualifiers (&quot;<i>&amp;</i>&quot; or &quot;<i>&amp;&amp;</i>&quot; though rare in practice)<br>        //                    IsNoexcept_v&lt;F&gt;); // Must be &quot;<i>noexcept</i>&quot;<br>        //        }<br>        //    };<br>        //////////////////////////////////////////////////////////////////////<br>        #error &quot;This example program is designed for C++20 or later (so it can rely on lambda templates which aren't available until C++20), but can be quickly and easily converted to target C++17 or later if required. See comments just above this error message (in the source code) for details&quot;.<br>    &#35;endif<br><br>    ///////////////////////////////////////////////////////////<br>    // Returns true since class &quot;<i>Test</i>&quot; (the 1st template arg)<br>    // has a <i>non-overloaded</i>, <i>non-static</i> (public) function<br>    // called &quot;<i>Whatever</i>&quot; matching the condition just above<br>    // (passed as the 2nd template arg in the following call).<br>    ///////////////////////////////////////////////////////////<br>    constexpr bool classHasNonOverloadedFunctionTraits_Whatever = ClassHasNonOverloadedFunctionTraits_Whatever_v&lt;Test, HasWhateverTraits&gt;;<br><br>    return 0;<br>}</pre></details></td>
    </tr>
    <tr valign="top">
      <td>6</td>
      <td><b><a href="#declare_class_has_non_overloaded_static_function_traits">DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_TRAITS(NAME)</a></b><br><br>Identical to macro on row 5 of this table just above but detects a <i>static</i> (<i>non-overloaded</i>) function instead of <i>non-static</i>.</td>
      <td><b><a href="#declare_class_has_non_overloaded_static_function_traits">DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_TRAITS(NAME)</a> </b>declares:<pre><b>template &lt;IS_CLASS_C T,<br>          IS_CLASS_C HasFunctionTraitsT,<br>          typename UserTypeT = void&gt;<br>REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)<br>struct ClassHasNonOverloadedStaticFunctionTraits_##NAME : std::bool_constant&lt;&gt;<br>{<br>};<br><br>template &lt;IS_CLASS_C T,<br>          IS_CLASS_C HasFunctionTraitsT,<br>          typename UserTypeT = void&gt;<br>REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)<br>inline constexpr bool ClassHasNonOverloadedStaticFunctionTraits_##NAME##_v;</b></pre>Functionally equivalent to the templates created by the <i>DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS</i> macro on row 5 of this table just above but detects a <i>static</i> function instead of <i>non-static</i>. The templates will therefore return true only if the target (non-overloaded) member function is <i>static</i> (but are otherwise identical to the templates on row 5 of this table just above). Note that the name of each template based on a <i>NAME</i> macro arg of, say, &quot;<i>Whatever</i>&quot;, will be &quot;<i>ClassHasNonOverloadedStaticFunctionTraits_Whatever</i>&quot; and &quot;<i>ClassHasNonOverloadedStaticFunctionTraits_Whatever_v</i>&quot;.</td>
      <td><b><a href="#declare_class_has_non_overloaded_static_function_traits">DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_TRAITS(NAME)</a></b><details><summary>Click here to open/close example 👀</summary><pre>// Standard C/C++ headers (only req'd for this example)<br>#include &lt;string_view&gt;<br><br>// Only file in this repository you need to explicitly #include<br>#include &quot;FunctionTraits.h&quot;<br><br>// Everything in the library declared in this namespace<br>using namespace StdExt;<br><br>////////////////////////////////////////////////////////////<br>// Class you wish to check for <i>non-overloaded</i>, <i>static</i><br>// (public) function &quot;<i>Whatever</i>&quot; with the particular traits<br>// seen further below (class &quot;<i>HasWhateverTraits</i>&quot;).<br>// &quot;<i>ClassHasNonOverloadedStaticFunctionTraits_Whatever_v</i>&quot;<br>// called further below will detect it only if it's not<br>// overloaded.<br>////////////////////////////////////////////////////////////<br>class Test<br>{<br>public:<br>    static int Whatever(std::string_view, float) noexcept;<br><br>    /////////////////////////////////////////////////////////////////////<br>    // Commented out. If uncommented then &quot;<i>Whatever</i>&quot; becomes overloaded<br>    // so &quot;<i>ClassHasNonOverloadedStaticFunctionTraits_Whatever_v</i>&quot; below<br>    // will then return false instead (since it only returns true for<br>    // non-overloaded functions)<br>    /////////////////////////////////////////////////////////////////////<br>    // static int Whatever(char) noexcept;<br>};<br><br>/////////////////////////////////////////////////////////////////////////////<br>// Declares template &quot;<i>ClassHasNonOverloadedStaticFunctionTraits_Whatever_v</i>&quot;<br>// called below which is set to true if the <i>non-overloaded</i>, <i>static</i> (<i>public</i>)<br>// function &quot;<i>Whatever</i>&quot; exists in the class you pass it (via the 1st template<br>// arg), is not overloaded, and has the traits passed via the 2nd template<br>// arg (&quot;<i>HasWhateverTraits</i>&quot; declared in function &quot;<i>main</i>&quot; below). False<br>// returned otherwise.<br>/////////////////////////////////////////////////////////////////////////////<br>DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_TRAITS(Whatever)<br><br>int main()<br>{<br>    //////////////////////////////////////////////////////////////////<br>    // See the same lambda template in the example on row 5 of this<br>    // table just above. The same long comments there apply here as<br>    // well but the following detects a <i>static</i> function instead. The<br>    // check for those traits that apply to <i>non-static</i> functions only<br>    // (as seen in the &quot;<i>HasWhateverTraits</i>&quot; lambda for the latter<br>    // example) have therefore been removed from the following<br>    // &quot;<i>HasWhateverTraits</i>&quot; declaration (namely, the checks for<br>    // cv-qualifiers &quot;<i>const</i>&quot; and &quot;<i>volatile</i>&quot; and ref-qualifiers &quot;<i>&amp;</i>&quot;<br>    // and &quot;<i>&amp;&amp;</i>&quot;).<br>    //////////////////////////////////////////////////////////////////<br>    &#35;if defined(LAMBDA_TEMPLATES_SUPPORTED) // Convenience macro in the library (lambda templates a C++20 feature)<br>        using HasWhateverTraits = decltype([]&lt;TRAITS_FUNCTION_C F&gt;() noexcept<br>                                           {<br>                                               return (IsReturnTypeSame_v&lt;F, int&gt; && // Must have &quot;<i>int</i>&quot; return type<br>                                                       CallingConvention_v&lt;F&gt; == DefaultCallingConvention_v&lt;true&gt; && // Must have the default calling convention for a <i>free</i> function (since it's static)<br>                                                       IsArgTypesSameTuple_v&lt;F, std::tuple&lt;std::string_view, float&gt; &gt; && // Must have these 2 args (only): &quot;<i>std::string_view</i>&quot; and &quot;<i>float</i>&quot;<br>                                                       IsNoexcept_v&lt;F&gt;); // Must be &quot;<i>noexcept</i>&quot;<br>                                           });<br>    &#35;else<br>        //////////////////////////////////////////////////////////////////////<br>        // Again, see the same comments in the example on row 5 just above.<br>        // The same comments there apply here as well. In C++17 simply <br>        // replace the lambda above with the following template declared at <br>        // namespace scope.<br>        //<br>        //    struct HasWhateverTraits<br>        //    {<br>        //        template &lt;typename F&gt;<br>        //        constexpr bool operator()() const noexcept<br>        //        {<br>        //            return (IsReturnTypeSame_v&lt;F, int&gt; && // Must have &quot;<i>int</i>&quot; return type<br>        //                    CallingConvention_v&lt;F&gt; == DefaultCallingConvention_v&lt;true&gt; && // Must have the default calling convention for a <i>free</i> function (since it's static)<br>        //                    IsArgTypesSameTuple_v&lt;F, std::tuple&lt;std::string_view, float&gt; &gt; && // Must have these 2 args (only): &quot;<i>std::string_view</i>&quot; and &quot;<i>float</i>&quot;<br>        //                    IsNoexcept_v&lt;F&gt;); // Must be &quot;<i>noexcept</i>&quot;<br>        //        }<br>        //    };<br>        //////////////////////////////////////////////////////////////////////<br>        #error &quot;This example program is designed for C++20 or later (so it can rely on lambda templates which aren't available until C++20), but can be quickly and easily converted to target C++17 or later if required. See comments just above this error message (in the source code) for details&quot;.<br>    &#35;endif<br><br>    ///////////////////////////////////////////////////////////<br>    // Returns true since class &quot;<i>Test</i>&quot; (the 1st template arg)<br>    // has a <i>non-overloaded</i>, <i>static</i> (public) function called<br>    // &quot;<i>Whatever</i>&quot; matching the condition just above (passed<br>    // as the 2nd template arg in the following call).<br>    ///////////////////////////////////////////////////////////<br>    constexpr bool classHasNonOverloadedStaticFunctionTraits_Whatever = ClassHasNonOverloadedStaticFunctionTraits_Whatever_v&lt;Test, HasWhateverTraits&gt;;<br><br>    return 0;<br>}</pre></details></td>
    </tr>
  </tbody>
</table>

<a name="HelperTemplates"></a>
## Helper templates (complete, alphabetical list)
The following provides a complete (alphabetical) list of all helper templates (see [Usage](#usage) section to get started). Two separate sections exist, the first for [Read traits](#readtraits), allowing you to read any component of a function's type (see [C++ Function Type Format](#cppfunctiontypeformat)), and the second for [Write traits](#writetraits), allowing you to update any component of a function's type (an [Additional helper templates](#additionalhelpertemplates) section also exists with some useful miscellaneous templates). To see a brief description of the library's main templates grouped by the C++ function component they target (from return type to noexcept), see [Summary of templates in this library](#summaryoftemplatesinthislibrary) (each template there simply links back to the full documentation in the following section).

Note that the first template arg of every template is the function you're targeting, whose name is always "*F*" (see [here](#templateargf) for details). IMPORTANT: When "*F*" is a functor, note that all traits implicitly refer (apply) to the non-static member function "*F::operator()*" unless noted otherwise (so if "*F*" is the type of a lambda for instance then the traits target "*operator()*" of the compiler-generated class for that lambda). It should therefore be understood that whenever "*F*" is a functor and it's cited in the description of each template below, the description is normally referring to member function "*F::operator()*", not class "*F*" itself. The same applies when "*F*" is a [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specialization, but the function being targeted is the template arg it's specialized on (i.e., the function that "*F*" wraps).

Please note that for all traits, a *TRAITS\_FUNCTION\_C* [concept](https://en.cppreference.com/w/cpp/language/constraints) (declared in "*FunctionTraits.h*") will kick in for illegal values of "*F*" in C++20 or later ("*F*" is declared a *TRAITS\_FUNCTION\_C* in all templates below), or a (compiler-specific) "*Undefined FunctionTraits*" error of some kind in C++17 otherwise (again, earlier versions aren't supported). Note that *TRAITS\_FUNCTION\_C* is just a #defined macro that resolves to the [concept](https://en.cppreference.com/w/cpp/language/constraints) "*StdExt::TraitsFunction\_c*" in C++20 or later (ensuring "*F*" is legal), or simply the "*typename*" keyword in C++17. In either case the "*FunctionTraits*" template is declared but not defined for illegal types of "*F*", allowing it to be used in [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) contexts. The concept itself will correctly fail SFINAE in C++20 or later, or the undefined "*FunctionTraits*" template error will correctly fail SFINAE in C++17. For many of the traits, "*F*" is the only template arg (again, see [here](#templateargf)). For those that take additional template args however (described on a case-by-case basis below), a macro similar to *TRAITS\_FUNCTION\_C* will resolve to the appropriate [concept](https://en.cppreference.com/w/cpp/language/constraints) or [requires](https://en.cppreference.com/w/cpp/language/requires) expression in order to trap illegal template args in C++20 or later (described for each template below), or again, the "*typename*" keyword in C++17 (or whitespace for [requires](https://en.cppreference.com/w/cpp/language/requires) expressions). In the latter case however (C++17), a "*static\_assert*" will then trigger instead, unlike for template arg "*F*" itself, which doesn't result in a "*static\_assert*" as described above (again, a compiler-specific "*Undefined FunctionTraits*" error of some kind occurs instead). Support for [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) in C++17 is therefore confined to template arg "*F*" only, not to other template args. See [Template arg "F"](#templateargF) for details.

Lastly, note that the "=" sign is omitted after the template name in each template declaration below. The "=" sign is removed since the implementation isn't normally relevant for most users. You can inspect the actual declaration in "*FunctionTraits.h*" if you wish to see it.

> [!NOTE]
> *Please note that each template below simply wraps the corresponding member of the "*FunctionTraits*" struct itself (or indirectly targets it). In most cases the name of each "*FunctionTraits*" member is identical to the helper template that wraps it below. In a few minor cases however the helper template name has been slightly changed from the name of the "*FunctionTraits*" member it wraps in order to prevent possible confusion, since the helper template is being called from the "*StdExt*" namespace scope opposed to the "*struct*" scope of "*FunctionTraits*" itself. For instance, when called from "*FunctionTraits*" directly, the member "*IsConst\_v*" is invoked as "*FunctionTraits::IsConst\_v*" so it should be reasonably clear what the "*Const*" in the name refers to (whether a function is declared with the "*const*" function qualifier). When using the helper template declared at the "*StdExt*" namespace scope however, a name like "*IsConst\_v*" might be confusing as to what the "*Const*" in the name refers to, i.e., perhaps not a function since the "*FunctionTraits*" struct itself is no longer prefixed to the name. The helper template is therefore named "*IsFunctionConst\_v*" instead to eliminate this confusion. Note that in all cases like this the template's documentation below will note the name change via a "*Note: Named*" comment in the declaration of each affected template. Nevertheless, while you can access "*FunctionTraits*" directly, the helper templates below normally make it unnecessary. "*FunctionTraits*" itself therefore isn't documented in this *README.md* file, nor are any of the helper templates taking a "*FunctionTraits*" template arg (as described in [Technique 2 of 2](#technique2of2)). The following helper templates taking a function template arg "*F*" instead are normally much easier (again, as described in [Technique 2 of 2](#technique2of2)). The syntax for accessing "*FunctionTraits*" directly (or using any of the helper templates taking a "*FunctionTraits*" template arg) is therefore not shown in this document as noted. In the unlikely event that you need (or wish) to use "*FunctionTraits*" directly then see [Technique 1 of 2](#technique1of2) for examples. The documentation below still applies since the templates below are just thin wrappers around the "*FunctionTraits*" members, only you'll need to change the calling syntax to use the (verbose) C++ "*typename*" and/or "*template*" keywords where applicable when using the "*FunctionTraits*" members directly (as shown in [Technique 1 of 2](#technique1of2)). However, since every public member of "*FunctionTraits*" has a helper template taking a function template arg "*F*" as seen in each template declaration below, which never require the "*typename*" and/or "*template*" keywords, these helper templates should normally be relied on instead. They're easier and cleaner, so the following documentation is normally all you ever need to consult (note that inspecting the code itself for additional insight is normally unnecessary and would be an arduous task).*

<a name="ReadTraits"></a>
### _Read traits_

<a name="ArgCount_v"></a><details><summary>ArgCount\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr std::size_t ArgCount_v;
```
"*std::size\_t*" variable storing the number of arguments in "*F*" *not* including variadic arguments if any (if present, the "..." for variadic args is completely ignored). Will be zero if function "*F*" has no (non-variadic) args. Note that this count is formally called "*arity*" but this variable is given a more user-friendly name.<br /><br /><ins>***IMPORTANT***</ins>:<br />Please note that if you wish to check if a function's argument list is completely empty, then inspecting this helper template for zero (0) is not sufficient, since it may return zero but still contain variadic args. To check for a completely empty argument list, call [IsArgListEmpty\_v](#isarglistempty_v) instead.
</blockquote></details>

<a name="ArgType_t"></a><details><summary>ArgType\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I,
          bool ReturnVoidIfInvalidIndex = false>
REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_VOID(F, I, ReturnVoidIfInvalidIndex)
using ArgType_t;
```
Type alias for the type of the "*I*th" arg in function "*F*", where "*I*" is in the range 0 to the number of arguments in "*F*" minus 1 (see [ArgCount\_v](#argcount_v) just above). Note that if "*I*" is greater than or equal to the number of args in "*F*" (again, see [ArgCount\_v](#argcount_v) just above), meaning you're targeting a non-existent arg (variadic args if any are always ignored - the "..." isn't relevant), then a compiler error will occur if "*ReturnVoidIfInvalidIndex*" is "*false*" (the default), or "*void*" will be returned otherwise (read on). Note that "*void*" function arg (parameter) types are invalid in C++ so if returned then it's guaranteed that "*I*" is invalid (targets a non-existent arg), and this can only occur if "*ReturnVoidIfInvalidIndex*" is "*true*". If "*I*" is invalid and "*ReturnVoidIfInvalidIndex*" is "*false*" however (again, the default), then a constraints violation will occur in C++20 or later (via the REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_VOID concept seen in the template's declaration), or a "*static\_assert*" in C++17 otherwise. Note that if "*F*" has no (non-variadic) args whatsoever then all values of "*I*" are invalid even if passing zero (so it's always guaranteed that either an error will occur as just described, or "*void*" will be returned, as controlled by "*ReturnVoidIfInvalidIndex*"). Note that passing "*true*" for the "*ReturnVoidIfInvalidIndex*" arg is useful in certain scenarios such as [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) situations, where you don't want an error to trigger if "*I*" targets an invalid arg (again, either a constraints violation in C++20 or later or a "*static\_assert*" in C++17). Note that since a constraints violation in C++20 or later is based on C++ concepts which respect [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae), it will therefore cause template substitution to simply fail if "*ArgType\_t*" is passed an invalid index for "*I*" (in an [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) situation). A "*static\_assert*" doesn't respect [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) however so in C++17 it will always cause a hard-compiler error instead, even in an [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) situation. Passing "*true*" for "*ReturnVoidIfInvalidIndex*" in an [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) situation can therefore be useful to prevent any error at all (a constraints violation in C++20 or later or a "*static_assert*" in C++17), since it will simply result in "*void*" being returned if there is no "*Ith*" arg (that your code can check for in whatever way it's designed by simply passing the result to [std::is\_void](https://en.cppreference.com/w/cpp/types/is_void)).

Lastly, note that even in a non-[SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) situation, passing "*true*" for "*ReturnVoidIfInvalidIndex*" can sometimes be useful to prevent a compiler error if "*I*" might target an invalid argument in the function you're dealing with. For instance, when using the [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS](#declare_class_has_non_overloaded_function_traits) macro (see this for details), if the "*operator()*" member of the "*HasFunctionTraitsT*" arg you pass to the templates created by this macro needs to call [IsArgTypeSame\_v](#isargtypesame_v), which ultimately relies on "*ArgType\_t*", you'll normally want to pass "*true*" for the "*CompareWithVoidIfInvalidIndex*" arg of [IsArgTypeSame\_v](#isargtypesame_v) (if there's any chance you might be targeting a non-existent arg), which just forwards it along to "*ArgType\_t*" via the "*ReturnVoidIfInvalidIndex*" arg just described. Otherwise the call to "*HasFunctionTraitsT::operator()*" might trigger a compiler error instead of just returning "*false*" (again, if you're targeting a non-existent arg). Again, see [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS](#declare_class_has_non_overloaded_function_traits) for details.

Finally, please note that if you require access to a function's specific arg via "*ArgType\_t*", ensure that another helper template doesn't already exist that will do what you require (in a way that's simpler than accessing "*ArgType_t*" directly). For instance, templates [IsArgTypeSame\_v](#isargtypesame_v) and [IsArgTypeMatch\_v](#isargtypematch_v) can be used to check individual args for a specific type (depending on your needs), without having to call "*ArgType\_t*" directly (they call it for you and then do the comparison - see each template for details).

*Examples:*
```C++
// Sample function
using F = float (const std::string &, double, int);

////////////////////////////////////////////////////////
// Retrieve type of the function's 3rd arg (an "int").
// Arg's index is zero-based, so passing 2 here (to
// target the 3rd arg).
////////////////////////////////////////////////////////
using Arg3Type_t = ArgType_t<F, 2>;

////////////////////////////////////////////////////////
// Retrieve type of the function's 4th arg (passing 3
// however since index is zero-based), but since there
// aren't 4 args, we're targeting a non-existent arg.
// By default this will result in a constraints
// violation in C++20 or later (via the
// REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_VOID concept
// seen in the template's declaration), or a
// "static_assert" in C++17. By explicitly passing
// "true" for the "ReturnVoidIfInvalidIndex" template
// arg however (it defaults to false, resulting in the
// above errors), "void" will be returned instead.
// Function args (parameters) in C++ can never be "void"
// however so if it's returned (always in this example),
// then it guarantees that the "I" template arg (3 in
// this case) refers to a non-existent arg.
////////////////////////////////////////////////////////
using Arg4Type_t = ArgType_t<F, 3, true>;

// Always true in this example (non-existent arg)
if constexpr (std::is_void_v<Arg4Type_t>)
{
   // Non-existent arg processing
}
```
</blockquote></details>

<a name="ArgTypeName_v"></a><details><summary>ArgTypeName\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I,
          bool ReturnEmptyStringIfInvalidIndex = false>
REQUIRES_I_LESS_THAN_ARG_COUNT_OR_RETURN_EMPTY_STRING(F, I, ReturnEmptyStringIfInvalidIndex)
inline constexpr tstring_view ArgTypeName_v;
```
Same as [ArgType\_t](#argtype_t) but returns this as a string (of type "*tstring\_view*" - see [TypeName\_v](#typename_v) for details). A *float* would therefore be (literally) returned as "*float*" for instance (quotes not included). See both of the latter templates for details ([ArgType\_t](#argtype_t) since \"*ArgTypeName\_v*\" just defers to it, and [TypeName\_v](#typename_v) which is used to create the actual string). Lastly, please note that the "*ReturnEmptyStringIfInvalidIndex*" template arg corresponds to the "*ReturnVoidIfInvalidIndex*" template arg of [ArgType\_t](#argtype_t) ("*ReturnEmptyStringIfInvalidIndex*" is simply passed to it), so if "*false*" (the default arg), then an error will occur if "*I*" is invalid (targets a non-existent arg in "*F*" - see [ArgType\_t](#argtype_t) for details). If "*ReturnEmptyStringIfInvalidIndex*" is "*true*" however (if "*I*" is invalid), then an empty string will be returned by "*ArgTypeName\_v*" instead (since [ArgType\_t](#argtype_t) will return *void* in this case but "*ArgTypeName_v*" will then return an empty string as noted, *not* the string "*void*" itself).
</blockquote></details>

<a name="ArgTypes_t"></a><details><summary>ArgTypes\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using ArgTypes_t;
```
Type alias for a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) specialized on all argument types in "*F*", i.e., its complete argument list *not* including variadic args if any (the "..." in function "*F*" is irrelevant - see [IsVariadic\_v](#isvariadic_v) if you need to determine if a function is variadic). This template is rarely required in practice however since you can usually rely on other templates to carry out your needs, such as [ArgType\_t](#argtype_t) or [ArgTypeName\_v](#argtypename_v) to retrieve the type or name of a specific argument, [IsArgTypesSame\_v](#isargtypessame_v), [IsArgTypeSame\_v](#isargtypesame_v), [IsArgTypesMatch\_v](#isargtypesmatch_v) or [IsArgTypeMatch\_v](#isargtypematch_v) to compare arguments, [ForEachArg](#foreacharg) to loop through all arguments, etc. If you require access to a function's args via "*ArgTypes\_t*", ensure that another helper template doesn't already exist that will do what you require (in a way that's simpler than accessing "*ArgTypes_t*" directly).
</blockquote></details>

<a name="CallingConvention_v"></a><details><summary>CallingConvention\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr CallingConvention CallingConvention_v;
```
Variable storing the calling convention of "*F*" returned as a "*CallingConvention*" enumerator (declared in "*FunctionTraits.h*"):
```C++
enum class CallingConvention
{
    Cdecl,
    Stdcall,
    Fastcall,
    Vectorcall,
    Thiscall,
    Regcall,
    Variadic = Cdecl // Variadic functions are always "Cdecl" on all supported compilers
};
```
Note that in all cases only those calling conventions that are supported by a given compiler can be returned by "*CallingConvention\_v*" (so, for instance, "*Vectorcall*" will never be returned on GCC since it doesn't support this calling convention). Also please note that compilers will sometimes change the calling convention declared on your functions to the "*Cdecl*" calling convention depending on the compiler options in effect at the time (in particular when compiling for 64 bits opposed to 32 bits, though some calling conventions like "*Vectorcall*" *are* supported on 64 bits though again, GCC doesn't support "*Vectorcall*"). In this case the calling convention on your function is ignored and "*CallingConvention\_v*" will correctly return the "*Cdecl*" calling convention instead (if that's what the compiler actually uses, which is normally the case). For a function's default calling convention see [DefaultCallingConvention\_v](#defaultcallingconvention_v).
</blockquote></details>

<a name="CallingConventionName_v"></a><details><summary>CallingConventionName\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view CallingConventionName_v;
```
Same as [CallingConvention\_v](#callingconvention_v) just above but returns this as a string (of type "*tstring\_view*" - see [TypeName\_v](#typename_v) for details). Note that unlike the "*CallingConvention*" enumerators returned by [CallingConvention\_v](#callingconvention_v) however (whose first letter is always uppercase), the strings returned by the following template are always returned in lowercase ("*cdecl*", "*stdcall*", "*fastcall*", "*vectorcall*", "*thiscall*" or "*regcall*"). See [CallingConvention\_v](#callingconvention_v) for details (since \"*CallingConventionName\_v*\" simply returns the string equivalent as just described).
</blockquote></details>

<a name="DefaultCallingConvention_v"></a><details><summary>DefaultCallingConvention\_v</summary>

<blockquote>

```C++
template <bool IsFreeFunc>
inline constexpr CallingConvention DefaultCallingConvention_v;
```
Variable storing the default calling convention for a function (for a specific function's calling convention see [CallingConvention\_v](#callingconvention_v)). Pass "*true*" for the "*IsFreeFunc*" template arg if you require the default calling convention for a *free* function (which includes *static* member functions), or "*false*" for a *non-static* member function (since the calling convention of *free* and *non-static* member functions might differ depending on the compiler and/or compiler options and/or target architecture - the Microsoft compiler in particular defaults to the ```__cdecl``` calling convention for *free* functions when targeting x86 but ```__thiscall``` for *non-static* member functions - see the [/Gd, /Gr, /Gv, /Gz](https://learn.microsoft.com/en-us/cpp/build/reference/gd-gr-gv-gz-calling-convention?view=msvc-170) Microsoft compiler options though no such options exist for all other supported compilers, which normally just default to ```__attribute__((cdecl))``` for both *free* and *non-static* member functions).

Note that the default calling convention refers to the calling convention applied to a function by the compiler when not explicitly specified in the function itself. The calling convention is returned as a "*CallingConvention*" enumerator declared in "*FunctionTraits.h*" (either free, non-static member or abominable):
```C++
enum class CallingConvention
{
    Cdecl,
    Stdcall,
    Fastcall,
    Vectorcall,
    Thiscall,
    Regcall,
    Variadic = Cdecl // Variadic functions are always "Cdecl" on all supported compilers
};
```
Note that in all cases above only those calling conventions that are supported by a given compiler can be returned by "*DefaultCallingConvention\_v*" (so for instance, "*Vectorcall*" will never be returned on GCC since it doesn't support this calling convention).
</blockquote></details>

<a name="DefaultCallingConventionName_v"></a><details><summary>DefaultCallingConventionName\_v</summary>

<blockquote>

```C++
template <bool IsFreeFunc>
inline constexpr tstring_view DefaultCallingConventionName_v;
```
Same as [DefaultCallingConvention\_v](#defaultcallingconvention_v) just above but returns this as a string (of type "*tstring\_view*" - see [TypeName\_v](#typename_v) for details). Note that unlike the "*CallingConvention*" enumerators returned by [DefaultCallingConvention\_v](#defaultcallingconvention_v) however (whose first letter is always uppercase), the strings returned by the following template are always returned in lowercase ("*cdecl*", "*stdcall*", "*fastcall*", "*vectorcall*", "*thiscall*" or "*regcall*"). See [DefaultCallingConvention\_v](#defaultcallingconvention_v) for details (since \"*DefaultCallingConventionName\_v*\" simply returns the string equivalent as just described).
</blockquote></details>

<a name="ForEachArg"></a><details><summary>ForEachArg</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,>
          FOR_EACH_ARG_FUNCTOR_C ForEachArgFunctorT>
inline constexpr bool ForEachArg(ForEachArgFunctorT &&);
```
Iterates all arguments for function "*F*", invoking "*ForEachArgFunctorT::operator()"*" for each function argument type. "*ForEachArgFunctorT*" must be a (default constructible) functor (class/struct) whose "*operator()*" member is a template in its own right, as per the following example (note that an invalid type will result in a constraints violation in C++20 or later via the FOR_EACH_ARG_FUNCTOR_C concept seen in the template's declaration, or a "*static\_assert*" in C++17 otherwise). See the full example further below which utilizes this but relies on an equivalent lambda template instead (which became available in C++20 - see full example further below for details):

```C++
///////////////////////////////////////////////////////
// Meets the requirements of the "ForEachArgFunctorT"
// template arg of "ForEachArg()" (i.e., its a functor
// whose "operator()" member is a template as seen
// below)
///////////////////////////////////////////////////////
struct DisplayArgType
{
    //////////////////////////////////////////////////////
    // Specialized and invoked by "ForEachArg" for each
    // arg type in the "F" template arg you pass the
    // latter template (so effectively loops through all
    // args in "F", invoking the following function for
    // each - the following is specialized for each arg
    // in function "F")
    //////////////////////////////////////////////////////
    template <std::size_t I, // Index of the zero-based function arg in "F"
              typename ArgTypeT> // Type of the "Ith" function arg ("I" just above) in "F"
    bool operator()() noexcept
    {
        ///////////////////////////////////////////////
        // See full example further below for details
        // (same code as the following but demos the
        // use of an equivalent C++20 lambda template
        // instead)
        ///////////////////////////////////////////////
        using namespace StdExt;
        tcout << (I + 1) << _T(") ") << TypeName_v<ArgTypeT> << _T("\n");

        return true;
    }
};
```

Now simply pass an instance of "*DisplayArgType*" above to "*ForEachArg*()" such as the following (where "*F*" is the function type whose args you wish to iterate, always explicitly passed as seen just below - see the full example further below):

```C++
ForEachArg<F>(DisplayArgType{});
```

The "*operator()*" member of this "*DisplayArgType*" instance will then be invoked once for each arg type in function "*F*", specializing "*operator()*" for each arg. The "*I*" template arg of "*operator()*" will therefore be set to the zero-based index of the arg on each iteration, and the "*ArgTypeT*" template arg will be the function's arg type at that index. "*operator()*" should then return "*true*" to continue iterating the next arg, or "*false*" to abort iteration, analogous to a "*break*" statement in a regular "*for*" loop. "*ForEachArg*" will then return this (i.e., "*true*" if all args are processed or "*false*" if "*operator()*" returns "*false*" to abort processing). Note that if "*F*" has no non-variadic args, i.e., [ArgCount\_v](#argcount_v) is zero (note that variadic args are completely ignored if any - the "..." in "*F*" isn't relevant), then no iterations will occur whatsoever. Your "*operator()*" member will not be called in this case.

Lastly, note you'll rarely need to call this template in practice, since most will usually rely on [ArgType\_t](#argtype_t) or [ArgTypeName\_v](#argtypename_v) instead (to retrieve the type of a specific argument), or other applicable templates as required. The need to loop through all args is comparatively rare.

The following example assumes C++20 or later for the lambda template seen below (lambda templates aren't available until C++20 - you can check if the library's LAMBDA_TEMPLATES_SUPPORTED[^7] macro is #defined to determine if they're supported). If targeting C++17 however you can easily replace it with your own functor instead (recall that a lambda is just syntactic sugar for a functor, where support for lambda templates equivalent to "*DisplayArgType*" above was added to the language in C++20). Simply remove the "*displayArgType*" lambda template from the code below and add "*DisplayArgType*" above instead (declare it outside of function "*main()*" since templates can't be declared at function scope, including member templates in a non-template class declared at function scope). Then simply pass an instance of "*DisplayArgType*" to "*ForEachArg*" as seen further above, i.e., ```ForEachArg<F>(DisplayArgType{});```. Lambda templates are more convenient though (if targeting C++20 or later) because they can be declared on-the-fly (though "*DisplayArgType*" above will work too of course). Note that the following example simply displays (streams) each function arg's type to the [stdout](https://en.cppreference.com/w/cpp/io/c/std_streams) (but you won't even need to do this normally, as you can rely on [DisplayAllFunctionTraits](#displayallfunctiontraits) instead, which displays all traits for a function and even relies on "*ForEach*" to displays its args, similar to the following example):

```C++
// Standard C++ header (to support "tcout" described below)
#include <iostream>

////////////////////////////////////////////////////////
// Only file in this repository you need to explicitly
// #include (see "Usage" section earlier)
////////////////////////////////////////////////////////
#include "FunctionTraits.h"

int main()
{
    // Everything declared in this namespace
    using namespace StdExt;

    // Free function whose arg types you wish to iterate
    float SomeFunc(const std::string &, double, int);

    // Type of the above function
    using F = decltype(SomeFunc);

    //////////////////////////////////////////////////////////////
    // Lambda template that will be invoked just below, once for
    // each arg in function "F" (where template arg "I" is the
    // zero-based "Ith" argument in function "F", and "ArgTypeT"
    // is its type). Note that lambda templates are available in
    // C++20 and later only. For C++17 (earlier versions aren't
    // supported), you need to rely on the "DisplayArgType"
    // struct seen further above instead (equivalent to the
    // following lambda but relies on a traditional functor
    // instead). Lambda templates are more convenient however
    // since they can be locally declared on-the-fly.
    //////////////////////////////////////////////////////////////
    const auto displayArgType = []<std::size_t I, typename ArgTypeT>() noexcept
                                {
                                    //////////////////////////////////////////////////////////
                                    // Display the type of the (zero-based) "Ith" arg in
                                    // function "F" (where template arg "I" just above stores
                                    // this index) and "ArgTypeT" is that arg's type. Note
                                    // that we never come through here if function "F" has no
                                    // args (or just variadic args only - variadic args aren't
                                    // processed). Also note that the following call to
                                    // "TypeName_v" simply returns "ArgTypeT" as a compile-time
                                    // string ("std::basic_string_view"). See "TypeName_v" in
                                    // the "Helper templates" section (consult its entry
                                    // there - it can be passed any C++ type).
                                    //
                                    // Finally, note that on non-Microsoft platforms, "tcout"
                                    // and "_T" (both automatically declared in namespace
                                    // "StdExt" when you #include "FunctionTraits.h"), always
                                    // resolve to "std::cout" and (for _T) simply the arg
                                    // you pass it (_T effectively does nothing). On
                                    // Microsoft platforms however they resolve to
                                    // "std::wcout" and L##x (for the given macro arg "x")
                                    // respectively when compiling for Unicode (normally the
                                    // case), or "std::cout" and (for _T) the arg you pass
                                    // it otherwise (_T effectively does nothing in the
                                    // latter case). For further details in general, see the
                                    // comments preceding the definitions of "_T", "tcout"
                                    // and "tchar" in "CompilerVersions.h" (for non-Microsoft
                                    // platforms but those targeting Microsoft may wish to
                                    // review this as well).
                                    //////////////////////////////////////////////////////////
                                    tcout << (I + 1) << _T(") ") << TypeName_v<ArgTypeT> << _T("\n");

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

    return 0;
}
```

</blockquote></details>

<a name="FunctionClassification_v"></a><details><summary>FunctionClassification\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr FunctionClassification FunctionClassification_v;
```
Variable storing the classification of function "*F*" returned as a "*FunctionClassification*" enumerator declared in "*FunctionTraits.h*":
```C++
enum class FunctionClassification
{
    ////////////////////////////////////////////////////////////////////////////////
    // Template arg "F" is a free function defined as any of the following:
    //
    // 1) Any type satisfying "std::is_function" not including abominable functions
    //    (see enum "Abominable" further below). "Free" functions therefore never
    //    contain the function qualifiers "const" and/or "volatile" and/or "&" or
    //    "&" (if they do then they are classified as enumerator "Abominable" below
    //    instead).
    // 2) Optionally cv-qualified pointer to 1 above, i.e., a pointer to any type
    //    satisfying "std::is_function" (non-static member function pointers never
    //    qualify - they are always classified as enum "NonStaticMember" below
    //    instead). Note that pointers to abominable functions aren't possible in
    //    C++ so if a pointer to a function satisfying "std::is_function" is
    //    encountered, then the function is guaranteed not to have the qualifiers
    //    "const" and/or "volatile" and/or "&" or "&" (hence it's always a "free"
    //    function).
    // 3) Reference (lvalue or rvalue) to 2 above, i.e., a reference to an optionally
    //    cv-qualified pointer to any type satisfying "std::is_function" (references
    //    to non-static member function pointers never qualify - they are always
    //    classified as enum "NonStaticMember" below instead). Note that if
    //    encountered then the function the (referenced) pointer points to is
    //    guaranteed not to be abominable (since pointers to abominable functions
    //    aren't possible in C++ - see 2 above).
    // 4) Reference (lvalue or rvalue) to any type satisfying 1 above, i.e., a
    //    reference to any type satisfying "std::is_function" (references to
    //    non-static member functions aren't legal in C++ so they will never be
    //    encountered). Note that references to abominable functions aren't possible
    //    in C++ so if a reference to a function satisfying "std::is_function" is
    //    encountered, the function is guaranteed not to have the qualifiers "const"
    //    and/or "volatile" and/or "&" or "&".
    // 5) Any non-overloaded *static* functor (i.e., "F" is an optionally
    //    cv-qualified class or struct with a non-overloaded, *static* "operator()"
    //    member - static versions of "operator()" were legalized in C++23, though
    //    are normally very rare in practice). The static "operator()" member of
    //    "F" is classified as a free function (as all static member functions are).
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
    //    cv-qualified pointer to any type satisfying "std::is_member_function_pointer").
    //    Note that references to non-static member functions aren't possible in C++
    //    so will never be encountered (only references to non-static member function
    //    *pointers* are allowed).
    // 3) Any non-overloaded, non-static functor including lambdas. In either case
    //    "F" is an optionally cv-qualified class or struct with a non-overloaded,
    //    non-static "operator()" member (lambda types always qualify since the
    //    compiler always generates a functor for the lambda behind the scenes,
    //    known as the lambda's "closure type"). The non-static "operator()" member
    //    of "F" is therefore classified as a non-static member function (as all
    //    non-static member functions are). If the functor's "operator()" member is
    //    static however which was legalized in C++23, then it's classified as enum
    //    "Free" instead (see items 5 and 6 under enum "Free" above). Note that
    //    *generic* lambdas including lambda templates can't be directly passed as
    //    template arg "F" to any of the templates in this library, though they can
    //    still be handled as described in the section "Template arg "F"" (see item
    //    5 here for details). Such lambdas still qualify as "NonStaticMember" but
    //    via item 1 above.
    // 4) Reference (lvalue or rvalue) to 3 above (i.e., "F" is a reference to an
    //    optionally cv-qualified, non-overloaded, non-static functor including
    //    lambdas - see 3 above). Again, if the functor's "operator()" member is
    //    *static* which was legalized in C++23, then it's classified as enum
    //    "Free" instead (see item 5 under enum "Free" above)
    ////////////////////////////////////////////////////////////////////////////////
    NonStaticMember,

    ////////////////////////////////////////////////////////////////////////////////
    // Template arg "F" is an "abominable" function defined as follows (note that
    // pointers and/or references to abominable functions aren't legal in C++ so
    // will never be encountered - a compiler error will result if attempting to
    // create one, hence no criteria exists below to handle them since they're
    // not possible):
    //
    // 1) Any type satisfying "std::is_function" that has the function qualifier(s)
    //    "const" and/or "volatile" and/or "&" or "&" (the definition of an
    //    "abominable" function). If the type satisfies "std::function" but is
    //    neither "const" nor "volatile" nor contains "&" or "&" (none of these
    //    qualifiers are present), then the function is classified as enum "Free"
    //    above instead.
    //
    // See https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html
    // for further details (and the section "C++ function types at a glance"
    // earlier in this README.md file)
    ////////////////////////////////////////////////////////////////////////////////
    Abominable
};
```
</blockquote></details>

<a name="FunctionOrigin_v"></a><details><summary>FunctionOrigin\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr FunctionOrigin FunctionOrigin_v;
```
Variable storing the origin of function "*F*" returned as a "*FunctionOrigin*" enumerator declared in "*FunctionTraits.h*" (details to follow):
```C++
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
    // "std::function" specializations are technically functors).
    ///////////////////////////////////////////////////////////////
    Functor,

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
```
For all function types handled by this library (arg "*F*" of all templates - see [Template arg "F"](#templateargF)), if "*F*" isn't a *fundamental* function type which for the purposes of this template means any type other than a [raw](#rawfunctiontypes) function type, or a pointer or a reference to a [raw](#rawfunctiontypes) function type (or a reference to a pointer to one), including *non-static* member functions pointers (those satisfying [std::is\_member\_function\_pointer](https://en.cppreference.com/w/cpp/types/is_member_function_pointer)), then the function represented by "*F*" must originate from "*F*" in some way but "*F*" isn't actually the function itself (nor a pointer or reference to one or reference to a pointer to one). "*F*" is the function's "*origin*" instead. The enumerator above therefore identifies what "*F*" actually is (since "*F*" is merely what the function it represents originates from). This template is therefore used to return this enumerator.

For instance, if "*F*" is a *non-static* functor instead of an actual [raw](#rawfunctiontypes) function type or a pointer or reference to a [raw](#rawfunctiontypes) function type (or a reference to a pointer to one), then it's not a *fundamental* function type so "*FunctionOrigin\_v*" will return what "*F*" actually is. In this case it will be "*FunctionOrigin::Functor*" seen above since that's where the function represented by "*F*" originates from (a functor, i.e., a class or struct with a *non-static* "*operator()*" member that originates from "*F*" but "*F*" itself isn't a [raw](#rawfunctiontypes) function type or a pointer or reference to one or a reference to a pointer to one). Note that functors with a *static* "operator()" member, which were legalized in C++23, will return "*FunctionOrigin::StaticFunctor*" instead.

Note that if "*F*" *is* a *fundamental* function type however then "*FunctionOrigin::None*" is always returned instead. However, it could be argued that even the *fundamental* function types arguably warrant their own "*FunctionOrigin*" enumerator, but they always yield "*FunctionOrigin::None*" as noted. While these could be easily assigned their own "*FunctionOrigin*" enumerator as well, such as, say, "*FunctionOrigin::Raw*", "*FunctionOrigin::Pointer*", "*FunctionOrigin::Reference*", etc., since these are fundamental types in the C++ type system it was decided not to. If you need to determine what "*F*" is when "*FunctionOrigin\_v*" returns "*FunctionOrigin::None*", then the language already provides templates [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function), [std::is\_pointer](https://en.cppreference.com/w/cpp/types/is_pointer) (for detecting free function pointers - always returns "*false*" for *non-static* member function pointers), [std::is\_member\_function\_pointer](https://en.cppreference.com/w/cpp/types/is_member_function_pointer) and [std::is\_reference](https://en.cppreference.com/w/cpp/types/is_reference). These can be combined as required and applied to "*F*" (returned by [FunctionType\_t](#functiontype_t) if you require it) to determine what "*F*" is whenever "*FunctionOrigin\_v*" returns "*FunctionOrigin::None*". Providing dedicated "*FunctionOrigin*" enumerators for the *fundamental* function types instead of "*FunctionOrigin::None*" was therefore deemed somewhat redundant and potentially confusing. The *fundamental* function types were therefore simply lumped together under one blanket enumerator, "*FunctionOrigin::None*". All other possible types of "*F*" will assume one of the other dedicated enumerators above. Note that the situation for "*FunctionOrigin::None*" may be reevaluated in a future release (should the situation warrant it).
</blockquote></details>

<a name="FunctionRawType_t"></a><details><summary>FunctionRawType\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionRawType_t; // Note: Named "RawType_t" in "struct FunctionTraits"
```
Type alias of the [raw](#rawfunctiontypes) (native) C++ function type for "*F*" (see [C++ Function Type Format](#cppfunctiontypeformat)), so just "*F*" itself after removing any pointer and/or reference that may exist in "*F*", if any (resulting in the actual C++ function type that "*F*" refers to which always satisfies [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function)). If "*F*" is a functor (i.e., [FunctionOrigin\_v](#functionorigin_v) returns "*FunctionOrigin::Functor*" or "*FunctionOrigin::StaticFunctor*"), then the [raw](#rawfunctiontypes) type of its "*operator()*" member is returned. If "*F*" is a [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specialization (i.e., [FunctionOrigin\_v](#functionorigin_v) returns "*FunctionOrigin::StdFunction*"), then the type the [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) was specialized on is returned (i.e., the function it wraps which the language itself requires must already be a [raw](#rawfunctiontypes) function type). Note that this library supports pointers and references to functions (and references to pointers to functions), as well as functors (including lambdas) and [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specializations, in addition to [raw](#rawfunctiontypes) C++ function types (see [Template arg "F"](#templateargf)), so this alias simply yields the actual (native) C++ function type of "*F*" which always satisfies [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) as noted (again, see [C++ Function Type Format](#cppfunctiontypeformat)). Also see [FunctionType\_t](#functiontype_t) if you wish to simply return the function type that "*F*" represents as-is (so any pointer and/or reference in the function represented by "*F*" remains intact).

*Examples:*
```C++
/////////////////////////////////////////////////////
// Returns the following (identical to the template
// arg being passed however since it's already a
// "raw" function type):
//
//    int (float) noexcept
/////////////////////////////////////////////////////
using RawFunc1_t = FunctionRawType_t<int (float) noexcept>;

//////////////////////////////////////////////////
// Returns the following ("raw" type of the free
// function this pointer points to):
//
//    int (float)
//////////////////////////////////////////////////
using RawFunc2_t = FunctionRawType_t<int (*)(float)>;

////////////////////////////////////////////////////////
// Returns the following ("raw" type of the non-static
// member function this pointer points to):
//
//    int (float) const noexcept
////////////////////////////////////////////////////////
using RawFunc3_t = FunctionRawType_t<int (SomeClass::*)(float) const noexcept>;

// Some lambda
constexpr auto lambda = [](float whatever) noexcept
                          {
                              int rc;
                              // ...
                              return rc;
                          };

////////////////////////////////////////////////////
// Returns the following ("raw" type of the above
// lambda which behind the scenes is just a
// functor generated by the compiler so the "raw"
// type of its "operator()" member is returned -
// note that the "const" below is implicitly
// added by the compiler in the above example):
//
//    int (float) const noexcept
////////////////////////////////////////////////////
using RawFunc4_t = FunctionRawType_t<decltype(lambda)>;

// Some "std::function" specialization
using StdFunction_t = std::function<int (float)>

/////////////////////////////////////////////////
// Returns the following (arg type passed to
// "std::function" above which is the function
// that "std::function" wraps - it's always a
// "raw" function in its own right since
// "std::function" only legally takes such types,
// though with certain restrictions that aren't
// relevant here - "FunctionRawType_t" simply
// returns it):
//
//    int (float)
/////////////////////////////////////////////////
using RawFunc5_t = FunctionRawType_t<StdFunction_t>;
```
</blockquote></details>

<a name="FunctionRawTypeName_v"></a><details><summary>FunctionRawTypeName\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view FunctionRawTypeName_v; // Note: Named "RawTypeName_v" in "struct FunctionTraits"
```
Same as [FunctionRawType\_t](#functionrawtype_t) just above but returns this as a string (of type "*tstring\_view*" - see [TypeName\_v](#typename_v) for details). A function whose [raw](#rawfunctiontypes) type is ```int (const Test &) noexcept``` for instance would therefore be (literally) returned as "*int (const Test &) noexcept*" (quotes not included). Note that the exact format of a given function's string is platform specific however, so it might not be identical on all platforms. Slight differences can occur depending on the function including differences in the string representation of its return type and arg types. See both [FunctionRawType\_t](#functionrawtype_t) and [TypeName\_v](#typename_v) for details (the former since \"*FunctionRawTypeName\_v*\" just defers to it, and the latter which is used to create the actual string).
</blockquote></details>

<a name="FunctionReference_v"></a><details><summary>FunctionReference\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr FunctionReference FunctionReference_v;
```
Variable storing the ref-qualifier of "*F*" returned as a "*FunctionReference*" enumerator declared in "*FunctionTraits.h*":
```C++
enum class FunctionReference
{
    None,   // No ref-qualifiers present
    LValue, // "&" ref-qualifier present
    RValue  // "&&" ref-qualifier present
};
```
Set to "*FunctionReference::None*" if the function isn't declared with any ref-qualifiers ("*&*" or "*&&*", and in practice neither is declared usually), "*FunctionReference::LValue*" if the function is declared with the "*&*" ref-qualifier, or "*FunctionReference::RValue*" if the function is declared with the "*&&*" ref-qualifier. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v)\<F\> returns "*FunctionClassification::Free*"), then this variable always returns "*FunctionReference::None*" (since free functions can't be declared with ref-qualifiers in C++).
</blockquote></details>

<a name="FunctionReferenceName_v"></a><details><summary>FunctionReferenceName\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          bool UseAmpersands = true>
inline constexpr tstring_view FunctionReferenceName_v;
```
Same as [FunctionReference\_v](#functionreference_v) just above but returns this as a string (of type "*tstring\_view*" - see [TypeName\_v](#typename_v) for details). Note however that the following template also takes an extra template arg besides function "*F*", a "*bool*" called "*UseAmpersands*", indicating whether the returned string should be returned as "*&*" or "*&&*" (if the function is declared with an "*&*" or "*&&*" ref-qualifier respectively), or "*lvalue*" or "*rvalue*" otherwise (always in lowercase as seen, which differs from the "*FunctionReference*" enumerator names themselves, as returned by [FunctionReference\_v](#functionreference_v)). Defaults to "*true*" if not specified so returns "*&*" or "*&&*" by default. Not applicable however if no ref-qualifiers are present in "*F*" (i.e., [FunctionReference\_v](#functionreference_v) returns "*FunctionReference::None*"), in which case an empty string ("") is always returned (which is usually the case - ref-qualifiers in functions are rarely encountered in practice). See [FunctionReference\_v](#functionreference_v) for further details (since \"*FunctionReferenceName\_v*\" simply returns the string equivalent as just described).
</blockquote></details>

<a name="FunctionType_t"></a><details><summary>FunctionType\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionType_t;
```
Type alias identical to "*F*" itself unless "*F*" is either a functor (i.e., [FunctionOrigin\_v](#functionorigin_v) returns "*FunctorOrigin::Functor*" or "*FunctorOrigin::StaticFunctor*"), in which case it's a type alias for the "*operator()*" member of the functor, or a [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specialization (i.e., [FunctionOrigin\_v](#functionorigin_v) returns "*FunctionOrigin::StdFunction*"), in which case it's a type alias for the function type that the [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) was specialized on (i.e., its template arg). To retrieve the [raw](#rawfunctiontypes) C++ type represented by "*F*" (just "*F*" itself after removing any pointer and/or reference in "*F*" if any, again, referring to "*F::operator()*" if "*F*" is a functor, or, if "*F*" is a [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specialization, the function type it was specialized on), then see [FunctionRawType\_t](#functionrawtype_t) instead.
</blockquote></details>

<a name="FunctionTypeName_v"></a><details><summary>FunctionTypeName\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view FunctionTypeName_v;
```
Same as [FunctionType\_t](#functiontype_t) just above but returns this as a string (of type "*tstring\_view*" - see [TypeName\_v](#typename_v) for details). If function "*F*" is ```int (Test::* const &)(const std::string &) noexcept``` for instance (a *reference* to a "*const*" non-static member function pointer which would be rare in practice but just to emphasize the situation), then the returned string would therefore (literally) be "*int (Test::\* const &)(const std::string &) noexcept*" (quotes not included). Note that the exact format of a given function's string is platform specific however, so it might not be identical on all platforms. Slight differences can occur depending on the function including differences in the string representation of its return type, arg types, and for non-static member functions, class type (a given compiler might add the prefix "*class*" to a class-based arg for instance, so the latter example might appear as "*int (Test::\* const &)(const class std::string &) noexcept*" instead, i.e., the string "*class*" now precedes the "*std::string*" arg type). See both [FunctionType\_t](#functiontype_t) and [TypeName\_v](#typename_v) for details (the former since \"*FunctionTypeName\_v*\" just defers to it, and the latter which is used to create the actual string).
</blockquote></details>

<a name="IsArgTypeMatch_v"></a><details><summary>IsArgTypeMatch\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F1,
          TRAITS_FUNCTION_C F2,
          std::size_t I,
          bool ReturnFalseIfOneHasArgOtherDoesnt = true>
REQUIRES_I_VALID_IN_BOTH_OR_RETURN_FALSE_IF_ONE_HAS_ARG_OTHER_DOESNT(F1, F2, I, ReturnFalseIfOneHasArgOtherDoesnt)
inline constexpr bool IsArgTypeMatch_v;
```
"*bool*" variable set to "*true*" if the arg type at (zero-based) index "*I*" of function "*F1*" is the same as the corresponding arg of function "*F2*" or "*false*" otherwise. This template is the same as passing the "*I*th" arg type of both functions to [std::is\_same\_v](https://en.cppreference.com/w/cpp/types/is_same), so "*true*" is returned if they match (the types are the same), or "*false*" otherwise. Note that if index "*I*" is greater than or equal to the number of arguments of either function but not both, then "*false*" is returned if "*ReturnFalseIfOneHasArgOtherDoesnt*" is "*true*" (the default), or a compiler error occurs otherwise (read on). In this case arg "*I*" doesn't exist in one function but does in the other, so for the purposes of this template the arg types at index "*I*" are considered unequal (because the arg exists in one function but not the other - they are therefore considered mismatched). The behavior is analogous to substituting "*void*" for the arg type that doesn't exist and comparing it with the arg type that does exist. The latter can never be of type "*void*" however since "*void*" arg (parameter) types aren't allowed in C++ so they'll always compare unequal (note that even the "*void*" in an empty argument list declared using the syntax "*(void)*" is just a syntactic convention, and does *not* qualify as an actual function parameter of type "*void*", which is always an invalid function parameter type). If you explicitly pass "*false*" for "*ReturnFalseIfOneHasArgOtherDoesnt*" however (again, relevant only if "*I*" is greater than or equal to the number of arguments in one function but not both - "*ReturnFalseIfOneHasArgOtherDoesnt*" is ignored otherwise), or "*I*" is greater than or equal to the number of arguments of *both* functions ("*ReturnFalseIfOneHasArgOtherDoesnt*" is *always* irrelevant in the latter case), then a constraints violation will occur in C++20 or later (via the REQUIRES\_I\_VALID\_IN\_BOTH\_OR\_RETURN\_FALSE\_IF\_ONE\_HAS\_ARG\_OTHER\_DOESNT concept seen in the template's declaration), or a "*static\_assert*" in C++17. Note that when "*I*" is greater than or equal to the number of arguments of both functions then an error is always guaranteed as described since no such argument exists in either function so the comparison is considered nonsensical (hence "*ReturnFalseIfOneHasArgOtherDoesnt*" is always irrelevant in this case as noted).

<ins>***IMPORTANT***</ins>:<br /> Note that this template targets non-variadic arguments only. If either function is a variadic function, i.e., [IsVariadic\_v](#isvariadic_v) returns "*true*" for either, then the template completely ignores this ("*I*" cannot be used to target variadic arguments - the "..." in either function is irrelevant).

*Examples:*
```C++
// Sample functions
using F1 = float (char, double);
using F2 = float (const std::string &, double, int);

//////////////////////////////////////////////////
// Compares arg 2 of both functions, returning
// true since they're the same (both a "double").
// Passing 1 here to target the 2nd arg (since
// index is zero-based).
//////////////////////////////////////////////////
constexpr bool isArg2TypeMatch_v = IsArgTypeMatch_v<F1, F2, 1>;

/////////////////////////////////////////////////
// Compare the 3rd arg this time but F1 has no
// such arg, only F2 does (an "int"). By default
// false will therefore be returned because the
// "ReturnFalseIfOneHasArgOtherDoesnt" template
// arg defaults to true. If false is explicitly
// passed for this however then a compiler error
// will occur instead (see details above).
/////////////////////////////////////////////////
constexpr bool isArg3TypeMatch_v = IsArgTypeMatch_v<F1, F2, 2>;

//////////////////////////////////////////////////
// Compare the 4th arg this time but neither
// function has such an arg, so a compiler error
// is guaranteed, as described earlier (the
// "ReturnFalseIfOneHasArgOtherDoesnt" template
// arg is ignored in this case - it only applies
// when targeting a non-existent arg in one
// function but not both)
//////////////////////////////////////////////////
constexpr bool isArg4TypeMatch_v = IsArgTypeMatch_v<F1, F2, 3>; // Error always occurs!
```
</blockquote></details>

<a name="IsArgListEmpty_v"></a><details><summary>IsArgListEmpty\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsArgListEmpty_v;
```
"*bool*" variable set to "*true*" if the function represented by "*F*" has an empty arg list (it has no args whatsoever including variadic args), or "*false*" otherwise. If "*true*" then note that [ArgCount\_v](#argcount_v) is guaranteed to return zero (0) and [IsVariadic\_v](#isvariadic_v) is guaranteed to return "*false*".<br /><br /><ins>***IMPORTANT***</ins>:<br />Note that you should rely on this helper to determine if a function's argument list is completely empty opposed to checking the [ArgCount\_v](#argcount_v) helper for zero (0), since the latter returns zero only if "*F*" has no non-variadic args. If it has variadic args but no others, i.e., its argument list is "(...)", then the argument list isn't empty even though [ArgCount\_v](#argcount_v) returns zero (since it still has variadic args). Caution advised.
</blockquote></details>

<a name="IsArgTypeSame_v"></a><details><summary>IsArgTypeSame\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I,
          typename T,
          bool CompareWithVoidIfInvalidIndex = false>
REQUIRES_I_LESS_THAN_ARG_COUNT_OR_COMPARE_WITH_VOID(F, I, CompareWithVoidIfInvalidIndex)
inline constexpr bool IsArgTypeSame_v;
```
"*bool*" variable set to "*true*" if the (zero-based) "*Ith*" arg of function "*F*" is the same as the given type "*T*" or "*false*"otherwise. This template is just a thin wrapper around [std::is\_same\_v](https://en.cppreference.com/w/cpp/types/is_same), where the latter template is passed the type of the "*Ith*" arg in function "*F*" and type "*T*". It therefore simply compares the type of the "*Ith*" arg in function "*F*" with type "*T*". This is sometimes required when testing function argument types so this template provides a convenient wrapper.

***IMPORTANT***: Please note that when using this template, all C++ rules for function parameter types remain in effect, so if a given function parameter type decays into another for instance, such as an array type (since an array type decays into a pointer type when used as a function parameter, e.g., a function parameter declared as ```const char [10]``` actually decays into ```const char *```), then the type you pass for template arg "*T*" must take this into account (i.e., passing ```const char [10]``` in the latter case will result in a "*false*" return value because the parameter isn't actually this type, it's a pointer type so passing ```const char *``` instead will return "*true*" instead - see example further below). Note that function types also have a similar issue (a function parameter declared as a function type decays into a function *pointer* instead). Other types of behavior to watch out for can also exist, such as when applying the cv-qualifiers "*const*" and/or "*volatile*" to a function's *value* parameters (though the "*volatile*" keyword itself has been deprecated on such types as of C++20), since they are effectively ignored by the compiler in such cases (they effectively don't exist in the function's own type). While such cv-qualifiers are respected inside the function's body if declared in the function's actual definition, they are *not* part of the function's type. A value parameter like ```const int``` for instance is therefore effectively treated by the compiler as just ```int``` instead (see example further below). Passing ```const int``` in this case for template arg "*T*" would therefore return "*false*" (passing ```int``` instead will return true). Cv-qualifiers for *non-value* parameter types however normally remain fully in effect (such as ```const std::string &``` - the "*const*" is relevant here when invoking this template). Caution should therefore be exercised to ensure that the type you pass for template arg "*T*" takes these issues into consideration (since the C++ rules for function parameter types aren't always WYSIWYG). The examples further below demonstrate some of these issues.

Finally, please note that if "*I*" is greater than or equal to the number of args in "*F*" (i.e., it refers to a non-existent arg - note that variadic args if any are ignored), then a compiler error will occur if "*CompareWithVoidIfInvalidIndex*" is "*false*" (the default - read on), or "*void*" will be substituted for the non-existent "*Ith*" arg otherwise. In the latter case (i.e., "*I*" is invalid and "*true*" is explicitly passed for "*CompareWithVoidIfInvalidIndex*"), since "*void*" will be substituted for the non-existent "*Ith*" arg, it will therefore be compared to template arg "*T*" so "*false*" is guaranteed to be returned so long as you don't pass (optionally cv-qualified) "*void*" itself for template arg "*T*" (since it will then match the "*void*" being substituted for non-existent arg "*I*" and "*true*" will therefore be returned instead - more on this in a moment). A "*false*" return in this case simply means that arg "*I*" doesn't exist but its absence is effectively treated as if it does exist but simply doesn't match template arg type "*T*" (the type you're comparing it with). Note that you'll handle things this way in those cases where you wish for false to be returned not only when a real mismatch occurs, i.e., when "*I*" is a valid arg but doesn't match template arg "*T*", but a "*mismatch*" occurs due to a non-existent arg as well (because there is no "*Ith*" arg in function "*F*"). Note that most users will normally be passing a non-void type for template arg "*T*" in order to compare it with an existing arg "*I*", which therefore guarantees a "*false*" return in this scenario (when arg "*I*" doesn't exist). If you pass (optionally cv-qualified) "*void*" itself for arg "*T*" however, then "*true*" will be returned instead in this scenario (when arg "*I*" doesn't exist and "*CompareWithVoidIfInvalidIndex*" is "*true*"), since you'll then be comparing your "*void*" arg "*T"* with the "*void*" that's substituted for non-existent arg "*I*", which therefore match. A "*true*" return value in this case therefore indicates that arg "*I*" simply doesn't exist, and a "*false*" return value means that it does. If you wish to check whether arg "*I"* exists or not using this technique then you're free to do so. Otherwise there's no reason to pass "*void*" for template arg "*T*" normally since function arg (parameter) types can't be "*void*" in C++ (so you won't normally pass "*void*" for template arg "*T*" since it will always result in a "*false*" return value unless "*I*" itself is illegal - note that even the "*void*" in an empty argument list declared using the syntax "*(void)*" is just a syntactic convention, and does *not* qualify as an actual function parameter of type "*void*", which is always an invalid function parameter type). In fact, if you wish to check for a legal value of "*I*" you can simply test that *I*" is less than [ArgCount\_v](#argcount_v) instead (which is cleaner). You should therefore pass "*void*" for template arg "*T*" only if you have some need to check for a legal value of "*I*" using this template instead ("*IsArgTypeSame_v*"), and explicitly pass "*true*" for "*CompareWithVoidIfInvalidIndex*" in this case (so a "*true*" return value then indicates that arg "*I*" doesn't exist and a "*false"* return value means that it does). Note that if "*CompareWithVoidIfInvalidIndex*" is "*false*" however (again, the default), then an error will always occur instead as noted earlier (if arg "*I*" doesn't exist). This will be a constraints violation in C++20 or later via the REQUIRES\_I\_LESS\_THAN\_ARG\_COUNT\_OR\_COMPARE\_WITH\_VOID concept seen in the template's declaration, or a "*static\_assert*" in C++17 otherwise. See the "*ReturnVoidIfInvalidIndex*" arg in [ArgType\_t](#argtype_t) for further details on this parameter (more suitably named "*CompareWithVoidIfInvalidIndex*" in the template you're now reading, but it corresponds with the "*ReturnVoidIfInvalidIndex*" arg of [ArgType\_t](#argtype_t)).

*Examples:*
```C++
// Sample function
using F = float (const char [10], const int);

//////////////////////////////////////////////////////////
// Compares the 1st arg of "F" with a "const char *",
// returning true since it's an exact match (passing 0
// here to target the 1st arg since index is zero-based).
// Note that the arg itself is declared as an array in
// this case however (const char [10]), but arrays decay
// into pointers when used as function parameters so it's
// actually a "const char *" in this case. We therefore
// pass that just below which results in "true" being
// returned (if we passed "const char[10]" instead then
// "false" would be returned). See details further above.
//////////////////////////////////////////////////////////
constexpr bool isArg1TypeSame_v = IsArgTypeSame_v<F, 0, const char *>;

/////////////////////////////////////////////////////////
// Compares the 2nd arg of "F" with an "int", returning
// true since it's an exact match (passing 1 here to
// target the 2nd arg since index is zero-based). Note
// that the arg itself is declared as "const" in this
// case however ("const int"), but since it's a value
// parameter the "const" is effectively ignored by the
// compiler as far as the function's type is concerned
// (though if declared as "const" in the function's
// definition it will be respected inside the function's
// body but its type is still treated as just a plain
// "int" in the function's own type due to C++ rules).
// We therefore pass just a plain "int" below which
// results in "true" being returned (if we passed
// "const int" instead then "false" would be returned).
// See details further above.
/////////////////////////////////////////////////////////
constexpr bool isArg2TypeSame_v = IsArgTypeSame_v<F, 1, int>;

/////////////////////////////////////////////////////////
// Compares the 3rd arg of "F" with an "int" (passing 2
// here to target the 3rd arg since index is
// zero-based), but since there is no 3rd arg in "F" a
// compiler error will occur by default (since the
// template's "CompareWithVoidIfInvalidIndex" arg
// defaults to false). A constraints violation will
// therefore occur in C++20 or later or a
// "static_assert" in C++17. Explicitly passing "true"
// for the "CompareWithVoidIfInvalidIndex" arg will
// avoid the error however, by substituting "void" for
// the non-existent 3rd arg but "void" isn't a valid
// function parameter type in C++ so it can never be
// encountered in a function (note that even in
// functions where the parameter list is "(void)", this
// is syntactically equivalent to an empty argument list
// so the "void" in "(void)" doesn't qualify as an actual
// function arg). The substituted "void" will therefore
// be compared with the type we're passing ("int" in
// this case) so "false" will be returned (indicating
// the 3rd arg doesn't match the "int" because there is
// no 3rd arg, though the reason they don't match isn't
// known to the caller - it's either because the 3rd arg
// doesn't exist as in this case or it does exist but
// isn't an "int", but in either case the caller doesn't
// know or care). If we pass "void" itself in the
// following call however instead of "int", then "true"
// will be returned instead since we'd then be comparing
// the "void" that was substituted for the non-existent
// 3rd arg with the "void" being passed here, which are
// therefore the same. True in this case however
// indicates a non-existent arg. Therefore pass "void"
// only if you wish to check for a non-existent arg
// using this technique, assuming
// "CompareWithVoidIfInvalidIndex" is also "true" or a
// compiler error will occur otherwise as described
// above (but again, as described earlier, if you need
// to check for a non-existent arg then it's cleaner to
// just check that "I" is less than "ArgCount_v" - if so
// then arg "I" exists, otherwise it doesn't).
/////////////////////////////////////////////////////////
constexpr bool isArg3TypeSame_v = IsArgTypeSame_v<F, 2, int>;
```
</blockquote></details>

<a name="IsArgTypesMatch_v"></a><details><summary>IsArgTypesMatch\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F1,
          TRAITS_FUNCTION_C F2,
          bool IgnoreVariadic = false>
inline constexpr bool IsArgTypesMatch_v;
```
"*bool*" variable set to "*true*" if the arg types of function "*F1*" match the arg types of function "*F2*" or "*false*" otherwise. This function is equivalent to calling [std::is\_same\_v](https://en.cppreference.com/w/cpp/types/is_same) for each corresponding arg type in both functions, returning "*true*" if all of them match or "*false*" otherwise (but "*true*" is only returned if the variadic status of both functions also match unless this is ignored via the "*IgnoreVariadic*" template arg - read on). If the number of arguments aren't the same in both functions however then "*false*" is guaranteed to be returned (since their arg lists aren't the same in this case).

Note that in addition to the check for matching non-variadic args just described (which must always match or "*false*" is guaranteed to be returned), the variadic status of both functions must also match if the "*IgnoreVariadic*" template arg is "*false*" (the default). This simply means that both functions must either be variadic or not variadic so they also match in this regard. If they don't match (one's variadic and the other isn't) then "*false*" is guaranteed to be returned. In this case the argument lists are considered to be mismatched even if the non-variadic args are the same (since one function's variadic and the other isn't - their args are considered mismatched based on this alone). If you wish to ignore the variadic status of each function however then explicitly pass "*true*" for the "*IgnoreVariadic*" arg. In this case only the *non-variadic* args of each function are compared as described above. The variadic status of each function is completely ignored. Please note that the variadic status of two functions can also be compared via a simple check for [IsVariadic\_v](#isvariadic_v)\<F1\> == [IsVariadic\_v](#isvariadic_v)\<F2\>.

*Example:*
```C++
//////////////////////////////////////////////
// Lambda whose arg types we wish to compare
// with "Test::Whatever" below
//////////////////////////////////////////////
constexpr auto myLambda = [](std::string_view someStr,
                             float someFloat) noexcept
                            {
                                int rc;

                                // ...

                                return rc;
                            };
class Test
{
public:
    ////////////////////////////////////////
    // Function whose arg types we wish to
    // compare with the lambda's above
    ////////////////////////////////////////
    int Whatever(std::string_view, float) const noexcept;
};

using F1 = decltype(myLambda);
using F2 = decltype(&Test::Whatever);

////////////////////////////////////////////////////
// Returns true since "F1" and "F2" have matching
// args AND their variadic status also matches
// (latter always checked by default as per the
// template's "IgnoreVariadic" arg, which defaults
// to "false" so the variadic status of each must
// match, either both variadic or not variadic,
// in this case not variadic which is usually the
// case for most functions - pass "true" for
// "IgnoreVariadic" to ignore the variadic status
// of both functions so only the non-variadic args
// are compared)
///////////////////////////////////////////////////
constexpr bool isArgTypesMatch_v = IsArgTypesMatch_v<F1, F2>;
```
</blockquote></details>

<a name="IsArgTypesSame_v"></a><details><summary>IsArgTypesSame\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          AndVariadic AndVariadicT,
          typename... ArgTypesToCompareT>
inline constexpr bool IsArgTypesSame_v;
```
"*bool*" variable set to "*true*" if the arg types of function "*F*" match the types passed via the "*ArgTypesToCompareT*" template args (parameter pack) or "*false*" otherwise (assuming "*F*" is also variadic or not as per the "*AndVariadicT*" template arg - details to follow). This template is effectively the same as calling [std::is\_same\_v](https://en.cppreference.com/w/cpp/types/is_same) for each arg in "*F*", passing the type of each arg in "*F*" as the 1st template arg to [std::is\_same\_v](https://en.cppreference.com/w/cpp/types/is_same), and the corresponding type in "*ArgTypesToCompareT*" as the second template arg. It therefore simply compares the type of each arg in function "*F*" with each corresponding type in "*ArgTypesToCompareT*" (in argument order as would be expected). If the number of args in "*F*" doesn't match the number of types in "*ArgTypesToCompareT*" then "*false*" is always returned (i.e., it's the equivalent of comparing two function argument lists of different sizes which are clearly not the same). Note that if "*F*" has no (non-variadic) args then "*true*" can only be returned if no types are passed for "*ArgTypesToCompareT*", i.e., it's an empty parameter pack (but whether "*true*" or "*false*" is returned is still subject to the "*AndVariadicT*" template arg described below).

***IMPORTANT***: Please note that when using this template, all C++ rules for function parameter types remain in effect, so the types you pass in the "*ArgTypesToCompareT*" parameter pack may need to take this into consideration. For instance, if function "*F*" has a parameter declared as an array type, such as  ```const char [10]```, it actually decays into a pointer type instead (the compiler will convert it to ```const char *```), so if the corresponding type you pass in "*ArgTypesToCompareT*" is an array type (```const char [10]``` in this case), then a match will *not* occur. The template will return false because the parameter isn't actually an array. You must pass a pointer type in "*ArgTypesToCompareT*" instead (```const char *``` in this case). See [IsArgTypeSame\_v](#isargtypesame_v) for further details.

In addition to the check for matching args just described, which is always carried out (so "*false*" is guaranteed to be returned if they don't match as just described), the variadic status of "*F*" can also be included in the comparison via the "*AndVariadicT*" template arg (i.e., whether "*F*" is also a variadic function or not). This enumerator is declared as follows:
```C++
enum class AndVariadic
{
    IsFalse = 0, // "F" must not be variadic (normally passed - see below)
    IsTrue = 1,  // "F" must be variadic (rarely passed - see below)
    Ignore       // Variadic status of "F" is ignored (rarely passed - see below)
};
```
If you explicitly pass "*AndVariadic::IsFalse*" which is usually required (read on), then "*F*" must be non-variadic, i.e., its arg list must not end with "..." or "*false*" is guaranteed to be returned. Note that since most functions aren't variadic it means you'll usually pass "*AndVariadic::IsFalse*" to ensure this is the case (that function "*F*" isn't variadic). Note that "*AndVariadicT*" doesn't default to "*AndVariadic::IsFalse*" however since the "*ArgTypesToCompareT*" template arg is a parameter pack so it must be the last template arg (making it impossible to apply a default value to "*AndVariadicT*"). You can rely on [IsArgTypesSameTuple\_v](#isargtypessametuple_v) instead if you wish however, whose "*AndVariadicT*" arg *does* default to "*AndVariadic::IsFalse*, so it's less verbose than "*IsArgTypesSame\_v*" in this regard (since you don't need to explicitly pass "*AndVariadic::IsFalse*" to [IsArgTypesSameTuple\_v](#isargtypessametuple_v), though you do have to pass its "*ArgTypesToCompareTupleT*" template arg as a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) which may be a tiny bit more verbose than the "*ArgTypesToCompareT*" parameter pack of the template you're now reading, but it's negligible).

In addition to "*AndVariadic::IsFalse*", you can explicitly pass "*AndVariadic::IsTrue*" instead if you need to ensure "*F*" *is* variadic (ends with "..."), or "*false*" is guaranteed to be returned if it's not. This is rarely required however since variadic functions don't occur that often in practice, so checking for them is rare. Lastly, you can explicitly pass "*AndVariadic::Ignore*" in which case the variadic status of "*F*" is completely ignored, so it can be variadic or not (it's irrelevant). Note that passing "*AndVariadic::Ignore*" is rarely required however since most will usually need to check the function's variadic status in addition to its non-variadic args (in order to check the function's entire argument list which includes both non-variadic and variadic args). Note that [IsVariadic\_v](#isvariadic_v) can also be independently called to determine if a function is variadic or not however, in which case "*AndVariadic::Ignore*" can be passed if you wish to rely on [IsVariadic\_v](#isvariadic_v) instead for some reason (though it's a separate call so less convenient - usually you'll just pass "*AndVariadic::Ignore*" to the template you're now reading when you don't need to check the function's variadic status at all). Again, regardless of what you pass for the "*AndVariadicT*" template arg (though it will usually be "*AndVariadic::IsFalse*" as noted), the function's *non-variadic* args (types) must always match "*ArgTypesToCompareT*" as previously described or "*false*" is guaranteed to be returned.

*Example:*
```C++
////////////////////////////////////////////////////////////
// Function whose arg list we wish to check. Note that the
// "const char [10]" decays into "const char *" however
// (the compiler treats it this way), so we pass
// "const char *" in the call to "IsArgTypesSame_v" below
// (passing "const char[10]" instead would result in "false"
// being returned - see "IMPORTANT" comments above for
// details).
////////////////////////////////////////////////////////////
using F = int (std::string_view, const char [10]);

/////////////////////////////////////////////////////////////
// Returns true since "F" has arg types "std::string_view"
// and "const char *" (an exact match for the types we're
// passing via the template's "ArgTypesToCompareT" parameter
// pack), and it's also not variadic (as per the
// "AndVariadic::IsFalse" template arg)
/////////////////////////////////////////////////////////////
constexpr bool isArgTypesSame_v = IsArgTypesSame_v<F,
                                                   AndVariadic::IsFalse, // "F" must be non-variadic (not end with "...")
                                                   std::string_view, const char *>; // "F" must have these args (exact match).
                                                                                    // Passing "const char *" here instead of
                                                                                    // "const char[10]" as described in the
                                                                                    // "using F" comments above.
```
</blockquote></details>

<a name="IsArgTypesSameTuple_v"></a><details><summary>IsArgTypesSameTuple\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          TUPLE_C ArgTypesToCompareTupleT,
          AndVariadic AndVariadicT = AndVariadic::IsFalse>
inline constexpr bool IsArgTypesSameTuple_v;
```
Identical to [IsArgTypesSame\_v](#isargtypessame_v) just above except the types to compare with the args in "*F*" are passed as a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of a parameter pack (via the 2nd template arg). The types in the [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) ("*ArgTypesToCompareTupleT*") are therefore compared with those in function "*F*". This template is otherwise identical to [IsArgTypesSame\_v](#isargtypessame_v). See this for details (but please note that the order of the "*AndVariadicT*" template arg in the latter template and this one is reversed, allowing the "*AndVariadic::IsFalse*" default to be supported in this template since it's usually required and arguably cleaner than calling [IsArgTypesSame\_v](#isargtypessame_v) - see this for further details).

***IMPORTANT***: Please note that when using this template, all C++ rules for function parameter types remain in effect, so the types you pass in the "*ArgTypesToCompareTupleT*" template arg may need to take this into consideration. See the *IMPORTANT* section in [IsArgTypesSame\_v](#isargtypessame_v) for details.

Lastly, note that the "*ArgTypesToCompareTupleT*" template arg must be a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) specialization or a constraints violation will occur in C++20 or later (via the TUPLE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17.

*Example:*
```C++
////////////////////////////////////////////////////////////
// Function whose arg list we wish to check. Note that the
// "const char [10]" decays into "const char *" however
// (the compiler treats it this way), so we pass
// "const char *" in the call to "IsArgTypesSameTuple_v"
// below (passing "const char[10]" instead would result in
// "false" being returned - see "IMPORTANT" comments above
// for details).
////////////////////////////////////////////////////////////
using F = int (std::string_view, const char [10]);

/////////////////////////////////////////////////////////////
// Returns true since "F" has arg types "std::string_view"
// and "const char *" (an exact match for the types we're
// passing via the template's "ArgTypesToCompareTupleT"
// arg), and it's also not variadic (in the latter case
// we're relying on the "AndVariadic::IsFalse" default
// template arg)
/////////////////////////////////////////////////////////////
constexpr bool isArgTypesSameTuple_v = IsArgTypesSameTuple_v<F,
                                                             std::tuple<std::string_view, const char *> // "F" must have these args (exact match
                                                                                                        // of the types in this "std::tuple").
                                                                                                        // Passing "const char *" here instead of
                                                                                                        // "const char[10]" as described in the
                                                                                                        // "using F" comments above.
                                                            >;
```
</blockquote></details>

<a name="IsFunctionConst_v"></a><details><summary>IsFunctionConst\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsFunctionConst_v; // Note: Named "IsConst_v" in "struct FunctionTraits"
```
"*bool*" variable set to "*true*" if the function is declared with the "*const*" cv-qualifier, or "*false*" otherwise. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), then this variable always returns "*false*" (since free functions can't be declared with the "*const*" cv-qualifier in C++).
</blockquote></details>

<a name="IsFunctionVolatile_v"></a><details><summary>IsFunctionVolatile\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsFunctionVolatile_v; // Note: Named "IsVolatile_v" in "struct FunctionTraits"
```
"*bool*" variable set to "*true*" if the function is declared with the "*volatile*" cv-qualifier, or "*false*" otherwise. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), then this variable always returns "*false*" (since free functions can't be declared with the "*volatile*" cv-qualifier in C++).
</blockquote></details>

<a name="IsNoexcept_v"></a><details><summary>IsNoexcept\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsNoexcept_v;
```
"*bool*" variable set to "*true*" if "*F*" is declared as "*noexcept*" or "*false*" otherwise (always "*false*" if the "*noexcept*" specifier is absent in the function, otherwise if present then the template evaluates to "*true*" if no bool expression is present in the "*noexcept*" specifier (the expression has been omitted), or the result of the bool expression otherwise - WYSIWYG).
</blockquote></details>

<a name="IsReturnTypeMatch_v"></a><details><summary>IsReturnTypeMatch\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F1,
          TRAITS_FUNCTION_C F2>
inline constexpr bool IsReturnTypeMatch_v;
```
"*bool*" variable set to "*true*" if the return type of function "*F1*" matches the return type of function "*F2*" or "*false*" otherwise. This template is the same as calling [std::is\_same\_v](https://en.cppreference.com/w/cpp/types/is_same), passing it the return type of each function. Also see [IsReturnTypesSame\_v](#isreturntypesame_v) if you wish to check the return type of a function for a specific type, or [IsReturnTypeVoid\_v](#isreturntypevoid_v) to check a return type for "*void*".
</blockquote></details>

<a name="IsReturnTypeSame_v"></a><details><summary>IsReturnTypeSame\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          typename T>
inline constexpr bool IsReturnTypeSame_v;
```
"*bool*" variable set to "*true*" if the return type of function "*F*" is the same as the given type "*T*" or "*false*" otherwise. This template is just a thin wrapper around [std::is\_same\_v](https://en.cppreference.com/w/cpp/types/is_same), where the latter template is passed the return type of function "*F*" and type "*T*". It therefore simply compares the return type of function "*F*" with type "*T*". This is sometimes required when testing function signatures (often in conjunction with the library's various templates for testing arg types), so this template provides a convenient wrapper. Also see [IsReturnTypeMatch\_v](#isreturntypematch_v) if you wish to compare the return types of two functions, or [IsReturnTypeVoid\_v](#isreturntypevoid_v) to check a return type for "*void*".
</blockquote></details>

<a name="IsReturnTypeVoid_v"></a><details><summary>IsReturnTypeVoid\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsReturnTypeVoid_v;
```
"*bool*" variable set to "*true*" if the return type of "*F*" is "*void*" (ignoring cv-qualifiers if any), or "*false*" otherwise. Note that this variable is identical to passing [ReturnType\_t](#returntype_t) to [std::is\_void](https://en.cppreference.com/w/cpp/types/is_void). The variable simply provides a convenient wrapper.
</blockquote></details>

<a name="IsVariadic_v"></a><details><summary>IsVariadic\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr bool IsVariadic_v;
```
"*bool*" variable set to "*true*" if "*F*" is a variadic function (last arg of "*F*" is "...") or "*false*" otherwise.
</blockquote></details>

<a name="MemberFunctionClass_t"></a><details><summary>MemberFunctionClass\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using MemberFunctionClass_t;
```
If "*F*" is a *non-static* member function (or a *non-static* functor, i.e., [FunctionOrigin\_v](#functionorigin_v) return "*FunctionOrigin::Functor*"), yields a type alias for the type of the class (or struct) that declares that function (note that if "*F*" is a *non-static* functor then this is just "*F*" itself if its non-overloaded "*operator()*" member is declared directly in "*F*", or the particular base class of "*F*" that declares it otherwise). Always "*void*" otherwise (for free functions which includes static member functions, or abominable functions). You may therefore wish to invoke [FunctionClassification\_v](#functionclassification_v) to detect if "*F*" is in fact a *non-static* member function before applying this trait (note that it will return "*FunctionClassification::NonStaticMember*" in this case including when "*F*" is a *non-static* functor).
</blockquote></details>

<a name="MemberFunctionClassName_v"></a><details><summary>MemberFunctionClassName\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view MemberFunctionClassName_v;
```
Same as [MemberFunctionClass\_t](#memberfunctionclass_t) just above but returns this as a string (of type "*tstring\_view*" - see [TypeName\_v](#typename_v) for details). A class called *Test* would therefore be (literally) returned as "*Test*" for instance (quotes not included, noting that the string's exact format is also platform-specific as described momentarily). An empty string is always returned however for free and abominable functions (i.e., functions where [FunctionClassification\_v](#functionclassification_v) doesn't return "*FunctionClassification::NonStaticMember*"). For *non-static* member functions however (including *non-static* functors), note that the exact format of a given class's string is platform specific, so it might not be identical on all platforms. A given compiler might add the prefix "*class*" to a class type for instance (or "*struct*" to a struct type), so the latter example might appear as "*class Test*" instead. See both [MemberFunctionClass\_t](#memberfunctionclass_t) and [TypeName\_v](#typename_v) for details (the former since \"*MemberFunctionClassName\_v*\" just defers to it, and the latter which is used to create the actual string).
</blockquote></details>

<a name="ReturnType_t"></a><details><summary>ReturnType\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using ReturnType_t;
```
Type alias for the return type of function "*F*"
</blockquote></details>

<a name="ReturnTypeName_v"></a><details><summary>ReturnTypeName\_v</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
inline constexpr tstring_view ReturnTypeName_v;
```
Same as [ReturnType\_t](#returntype_t) just above but returns this as a string (of type "*tstring\_view*" - see [TypeName\_v](#typename_v) for details). A *float* would therefore be (literally) returned as "*float*" for instance (quotes not included). See both of the latter templates for details ([ReturnType\_t](#returntype_t) itself since \"*ReturnTypeName\_v*\" just defers to it, and [TypeName\_v](#typename_v) which is used to create the actual string).
</blockquote></details>

---

<a name="WriteTraits"></a>
### _Write traits_[^8]

<a name="AddNoexcept_t"></a><details><summary>AddNoexcept\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using AddNoexcept_t;
```

Type alias for "*F*" after adding "*noexcept*" to "*F*" if not already present.
</blockquote></details>

<a name="AddVariadicArgs_t"></a><details><summary>AddVariadicArgs\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using AddVariadicArgs_t;
```
Type alias for "*F*" after adding "..." to the end of its argument list if not already present. Note that the calling convention is also changed to the "*Cdecl*" calling convention for the given compiler. This is the only supported calling convention for variadic functions in this release, but most platforms require this calling convention for variadic functions. It ensures that the calling function (opposed to the called function) pops the stack of arguments after the function is called, which is required by variadic functions. Other calling conventions that also do this are possible though none are currently supported in this release (since none of the currently supported compilers support this - such calling conventions are rare in practice).
</blockquote></details>

<a name="ArgsAppend_t"></a><details><summary>ArgsAppend\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          VALID_FUNCTION_ARG_TYPE_C... ArgsToAppendT>
using ArgsAppend_t;
```
Type alias identical to function "*F*" but where the types in "*ArgsToAppendT*" have been appended to the end of the args in "*F*", yielding an alias for the resulting function (identical to "*F*" except all types in "*ArgsToAppendT*" have been appended at the end of the existing args in "*F*").

Note that this template is just the special (common use) case of template [ArgsModify\_t](#argsmodify_t) (or its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t)), where its "*I*" template arg (the index where you wish to replace the existing args of "*F*" with the "*NewArgsT*" template arg of [ArgsModify\_t](#argsmodify_t)) is [ArgCount\_v](#argcount_v)\<F\>, i.e., the end of the argument list in "*F*" (see [ArgsModify\_t](#argsmodify_t) for details). Similarly, it's also equivalent to [ArgsInsert\_t](#argsinsert_t) (or its cousin [ArgsInsertTuple\_t](#argsinserttuple_t)) for the same reason (i.e., when [ArgCount\_v](#argcount_v)\<F\> is passed for its "*I*" template arg so insertion of the types in its "*ArgsToInsertT*" template arg occurs at the end of the argument list in "*F*"). "*ArgsAppend\_t*" is cleaner than either of the latter techniques however since the name better reflects the template's purpose and neither the "*I*" template arg of both [ArgsModify\_t](#argsmodify_t) and [ArgsInsert\_t](#argsinsert_t) nor the "*Count*" template arg of [ArgsModify\_t](#argsmodify_t) need to be explicitly passed (they are always implicitly [ArgCount\_v](#argcount_v)\<F\> and zero (0) respectively).

Lastly, please note that the "*ArgsToAppendT*" template arg (parameter pack) must contain valid (legal) C++ function argument (parameter) types ("*void*" not permitted for instance) or a constraints violation will occur in C++20 or later (via the VALID_FUNCTION_ARG_TYPE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17.

*Example:*
```C++
// Sample function we'll be appending new args to below
using F = void (char, long, float, double, std::string);

////////////////////////////////////////////////////////////////////////////
// Call "ArgsAppend_t" to append an "int", "unsigned char" and "bool" to
// the end of the arg list for "F", resulting in the following type:
//
//     void (char, long, float, double, std::string, int, unsigned char, bool)
//
// Note however that "ArgsAppend_t" is just a thin wrapper around
// "ArgsModify_t", identical to the latter template whenever "ArgCount_v<F>"
// is passed for its "I" template arg. Note that its "Count" template arg
// is then immaterial since "I" is one beyond the end of the arg list for
// "F" so the template will always force (truncate) the "Count" arg to zero
// in this case. The following will therefore produce the identical result:
//
//     using F_WithNewArgsAppendedAtEnd = ArgsModify_t<F, ArgCount_v<F>, 0, int, unsigned char, bool>;
//
// Note that "ArgsAppend_t" is also just a thin wrapper around "ArgsInsert_t",
// identical to the latter template whenever "ArgCount_v<F>" is passed for
// its "I" template arg (analogous to the "ArgsModify_t" case just above). The
// following will therefore produce the identical result as well:
//
//     using F_WithNewArgsAppendedAtEnd = ArgsInsert_t<F, ArgCount_v<F>, int, unsigned char, bool>;
//
// "ArgsAppend_t" is cleaner than both "ArgsModify_t" and "ArgsInsert_t"
// however, and should normally be relied on instead (whenever you need to
// append new args - it's less verbose than both of the above calls and its
// name better reflects its purpose).
////////////////////////////////////////////////////////////////////////////
using F_WithNewArgsAppendedAtEnd = ArgsAppend_t<F, int, unsigned char, bool>;
```
</blockquote></details>

<a name="ArgsAppendTuple_t"></a><details><summary>ArgsAppendTuple\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToAppendTupleT>
using ArgsAppendTuple_t;
```
Identical to template [ArgsAppend\_t](#argsappend_t) just above except its "*ArgsToAppendT*" template arg is passed as a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of a parameter pack (via the 2nd template arg). The types in the [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) are therefore appended to the end of the function's existing argument list. The two templates are identical otherwise (so you can use whichever is more suitable or convenient). See [ArgsAppend\_t](#argsappend_t) for details.

Lastly, please note that the "*ArgsToAppendTupleT*" template arg must be a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) specialization containing valid (legal) C++ function argument (parameter) types ("*void*" not permitted for instance) or a constraints violation will occur in C++20 or later (via the VALID_FUNCTION_ARG_TYPES_TUPLE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17.
</blockquote></details>

<a name="ArgsDelete_t"></a><details><summary>ArgsDelete\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I = 0,
          std::size_t Count = StdExt::size_t_npos>
REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
using ArgsDelete_t;
```
Type alias identical to function "*F*" but where all args in "*F*" starting at (zero-based) index "*I*" for a length of "*Count*" have been deleted (non-variadic args only - the "..." for variadic functions always remains intact - use [RemoveVariadicArgs\_t](#removevariadicargs_t) if you wish to remove it). The template then yields an alias for the resulting function (identical to "*F*" except all arguments in "*F"* at index "*I*" for a length of "*Count*" inclusive have been removed). Note that "*I*" must be less than or equal to the number of arguments in function "*F*" or a constraints violation will occur in C++20 or later (via the REQUIRES\_I\_LESS\_THAN\_OR\_EQUAL\_TO\_ARG\_COUNT concept seen in the template's declaration), or a "*static\_assert*" in C++17 (i.e., you must target an existing range to delete in the function's arguments so "*I*" must be within bounds). Note that if "*I*" is [ArgCount\_v](#argcount_v)\<F\> itself (so it refers to one beyond the end of the existing args), or "*Count*" is zero then nothing is deleted (the resulting alias is then identical to "*F*"). Note that "*Count*" can exceed the number of arguments remaining in "*F*" (from index "*I*" inclusive), in which case all args are deleted from index "*I*" inclusive to the end of the argument list (noting that "*Count*" defaults to "*StdExt::size\_t\_npos*", analogous to other occurrences of "*npos*" in C++ such as [std::basic\_string::npos](https://en.cppreference.com/w/cpp/string/basic_string/npos), so "*Count*" can be omitted altogether if you wish - all remaining args from "*I*" inclusive to the end of the argument list in "*F*" will then be deleted - passing zero for "*I*" in this case, the default arg, therefore means that all args will be deleted).

Note that this template is just the special (common use) case of template [ArgsModify\_t](#argsmodify_t) (or its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t)), where its "*NewArgsT*" template arg is an empty parameter pack (see [ArgsModify\_t](#argsmodify_t) for details). "*ArgsDelete\_t*" is cleaner however since the name better reflects the template's purpose.

*Example:*
```C++
// Sample function we'll be deleting args from below
using F = void (char, long, float, double, std::string);

////////////////////////////////////////////////////////////////////////////
// Call "ArgsDelete_t" to delete the args in "F" at index 1 for a length
// of 2, i.e., the "long" and the "float" are deleted. This yields the
// following result:
//
//     void (char, double, std::string)
//
// Note however that "ArgsDelete_t" is just a thin wrapper around
// "ArgsModify_t", identical to the latter template whenever an empty
// parameter pack is passed for its "NewArgsT" template arg (since no
// types are being passed for the "NewArgsT" arg - the args you're
// targeting in "F" are therefore replaced with an empty parameter
// pack, so are effectively deleted). The following will therefore
// produce the identical result (note that no types are being passed
// for the "NewArgsT" template arg as described - it's an empty
// parameter pack):
//
//     using F_WithArgsDeletedAtIndex1Size2 = ArgsModify_t<F, 1, 2>;
//
// "ArgsDelete_t" is cleaner though and should normally be relied on
// instead (whenever you need to delete args - its name better reflects
// its purpose).
////////////////////////////////////////////////////////////////////////////
using F_WithArgsDeletedAtIndex1Size2 = ArgsDelete_t<F, 1, 2>;
```
</blockquote></details>

<a name="ArgsInsert_t"></a><details><summary>ArgsInsert\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I,
          VALID_FUNCTION_ARG_TYPE_C... ArgsToInsertT>
REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
using ArgsInsert_t;
```
Type alias identical to function "*F*" but where the types in "*ArgsToInsertT*" have been inserted into the args of "*F*" at (zero-based) index "*I*", shifting the existing arg at index "*I*" and all remaining args over to accommodate the newly inserted args. The template then yields an alias for the resulting function (identical to "*F*" except all arguments in "*ArgsToInsertT*" have been inserted into the existing args of "*F*" at index "*I*"). Note that "*I*" must be less than or equal to the number of arguments in function "*F*" or a constraints violation will occur in C++20 or later (via the REQUIRES\_I\_LESS\_THAN\_OR\_EQUAL\_TO\_ARG\_COUNT concept seen in template's declaration), or a "*static\_assert*" in C++17 (i.e., you must target a valid index in the function's existing arguments where you wish to insert "*ArgsToInsertT*"). If "*I*" equals the number of arguments itself however (i.e., it equals [ArgCount\_v](#argcount_v)\<F\>), then "*ArgsToInsert*" is inserted at the end of the argument list in "*F*" so it's effectively appended. In this case you can rely on [ArgsAppend\_t](#argsappend_t) instead if you wish (or its cousin [ArgsAppendTuple\_t](#argsappendtuple_t)). They are cleaner helper templates for this specific purpose (that just defer to "*ArgsInsert_t*" as just described).

Note that this template is just the special (common use) case of template [ArgsModify\_t](#argsmodify_t) (or its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t)), where its "*Count*" template arg is zero (see [ArgsModify\_t](#argsmodify_t) for details). "*ArgsInsert\_t*" is cleaner however since the name better reflects the template's purpose and the "*Count*" arg of [ArgsModify\_t](#argsmodify_t) no longer needs to be explicitly passed (it's always implicitly zero).

Lastly, please note that the "*ArgsToInsertT*" template arg (parameter pack) must contain valid (legal) C++ function argument (parameter) types ("*void*" not permitted for instance) or a constraints violation will occur in C++20 or later (via the VALID_FUNCTION_ARG_TYPE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17.

*Example:*
```C++
// Sample function we'll be inserting new args into below
using F = void (char, long, float, double, std::string);

////////////////////////////////////////////////////////////////////////////
// Call "ArgsInsert_t" to insert an "int", "unsigned char" and "bool" into
// the arg list for "F" at (zero-based) index 1, resulting in the following
// type:
//
//     void (char, int, unsigned char, bool, long, float, double, std::string)
//
// Note however that "ArgsInsert_t" is just a thin wrapper around
// "ArgsModify_t", identical to the latter template whenever zero (0) is
// passed for its "Count" template arg. The following will therefore
// produce the identical result:
//
//     using F_WithNewArgsInsertedAtIndex1 = ArgsModify_t<F, 1, 0, int, unsigned char, bool>;
//
// "ArgsInsert_t" is cleaner though and should normally be relied on
// instead (whenever you need to insert new args - it's less verbose than
// the above call and its name better reflects its purpose).
////////////////////////////////////////////////////////////////////////////
using F_WithNewArgsInsertedAtIndex1 = ArgsInsert_t<F, 1, int, unsigned char, bool>;
```
</blockquote></details>

<a name="ArgsInsertTuple_t"></a><details><summary>ArgsInsertTuple\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I,
          VALID_FUNCTION_ARG_TYPES_TUPLE_C ArgsToInsertTupleT>
REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
using ArgsInsertTuple_t;
```
Identical to template [ArgsInsert\_t](#argsinsert_t) just above except its "*ArgsToInsertT*" template arg is passed as a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of a parameter pack (via the last template arg). The types in the [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) are therefore inserted into the function's existing argument list. The two templates are identical otherwise (so you can use whichever is more suitable or convenient). See [ArgsInsert\_t](#argsinsert_t) for details.

Lastly, please note that the "*ArgsToInsertTupleT*" template arg must be a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) specialization containing valid (legal) C++ function argument (parameter) types ("*void*" not permitted for instance) or a constraints violation will occur in C++20 or later (via the VALID_FUNCTION_ARG_TYPES_TUPLE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17.
</blockquote></details>

<a name="ArgsMakeVoid_t"></a><details><summary>ArgsMakeVoid\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using ArgsMakeVoid_t;
```
Type alias identical to function "*F*" but where the argument list has been replaced with "*(void)*" (or identically just "*()*" which is more commonly used by most). The resulting argument list is therefore completely empty (all existing non-variadic args if any *and* the \"...\" for variadic args if any are completely removed). This template is equivalent to calling [ArgsDelete\_t](#argsdelete_t)\<F\> and [RemoveVariadicArgs\_t](#removevariadicargs_t)\<F\> (the order doesn't matter), providing a convenient helper if you need to completely remove a function's arguments.
</blockquote></details>

<a name="ArgsModify_t"></a><details><summary>ArgsModify\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I,
          std::size_t Count,
          VALID_FUNCTION_ARG_TYPE_C... NewArgsT>
REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
using ArgsModify_t;
```
Type alias identical to function "*F*" but where all (non-variadic) args in "*F*" starting at (zero-based) index "*I*" for a length of "*Count*" have been replaced with the args (types) in "*NewArgsT*", yielding an alias for the resulting function (the "..." for variadic args in "*F*" remain intact however, if present - use [RemoveVariadicArgs\_t](#removevariadicargs_t) if you need to remove them). Note that "*I*" must be less than or equal to the number of arguments in the function or a constraints violation will occur in C++20 or later (via the REQUIRES\_I\_LESS\_THAN\_OR\_EQUAL\_TO\_ARG\_COUNT concept seen in the template's declaration), or a "*static\_assert*" in C++17 (i.e., you must target an existing range to replace in the function's arguments so "*I*" must be within bounds, though "*I*" can target one beyond the last arg to append new types just beyond the last arg, as described shortly). Note that "*Count*" can exceed the number of arguments remaining from index "*I*" however, since the template will gracefully cap it at the number of existing arguments actually remaining. It makes no sense to replace more arguments than actually remain from index "*I*" that is (hence "*Count*" is cut off at the last arg should it exceed it), though you can replace them with more arguments than remain if "*NewArgsT*" contains more.

Note that passing zero (0) for the "*Count*" arg can be used to insert new args into the function's existing args at index "*I*" instead of replacing the existing args there, since zero (0) effectively means no existing args will be replaced. The args in "*NewArgsT*" will effectively be inserted at index "*I*" instead, shifting over all remaining args from index "*I*" inclusive until the end of the argument list (to accommodate the args in "*NewArgsT*"). The helper template [ArgsInsert\_t](#argsinsert_t) (and its cousin [ArgsInsertTuple\_t](#argsinserttuple_t)) exists for this specific purpose however and should ideally be used instead (since the template's name better reflects its purpose and the template is cleaner than calling "*ArgsModify_t*" directly in this case). See "*Example 2*" below.

Note that passing [ArgCount\_v](#argcount_v)\<F\> (the number of arguments in "*F*") for "*I*" can be used to append new args to the end of the function's existing args instead of replacing existing args, since "*I*" targets the end of the existing args in this case (one beyond the last arg). Note that the "*Count*" arg is then immaterial since it will always be capped at zero (0) in this case (since it's cut off at the end of the existing args as previously described but it's already at the end), so you can just pass zero (0) or "*StdExt::size\_t\_npos*" or any other value (it's irrelevant). The helper template [ArgsAppend\_t](#argsappend_t) (and its cousin [ArgsAppendTuple\_t](#argsappendtuple_t)) exists for this specific purpose however and should ideally be used instead (since the template's name better reflects its purpose and the template is cleaner than calling "*ArgsModify_t*" directly in this case). See "*Example 3*" below.

Note that passing an empty parameter pack for "*NewArgsT*" can be used to delete the args at index "*I*" for a length of "*Count*" instead of replacing them with new args (since they're being replaced with an empty parameter pack so they're not replaced with anything - they're effectively deleted instead). The helper template [ArgsDelete\_t](#argsdelete_t) exists for this specific purpose however and should ideally be used instead (since the template's name better reflects its purpose and the template is cleaner than calling "*ArgsModify_t*" directly in this case). See "*Example 4*" below.

Note that this template is the workhorse of all other (function) argument modification templates in the library. It can effectively be used to carry out any type of change to a function's existing arguments from replacing existing args to inserting, appending and deleting args as described above (though in the rare case where you need to target a non-contiguous range of args you'll have to call it more than once). To this end, note that all other argument modification templates in the library (e.g., [ArgsInsert\_t](#argsinsert_t), [ArgsAppend\_t](#argsappend_t), [ArgsDelete\_t](#argsdelete_t), [ReplaceArgs\_t](#replaceargs_t), [ReplaceArg\_t](#replacearg_t) etc.) can be carried out by directly invoking "*ArgsModify\_t*" with suitable template args (and most of the other argument modification templates do ultimately defer back to "*ArgsModify\_t*" to carry out their actual work). The other argument modification templates should normally be used instead however, when appropriate, since they are specialized for the most common use cases they're designed for (and are therefore cleaner and more suitably named than calling "*ArgsModify\_t*" directly). Before using "*ArgsModify\_t*" you should therefore ensure that a more dedicated template doesn't already exist for your needs.

Lastly, please note that the "*NewArgsT*" template arg (parameter pack) must contain valid (legal) C++ function argument (parameter) types ("*void*" not permitted for instance) or a constraints violation will occur in C++20 or later (via the VALID_FUNCTION_ARG_TYPE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17. In particular, if you wish to create a completely empty argument list, *don't* pass zero (0), [ArgCount\_v](#argcount_v)\<F\> and "*void*" for template args "*I*", "*Count*" and "*NewArgsT*" respectively (such as ``` ArgsModify_t<F, 0, ArgCount_v<F>, void>```), the reason you normally would, in an attempt to create the empty argument list "*(void)*". A compiler error will occur in this case since "*void*" is treated as an actual parameter type in this context (opposed to an empty parameter list), and "*void*" is not a valid C++ parameter type as just described (and the "*...*" for variadic functions isn't removed by this template regardless, should it be present in "*F*" - in such cases it would therefore result in an invalid "*(void, ...)*" parameter list instead). Call [ArgsMakeVoid\_t](#argsmakevoid_t) instead if you wish to create a completely empty parameter list (see this for details).

*Examples:*
```C++
// Sample function used in all examples below
using F = void (char, long, float, double, std::string);

////////////////////////////////////////////////////////////////////////////
// Example 1 (replacing args in a function's argument list)
// --------------------------------------------------------
// Use the helper alias "ArgsModify_t" to modify (replace) the args in "F"
// at (zero-based) index 1 for a length of 2 (so the "long" and the
// "float") with the new args being passed below (i.e., the "long" and
// "float" in "F" are replaced with the types being passed). The resulting
// function type is therefore:
//
//    void (char, int, unsigned char, bool, double, std::string)
//
// Note that you can also rely on template "ArgsModifyTuple_t" if you wish
// to pass the "NewArgsT" parameter pack below as a "std::tuple" instead.
// The templates are identical otherwise. Also see "ReplaceArg_t" if you
// wish to replace a single arg only with another single arg which is just
// the special case of "ArgsModify_t" where the "Count" arg is 1 and the
// "NewArgsT" arg contains a single arg type only (a common case so
// "ReplaceArg_t" provides a convenient helper). See "ReplaceArgs_t" (or
// its cousin "ReplaceArgsTuple_t") if you wish to replace *all*
// (non-variadic) args in the function (i.e., assign a completely new
// argument list), which is just the special case of "ArgsModify_t" where
// the "I" arg is zero (0) and the "Count" arg is "ArgCount_v<F>" (i.e.,
// all existing (non-variadic) args in "F" will be replaced, a common case
// so "ReplaceArgs_t" provides a convenient helper).
////////////////////////////////////////////////////////////////////////////
using F_WithArgsReplacedAtIndex1Size2 = ArgsModify_t<F, 1, 2, int, unsigned char, bool>;

////////////////////////////////////////////////////////////////////////////
// Example 2 (inserting new args in a function's argument list)
// ------------------------------------------------------------
// By passing zero (0) for "Count", the args you pass (parameter pack
// "NewArgsT") can be inserted into "F" instead of replacing the existing
// args in "F" (i.e., no existing args in "F" are replaced since "Count"
// is zero so the args being passed, "NewArgsT", are effectively inserted
// at index "I" instead of replacing the existing args there - the existing
// args remain intact but are just shifted over to accommodate the new args
// being inserted). If doing so however then you can just rely on the helper
// templates "ArgsInsert_t" or its cousin "ArgsInsertTuple_t" instead (both
// declared further below), which just defer to the following template,
// passing zero for "Count" as described (see alternate "using" statement
// just below the following call for an example - both "using" statements are
// equivalent).
//
// The following example inserts the new types into "F" at index 1 (inserted
// because "Count" is zero as described just above). The resulting alias is
// therefore:
//
//     void (char, int, unsigned char, bool, long, float, double, std::string)
////////////////////////////////////////////////////////////////////////////
using F_WithNewArgsInsertedAtIndex1 = ArgsModify_t<F, 1, 0, int, unsigned char, bool>;

////////////////////////////////////////////////////////
// Cleaner alternative to example 2 just above (using
// the "ArgsInsert_t" template), which just invokes
// the above but the name better reflects its purpose
// and the "Count" arg of zero (0) seen above no longer
// needs to be explicitly passed (it's implicit)
////////////////////////////////////////////////////////
// using F_WithNewArgsInsertedAtIndex1 = ArgsInsert_t<F, 1, int, unsigned char, bool>;

////////////////////////////////////////////////////////////////////////////
// Example 3 (appending new args to the end of a function's argument list)
// -----------------------------------------------------------------------
// By passing "ArgCount_v<F>" for the "I" template arg (i.e., the number of
// args in function "F" - the "Count" is then immaterial since the template
// will always force it to zero in this case), the args you pass (parameter
// pack "NewArgsT") will be appended to the end of "F" instead of replacing
// any existing args in "F" with "NewArgsT" (i.e., no existing args in "F"
// are replaced since "I" is one beyond the end of the argument list so
// "NewArgsT" is effectively appended to the end). If doing so however then
// you can just rely on the helper template "ArgsAppend_t" or its cousin
// "ArgsAppendTuple_t" instead (both declared further below), which just
// defers to the following template, passing "ArgCount_v<F>" for index "I"
// as described (see alternate "using" statement just below the following
// call for an example - both "using" statements are equivalent).
//
// The following example appends the new types to the end of "F" (appended
// because "I" is "ArgCount_v<F>" as described just above). The resulting
// alias is therefore:
//
//     void (char, long, float, double, std::string, int, unsigned char, bool)
////////////////////////////////////////////////////////////////////////////
using F_WithNewArgsAppendedAtEnd = ArgsModify_t<F, ArgCount_v<F>, 0, int, unsigned char, bool>;

///////////////////////////////////////////////////////
// Cleaner alternative to example 3 just above (using
// the "ArgsAppend_t" template), which just invokes
// the above but the name better reflects its purpose
// and neither the "ArgCount_v<F>" arg being passed
// above nor the "Count" arg of 0 passed above needs
// to be explicitly passed (they are both implicit)
///////////////////////////////////////////////////////
// using F_WithNewArgsAppendedAtEnd = ArgsAppend_t<F, int, unsigned char, bool>;

////////////////////////////////////////////////////////////////////////////
// Example 4 (deleting args from a function's argument list)
// ---------------------------------------------------------
// By passing an empty parameter pack for "NewArgsT", you can effectively
// delete the args in function "F" at index "I" for a length of "Count"
// instead of replacing them, since they're being replaced with the args in
// an empty parameter pack. Since it's empty the args in "F" at index "I"
// for a length of "Count" are therefore replaced with nothing (so
// effectively deleted). If doing so however then you can just rely on
// "ArgsDelete_t" instead. It just defers to the following template,
// passing an empty parameter pack for "NewArgsT" as described (see
// alternate "using" statement just below the following call for an
// example - both "using" statements are equivalent).
//
// The following example deletes the args in "F" at index 1 for a length of
// 2, i.e., the "long" and the "float" are deleted (since no types are
// being passed for the "NewArgsT" arg - it's an empty parameter pack so
// the "long" and the "float" args are replaced with nothing, i.e.,
// effectively deleted). The resulting alias is therefore:
//
//     void (char, double, std::string)
////////////////////////////////////////////////////////////////////////////
using F_WithArgsDeletedAtIndex1Size2 = ArgsModify_t<F, 1, 2>;

///////////////////////////////////////////////////////
// Cleaner alternative to example 4 just above (using
// the "ArgsDelete_t" template), which just invokes
// the above but the name better reflects its purpose
///////////////////////////////////////////////////////
// using F_WithArgsDeletedAtIndex1Size2 = ArgsDelete_t<F, 1, 2>;
```
</blockquote></details>

<a name="ArgsModifyTuple_t"></a><details><summary>ArgsModifyTuple\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I,
          std::size_t Count,
          VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT>
REQUIRES_I_LESS_THAN_OR_EQUAL_TO_ARG_COUNT(F, I)
using ArgsModifyTuple_t;
```
Identical to template [ArgsModify\_t](#argsmodify_t) just above except its "*NewArgsT*" template arg is passed as a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of a parameter pack (via the last template arg). The types in the [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) are therefore used to replace the existing args you're targeting. The two templates are identical otherwise (so you can use whichever is more suitable or convenient). See [ArgsModify\_t](#argsmodify_t) for details.

Lastly, please note that the "*NewArgsTupleT*" template arg must be a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) specialization containing valid (legal) C++ function argument (parameter) types ("*void*" not permitted for instance) or a constraints violation will occur in C++20 or later (via the VALID_FUNCTION_ARG_TYPES_TUPLE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17. See [ArgsModify\_t](#argsmodify_t) for details.
</blockquote></details>

<a name="FunctionAddConst_t"></a><details><summary>FunctionAddConst\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionAddConst_t; // Note: Named "AddConst_t" in "struct FunctionTraits"
```
Type alias for "*F*" after adding the "*const*" cv-qualifier to "*F*" if not already present. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), then the resulting type alias becomes an [abominable function](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) (so [FunctionClassification\_v](#functionclassification_v) returns "*Abominable*"). In this case note that if "*F*" is a pointer or reference to a free function (or a reference to a pointer), then the pointer and/or reference is removed (since pointers and/or references to abominable functions are illegal in C++).
</blockquote></details>

<a name="FunctionAddCV_t"></a><details><summary>FunctionAddCV\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionAddCV_t; // Note: Named "AddCV_t" in "struct FunctionTraits"
```
Type alias for "*F*" after adding both the "*const*" AND "*volatile*" cv-qualifiers to "*F*" if not already present. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), then the resulting type alias becomes an [abominable function](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) (so [FunctionClassification\_v](#functionclassification_v) returns "*Abominable*"). In this case note that if "*F*" is a pointer or reference to a free function (or a reference to a pointer), then the pointer and/or reference is removed (since pointers and/or references to abominable functions are illegal in C++).
</blockquote></details>

<a name="FunctionAddLValueReference_t"></a><details><summary>FunctionAddLValueReference\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionAddLValueReference_t; // Note: Named "AddLValueReference_t" in "struct FunctionTraits"
```
Type alias for "*F*" after adding the "*&*" ref-qualifier to "*F*" if not already present (replacing the "*&&*" ref-qualifier if present). Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), then the resulting type alias becomes an [abominable function](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) (so [FunctionClassification\_v](#functionclassification_v) returns "*Abominable*"). In this case note that if "*F*" is a pointer or reference to a free function (or a reference to a pointer), then the pointer and/or reference is removed (since pointers and/or references to abominable functions are illegal in C++).
</blockquote></details>

<a name="FunctionAddRValueReference_t"></a><details><summary>FunctionAddRValueReference\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionAddRValueReference_t; // Note: Named "AddRValueReference_t" in "struct FunctionTraits"
```
Type alias for "*F*" after adding the "*&&*" ref-qualifier to "*F*" if not already present (replacing the "*&*" ref-qualifier if present). Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), then the resulting type alias becomes an [abominable function](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) (so [FunctionClassification\_v](#functionclassification_v) returns "*Abominable*"). In this case note that if "*F*" is a pointer or reference to a free function (or a reference to a pointer), then the pointer and/or reference is removed (since pointers and/or references to abominable functions are illegal in C++).
</blockquote></details>

<a name="FunctionAddVolatile_t"></a><details><summary>FunctionAddVolatile\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionAddVolatile_t; // Note: Named "AddVolatile_t" in "struct FunctionTraits"
```
Type alias for "*F*" after adding the "*volatile*" cv-qualifier to "*F*" if not already present. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), then the resulting type alias becomes an [abominable function](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0172r0.html) (so [FunctionClassification\_v](#functionclassification_v) returns "*Abominable*"). In this case note that if "*F*" is a pointer or reference to a free function (or a reference to a pointer), then the pointer and/or reference is removed (since pointers and/or references to abominable functions are illegal in C++).
</blockquote></details>

<a name="FunctionRemoveConst_t"></a><details><summary>FunctionRemoveConst\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionRemoveConst_t; // Note: Named "RemoveConst_t" in "struct FunctionTraits"
```
Type alias for "*F*" after removing the "*const*" cv-qualifier from "*F*" if present. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "**"), yields "*F*" itself (effectively does nothing since "*const*" doesn't apply to free functions so will never be present).
</blockquote></details>

<a name="FunctionRemoveCV_t"></a><details><summary>FunctionRemoveCV\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionRemoveCV_t; // Note: Named "RemoveCV_t" in "struct FunctionTraits"
```
Type alias for "*F*" after removing both the "*const*" and "*volatile*" cv-qualifiers from "*F*" if present. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), yields "*F*" itself (effectively does nothing since "*const*" and "*volatile*" don't apply to free functions so will never be present).
</blockquote></details>

<a name="FunctionRemoveReference_t"></a><details><summary>FunctionRemoveReference\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionRemoveReference_t; // Note: Named "RemoveReference_t" in "struct FunctionTraits"
```
Type alias for "*F*" after removing the "*&*" or "*&&*" ref-qualifier from "*F*" if present. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), yields "*F*" itself (effectively does nothing since "*&*" and "*&&*" don't apply to free functions so will never be present).
</blockquote></details>

<a name="FunctionRemoveVolatile_t"></a><details><summary>FunctionRemoveVolatile\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using FunctionRemoveVolatile_t; // Note: Named "RemoveVolatile_t" in "struct FunctionTraits"
```
Type alias for "*F*" after removing the "*volatile*" cv-qualifier from "*F*" if present. Note that if "*F*" is a free function (i.e., [FunctionClassification\_v](#functionclassification_v) returns "*Free*"), yields "*F*" itself (effectively does nothing since "*volatile*" doesn't apply to free functions so will never be present).
</blockquote></details>

<a name="ReplaceMemberFunctionClass_t"></a><details><summary>ReplaceMemberFunctionClass\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          IS_CLASS_OR_VOID_C NewClassT>
using ReplaceMemberFunctionClass_t;
```
Replaces the existing member class of function "*F*" with "*NewClassT*" or removes the existing class if "*NewClassT*" is "*void*". The behavior of this function however depends on whether "*F*" refers to a *non-static* member function since it's already a member of an existing class (which you wish to replace with "*NewClassT*"), or a free or abominable function instead, which aren't members of an existing class by definition but you wish to assign it to one (namely "*NewClassT*", so the function becomes a *non-static* member of that class).

If "*F*" is a *non-static* member function, i.e., [FunctionClassification\_v](#functionclassification_v)\<F\> returns "*FunctionClassification::NonStaticMember*", yields a type alias for "*F*" after replacing the existing class it belongs to with "*NewClassT*" (so the resulting type is identical to "*F*" except its class has been replaced with "*NewClassT*" - for *non-static* functors the class "*F*" in "*F::operator()*" is replaced with "*NewClassT*" so a *non-static* member function pointer type for "*NewClassT::operator()*" is returned). Passing "*void*" for "*NewClassT*" however will remove the existing class from "*F*" instead, effectively converting the function from a *non-static* member function to its [raw](#rawfunctiontypes) (native) C++ function equivalent instead, identical to [FunctionRawType\_t](#functionrawtype_t)\<F\> (and therefore always satisfying [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) - passing the resulting type to template [FunctionClassification\_v](#functionclassification_v) will therefore always return either "*FunctionClassification::Free*" or "*FunctionClassification::Abominable*"). Again, if "*F*" is a *non-static* functor then passing "*void*" for "*NewClassT*" will result in the [raw](#rawfunctiontypes) type of its "*F::operator*()" member.

For all other types of "*F*", i.e., those where [FunctionClassification\_v](#functionclassification_v)\<F\> returns either "*FunctionClassification::Free*" or "*FunctionClassification::Abominable*" (so functions other than a *non-static* member function) , converts the function to a *non-static* member function of class "*NewClassT*" (effectively assigns the function to "*NewClassT*"), yielding ```RawTypeF NewClassT::*```, where "*RawTypeF*" is [FunctionRawType\_t<F\>](#functionrawtype_t), i.e., the [raw](#rawfunctiontypes) (native) C++ type of the function that "*F*" refers to. Note that if "*F*" is a *static* functor which were legalized in C++23 (i.e., its "*operator()*" member is static), then the resulting type will refer to the *non-static* member function "*NewClassT::operator()*" (i.e., the *static* "*operator()*" member of functor "*F*" has been assigned as a *non-static* member function of class "*NewClassT*"). Note that if "*F*" is a [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specialization then the resulting type will refer to the *non-static* member function "*NewClassT::StdFunctionTemplateArg*", where "*StdFunctionTemplateArg*" is the template arg (function) that [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) "*F*" was specialized on (i.e., the free function that "*F*" was specialized on has been assigned as a *non-static* member of class "*NewClassT*"). In all cases above "*F*" is therefore simply converted from a free or abominable function to a *non-static* member function in the given class "*NewClassT*", so passing the resulting type to template [FunctionClassification\_v](#functionclassification_v) will therefore always return "*FunctionClassification::NonStaticMember*". Note that passing "*void*" for "*NewClassT*" however is ignored in all the above cases, so the type of function "*F"* is returned unchanged (since "*void*" means remove the existing class from a *non-static* member function but free and abominable functions aren't *non-static* member functions, so there's nothing to remove).

Lastly, note that the "*NewClassT*" template arg must either be a class type (i.e., it must satisfy [std::is\_class](https://en.cppreference.com/w/cpp/types/is_class)) or must be "*void*", or a constraints violation will occur in C++20 or later (via the IS_CLASS_OR_VOID_C concept seen in the template's declaration), or a "*static\_assert*" in C++17.

*Examples:*
```C++
// Class for demo purposes below
class Test
{
    // ...
};

// Class for demo purposes below
class Whatever
{
    // ...
};

// Non-static member function pointer in class "Test"
using F1 = int (Test::*)(std::string_view, float) const noexcept;

//////////////////////////////////////////////////////////////
// Replace class "Test" in "F1" with class "Whatever" so the
// resulting type is:
//
// int (Whatever::*)(std::string_view, float) const noexcept
//////////////////////////////////////////////////////////////
using F1ReplacedClassWithWhatever = ReplaceMemberFunctionClass_t<F1, Whatever>;

///////////////////////////////////////////////////////////
// Replace class "Test" in "F1" with class "void" but
// because it's "void" and therefore not an actual class,
// class "Test" is removed from "F1" instead, effectively
// converting it to a "raw" (native C++) function type (in
// this case an "abominable" function because it contains
// "const" - see "C++ function types at a glance" earlier
// in this document for details on abominable functions
// but it's irrelevant here - qualifiers like "const" in
// "F1" aren't relevant to "ReplaceMemberFunctionClass_t",
// they are just migrated to the resulting type as would
// be expected):
//
//    int (std::string_view, float) const noexcept
///////////////////////////////////////////////////////////
using F1ConvertedToRawFunction = ReplaceMemberFunctionClass_t<F1, void>;

///////////////////////////////////////////////////////////
// Similar to example just above but we're now passing a
// lambda. Since a lambda is just a functor behind the
// scenes, the "operator()" member of the functor is the
// function being targeted and since we're passing "void"
// as the 2nd template arg to "ReplaceMemberFunctionClass_t"
// (just like the above example), the lambda's compiler
// generated class is removed and the lambda's "raw"
// function is therefore returned (note that it contains
// "const" which the compiler implicitly assigns to the
// lambda's "operator()" member behind the scenes in this
// case):
//
//    int (std::string_view, float) const noexcept
///////////////////////////////////////////////////////////
constexpr auto someLambda = [](std::string_view someStr, float someFloat) noexcept
                               {
                                   int rc;

                                   // ...

                                   return rc;
                               };
using SomeLambdaConvertedToRawFunction = ReplaceMemberFunctionClass_t<decltype(someLambda), void>;

////////////////////////////////////////////////////////
// Raw (native C++) function type (in this case an
// abominable function because it contains "const" -
// again, see "C++ function types" earlier in this
// document for details on abominable functions but
// it's irrelevant here - qualifiers like "const" in
// "F1" aren't relevant to "ReplaceMemberFunctionClass_t",
// they are just migrated to the resulting type as
// would be expected):
////////////////////////////////////////////////////////
using F2 = int (std::string_view, float) const noexcept;

//////////////////////////////////////////////////////////////////
// Replace the existing class in "F2" with class "Whatever" but
// "F2" is a "raw" (native C++) function (again, an "abominable"
// function in this example because it contains "const"), so it
// has no existing class (since only member functions belong to
// classes). It's therefore assigned one ("Whatever"), effectively
// converting "F2" to a non-static member function pointer:
//
//    int (Whatever::*)(std::string_view, float) const noexcept
//////////////////////////////////////////////////////////////////
using F2ConvertedToNonStaticMemberFunctionPtr = ReplaceMemberFunctionClass_t<F2, Whatever>;

///////////////////////////////////////////////////////////
// Replace the existing class in "F2" with class "void"
// but "F2" is a "raw" (native C++) function so it has no
// existing class (since only member functions belong to
// classes), nor is "void" a class. Because it's "void"
// however the call will therefore remove the existing
// class but since it has none (since it's a "raw"
// function type), there is nothing to remove. The
// resulting type is therefore just "F2" unaltered:
//
//    int (std::string_view, float) const noexcept
///////////////////////////////////////////////////////////
using F2ReplacedClassWithVoidButNoChange = ReplaceMemberFunctionClass_t<F2, void>;
```
</blockquote></details>

<a name="RemoveNoexcept_t"></a><details><summary>RemoveNoexcept\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using RemoveNoexcept_t;
```
Type alias for "*F*" after removing "*noexcept*" from "*F*" if present.
</blockquote></details>

<a name="RemoveVariadicArgs_t"></a><details><summary>RemoveVariadicArgs\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F>
using RemoveVariadicArgs_t;
```
If "*F*" is a variadic function (its last arg is "..."), yields a type alias for "*F*" after removing the "..." from the argument list. All non-variadic arguments (if any) remain intact (only the "..." is removed).
</blockquote></details>

<a name="ReplaceArg_t"></a><details><summary>ReplaceArg\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          std::size_t I,
          VALID_FUNCTION_ARG_TYPE_C NewArgT>
REQUIRES_I_LESS_THAN_ARG_COUNT(F, I)
using ReplaceArg_t;
```
Type alias for "*F*" after replacing its (zero-based) "*I*th" argument with "*NewArgT*". Pass "*I*" via the 2nd template arg (i.e., the zero-based index of the arg you're targeting), and the type you wish to replace it with via the 3rd template arg ("*NewArgT*"). The resulting alias is therefore identical to "*F*" except its "*I*th" argument is replaced by "*NewArgT*" (so passing, say, (zero-based) 2 for "*I*" and "*int*" for "*NewArgT*" would replace the 3rd function argument with an "*int*"). Note that "*I*" must target an existing arg in the function (i.e., it must be less than the number of arguments in the function) or a constraints violation will occur in C++20 or later (via the REQUIRES_I_LESS_THAN_ARG_COUNT concept seen in the template's declaration), or a "*static\_assert*" in C++17 (a new argument type can't be added using this trait, only an existing argument type replaced). Also note that the "*NewArgT*" template arg must be a legal C++ function argument (parameter) type ("*void*" not permitted for instance) or a constraints violation will occur in C++20 or later (via the VALID_FUNCTION_ARG_TYPE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17 (noting that other than type "*void*", illegal argument types are extremely rare normally). If you need to replace multiple arguments then either call "*ReplaceArg\_t*" again, passing the result of the previous call as the "*F*" template arg in the next call to "*ReplaceArg\_t*" (as many times as you need to, each time specifying a new "*I*" and "*NewArgT*"), or, depending on your needs, rely on [ReplaceArgs\_t](#replaceargs_t) (or its cousin [ReplaceArgsTuple\_t](#replaceargstuple_t), in either case to replace all arg types at once), or [ArgsModify\_t](#argsmodify_t) (or its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t)). "*ReplaceArg\_t*" itself just defers to the latter template (more on this below).

Lastly, note that if "*F*" has variadic arguments (it ends with "..."), then these remain intact. If you need to remove them then call [RemoveVariadicArgs\_t](#removevariadicargs_t) before or after the call to "*ReplaceArg\_t*" (the order is irrelevant).

Finally, note that this template is just the special (common use) case of template [ArgsModify\_t](#argsmodify_t) (or its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t)), where you need to replace a single arg only. The "*Count*" template arg of [ArgsModify\_t](#argsmodify_t) is therefore always passed 1, and its "*NewArgsT*" template arg is simply forwarded the "*NewArgT*" template arg you pass to "*ReplaceArg\_t*" (in addition to its "*I*" template arg to replace that single arg only). "*ReplaceArg\_t*" is cleaner however since the template's name better reflects its purpose and you don't need to explicitly pass the "*Count*" arg of [ArgsModify\_t](#argsmodify_t) (it's always implicitly 1).

*Example:*
```C++
// Sample function (we wish to replace its 2nd arg)
using F = void (char, long, float, double, std::string);

//////////////////////////////////////////////////
// Replace the 2nd arg of the above function (a
// "long") with an "int". Passing (zero-based) 1
// here to target the 2nd arg. Results in:
//
// void (char, int, float, double, std::string)
//////////////////////////////////////////////////
using F_ReplacedArg2WithInt = ReplaceArg_t<F, 1, int>;
```
</blockquote></details>

<a name="ReplaceArgs_t"></a><details><summary>ReplaceArgs\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          VALID_FUNCTION_ARG_TYPE_C... NewArgsT>
using ReplaceArgs_t;
```
Type alias for "*F*" after replacing all its existing non-variadic arguments with the args given by "*NewArgsT*" (a parameter pack of the types that become the new argument list). If none are passed then an empty argument list results instead. The resulting alias is identical to "*F*" itself except that the non-variadic arguments in "*F*" are completely replaced with "*NewArgsT*". Note that if "*F*" is a variadic function (its last parameter is "..."), then it remains a variadic function after the call (the "..." remains intact). If you wish to explicitly add or remove the "..." as well then pass the resulting type to [AddVariadicArgs\_t](#addvariadicargs_t) or [RemoveVariadicArgs\_t](#removevariadicargs_t) respectively (either before or after the call to "*ReplaceArgs\_t*", the order is irrelevant). Note that if you wish to replace specific arguments instead of all of them, then call [ReplaceArg\_t](#replacearg_t) instead. Lastly, you can alternatively use [ReplaceArgsTuple\_t](#replaceargstuple_t) instead of "*ReplaceArgs\_t*" if you have a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) of types you wish to use for the argument list instead of a parameter pack. [ReplaceArgsTuple\_t](#replaceargstuple_t) is identical to "*ReplaceArgs\_t*" otherwise (it ultimately defers to it).

Note that this template is just the special (common use) case of template [ArgsModify\_t](#argsmodify_t) (or its cousin [ArgsModifyTuple\_t](#argsmodifytuple_t)), where you need to replace *all* (non-variadic) args in the function. The "*I*" template arg of [ArgsModify\_t](#argsmodify_t) is therefore always passed zero (0), and the "*Count*" template arg is always passed [ArgCount\_v](#argcount_v)\<F\>. "*ReplaceArgs\_t*" is cleaner however since the name better reflects the template's purpose and neither the "*I*" nor "*Count*" args of [ArgsModify\_t](#argsmodify_t) need to be explicitly passed (they are always implicitly zero (0) and [ArgCount\_v](#argcount_v)\<F\> respectively).

Lastly, please note that the "*NewArgsT*" template arg (parameter pack) must contain valid (legal) C++ function argument (parameter) types ("*void*" not permitted for instance) or a constraints violation will occur in C++20 or later (via the VALID_FUNCTION_ARG_TYPE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17. In particular, if you wish to create a completely empty argument list, don't pass "*void*" to this template, the reason you normally would, in an attempt to create the empty argument list "*(void)*". A compiler error will occur in this case since "*void*" is treated as an actual parameter type in this context (opposed to an empty parameter list) and "*void*" is not a valid C++ parameter type as just described (and the "*...*" for variadic functions isn't removed by this template regardless, should it be present in "*F*" - in such cases it would therefore result in an invalid "*(void, ...)*" parameter list instead). Call [ArgsMakeVoid\_t](#argsmakevoid_t) instead if you wish to create a completely empty parameter list (see this for details).

*Example:*
```C++
////////////////////////////////////////////////
// Sample function (we wish to replace all its
// non-variadic args)
////////////////////////////////////////////////
using F = void (char, long, float, std::string, ...);

///////////////////////////////////////////////////
// Replace all non-variadic of the above function
// with an "int" and a "double". Results in the
// following (note that the "..." for variadic
// functions remains unaffected by this template):
//
// void (int, double, ...)
///////////////////////////////////////////////////
using F_ReplacedArgsWithIntAndDouble = ReplaceArgs_t<F, int, double>;
```
</blockquote></details>

<a name="ReplaceArgsTuple_t"></a><details><summary>ReplaceArgsTuple\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          VALID_FUNCTION_ARG_TYPES_TUPLE_C NewArgsTupleT>
using ReplaceArgsTuple_t;
```
Identical to [ReplaceArgs\_t](#replaceargs_t) just above except its "*NewArgsT*" template arg is passed as a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) instead of a parameter pack (via the 2nd template arg). The types in the [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) are therefore used to replace the existing args in function "*F*". The two templates are identical otherwise (so you can use whichever is more suitable or convenient). See [ReplaceArgs\_t](#replaceargs_t) for details.

Lastly, note that the "*NewArgsTupleT*" template arg must be a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) specialization containing valid (legal) C++ function argument (parameter) types ("*void*" not permitted for instance - see [ReplaceArgs\_t](#replaceargs_t) for details), or a constraints violation will occur in C++20 or later (via the VALID_FUNCTION_ARG_TYPES_TUPLE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17.

*Example:*
```C++
////////////////////////////////////////////////
// Sample function (we wish to replace all its
// non-variadic args)
////////////////////////////////////////////////
using F = void (char, long, float, std::string, ...);

///////////////////////////////////////////////////
// Replace all non-variadic of the above function
// with an "int" and a "double". Results in the
// following (note that the "..." for variadic
// functions remain unaffected by this template):
//
// void (int, double, ...)
///////////////////////////////////////////////////
using NewArgsTuple = std::tuple<int, double>;
using F_ReplacedArgsWithNewArgsTuple = ReplaceArgsTuple_t<F, NewArgsTuple>;
```
</blockquote></details>

<a name="ReplaceCallingConvention_t"></a><details><summary>ReplaceCallingConvention\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          CallingConvention NewCallingConventionT>
using ReplaceCallingConvention_t;
```
Type alias for "*F*" after replacing its calling convention with the platform-specific calling convention corresponding to "*NewCallingConventionT*" (a "*CallingConvention*" enumerator declared in "*FunctionTraits.h*"):
```C++
enum class CallingConvention
{
    Cdecl,
    Stdcall,
    Fastcall,
    Vectorcall,
    Thiscall,
    Regcall,
    Variadic = Cdecl // Variadic functions are always "Cdecl" on all supported compilers
};
```
For instance, if you pass  "*CallingConvention::FastCall*" then the calling convention for "*F*" is replaced with ```__attribute__((fastcall))``` on GCC, Clang and Intel compilers, but ```__fastcall``` on the Microsoft compiler. Note however that the calling convention for variadic functions (those whose last arg is "...") can't be changed in this release. Variadic functions require that the calling function pop the stack to clean up passed arguments and only the "*Cdecl*" calling convention supports that on all supported compilers at this writing. Attempts to change it are therefore ignored (the resulting type is just function "*F*" unchanged). Note that you also can't change the calling convention to an unsupported calling convention. For instance, passing "*CallingConvention::Thiscall*" for a free function (which includes static member functions) is ignored since the latter calling convention applies to non-static member functions only. Similarly, passing a calling convention that's not supported by a given compiler is also ignored, such as passing "*CallingConvention::VectorCall*" on GCC (since it doesn't support this calling convention).

Lastly, please note that compilers will sometimes change the calling convention declared on your functions to the "*Cdecl*" calling convention depending on the compiler options in effect at the time (in particular when compiling for 64 bits opposed to 32 bits, though some calling conventions such as "*Vectorcall*" *are* supported on 64 bits, assuming the compiler supports that particular calling convention). Therefore, if you specify a calling convention that the compiler changes to "*Cdecl*" based on the compiler options currently in effect, then "*ReplaceCallingConvention\_t*" will attempt to change the calling convention to the one you specify but it will be effectively ignored by the compiler which will apply "*Cdecl*" instead. This is normal behavior but caution advised. Changing the calling convention of a function is rare for most but doing so doesn't guarantee the calling convention you're passing will be applied for the reasons discussed (either because the calling convention you pass isn't supported by the target compiler as previously described, or because the compiler changes it to the "Cdecl" calling convention normally due to a 64 bit build or possibly some other compiler option).
</blockquote></details>

<a name="ReplaceReturnType_t"></a><details><summary>ReplaceReturnType\_t</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          VALID_RETURN_TYPE_C NewReturnTypeT>
using ReplaceReturnType_t;
```
Type alias for "*F*" after replacing its return type with "*NewReturnTypeT*". Note that the "*NewReturnTypeT*" template arg must be a valid (legal) C++ function return type or a constraints violation will occur in C++20 or later (via the VALID_RETURN_TYPE_C concept seen in the template's declaration), or a "*static\_assert*" in C++17.
</blockquote></details>

---
<a name="AdditionalHelperTemplates"></a>
### _Additional helper templates_

<a name="DECLARE_CLASS_HAS_FUNCTION"></a><details><summary>DECLARE\_CLASS\_HAS\_FUNCTION[^9]</summary>

<blockquote>

```C++
#define DECLARE_CLASS_HAS_FUNCTION(NAME)
```
This macro and its cousins declare templates allowing you to determine if an arbitrary class has a (public) member function with a given name and [function type](#cppfunctiontypeformat). For an overview and comparative summary of these macros and the templates they create (with full examples), please see [Determining if a member function exists](#determiningifamemberfunctionexists). The *DECLARE\_CLASS\_HAS\_FUNCTION* macro specifically detects *non-static* (public) member functions only, whether overloaded or not (the other macros documented further below provide slightly different flavors to target *static* member functions and/or *non-overloaded* functions). *DECLARE\_CLASS\_HAS\_FUNCTION* allows you to do this by declaring templates taking the class type you're targeting as the 1st template arg, and a function type as the 2nd template arg, such as ```int (std::string_view, float) const noexcept``` (passed using any format supported by this library - see [Template arg "F"](#templateargf)). The templates will then determine if the given class (the 1st template arg) has a *non-static* (public) member function with the name you're targeting (read on), whose [raw](#rawfunctiontypes) C++ function type exactly matches that of the 2nd template arg. The target function name, "*Whatever*" for instance, is passed to the macro itself and embedded in the template's name. The templates created by the macro therefore always target functions with that specific name only. To this end, the *DECLARE\_CLASS\_HAS\_FUNCTION* macro is used to declare template "*ClassHasFunction\_NAME*", a [std::bool\_constant](https://en.cppreference.com/w/cpp/types/integral_constant#Helper_alias_templates) derivative that resolves to [std::true\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) or [std::false\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) if the function exists or not respectively (more on this below), and a "*bool*" helper variable "*ClassHasFunction\_NAME\_v*" you'll normally use instead (the latter template just defers to the former template). The "*ClassHasFunction\_*" prefix and "*\_v*" suffix in the templates' names are hardcoded and never change. The "*A*" portion of the templates however is the name of the *non-static* (public) member function you wish to detect which you pass via the macro's NAME arg like so (function name "*Whatever*" in this example). You'll always declare this at namespace scope:
```C++
DECLARE_CLASS_HAS_FUNCTION(Whatever)
```
This declares the following two templates used to check for the *non-static* (public) member function "*Whatever*" in any arbitrary class (that matches the function type you're interested in). Again, the 2nd template below is just a helper (bool) variable template with the usual "*\_v*" suffix that defers to the 1st template in the usual C++ way (as seen in its declaration below):
```C++
template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
struct ClassHasFunction_Whatever : std::bool_constant<> // Base class described below
{
};

template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
inline constexpr bool ClassHasFunction_Whatever_v = ClassHasFunction_Whatever<T, F>::value;
```
These templates (again, you'll usually use the "*\_v*" version just above), can then be used to determine if the class given by template arg "*T*" has a *non-static* (public) member function named "*Whatever*" with the exact function type passed via template arg "*F*" (which must conform to any function type supported by this library - see [Template arg "F"](#templateargf)). The comparison itself occurs between the [raw](#rawfunctiontypes) function type of function "*Whatever*", assuming a *non-static* (public) member function with this name exists in class "*T*", and the [raw](#rawfunctiontypes) function type of template arg "*F*" (see [C++ function types at a glance](#cppfunctiontypesataglance) for details on "raw" (native) C++ function types). The "*ClassHasFunction\_Whatever*" template above always inherits from [std::true\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) if the [raw](#rawfunctiontypes) function types are identical (every component of their [C++ function types](#cppfunctiontypeformat) must match exactly including return type, calling convention, etc.), or [std::false\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) otherwise ([std::true\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) and [std::false\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) are just specializations of [std::bool\_constant](https://en.cppreference.com/w/cpp/types/integral_constant#Helper_alias_templates) seen in the template's declaration above - note that its "*true*" or "*false*" template arg is omitted in the [std::bool\_constant](https://en.cppreference.com/w/cpp/types/integral_constant#Helper_alias_templates) declaration above however since it's an implementation detail handled by the library itself). Again, the second template above is just the "*bool*" equivalent.

For instance, after calling this macro via ```DECLARE_CLASS_HAS_FUNCTION(Whatever)```, you can then check any class to determine if it has a *non-static* (public) member function called "*Whatever*" with the given function type you're interested in, such as the following:
```C++
////////////////////////////////////////////////////////
// Only file in this repository you need to explicitly
// #include (see "Usage" section earlier)
////////////////////////////////////////////////////////
#include "FunctionTraits.h"

///////////////////////////////////////////////////////
// Declares templates "ClassHasFunction_Whatever" and
// its bool helper "ClassHasFunction_Whatever_v"
// described above (the latter invoked below). Always
// declare this at namespace scope.
///////////////////////////////////////////////////////
DECLARE_CLASS_HAS_FUNCTION(Whatever)

///////////////////////////////////////////////////////////
// Class you wish to check for non-static (public) member
// function "Whatever" with the exact type of the first
// overload below ("ClassHasFunction_Whatever_v" called
// further below will detect it regardless of overloads).
///////////////////////////////////////////////////////////
class Test
{
public:
    int Whatever(std::string_view, float) const noexcept;

    /////////////////////////////////////////////////////
    // Overload but above function will still be found.
    // Template "ClassHasFunction_Whatever_v" below
    // works even when "Whatever" is overloaded.
    /////////////////////////////////////////////////////
    int Whatever(char) const noexcept;
};

int main()
{
    ///////////////////////////////////////////////////////////
    // Returns true since class "Test" (the 1st template arg)
    // has a non-static (public) member function called
    // "Whatever" with the exact type being passed via the
    // 2nd template arg (so we're passing the actual C++
    // function type matching "Whatever" above in this case,
    // though you can pass this using any function type
    // supported by this library, since only the "raw"
    // function type of the function you pass is relevant -
    // see "C++ Function Type Format" earlier in this
    // document).
    ///////////////////////////////////////////////////////////
    constexpr bool classHasFunction_Whatever = ClassHasFunction_Whatever_v<Test,
                                                                           int (std::string_view, float) const noexcept>;

    return 0;
}
```
The above variable, "*classHasFunction\_Whatever*", will be set to "*true*" because class "*Test*" (the 1st template arg passed in the above call) has a *non-static* (public) member function called "*Whatever*" (the *NAME* arg passed to the *DECLARE\_CLASS\_HAS\_FUNCTION* macro), whose [function type](#cppfunctiontypeformat) exactly matches the 2nd template arg being passed. Again, note that the 2nd template arg "*F*" can be any function type supported by this library (see [Template arg "F"](#templateargf)), but only the [raw](#rawfunctiontypes) function type of "*F*" is relevant (see [C++ function types at a glance](#cppfunctiontypesataglance)). In this example we're passing the actual [raw](#rawfunctiontypes) function type itself opposed to a pointer and/or reference to the function (or any other function type supported by the library, such as a functor, whose "*operator()*" member would then be used for "*F*"), so ```int (std::string_view, float) const noexcept``` in this case (exactly matching the type of function "*Test::Whatever*"). A match will occur if *non-static* function "*Whatever* exists in class "*Test*" and its [raw](#rawfunctiontypes) C++ function type matches the [raw](#rawfunctiontypes) C++ function type of "*F*", so any pointers and/or references that may exist in "*F*" (if it's a function pointer and/or reference) are immaterial (the pointer and/or reference is removed to retrieve the [C++ function type](#cppfunctiontypeformat)). Similarly, if "*F*" is a *non-overloaded* functor, the [raw](#rawfunctiontypes) function type of its "*operator*()" member will be used for the comparison. The [raw](#rawfunctiontypes) type of "*F*" is always used for all supported types of [Template arg "F"](#templateargf).

 "*true*" is therefore returned by the template only if the class you pass for the 1st template arg has a *non-static* (public) member function named "*Whatever*" with the exact ([raw](#rawfunctiontypes)) C++ function type of "*F*". This means that every component comprising a C++ function type is tested for in function "*Whatever*" and must match your 2nd template arg *exactly* for these templates to return "*true*" (including the return type, calling convention, etc.). The comparison therefore looks for an exact function match of the 2nd template arg (of its [raw](#rawfunctiontypes) C++ function type) so caution advised. In the above example for instance, if the return type of "*Test::Whatever*" is changed from "*int*" to, say, "*char*", then the call to "*ClassHasFunction\_Whatever\_v*" will no longer return "*true*" (unlike how function signatures are handled in overloaded function resolution in C++ for instance, where the return type *doesn't* factor in - it *does* here however). It will (therefore) now return "*false*" unless you also change "*int*" to "*char*" in the 2nd template arg so that the return types match again. Also note that parameter conversions aren't relevant, so if you pass, say, ```int (std::string_view, int) const noexcept``` as the 2nd template arg instead, so the "*float*" parameter in the original call has now been changed to "*int*", then again, the template will now return "*false*" even though "*int*" can be implicitly converted to "*float*" (the actual type of the 2nd arg of "*Test::Whatever*"). Again, these templates look for an *exact* match of the [raw](#rawfunctiontypes) function type you pass as the 2nd template arg so every component comprising a C++ function (again, see [C++ Function Type Format](#cppfunctiontypeformat)) must match *exactly* for "*true*" to be returned (so it's effectively the equivalent of passing the [raw](#rawfunctiontypes) function type of "*Test::Whatever*" and the [raw](#rawfunctiontypes) function type of the 2nd template arg of "*ClassHasFunction\_Whatever\_v*" to [std::is\_same](https://en.cppreference.com/w/cpp/types/is_same) - the latter targets *every* component of a [C++ function type](#cppfunctiontypeformat) when passed a function type, including return type, calling convention, etc.).
</blockquote></details>

<a name="DECLARE_CLASS_HAS_STATIC_FUNCTION"></a><details><summary>DECLARE\_CLASS\_HAS\_STATIC\_FUNCTION[^9]</summary>

<blockquote>

```C++
#define DECLARE_CLASS_HAS_STATIC_FUNCTION(NAME)
```
This macro is functionally equivalent to macro [DECLARE\_CLASS\_HAS\_FUNCTION](#declare_class_has_function) just above but targets *static* member functions instead of *non-static*. See the latter macro for details. The following templates therefore won't detect functions unless they're *static*. The macro creates the following two templates for this (assuming "*Whatever*" is passed to the macro):
```C++
template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
struct ClassHasStaticFunction_Whatever : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
inline constexpr bool ClassHasStaticFunction_Whatever_v = ClassHasStaticFunction_Whatever<T, F>::value;
```
These templates are just the *static* counterparts of the templates targeting *non-static* functions created by [DECLARE\_CLASS\_HAS\_FUNCTION](#declare_class_has_function). Again, see the latter macro for details. Please note however that unlike the templates created by the latter macro, if template arg "*F*" contains the "*const*" and/or "*volatile*" and/or "*&*" or "*&&*" qualifiers, which don't apply to *static* member functions, the above templates will always return "*false*" (since a *static* member function will never have these qualifiers).
</blockquote></details>

<a name="DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION"></a><details><summary>DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION[^9]</summary>

<blockquote>

```C++
#define DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION(NAME)
```
This macro is functionally equivalent to macro [DECLARE\_CLASS\_HAS\_FUNCTION](#declare_class_has_function) but targets *non-overloaded* member functions only. These templates will therefore return "*true*" only if the *non-static* function you target exists *and* is not overloaded (unlike the latter macro whose templates will detect *non-static* functions whether overloaded or not). The two macros are functionally equivalent otherwise. See [DECLARE\_CLASS\_HAS\_FUNCTION](#declare_class_has_function) for details.

The macro creates the following two templates for this (assuming "*Whatever*" is passed to the macro):

```C++
template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
struct ClassHasNonOverloadedFunction_Whatever : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
inline constexpr bool ClassHasNonOverloadedFunction_Whatever_v = ClassHasNonOverloadedFunction_Whatever<T, F>::value;
```
These templates are identical to those created by [DECLARE\_CLASS\_HAS\_FUNCTION](#declare_class_has_function) but again, only return "*true*" if the target function is not overloaded. See the latter macro for complete details.
</blockquote></details>

<a name="DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION"></a><details><summary>DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_STATIC\_FUNCTION</summary>

<blockquote>

```C++
#define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION(NAME)
```
This macro is functionally equivalent to macro [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function) just above but targets *static* member functions instead of *non-static*. See the latter macro for details. The following templates therefore won't detect functions unless they're *static* as well as *non-overloaded* (since both macros detect *non-overloaded* functions only). The macro creates the following two templates for this (assuming "*Whatever*" is passed to the macro):
```C++
template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
struct ClassHasNonOverloadedStaticFunction_Whatever : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
inline constexpr bool ClassHasNonOverloadedStaticFunction_Whatever_v = ClassHasNonOverloadedStaticFunction_Whatever<T, F>::value;
```
These templates are just the *static* counterparts of the templates targeting *non-static* functions created by [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function). Again, see the latter macro for details. Please note however that unlike the templates created by the latter macro, if template arg "*F*" contains the "*const*" and/or "*volatile*" and/or "*&*" or "*&&*" qualifiers, which don't apply to *static* member functions, the above templates will always return "*false*" (since a *static* member function will never have these qualifiers).
</blockquote></details>

<a name="DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS"></a><details><summary>DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS</summary>

<blockquote>

*NOTE: This section is quite lengthy since it details a powerful and flexible feature*

```C++
#define DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS(NAME)
```
This macro is similar to [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function), each creating templates to determine if a class or struct has a *non-overloaded*, *non-static* (public) member function with a given name and [function type](#cppfunctiontypeformat). Unlike the latter macro however, whose templates always detect the target function based on an *exact* match of the [function type](#cppfunctiontypeformat) you pass (via the 2nd template arg), the following macro allows you to perform a customized match instead (based on the specific function traits you're interested in). You therefore have complete control over which components of the [function's type](#cppfunctiontypeformat) you wish to inspect for a match, unlike the latter macro whose templates always check the entire type (i.e., every component of the [function's type](#cppfunctiontypeformat)).

Before proceeding further however, please consult [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function) to familiarize yourself with the basic details first. This will allow you to better understand the difference between the templates created by that macro and the ones created in the following section. An overview and comparative summary of all the function detection macros and the templates they create can also be found under [Determining if a member function exists](#determiningifamemberfunctionexists).

As described under [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function), the latter macro creates templates used to detect if the *non-overloaded*, *non-static* (public) member function you're checking for exists and is an *exact* type match of the [function type](#cppfunctiontypeformat) you pass as the 2nd template arg (say, ```int (std::string_view, float) const noexcept```, though any function type supported by this library may be passed - see [Template arg "F"](#templateargF)). The following macro however, *DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS*, creates templates allowing you to perform a customized match instead, so instead of passing a function type to compare with, as required by the [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function) templates (via the 2nd template arg), you'll effectively pass the specific function traits you wish to compare with instead (as the 2nd template arg). You'll do this if you need to ignore certain components of the function like its return type, its calling convention, its *noexcept* specification, etc. (again, see [C++ Function Type Format](#cppfunctiontypeformat)), but you can check for anything required. You can check for specific function parameters only for instance (using [IsArgTypeSame\_v](#isargtypesame_v) usually), assuming you don't need to check them all (using [IsArgTypesSame\_v](#isargtypessame_v) usually). You have complete control over the matching process. Therefore, choose [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function) if you wish to check if a *non-overloaded*, *non-static* (public) member function exists and precisely matches the exact function type you pass (every component described under [C++ Function Type Format](#cppfunctiontypeformat) must precisely match), or *DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS* if you need more fine-grained control over the matching criteria (i.e., you need to check specific function traits only).

The DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS macro is called the same way as [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function) (and all other function detection macros in the library). You simply pass the name of the *non-overloaded*, *non-static* (public) member function you wish to detect via the macro's NAME arg ("*Whatever*" in the following example). Like the other macros you'll always declare this at namespace scope:

```C++
DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS(Whatever)
```
This declares the following two templates, where the name of each template always starts with "*ClassHasNonOverloadedFunctionTraits\_*" followed by the *NAME* arg passed to the macro (representing the name of the member function you wish to detect, "*Whatever*" in this case). Like all the other member function detection templates in this library, the 1st template below inherits from [std::true\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) if a match is found (described below), or [std::false\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) otherwise. The 2nd template below is just a (bool) helper variable template with the usual "*\_v*" suffix that defers to the 1st template in the usual C++ way:
```C++
template <IS_CLASS_C T,
          IS_CLASS_C HasFunctionTraitsT,
          typename UserTypeT = void>
REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
struct ClassHasNonOverloadedFunctionTraits_Whatever : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          IS_CLASS_C HasFunctionTraitsT,
          typename UserTypeT = void>
REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
inline constexpr bool ClassHasNonOverloadedFunctionTraits_Whatever_v = ClassHasNonOverloadedFunctionTraits_Whatever<T, HasFunctionTraitsT, UserTypeT>::value;
```
These templates (you'll usually use the "*\_v*" version just above) can then be used to determine if the class or struct passed via the 1st template arg (*T*), has a *non-overloaded*, *non-static* (public) member function called "*Whatever*" whose traits match those passed via the 2nd template arg ("*HasFunctionTraitsT*", described shortly). Again, the struct itself (the first template above) always inherits from [std::true\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) if so or [std::false\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) otherwise (and the second template above is just the "*bool*" equivalent). Note that unlike the [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function) templates however, where the 2nd template arg is the actual function type you wish to match, the 2nd arg of both templates above, "*HasFunctionTraitsT*", must be a functor instead, whose "*operator()*" member is a template in its own right, opposed to a non-template member normally associated with "*operator()*" (since it's not normally a template in order to fulfill its usual purpose in C++, but for the purposes here it must be). If the *non-static* member function "*Whatever*" exists in class "*T*" and is not overloaded (since the templates created by the *DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS* macro detect *non-overloaded* functions only), then your "*operator()*" member (template) will be invoked with the type of "*&T::Whatever*" as its 1st template arg (your "*operator()*" template will usually call this "*F*" for consistency with the rest of the library - an example will clarify the details in a moment). Your "*operator()*" member (template) should then simply return "*true*" if "*F*" matches your search criteria or "*false*" otherwise (note that your "*operator*()" member can also be optionally declared with a 2nd template arg for passing your own user-defined type if you require it, but more on this later). "*ClassHasNonOverloadedFunctionTraits\_Whatever"* will then inherit from [std::true\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) or [std::false\_type](https://en.cppreference.com/w/cpp/types/integral_constant#Specializations) based on the return value of "*operator()*", indicating whether "*T::Whatever*" exists or not based on your search criteria. "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v"* will therefore resolve to "*true*" or "*false*" to reflect this as well.

*Example*:

```C++
// Only header you need to explicitly #include in this library
#include "FunctionTraits.h"

// Everything in the library declared in this namespace
using namespace StdExt;

///////////////////////////////////////////////////////////
// Class you wish to check for non-overloaded, non-static
// member function "Whatever" with the specific traits
// you're interested in (via the "HasWhateverTraits"
// template declared further below)
///////////////////////////////////////////////////////////
class Test
{
public:
    // Non-overloaded, non-static function we wish to detect
    int Whatever(std::string_view, float) const noexcept;

    /////////////////////////////////////////////////////////////////////
    // Commented out. If uncommented then "Whatever" becomes overloaded
    // so "ClassHasNonOverloadedFunctionTraits_Whatever_v" called below
    // will then return false instead (since it only returns true for
    // *non-overloaded* functions)
    /////////////////////////////////////////////////////////////////////
    // int Whatever(char) const noexcept;
};

///////////////////////////////////////////////////////
// Call this macro to declare templates
// "ClassHasNonOverloadedFunctionTraits_Whatever" and
// its "ClassHasNonOverloadedFunctionTraits_Whatever_v"
// helper variable described previously
///////////////////////////////////////////////////////
DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS(Whatever)

///////////////////////////////////////////////////////////
// For passing to the "HasFunctionTraitsT" template arg
// of the above templates (see usage in "main()" below).
// The following struct must be default constructible.
// Note however that another similar technique exists you
// can also rely on and in C++20 or later you can also use
// an equivalent lambda template instead of the following
// struct, which is more convenient. Both techniques will
// be shown shortly.
///////////////////////////////////////////////////////////
struct HasWhateverTraits
{
    ////////////////////////////////////////////////////
    // This template must always exist, and if the type
    // of the non-overloaded, non-static member
    // function being checked for exists when
    // "ClassHasNonOverloadedFunctionTraits_Whatever_v"
    // is called below, namely, "Test::Whatever", then
    // the following will be invoked, passing it the
    // type of "&Test::Whatever" via the "F" template
    // arg. The code below then simply returns "true"
    // if that function meets the search (detection)
    // criteria or "false" otherwise.
    ////////////////////////////////////////////////////
    template <TRAITS_FUNCTION_C F>
    constexpr bool operator()() const noexcept
    {
        return IsReturnTypeSame_v<F, int> && // Must have "int" return type
               IsArgTypesSame_v<F, AndVariadic::IsFalse, std::string_view, float>; // Must have "(std::string_view, float)" arg list
                                                                                    // and not be variadic (can't end with "...")
    }
};

int main()
{
    ////////////////////////////////////////////////////////
    // Checks if class "Test" (the 1st template arg), has
    // a non-overloaded, non-static member function called
    // "Whatever". If so then the template will invoke
    // "HasWhateverTraits::operator()", specializing it on
    // the type of "&Test::Whatever", so its "F" template
    // arg will resolve to the following in this example:
    //
    //   int (Test::*)(std::string_view, float) const noexcept
    //
    // "HasWhateverTraits::operator()" will then return "true"
    // if the above type matches the search criteria (it does
    // in this example), or "false" otherwise.
    ////////////////////////////////////////////////////////
    constexpr bool classHasNonOverloadedFunctionTraits_Whatever = ClassHasNonOverloadedFunctionTraits_Whatever_v<Test,
                                                                                                                 HasWhateverTraits>;
    return 0;
}
```

The code above (as described in its comments), determines if class "*Test*" has a *non-overloaded*, *non-static* (public) member function called "*Whatever*" with the specific traits specified in the "*HasWhateverTraits*" struct above (details to follow). As noted in the code above however, there's another similar technique you can also rely on instead, and in C++20 or later you can also use an equivalent lambda template instead, which is often more convenient. Both techniques will be shown shortly.

In the above example, the "*operator()*" (template) member of "*HasWhateverTraits*" will be specialized on the type of *non-overloaded*, *non-static* (public) member function "*&Test::Whatever*" if it actually exists ("*operator()*" isn't called otherwise), so its "*F*" template arg will resolve to ```int (Test::*)(std::string_view, float) const noexcept``` in this example (i.e., the type of "*&Test::Whatever*"). The "*operator()*" (template) member in the above example is then simply testing that function for an "*int*" return type and an argument list matching ```(std::string_view, float)``` (and explicitly no variadic args), but you can test for anything you require here. Contrast this with the templates created by [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function), which always match every component of the function instead (again, see [C++ Function Type Format](#cppfunctiontypeformat)).

As seen in the above example, your "*operator()*" template will normally just invoke whatever traits you're interested in from this library, returning "*true*" or "*false*" to indicate if function "*F*" exists or not (again, "*F*" resolves to "*&Test::Whatever*" in this example). In the above example, we're only interested in the function's return type and argument (parameter) types, hence we invoked ```IsReturnTypeSame_v<F, int>``` and ```IsArgTypesSame_v<F, AndVariadic::IsFalse, std::string_view, float>``` (in the latter case the "*AndVariadic::IsFalse*" template arg also ensures the function isn't variadic). Therefore, when "*HasWhateverTraits*" is passed as the 2nd template arg to "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*" above, its "*operator()*" member will return "*true*" if the 1st template arg of "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*" is a class or struct with a *non-overloaded*, *non-static* (public) member function called "*Whatever*" (i.e., the *NAME* arg passed to the *DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS* macro), but only if the return type of "*Whatever*" is declared an "*int*" and the function takes 2 args of type "*std::string\_view*" and *float*" (and exactly those types only - conversions to those types don't factor in because the "*HasWhateverTraits*" struct in this example is relying on *[IsReturnTypeSame\_v](#isreturntypesame_v)* and *[IsArgTypesSame\_v](#isargtypessame_v)* which both perform an *exact* type match - the latter call is also ensuring your function isn't variadic via its "*AndVariadic::IsFalse*" template arg). If you wish to loosen the check to support conversions as well, or anything else then just modify your "*HasWhateverTraits*" struct to do so.

However, it was previously mentioned that another similar technique also exists you can rely on instead of the above, which may be better depending on your needs (or preference). Instead of having your "*operator()*" member directly test "*F*" for the hardcoded traits you're interested in as seen in the above example, you can rely on the following variation instead by using the optional "*UserTypeT*" template arg of "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*". To do so, we simply make the following change to the "*operator()*" member of "*HasWhateverTraits*":
```C++
struct HasWhateverTraits
{
    template <TRAITS_FUNCTION_C F,
              TRAITS_FUNCTION_C CompareF>
    constexpr bool operator()() const noexcept
    {
        return IsReturnTypeMatch_v<F, CompareF> && // "F" must have same return type as "CompareF" ("int" in this example)
               IsArgTypesMatch_v<F, CompareF>; // "F" must have the same args as "CompareF" ("std::string_view" and "float" in this example)
    }
};
```
As in the previous example, you pass this class via the "*HasFunctionTraitsT*" template arg of "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*" but note that "*operator()*" above now has a 2nd template arg called "*CompareF*", unlike the previous example which only had one template arg ("*F"* only). The 2nd template arg can be optionally passed any type you require which you'll do via the optional "*UserTypeT*" template arg of "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*". It defaults to "*void*" however so if you don't explicitly pass it then the "*operator()*" template of your "*HasWhateverTraits*" must only have one template arg, as seen in the previous example (though you're also free to declare it with more template args so long as they're all assigned default values, but more than 2 would be unusual, serving no beneficial purpose normally). If you explicitly pass a *non-void* template arg for the "*UserTypeT*" parameter of "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*" however, then the "*operator()*" member of your "*HasWhateverTraits*" class *must* have 2 template args to accommodate it, as seen in the above example (though again, more than 2 are supported if they're all assigned default values, but it wouldn't serve any beneficial purpose as previously noted). The "*UserTypeT*" parameter (type) you pass to "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*" will simply be passed as the 2nd template arg to your "*operator()*" function but note in the above implementation of "*operator()*" that we've called it "*CompareF*" instead. That's because in this particular example, we're going to pass a function type for the "*UserTypeT*" parameter of "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*", whose traits we expect member function "*Whatever*" to match (i.e., a comparison function for this purpose). We've therefore given it a more appropriate name than "*UserTypeT*" in the actual "*operator()*" member above, "*CompareF*", though you can pass anything you want for the "*UserTypeT*" template parameter and simply name it accordingly in your "*operator()*" function (such as, say, "*ArgTypesToCompareTupleT*" for passing a [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) of types you wish to compare with template arg "*F*", usually using [IsArgTypesSameTuple\_v](#isargtypessametuple_v)). In practice however most users will typically use it for the purpose we're using it for in this example (allowing you to pass in an entire function type for comparing whichever of its components are relevant), so you'll normally want to stick with the name "*CompareF*" or any other similar name you prefer instead.

Note that using this technique (relying on "*UserTypeT*" which we've simply called "*CompareF*" in "*operator()*" itself), we can now check for the presence of member function "*Test::Whatever*" like so:
```C++
constexpr bool classHasNonOverloadedFunctionTraits_Whatever = ClassHasNonOverloadedFunctionTraits_Whatever_v<Test,
                                                                                                             HasWhateverTraits,
                                                                                                             int (std::string_view, float) const noexcept>;
```
This call is identical to the previous example except we're now passing our user-defined type via the "*UserTypeT*" template arg of "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*", which we didn't explicitly pass in the previous example (so it defaulted to "*void*" and was therefore ignored). Because we're now explicitly passing it however, it's no longer "*void*" so your "*operator()*" member must have an extra template arg to accommodate it, as previously described. Again, we've simply called this "*CompareF*" in the "*operator()*" template of the above example and as you can see, we're passing the function to compare "*F*" with, i.e., ```int (std::string_view, float) const noexcept```, in our call to "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v* (via its "*UserTypeT*" template arg). Our implementation of "*operator()*" now performs the same check as it did in the previous example, only this time it checks the return type and argument types of "*&Test::Whatever*" (template arg "*F*") against those in the function type that we passed via "*UserTypeT*", again, ```int (std::string_view, float) const noexcept```, which is simply passed to "*CompareF*" in "*operator()*". This version of "*operator()*" is therefore identical to the previous example except now it does its job by comparing the traits of "*&Test::Whatever*" with those of the function we passed via the "*UserTypeT*" arg (which we named "*CompareF*" in "*operator()*"), instead of the previous example which directly tested "*&Test::Whatever*" for those traits instead. Both versions therefore check for the same traits in "*&Test::Whatever*" but using the two different approaches. Whether you'ld prefer to use the technique of the previous example or the one just described is entirely up to you however (and your particular needs). The example just described does allow you to dynamically change the traits you wish to compare however, by effectively extracting them from a function you pass on-the-fly using the "*UserTypeT*" template arg. That's its purpose in the above design though again, you can use "*UserTypeT*" for any purpose you wish. The above technique will likely be the most common for most however.

The upshot in both examples above, whether using the optional "*UserTypeT*" template arg or not, is that the "*HasFunctionTraitsT*" template arg of "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*" can be customized to check for a *non-overloaded*, *non-static* (public) member function called "*Whatever* by simply customizing the "*operator()*" template member of "*HasFunctionTraitsT*" to test for "*Whatever*" using any required criteria. The optional "*UserTypeT*" arg of "*ClassHasNonOverloadedFunctionTraits\_Whatever\_v*" can also be used if you wish to pass your own user-defined type to "*operator()*" as well, for any required purpose but typically to support the "*CompareF*" technique just demonstrated.

In either case above, whether you use the "*UserTypeT* template arg or not, you can take the following general approach to creating your "*HasFuntionTraits*" class. First, note that as described in the [C++ function types at a glance](#cppfunctiontypesataglance) but repeated here for your convenience, function types in C++ are always of the following form, where items in square brackets are optional (but see the latter link for details):

<pre><a href="#returntype">ReturnType</a> [<a href="#callingconvention">CallingConvention</a>] ([<a href="#args">Args</a>] [[,] <a href="#variadic">...</a>]) [<a href="#const">const</a>] [<a href="#volatile">volatile</a>] [<a href="#refqualifiers">&amp;|&amp;&amp;</a>] [<a href="#noexcept">noexcept</a>[(true_or_false_expression)]]</pre>

You're therefore free to check for any of the components comprising a C++ function just above by writing a template analogous to "*HasWhateverTraits*" and checking for only those components you're interested in. To this end, if "*Test::Whatever*" were explicitly declared with *every* possible component of a C++ function as seen just above, such as the following (just to emphasize the situation) ...
```C++
class Test
{
public:
    ////////////////////////////////////////////////////////////
    // Note that STDEXT_CC_STDCALL isn't officially documented
    // in this README.md file, but still available for use by
    // users of "FunctionTraits.h" (a #defined constant that
    // resolves to the platform-specific "stdcall" calling
    // convention - one such constant exists for each calling
    // convention supported by "FunctionTraits").
    ////////////////////////////////////////////////////////////
    int STDEXT_CC_STDCALL Whatever(std::string_view, float, ...) const volatile & noexcept;
};
```
... then a "*HasFunctionTraits*" struct to check for *every* possible function component in "*Whatever*" above would be declared as follows (but if checking *every* component then you can just rely on [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function) instead - that's its purpose and it's both easier and cleaner, but the purpose of the following will be come apparent in a moment):
```C++
struct HasAllTraits
{
    template <TRAITS_FUNCTION_C F>
    constexpr bool operator()() const noexcept
    {
        return IsReturnTypeSame_v<F, int> && // Must have "int" return type
               CallingConvention_v<F> == CallingConvention::Stdcall && // Must have the "Stdcall" calling convention (though usually you'll rely on "DefaultCallingConvention_v" here instead)
               IsArgTypesSame_v<F, AndVariadic::IsFalse, std::string_view, float> && // Must have 2 args "std::string_view" and "float"
               IsFunctionConst_v<F> && // Must be "const"
               IsFunctionVolatile_v<F> && // Must be "volatile"
               FunctionReference_v<F> == FunctionReference::LValue && // Must have the ref-qualifier "&" (though rare in practice)
               IsNoexcept_v<F>;
    }
};
```
This struct doesn't rely on the "*UserTypeT*" ("*CompareF*") template arg technique described previously (though it easily can and some may prefer it), but notwithstanding that, in practice you can customize your own version of the above struct by simply copying it into your code and removing any of the conditions that don't matter to you (and renaming the struct to something appropriate for whatever it's checking). For instance, removing the call to [CallingConvention\_v](#callingconvention_v) seen above means the calling convention will no longer be checked so it can be anything (i.e., the calling convention of "*Whatever*" isn't relevant to you). The same applies to the other traits seen above as well. Simply remove those you don't care about and keep the rest (and again, give your struct a meaningful name). Note that for the call to [IsArgTypesSame\_v](#isargtypessame_v) above, you can even replace this with calls to [IsArgTypeSame\_v](#isargtypesame_v) if you wish to drill down into individual arguments (if you don't need to check them all), or perform any other type of argument checks you may require. Otherwise just rely on [IsArgTypesSame\_v](#isargtypessame_v) as seen above.

Finally, as also briefly mentioned earlier, if targeting C++20 or later then it's often much more convenient to pass a lambda template type for the "*HasFunctionTraitsT*" template arg instead of the structs seen in the previous examples (lambda templates aren't available until C++20 but you can check the library's LAMBDA_TEMPLATES_SUPPORTED[^7] macro to determine if they're supported - the macro will only be #defined if they are). For instance, you can replace the "*HasAllTraits*" struct just above with the following equivalent lambda template instead (which can be conveniently declared at function scope opposed to the "*HasAllTraits*" struct above):
```C++
using HasAllTraits = decltype([]<TRAITS_FUNCTION_C F>() noexcept
                              {
                                  return IsReturnTypeSame_v<F, int> && // Must have "int" return type
                                         CallingConvention_v<F> == CallingConvention::Stdcall && // Must have the "Stdcall" calling convention (though usually you'll rely on the "DefaultCallingConvention_v" template here instead or just ignore the calling convention altogether)
                                         IsArgTypesSame_v<F, AndVariadic::IsFalse, std::tuple<std::string_view, float> > && // Must have these 2 args only: "std::string_view" and "float"
                                         IsFunctionConst_v<F> && // Must be "const"
                                         IsFunctionVolatile_v<F> && // Must be "volatile"
                                         FunctionReference_v<F> == FunctionReference::LValue && // Must have the "&" ref-qualifier (though rare in practice)
                                         IsNoexcept_v<F>;
                             });

constexpr bool classHasNonOverloadedFunctionTraits_Whatever = ClassHasNonOverloadedFunctionTraits_Whatever_v<Test,
                                                                                                             int (std::string_view, float) const noexcept>;
```
Note that you can even just call it on-the-fly if you prefer like so (the following is quite verbose however, given the long names, so the linefeed after the variable's '=' sign is by design):
```C++
constexpr bool classHasNonOverloadedFunctionTraits_Whatever =
    ClassHasNonOverloadedFunctionTraits_Whatever_v<Test,
                                                   decltype([]<TRAITS_FUNCTION_C F>() noexcept
                                                            {
                                                               return IsReturnTypeSame_v<F, int> && // Must have "int" return type
                                                                      CallingConvention_v<F> == CallingConvention::Stdcall && // Must have the "Stdcall" calling convention (though usually you'll rely on the "DefaultCallingConvention_v" template here instead or just ignore the calling convention altogether)
                                                                      IsArgTypesSame_v<F, AndVariadic::IsFalse, std::string_view, float> && // Must have these 2 args only: "std::string_view" and "float"
                                                                      IsFunctionConst_v<F> && // Must be "const"
                                                                      IsFunctionVolatile_v<F> && // Must be "volatile"
                                                                      FunctionReference_v<F> == FunctionReference::LValue && // Must have the "&" ref-qualifier (though rare in practice)
                                                                      IsNoexcept_v<F>;
                                                            }
                                                           )>;
```
Behind the scenes, lambda templates like these simply resolve to the equivalent of the (previous) "*HasAllTraits*" struct (the compiler generates a struct with a template version of "*operator()*"), so it's effectively identical to the "*HasAllTraits*" struct. It also supports the optional "*UserTypeT*" template arg described earlier. Just add this as an extra template arg to your lambda if you wish, no different than you would for a non-lambda struct (usually calling it "*CompareF*" as described in the earlier example). The "*HasAllTraits*" example above would therefore now distill down to the following (a complete working example targeting C++20 or later so we can use a lambda template but also relying on the "*UserTypeT*" template arg technique previously shown):

```C++
// Only header you need to explicitly #include in this library
#include "FunctionTraits.h"

// Everything in the library declared in this namespace
using namespace StdExt;

///////////////////////////////////////////////////////////
// Class you wish to check for non-overloaded, non-static
// member function "Whatever" with the specific traits
// you're interested in (via the "HasAllTraits" lambda
// template declared in "main()" below)
///////////////////////////////////////////////////////////
class Test
{
public:
    // Non-overloaded, non-static function we wish to detect
    int Whatever(std::string_view, float) const noexcept;

    /////////////////////////////////////////////////////////////////////
    // Commented out. If uncommented then "Whatever" becomes overloaded
    // so "ClassHasNonOverloadedFunctionTraits_Whatever_v" called below
    // will then return false instead (since it only returns true for
    // *non-overloaded* functions)
    /////////////////////////////////////////////////////////////////////
    // int Whatever(char) const noexcept;
};

///////////////////////////////////////////////////////
// Call this macro to declare templates
// "ClassHasNonOverloadedFunctionTraits_Whatever" and
// its "ClassHasNonOverloadedFunctionTraits_Whatever_v"
// helper variable described previously
///////////////////////////////////////////////////////
DECLARE_CLASS_HAS_NON_OVERLOADED_FUNCTION_TRAITS(Whatever)

int main()
{
    // Lambda template (see comments below)
    using HasAllTraits = decltype([]<TRAITS_FUNCTION_C F,
                                     TRAITS_FUNCTION_C CompareF>() noexcept
                                  {
                                      return IsReturnTypeMatch_v<F, CompareF> &&
                                             CallingConvention_v<F> == CallingConvention_v<CompareF> &&
                                             IsArgTypesMatch_v<F, CompareF> &&
                                             IsFunctionConst_v<F> == IsFunctionConst_v<CompareF> &&
                                             IsFunctionVolatile_v<F> == IsFunctionVolatile_v<CompareF> &&
                                             FunctionReference_v<F> == FunctionReference_v<CompareF> &&
                                             IsNoexcept_v<F> == IsNoexcept_v<CompareF>;
                                  });

    ////////////////////////////////////////////////////////
    // Checks if class "Test" (the 1st template arg), has a
    // non-overloaded, non-static member function called
    // "Whatever". If so then the following template will
    // invoke the above lambda template which is passed as
    // the 2nd template arg in the following call. The
    // lambda template will be specialized on both the type
    // of "&Test::Whatever" (the lambda template's "F"
    // arg above), as well as the function we're passing
    // via the 3rd template arg in the following call (the
    // "UserTypeT" template arg of the following template
    // which we've named "CompareF" in the lambda template
    // above). The "F" template arg in the lambda template
    // will therefore resolve to the following in this
    // example (again, this is just "&Test::Whatever"):
    //
    //   int (Test::*)(std::string_view, float) const noexcept
    //
    // This matches the lambda template's "CompareF"
    // template arg (again, as passed via the 3rd arg in
    // the following call - we're passing the raw function
    // equivalent of "&Test::Whatever" here), so the lambda
    // will therefore return "true" (based on the search
    // criteria it's testing for above).
    ////////////////////////////////////////////////////////
    constexpr bool classHasNonOverloadedFunctionTraits_Whatever = ClassHasNonOverloadedFunctionTraits_Whatever_v<Test, // Class we wish to check for non-overloaded, non-static member function "Whatever"
                                                                                                                 HasAllTraits, // Lambda template above
                                                                                                                 int (std::string_view, float) const noexcept>; // "UserTypeT"
    return 0;
}
```
With the techniques described in this section now at your disposal, you can easily check if a *non-overloaded*, *non-static* member function exists based on any criteria you require. With the availability of lambda templates in C++20, you can also conveniently create the "*HasFunctionTraitsT*" template arg as a local lambda or even create it on-the-fly if you wish (as demonstrated).
</blockquote></details>

<a name="DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_TRAITS"></a><details><summary>DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_STATIC\_FUNCTION\_TRAITS</summary>

<blockquote>

```C++
#define DECLARE_CLASS_HAS_NON_OVERLOADED_STATIC_FUNCTION_TRAITS(NAME)
```
This macro is functionally equivalent to macro [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS](#declare_class_has_non_overloaded_function_traits) just above but targets *static* (public) member functions instead of *non-static*. See the latter macro for details. The following templates therefore won't detect functions unless they're *static* as well as non-overloaded (since both macros detect non-overloaded functions only). The macro creates the following two templates for this (assuming "*Whatever*" is passed to the macro):
```C++
template <IS_CLASS_C T,
          IS_CLASS_C HasFunctionTraitsT,
          typename UserTypeT = void>
REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
struct ClassHasNonOverloadedStaticFunctionTraits_Whatever : std::bool_constant<>
{
};

template <IS_CLASS_C T,
         IS_CLASS_C HasFunctionTraitsT,
         typename UserTypeT = void>
REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
inline constexpr bool ClassHasNonOverloadedStaticFunctionTraits_Whatever_v = ClassHasNonOverloadedStaticFunctionTraits_Whatever<T, HasFunctionTraitsT, UserTypeT>::value;
```
These templates are just the *static* counterparts of the templates targeting *non-static* functions created by [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS](#declare_class_has_non_overloaded_function_traits). Again see the latter macro for details. Please note however that unlike the "*HasFunctionTraitsT::operator()*" template described for the latter macro, the function passed via the "*F*" template arg of the latter operator will never contain the "*const*" and/or "*volatile*" and/or "*&*" or "*&&*" qualifiers, since "*F*" will always be a *static* member function which never includes such qualifiers (since they apply to *non-static* member functions only). You therefore never need to check for them when using DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_STATIC\_FUNCTION\_TRAITS (they will never be present in the "*F*" template arg your "*HasFunctionTraitsT::operator()*" member is specialized on).
</blockquote></details>

<a name="ClassHasOperator_FunctionCall"></a><details><summary>ClassHasOperator\_FunctionCall[^9]</summary>

<blockquote>

```C++
template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
struct ClassHasOperator_FunctionCall : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
inline constexpr bool ClassHasOperator_FunctionCall_v = ClassHasOperator_FunctionCall<T, F>::value;
```
These templates are functionally equivalent to the templates created by the [DECLARE\_CLASS\_HAS\_FUNCTION](#declare_class_has_function) macro but specifically target the function call operator "*operator()*" (so they detect if a class has a *non-static* "*operator()*" member matching the specific type you pass, whether overloaded or not). Unlike the latter macro however, no macro needs to be called to create these templates. They are already declared for you. Other than the names of these templates, they are identical to those created by the latter macro. See this for complete details.
</blockquote></details>

<a name="ClassHasStaticOperator_FunctionCall"></a><details><summary>ClassHasStaticOperator\_FunctionCall (C++23 or later)[^9]</summary>

<blockquote>

```C++
template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
struct ClassHasStaticOperator_FunctionCall : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
inline constexpr bool ClassHasStaticOperator_FunctionCall_v = ClassHasStaticOperator_FunctionCall<T, F>::value;
```
Available in C++23 or later only (since the function call operator was made optionally static in that version), these templates are functionally equivalent to the templates created by the [DECLARE\_CLASS\_HAS\_STATIC\_FUNCTION](#declare_class_has_static_function) macro but specifically target the [static function call operator](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1169r4.html) "*operator()*" (so they detect if a class has a *static* "*operator()*" member matching the specific type you pass, whether overloaded or not). Unlike the latter macro however, no macro needs to be called to create these templates. They are already declared for you. Other than the names of these templates, they are identical to those created by the latter macro. See this for complete details.
</blockquote></details>

<a name="ClassHasNonOverloadedOperator_FunctionCall"></a><details><summary>ClassHasNonOverloadedOperator\_FunctionCall[^9]</summary>

<blockquote>

```C++
template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
struct ClassHasNonOverloadedOperator_FunctionCall : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
inline constexpr bool ClassHasNonOverloadedOperator_FunctionCall_v = ClassHasNonOverloadedOperator_FunctionCall<T, F>::value;
```
These templates are functionally equivalent to the templates created by the [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function) macro but specifically target the function call operator "*operator()*" (so they detect if a class has a *non-overloaded*, *non-static* "*operator()*" member matching the specific type you pass). Unlike the latter macro however, no macro needs to be called to create these templates. They are already declared for you. Other than the names of these templates, they are identical to those created by the latter macro. See this for complete details.
</blockquote></details>

<a name="ClassHasNonOverloadedStaticOperator_FunctionCall"></a><details><summary>ClassHasNonOverloadedStaticOperator\_FunctionCall (C++23 or later)</summary>

<blockquote>

```C++
template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
struct ClassHasNonOverloadedStaticOperator_FunctionCall : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          TRAITS_FUNCTION_C F>
inline constexpr bool ClassHasNonOverloadedStaticOperator_FunctionCall_v = ClassHasNonOverloadedStaticOperator_FunctionCall<T, F>::value;
```
Available in C++23 or later only (since the function call operator was made optionally static in that version), these templates are functionally equivalent to the templates created by the [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_STATIC\_FUNCTION](#declare_class_has_non_overloaded_static_function) macro but specifically target the [static function call operator](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1169r4.html) "*operator()*" (so they detect if a class has a *non-overloaded*, *static* "*operator()*" member matching the specific type you pass). Unlike the latter macro however, no macro needs to be called to create these templates. They are already declared for you. Other than the names of these templates, they are identical to those created by the latter macro. See this for complete details.
</blockquote></details>

<a name="ClassHasNonOverloadedOperatorTraits_FunctionCall"></a><details><summary>ClassHasNonOverloadedOperatorTraits\_FunctionCall</summary>

<blockquote>

```C++
template <IS_CLASS_C T,
          IS_CLASS_C HasFunctionTraitsT,
          typename UserTypeT = void>
REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
struct ClassHasNonOverloadedOperatorTraits_FunctionCall : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          IS_CLASS_C HasFunctionTraitsT,
          typename UserTypeT = void>
REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
inline constexpr bool ClassHasNonOverloadedOperatorTraits_FunctionCall_v = ClassHasNonOverloadedOperatorTraits_FunctionCall<T, HasFunctionTraitsT, UserTypeT>::value;
```
These templates are functionally equivalent to the templates created by the [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION\_TRAITS](#declare_class_has_non_overloaded_function_traits) macro but specifically target the function call operator "*operator()*" (so they detect if a class has a *non-overloaded*, *non-static* "*operator()*" member with the specific traits you pass). Unlike the latter macro however, no macro needs to be called to create these templates. They are already declared for you. Other than the names of these templates, they are identical to those created by the latter macro. See this for complete details.
</blockquote></details>

<a name="ClassHasNonOverloadedStaticOperatorTraits_FunctionCall"></a><details><summary>ClassHasNonOverloadedStaticOperatorTraits\_FunctionCall (C++23 or later)</summary>

<blockquote>

```C++
template <IS_CLASS_C T,
          IS_CLASS_C HasFunctionTraitsT,
          typename UserTypeT = void>
REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
struct ClassHasNonOverloadedStaticOperatorTraits_FunctionCall : std::bool_constant<>
{
};

template <IS_CLASS_C T,
          IS_CLASS_C HasFunctionTraitsT,
          typename UserTypeT = void>
REQUIRES_IS_HAS_FUNCTION_TRAITS_C(HasFunctionTraitsT, UserTypeT)
inline constexpr bool ClassHasNonOverloadedStaticOperatorTraits_FunctionCall_v = ClassHasNonOverloadedStaticOperatorTraits_FunctionCall<T, HasFunctionTraitsT, UserTypeT>::value;
```
Available in C++23 or later only (since the function call operator was made optionally static in that version), these templates are functionally equivalent to the templates created by the [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_STATIC\_FUNCTION\_TRAITS](#declare_class_has_non_overloaded_static_function_traits) macro but specifically target the [static function call operator](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1169r4.html) "*operator()*" (so they detect if a class has a *non-overloaded*, *static* "*operator()*" member with the specific traits you pass). Unlike the latter macro however, no macro needs to be called to create these templates. They are already declared for you. Other than the names of these templates, they are identical to those created by the latter macro. See this for complete details.
</blockquote></details>

<a name="DisplayAllFunctionTraits"></a><details><summary>DisplayAllFunctionTraits</summary>

<blockquote>

```C++
template <TRAITS_FUNCTION_C F,
          typename CharT,
          typename CharTraitsT = std::char_traits<CharT>>
          std::basic_ostream<CharT, CharTraitsT>& DisplayAllFunctionTraits(std::basic_ostream<CharT, CharTraitsT> &stream);
```
Not a traits template (unlike most others), but a helper function template you can use to display all function traits for function "*F*" to the given "*stream*" (and the same "*stream*" reference you pass is also returned so the function supports "*chaining*" - you can immediately apply the "\<\<" operator to the return value if you need to stream additional data to "*stream*", such as linefeeds if you wish to display additional data after the displayed traits but on a new line). The traits are displayed in a user-friendly format seen in the example below (all traits in the library that apply to "*F*" are displayed). Note that the function is typically used in a debug or test environment if you wish to inspect all traits for a given function in a user-friendly (human readable) format. This can be useful for different purposes, such as when you simply want to test the library by dumping all traits for a given function at once, or perhaps you may have a function with a complicated declaration you're having trouble understanding (some can be notoriously complex in C++), since the function's output might make it easier to decipher. Note that the function's output is in English only (the labels seen in the example below are not localized based on the current [std::locale](https://en.cppreference.com/w/cpp/locale/locale)), and the function itself isn't designed to override the format seen below (with your own customized format). This was a design decision since the format will normally serve the needs of most users. If you require a different format then you'll need to roll your own but this would be very rare (though if ever required the implementation for this function in "*FunctionTraits.h*" can simply be copied and modified for your needs).

Note that the format of the displayed traits is best demonstrated using an example:
```C++
// Standard C++ headers
#include <iostream>
#include <string>

////////////////////////////////////////////////////////
// Only file in this repository you need to explicitly
// #include (see "Usage" section earlier)
////////////////////////////////////////////////////////
#include "FunctionTraits.h"

/////////////////////////////////////////////////////////
// Namespace with sample function whose traits you wish
// to display (all of them)
/////////////////////////////////////////////////////////
namespace Test
{
    class SomeClass
    {
    public:
        int DoSomething(const std::basic_string<wchar_t>&,
                        const char*,
                        short,
                        int,
                        float,
                        long,
                        double,
                        ...) const volatile && noexcept;
    };
}

int main()
{
    // Everything in the library declared in this namespace
    using namespace StdExt;

    // Above member function whose traits we wish to display
    using F = decltype(&Test::SomeClass::DoSomething);

    //////////////////////////////////////////////////
    // Display all traits for "F" to "tcout" (or pass
    // any other required stream). Note that "tcout"
    // isn't explicitly documented in this README.md
    // file (since it only documents templates
    // specifically associated with "FunctionTraits"
    // itself), but is nevertheless available from
    // the library's support file
    // "CompilerVersions.h" (so automatically
    // available when you #include "FunctionTraits.h"
    // itself). It always resolves to "std::cout" on
    // non-Microsoft platforms and on Microsoft
    // platforms, to "std::wcout" when compiling for
    // Unicode (normally the case on Windows), or
    // "std::cout" otherwise (very rare on Windows
    // anymore - applies only when compiling for ANSI
    // which has long been antiquated).
    //////////////////////////////////////////////////
    DisplayAllFunctionTraits<F>(tcout);

    return 0;
}
```
This will stream the traits for the non-static member function "*Test::SomeClass::DoSomething()*" above to "*tcout*" (described in the comments above) using the following format (note that the format may vary slightly depending on the function and the target compiler):

      1) Function: int (Test::SomeClass::*)(const std::basic_string<wchar_t>&, const char*, short int, int, float, long, double, ...) const volatile && noexcept
      2) Classification: Non-static member function
      3) Class: Test::SomeClass
      4) Calling convention: cdecl
      5) Return: int
      6) Arguments (7 + variadic):
              1) const std::basic_string<wchar_t>&
              2) const char*
              3) short
              4) int
              5) float
              6) long
              7) double
              8) ...
      7) const: true
      8) volatile: true
      9) Reference: &&
      10) noexcept: true

Each trait is sequentially numbered as seen (the numbers aren't relevant otherwise), and the "*Arguments*" section is also independently numbered but in argument order (so the numbers *are* relevant within that section - each indicates the 1-based "Ith" arg).

Note that for free functions (which includes static member functions), the "*Classification*" seen above (item 2) will indicate "*Free function*" instead of "*Non-static member function*" but the "*Class*" trait seen above (item 3) will normally be removed since it's available for *non-static* member functions only. To this end, note that even though (class) *static* member functions are class members, and therefore free functions (since they're static), the language itself incorporates no information tying them back to the class they originate from so the class itself can't be retrieved (and so the class type of static member functions isn't available via any trait in the library and therefore can't be displayed). In the rare case where a *static* functor is passed for template arg "*F*" itself however, i.e., [FunctionOrigin\_v](#functionorigin_v)\<F\> is "*FunctionOrigin::StaticFunctor*" (meaning "*F*" is a class with a static "*operator()*" member which was legalized in C++23), the "*Class*" *will* be displayed in this case (always displaying class "*F*" itself). Lastly, note that since free functions don't support the cv-qualifiers "*const*" and "*volatile*" nor the reference qualifiers "*&*" or "*&&*", these traits (items 7, 8 and 9 above) are also removed. Note that when displaying free functions, the numbers seen in the example above are simply renumbered to account for items that have been removed (as just described).

Finally, note that the information displayed in the example above will vary slightly depending on the function's classification and/or origin (see [FunctionClassification\_v](#functionclassification_v) and [FunctionOrigin\_v](#functionorigin_v) respectively). The precise format isn't documented here since the information is intended for display purposes only and the exact format therefore isn't normally relevant for most users (and it might change slightly in future releases as well, in particular if other traits become possible once reflection is available in C++26, and/or new function origins are supported, among other possible reasons).
</blockquote></details>

<a name="TypeName_v"></a><details><summary>TypeName\_v</summary>

<blockquote>

```C++
template <typename T>
inline constexpr tstring_view TypeName_v;
```
Not a template associated with "*FunctionTraits*" per se, but a helper template you can use to return the user-friendly name of any C++ type as a "*tstring\_view*" (more on this shortly). The user-friendly name is normally WYSIWYG, such as "*int*", "*float*", etc., unlike when invoking the native [typeid](https://en.cppreference.com/w/cpp/language/typeid) operator in C++, which returns a [std::type\_info](https://en.cppreference.com/w/cpp/types/type_info) object (reference) but its "*name*" member is typically a [mangled](https://en.wikipedia.org/wiki/Name_mangling) C++ name instead (so not user-friendly). Note that the exact format of the string returned by "*TypeName\_v*" is platform specific however. Because "*TypeName\_v*" relies on undocumented though normally reliable techniques to carry out its work (since C++ has no official way to retrieve the *user-friendly* string equivalent of a type name, though this might change with the arrival of reflection in C++26), the resulting string for a given type might not be identical on all platforms. A given compiler might add the prefix "*class*" to a class type for instance (or "*struct*" to a struct type), so a class called "*Test*" might be returned as "*class Test*" instead of just plain "*Test*". Moreover, when targeting the class type of a lambda (officially known as its "*closure type*"), which you'll normally do using [MemberFunctionClassName\_v](#memberfunctionclassname_v) (which invokes "*TypeName_v*" for you), the name of the class will normally include extraneous compiler-generated info, such as the name of the function the lambda was locally declared in in your code (if declared inside a function), or possibly even the source file name and line number where the lambda is declared (since the lambda's actual compiler-generated class is internal and this is just how a given compiler might return its name). Note that even for fundamental types, a given compiler might add the prefix "*signed*" to a signed integer type for instance, such as "*signed int*", but return it as simply "*int*" on another platform instead (perhaps if "*signed*" is the default for integers on that platform - if so then it might prepend "*unsigned*" to unsigned integers since "*unsigned*" is *not* the default). The upshot is that the exact format of a given type's string is platform specific so caution advised if this might affect you. The strings returned by "*TypeName\_v*" should normally be used for display purposes only (and there's no guarantee they'll be the same on different platforms).

To use this template simply pass the type you're interested in as the template's only template arg (to return its string equivalent). Note however that all helper aliases in the library such as [ArgType\_t](#argtype_t) have a corresponding helper "*Name*" template ([ArgTypeName\_v](#argtypename_v) in the latter case) that simply rely on "*TypeName\_v*" to return the type's user-friendly name (by simply passing the alias itself to "*TypeName\_v*"). You therefore don't have to call "*TypeName\_v*" directly for any of the type aliases in this library since a helper variable template already exists that does this for you (again, one for every alias template in the library, where the name of the variable template returning the type's name is the same as the name of the alias template itself but with the "*\_t*" suffix in the alias' name replaced with "*Name\_v*", e.g., [ArgType\_t](#argtype_t) and [ArgTypeName\_v](#argtypename_v)). You don't have to rely on these helper (name) variable templates however, you can manually (directly) pass the type to "*TypeName\_v*" instead if you wish (such as [ArgType\_t](#argtype_t)). Relying on the dedicated helper (name) variable templates is syntactically cleaner however (less verbose).<br/><br/>Note that "*TypeName\_v*" can be passed any C++ type however, not just types associated with "*FunctionTraits*". You can therefore use it for your own purposes whenever you need the user-friendly name of a C++ type as a compile-time string. Note that "*TypeName\_v*" returns a "*tstring\_view*" (in the "*StdExt*" namespace), which always resolves to "*std::string\_view*" on non-Microsoft platforms, and on Microsoft platforms, to "*std::wstring\_view*" when compiling for Unicode (usually the case - strings are normally stored in UTF-16 in modern-day Windows), or "*std::string\_view*" otherwise (when compiling for ANSI but this is very rare these days). Note that the returned "*tstring\_view*" is always guaranteed to be null-terminated[^10], so you can pass its "*data()*" member to a function that expects this for instance. Its "*size()*" member does not include the null-terminator however, as would normally be expected.
</blockquote></details>

<a name="ModuleUsage"></a>
## Module support in C++20 or later (experimental)

Note that you can also optionally use the module version of "*FunctionTraits*" if you're targeting C++20 or later (modules aren't available in C++ before that). Module support is still experimental however since C++ modules are still relatively new at this writing and not all compilers support them yet (or fully support them). GCC for instance, after correcting a compiler [bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=109679) that prevented module compilation, still fails to compile (a new issue has surfaced). The situation is still under review so for now module support in GCC is still unavailable. The Intel compiler may also fail to compile depending on the platform (informally confirmed by Intel [here](https://community.intel.com/t5/Intel-oneAPI-Data-Parallel-C/Moving-existing-code-with-modules-to-Intel-c/m-p/1550610/thread-id/3448)), even though its documentation states that modules are partially supported at this writing. Only Microsoft and Clang therefore support the module version of "*FunctionTraits*" for now though GCC and Intel may be fixed before too long (and this documentation will be updated as soon as they are).

In addition to supporting modules in C++20, the "*std*" library is also available as a module in C++23 (see the official document for this feature [here](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf)). This occurs in the form of "*import std*" and/or "*import std.compat*" statements (see latter link). Support for this is also very new and not available on most compilers yet (nor supported by CMake at this writing - see CMake [Limitations](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#limitations)). Microsoft recently started supporting it however as of Visual Studio 2022 V17.5 (see [here](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170#enable-modules-in-the-microsoft-c-compiler)), though this version doesn't yet support mixing #include statements from the "*std*" library and the "*import std*" statement in C++23. Until available you must therefore always exclusively rely on one or the other only (i.e., for now you can't mix #includes from the "*std*" library and the "*import std*" statement in C++23 - Microsoft is on record about this). Also note that not all supported compilers may #define the C++23 feature macro [\_\_cpp\_lib\_modules](https://en.cppreference.com/w/cpp/feature_test#cpp_lib_modules) yet, indicating that "*import std*" and "*import std.compat*" are available. Where they are available however they're still considered experimental so "*FunctionTraits*" won't rely on them without your explicit permission first (via the transitional *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* macro described shortly).

As a result of all these issues, module support for "*FunctionTraits*" is therefore considered experimental until all compilers fully support both C++ modules (a C++20 feature), and (though less critically), the importing of the standard library in C++23 (again, via "*import std*" and "*import std.compat*"). The instructions below therefore reflect the incomplete state of module support in all compilers at this writing, and are therefore (unavoidably) longer they should be (for now). Don't be discouraged however, as they're much simpler than they first appear.

Note that the following instructions will be condensed and simplified in a future release however, once all compilers are fully compliant with the C++20 and C++23 standard (in their support of modules). For now, because they're not fully compliant, the instructions below need to deal with the situation accordingly, such as the *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* constant in 4 below, which is a transitional constant only. It will be removed in a future release.

To use the module version of "FunctionTraits":

1. Ensure your project is set up to handle C++ modules if it's not already by default (again, since compiler support for modules is still evolving so isn't available out-of-the-box in some compilers). How to do this depends on the target compiler and your build environment (though C++20 or greater is always required), as each platform has its own unique way (such as the *-fmodules-ts* option in GCC - see GCC link just below). The details for each compiler are beyond the scope of this documentation but the following official (module) links can help get you started (though you'll likely need to do additional research if you're not already familiar with the process). Note that the "*CMake*" link below however does provide additional version details about GCC, Microsoft and Clang:
    1. [C++ modules (official specification)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1103r3.pdf)
    2. [GCC](https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Modules.html) (not currently supported by "*FunctionTraits*" however due to ongoing issues with GCC's support of modules - still a work-in-progress)
    3. [Microsoft](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170)
    4. [Clang](https://clang.llvm.org/docs/StandardCPlusPlusModules.html)
    5. [Intel](https://www.intel.com/content/www/us/en/developer/articles/technical/c20-features-supported-by-intel-cpp-compiler.html) (search page for [P1103R3](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1103r3.pdf) - no other Intel docs on modules can be found at this writing)
    6. [CMake](https://www.kitware.com/import-cmake-the-experiment-is-over/)
2. Add the primary module interface files "*FunctionTraits.cppm*" and "*CompilerVersions.cppm*" from this repository to your project, which builds the modules "*FunctionTraits*" and "*CompilerVersions*" respectively (corresponding to "*FunctionTraits.h*" and "*CompilerVersions.h*" described in the [Usage](#usage) section earlier - both ".h" files are still required however as each module simply defers to its ".h" file to implement the module). Ensure your build environment is set up to process these "*.cppm*" files as C++ modules if it doesn't handle it by default (based on the extension for instance). Consult the docs for your specific platform (such as changing the extension to "*.ixx*" on Microsoft platforms - more on this later). Note that you're free to change the "*.cppm*" extension to whatever you require, assuming "*.cppm*" doesn't suffice (again, more on this later). You can then import each module wherever you need it (read up on C++ modules for details), either using an "*import*" statement as would normally be expected (normally just "*import FunctionTraits*" - this is how modules are normally imported in C++), or by continuing to #include the header itself, normally just *#include "FunctionTraits.h"* (again, as described in the [Usage](#usage) section earlier). In the latter case (when you *#include "FunctionTraits.h"*), this will not only import "*FunctionTraits*" for you as described in 3 below, but also has the benefit of making all public macros in "*FunctionTraits.h*" available as well (should you require any of them), the reason you would choose to *#include "FunctionTraits.h*" in the module version instead of "*import FunctionTraits*" directly (more on this shortly).
3. #define the constant *STDEXT\_USE\_MODULES* when you build your project (add this to your project's build settings). Doing so changes the behavior of both "*FunctionTraits.h*" and "*CompilerVersions.h*" (again, "*FunctionTraits.h*" automatically #includes "*CompilerVersions.h*" - see [Usage](#usage) section), so that instead of declaring all C++ declarations as each header normally would (when *STDEXT\_USE\_MODULES* isn't #defined), each header simply imports the module instead (e.g., *#include "FunctionTraits.h"* simply issues an "*import FunctionTraits*" statement). All other C++ declarations in the file are then preprocessed out except for (public) macros, since they're not exported by C++ modules (so when required, the files that #define them must still be #included in the usual C++ way). Therefore, by #including "*FunctionTraits.h*", you're effectively just creating an "*import FunctionTraits*" statement (as well as "*import CompilerVersions*"), but also #defining all public macros in the header as well (including those in "*CompilerVersions.h*" - more on these macros shortly). All other declarations in the file are preprocessed out as noted (they're not needed because the "*import FunctionTraits*" statement itself makes them available). Note that if *STDEXT\_USE\_MODULES* isn't #defined however (though you normally should #define it), then each header is #included in the usual C++ way (no "*import*" statement will exist and all declarations in the header are declared normally), which effectively defeats the purpose of using modules (unless you manually code your own "*import FunctionTraits*" statement which can safely coexist with *#include "FunctionTraits.h"* if you use both but there's no reason to normally). Note that if you don't use any of the macros in "*FunctionTraits.h*" or "*CompilerVersions.h*" in your code however (again, more on these macros shortly), then you can simply apply your own "*import*" statement as usual, which is normally the natural way to do it (and #including the header instead is even arguably misleading since it will appear to the casual reader of your code that it's just a traditional header when it's actually applying an "*import*" statement instead, as just described). #including either ".h" file however to pick up the "*import*" statement instead of directly applying "*import*" yourself has the benefit of #defining all macros as well should you ever need any of them (though you're still free to directly import the module yourself at your discretion - redundant "*import*" statements are harmless if you directly code your own "*import FunctionTraits*" statement _**and**_ *#include "FunctionTraits.h"* as well, since the latter also applies its own "*import FunctionTraits*" statement as described). Note that macros aren't documented in this README file however since the focus of this documentation is on "*FunctionTraits*" itself (the subject of this GitHub repository). Both "*FunctionTraits.h*" and "*CompilerVersions.h*" contain various support macros however, such as the *TRAITS\_FUNCTION\_C* macro in "*FunctionTraits.h*" (see this in the [Helper templates](#helpertemplates) section earlier), and the "*Compiler Identification Macros*" and "*C++ Version Macros*" in "*CompilerVersions.h*" (a complete set of macros allowing you to determine which compiler and version of C++ is running - see these in "*CompilerVersions.h*" for details). "*FunctionTraits*" itself utilizes these macros for its own internal needs as required but end-users may wish to use them as well (for their own purposes). Lastly, note that as described in the [Usage](#usage) section earlier, #including "*FunctionTraits.h*" automatically picks up everything in "*CompilerVersions.h*" as well since the latter is a dependency, and this behavior is also extended to the module version (though if you directly "*import FunctionTraits*" instead of #include "*FunctionTraits.h*", it automatically imports module "*CompilerVersions*" as well but none of the macros in "*CompilersVersions.h*" will be available, again, since macros aren't exported by C++ modules - if you require any of the macros in "*CompilersVersions.h*" then you must *#include "FunctionTraits.h"* instead, or alternatively just *#include "CompilersVersions.h"* directly).
4. If targeting C++23 or later (the following constant is ignored otherwise), *and* the C++23 [import std](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) feature is supported by your compiler (read on), optionally #define the constant *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* when you build your project (add this to your project's build settings). This constant is transitional only as a temporary substitute for the C++23 feature macro [\_\_cpp\_lib\_modules](https://en.cppreference.com/w/cpp/feature_test#cpp_lib_modules) (used to indicate that "*import std*" and "*import std.compat*" are supported). If *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* is #defined (in C++23 or later), then the "*FunctionTraits*" library will use an [import std](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) statement to pick up all its internal dependencies from the "*std*" library instead of #including each individual "*std*" header it depends on. This is normally recommended in C++23 or later since it's much faster to rely on [import std](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) than to #include each required header from the "*std*" library (the days of doing so will likely become a thing of the past). Note that if you #define *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* then it's assumed that [import std](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) is in fact supported on your platform or cryptic compilers errors will likely occur (if targeting C++23 or later - again, the constant is ignored otherwise). Also note that as described earlier, if targeting Microsoft platforms then your own project must also currently rely on it everywhere since you can't (currently) mix headers from the "*std*" library and "*import std*" (until Microsoft corrects this). In any case, please see [Footnotes](#footnotes) for module versioning information for each supported compiler (which includes version support information for "*import std*" and "*import std.compat*"). Note that *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* will be removed in a future release however once all supported compilers fully support the [\_\_cpp\_lib\_modules](https://en.cppreference.com/w/cpp/feature_test#cpp_lib_modules) feature macro (which will then be used instead). For now the latter macro either isn't #defined on all supported platforms at this writing (most don't support "*import std*" and "*import std.compat*" yet), or if it is #defined such as in recent versions of MSVC, "*FunctionTraits*" doesn't rely on it yet (for the reasons described but see the comments preceding the check for *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* in "*FunctionTraits.h*" for complete details). Instead, if you wish for "*FunctionTraits*" to rely on [import std](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) then you must grant explicit permission by #defining *STDEXT\_IMPORT\_STD\_EXPERIMENTAL*. The C++ feature macro [\_\_cpp\_lib\_modules](https://en.cppreference.com/w/cpp/feature_test#cpp_lib_modules) itself is completely ignored by the "*FunctionTraits*" library in this release.

Finally, note that you're free to rename each "*.cppm*" extension to whatever you require on your particular platform. On Microsoft platforms for instance you can (and likely will) change it to "*.ixx*" (the default module extension for Microsoft), though you can optionally maintain the "*.cppm*" extension on Microsoft as well if you wish (or change it to anything else). If you do so however then in your Microsoft Visual Studio project you'll need to set each "*.cppm*" file's "*Compile As*" property to "*Compile as C++ Module Code (/interface)*" (you'll find this in the file's properties under "*Configuration Properties -> C/C++ -> Advanced*" - see the [/interface](https://learn.microsoft.com/en-us/cpp/build/reference/interface?view=msvc-170) command line switch in Microsoft's documentation for details). "*CMake*" developers may wish to consult the following Microsoft [issue](https://gitlab.kitware.com/cmake/cmake/-/issues/25643) however (as described in the latter link, Visual Studio solutions built with CMake don't add the [/interface](https://learn.microsoft.com/en-us/cpp/build/reference/interface?view=msvc-170) property as they normally should). In any case, note that each "*.cppm*" file in the "*FunctionTraits*" library is platform neutral so the extension isn't relevant, so long as your compiler is instructed to identify it as a module (if it doesn't recognize it by default). The C++ standard itself doesn't address the situation so while the extension "*.cppm*" has become widely adopted, there's no universal convention (and other extensions do exist).

<a name="WhyChooseThisLibrary"></a>
## Why choose this library
In a nutshell, because it's extremely easy to use, with syntax that's consistently very clean (when relying on [Technique 2 of 2](#technique2of2) as most normally will), it handles all mainstream function types you wish to pass as template args (see [here](#templateargf)), it has a very small footprint (once you ignore the many comments in "*FunctionTraits.h*"), and it may be the most complete function traits library available on the web at this writing (based on attempts to find an equivalent library with calling convention support in particular). The library effectively handles every mainstream trait supported by current versions of C++ (additional traits may be added once reflection is available in C++26), and should normally meet the requirements of most users looking for a function traits library (cleanly and reliably).

Note that for what it offers, "*FunctionTraits*" is also significantly leaner than the Boost version ("*boost::callable\_traits*"), which consists of a bloated (excessive) number of files and roughly twice the amount of code at this writing (largely due to a needlessly complex design, no disrespect intended). While "*FunctionTraits*" may initially appear large based on a cursory inspection of its header "*FunctionTraits.h*", well over half the lines in the file consist of comments only, and a significant amount of the code is also dedicated to features beyond what the Boost version offers, including extensive error handling to trap invalid template args (either using concepts in C++20 or later or "*static_asserts*" in C++17). The actual code required to implement "*FunctionTraits*" itself (the equivalent of what the Boost version offers), is actually relatively small, yet it still provides the same features as the Boost version for all intents and purposes, as well as additional features such as templates for retrieving the user-friendly name of any type (including the names of function arguments), iterating all argument types, modifying argument types, detecting the existence of member functions, etc. It also supports all (mainstream) calling conventions as emphasized earlier, and can therefore target functions regardless of calling convention, which isn't officially available in "*boost::callable\_traits*" (and even if activated there though it's not officially documented or supported, it has several other caveats as described earlier in this document).

Also note that the Boost implementation has several shortcomings that could be improved for a better experience. For instance, its "*args*" template (or "*args\_t_*" helper template), the template that developers typically use the most in practice (to access a function's argument types and count), is less user-friendly than it should be, requiring additional overhead to access the individual argument types and/or argument count (since "*args*" itself just returns the collection of argument types - no Boost-specific helper templates exist to drill into it leaving most users having to make additional calls to [std::tuple\_element](https://en.cppreference.com/w/cpp/utility/tuple_element) and/or [std::tuple\_size](https://en.cppreference.com/w/cpp/utility/tuple/tuple_size), or rely on their own wrappers to consolidate the calls). Additional helper templates to simplify the process would have been beneficial (though the code does seem to have at least one internal template the author may have been experimenting with towards this goal but the situation is murky). Moreover, note that "*args*" also has a few strange quirks, such as including the function's member class type itself when handling non-static member functions (always at index zero in "*args*"), even though the class type has nothing to do with a function's arguments (so it's presence in "*args*" is strange). It also means the function's first argument type is at index 1 in "*args*" for non-static member functions but index zero for free functions, which forces developers to have to subtract 1 from [std::tuple\_size](https://en.cppreference.com/w/cpp/utility/tuple/tuple_size) to retrieve the number of arguments for non-static member functions, but not for free functions. This needlessly complicates things for any code that may be handling both non-static member functions and free functions (when retrieving the number of arguments or for any other purpose). The behavior also overlaps with the purpose of template "*qualified\_class\_of*", whose main purpose is to return a non-static member function's class type (the same type that template "*args*" stores at index zero), and even template "*class\_of*", rendering all these templates somewhat redundant and a bit confusing (and "*args*" also confusingly handles member *data* pointer types as well, which really has nothing to do with its intended purpose of handling function arguments). Note that by contrast, "*FunctionTraits*" provides clean and easy-to-use helper templates out-of-the-box (no need to explicitly call [std::tuple\_element](https://en.cppreference.com/w/cpp/utility/tuple_element) or [std::tuple\_size](https://en.cppreference.com/w/cpp/utility/tuple/tuple_size)), and no such oddities or redundant behavior (or behavior unrelated to the task of handling functions only).

Lastly, note that "*boost::callable\_traits*" does support the experimental "*transaction\_safe*" keyword, but "*FunctionTraits*" doesn't by design. Since this keyword isn't in the official C++ standard (most have never likely heard of it), and it's questionable if it ever will be (it was first floated in 2015), I've deferred its inclusion until it's actually implemented, if ever. Very few users will be impacted by its absence and including it in "*FunctionTraits*" can likely be done very quickly based on my review of the situation.

The upshot is that "*FunctionTraits*" is more complete and robust than all other similar libraries (and easier to use than Boost at times), primarily due to both its (mainstream) calling convention support (so it can detect any function regardless of calling convention), as well as the additional features it offers that aren't available in any other similar library (some of which aren't viable in those libraries based on their current design, due to lack of calling convention support or for other reasons). Very little could be added at this stage that would benefit most users and would usually require improvements to the C++ standard itself to accommodate (again, such features may be attainable once reflection becomes available in C++26).

<a name="DesignConsiderations"></a>
## "FunctionTraits" design considerations (for those who care - not required reading)
Note that "*FunctionTraits*" is not a canonical C++ "traits" class that would likely be considered for inclusion in the C++ standard itself. While it could be with some fairly easy-to-implement changes and (rarely-used) additions (read on), it wasn't designed for this. It was designed for practical, real-world use instead. For instance, it supports most mainstream calling conventions as emphasized earlier which isn't currently addressed in the C++ standard itself. Also note that template arg "*F*" of "*FunctionTraits*" and its helper templates isn't confined to pure C++ function types. As described earlier in this document, you can also pass pointers to functions, references to functions, references to pointers to functions, functors and [std::function](https://en.cppreference.com/w/cpp/utility/functional/function) specializations (in all cases referring to their actual C++ types, not any actual instances). Note that references to non-static member function pointers aren't supported however because they're not supported by C++ itself.

While I'm not a member of the C++ committee, it seems very likely that a "canonical" implementation for inclusion in the standard itself likely wouldn't (or might not) address certain features, like calling conventions since they don't currently exist in the C++ standard as described earlier (unless it's ever added at some point). It's also unlikely to support all the variations of pointers and reference types (and/or possibly functors) that "*FunctionTraits*" handles. In the real world however function types include calling conventions so they need to be detected by a function traits library (not just the default calling convention), and programmers are also often working with raw function types or pointers to functions or functors (or references to functions or references to pointers to functions though these are typically encountered less frequently). For non-static member functions in particular, pointers to member functions are the norm. In fact, for non-static member functions, almost nobody in the real world ever works with their actual (non-pointer) function type. I've been programming in C++ for decades and don't recall a single time I've ever worked with the actual type of one. You work with pointers to non-static member functions instead when you need to, such as ```void (YourClass::*)()```. Even for free functions (which for our purposes also includes static member functions), since the name of a free function decays into a pointer to the function in most expressions, you often find yourself working with a pointer to such functions, not the actual function type (though you do work with the actual function type sometimes, unlike for non-static member functions which is extremely rare in the real world). Supporting pointers and references to functions and even references to pointers to functions therefore just makes things easier even if some consider it unclean (I don't). A practical traits struct should just make it very easy to use without having to manually remove things using "*std::remove\_pointer*" and/or "*std::remove\_reference*" first (which would be required if such a traits struct was designed to handle pure C++ function types only, not pointers and references to function types or even references to pointers). It's just easier and more flexible to use it this way (syntactically cleaner). Some would argue it's not a clean approach but I disagree. It may not be consistent with how things are normally done in the standard itself (again, I suspect it might handle raw function types only), but it better deals with the real-world needs of most developers IMHO (so it's "clean" because its syntax is designed to cleanly support that).

<a name="Footnotes"></a>
## Footnotes
[^1]: **_GCC minimum required version:_**
    1. Non-module (*\*.h*) version of FunctionTraits: GCC V10.2 or later
    2. Module (*\*.cppm*) version of FunctionTraits (see [Module support in C++20 or later](#moduleusage)): Not currently compiling at this writing due to ongoing issues in GCC's support of modules (still a work-in-progress so unavailable for now)

    Note that GCC compatible compilers are also supported based on the presence of the #defined constant \_\_GNUC\_\_

[^2]: **_Microsoft Visual C++ minimum required version:_**
    1. Non-module (*\*.h*) version of FunctionTraits: Microsoft Visual C++ V19.16 or later for [Read traits](#readtraits) (normally installed with Visual Studio 2017 V15.9 or later), or V19.20 or later for [Write traits](#writetraits) (normally installed with Visual Studio 2019 or later). Note that [Write traits](#writetraits) are unavailable in Visual Studio 2017 releases of VC++ due to compiler bugs in those versions.
    2. Module (*\*.cppm*) version of FunctionTraits (see [Module support in C++20 or later](#moduleusage)): Microsoft Visual C++ V19.31 or later (normally installed with Visual Studio 2022 V17.1 or later - see [here](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170#enable-modules-in-the-microsoft-c-compiler)). Note that if using CMake then it has its own requirements (toolset 14.34 or later provided with Visual Studio 2022 V17.4 or later - see [here](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#compiler-support)).

    Note that [import std](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) in C++23 is supported by Microsoft Visual C++ V19.35 or later (normally installed with Visual Studio 2022 V17.5 or later - see [here](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170#enable-modules-in-the-microsoft-c-compiler)), so the transitional *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* macro described in [Module support in C++20 or later](#moduleusage) is ignored in earlier versions

[^3]: **_Clang minimum required version:_**
    1. Non-module (*\*.h*) version of FunctionTraits: Clang V11.0 or later. Note that depending on the "*FunctionTraits*" feature used, early versions of Clang targeting C++20 may produce erroneous warnings that were corrected by Clang in later versions ("*FunctionTraits*" should produce no warnings and normally doesn't, but some might occur in C++20 due to this issue). Testing shows that Clang V14.0 or later seems to be the minimum version for eliminating all known C++20 warnings (related to this issue).
    2. Module (*\*.cppm*) version of FunctionTraits (see [Module support in C++20 or later](#moduleusage)): Clang V16.0 or later

    Note that [Microsoft Visual C++ compatibility mode](https://clang.llvm.org/docs/MSVCCompatibility.html) is also supported. Please note that [import std](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) in C++23 is not supported by Clang yet (at this writing), so the *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* macro described in [Module support in C++20 or later](#moduleusage) shouldn't be #defined until it is (until then a compiler error will occur if targeting C++23 or later).

[^4]: **_Intel oneAPI DPC++/C++ minimum required version:_**
    1. Non-module (*\*.h*) version of FunctionTraits: Intel oneAPI DPC++/C++ V2021.4.0 or later
    2. Module (*\*.cppm*) version of FunctionTraits (see [Module support in C++20 or later](#moduleusage)): Listed as partially supported by Intel [here](https://www.intel.com/content/www/us/en/developer/articles/technical/c20-features-supported-by-intel-cpp-compiler.html) (search for "*Modules: Merging Modules*"), but not yet working for unknown reasons (informally confirmed by Intel [here](https://community.intel.com/t5/Intel-oneAPI-Data-Parallel-C/Moving-existing-code-with-modules-to-Intel-c/m-p/1550610/thread-id/3448)). Note that even when it is finally working, if using CMake then note that it doesn't natively support module dependency scanning for Intel at this writing (see CMake [Compiler Support](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html#compiler-support) for modules).

    Note that [Microsoft Visual C++ compatibility mode](https://www.intel.com/content/www/us/en/docs/dpcpp-cpp-compiler/developer-guide-reference/2024-0/microsoft-compatibility.html) is also supported. Please note that [import std](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) in C++23 is not supported by Intel yet (at this writing), so the *STDEXT\_IMPORT\_STD\_EXPERIMENTAL* macro described in [Module support in C++20 or later](#moduleusage) shouldn't be #defined until it is (until then a compiler error will occur if targeting C++23 or later).

[^5]: **_GCC and Clang compiler bugs affecting the member function detection templates:_**

    Note that when using the library's member function detection templates (see [here](#determiningifamemberfunctionexists)), both Clang and GCC have compiler bugs at this writing that can potentially occur when targeting non-public functions:

    1. *GCC*: Compilation in Clang may fail if you attempt to detect a function that exists and is declared "*private*". The issue exists due to the following [GCC bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=116639).
    2. *Clang*: Two known issues exist:
        1. Compilation in GCC may fail if you attempt to detect a function that exists and is declared "*protected*" (but strangely works if *private*). The issue exists due to the following [Clang bug](https://github.com/llvm/llvm-project/issues/103895).
        2. The library's member function detection templates might incorrectly return "*true*" if you attempt to detect a *private*, *overloaded* base class function via a derived class (and the function exists). The issue occurs due to the following [Clang bug](https://github.com/llvm/llvm-project/issues/107629).

    Note that the extent of these bugs remain unknown so they might potentially manifest themselves in other similar non-public function scenarios until corrected by GCC and Clang. Users of the member function detection templates should be aware of the situation.

[^6]: **_Microsoft design flaw affecting the member function detection templates:_**

    Due to a (flawed) design issue with how Microsoft handles the default calling convention when compiling for x86 only (the issue doesn't exist when compiling for other architectures, most notably x64, or when using non-Microsoft compilers), then the templates created by both the [DECLARE\_HAS\_FUNCTION](#declare_has_function) and [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function) macros might erroneously return the wrong ("*true*" or "*false*") value if ***both*** of the following hold for template arg "*F*" of these templates:

    1. Template arg "*F"* is a [raw](#rawfunctiontypes) (native) C++ function type (i.e., it satisfies [std::is\_function](https://en.cppreference.com/w/cpp/types/is_function) - see [C++ function types at a glance](#cppfunctiontypesataglance)). If a *non-static* member function pointer instead then no issue occurs.
    2. Template arg "*F"* includes an *explicitly* declared calling convention that's set to the compiler default (via the *\_\_cdecl (/Gd)*, *\_\_fastcall (/Gr)*, *\_\_stdcall (/Gz)* or *\_\_vectorcall (/Gv)* compiler options available in Visual Studio under your project's properties - see "*Configuration Properties -> C/C++ -> Advanced -> Calling Convention*" - defaults to *\_\_cdecl (/Gd)* as described [here](https://learn.microsoft.com/en-us/cpp/build/reference/gd-gr-gv-gz-calling-convention?view=msvc-170)). If the calling convention is *implicitly* included however (i.e., none is explicitly set in template arg "*F*", which is usually the case in practice), then no issue occurs.

    The details are beyond the scope of this documentation but the upshot is that if template arg "*F*" is a [raw](#rawfunctiontypes) (native) C++ function type (opposed to a *non-static* member function pointer type which has no such issue) *AND* you explicitly set the calling convention in template arg "*F*" (and specifically to the default calling convention in effect via the [/Gd, /Gr, /Gv or /Gz](https://learn.microsoft.com/en-us/cpp/build/reference/gd-gr-gv-gz-calling-convention?view=msvc-170) Microsoft compiler options), then these specific templates might erroneously return "*true*" when "*false*" should be returned or vice versa (though the condition under which it occurs will usually be rare in practice). If you wish to completely avoid the situation, then either always pass a *non-static* member function pointer type for "*F*" instead of a [raw](#rawfunctiontypes) native function type (see [C++ function types at a glance](#cppfunctiontypesataglance) for details on [raw](#rawfunctiontypes) function types), or don't explicitly include the *default* calling convention in template arg "*F*" (if you wish to pass a [raw](#rawfunctiontypes) C++ function type for "*F*" instead of a non-static member function pointer). In either case the issue is always avoided.

[^7]: **_LAMBDA\_TEMPLATES\_SUPPORTED macro_**

    This #defined macro (constant), which is automatically available when you #include the library's main header "*FunctionTraits.h*" (though it's actually #defined in "*CompilerVersions.h*" which "*FunctionTraits.h*" depends on), can be used to determine if lambda templates are supported. Note that lambda templates became available in C++20 and some of the library's features optionally allow you use them for certain purposes (to make it easier to work with those features). To determine if lambda templates are supported in the version of C++ you're using, you can simply check if LAMBDA\_TEMPLATES\_SUPPORTED is #defined. While it would normally suffice to simply check for C++20 or later, which can be done via a call to ``` #if CPP20_OR_LATER ``` or any other technique you may rely on (the latter macro however is part of a family of C++ version constants available in "*CompilerVersions.h*" - again, they're automatically #included by "*FunctionTraits.h*"), some compilers may not support lambda templates in early versions of those compilers even when targeting C++20 (or they may contain bugs in those early versions). Note that there is no official way to check for lambda templates in the language itself but for all intents and purposes the quasi-official (most reliable) way is to check for both ``` #if CPP20_OR_LATER ``` *AND* the official C++ feature macro [\_\_cpp\_generic\_lambdas](https://en.cppreference.com/w/cpp/feature_test#cpp_generic_lambdas) for a value >= 201707, which LAMBDA\_TEMPLATES\_SUPPORTED does. It's therefore safer to rely on this macro (constant) to check if lambda templates are supported rather than simply check for C++20 or later (unless you're always targeting a reasonably up-to-date version of C++20, rendering the additional check for [\_\_cpp\_generic\_lambdas](https://en.cppreference.com/w/cpp/feature_test#cpp_generic_lambdas) redundant - note that GCC supports lambda templates in all of its C++20 releases so there are no known issues, but for Clang a call to ``` #if CPP20_OR_LATER ``` is only considered reliable in version 14.0 or later (for checking if lambda templates exist), and for Microsoft it's reliable for V19.27 or later, which was released with Visual Studio V16.7 - LAMBDA_TEMPLATES_SUPPORTED should be relied on vs ``` #if CPP20_OR_LATER ``` otherwise but it's safer and more clear to just always rely on it).

[^8]: **_Removing write traits if not required (optional):_**

    Note that most users don't use [Write traits](#writetraits) (most use [Read traits](#readtraits) only), so those who wish to remove them can optionally do so by #defining the preprocessor constant *REMOVE\_FUNCTION\_WRITE\_TRAITS* (but read on for an important caveat if you do). Doing so will preprocess out all [Write traits](#writetraits) so they no longer exist, and therefore eliminate the overhead of compiling them (though the overhead is normally negligible). Please note however that some of the library's function detection templates (see footnote [^9] just below) depend on write traits to carry out their work (by necessity), so *REMOVE\_FUNCTION\_WRITE\_TRAITS* should not be #defined if you rely on them. Compiler errors will occur otherwise (since the required write traits will have been preprocessed out). Lastly, please note that [Write traits](#writetraits) are *always* unavailable in Microsoft Visual Studio 2017 releases of VC++ regardless of this constant due to compiler bugs in those versions (also noted in footnote [^2] above).

[^9]: **_Function detection templates that depend on write traits:_**

    Note that some of the library's function detection templates, namely those created by the following macros:

    1. [DECLARE\_CLASS\_HAS\_FUNCTION](#declare_class_has_function)
    2. [DECLARE\_CLASS\_HAS\_STATIC\_FUNCTION](#declare_class_has_static_function)
    3. [DECLARE\_CLASS\_HAS\_NON\_OVERLOADED\_FUNCTION](#declare_class_has_non_overloaded_function)

    As well as the following templates:

    1. [ClassHasOperator\_FunctionCall](#classhasoperator_functioncall)
    2. [ClassHasStaticOperator\_FunctionCall](#classhasstaticoperator_functioncall) (C++23 or later)
    3. [ClassHasNonOverloadOperator\_FunctionCall](#classhasnonoverloadedoperator_functioncall)

     ... depend on the library's [Write traits](#writetraits) to do their job (by necessity), so these templates won't be available if you #define *REMOVE\_FUNCTION\_WRITE\_TRAITS* to remove the library's write traits (they will be completely preprocessed out of the library). See footnote [^8] just above for details. Note that as described in the latter footnote, [Write traits](#writetraits) are *always* unavailable in Microsoft Visual Studio 2017 releases of VC++ due to bugs in those versions, so the above function detection templates are *always* unavailable in Microsoft Visual Studio 2017 for this reason.

[^10]: **_"TypeName\_v" null termination:_**

    Note that the string returned by [TypeName\_v](#typename_v) is guaranteed to be null-terminated only if the internal constant *TYPENAME\_V\_DONT\_MINIMIZE\_REQD\_SPACE* isn't #defined. It's not #defined by default so null-termination is guaranteed by default. See this constant in "*FunctionTraits.h*" for full details (not documented in this README file since few users will ever need to #define it).
