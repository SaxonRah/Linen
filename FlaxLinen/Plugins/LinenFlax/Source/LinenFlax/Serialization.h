// v Serialization.h

#pragma once

#include "LinenSystem.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstdint>

class BinaryWriter {
public:
    BinaryWriter(const std::string& filename) : m_stream(filename, std::ios::binary | std::ios::out) {}
    ~BinaryWriter() { m_stream.close(); }
    
    bool IsValid() const { return m_stream.good(); }
    
    // Write primitives
    void Write(bool value) { Write(&value, sizeof(bool)); }
    void Write(int32_t value) { Write(&value, sizeof(int32_t)); }
    void Write(uint32_t value) { Write(&value, sizeof(uint32_t)); }
    void Write(float value) { Write(&value, sizeof(float)); }
    void Write(double value) { Write(&value, sizeof(double)); }
    
    // Write string
    void Write(const std::string& value) {
        uint32_t length = static_cast<uint32_t>(value.length());
        Write(length);
        if (length > 0) {
            Write(value.data(), length);
        }
    }
    
    // Write raw data
    void Write(const void* data, size_t size) {
        m_stream.write(static_cast<const char*>(data), size);
    }
    
    // Write container helpers
    template<typename T>
    void WriteVector(const std::vector<T>& vec) {
        uint32_t size = static_cast<uint32_t>(vec.size());
        Write(size);
        for (const auto& item : vec) {
            Write(item);
        }
    }
    
    // Write vector of serializable objects
    template<typename T>
    void WriteSerializableVector(const std::vector<std::unique_ptr<T>>& vec) {
        uint32_t size = static_cast<uint32_t>(vec.size());
        Write(size);
        for (const auto& item : vec) {
            item->Serialize(*this);
        }
    }
    
    // Write map
    template<typename K, typename V>
    void WriteMap(const std::unordered_map<K, V>& map) {
        uint32_t size = static_cast<uint32_t>(map.size());
        Write(size);
        for (const auto& pair : map) {
            Write(pair.first);
            Write(pair.second);
        }
    }
    
private:
    std::ofstream m_stream;
};

class BinaryReader {
public:
    BinaryReader(const std::string& filename) : m_stream(filename, std::ios::binary | std::ios::in) {}
    ~BinaryReader() { m_stream.close(); }
    
    bool IsValid() const { return m_stream.good() && !m_stream.eof(); }
    
    // Read primitives
    void Read(bool& value) { Read(&value, sizeof(bool)); }
    void Read(int32_t& value) { Read(&value, sizeof(int32_t)); }
    void Read(uint32_t& value) { Read(&value, sizeof(uint32_t)); }
    void Read(float& value) { Read(&value, sizeof(float)); }
    void Read(double& value) { Read(&value, sizeof(double)); }
    
    // Read string
    void Read(std::string& value) {
        uint32_t length = 0;
        Read(length);
        value.resize(length);
        if (length > 0) {
            Read(&value[0], length);
        }
    }
    
    // Read raw data
    void Read(void* data, size_t size) {
        m_stream.read(static_cast<char*>(data), size);
    }
    
    // Read container helpers
    template<typename T>
    void ReadVector(std::vector<T>& vec) {
        uint32_t size = 0;
        Read(size);
        vec.resize(size);
        for (uint32_t i = 0; i < size; ++i) {
            Read(vec[i]);
        }
    }
    
    // Read vector of serializable objects
    template<typename T>
    void ReadSerializableVector(std::vector<std::unique_ptr<T>>& vec) {
        uint32_t size = 0;
        Read(size);
        vec.clear();
        for (uint32_t i = 0; i < size; ++i) {
            auto item = std::make_unique<T>();
            item->Deserialize(*this);
            vec.push_back(std::move(item));
        }
    }
    
    // Read map
    template<typename K, typename V>
    void ReadMap(std::unordered_map<K, V>& map) {
        uint32_t size = 0;
        Read(size);
        map.clear();
        K key;
        V value;
        for (uint32_t i = 0; i < size; ++i) {
            Read(key);
            Read(value);
            map[key] = value;
        }
    }
    
private:
    std::ifstream m_stream;
};
// ^ Serialization.h