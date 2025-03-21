#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "ILinenSerializable.h"

// Forward declarations
class Linen;
class BinaryReader;
class BinaryWriter;

// Base class for all RPG systems
class RPGSystem : public ILinenSerializable {
public:
    virtual ~RPGSystem() = default;
    
    // Core lifecycle methods
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual void Update(float deltaTime) {}
    
    // System dependencies
    const std::unordered_set<std::string>& GetDependencies() const { return m_dependencies; }
    
    // System identification
    virtual std::string GetSystemName() const = 0;
    
    // Plugin reference for accessing other systems
    void SetPlugin(Linen* plugin) { m_plugin = plugin; }
    
    // Default serialization - can be overridden by systems
    virtual void Serialize(BinaryWriter& writer) const override {}
    virtual void Deserialize(BinaryReader& reader) override {}

protected:
    Linen* m_plugin = nullptr;
    std::unordered_set<std::string> m_dependencies;
};