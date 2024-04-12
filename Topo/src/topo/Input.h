#pragma once
#include "Core.h"
#include "KeyCode.h"

namespace topo
{
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class TOPO_API Input
{
public:
	static inline bool IsKeyDown(KeyCode keyCode) noexcept { return Get().IsKeyDownImpl(keyCode); }
	static inline std::pair<float, float> MousePosition() noexcept { return Get().MousePositionImpl(); }
	static inline float MousePositionX() noexcept { return Get().MousePositionXImpl(); }
	static inline float MousePositionY() noexcept { return Get().MousePositionYImpl(); }

private:
	friend class Window;

	static Input& Get()
	{
		static Input am{};
		return am;
	}

	constexpr Input() noexcept : 
		m_keyDownStates{},
		m_mousePosition{ 0.0f, 0.0f }
	{}
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(const Input&) = delete;
	Input& operator=(Input&&) = delete;

	// Getters
	constexpr bool IsKeyDownImpl(KeyCode keyCode) const noexcept { return m_keyDownStates[static_cast<unsigned int>(keyCode)]; }
	constexpr std::pair<float, float> MousePositionImpl() const noexcept { return m_mousePosition; }
	constexpr float MousePositionXImpl() noexcept { return std::get<0>(m_mousePosition); }
	constexpr float MousePositionYImpl() noexcept { return std::get<1>(m_mousePosition); }

	// Setters
	static inline void SetKeyDownState(KeyCode keyCode, bool isDown) noexcept { return Get().SetKeyDownStateImpl(keyCode, isDown); }
	static inline void SetMousePosition(float x, float y) noexcept { return Get().SetMousePositionImpl(x, y); }

	inline void SetKeyDownStateImpl(KeyCode keyCode, bool isDown) noexcept { m_keyDownStates[static_cast<unsigned int>(keyCode)] = isDown; }
	inline void SetMousePositionImpl(float x, float y) noexcept { m_mousePosition = { x, y }; }

	std::array<bool, 0xFF> m_keyDownStates;
	std::pair<float, float> m_mousePosition;
};
#pragma warning( pop )
}