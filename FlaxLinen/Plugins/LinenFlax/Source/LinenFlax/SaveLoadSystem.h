// v SaveLoadSystem.h
#pragma once

#include "RPGSystem.h"
#include <string>
#include <mutex>
#include <unordered_set>
#include <functional>


class SaveLoadSystem : public RPGSystem {
public:
    
    // Delete copy constructor and assignment operator
    SaveLoadSystem(const SaveLoadSystem&) = delete;
    SaveLoadSystem& operator=(const SaveLoadSystem&) = delete;

    // RPGSystem interface
    void Initialize() override;
    void Shutdown() override;
    void Update(float deltaTime) override;

    std::string GetName() const override { return "SaveLoadSystem"; }
    
    // Save/Load functionality
    bool SaveGame(const std::string& filename);
    bool LoadGame(const std::string& filename);
    
    // System registration for save/load
    void RegisterSerializableSystem(const std::string& systemName);
    
    // Default serialization - can be overridden by systems
    virtual void Serialize(BinaryWriter& writer) const override {}
    virtual void Deserialize(BinaryReader& reader) override {}
    
    // Static access method
    static SaveLoadSystem* GetInstance() {
        if (!s_instance) s_instance = new SaveLoadSystem();
        return s_instance;
    }

    // Cleanup method
    static void Destroy() {
        delete s_instance;
        s_instance = nullptr;
    }
    
    ~SaveLoadSystem() {
        Shutdown();
    }

private:
    SaveLoadSystem() {};
    
    // Singleton Instance
    static SaveLoadSystem* s_instance;

    // Thread safety
    // std::mutex m_mutex;
    
    // Track which systems need serialization
    std::unordered_set<std::string> m_serializableSystems;
};
// ^ SaveLoadSystem.h