#pragma once
#include "topo/Core.h"
#include "Control.h"


namespace topo
{
class Button : public Control
{
public:
	Button(float left, float top, float right, float bottom);
	Button(const Button&) = default;
	Button(Button&&) noexcept = default;
	Button& operator=(const Button&) = default;
	Button& operator=(Button&&) noexcept = default;

	virtual void Render(UIRenderer& renderer, const Timer& timer) override;

	ND virtual float GetAutoHeight() const noexcept override { return 20.0f; }
	ND virtual float GetAutoWidth() const noexcept override { return 20.0f; }

private:
	void OneTimeInitialization();

private:
	static bool m_isInitialized;
};

}