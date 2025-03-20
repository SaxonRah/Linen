#pragma once

#include "Engine/Scripting/Plugins/GamePlugin.h"
#include "RPGSystem.h"
#include "EventSystem.h"

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <typeindex>

// API_CLASS(Namespace = "LINEN") class LinenPlugin : public GamePlugin {
class LinenPlugin : public GamePlugin {

    // DECLARE_SCRIPTING_TYPE(LINEN);

public:
    LinenPlugin(const SpawnParams& params);
    ~LinenPlugin();
    
    // Core plugin lifecycle
    virtual void Initialize() override;
    virtual void Deinitialize() override;
    virtual void Update(float deltaTime);
    
    // System management
    template <typename T>
    bool RegisterSystem();
    
    template <typename T>
    bool LoadSystem();
    
    template <typename T>
    bool UnloadSystem();
    
    // Safe system access
    template <typename T>
    T* GetSystem();

    template <typename T>
    T* GetSystem(const std::string& systemName);
    
    // Thread-safe event system access
    EventSystem& GetEventSystem() { return m_eventSystem; }
    
private:
    // Determines correct initialization order based on dependencies
    bool DetectCycle(const std::string& systemName, 
        std::unordered_set<std::string>& visited, 
        std::unordered_set<std::string>& recursionStack);
    void CalculateInitializationOrder();
    
    // Organizes systems by dependencies
    std::vector<std::string> m_initializationOrder;
    
    // Maps for system management
    std::unordered_map<std::string, std::unique_ptr<RPGSystem>> m_registeredSystems;
    std::unordered_map<std::string, RPGSystem*> m_activeSystems;
    std::unordered_map<std::type_index, std::string> m_typeToName;
    
    // Thread safety
    std::mutex m_systemsMutex;
    
    // Centralized event system
    EventSystem m_eventSystem;
};

// Template implementations
template <typename T>
bool LinenPlugin::RegisterSystem() {
    static_assert(std::is_base_of<RPGSystem, T>::value, "T must derive from RPGSystem");
    
    std::lock_guard<std::mutex> lock(m_systemsMutex);
    
    auto system = std::make_unique<T>();
    std::string systemName = system->GetSystemName();
    
    if (m_registeredSystems.find(systemName) != m_registeredSystems.end()) {
        LOG(Warning, "System already registered: {0}", String(systemName.c_str()));
        return false;
    }
    
    // Store type information for safer access
    m_typeToName[std::type_index(typeid(T))] = systemName;
    
    // Set plugin reference and store system
    system->SetPlugin(this);
    m_registeredSystems[systemName] = std::move(system);
    
    // Recalculate initialization order
    CalculateInitializationOrder();
    
    LOG(Info, "Registered system: {0}", String(systemName.c_str()));
    return true;
}

template <typename T>
bool LinenPlugin::LoadSystem() {
    static_assert(std::is_base_of<RPGSystem, T>::value, "T must derive from RPGSystem");
    
    std::lock_guard<std::mutex> lock(m_systemsMutex);
    
    auto typeIndex = std::type_index(typeid(T));
    
    // Check if we know about this type
    if (m_typeToName.find(typeIndex) == m_typeToName.end()) {
        LOG(Warning, "System not registered: {0}", String(typeid(T).name()));
        return false;
    }
    
    std::string systemName = m_typeToName[typeIndex];
    
    // Check if already loaded
    if (m_activeSystems.find(systemName) != m_activeSystems.end()) {
        LOG(Info, "System already loaded: {0}", String(systemName.c_str()));
        return true;
    }
    
    // Check if system is registered
    if (m_registeredSystems.find(systemName) == m_registeredSystems.end()) {
        LOG(Warning, "System not registered: {0}", String(systemName.c_str()));
        return false;
    }
    
    // First load dependencies
    auto& system = m_registeredSystems[systemName];
    for (const auto& dependency : system->GetDependencies()) {
        if (m_activeSystems.find(dependency) == m_activeSystems.end()) {
            LOG(Info, "Loading dependency: {0} for {1}", 
                String(dependency.c_str()), String(systemName.c_str()));
            
            // Find and load the dependency
            if (m_registeredSystems.find(dependency) != m_registeredSystems.end()) {
                m_registeredSystems[dependency]->Initialize();
                m_activeSystems[dependency] = m_registeredSystems[dependency].get();
            } else {
                LOG(Warning, "Missing dependency: {0}", String(dependency.c_str()));
                return false;
            }
        }
    }
    
    // Initialize the system
    system->Initialize();
    m_activeSystems[systemName] = system.get();
    
    LOG(Info, "Loaded system: {0}", String(systemName.c_str()));
    return true;
}

template <typename T>
bool LinenPlugin::UnloadSystem() {
    static_assert(std::is_base_of<RPGSystem, T>::value, "T must derive from RPGSystem");
    
    std::lock_guard<std::mutex> lock(m_systemsMutex);
    
    auto typeIndex = std::type_index(typeid(T));
    if (m_typeToName.find(typeIndex) == m_typeToName.end()) {
        LOG(Warning, "System not registered: {0}", String(typeid(T).name()));
        return false;
    }
    
    std::string systemName = m_typeToName[typeIndex];
    
    // Check if system is active
    auto it = m_activeSystems.find(systemName);
    if (it == m_activeSystems.end()) {
        LOG(Info, "System not active: {0}", String(systemName.c_str()));
        return true;
    }
    
    // Check for dependent systems
    for (const auto& pair : m_activeSystems) {
        auto& activeSystem = m_registeredSystems[pair.first];
        if (activeSystem->GetDependencies().find(systemName) != activeSystem->GetDependencies().end()) {
            LOG(Warning, "Cannot unload {0}, it is a dependency of {1}",
                String(systemName.c_str()), String(pair.first.c_str()));
            return false;
        }
    }
    
    // Shutdown the system
    m_registeredSystems[systemName]->Shutdown();
    m_activeSystems.erase(it);
    
    LOG(Info, "Unloaded system: {0}", String(systemName.c_str()));
    return true;
}

template <typename T>
T* LinenPlugin::GetSystem() {
    static_assert(std::is_base_of<RPGSystem, T>::value, "T must derive from RPGSystem");
    
    std::lock_guard<std::mutex> lock(m_systemsMutex);
    
    auto typeIndex = std::type_index(typeid(T));
    if (m_typeToName.find(typeIndex) == m_typeToName.end()) {
        return nullptr;
    }
    
    std::string systemName = m_typeToName[typeIndex];
    auto it = m_activeSystems.find(systemName);
    if (it == m_activeSystems.end()) {
        return nullptr;
    }
    
    return static_cast<T*>(it->second);
}

template <typename T>
T* LinenPlugin::GetSystem(const std::string& systemName) {
    static_assert(std::is_base_of<RPGSystem, T>::value, "T must derive from RPGSystem");

    std::lock_guard<std::mutex> lock(m_systemsMutex);

    auto it = m_activeSystems.find(systemName);
    if (it == m_activeSystems.end()) {
        return nullptr;
    }

    // Dynamic cast to ensure type safety
    return dynamic_cast<T*>(it->second);
}