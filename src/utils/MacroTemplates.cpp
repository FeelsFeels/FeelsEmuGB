#pragma once
#include <type_traits>

#pragma region SCRIPTING

// TODO: dllexport is not compatible with android. To get scripting working, we have to find another way, or fallback to the old scriptglue implementation...
#ifdef __ANDROID__
#define SCRIPT_CALLABLE
#else
#define SCRIPT_CALLABLE extern "C" __declspec(dllexport)
#endif

#define SCRIPT_CALLABLE_COMP_SETTER(CompType, CallableFuncName, ArgType, SetterFuncName) \
SCRIPT_CALLABLE void CallableFuncName(ecs::EntityHandle entity, ArgType arg) \
{ \
	util::AssertCompExistsOnValidEntityAndGet<CompType>(entity)->SetterFuncName(arg); \
}

#pragma endregion // SCRIPTING

#pragma region ENUM

#define GENERATE_ENUM_CLASS_BITWISE_OPERATORS(EnumType) \
inline constexpr EnumType operator&(EnumType x, EnumType y) { \
	return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(x) & static_cast<std::underlying_type_t<EnumType>>(y)); \
} \
inline constexpr EnumType operator|(EnumType x, EnumType y) { \
	return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(x) | static_cast<std::underlying_type_t<EnumType>>(y)); \
}

#define GENERATE_ENUM_CLASS_ITERATION_OPERATORS(EnumType) \
inline constexpr EnumType& operator++(EnumType& x) { \
	return x = static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(x) + 1); \
} \
inline constexpr bool operator<(EnumType x, EnumType y) { \
	return static_cast<std::underlying_type_t<EnumType>>(x) < static_cast<std::underlying_type_t<EnumType>>(y); \
}

#define GENERATE_ENUM_CLASS_ARITHMETIC_OPERATORS(EnumType) \
inline constexpr EnumType operator+(EnumType x, int offset) { \
	return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(x) + offset); \
}

#pragma endregion // ENUM
