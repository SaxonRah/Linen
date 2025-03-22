// v DayNightCycle.h
#pragma once

#include "Engine/Scripting/Script.h"
#include "Engine/Level/Actor.h"
#include "Engine/Level/Actors/Light.h"
#include "Engine/Level/Actors/DirectionalLight.h"

API_CLASS() class LINENFLAX_API DayNightCycle : public Script
{
API_AUTO_SERIALIZATION();
DECLARE_SCRIPTING_TYPE(DayNightCycle);

public:
    // Reference to the directional light that will be rotated
    API_FIELD(Attributes="EditorOrder(0), EditorDisplay(\"Light Settings\"), Tooltip(\"The directional light that will be rotated based on time of day\")")
    DirectionalLight* SunLight = nullptr;
    
    // Color for daytime
    API_FIELD(Attributes="EditorOrder(1), EditorDisplay(\"Light Settings\"), Tooltip(\"Color of the light during daytime\")")
    Color DaytimeColor = Color(1.0f, 0.9f, 0.7f, 1.0f);
    
    // Color for nighttime
    API_FIELD(Attributes="EditorOrder(2), EditorDisplay(\"Light Settings\"), Tooltip(\"Color of the light during nighttime\")")
    Color NighttimeColor = Color(0.1f, 0.1f, 0.3f, 1.0f);
    
    // Intensity for daytime
    API_FIELD(Attributes="EditorOrder(3), EditorDisplay(\"Light Settings\"), Tooltip(\"Intensity of the light during daytime\")")
    float DaytimeIntensity = 10.0f;
    
    // Intensity for nighttime
    API_FIELD(Attributes="EditorOrder(4), EditorDisplay(\"Light Settings\"), Tooltip(\"Intensity of the light during nighttime\")")
    float NighttimeIntensity = 0.5f;

    // Control the time scale via the editor
    API_FIELD(Attributes="EditorOrder(5), EditorDisplay(\"Time Settings\"), Tooltip(\"How fast time passes. Higher values = faster day/night cycle\")")
    float TimeScale = 60.0f; // 1 game minute passes every real second by default
    
    // Editor debug controls
    API_FIELD(Attributes="EditorOrder(6), EditorDisplay(\"Debug\"), Tooltip(\"Set a specific hour for testing\")")
    int32 DebugHour = -1;
    
    // Whether to use the debug hour
    API_FIELD(Attributes="EditorOrder(7), EditorDisplay(\"Debug\"), Tooltip(\"When enabled, uses the debug hour instead of real-time progression\")")
    bool UseDebugHour = false;

    API_FIELD(Attributes="EditorOrder(8), EditorDisplay(\"Debug\"), Tooltip(\"Force advance time by this many seconds each frame\")")
    float DebugForceTimeAdvanceSeconds = 0.0f;
    
    API_FIELD(Attributes="EditorOrder(9), EditorDisplay(\"Debug\"), Tooltip(\"Override the day progress value (0.0-1.0)\")")
    float DebugOverrideDayProgress = -1.0f;

    API_FIELD(Attributes="EditorOrder(10), EditorDisplay(\"Debug\"), Tooltip(\"Log debug information\")")
    bool DebugLogging = false;

private:
    // Previous hour for detecting hour changes
    int m_prevHour = -1;
    
    // Previous debug settings for detecting changes
    int m_prevDebugHour = -1;
    bool m_prevUseDebugHour = false;

public:
    // Called when script is being initialized
    void OnEnable() override;
    
    // Called when script is being disabled
    void OnDisable() override;
    
    // Called when script is updated (once per frame)
    void OnUpdate() override;
    
    // Updates the sun's rotation and color based on time of day
    void UpdateSun(float dayProgress);
};