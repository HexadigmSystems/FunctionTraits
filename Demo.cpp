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
// Demo program showing how to use template "FunctionTraits" declared in
// "TypeTraits.h" (used to retrieve information about any function including
// its return type, argument types, and other information). Just run the
// program to display all available information about the 3 sample functions
// seen in namespace "TestFuncs" further below (traits displayed to the stdout).
// You can update these three functions with any signature whose traits you
// want to display, and re-run the app. No other changes are required.
/////////////////////////////////////////////////////////////////////////////

// #included first so we can check CPP17_OR_LATER just below
#include "CompilerVersions.h"

// We only support C++17 or later (stop compiling otherwise)
#if CPP17_OR_LATER

// Standard C/C++ headers
#include <cstddef>
#include <iostream>
#include <string_view>

// Our headers
#include "TypeTraits.h"

////////////////////////////////////////
// Everything in our own headers just
// above is declared in this namespace
////////////////////////////////////////
using namespace StdExt;

///////////////////////////////////////////////////////////////////////////////
// OutputArgI(). Displays the "Ith" arg to the stdout (formatted as required
// for this app). Note that "I" refers to the (zero-based) "Ith" arg in the
// function we're being called for (in left-to-right order) and "argTypeAsStr"
// is its type (converted to a WYSIWYG string)
///////////////////////////////////////////////////////////////////////////////
void OutputArgI(const std::size_t i, // "Ith" arg (zero-based)
                std::basic_string_view<TCHAR> argTypeAsStr) // Type of the "Ith" arg just above (as a string)
{
    tcout << _T("\t") <<
             (i + 1) << // "Ith" arg ("i" is zero-based so we add 1 to display
                        // it as 1-based - more natural for display purposes)
             _T(") ") <<
             argTypeAsStr <<
             _T("\n");
}

/////////////////////////////////////////////////////////////////////////////
// OutputArgI(). Converts "ArgTypeT" template arg to a "pretty" (WYSIWYG)
// string and pass it to above (non-template) overload where it's displayed
// to the "stdout" (formatted as required for this app). Note that "I" refers
// to the (zero-based) "Ith" arg in the function we're being called for (in
// left-to-right order) and "ArgTypeT" is its type.
/////////////////////////////////////////////////////////////////////////////
template <std::size_t I, typename ArgTypeT>
void OutputArgI() // "Ith" arg (zero-based)
{
    ////////////////////////////////////////////
    // Convert "ArgTypeT" to a string and pass
    // it to the overload just above
    ////////////////////////////////////////////
    OutputArgI(I, TypeName_v<ArgTypeT>);
}

///////////////////////////////////////////////////////////////////////////////
// C++17? Note that we don't support earlier versions and an #error directive
// would have already kicked in if now compiling for versions prior to C++17
// (see check for CPP17_OR_LATER earlier). For C++20 or later however we
// replace "class DisplayArgType" just below with its lambda template
// equivalent at point of use later on. Both just invoke "OutputArgI()" above
// so are identical but the lambda is more convenient.
///////////////////////////////////////////////////////////////////////////////
 #if CPP17
    class DisplayArgType
    {
    public:
        ///////////////////////////////////////////////////
        // Functor we'll be invoking once for each arg in
        // the function whose traits we're displaying
        // (where "I" is the zero-based number of the arg,
        // and "ArgTypeT" is its type). Following is a
        // template so it effectively gets stamped out
        // once for each argument in the target function.
        // See call to "ForEachArg()" later on (which
        // processes this class in C++ 17 only).
        ///////////////////////////////////////////////////
        template <std::size_t I, typename ArgTypeT>
        bool operator()() const
        {
            // Defer to this function (template)
            OutputArgI<I, ArgTypeT>();

            //////////////////////////////////////////////
            // Return true to continue iterating so this
            // function gets called again with "I + 1"
            // (until all function args are processed).
            // Note that returning false would stop
            // iterating, equivalent to a "break"
            // statement in a regular"for" loop.
            //////////////////////////////////////////////
            return true;
        }
    };
#endif

///////////////////////////////////////////////////////////////////////////
// "DisplayArgTypes()". Displays all arguments for function "F" to the
// stdout as per the following example:
//
// If "F" is a function of the following type:
//
//       int (const std::string &, const char *, short, ...) noexcept;
//
// Then displays (note that actual strings below are compiler-dependent but
// are usually identical or very similar):
//
//    	 1) const std::basic_string<char> &
//       2) const char *
//	     3) short
//	     4) ...
///////////////////////////////////////////////////////////////////////////
template <typename F>
void DisplayArgTypes()
{    
    // Lamba templates available starting in C++20
    #if CPP20_OR_LATER
        ///////////////////////////////////////////////////
        // Lambda we'll be invoking once for each arg in
        // the function whose traits we're displaying
        // (where "I" is the zero-based number of the arg,
        // and "ArgTypeT" is its type). Following is a
        // template so it effectively gets stamped out
        // once for each argument in function "F".
        ///////////////////////////////////////////////////
        const auto displayArgType = []<std::size_t I, typename ArgTypeT>()
                                    {
                                        // Defer to this function (template)
                                        OutputArgI<I, ArgTypeT>();

                                        //////////////////////////////////////////////
                                        // Return true to continue iterating so this
                                        // function gets called again with "I + 1"
                                        // (until all function args are processed).
                                        // Note that returning false would stop
                                        // iterating, equivalent to a "break"
                                        // statement in a regular"for" loop.
                                        //////////////////////////////////////////////
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
        const DisplayArgType displayArgType;
    #endif

    ////////////////////////////////////////////////////////
    // Invoke the above functor ("displayArgType") once
    // for each arg in the function whose traits we're
    // processing ("F"). Note that the following generic
    // function (template) "ForEachArg()" will effectively
    // stamp out one copy of member function (template)
    // "operator()<I, ArgTypeT>" in "displayArgType" above
    // (for each argument in function "F" we're now
    // processing). IOW, we wind up with "N" versions of
    // "operator()" for "N" in the range zero to the number
    // of args in function "F" minus 1 (where the template
    // args of each "operator()" reflect the "Ith" arg in
    // function "F")
    ////////////////////////////////////////////////////////
    ForEachArg<F>(displayArgType);

    // Is function variadic? (i.e., last arg is "...")
    if (IsVariadic_v<F>)
    {
        OutputArgI(ArgCount_v<F>, _T("..."));
    }
}

///////////////////////////////////////////////////////////////////////////
// "DisplayFunctionTraits()" overload (1 of 2). Dumps all the traits data
// for template arg "F" to the stdout (in the format seen when you run
// this app). Note that "F" is the function's type. Any legal function
// signature will do for "F" including pointers and references to
// functions (note that references to non-static member functions aren't
// supported in C++ however), references to pointers to functions, and
// functors (for functors "F" will be the functor's class name noting that
// overloads of "operator()" aren't supported since it will cause
// ambiguity errors - you can always pass the address of the specfic
// overload however as a normal member' function pointer)
///////////////////////////////////////////////////////////////////////////
template <typename F>
void DisplayFunctionTraits()
{
    /////////////////////////////////////////////////////////////
    // Lambda for displaying the "#)" seen at the start of each
    // output line (where "#" is the argument # starting at 1)
    // but not the "#)" seen for each arg in the "Arguments"
    // section of the output, which are handled elsewhere (see
    // "DisplayArgTypes()" above)
    /////////////////////////////////////////////////////////////
    auto tcoutNumBracket = [num = 1]() mutable -> auto&
                           {
                               return tcout << num++ << _T(") ");
                           };

    ///////////////////////////////
    // #) Type
    ///////////////////////////////
    tcoutNumBracket() << _T("Type: ") << FunctionTypeName_v<F> << _T("\n");

    ///////////////////////////////
    // #) Classification
    ///////////////////////////////
    tcoutNumBracket() << _T("Classification: ");

    // Non-static member function? (includes functors)
    if (IsMemberFunction_v<F>)
    {
        tcout << (IsFunctor_v<F>? _T("Functor") : _T("Non-static member"));
        tcout << _T(" (class/struct=\"") << MemberFunctionClassName_v<F> << _T("\")\n");
    }
    // Free function (which includes static member functions)
    else
    {
        tcout << _T("Free or static member\n");
    }

    ///////////////////////////////
    // #) Calling convention
    ///////////////////////////////
    tcoutNumBracket() << _T("Calling convention: ") << CallingConventionName_v<F> << _T("\n");

    ///////////////////////////////
    // #) Return
    ///////////////////////////////
    tcoutNumBracket() << _T("Return: ") << ReturnTypeName_v<F> << _T("\n");

    ///////////////////////////////
    // #) Arguments
    ///////////////////////////////
    tcoutNumBracket() << _T("Arguments (") <<
                         ArgCount_v<F> <<
                         (IsVariadic_v<F> ? _T(" + variadic") : _T("")) <<
                         _T("):");

    ///////////////////////////////////////////////////
    // Function's arg list not "(void)" (so has at
    // least 1 (non-variadic) arg and/or is variadic)
    ///////////////////////////////////////////////////
    if (ArgCount_v<F> != 0 || IsVariadic_v<F>)
    {
        tcout << _T("\n");

        DisplayArgTypes<F>();
    }
    else
    {
        ////////////////////////////////////////
        // Function's arg list is "(void)" (it
        // has no args and is not variadic)
        ////////////////////////////////////////
        tcout << _T(" None\n");
    }

    ///////////////////////////////////////////////////
    // Is a non-static member function which includes
    // functors. If so then display traits applicable
    // to those only (i.e., cv and ref qualifiers
    // which aren't applicable to free functions or
    // static member functions)
    ///////////////////////////////////////////////////
    if (IsMemberFunction_v<F>)
    {
        ///////////////////////////////
        // #) const
        ///////////////////////////////
        tcoutNumBracket() << _T("const: ") << std::boolalpha << IsMemberFunctionConst_v<F> << _T("\n");

        ///////////////////////////////
        // #) volatile
        ///////////////////////////////
        tcoutNumBracket() << _T("volatile: ") << std::boolalpha << IsMemberFunctionVolatile_v<F> << _T("\n");

        ///////////////////////////////
        // #) Ref-qualifier
        ///////////////////////////////
        tcoutNumBracket() << _T("Ref-qualifier: ") << MemberFunctionRefQualifierName_v<F> << _T("\n");
    }

    ///////////////////////////////
    // #) noexcept
    ///////////////////////////////
    tcoutNumBracket() << _T("noexcept: ") << std::boolalpha << IsNoexcept_v<F> << _T("\n");
}

/////////////////////////////////////////////////////////////////////////////
// "DisplayFunctionTraits()" overload (2 of 2)
/////////////////////////////////////////////////////////////////////////////
template <typename F>
void DisplayFunctionTraits(std::basic_string_view<TCHAR> caption)
{
    // Display the caption for this function to the stdout
    tcout << _T("************************************\n");
    tcout << _T("* ") << caption << _T("\n");
    tcout << _T("************************************\n");

    ///////////////////////////////////////////
    // Defer to overload 1 of 2 just above to
    // display the function's traits to the
    // stdout
    ///////////////////////////////////////////
    DisplayFunctionTraits<F>();

    tcout << _T("\n");
}

//////////////////////////////////////////////////////////////////////////////
// namespace TestFuncs. Contains 3 functions whose traits this demo displays.
// Functions need not be defined in any namespace however (for this demo to
// work). Doing so just to keep things organized (and placed in this
// namespace for easy access - you can quickly change all functions in this
// one location). Note that you can experiment with this program by simply
// changing the signatures of these functions to whatever you wish and
// re-running the app (but don't change the function names themselves unless
// you also change them where they're actually used later on, though there's
// normally no need to - the function signature is all that matters, not the
// name)
//////////////////////////////////////////////////////////////////////////////
namespace TestFuncs
{
    //////////////////////////////////////////////////////
    // Free function. Note: Need not be defined for this
    // demo to work and it's not (only the function's
    // type matters)
    //////////////////////////////////////////////////////
    int FreeFunc(const std::wstring &, const char *, short, int, float, long, double, ...) noexcept;

    class SomeClass
    {
    public:
        //////////////////////////////////////////////////////
        // Non-static member function (static functions also
        // supported but processed like a free function so
        // the one above is used). Note: Need not be defined
        // for this demo to work and it's not (only the
        // function's type matters)
        //////////////////////////////////////////////////////
        int STDEXT_CC_STDCALL DoSomething(double &, const std::string_view &) const volatile && noexcept;

        ///////////////////////////////////////////////////
        // Adding this to make the class a functor (which
        // are also supported)
        ///////////////////////////////////////////////////        
        std::basic_string<TCHAR> operator()(std::size_t) const;
    };
}

//////////////////////////////////////////////////////////////////////////////
// DisplayFreeFunctionTraits()
//////////////////////////////////////////////////////////////////////////////
void DisplayFreeFunctionTraits()
{
    ////////////////////////////////////////////////////////////
    // Free function type or static member function type whose
    // traits we wish to display (non-static member functions
    // are handled by "DisplayMemberFunctionTraits()" below).
    // Note that pointers or reference to the function are also
    // supported, as well as references to function pointers.
    // The following is the actual (raw) function type however,
    // neither pointer nor reference, though most developers
    // don't work with the actual function type in code too
    // often (since function names decay to a pointer to the
    // function in most scenarios so pointers to functions are
    // much more common in most code - "FunctionTraits" will
    // handle any legal type that identifies the function
    // however, whether the function's actual (raw) C++ type,
    // or pointers or references to functions, or references
    // to pointers to functions)
    ////////////////////////////////////////////////////////
    using F = decltype(TestFuncs::FreeFunc);
    // using F = decltype(&TestFuncs::FreeFunc); // Will also work (pointer to function and usually
                                                 // more common in practice)
    // using F = decltype(TestFuncs::FreeFunc) &; // Will also work (lvalue reference to function,
                                                  // though rvalue works too - both are rare in
                                                  // practice though, noting that this particular
                                                  // "decltype" syntax is even more rare - usually
                                                  // you'll work with a reference to the function
                                                  // in code instead, such as "int (&)()" for
                                                  // example (reference to a function taking no
                                                  // args and returning an "int"), preferably
                                                  // using an alias to simplify the latter
                                                  // (cryptic) syntax

    DisplayFunctionTraits<F>(_T("Free function traits demo"));
}

//////////////////////////////////////////////////////////////////////////////
// DisplayMemberFunctionTraits()
//////////////////////////////////////////////////////////////////////////////
void DisplayMemberFunctionTraits()
{
    ///////////////////////////////////////////////////////////
    // Non-static member function type whose traits we wish to
    // display. Always a member function pointer (references
    // to member function pointers also supported) unlike the
    // free function version in "DisplayFreeFunctionTraits()"
    // above, which supports the function's actual (raw) C++
    // type as well. We don't support the raw C++ type for
    // non-static member functions however, just member
    // function pointers only (or references to such pointers).
    // To this end, when passing a raw function type, it's
    // always assumed to be a free function (which for our
    // purposes also includes static member functions).
    ///////////////////////////////////////////////////////////
    using F = decltype(&TestFuncs::SomeClass::DoSomething);

    DisplayFunctionTraits<F>(_T("Member function traits demo"));
}

//////////////////////////////////////////////////////////////////////////////
// DisplayFunctorTraits()
//////////////////////////////////////////////////////////////////////////////
void DisplayFunctorTraits()
{
    ///////////////////////////////////////////////////////
    // Functor type whose traits we wish to display. For
    // functors, "F" must always be the class type itself,
    // where the class has a single member function called
    // "operator()" (by definition of a functor). Note
    // that overloads of "operator()" are ambiguous
    // however so aren't supported (a compiler error will
    // result). If the function has overloads of this
    // member (rare) then you should take the address of
    // the overload you're interested in and use that
    // instead (i.e., just a normal non-static member
    // function pointer to that overload). In fact, even
    // for functors with just one "operator()" member
    // (like the following example), "operator()" is still
    // just a member function no different than any other,
    // so internally "FunctionTraits" defers to the member
    // function specialization by simply targeting
    // "&F::operator()"
    ///////////////////////////////////////////////////////
    using F = TestFuncs::SomeClass;

    DisplayFunctionTraits<F>(_T("Functor traits demo"));
}

/////////////////////////////////////////////////////////////////////////////
// main()
/////////////////////////////////////////////////////////////////////////////
int main()
{
    tcout << _T("FunctionTraits demo (detected compiler: ") << GetCompilerName() << _T(")\n\n");

    DisplayFreeFunctionTraits();
    DisplayMemberFunctionTraits();
    DisplayFunctorTraits();

    return 0;
}

#else
    #error "This program is only supported in C++17 or later (an earlier version was detected). Please set the appropriate compiler option to target C++17 or later and try again ("-std=c++17" in GCC, Clang and Intel, or "/std:c++17" in MSFT - later versions also supported of course)"
#endif // CPP17_OR_LATER