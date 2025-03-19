#pragma once

#include "Engine/Scripting/Script.h"
#include "Engine/Core/Math/Vector4.h"

/// <summary>
/// LinenFunctionLib Function Library
/// </summary>
API_CLASS(Static) class LINEN_API LinenFunctionLib 
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(LinenFunctionLib);
public:

    /// <summary>
    /// Logs the function parameter natively.
    /// </summary>
    /// <param name="data">Data to pass to native code</param>
    API_FUNCTION() static void RunNativeAction(Vector4 data);
};
