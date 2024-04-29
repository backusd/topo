#include "pch.h"
#include "RenderRectangle2D.h"


namespace topo
{
RenderRectangle2D::RenderRectangle2D(const std::shared_ptr<UIRenderer>& renderer, float left, float right, float top, float bottom, const Color& color) :
	m_renderer(renderer),
	m_left(left),
	m_top(top),
	m_right(right),
	m_bottom(bottom),
	m_color(color),
	m_usingColor(true)
{
	RenderEffect2D effect = (m_color.A < 1.0f) ? RenderEffect2D::Transparent: RenderEffect2D::Opaque;
	m_uuid = m_renderer->RegisterObject(effect, BasicGeometry2D::Rectangle);
	SendUpdate();
}
RenderRectangle2D::RenderRectangle2D(const RenderRectangle2D& rhs) :
	m_renderer(rhs.m_renderer),
	m_left(rhs.m_left),
	m_top(rhs.m_top),
	m_right(rhs.m_right),
	m_bottom(rhs.m_bottom),
	m_color(rhs.m_color),
	m_usingColor(rhs.m_usingColor)
{
	if (m_usingColor)
	{
		RenderEffect2D effect = (m_color.A < 1.0f) ? RenderEffect2D::Transparent : RenderEffect2D::Opaque;
		m_uuid = m_renderer->RegisterObject(effect, BasicGeometry2D::Rectangle);
	}
	SendUpdate();
}
RenderRectangle2D::RenderRectangle2D(RenderRectangle2D&& rhs) noexcept :
	m_renderer(rhs.m_renderer),
	m_left(rhs.m_left),
	m_top(rhs.m_top),
	m_right(rhs.m_right),
	m_bottom(rhs.m_bottom),
	m_color(rhs.m_color),
	m_usingColor(rhs.m_usingColor),
	m_uuid(rhs.m_uuid)
{
	rhs.m_movedFrom = true;
}
RenderRectangle2D& RenderRectangle2D::operator=(const RenderRectangle2D& rhs)
{
	m_renderer = rhs.m_renderer;
	m_left = rhs.m_left;
	m_top = rhs.m_top;
	m_right = rhs.m_right;
	m_bottom = rhs.m_bottom;
	m_color = rhs.m_color;
	m_usingColor = rhs.m_usingColor;

	if (m_usingColor)
	{
		RenderEffect2D effect = (m_color.A < 1.0f) ? RenderEffect2D::Transparent : RenderEffect2D::Opaque;
		m_uuid = m_renderer->RegisterObject(effect, BasicGeometry2D::Rectangle);
	}
	SendUpdate();

	return *this;
}
RenderRectangle2D& RenderRectangle2D::operator=(RenderRectangle2D&& rhs) noexcept
{
	m_renderer = rhs.m_renderer;
	m_left = rhs.m_left;
	m_top = rhs.m_top;
	m_right = rhs.m_right;
	m_bottom = rhs.m_bottom;
	m_color = rhs.m_color;
	m_usingColor = rhs.m_usingColor;
	m_uuid = rhs.m_uuid;

	rhs.m_movedFrom = true;

	return *this;
}
RenderRectangle2D::~RenderRectangle2D()
{
	if (!m_movedFrom)
	{
		m_renderer->UnregisterObject(m_uuid);
	}
}

void RenderRectangle2D::SendUpdate()
{
	if (m_usingColor)
		m_renderer->UpdateRectangle(m_uuid, m_left, m_top, m_right, m_bottom, m_color);
}

}