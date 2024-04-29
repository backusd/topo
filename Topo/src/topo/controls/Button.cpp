#include "pch.h"
#include "Button.h"
#include "topo/rendering/AssetManager.h"

namespace topo
{
Button::Button(const std::shared_ptr<UIRenderer>& renderer, float left, float top, float right, float bottom) :
	Control(left, top, right, bottom),
	m_renderRect(renderer, left, top, right, bottom, { 0.0f, 0.0f, 1.0f, 1.0f }),
	m_layout(renderer, left, top, right, bottom)
{
	m_layout.AddRow(topo::RowColumnType::STAR, 1.0f);
	m_layout.AddColumn(topo::RowColumnType::STAR, 1.0f);
}

void Button::Update(const Timer& timer)
{
	OnUpdate(this, timer);

	m_layout.Update(timer);
}

}