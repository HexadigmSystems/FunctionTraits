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
// Module version of "FunctionTraits.h". Simply defers to "FunctionTraits.h"
// to export all public declarations in that header, in particular struct
// "FunctionTraits" and all declarations associated with it. Other public
// declarations unrelated to "FunctionTraits" itself are also available in
// "FunctionTraits.h" however and are therefore also exported by this module
// (mostly support declarations that "FunctionTraits" itself relies on but
// sometimes useful by end-users). The focus of the module for now however
// is "FunctionTraits" even though some of the exported declarations below
// aren't directly related to it (so not documented at the library's GitHub
// web site but exported anyway in case anyone wants to use them). For
// complete details on module support in "FunctionTraits", see
// https://github.com/HexadigmSystems/FunctionTraits#moduleusage
/////////////////////////////////////////////////////////////////////////////

module;

////////////////////////////////////////////////////////////
// Let "FunctionTraits.h" just below know we're building
// the "FunctionTraits" module. Following is only #defined
// when we are ...
////////////////////////////////////////////////////////////
#define STDEXT_BUILDING_MODULE_FUNCTION_TRAITS
#include "FunctionTraits.h"
#undef STDEXT_BUILDING_MODULE_FUNCTION_TRAITS

export module FunctionTraits;

////////////////////////////////////////////////////////////////////////
// Export "CompilerVersions" to make the module version of
// "FunctionTraits" consistent with the non-module version. In the
// non-module version, a call to #include "FunctionTraits.h"
// automatically #includes "CompilerVersions.h" as well, so in the
// module version, a call to "import FunctionTraits" automatically
// imports "CompilerVersions" via the following call (so for most
// intents and purposes it's consistent with the behavior of the
// non-module version). To pick up the macros in "CompilerVersions.h"
// as well however (since macros aren't exported by C++ modules), just
// #include either "FunctionTraits.h" or "CompilerVersions.h" directly
// instead (and if so you don't even have to directly code your own
// "import" statement, as each header does this for you when the
// constant STDEXT_USE_MODULES is #defined as it normally should be
// (when using the module version of "FunctionTraits"). See the
// following for complete details:
//
//    https://github.com/HexadigmSystems/FunctionTraits#moduleusage
////////////////////////////////////////////////////////////////////////
export import CompilerVersions;

//////////////////////////////////////////////////////////////////////
// Interface for this module. We simply rely on "using" declarations
// in the code below to export all public declarations from
// "FunctionTraits.h" above (internal declarations from
// "FunctionTraits.h" not intended for public use are all declared in
// namespace "StdExt::Private" and are not exported below - all
// others are). Note that only declarations associated with
// "FunctionTraits" are documented at
// https://github.com/HexadigmSystems/FunctionTraits however. All
// other exported declarations below are still available for public
// use however though they're not the focus of the above link. The
// declarations specifically related to "FunctionTraits" are (though
// the "FunctionTraits" declarations rely on the other declarations
// to carry out their work). The upshot is that while all other
// declarations below are undocumented at the above link, users who
// wish to use them for their own purposes may safely do so (just
// consult them in "FunctionTraits.h" itself for details, not the
// above link). See the following for details on using this module:
//
//     https://github.com/HexadigmSystems/FunctionTraits#moduleusage
//
// IMPORTANT:
// ---------
// Note that GCC is currently buggy at this writing (modules still
// under development), and fails to compile the code below (so until
// corrected, this module can't be used in GCC). See the following
// (effectively identical) GCC bug reports (now showing the bug has
// in fact been flagged as corrected but an updated version of GCC
// with the fix hasn't been released yet at this writing - it may be
// before too long though):
//
//    https://gcc.gnu.org/bugzilla/show_bug.cgi?id=109679
//    https://gcc.gnu.org/bugzilla/show_bug.cgi?id=113129
//////////////////////////////////////////////////////////////////////
export namespace StdExt
{
    using StdExt::AlwaysFalse_v;
    using StdExt::AlwaysTrue_v;
    using StdExt::TypeName_v;

    #if defined(USE_CONCEPTS)
        using StdExt::IsFunction_c;
    #endif

    #if defined(USE_CONCEPTS)
        using StdExt::IsClass_c;
    #endif

    using StdExt::IsClassOrVoid_v;
    #if defined(USE_CONCEPTS)
        using StdExt::IsClassOrVoid_c;
    #endif

    using StdExt::IsConstOrVolatile_v;
    using StdExt::IsSpecialization;
    using StdExt::IsSpecialization_v;
    using StdExt::TemplateArgsTuple;
    using StdExt::TemplateArgsTuple_t;
    using StdExt::IthTemplateArg_t;
    using StdExt::FirstTemplateArg_t;
    using StdExt::TemplateArg_t;
    using StdExt::IsTemplateBaseOf_v;
    using StdExt::RemoveCvRef_t;
    using StdExt::RemovePtrRef_t;
    using StdExt::size_t_npos;

    using StdExt::LessThanOrEqual_v;
    #if defined(USE_CONCEPTS)
        using StdExt::LessThanOrEqual_c;
    #else
        using StdExt::StaticAssertLessThanOrEqual;
    #endif

    using StdExt::CountRemaining_v;
    using StdExt::CountExceedsRemaining_v;
    using StdExt::MinOfCountOrRemaining_v;
    using StdExt::MakeIndexSequenceStartAt;
    using StdExt::IsForEachFunctor;
    using StdExt::IsForEachFunctor_v;
    #if defined(USE_CONCEPTS)
        using StdExt::ForEachFunctor_c;
    #endif
    using StdExt::ForEach;

    using StdExt::IsTupleSpecialization_v;
    #if defined(USE_CONCEPTS)
        using StdExt::Tuple_c;
    #else
        using StdExt::StaticAssertIsTuple;
    #endif

    using StdExt::IndexLessThanTupleSize_v;
    #if defined(USE_CONCEPTS)
        using StdExt::IndexLessThanTupleSize_c;
    #else
        using StdExt::StaticAssertIndexLessThanTupleSize;
    #endif

    using StdExt::IndexLessThanOrEqualToTupleSize_v;
    #if defined(USE_CONCEPTS)
        using StdExt::IndexLessThanOrEqualToTupleSize_c;
    #else
        using StdExt::StaticAssertIndexLessThanOrEqualToTupleSize;
    #endif

    using StdExt::MinOfCountOrTupleTypesRemaining_v;
    using StdExt::SubTuple_t;
    using StdExt::TupleModifyTuple_t;
    using StdExt::TupleModify_t;
    using StdExt::TupleInsertTuple_t;
    using StdExt::TupleInsert_t;
    using StdExt::TupleAppendTuple_t;
    using StdExt::TupleAppend_t;
    using StdExt::TupleDelete_t;
    using StdExt::IsForEachTupleTypeFunctor;
    using StdExt::IsForEachTupleTypeFunctor_v;
    #if defined(USE_CONCEPTS)
        using StdExt::ForEachTupleTypeFunctor_c;
    #endif
    using StdExt::ForEachTupleType;

    using StdExt::IsFreeFunction_v;
    #if defined(USE_CONCEPTS)
        using StdExt::FreeFunction_c;
    #endif

    using StdExt::IsFreeFunctionPointer_v;
    using StdExt::IsAbominableFunction_v;
    #if defined(USE_CONCEPTS)
        using StdExt::AbominableFunction_c;
    #endif

    #if defined(USE_CONCEPTS)
        using StdExt::MemberFunctionPointer_c;
    #endif

    using StdExt::IsNonOverloadedFunctor;
    using StdExt::IsNonOverloadedFunctor_v;
    #if defined(USE_CONCEPTS)
        using StdExt::NonOverloadedFunctor_c;
    #endif

    using StdExt::IsNonOverloadedStaticFunctor;
    using StdExt::IsNonOverloadedStaticFunctor_v;
    #if defined(USE_CONCEPTS)
        using StdExt::NonOverloadedStaticFunctor_c;
    #endif

    using StdExt::IsStdFunctionSpecialization_v;
    #if defined(USE_CONCEPTS)
        using StdExt::StdFunction_c;
    #endif

    using StdExt::StdFunctionTemplateArg_t;

    using StdExt::IsTraitsFreeFunction_v;
    #if defined(USE_CONCEPTS)
        using StdExt::TraitsFreeFunction_c;
    #endif

    using StdExt::IsTraitsMemberFunction_v;
    #if defined(USE_CONCEPTS)
        using StdExt::TraitsMemberFunction_c;
    #endif

    using StdExt::IsTraitsStdFunction_v;
    #if defined(USE_CONCEPTS)
        using StdExt::TraitsStdFunction_c;
    #endif

    using StdExt::IsTraitsFunctor_v;
    #if defined(USE_CONCEPTS)
        using StdExt::TraitsFunctor_c;
    #endif

    using StdExt::IsTraitsStaticFunctor_v;
    #if defined(USE_CONCEPTS)
        using StdExt::TraitsStaticFunctor_c;
    #endif

    using StdExt::IsTraitsNonStaticOrStaticFunctor_v;
    #if defined(USE_CONCEPTS)
        using StdExt::TraitsNonStaticOrStaticFunctor_c;
    #endif

    using StdExt::IsTraitsFunction_v;
    #if defined(USE_CONCEPTS)
        using StdExt::TraitsFunction_c;
    #endif

    using StdExt::CallingConvention;
    using StdExt::CallingConventionCount_v;
    using StdExt::CallingConventionToString;
    using StdExt::CallingConventionReplacedWithCdecl;
    using StdExt::FunctionReference;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::RefQualifier;
    #endif
    using StdExt::FunctionReferenceToString;
    using StdExt::FunctionClassification;
    using StdExt::FunctionOrigin;
    using StdExt::AndVariadic;

    using StdExt::IsValidReturnType;
    using StdExt::IsValidReturnType_v;
    #if defined(USE_CONCEPTS)
        using StdExt::ValidReturnType_c;
    #endif

    using StdExt::IsValidFunctionArgTypesTuple;
    using StdExt::IsValidFunctionArgTypesTuple_v;
    #if defined(USE_CONCEPTS)
        using StdExt::ValidFunctionArgTypesTuple_c;
    #endif

    using StdExt::IsValidFunctionArgTypes;
    using StdExt::IsValidFunctionArgTypes_v;
    #if defined(USE_CONCEPTS)
        using StdExt::ValidFunctionArgType_c;
    #endif

    using StdExt::IsForEachArgFunctor_v;
    #if defined(USE_CONCEPTS)
        using StdExt::ForEachArgFunctor_c;
    #endif

    using StdExt::FunctionTraits;
    using StdExt::DefaultCallingConvention_v;
    using StdExt::DefaultCallingConventionName_v;
    using StdExt::IsFunctionTraitsSpecialization_v;
    #if defined(USE_CONCEPTS)
        using StdExt::FunctionTraits_c;
    #endif

    /////////////////////////////////////////////////////
    // "FunctionTraits" helper templates (read traits)
    // taking a "FunctionTraits" template arg. Rarely
    // used directly as most will rely on the
    // "FunctionTraits" helper templates taking a
    // function template arg "F" instead (which simply
    // defer to the following)
    /////////////////////////////////////////////////////
    using StdExt::FunctionTraitsArgCount_v;
    #if defined(USE_CONCEPTS)
        using StdExt::FunctionTraitsIndexLessThanArgCount_v;
        using StdExt::FunctionTraitsIndexLessThanArgCount_c;
    #endif
    #if defined(USE_CONCEPTS)
        using StdExt::FunctionTraitsIndexLessThanOrEqualToArgCount_v;
        using StdExt::FunctionTraitsIndexLessThanOrEqualToArgCount_c;
    #endif
    #if defined(USE_CONCEPTS)
        using StdExt::FunctionTraitsIndexLessThanArgCountOrReturnVoid_v;
    #endif
    using StdExt::FunctionTraitsArgType_t;
    #if defined(USE_CONCEPTS)
        using StdExt::FunctionTraitsIndexLessThanArgCountOrReturnEmptyString_v;
    #endif
    using StdExt::FunctionTraitsArgTypeName_v;
    using StdExt::FunctionTraitsArgTypes_t;
    using StdExt::FunctionTraitsCallingConvention_v;
    using StdExt::FunctionTraitsCallingConventionName_v;
    using StdExt::FunctionTraitsForEachArg;
    using StdExt::FunctionTraitsFunctionClassification_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::FunctionTraitsIsMemberFunction_v;
    #endif
    using StdExt::FunctionTraitsFunctionOrigin_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::FunctionTraitsIsFunctor_v;
    #endif
    using StdExt::FunctionTraitsFunctionRawType_t;
    using StdExt::FunctionTraitsFunctionRawTypeName_v;
    using StdExt::FunctionTraitsFunctionType_t;
    using StdExt::FunctionTraitsFunctionTypeName_v;
    #if defined(USE_CONCEPTS)
        using StdExt::FunctionTraitsIndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v;
    #endif
    using StdExt::FunctionTraitsIsArgListEmpty_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::FunctionTraitsIsEmptyArgList_v;
    #endif
    using StdExt::FunctionTraitsIsArgTypeMatch_v;
    using StdExt::FunctionTraitsIsArgTypesMatch_v;
    #if defined(USE_CONCEPTS)
        using StdExt::FunctionTraitsIndexLessThanArgCountOrCompareWithVoid_v;
    #endif
    using StdExt::FunctionTraitsIsArgTypeSame_v;
    using StdExt::FunctionTraitsIsArgTypesSame_v;
    using StdExt::FunctionTraitsIsArgTypesSameTuple_v;
    using StdExt::FunctionTraitsIsFunctionConst_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::FunctionTraitsIsMemberFunctionConst_v;
    #endif
    using StdExt::FunctionTraitsIsFunctionVolatile_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::FunctionTraitsIsMemberFunctionVolatile_v;
    #endif
    using StdExt::FunctionTraitsFunctionReference_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::FunctionTraitsMemberFunctionRefQualifier_v;
    #endif
    using StdExt::FunctionTraitsFunctionReferenceName_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::FunctionTraitsMemberFunctionRefQualifierName_v;
    #endif
    using StdExt::FunctionTraitsIsNoexcept_v;
    using StdExt::FunctionTraitsIsReturnTypeMatch_v;
    using StdExt::FunctionTraitsIsReturnTypeSame_v;
    using StdExt::FunctionTraitsIsReturnTypeVoid_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::FunctionTraitsIsVoidReturnType_v;
    #endif
    using StdExt::FunctionTraitsIsVariadic_v;
    using StdExt::FunctionTraitsMemberFunctionClass_t;
    using StdExt::FunctionTraitsMemberFunctionClassName_v;
    using StdExt::FunctionTraitsReturnType_t;
    using StdExt::FunctionTraitsReturnTypeName_v;

    /////////////////////////////////////////////////////
    // "FunctionTraits" helper templates (write traits)
    // taking a "FunctionTraits" template arg. Rarely
    // used as most will rely on the "FunctionTraits"
    // helper templates taking a function template arg
    // "F" instead (which simply defer to the following)
    /////////////////////////////////////////////////////
    #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
        using StdExt::FunctionTraitsAddNoexcept_t;
        using StdExt::FunctionTraitsAddVariadicArgs_t;
        using StdExt::FunctionTraitsFunctionAddConst_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionAddConst_t;
        #endif
        using StdExt::FunctionTraitsFunctionRemoveConst_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionRemoveConst_t;
        #endif
        using StdExt::FunctionTraitsFunctionAddVolatile_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionAddVolatile_t;
        #endif
        using StdExt::FunctionTraitsFunctionRemoveVolatile_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionRemoveVolatile_t;
        #endif
        using StdExt::FunctionTraitsFunctionAddCV_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionAddCV_t;
        #endif
        using StdExt::FunctionTraitsFunctionRemoveCV_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionRemoveCV_t;
        #endif
        using StdExt::FunctionTraitsFunctionAddLValueReference_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionAddLValueReference_t;
        #endif
        using StdExt::FunctionTraitsFunctionAddRValueReference_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionAddRValueReference_t;
        #endif
        using StdExt::FunctionTraitsFunctionRemoveReference_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionRemoveReference_t;
        #endif
        using StdExt::FunctionTraitsReplaceMemberFunctionClass_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsMemberFunctionReplaceClass_t;
        #endif
        using StdExt::FunctionTraitsRemoveNoexcept_t;
        using StdExt::FunctionTraitsRemoveVariadicArgs_t;
        using StdExt::FunctionTraitsReplaceArgs_t;
        using StdExt::FunctionTraitsReplaceArgsTuple_t;
        using StdExt::FunctionTraitsReplaceCallingConvention_t;
        using StdExt::FunctionTraitsArgsModify_t;
        using StdExt::FunctionTraitsArgsModifyTuple_t;
        using StdExt::FunctionTraitsArgsInsert_t;
        using StdExt::FunctionTraitsArgsInsertTuple_t;
        using StdExt::FunctionTraitsArgsMakeVoid_t;
        using StdExt::FunctionTraitsArgsAppend_t;
        using StdExt::FunctionTraitsArgsAppendTuple_t;
        using StdExt::FunctionTraitsArgsDelete_t;
        using StdExt::FunctionTraitsReplaceArg_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::FunctionTraitsReplaceNthArg_t;
        #endif
        using StdExt::FunctionTraitsReplaceReturnType_t;
        using StdExt::FunctionTraitsMakeMemberFunctionPtr_t;
    #endif

    ////////////////////////////////////////////////////
    // "FunctionTraits" helper templates (read traits)
    // taking a function template arg "F". Along with
    // the write traits just below, these are the
    // templates most will normally rely on as fully
    // documented here:
    //
    //   https://github.com/HexadigmSystems/FunctionTraits/#writetraits
    ////////////////////////////////////////////////////
    using StdExt::ArgCount_v;
    #if defined(USE_CONCEPTS)
        using StdExt::IndexLessThanArgCount_v;
        using StdExt::IndexLessThanArgCount_c;
    #endif
    #if defined(USE_CONCEPTS)
        using StdExt::IndexLessThanOrEqualToArgCount_v;
        using StdExt::IndexLessThanOrEqualToArgCount_c;
    #endif
    #if defined(USE_CONCEPTS)
        using StdExt::IndexLessThanArgCountOrReturnVoid_v;
    #endif
    using StdExt::ArgType_t;
    #if defined(USE_CONCEPTS)
        using StdExt::IndexLessThanArgCountOrReturnEmptyString_v;
    #endif
    using StdExt::ArgTypeName_v;
    using StdExt::ArgTypes_t;
    using StdExt::CallingConvention_v;
    using StdExt::CallingConventionName_v;
    using StdExt::ForEachArg;
    using StdExt::FunctionClassification_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::IsMemberFunction_v;
    #endif
    using StdExt::FunctionOrigin_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::IsFunctor_v;
    #endif
    using StdExt::FunctionRawType_t;
    using StdExt::FunctionRawTypeName_v;
    using StdExt::FunctionType_t;
    using StdExt::FunctionTypeName_v;
    #if defined(USE_CONCEPTS)
        using StdExt::IndexValidInBothOrReturnFalseIfOneHasArgOtherDoesnt_v;
    #endif
    using StdExt::IsArgListEmpty_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::IsEmptyArgList_v;
    #endif
    using StdExt::IsArgTypeMatch_v;
    using StdExt::IsArgTypesMatch_v;
    #if defined(USE_CONCEPTS)
        using StdExt::IndexLessThanArgCountOrCompareWithVoid_v;
    #endif
    using StdExt::IsArgTypeSame_v;
    using StdExt::IsArgTypesSame_v;
    using StdExt::IsArgTypesSameTuple_v;
    using StdExt::IsFunctionConst_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::IsMemberFunctionConst_v;
    #endif
    using StdExt::IsFunctionVolatile_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::IsMemberFunctionVolatile_v;
    #endif
    using StdExt::FunctionReference_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::MemberFunctionRefQualifier_v;
    #endif
    using StdExt::FunctionReferenceName_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::MemberFunctionRefQualifierName_v;
    #endif
    using StdExt::IsNoexcept_v;
    using StdExt::IsReturnTypeMatch_v;
    using StdExt::IsReturnTypeSame_v;
    using StdExt::IsReturnTypeVoid_v;
    #if defined(STDEXT_SUPPORT_DEPRECATED)
        using StdExt::IsVoidReturnType_v;
    #endif
    using StdExt::IsVariadic_v;
    using StdExt::MemberFunctionClass_t;
    using StdExt::MemberFunctionClassName_v;
    using StdExt::ReturnType_t;
    using StdExt::ReturnTypeName_v;

    ////////////////////////////////////////////////////
    // "FunctionTraits" helper templates (write traits)
    // taking a function template arg "F". Along with
    // the read traits just above, these are the
    // templates most will normally rely on as fully
    // documented here:
    //
    //   https://github.com/HexadigmSystems/FunctionTraits/#writetraits
    ////////////////////////////////////////////////////
    #if defined(FUNCTION_WRITE_TRAITS_SUPPORTED)
        using StdExt::AddNoexcept_t;
        using StdExt::AddVariadicArgs_t;
        using StdExt::FunctionAddConst_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionAddConst_t;
        #endif
        using StdExt::FunctionRemoveConst_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionRemoveConst_t;
        #endif
        using StdExt::FunctionAddVolatile_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionAddVolatile_t;
        #endif
        using StdExt::FunctionRemoveVolatile_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionRemoveVolatile_t;
        #endif
        using StdExt::FunctionAddCV_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionAddCV_t;
        #endif
        using StdExt::FunctionRemoveCV_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionRemoveCV_t;
        #endif
        using StdExt::FunctionAddLValueReference_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionAddLValueReference_t;
        #endif
        using StdExt::FunctionAddRValueReference_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionAddRValueReference_t;
        #endif
        using StdExt::FunctionRemoveReference_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionRemoveReference_t;
        #endif
        using StdExt::ReplaceMemberFunctionClass_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::MemberFunctionReplaceClass_t;
        #endif
        using StdExt::RemoveNoexcept_t;
        using StdExt::RemoveVariadicArgs_t;
        using StdExt::ReplaceArgs_t;
        using StdExt::ReplaceArgsTuple_t;
        using StdExt::ReplaceCallingConvention_t;
        using StdExt::ArgsModify_t;
        using StdExt::ArgsModifyTuple_t;
        using StdExt::ArgsInsert_t;
        using StdExt::ArgsInsertTuple_t;
        using StdExt::ArgsMakeVoid_t;
        using StdExt::ArgsAppend_t;
        using StdExt::ArgsAppendTuple_t;
        using StdExt::ArgsDelete_t;
        using StdExt::ReplaceArg_t;
        #if defined(STDEXT_SUPPORT_DEPRECATED)
            using StdExt::ReplaceNthArg_t;
        #endif
        using StdExt::ReplaceReturnType_t;
        using StdExt::MakeMemberFunctionPtr_t;
    #endif

    using StdExt::DisplayAllFunctionTraits;

    using StdExt::IsHasFunctionTraits_v;
    #if defined(USE_CONCEPTS)
        using StdExt::IsHasFunctionTraits_c;
    #endif

    //////////////////////////////////////////////////////////////
    // Always false (undefined) at this writing (for now). May be
    // #defined (or removed) in a future release however (still a
    // work-in-progress but deferred until reflection is
    // available in C++26 - will hopefully resolve several issues
    // required to cleanly implement operator detection).
    //////////////////////////////////////////////////////////////
    #if defined(STDEXT_SUPPORTS_OPERATOR_DETECTION_TEMPLATES)
        using StdExt::OperatorFunctions;

        #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)
            using StdExt::ClassHasOperator;
            using StdExt::ClassHasOperator_v;
            using StdExt::ClassHasStaticOperator;
            using StdExt::ClassHasStaticOperator_v;
            using StdExt::ClassHasNonOverloadedOperator;
            using StdExt::ClassHasNonOverloadedOperator_v;
        #endif // #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)

        using StdExt::ClassHasNonOverloadedStaticOperator;
        using StdExt::ClassHasNonOverloadedStaticOperator_v;
        using StdExt::ClassHasNonOverloadedOperatorTraits;
        using StdExt::ClassHasNonOverloadedOperatorTraits_v;
        using StdExt::ClassHasNonOverloadedStaticOperatorTraits;
        using StdExt::ClassHasNonOverloadedStaticOperatorTraits_v;
    #endif // #if defined(STDEXT_SUPPORTS_OPERATOR_DETECTION_TEMPLATES)

    #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)
        using StdExt::ClassHasOperator_FunctionCall;
        using StdExt::ClassHasOperator_FunctionCall_v;
        using StdExt::ClassHasNonOverloadedOperator_FunctionCall;
        using StdExt::ClassHasNonOverloadedOperator_FunctionCall_v;
    #endif // #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)

    using StdExt::ClassHasNonOverloadedOperatorTraits_FunctionCall;
    using StdExt::ClassHasNonOverloadedOperatorTraits_FunctionCall_v;

    // See https://wg21.link/P1169R4 (C++23 or later)
    #if __cpp_static_call_operator
        #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)
            using StdExt::ClassHasStaticOperator_FunctionCall;
            using StdExt::ClassHasStaticOperator_FunctionCall_v;
        #endif // #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)

        using StdExt::ClassHasNonOverloadedStaticOperator_FunctionCall;
        using StdExt::ClassHasNonOverloadedStaticOperator_FunctionCall_v;
        using StdExt::ClassHasNonOverloadedStaticOperatorTraits_FunctionCall;
        using StdExt::ClassHasNonOverloadedStaticOperatorTraits_FunctionCall_v;
    #endif
} // export namespace StdExt

export namespace StdExt::Private
{
    //////////////////////////////////////////////////////////
    // Both of these are declared in "StdExt::Private" (so
    // for internal use by the library only) but need to be
    // exported anyway (the 1st below only required though if
    // the constant just below is #defined). The library's
    // function detection templates are declared in the
    // user's own code using the function detection macros
    // and some of these templates invoke the following
    // templates (declared in namespace "StdExt::Private").
    // Because the function detection templates themselves
    // are declared in the user's own code however (so
    // declared in the user's own namespace, not the
    // library's), the following need to be exported so they
    // can be called from those templates (in whatever
    // namespace the user declares these templates in).
    //////////////////////////////////////////////////////////
    #if defined(WRITE_DEPENDENT_FUNCTION_DETECTION_TEMPLATES_SUPPORTED)
        using StdExt::Private::ClassHasFunctionStaticCastTarget_t;
    #endif
    using StdExt::Private::InvokeHasFunctionTraits_v;
} // export namespace StdExt::Private