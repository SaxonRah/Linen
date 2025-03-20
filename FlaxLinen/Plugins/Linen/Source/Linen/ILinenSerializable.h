#pragma once

#include <memory>

// Forward declarations
class BinaryReader;
class BinaryWriter;

class ILinenSerializable {
public:
    virtual ~ILinenSerializable() = default;
    
    // Serialization methods
    virtual void Serialize(BinaryWriter& writer) const = 0;
    virtual void Deserialize(BinaryReader& reader) = 0;
};