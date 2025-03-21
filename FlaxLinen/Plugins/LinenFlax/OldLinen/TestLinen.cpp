#include "TestLinen.h"

#include "Engine/Core/Log.h"
#include "Engine/Scripting/Plugins/PluginManager.h"

#include "QuestSystem.h"  // Adjust path as needed
#include "Linen.h"

TestLinen::TestLinen(const SpawnParams& params)
    : Script(params)
{
    // Enable ticking OnUpdate function
    _tickUpdate = true;
}

void TestLinen::OnEnable()
{
    auto* plugin = PluginManager::GetPlugin<Linen>();
    if (plugin) {
        LOG(Info, "Found Linen Plugin!");
        // Test some functionality
        auto* questSystem = plugin->GetSystem<QuestSystem>();
        if (questSystem) {
            LOG(Info, "Quest System loaded");
            questSystem->AddQuest("test_quest", "Test Quest", "A test quest");
        
        } else {
            LOG(Error, "Quest System not found!");
        }
    } else {
        LOG(Error, "Linen Plugin not found!");
    }
}

void TestLinen::OnDisable()
{
    // Here you can add code that needs to be called when script is disabled (eg. unregister from events)
}

void TestLinen::OnUpdate()
{
    // Here you can add code that needs to be called every frame
}
