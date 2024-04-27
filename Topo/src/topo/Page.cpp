#include "pch.h"
#include "Page.h"

namespace topo
{
Page::Page(float width, float height) :
	m_layout(0.0f, 0.0f, width, height)
{

}





// Window Event Handlers
bool Page::OnWindowClosed()
{
	m_layout.OnWindowClosed();
	PostQuitMessage(0);
	return true;
}
bool Page::OnWindowResized(float width, float height)
{
	m_layout.SetPosition(0.0f, 0.0f, width, height);
	return true;
}
bool Page::OnKillFocus()
{
	m_layout.OnKillFocus();
	return true;
}
bool Page::OnDPIChanged()
{
	return true;
}

// Mouse Event Handlers
bool Page::OnLButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnLButtonDown(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnLButtonDown(mouseX, mouseY, keyStates);

	// With mouse button down events, if there is a handling control, we set keyboard handling control 
	// to be the same, otherwise, we leave it as it was
	if (m_mouseHandlingControl != nullptr)
		m_keyboardHandlingControl = m_mouseHandlingControl;

	return true;
}
bool Page::OnLButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnLButtonUp(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnLButtonUp(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnLButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnLButtonDoubleClick(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnLButtonDoubleClick(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnMButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnMButtonDown(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnMButtonDown(mouseX, mouseY, keyStates);

	// With mouse button down events, if there is a handling control, we set keyboard handling control 
	// to be the same, otherwise, we leave it as it was
	if (m_mouseHandlingControl != nullptr)
		m_keyboardHandlingControl = m_mouseHandlingControl;

	return true;
}
bool Page::OnMButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnMButtonUp(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnMButtonUp(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnMButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnMButtonDoubleClick(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnMButtonDoubleClick(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnRButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnRButtonDown(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnRButtonDown(mouseX, mouseY, keyStates);

	// With mouse button down events, if there is a handling control, we set keyboard handling control 
	// to be the same, otherwise, we leave it as it was
	if (m_mouseHandlingControl != nullptr)
		m_keyboardHandlingControl = m_mouseHandlingControl;

	return true;
}
bool Page::OnRButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnRButtonUp(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnRButtonUp(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnRButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnRButtonDoubleClick(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnRButtonDoubleClick(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnX1ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnX1ButtonDown(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnX1ButtonDown(mouseX, mouseY, keyStates);

	// With mouse button down events, if there is a handling control, we set keyboard handling control 
	// to be the same, otherwise, we leave it as it was
	if (m_mouseHandlingControl != nullptr)
		m_keyboardHandlingControl = m_mouseHandlingControl;

	return true;
}
bool Page::OnX1ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnX1ButtonUp(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnX1ButtonUp(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnX1ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnX1ButtonDoubleClick(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnX1ButtonDoubleClick(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnX2ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnX2ButtonDown(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnX2ButtonDown(mouseX, mouseY, keyStates);

	// With mouse button down events, if there is a handling control, we set keyboard handling control 
	// to be the same, otherwise, we leave it as it was
	if (m_mouseHandlingControl != nullptr)
		m_keyboardHandlingControl = m_mouseHandlingControl;

	return true;
}
bool Page::OnX2ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnX2ButtonUp(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnX2ButtonUp(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnX2ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnX2ButtonDoubleClick(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnX2ButtonDoubleClick(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnMouseMoved(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnMouseMoved(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnMouseMoved(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnMouseEntered(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnMouseEntered(mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnMouseEntered(mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnMouseLeave()
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnMouseLeave();

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnMouseLeave();

	return true;
}
bool Page::OnMouseWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnMouseWheel(wheelDelta, mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnMouseWheel(wheelDelta, mouseX, mouseY, keyStates);

	return true;
}
bool Page::OnMouseHWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// Try passing the event to the handling control
	if (m_mouseHandlingControl != nullptr)
		m_mouseHandlingControl = m_mouseHandlingControl->OnMouseHWheel(wheelDelta, mouseX, mouseY, keyStates);

	// If no handling control, pass event to the layout
	if (m_mouseHandlingControl == nullptr)
		m_mouseHandlingControl = m_layout.OnMouseHWheel(wheelDelta, mouseX, mouseY, keyStates);

	return true;
}

// Keyboard Event Handlers
bool Page::OnChar(unsigned int character, unsigned int repeatCount)
{
	// Try passing the event to the handling control
	if (m_keyboardHandlingControl != nullptr)
		m_keyboardHandlingControl = m_keyboardHandlingControl->OnChar(character, repeatCount);

	// If no handling control, pass event to the layout
	if (m_keyboardHandlingControl == nullptr)
		m_keyboardHandlingControl = m_layout.OnChar(character, repeatCount);

	return true;
}
bool Page::OnKeyDown(KeyCode keyCode, unsigned int repeatCount)
{
	// Try passing the event to the handling control
	if (m_keyboardHandlingControl != nullptr)
		m_keyboardHandlingControl = m_keyboardHandlingControl->OnKeyDown(keyCode, repeatCount);

	// If no handling control, pass event to the layout
	if (m_keyboardHandlingControl == nullptr)
		m_keyboardHandlingControl = m_layout.OnKeyDown(keyCode, repeatCount);

	return true;
}
bool Page::OnKeyUp(KeyCode keyCode, unsigned int repeatCount)
{
	// Try passing the event to the handling control
	if (m_keyboardHandlingControl != nullptr)
		m_keyboardHandlingControl = m_keyboardHandlingControl->OnKeyUp(keyCode, repeatCount);

	// If no handling control, pass event to the layout
	if (m_keyboardHandlingControl == nullptr)
		m_keyboardHandlingControl = m_layout.OnKeyUp(keyCode, repeatCount);

	return true;
}
bool Page::OnSysKeyDown(KeyCode keyCode, unsigned int repeatCount)
{
	// Try passing the event to the handling control
	if (m_keyboardHandlingControl != nullptr)
		m_keyboardHandlingControl = m_keyboardHandlingControl->OnSysKeyDown(keyCode, repeatCount);

	// If no handling control, pass event to the layout
	if (m_keyboardHandlingControl == nullptr)
		m_keyboardHandlingControl = m_layout.OnSysKeyDown(keyCode, repeatCount);

	return true;
}
bool Page::OnSysKeyUp(KeyCode keyCode, unsigned int repeatCount)
{
	// Try passing the event to the handling control
	if (m_keyboardHandlingControl != nullptr)
		m_keyboardHandlingControl = m_keyboardHandlingControl->OnSysKeyUp(keyCode, repeatCount);

	// If no handling control, pass event to the layout
	if (m_keyboardHandlingControl == nullptr)
		m_keyboardHandlingControl = m_layout.OnSysKeyUp(keyCode, repeatCount);

	return true;
}
}