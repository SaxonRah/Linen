// v CharacterProgressionSystem.h
#pragma once

#include "RPGSystem.h"
#include "QuestEvents.h"
#include "Serialization.h"

#include <unordered_map>
#include <string>
#include <mutex>
#include <vector>
#include <memory>

class Skill {
public:
    Skill(const std::string& id, const std::string& name, const std::string& description);
    
    std::string GetId() const { return m_id; }
    std::string GetName() const { return m_name; }
    std::string GetDescription() const { return m_description; }
    int GetLevel() const { return m_level; }
    
    void SetLevel(int level) { m_level = level; }
    void IncreaseLevel(int amount = 1) { m_level += amount; }
    
private:
    std::string m_id;
    std::string m_name;
    std::string m_description;
    int m_level = 0;
};

class CharacterProgressionSystem : public RPGSystem {
public:
    // Delete copy constructor and assignment operator
    CharacterProgressionSystem(const CharacterProgressionSystem&) = delete;
    CharacterProgressionSystem& operator=(const CharacterProgressionSystem&) = delete;

    // Static access method
    static CharacterProgressionSystem* GetInstance() {
        if (!s_instance) s_instance = new CharacterProgressionSystem();
        return s_instance;
    }

    // RPGSystem interface
    void Initialize() override;
    void Shutdown() override;
    void Update(float deltaTime) override;
    
    // Implement GetName from LinenSystem
    std::string GetName() const override { return "CharacterProgressionSystem"; }
    
    // Serialization override
    void Serialize(BinaryWriter& writer) const override;
    void Deserialize(BinaryReader& reader) override;
    
    // Skill management
    bool AddSkill(const std::string& id, const std::string& name, const std::string& description);
    bool IncreaseSkill(const std::string& id, int amount = 1);
    int GetSkillLevel(const std::string& id) const;
    
    // Experience management
    void GainExperience(int amount);
    int GetExperience() const;
    int GetLevel() const;
    
    // Requirements checking
    const std::unordered_map<std::string, int>& GetSkills() const;
    
    // Cleanup method
    static void Destroy() {
        delete s_instance;
        s_instance = nullptr;
    }
    
    ~CharacterProgressionSystem();

private:
    // Private constructor
    CharacterProgressionSystem();

    // Event handlers
    void HandleQuestCompleted(const QuestCompletedEvent& event);
    
    // Singleton Instance
    static CharacterProgressionSystem* s_instance;

    // Character data
    int m_experience = 0;
    int m_level = 1;
    std::unordered_map<std::string, std::unique_ptr<Skill>> m_skills;
    std::unordered_map<std::string, int> m_skillLevels; // Cache for requirements checking
};
// ^ CharacterProgressionSystem.h