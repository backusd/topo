#pragma once
#include "pch.h"
#include "Core.h"
#include "Layout.h"
#include "events/MouseButtonEventKeyStates.h"
#include "KeyCode.h"
#include "utils/Timer.h"

namespace topo
{
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class TOPO_API Page
{
public:
	Page(float width, float height);


	void Update(const Timer& timer);
	void Render();

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

private:
	Layout m_layout;





};
#pragma warning( pop )
}