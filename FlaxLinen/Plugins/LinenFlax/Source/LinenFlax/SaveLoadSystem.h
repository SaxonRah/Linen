// v SaveLoadSystem.h
#pragma once

#include "RPGSystem.h"
#include <string>
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
    
     // Meyer's Singleton - thread-safe in C++11 and beyond
     static SaveLoadSystem* GetInstance() {
        // Thread-safe in C++11 and beyond
        static SaveLoadSystem* instance = new SaveLoadSystem();
        return instance;
    }

    // Cleanup method
    static void Destroy() {
        static SaveLoadSystem* instance = GetInstance();
        delete instance;
        instance = nullptr;
    }
    
    ~SaveLoadSystem();

private:
    SaveLoadSystem() {};
    
    // Track which systems need serialization
    std::unordered_set<std::string> m_serializableSystems;
};
// ^ SaveLoadSystem.h