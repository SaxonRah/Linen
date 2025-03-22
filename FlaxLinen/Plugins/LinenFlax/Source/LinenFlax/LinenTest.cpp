// v LinenTest.cpp
#include "LinenTest.h"
#include "Linen.h"
#include "LinenSystemIncludes.h"
#include "Engine/Core/Log.h"
#include "Engine/Scripting/Plugins/PluginManager.h"

LinenTest::LinenTest(const SpawnParams& params)
    : Script(params)
{
    _tickUpdate = true;
}

void LinenTest::OnEnable()
{   
    try {
        LOG(Info, "LinenTest::OnEnable : Starting LinenTest");

        // Try to get the plugin from the PluginManager
        auto* plugin = PluginManager::GetPlugin<Linen>();
        
        if (plugin) {
            LOG(Info, "LinenTest::OnEnable : Found Linen Plugin, manually initializing...");
            
            // Set plugin references first
            TestSystem::GetInstance()->SetPlugin(plugin);
            CharacterProgressionSystem::GetInstance()->SetPlugin(plugin);
            QuestSystem::GetInstance()->SetPlugin(plugin);

            LOG(Info, "Initializing systems...");
            plugin->Initialize();
            
            // Test CharacterProgressionSystem functionality
            auto* characterProgressionSystem = plugin->GetSystem<CharacterProgressionSystem>();
            if (characterProgressionSystem) {
                LOG(Info, "Character Progression System loaded");
                characterProgressionSystem->AddSkill("strength", "Strength", "Physical power");
                characterProgressionSystem->AddSkill("intelligence", "Intelligence", "Mental acuity");
            } else {
                LOG(Error, "Character Progression System not found!");
            }

            // Test QuestSystem functionality
            auto* questSystem = plugin->GetSystem<QuestSystem>();
            if (questSystem) {
                LOG(Info, "Quest System loaded");
                questSystem->AddQuest("test_quest_completed", "Test Quest Complete", "A test quest complete.");
                questSystem->AddQuest("test_quest_failed", "Test Quest Fail", "A test quest failing.");

                questSystem->ActivateQuest("test_quest_completed");
                questSystem->CompleteQuest("test_quest_completed");
                
                questSystem->ActivateQuest("test_quest_failed");
                questSystem->FailQuest("test_quest_failed");

            } else {
                LOG(Error, "Quest System not found!");
            }

            // Test the TestSystem
            auto* testSystem = plugin->GetSystem<TestSystem>();
            if (testSystem) {
                LOG(Info, "LinenTest::OnEnable : Test System loaded");
                LOG(Info, "LinenTest::OnEnable : About to add value");
                testSystem->AddValue(42);
                
                LOG(Info, "LinenTest::OnEnable : About to get value");
                int value = testSystem->GetValue();
                LOG(Info, "LinenTest::OnEnable : Retrieved value: {0}", value);
            }
            else {
                LOG(Warning, "LinenTest::OnEnable : Test System not found");
            }
        }
        else {
            LOG(Error, "LinenTest::OnEnable : Linen Plugin not found!");
            
            // Instead of creating a local instance, we should find out why the plugin isn't registered
            LOG(Info, "LinenTest::OnEnable : TODO Checking for all available plugins");
            // This would require additional code to list all plugins
        }
    }
    catch (...) {
        LOG(Error, "LinenTest::OnEnable : Exception during Linen testing");
    }

    LOG(Info, "LinenTest::OnEnable completed");
}

void LinenTest::OnDisable()
{
    // Minimal implementation
    LOG(Info, "LinenTest::OnDisable : ran.");
}

void LinenTest::OnUpdate()
{
    // Minimal implementation
    LOG(Info, "LinenTest::OnUpdate : ran.");
}
// ^ LinenTest.cpp