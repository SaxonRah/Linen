#include "LinenPlugin.h"
#include "Engine/Core/Log.h"
#include <queue>

LinenPlugin::LinenPlugin(const SpawnParams& params)
    : GamePlugin(params)
{
    _description.Name = TEXT("Linen");
#if USE_EDITOR
    _description.Category = TEXT("Gameplay");
    _description.Description = TEXT("Linen is a dynamic and modular RPG Systems manager.");
    _description.Author = TEXT("ParabolicLabs");
    _description.RepositoryUrl = TEXT("");
#endif
    _description.Version = Version(3, 1, 0);
}

LinenPlugin::~LinenPlugin() {
    Deinitialize();
}

void LinenPlugin::Initialize() {
    Plugin::Initialize();
    LOG(Info, "Linen Plugin Initialized.");
}

void LinenPlugin::Deinitialize() {
    std::lock_guard<std::mutex> lock(m_systemsMutex);
    
    // Unload systems in reverse dependency order
    for (auto it = m_initializationOrder.rbegin(); it != m_initializationOrder.rend(); ++it) {
        auto systemIt = m_activeSystems.find(*it);
        if (systemIt != m_activeSystems.end()) {
            LOG(Info, "Shutting down system: {0}", String(systemIt->first.c_str()));
            m_registeredSystems[systemIt->first]->Shutdown();
        }
    }
    
    m_activeSystems.clear();
    m_registeredSystems.clear();
    m_typeToName.clear();
    m_initializationOrder.clear();
    
    LOG(Info, "Linen Plugin Deinitialized.");
}

void LinenPlugin::Update(float deltaTime) {
    // Update all active systems in initialization order
    for (const auto& systemName : m_initializationOrder) {
        auto it = m_activeSystems.find(systemName);
        if (it != m_activeSystems.end()) {
            it->second->Update(deltaTime);
        }
    }
    
    // Process events after all systems have updated
    m_eventSystem.ProcessEvents();
}

bool LinenPlugin::DetectCycle(const std::string& systemName, 
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

void LinenPlugin::CalculateInitializationOrder() {
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

template RPGSystem* LinenPlugin::GetSystem<RPGSystem>(const std::string&);