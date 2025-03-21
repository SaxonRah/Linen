#include "CharacterProgressionSystem.h"
#include "Linen.h"
#include "Engine/Core/Log.h"

Skill::Skill(const std::string& id, const std::string& name, const std::string& description)
    : m_id(id)
    , m_name(name)
    , m_description(description)
    , m_level(0)
{
}

CharacterProgressionSystem::CharacterProgressionSystem() {
    // No dependencies for this system
}

CharacterProgressionSystem::~CharacterProgressionSystem() {
    Shutdown();
}

void CharacterProgressionSystem::Initialize() {
    // Subscribe to quest completed events
    m_plugin->GetEventSystem().Subscribe<QuestCompletedEvent>(
        [this](const QuestCompletedEvent& event) {
            this->HandleQuestCompleted(event);
        });
    
    LOG(Info, "Character Progression System Initialized.");
}

void CharacterProgressionSystem::Shutdown() {
    // std::lock_guard<std::mutex> lock(m_mutex);
    m_skills.clear();
    m_skillLevels.clear();
    LOG(Info, "Character Progression System Shutdown.");
}

void CharacterProgressionSystem::Update(float deltaTime) {
    // Nothing to update regularly
}

bool CharacterProgressionSystem::AddSkill(const std::string& id, const std::string& name, const std::string& description) {
    // std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_skills.find(id) != m_skills.end()) {
        LOG(Warning, "Skill already exists: {0}", String(id.c_str()));
        return false;
    }
    
    auto skill = std::make_unique<Skill>(id, name, description);
    m_skills[id] = std::move(skill);
    m_skillLevels[id] = 0;
    
    LOG(Info, "Added skill: {0}", String(name.c_str()));
    return true;
}

bool CharacterProgressionSystem::IncreaseSkill(const std::string& id, int amount) {
    // std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_skills.find(id);
    if (it == m_skills.end()) {
        LOG(Warning, "Skill not found: {0}", String(id.c_str()));
        return false;
    }
    
    it->second->IncreaseLevel(amount);
    m_skillLevels[id] = it->second->GetLevel();
    
    LOG(Info, "Increased skill {0} by {1} to level {2}", 
        String(id.c_str()), amount, it->second->GetLevel());
    return true;
}

int CharacterProgressionSystem::GetSkillLevel(const std::string& id) const {
    // std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_skills.find(id);
    if (it == m_skills.end()) {
        return 0;
    }
    
    return it->second->GetLevel();
}

void CharacterProgressionSystem::GainExperience(int amount) {
    // std::lock_guard<std::mutex> lock(m_mutex);
    
    int oldLevel = m_level;
    m_experience += amount;
    
    // Simple level up formula: level = sqrt(experience / 100)
    m_level = 1 + static_cast<int>(sqrt(m_experience / 100.0));
    
    LOG(Info, "Gained {0} XP. Total XP: {1}", amount, m_experience);
    
    if (m_level > oldLevel) {
        LOG(Info, "Level up! New level: {0}", m_level);
    }
}

int CharacterProgressionSystem::GetExperience() const {
    // std::lock_guard<std::mutex> lock(m_mutex);
    return m_experience;
}

int CharacterProgressionSystem::GetLevel() const {
    // std::lock_guard<std::mutex> lock(m_mutex);
    return m_level;
}

const std::unordered_map<std::string, int>& CharacterProgressionSystem::GetSkills() const {
    // No need to lock here as we're returning a const reference to the cached skill levels
    return m_skillLevels;
}

void CharacterProgressionSystem::HandleQuestCompleted(const QuestCompletedEvent& event) {
    if (event.experienceGained > 0) {
        GainExperience(event.experienceGained);
        LOG(Info, "Gained {0} XP from completed quest: {1}", 
            event.experienceGained, String(event.questTitle.c_str()));
    }
}

void CharacterProgressionSystem::Serialize(void* writer) const {
    // Implementation for save system
}

void CharacterProgressionSystem::Deserialize(void* reader) {
    // Implementation for load system
}