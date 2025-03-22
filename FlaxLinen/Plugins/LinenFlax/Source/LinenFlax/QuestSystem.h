// v QuestSystem.h
#pragma once

#include "RPGSystem.h"
#include "QuestEvents.h"
#include "QuestTypes.h"
#include "Serialization.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

// Forward declaration
class CharacterProgressionSystem;

enum class QuestResult {
    Success,            
    NotFound,           // Quest ID not found
    AlreadyExists,      // When trying to add a quest that already exists
    InvalidState,       // Quest is in wrong state for the operation
    RequirementsNotMet, // Player doesn't meet skill requirements
    Error               // Generic error
};

class Quest {
public:
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
    // Delete copy constructor and assignment operator
    QuestSystem(const QuestSystem&) = delete;
    QuestSystem& operator=(const QuestSystem&) = delete;

    // RPGSystem interface
    void Initialize() override;
    void Shutdown() override;
    void Update(float deltaTime) override;
    
    // Implement GetName from LinenSystem
    std::string GetName() const override { return "QuestSystem"; }
    
    // Serialization override
    void Serialize(BinaryWriter& writer) const override;
    void Deserialize(BinaryReader& reader) override;
    
    // Quest management
    QuestResult AddQuest(const std::string& id, const std::string& title, const std::string& description);
    QuestResult ActivateQuest(const std::string& id);
    QuestResult CompleteQuest(const std::string& id);
    QuestResult FailQuest(const std::string& id);

    // Quest queries
    Quest* GetQuest(const std::string& id);
    std::vector<Quest*> GetAvailableQuests() const;
    std::vector<Quest*> GetActiveQuests() const;
    std::vector<Quest*> GetCompletedQuests() const;
    std::vector<Quest*> GetFailedQuests() const;
    
    // Meyer's Singleton - thread-safe in C++11 and beyond
    static QuestSystem* GetInstance() {
        // Thread-safe in C++11 and beyond
        static QuestSystem* instance = new QuestSystem();
        return instance;
    }
    
    // Cleanup method
    static void Destroy() {
        static QuestSystem* instance = GetInstance();
        delete instance;
        instance = nullptr;
    }
    
    ~QuestSystem();
    
private:
    // Private constructor
    QuestSystem();

    // Quest storage
    std::unordered_map<std::string, std::unique_ptr<Quest>> m_quests;
};
// ^ QuestSystem.h