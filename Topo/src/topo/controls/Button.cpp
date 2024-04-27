#include "pch.h"
#include "Button.h"
#include "topo/rendering/AssetManager.h"

namespace topo
{
Button::Button(float left, float top, float right, float bottom) : 
	Control(left, top, right, bottom),
	m_layout(left, top, right, bottom)
{
	m_layout.AddRow(topo::RowColumnType::STAR, 1.0f);
	m_layout.AddColumn(topo::RowColumnType::STAR, 1.0f);
}

void Button::Render(UIRenderer& renderer, const Timer& timer)
{
	renderer.DrawRectangle(
		m_positionRect.Left + m_margin.Left, 
		m_positionRect.Top + m_margin.Top, 
		m_positionRect.Right - m_margin.Right, 
		m_positionRect.Bottom - m_margin.Bottom, 
		{ 0.0f, 0.0f, 1.0f, 1.0f }
	);

	m_layout.Render(renderer, timer);
}

}