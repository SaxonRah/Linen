// v LinenTest.cpp
#include "LinenTest.h"
#include "LinenFlax.h"
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
        auto* plugin = PluginManager::GetPlugin<LinenFlax>();
        if (plugin && typeid(*plugin) == typeid(LinenFlax)) {

            // Test CharacterProgressionSystem functionality
            auto* characterProgressionSystem = plugin->GetSystem<CharacterProgressionSystem>();
            if (characterProgressionSystem) {
                LOG(Info, "Character Progression System loaded");

                // Skills
                characterProgressionSystem->AddSkill("strength", "Strength", "Physical power");
                characterProgressionSystem->AddSkill("intelligence", "Intelligence", "Mental acuity");
                characterProgressionSystem->IncreaseSkill("strength", 42);
                characterProgressionSystem->IncreaseSkill("intelligence", 42);
                int str_skill_level = characterProgressionSystem->GetSkillLevel("strength");
                LOG(Info, "LinenTest::OnEnable : characterProgressionSystem Retrieved Skill Level: {0}", str_skill_level);
                int int_skill_level = characterProgressionSystem->GetSkillLevel("intelligence");
                LOG(Info, "LinenTest::OnEnable : characterProgressionSystem Retrieved Skill Level: {0}", int_skill_level);
                
                // Experience
                int experience = characterProgressionSystem->GetExperience();
                LOG(Info, "LinenTest::OnEnable : characterProgressionSystem Retrieved Experience: {0}", experience);
                characterProgressionSystem->GainExperience(42);
                experience = characterProgressionSystem->GetExperience();
                LOG(Info, "LinenTest::OnEnable : characterProgressionSystem Retrieved Experience: {0}", experience);
                int level = characterProgressionSystem->GetLevel();
                LOG(Info, "LinenTest::OnEnable : characterProgressionSystem Retrieved Level: {0}", level);
            } else {
                LOG(Error, "Character Progression System not found!");
            }

            // Test QuestSystem functionality
            auto* questSystem = plugin->GetSystem<QuestSystem>();
            if (questSystem) {
                LOG(Info, "Quest System loaded");

                // Quest Management
                questSystem->AddQuest("test_quest_completed", "Test Quest Complete", "A test quest complete.");
                questSystem->AddQuest("test_quest_failed", "Test Quest Fail", "A test quest failing.");
                questSystem->ActivateQuest("test_quest_completed");
                questSystem->CompleteQuest("test_quest_completed");
                questSystem->ActivateQuest("test_quest_failed");
                questSystem->FailQuest("test_quest_failed");

                // Quest queries
                questSystem->AddQuest("test_quest_query", "Test Quest Query", "A test quest query.");
                questSystem->AddQuest("test_quest_query_2", "Test Quest Query 2", "A test quest query 2.");
                questSystem->ActivateQuest("test_quest_query");
                Quest* quest = questSystem->GetQuest("test_quest_query");
                
                std::vector<Quest*> availableQuests = questSystem->GetAvailableQuests();
                std::vector<Quest*> activeQuests = questSystem->GetActiveQuests();
                std::vector<Quest*> completedQuests = questSystem->GetCompletedQuests();
                std::vector<Quest*> failedQuests = questSystem->GetFailedQuests();

                // Log just the sizes
                LOG(Info, "LinenTest::OnEnable : questSystem Retrieved Available Quests: {0}", availableQuests.size());
                LOG(Info, "LinenTest::OnEnable : questSystem Retrieved Active Quests: {0}", activeQuests.size());
                LOG(Info, "LinenTest::OnEnable : questSystem Retrieved Completed Quests: {0}", completedQuests.size());
                LOG(Info, "LinenTest::OnEnable : questSystem Retrieved Failed Quests: {0}", failedQuests.size());

                // For available quests
                String availableQuestIds;
                for (size_t i = 0; i < availableQuests.size(); i++) {
                    availableQuestIds += String(availableQuests[i]->GetId().c_str());
                    if (i < availableQuests.size() - 1) availableQuestIds += TEXT(", ");
                }
                LOG(Info, "LinenTest::OnEnable : questSystem Retrieved Available Quests: {0} [{1}]", 
                    availableQuests.size(), 
                    availableQuestIds);

                // For active quests
                String activeQuestIds;
                for (size_t i = 0; i < activeQuests.size(); i++) {
                    activeQuestIds += String(activeQuests[i]->GetId().c_str());
                    if (i < activeQuests.size() - 1) activeQuestIds += TEXT(", ");
                }
                LOG(Info, "LinenTest::OnEnable : questSystem Retrieved Active Quests: {0} [{1}]", 
                    activeQuests.size(), 
                    activeQuestIds);

                // For completed quests
                String completedQuestIds;
                for (size_t i = 0; i < completedQuests.size(); i++) {
                    completedQuestIds += String(completedQuests[i]->GetId().c_str());
                    if (i < completedQuests.size() - 1) completedQuestIds += TEXT(", ");
                }
                LOG(Info, "LinenTest::OnEnable : questSystem Retrieved Completed Quests: {0} [{1}]", 
                    completedQuests.size(), 
                    completedQuestIds);
                    
                // For failed quests
                String failedQuestIds;
                for (size_t i = 0; i < failedQuests.size(); i++) {
                    failedQuestIds += String(failedQuests[i]->GetId().c_str());
                    if (i < failedQuests.size() - 1) failedQuestIds += TEXT(", ");
                }
                LOG(Info, "LinenTest::OnEnable : questSystem Retrieved Failed Quests: {0} [{1}]", 
                    failedQuests.size(), 
                    failedQuestIds);

            } else {
                LOG(Error, "Quest System not found!");
            }

            // Test the SaveLoadSystem
            auto* saveLoadSystem = plugin->GetSystem<SaveLoadSystem>();
            if (saveLoadSystem) {
                LOG(Info, "LinenTest::OnEnable : Save Load System loaded");
                
                // Test binary serialization
                saveLoadSystem->SaveGame("TestSave.bin", SerializationFormat::Binary);
                saveLoadSystem->LoadGame("TestSave.bin", SerializationFormat::Binary);
                
                // Test text serialization
                saveLoadSystem->SaveGame("TestSave.txt", SerializationFormat::Text);
                saveLoadSystem->LoadGame("TestSave.txt", SerializationFormat::Text);
            }
            else {
                LOG(Warning, "LinenTest::OnEnable : Save Load System not found");
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
        } else {
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