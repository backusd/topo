#pragma once
#include "topo/Core.h"
#include "topo/rendering/UIRenderer.h"
#include "topo/utils/Color.h"



namespace topo
{
class RenderRectangle2D
{
public:
	RenderRectangle2D(const std::shared_ptr<UIRenderer>& renderer, float left, float right, float top, float bottom, const Color& color);
	RenderRectangle2D(const RenderRectangle2D&);
	RenderRectangle2D(RenderRectangle2D&&) noexcept;
	RenderRectangle2D& operator=(const RenderRectangle2D&);
	RenderRectangle2D& operator=(RenderRectangle2D&&) noexcept;
	~RenderRectangle2D();

	inline void SetRect(float left, float top, float right, float bottom)
	{
		m_left = left;
		m_top = top;
		m_right = right;
		m_bottom = bottom;
		SendUpdate();
	}
	inline void SetColor(const Color& color)
	{
		m_color = color;
		SendUpdate();
	}

private:
	void SendUpdate();

	std::shared_ptr<UIRenderer> m_renderer;
	unsigned int m_uuid = 0;
	float m_left = 0.0f;
	float m_top = 0.0f;
	float m_right = 0.0f;
	float m_bottom = 0.0f;
	Color m_color = {};
	bool m_usingColor = false;
	bool m_movedFrom = false;
};
}