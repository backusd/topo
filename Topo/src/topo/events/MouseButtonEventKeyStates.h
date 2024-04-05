#pragma once


namespace topo
{
class MouseButtonEventKeyStates
{
public:
	enum DownStates
	{
		L_BUTTON_DOWN = 0,
		M_BUTTON_DOWN = 1,
		R_BUTTON_DOWN = 2,
		X1_BUTTON_DOWN = 3,
		X2_BUTTON_DOWN = 4,
		CTRL_KEY_DOWN = 5,
		SHIFT_KEY_DOWN = 6
	};

public:
#ifdef TOPO_PLATFORM_WINDOWS
	constexpr MouseButtonEventKeyStates(WPARAM wParam) noexcept
	{
		WORD keyStates = GET_KEYSTATE_WPARAM(wParam);
		m_state[L_BUTTON_DOWN] = keyStates & MK_LBUTTON;
		m_state[M_BUTTON_DOWN] = keyStates & MK_MBUTTON;
		m_state[R_BUTTON_DOWN] = keyStates & MK_RBUTTON;
		m_state[X1_BUTTON_DOWN] = keyStates & MK_XBUTTON1;
		m_state[X2_BUTTON_DOWN] = keyStates & MK_XBUTTON2;
		m_state[CTRL_KEY_DOWN] = keyStates & MK_CONTROL;
		m_state[SHIFT_KEY_DOWN] = keyStates & MK_SHIFT;
	}
#endif

	constexpr bool LButtonIsDown() const noexcept { return m_state[DownStates::L_BUTTON_DOWN]; }
	constexpr bool MButtonIsDown() const noexcept { return m_state[DownStates::M_BUTTON_DOWN]; }
	constexpr bool RButtonIsDown() const noexcept { return m_state[DownStates::R_BUTTON_DOWN]; }
	constexpr bool X1ButtonIsDown() const noexcept { return m_state[DownStates::X1_BUTTON_DOWN]; }
	constexpr bool X2ButtonIsDown() const noexcept { return m_state[DownStates::X2_BUTTON_DOWN]; }
	constexpr bool CTRLKeyIsDown() const noexcept { return m_state[DownStates::CTRL_KEY_DOWN]; }
	constexpr bool ShiftKeyIsDown() const noexcept { return m_state[DownStates::SHIFT_KEY_DOWN]; }

private:
	std::bitset<8> m_state = { 0x00 };
};
}