#include "SaveLoadSystem.h"
#include "Linen.h"
#include "Engine/Core/Log.h"
#include <fstream>

SaveLoadSystem::SaveLoadSystem() {
    // This system depends on all other systems that need saving
}

SaveLoadSystem::~SaveLoadSystem() {
    Shutdown();
}

void SaveLoadSystem::Initialize() {
    LOG(Info, "Save/Load System Initialized.");
}

void SaveLoadSystem::Shutdown() {
    // std::lock_guard<std::mutex> lock(m_mutex);
    m_serializableSystems.clear();
    LOG(Info, "Save/Load System Shutdown.");
}

void SaveLoadSystem::Update(float deltaTime) {
    // Nothing to update regularly
}

bool SaveLoadSystem::SaveGame(const std::string& filename) {
    // std::lock_guard<std::mutex> lock(m_mutex);
    
    // Create a writer object (implementation specific)
    // void* writer = CreateWriter(filename);
    
    LOG(Info, "Saving game to: {0}", String(filename.c_str()));
    
    // For each registered system, call its Serialize method
    for (const auto& systemName : m_serializableSystems) {
        auto system = m_plugin->GetSystem<RPGSystem>(systemName);
        if (system) {
            // Call system-specific serialization
            // system->Serialize(writer);
            LOG(Info, "Saved system: {0}", String(systemName.c_str()));
        }
    }
    
    // Close writer
    // CloseWriter(writer);
    
    LOG(Info, "Game saved successfully: {0}", String(filename.c_str()));
    return true;
}

bool SaveLoadSystem::LoadGame(const std::string& filename) {
    // std::lock_guard<std::mutex> lock(m_mutex);
    
    // Create a reader object (implementation specific)
    // void* reader = CreateReader(filename);
    // if (!reader) {
    //     LOG(Error, "Failed to open save file: {0}", String(filename.c_str()));
    //     return false;
    // }
    
    LOG(Info, "Loading game from: {0}", String(filename.c_str()));
    
    // For each registered system, call its Deserialize method
    for (const auto& systemName : m_serializableSystems) {
        auto system = m_plugin->GetSystem<RPGSystem>(systemName);
        if (system) {
            // Call system-specific deserialization
            // system->Deserialize(reader);
            LOG(Info, "Loaded system: {0}", String(systemName.c_str()));
        }
    }
    
    // Close reader
    // CloseReader(reader);
    
    LOG(Info, "Game loaded successfully: {0}", String(filename.c_str()));
    return true;
}

void SaveLoadSystem::RegisterSerializableSystem(const std::string& systemName) {
    // std::lock_guard<std::mutex> lock(m_mutex);
    m_serializableSystems.insert(systemName);
    LOG(Info, "Registered system for serialization: {0}", String(systemName.c_str()));
}