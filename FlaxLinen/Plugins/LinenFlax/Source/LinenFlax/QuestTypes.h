// v QuestTypes.h
#pragma once

// Forward declarations and common types for Quest-related classes
class Quest;

// Quest state enum that can be used by multiple files
enum class QuestState {
    Available,
    Active, 
    Completed,
    Failed
};
// ^ QuestTypes.h