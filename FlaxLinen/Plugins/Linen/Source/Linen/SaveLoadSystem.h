#pragma once

#include "RPGSystem.h"
#include <string>
#include <mutex>
#include <unordered_set>
#include <functional>

class SaveLoadSystem : public RPGSystem {
public:
    SaveLoadSystem();
    ~SaveLoadSystem();
    
    // RPGSystem interface
    void Initialize() override;
    void Shutdown() override;
    void Update(float deltaTime) override;
    std::string GetSystemName() const override { return "SaveLoadSystem"; }
    
    // Save/Load functionality
    bool SaveGame(const std::string& filename);
    bool LoadGame(const std::string& filename);
    
    // System registration for save/load
    void RegisterSerializableSystem(const std::string& systemName);
    
private:
    // Thread safety
    std::mutex m_mutex;
    
    // Track which systems need serialization
    std::unordered_set<std::string> m_serializableSystems;
};