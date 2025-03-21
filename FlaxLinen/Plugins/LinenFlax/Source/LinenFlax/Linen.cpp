// v Linen.cpp
#include "Linen.h"
#include "Engine/Core/Log.h"

// int TestSystem::s_testValue = 0;
TestSystem* TestSystem::s_instance = nullptr;

Linen::Linen(const SpawnParams& params)
    : GamePlugin(params)
{
    _description.Name = TEXT("Linen");
#if USE_EDITOR
    _description.Category = TEXT("Gameplay");
    _description.Description = TEXT("Linen plugin");
    _description.Author = TEXT("ParabolicLabs");
    _description.RepositoryUrl = TEXT("");
#endif
    _description.Version = Version(1, 0, 0);
}

Linen::~Linen() {
    Deinitialize();
}

void Linen::Initialize() {
    GamePlugin::Initialize();
    
    LOG(Info, "Linen::Initialize : ran");
}

void Linen::Deinitialize() {
    // Shutdown test system
    TestSystem::Destroy();    
    LOG(Info, "Linen::Deinitialize : ran");
}

void Linen::Update(float deltaTime) {
    // Get the singleton instance and update it if it exists
    TestSystem* testSystem = TestSystem::GetInstance();
    if (testSystem) {
        try {
            testSystem->Update(deltaTime);
        }
        catch (...) {
            LOG(Error, "Linen::Update : Error updating TestSystem");
        }
    }
}
// ^ Linen.cpp