// v DayNightCycle.cpp (final fix)
#include "DayNightCycle.h"
#include "../LinenFlax.h"
#include "../TimeSystem.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Math/Math.h"
#include "Engine/Core/Math/Quaternion.h"
#include "Engine/Level/Level.h"
#include "Engine/Level/Actors/Light.h"
#include "Engine/Level/Actors/DirectionalLight.h"
#include "Engine/Scripting/Plugins/PluginManager.h"

DayNightCycle::DayNightCycle(const SpawnParams& params)
    : Script(params)
{
    // Enable ticking by default
    _tickUpdate = true;
}

void DayNightCycle::OnEnable()
{
    LOG(Info, "DayNightCycle script enabled");
    
    // If no light is assigned, try to find one in the level
    if (SunLight == nullptr)
    {
        Array<Actor*> lights = Level::GetActors(DirectionalLight::GetStaticClass(), true);

        for (Actor* light : lights)
        {
            if (light && light->Is<DirectionalLight>())
            {
                SunLight = static_cast<DirectionalLight*>(light);
                LOG(Info, "Automatically assigned directional light: {0}", SunLight->GetNamePath());
                break;
            }
        }

        if (SunLight == nullptr)
        {
            LOG(Warning, "No directional light found in the level. Please assign one manually.");
        }
    }

    // Set initial state
    m_prevHour = -1;
    
    // Try to get the plugin and time system
    auto* plugin = PluginManager::GetPlugin<LinenFlax>();
    if (plugin)
    {
        auto* timeSystem = plugin->GetSystem<TimeSystem>();
        if (timeSystem)
        {
            // Set time scale from our property
            timeSystem->SetTimeScale(TimeScale);
            LOG(Info, "Time scale set to {0}", TimeScale);
            
            // Set debug hour if needed
            if (UseDebugHour && DebugHour >= 0 && DebugHour < 24)
            {
                timeSystem->DebugSetTime(DebugHour, 0);
                LOG(Info, "Debug time set to {0}:00", DebugHour);
            }
        }
        else
        {
            LOG(Warning, "TimeSystem not available. Day/night cycle won't function properly.");
        }
    }
    else
    {
        LOG(Warning, "LinenFlax plugin not available. Day/night cycle won't function properly.");
    }
}

void DayNightCycle::OnDisable()
{
    LOG(Info, "DayNightCycle script disabled");
}

void DayNightCycle::OnUpdate()
{
    if (SunLight == nullptr)
        return;
    
    // Check for debug mode changes
    bool settingsChanged = (UseDebugHour != m_prevUseDebugHour || 
                           (UseDebugHour && DebugHour != m_prevDebugHour));
    
    m_prevUseDebugHour = UseDebugHour;
    m_prevDebugHour = DebugHour;
    
    // Try to get the plugin and time system
    auto* plugin = PluginManager::GetPlugin<LinenFlax>();
    if (plugin)
    {
        auto* timeSystem = plugin->GetSystem<TimeSystem>();
        if (timeSystem)
        {
            // Update time scale if needed
            if (timeSystem->GetTimeScale() != TimeScale)
            {
                timeSystem->SetTimeScale(TimeScale);
            }
            
            // Handle debug hour setting
            if (settingsChanged && UseDebugHour && DebugHour >= 0 && DebugHour < 24)
            {
                timeSystem->DebugSetTime(DebugHour, 0);
                LOG(Info, "Debug time set to {0}:00", DebugHour);
            }
            
            // Force advance time if requested (convert minutes to hours)
            if (DebugForceTimeAdvanceSeconds > 0.0f)
            {
                timeSystem->AdvanceTimeSeconds(static_cast<int>(DebugForceTimeAdvanceSeconds));
                
                if (DebugLogging)
                {
                    LOG(Info, "Forced time advance by {0} seconds", DebugForceTimeAdvanceSeconds);
                }
            }
            
            // Get current hour and day progress
            int currentHour = timeSystem->GetHour();
            float dayProgress = timeSystem->GetDayProgress();
            
            // Override day progress for direct control
            if (DebugOverrideDayProgress >= 0.0f && DebugOverrideDayProgress <= 1.0f)
            {
                dayProgress = DebugOverrideDayProgress;
                
                if (DebugLogging)
                {
                    LOG(Info, "Using override day progress: {0:0.3f}", dayProgress);
                }
            }
            
            // Log time updates once per hour or when debugging
            if (currentHour != m_prevHour || DebugLogging)
            {
                m_prevHour = currentHour;
                LOG(Info, "Time: {0}, Day progress: {1:0.3f}, Is daytime: {2}", 
                    String(timeSystem->GetFormattedTime().c_str()),
                    dayProgress,
                    String(timeSystem->IsDaytime() ? "Yes" : "No"));
            }
            
            // Update the sun position and color
            UpdateSun(dayProgress);
        }
    }
}

void DayNightCycle::UpdateSun(float dayProgress)
{
    if (SunLight == nullptr)
        return;

    // Calculate rotation based on time of day (2π = full day)
    // We want the sun to be overhead at noon (0.5 day progress)
    const float twoPi = 6.28318f;
    const float pi = 3.14159f;
    const float halfPi = 1.57079f;
    float angle = (dayProgress * twoPi) - halfPi;
    
    // Create rotation based on the angle
    // The sun rotates on the X axis for elevation and rotates around the Y axis for the day cycle
    float sinAngle = Math::Sin(angle);
    float degreesSinAngle = sinAngle * 90.0f;
    float degreesAngle = angle * 180.0f / pi;
    
    // Quaternion rotation = Quaternion::Euler(-90.0f + degreesSinAngle, degreesAngle, 0.0f);
    Quaternion rotation = Quaternion::Euler(degreesAngle, -90.0f + degreesSinAngle, 0.0f);
    
    // Apply rotation to the sun light
    SunLight->SetLocalOrientation(rotation);
    
    // Calculate the light color and intensity based on time
    // Use a smooth transition between day and night
    // Sunrise is around 0.25 day progress, sunset around 0.75
    float dayFactor = 0.0f;
    
    if (dayProgress < 0.2f) // Late night to dawn
    {
        dayFactor = Math::SmoothStep(0.0f, 1.0f, (dayProgress - 0.15f) * 20.0f); 
    }
    else if (dayProgress < 0.8f) // Day
    {
        dayFactor = 1.0f;
    }
    else if (dayProgress < 0.85f) // Dusk
    {
        dayFactor = Math::SmoothStep(1.0f, 0.0f, (dayProgress - 0.8f) * 20.0f);
    }
    else // Night
    {
        dayFactor = 0.0f;
    }
    
    // Blend between night and day colors and intensities
    Color lightColor = Color::Lerp(NighttimeColor, DaytimeColor, dayFactor);
    float lightIntensity = Math::Lerp(NighttimeIntensity, DaytimeIntensity, dayFactor);
    
    // In Flax 1.9, we'll need to directly set properties
    SunLight->Color = lightColor;
    SunLight->Brightness = lightIntensity;
    
    // If it's night time, rotate the light to simulate moonlight (opposite of sun)
    if (dayFactor < 0.1f)
    {
        // Flip the light direction for night
        float nightAngle = angle + pi;
        float sinNightAngle = Math::Sin(nightAngle);
        float degreesNightSinAngle = sinNightAngle * 90.0f;
        float degreesNightAngle = nightAngle * 180.0f / pi;
        
        // Quaternion nightRotation = Quaternion::Euler(-90.0f + degreesNightSinAngle, degreesNightAngle, 0.0f);
        Quaternion nightRotation = Quaternion::Euler(degreesNightAngle, -90.0f + degreesNightSinAngle, 0.0f);
        SunLight->SetLocalOrientation(nightRotation);
    }
}
