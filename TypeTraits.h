// This header is deprecated! See details below.

#ifndef TYPE_TRAITS
#define TYPE_TRAITS
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
// This file is deprecated. Users should #include "FunctionTraits.h" instead.
// The following header just defers to it with a warning message (see this
// below for details).
/////////////////////////////////////////////////////////////////////////////
#pragma message("Warning: This header (\"TypeTraits.h\") was renamed to \"FunctionTraits.h\". The old name continues to be supported for now (it just defers to the new header), but may be removed in a future release. Users should update their code to #include \"FunctionTraits.h\" instead.")
#include "FunctionTraits.h"

#endif // #ifndef TYPE_TRAITS (#include guard)