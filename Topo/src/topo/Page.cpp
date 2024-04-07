#include "pch.h"
#include "Page.h"


namespace topo
{
Page::Page(float width, float height) :
	m_layout(width, height)
{

}


// Window Event Handlers
bool Page::OnWindowClosed()
{
	PostQuitMessage(0);
	return true;
}
bool Page::OnWindowResized(float height, float width)
{
	return true;
}
bool Page::OnKillFocus()
{
	return true;
}
bool Page::OnDPIChanged()
{
	return true;
}

// Mouse Event Handlers
bool Page::OnLButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnLButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnLButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnMButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnMButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnMButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnRButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnRButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnRButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnX1ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnX1ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnX1ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnX2ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnX2ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnX2ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnMouseMoved(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnMouseEntered(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnMouseLeave()
{
	return true;
}
bool Page::OnMouseWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}
bool Page::OnMouseHWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return true;
}

// Keyboard Event Handlers
bool Page::OnChar(unsigned int character, unsigned int repeatCount)
{
	return true;
}
bool Page::OnKeyDown(KeyCode keyCode, unsigned int repeatCount)
{
	return true;
}
bool Page::OnKeyUp(KeyCode keyCode, unsigned int repeatCount)
{
	return true;
}
bool Page::OnSysKeyDown(KeyCode keyCode, unsigned int repeatCount)
{
	return true;
}
bool Page::OnSysKeyUp(KeyCode keyCode, unsigned int repeatCount)
{
	return true;
}
}