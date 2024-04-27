#pragma once
#include "Core.h"
#include "Layout.h"
#include "events/MouseButtonEventKeyStates.h"
#include "KeyCode.h"
#include "utils/Timer.h"
#include "rendering/UIRenderer.h"


namespace topo
{
class Page
{
public:
	Page(float width, float height);

	inline void Render(UIRenderer& renderer, const Timer& timer) { m_layout.Render(renderer, timer); }

	// Window Event Handlers
	bool OnWindowClosed();
	bool OnWindowResized(float width, float height);
	bool OnKillFocus();
	bool OnDPIChanged();

	// Mouse Event Handlers
	bool OnLButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnLButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnLButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnRButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnRButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnRButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX1ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX1ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX1ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX2ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX2ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnX2ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMouseMoved(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMouseEntered(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMouseLeave();
	bool OnMouseWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);
	bool OnMouseHWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates);

	// Keyboard Event Handlers
	bool OnChar(unsigned int character, unsigned int repeatCount);
	bool OnKeyDown(KeyCode keyCode, unsigned int repeatCount);
	bool OnKeyUp(KeyCode keyCode, unsigned int repeatCount);
	bool OnSysKeyDown(KeyCode keyCode, unsigned int repeatCount);
	bool OnSysKeyUp(KeyCode keyCode, unsigned int repeatCount);	

protected:
	Layout          m_layout;
	IEventReceiver* m_mouseHandlingControl    = nullptr;
	IEventReceiver* m_keyboardHandlingControl = nullptr;


};

}