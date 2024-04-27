#pragma once
#include "topo/Core.h"
#include "topo/utils/Rect.h"
#include "topo/events/MouseButtonEventKeyStates.h"
#include "topo/KeyCode.h"
#include "topo/utils/Timer.h"
#include "topo/rendering/UIRenderer.h"

namespace topo
{
struct Margin
{
	float Left = 0.0f;
	float Top = 0.0f;
	float Right = 0.0f;
	float Bottom = 0.0f;
};
struct Padding
{
	float Left = 0.0f;
	float Top = 0.0f;
	float Right = 0.0f;
	float Bottom = 0.0f;
};

class IEventReceiver
{
public:
	// Window Event Methods
	virtual void OnWindowClosed() = 0;
	virtual void OnKillFocus() = 0;

	// Mouse Event Methods
	virtual IEventReceiver* OnLButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnLButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnLButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnMButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnMButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnMButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnRButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnRButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnRButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnX1ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnX1ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnX1ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnX2ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnX2ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnX2ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnMouseMoved(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnMouseEntered(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnMouseLeave() = 0;
	virtual IEventReceiver* OnMouseWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;
	virtual IEventReceiver* OnMouseHWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) = 0;

	// Keyboard Event Methods
	virtual IEventReceiver* OnChar(unsigned int character, unsigned int repeatCount) = 0;
	virtual IEventReceiver* OnKeyDown(KeyCode keyCode, unsigned int repeatCount) = 0;
	virtual IEventReceiver* OnKeyUp(KeyCode keyCode, unsigned int repeatCount) = 0;
	virtual IEventReceiver* OnSysKeyDown(KeyCode keyCode, unsigned int repeatCount) = 0;
	virtual IEventReceiver* OnSysKeyUp(KeyCode keyCode, unsigned int repeatCount) = 0;
};


class Control : public IEventReceiver
{
public:
	constexpr Control(float left, float top, float right, float bottom) noexcept :
		m_positionRect{ left, top, right, bottom }
	{}
	Control(const Control&) = default;
	Control(Control&&) noexcept = default;
	Control& operator=(const Control&) = default;
	Control& operator=(Control&&) noexcept = default;

	virtual void Render(UIRenderer& renderer, const Timer& timer) = 0;

	ND constexpr void SetPositionRect(float left, float top, float right, float bottom) noexcept { m_positionRect = { left, top, right, bottom }; }

	ND virtual float GetAutoHeight() const noexcept { return 0.0f; }
	ND virtual float GetAutoWidth() const noexcept { return 0.0f; }

	// Window Event Methods
	virtual void OnWindowClosed() override { return; }
	virtual void OnKillFocus() override { return; }

	// Mouse Event Methods
	virtual IEventReceiver* OnLButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnLButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnLButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnMButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnMButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnMButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnRButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnRButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnRButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnX1ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnX1ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnX1ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnX2ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnX2ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnX2ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnMouseMoved(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnMouseEntered(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnMouseLeave() override { return nullptr; }
	virtual IEventReceiver* OnMouseWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }
	virtual IEventReceiver* OnMouseHWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override { return nullptr; }

	// Keyboard Event Methods
	virtual IEventReceiver* OnChar(unsigned int character, unsigned int repeatCount) override { return nullptr; }
	virtual IEventReceiver* OnKeyDown(KeyCode keyCode, unsigned int repeatCount) override { return nullptr; }
	virtual IEventReceiver* OnKeyUp(KeyCode keyCode, unsigned int repeatCount) override { return nullptr; }
	virtual IEventReceiver* OnSysKeyDown(KeyCode keyCode, unsigned int repeatCount) override { return nullptr; }
	virtual IEventReceiver* OnSysKeyUp(KeyCode keyCode, unsigned int repeatCount) override { return nullptr; }

protected:
	Rect m_positionRect;


// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept { m_name = name; }
	ND const std::string& GetDebugName() const noexcept { return m_name; }
private:
	std::string m_name = "Unnamed Layout";
#endif
};
}