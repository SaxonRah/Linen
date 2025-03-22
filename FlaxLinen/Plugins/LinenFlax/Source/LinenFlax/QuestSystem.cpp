// v QuestSystem.cpp
#include "QuestSystem.h"
#include "CharacterProgressionSystem.h"
#include "Linen.h"
#include "Engine/Core/Log.h"

// QuestSystem* QuestSystem::s_instance = nullptr;

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
    m_quests.clear();
    LOG(Info, "Quest System Shutdown.");
}

void QuestSystem::Update(float deltaTime) {
    // Check for time-based quest updates
}

QuestResult QuestSystem::AddQuest(const std::string& id, const std::string& title, const std::string& description) {
    
    if (m_quests.find(id) != m_quests.end()) {
        LOG(Warning, "Quest already exists: {0}", String(id.c_str()));
        return QuestResult::AlreadyExists;
    }
    
    try {
        auto quest = std::make_unique<Quest>(id, title, description);
        m_quests[id] = std::move(quest);
        
        LOG(Info, "Added quest: {0}", String(title.c_str()));
        return QuestResult::Success;
    }
    catch (const std::exception& e) {
        LOG(Error, "Failed to add quest: {0}. Error: {1}", 
            String(id.c_str()), String(e.what()));
        return QuestResult::Error;
    }
}

QuestResult QuestSystem::ActivateQuest(const std::string& id) {
    auto it = m_quests.find(id);
    if (it == m_quests.end()) {
        LOG(Warning, "Quest not found: {0}", String(id.c_str()));
        return QuestResult::NotFound;
    }

    Quest* quest = it->second.get();
    if (quest->GetState() != QuestState::Available) {
        LOG(Warning, "Quest not available: {0}", String(id.c_str()));
        return QuestResult::InvalidState;
    }

    // Check character progression requirements
    auto* progressionSystem = m_plugin->GetSystem<CharacterProgressionSystem>();

    if (progressionSystem) {
        if (!quest->CheckRequirements(progressionSystem->GetSkills())) {
            LOG(Info, "Character doesn't meet quest requirements: {0}", String(id.c_str()));
            return QuestResult::RequirementsNotMet;
        }
    }

    // Store old state and update to new state
    QuestState oldState = quest->GetState();
    quest->SetState(QuestState::Active);

    // Create and publish event
    QuestStateChangedEvent event;
    event.questId = id;
    event.questTitle = quest->GetTitle();
    event.oldState = oldState;
    event.newState = QuestState::Active;
    
    LOG(Info, "About to publish event...");
    if (m_plugin) {
        LOG(Info, "Plugin is not null");
        m_plugin->GetEventSystem().Publish(event);
    } else {
        LOG(Warning, "Cannot publish event: plugin reference is null");
    }
    
    LOG(Info, "Activated quest: {0}", String(id.c_str()));
    return QuestResult::Success;
}

QuestResult QuestSystem::CompleteQuest(const std::string& id) {
    std::string questTitle;
    int experienceReward = 0;
    QuestState oldState;
    bool success = false;
    
        
    auto it = m_quests.find(id);
    if (it == m_quests.end()) {
        LOG(Warning, "Quest not found: {0}", String(id.c_str()));
        return QuestResult::NotFound;
    }
    
    Quest* quest = it->second.get();
    if (quest->GetState() != QuestState::Active) {
        LOG(Warning, "Quest not active: {0}", String(id.c_str()));
        return QuestResult::InvalidState;
    }
    
    oldState = quest->GetState();
    questTitle = quest->GetTitle();
    experienceReward = quest->GetExperienceReward();
    
    quest->SetState(QuestState::Completed);
    success = true;
    
    if (success) {
        // Completion event
        QuestCompletedEvent completedEvent;
        completedEvent.questId = id;
        completedEvent.questTitle = questTitle;
        completedEvent.experienceGained = experienceReward;
        m_plugin->GetEventSystem().Publish(completedEvent);
        
        // State change event 
        QuestStateChangedEvent stateEvent;
        stateEvent.questId = id;
        stateEvent.questTitle = questTitle;
        stateEvent.oldState = oldState;
        stateEvent.newState = QuestState::Completed;
        m_plugin->GetEventSystem().Publish(stateEvent);

        LOG(Info, "Completed quest: {0}", String(id.c_str()));
        return QuestResult::Success;
    }
    
    return QuestResult::Error;
}

QuestResult QuestSystem::FailQuest(const std::string& id) {
    std::string questTitle;
    QuestState oldState;
    bool success = false;
    
    auto it = m_quests.find(id);
    if (it == m_quests.end()) {
        LOG(Warning, "Quest not found: {0}", String(id.c_str()));
        return QuestResult::NotFound;
    }
    
    Quest* quest = it->second.get();
    if (quest->GetState() != QuestState::Active) {
        LOG(Warning, "Quest not active: {0}", String(id.c_str()));
        return QuestResult::InvalidState;
    }
    
    oldState = quest->GetState();
    questTitle = quest->GetTitle();
    quest->SetState(QuestState::Failed);
    success = true;

    if (success) {
        QuestStateChangedEvent event;
        event.questId = id;
        event.questTitle = questTitle;
        event.oldState = oldState;
        event.newState = QuestState::Failed;
        m_plugin->GetEventSystem().Publish(event);

        LOG(Info, "Failed quest: {0}", String(id.c_str()));
        return QuestResult::Success;
    }
    
    return QuestResult::Error;
}

Quest* QuestSystem::GetQuest(const std::string& id) {    
    auto it = m_quests.find(id);
    if (it == m_quests.end()) { return nullptr; }
    return it->second.get();
}

std::vector<Quest*> QuestSystem::GetAvailableQuests() const {
    std::vector<Quest*> result;
    for (const auto& pair : m_quests) {
        if (pair.second->GetState() == QuestState::Available) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

std::vector<Quest*> QuestSystem::GetActiveQuests() const {
    std::vector<Quest*> result;
    for (const auto& pair : m_quests) {
        if (pair.second->GetState() == QuestState::Active) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

std::vector<Quest*> QuestSystem::GetCompletedQuests() const {
    std::vector<Quest*> result;
    for (const auto& pair : m_quests) {
        if (pair.second->GetState() == QuestState::Completed) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

void QuestSystem::Serialize(BinaryWriter& writer) const {
    // Implementation
}

void QuestSystem::Deserialize(BinaryReader& reader) {
    // Implementation
}
// ^ QuestSystem.cpp