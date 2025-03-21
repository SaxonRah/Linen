#pragma once

#include "Engine/Scripting/Script.h"

API_CLASS() class LINEN_API TestLinen : public Script
{
API_AUTO_SERIALIZATION();
DECLARE_SCRIPTING_TYPE(TestLinen);

    // [Script]
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate() override;
};
