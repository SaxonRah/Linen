#include "QuestSystem.h"
#include "LinenPlugin.h"
#include "CharacterProgressionSystem.h"
#include "Engine/Core/Log.h"

Quest::Quest(const std::string& id, const std::string& title, const std::string& description)
    : m_id(id)
    , m_title(title)
    , m_description(description)
    , m_state(QuestState::Available)
    , m_experienceReward(0)
{
}

void Quest::AddSkillRequirement(const std::string& skillName, int requiredLevel) {
    m_skillRequirements[skillName] = requiredLevel;
}

bool Quest::CheckRequirements(const std::unordered_map<std::string, int>& playerSkills) const {
    for (const auto& req : m_skillRequirements) {
        auto it = playerSkills.find(req.first);
        if (it == playerSkills.end() || it->second < req.second) {
            return false;
        }
    }
    return true;
}

void Quest::Serialize(void* writer) const {
    // Serialization implementation
}

void Quest::Deserialize(void* reader) {
    // Deserialization implementation
}

QuestSystem::QuestSystem() {
    // Define system dependencies
    m_dependencies.insert("CharacterProgressionSystem");
}

QuestSystem::~QuestSystem() {
    Shutdown();
}

void QuestSystem::Initialize() {
    LOG(Info, "Quest System Initialized.");
}

void QuestSystem::Shutdown() {
    std::lock_guard<std::mutex> lock(m_questsMutex);
    m_quests.clear();
    LOG(Info, "Quest System Shutdown.");
}

void QuestSystem::Update(float deltaTime) {
    // Check for time-based quest updates
}

bool QuestSystem::AddQuest(const std::string& id, const std::string& title, const std::string& description) {
    std::lock_guard<std::mutex> lock(m_questsMutex);
    
    if (m_quests.find(id) != m_quests.end()) {
        LOG(Warning, "Quest already exists: {0}", String(id.c_str()));
        return false;
    }
    
    auto quest = std::make_unique<Quest>(id, title, description);
    m_quests[id] = std::move(quest);
    
    LOG(Info, "Added quest: {0}", String(title.c_str()));
    return true;
}

bool QuestSystem::ActivateQuest(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_questsMutex);
    
    auto it = m_quests.find(id);
    if (it == m_quests.end()) {
        LOG(Warning, "Quest not found: {0}", String(id.c_str()));
        return false;
    }
    
    Quest* quest = it->second.get();
    if (quest->GetState() != QuestState::Available) {
        LOG(Warning, "Quest not available: {0}", String(id.c_str()));
        return false;
    }
    
    // Check character progression requirements
    auto progressionSystem = m_plugin->GetSystem<CharacterProgressionSystem>();
    if (progressionSystem) {
        if (!quest->CheckRequirements(progressionSystem->GetSkills())) {
            LOG(Info, "Character doesn't meet quest requirements: {0}", String(id.c_str()));
            return false;
        }
    }
    
    QuestState oldState = quest->GetState();
    quest->SetState(QuestState::Active);
    
    // Publish event without lock held
    m_questsMutex.unlock();
    PublishQuestStateChanged(quest, oldState);
    m_questsMutex.lock();
    
    LOG(Info, "Activated quest: {0}", String(id.c_str()));
    return true;
}

bool QuestSystem::CompleteQuest(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_questsMutex);
    
    auto it = m_quests.find(id);
    if (it == m_quests.end()) {
        LOG(Warning, "Quest not found: {0}", String(id.c_str()));
        return false;
    }
    
    Quest* quest = it->second.get();
    if (quest->GetState() != QuestState::Active) {
        LOG(Warning, "Quest not active: {0}", String(id.c_str()));
        return false;
    }
    
    QuestState oldState = quest->GetState();
    quest->SetState(QuestState::Completed);
    
    // Release lock before event publishing to prevent deadlocks
    m_questsMutex.unlock();
    
    // Publish completion event
    QuestCompletedEvent event;
    event.questId = id;
    event.questTitle = quest->GetTitle();
    event.experienceGained = quest->GetExperienceReward();
    
    m_plugin->GetEventSystem().Publish(event);
    
    // Publish state change event
    PublishQuestStateChanged(quest, oldState);
    
    // Reacquire lock
    m_questsMutex.lock();
    
    LOG(Info, "Completed quest: {0}", String(id.c_str()));
    return true;
}

bool QuestSystem::FailQuest(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_questsMutex);
    
    auto it = m_quests.find(id);
    if (it == m_quests.end()) {
        LOG(Warning, "Quest not found: {0}", String(id.c_str()));
        return false;
    }
    
    Quest* quest = it->second.get();
    if (quest->GetState() != QuestState::Active) {
        LOG(Warning, "Quest not active: {0}", String(id.c_str()));
        return false;
    }
    
    QuestState oldState = quest->GetState();
    quest->SetState(QuestState::Failed);
    
    // Publish event without lock held
    m_questsMutex.unlock();
    PublishQuestStateChanged(quest, oldState);
    m_questsMutex.lock();
    
    LOG(Info, "Failed quest: {0}", String(id.c_str()));
    return true;
}

Quest* QuestSystem::GetQuest(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_questsMutex);
    
    auto it = m_quests.find(id);
    if (it == m_quests.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

std::vector<Quest*> QuestSystem::GetAvailableQuests() const {
    std::lock_guard<std::mutex> lock(m_questsMutex);
    
    std::vector<Quest*> result;
    for (const auto& pair : m_quests) {
        if (pair.second->GetState() == QuestState::Available) {
            result.push_back(pair.second.get());
        }
    }
    
    return result;
}

std::vector<Quest*> QuestSystem::GetActiveQuests() const {
    std::lock_guard<std::mutex> lock(m_questsMutex);
    
    std::vector<Quest*> result;
    for (const auto& pair : m_quests) {
        if (pair.second->GetState() == QuestState::Active) {
            result.push_back(pair.second.get());
        }
    }
    
    return result;
}

std::vector<Quest*> QuestSystem::GetCompletedQuests() const {
    std::lock_guard<std::mutex> lock(m_questsMutex);
    
    std::vector<Quest*> result;
    for (const auto& pair : m_quests) {
        if (pair.second->GetState() == QuestState::Completed) {
            result.push_back(pair.second.get());
        }
    }
    
    return result;
}

void QuestSystem::PublishQuestStateChanged(Quest* quest, QuestState oldState) {
    QuestStateChangedEvent event;
    event.questId = quest->GetId();
    event.questTitle = quest->GetTitle();
    event.oldState = oldState;
    event.newState = quest->GetState();
    
    m_plugin->GetEventSystem().Publish(event);
}

void QuestSystem::Serialize(void* writer) const {
    // Implementation for save system
}

void QuestSystem::Deserialize(void* reader) {
    // Implementation for load system
}