// v Linen.cpp
#include "Linen.h"
#include "LinenSystemIncludes.h" // Include all systems
#include "Engine/Core/Log.h"

// int TestSystem::s_testValue = 0;
TestSystem* TestSystem::s_instance = nullptr;

Linen::Linen(const SpawnParams& params)
    : GamePlugin(params)
{
    _description.Name = TEXT("Linen");
#if USE_EDITOR
    _description.Category = TEXT("Gameplay");
    _description.Description = TEXT("Linen plugin");
    _description.Author = TEXT("ParabolicLabs");
    _description.RepositoryUrl = TEXT("");
#endif
    _description.Version = Version(1, 0, 0);
}

Linen::~Linen() {
    Deinitialize();
}

void Linen::Initialize() {
    GamePlugin::Initialize();
    
    LOG(Info, "Linen::Initialize : ran");

    // Set plugin references first
    TestSystem::GetInstance()->SetPlugin(this);
    CharacterProgressionSystem::GetInstance()->SetPlugin(this);
    QuestSystem::GetInstance()->SetPlugin(this);
    
    // Then initialize systems in dependency order
    TestSystem::GetInstance()->Initialize();
    CharacterProgressionSystem::GetInstance()->Initialize();
    QuestSystem::GetInstance()->Initialize();
    
    LOG(Info, "All Linen RPG Systems initialized");
}

void Linen::Deinitialize() {
    LOG(Info, "Linen::Deinitialize : ran");
    
    // Shutdown systems in reverse order
    QuestSystem::GetInstance()->Shutdown();
    CharacterProgressionSystem::GetInstance()->Shutdown();
    TestSystem::GetInstance()->Shutdown();
    
    // Then destroy instances
    QuestSystem::Destroy();
    CharacterProgressionSystem::Destroy();
    TestSystem::Destroy();
    
    LOG(Info, "Linen Plugin Deinitialized.");
}

void Linen::Update(float deltaTime) {
    // Update all singleton systems
    TestSystem::GetInstance()->Update(deltaTime);
    CharacterProgressionSystem::GetInstance()->Update(deltaTime);
    QuestSystem::GetInstance()->Update(deltaTime);
    
    // Process events after all systems have updated
    m_eventSystem.ProcessEvents();
}

bool Linen::DetectCycle(const std::string& systemName, 
    std::unordered_set<std::string>& visited, 
    std::unordered_set<std::string>& recursionStack) {
    if (recursionStack.count(systemName)) return true;  // Cycle detected
    if (visited.count(systemName)) return false;

    visited.insert(systemName);
    recursionStack.insert(systemName);

    for (const auto& dependency : m_registeredSystems[systemName]->GetDependencies()) {
    if (DetectCycle(dependency, visited, recursionStack)) return true;
    }

    recursionStack.erase(systemName);
    return false;
}

void Linen::CalculateInitializationOrder() {
    m_initializationOrder.clear();
    
    // Topological sort of systems based on dependencies
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> inProgress;
    std::unordered_set<std::string> recursionStack;

    for (const auto& pair : m_registeredSystems) {
        if (DetectCycle(pair.first, visited, recursionStack)) {
            LOG(Error, "Cyclic dependency detected in system: {0}", String(pair.first.c_str()));
            return;
        }
    }

    std::function<void(const std::string&)> visit = [&](const std::string& systemName) {
        if (inProgress.find(systemName) != inProgress.end()) {
            LOG(Error, "Circular dependency detected for system: {0}", String(systemName.c_str()));
            return;
        }
        
        if (visited.find(systemName) != visited.end()) {
            return;
        }
        
        inProgress.insert(systemName);
        
        auto it = m_registeredSystems.find(systemName);
        if (it != m_registeredSystems.end()) {
            for (const auto& dep : it->second->GetDependencies()) {
                visit(dep);
            }
        }
        
        inProgress.erase(systemName);
        visited.insert(systemName);
        m_initializationOrder.push_back(systemName);
    };
    
    // Visit all registered systems
    for (const auto& pair : m_registeredSystems) {
        visit(pair.first);
    }
}
// ^ Linen.cpp