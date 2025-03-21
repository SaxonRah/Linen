// v Linen.h
#pragma once

#include "Engine/Scripting/Plugins/GamePlugin.h"
#include "Engine/Core/Log.h"
#include "EventSystem.h"
#include "LinenSystem.h"

#include <string>
#include <unordered_set> 
#include <unordered_map>
#include <memory>
#include <vector>
#include <mutex>
#include <typeindex>
#include <functional>

// Forward declarations
class RPGSystem;
class TestSystem;
class CharacterProgressionSystem;
class QuestSystem;
class BinaryReader;
class BinaryWriter;

// Example test system
class TestSystem : public LinenSystem {
private:
    static TestSystem* s_instance;
    int _testValue;

    // Private constructor to prevent direct instantiation
    TestSystem() : _testValue(0) {}

public:
    // Delete copy constructor and assignment operator
    TestSystem(const TestSystem&) = delete;
    TestSystem& operator=(const TestSystem&) = delete;

    void Initialize() override {
        LOG(Info, "TestSystem Initialized");
        _testValue = 0;
    }
    
    void Shutdown() override {
        LOG(Info, "TestSystem Shutdown");
    }

    // Implement required abstract methods
    std::string GetName() const override { return "TestSystem"; }
    
    void Serialize(BinaryWriter& writer) const override {
        // Simple implementation
        // writer.Write(_testValue);
    }
    
    void Deserialize(BinaryReader& reader) override {
        // Simple implementation
        // reader.Read(_testValue);
    }

    // Cleanup method (important!)
    static void Destroy() {
        delete s_instance;
        s_instance = nullptr;
    }
    
    void Update(float deltaTime) override {}
    
    static TestSystem* GetInstance() {
        if (!s_instance) s_instance = new TestSystem();
        return s_instance;
    }

    bool AddValue(int value) {
        LOG(Info, "TestSystem::AddValue : starting with value: {0}", value);
        _testValue = value;
        LOG(Info, "TestSystem::AddValue : set value to: {0}", _testValue);
        return true;
    }
    
    int GetValue() const {
        LOG(Info, "TestSystem::GetValue : returning: {0}", _testValue);
        return _testValue;
    }
};

class Linen : public GamePlugin {
public:
    Linen(const SpawnParams& params);
    ~Linen();
    
    // Core plugin lifecycle
    void Initialize() override;
    void Deinitialize() override;
    void Update(float deltaTime);

    // System management
    template <typename T>
    bool RegisterSystem();
    
    template <typename T>
    bool LoadSystem();
    
    template <typename T>
    bool UnloadSystem();
    
    // Safe system access
    template <typename T>
    T* GetSystemOld();

    template <typename T>
    T* GetSystemOld(const std::string& systemName);
    
    // Thread-safe event system access
    EventSystem& GetEventSystem() { return m_eventSystem; }

    template <typename T>
    T* GetSystem() {
        LOG(Info, "Linen::GetSystem : Called for type: {0}", String(typeid(T).name()));

        if (std::is_same<T, TestSystem>::value) {
            LOG(Info, "Linen::GetSystem : Getting TestSystem instance");
            return reinterpret_cast<T*>(TestSystem::GetInstance());
        }

        if (std::is_same<T, CharacterProgressionSystem>::value) {
            LOG(Info, "Linen::GetSystem : Type match for CharacterProgressionSystem");
            return reinterpret_cast<T*>(CharacterProgressionSystem::GetInstance());
        }

        if (std::is_same<T, QuestSystem>::value) {
            LOG(Info, "Linen::GetSystem : Type match for QuestSystem");
            return reinterpret_cast<T*>(QuestSystem::GetInstance());
        }

        LOG(Warning, "Linen::GetSystem : No matching system found");
        return nullptr;
    }

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
bool Linen::RegisterSystem() {
    static_assert(std::is_base_of<RPGSystem, T>::value, "T must derive from RPGSystem");
    
    std::lock_guard<std::mutex> lock(m_systemsMutex);
    
    auto system = std::make_unique<T>();
    std::string systemName = system->GetName();
    
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
bool Linen::LoadSystem() {
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
bool Linen::UnloadSystem() {
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
T* Linen::GetSystemOld() {
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
T* Linen::GetSystemOld(const std::string& systemName) {
    static_assert(std::is_base_of<RPGSystem, T>::value, "T must derive from RPGSystem");

    std::lock_guard<std::mutex> lock(m_systemsMutex);

    auto it = m_activeSystems.find(systemName);
    if (it == m_activeSystems.end()) {
        return nullptr;
    }

    // Dynamic cast to ensure type safety
    return dynamic_cast<T*>(it->second);
}

// ^ Linen.h