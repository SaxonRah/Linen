// v Linen.h
#pragma once

#include "Engine/Scripting/Plugins/GamePlugin.h"
#include "Engine/Core/Log.h"
#include <string>
#include <unordered_map>
#include <memory>

// Simple base system class
class LinenSystem {
public:
    virtual ~LinenSystem() = default;
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual void Update(float deltaTime) {}
    virtual std::string GetName() const = 0;
};

// Forward declaration
class Linen;

// Example test system
class TestSystem : public LinenSystem {
private:
    static TestSystem* s_instance;
    // Use a static variable instead of instance variable
    // static int s_testValue;
    int _testValue;

    // Private constructor to prevent direct instantiation
    TestSystem() : _testValue(0) {}

public:

    // Delete copy constructor and assignment operator
    TestSystem(const TestSystem&) = delete;
    TestSystem& operator=(const TestSystem&) = delete;

    void Initialize() override {
        LOG(Info, "TestSystem Initialized");
        // Static initialization instead of member variable
        // s_testValue = 0;
        // member var
        _testValue = 0;
    }
    
    void Shutdown() override {
        LOG(Info, "TestSystem Shutdown");
        // Reset the static value on shutdown
        // s_testValue = 0;
    }

    // Cleanup method (important!)
    static void Destroy() {
        delete s_instance;
        s_instance = nullptr;
    }
    
    void Update(float deltaTime) override {}
    
    std::string GetName() const override { return "TestSystem"; }

    static TestSystem* GetInstance() {
        if (!s_instance) s_instance = new TestSystem();
        return s_instance;
    }

    // Static methods that don't rely on instance variables
    bool AddValue(int value) {
        LOG(Info, "TestSystem::AddValue : starting with value: {0}", value);
        
        // Use static variable instead of member variable
        // s_testValue = value;
        // member var 
        _testValue = value;

        // LOG(Info, "TestSystem::AddValue : set value to: {0}", s_testValue);
        LOG(Info, "TestSystem::AddValue : set value to: {0}", _testValue);
        return true;
    }
    
    int GetValue() const {
        // LOG(Info, "TestSystem::GetValue : returning: {0}", s_testValue);
        // return s_testValue;
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
    
    // Get system (simplified)
    template <typename T>
    T* GetSystem() {
        LOG(Info, "Linen::GetSystem : Called for type: {0}", String(typeid(T).name()));

        // For TestSystem, use the singleton pattern
        if (std::is_same<T, TestSystem>::value) {
            LOG(Info, "Linen::GetSystem : Getting TestSystem instance");
            return static_cast<T*>(TestSystem::GetInstance());
        }

        LOG(Warning, "Linen::GetSystem : No matching system found");
        return nullptr;
    }
};
// ^ Linen.h