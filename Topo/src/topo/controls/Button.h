#pragma once
#include "topo/Core.h"
#include "Control.h"
#include "topo/Layout.h"


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

	void SetMargin(const Margin& margin) noexcept { m_margin = margin; UpdateLayoutPosition(); }
	void SetMargin(float left, float top, float right, float bottom) noexcept { m_margin = { left, top, right, bottom }; UpdateLayoutPosition(); }
	void SetMargin(float leftright, float topbottom) noexcept { m_margin = { leftright, topbottom, leftright, topbottom }; UpdateLayoutPosition(); }
	void SetMargin(float all) noexcept { m_margin = { all, all, all, all }; UpdateLayoutPosition(); }

	void SetPadding(const Padding& padding) noexcept { m_padding = padding; UpdateLayoutPosition(); }
	void SetPadding(float left, float top, float right, float bottom) noexcept { m_padding = { left, top, right, bottom }; UpdateLayoutPosition(); }
	void SetPadding(float leftright, float topbottom) noexcept { m_padding = { leftright, topbottom, leftright, topbottom }; UpdateLayoutPosition(); }
	void SetPadding(float all) noexcept { m_padding = { all, all, all, all }; UpdateLayoutPosition(); }


private:
	inline void UpdateLayoutPosition() noexcept
	{
		m_layout.SetPosition(
			m_positionRect.Left + m_margin.Left + m_padding.Left,
			m_positionRect.Top + m_margin.Top + m_padding.Top,
			m_positionRect.Right - m_margin.Right - m_padding.Right,
			m_positionRect.Bottom - m_margin.Bottom - m_padding.Bottom
		);
	}

	Margin m_margin = {};
	Padding m_padding = {};
	Layout m_layout;

};

}