#pragma once

#include "RPGSystem.h"

#include "QuestEvents.h"
#include "QuestTypes.h"

#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <memory>

// Forward declaration
class CharacterProgressionSystem;

class Quest {
public:
    enum class State { Available, Active, Completed, Failed };
    
    Quest(const std::string& id, const std::string& title, const std::string& description);
    
    // Getters/Setters
    std::string GetId() const { return m_id; }
    std::string GetTitle() const { return m_title; }
    std::string GetDescription() const { return m_description; }
    QuestState GetState() const { return m_state; }
    int GetExperienceReward() const { return m_experienceReward; }
    
    void SetState(QuestState state) { m_state = state; }
    void SetExperienceReward(int reward) { m_experienceReward = reward; }
    
    // Add required skill check
    void AddSkillRequirement(const std::string& skillName, int requiredLevel);
    
    // Check if player meets skill requirements
    bool CheckRequirements(const std::unordered_map<std::string, int>& playerSkills) const;
    
    // For serialization
    virtual void Serialize(void* writer) const;
    virtual void Deserialize(void* reader);
    
private:
    std::string m_id;
    std::string m_title;
    std::string m_description;
    QuestState m_state;
    int m_experienceReward;
    
    // Requirements to take/complete the quest
    std::unordered_map<std::string, int> m_skillRequirements;
};

class QuestSystem : public RPGSystem {
public:
    QuestSystem();
    ~QuestSystem();
    
    // RPGSystem interface
    void Initialize() override;
    void Shutdown() override;
    void Update(float deltaTime) override;
    std::string GetSystemName() const override { return "QuestSystem"; }
    
    // Quest management
    bool AddQuest(const std::string& id, const std::string& title, const std::string& description);
    bool ActivateQuest(const std::string& id);
    bool CompleteQuest(const std::string& id);
    bool FailQuest(const std::string& id);
    
    // Quest queries
    Quest* GetQuest(const std::string& id);
    std::vector<Quest*> GetAvailableQuests() const;
    std::vector<Quest*> GetActiveQuests() const;
    std::vector<Quest*> GetCompletedQuests() const;
    
    // Serialization (save/load)
    void Serialize(void* writer) const;
    void Deserialize(void* reader);
    
private:
    // Internal helper methods
    void PublishQuestStateChanged(Quest* quest, QuestState oldState);
    
    // Thread safety
    // mutable std::mutex m_questsMutex;
    
    // Quest storage
    std::unordered_map<std::string, std::unique_ptr<Quest>> m_quests;
};