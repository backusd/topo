#include "MainPage.h"


MainPage::MainPage(float width, float height) : topo::Page(width, height)
{
	SET_DEBUG_NAME(m_layout, "MainPage Layout");

	m_layout.AddRow(topo::RowColumnType::STAR, 1.0f);
	m_layout.AddRow(topo::RowColumnType::AUTO, 0.0f);
	m_layout.AddRow(topo::RowColumnType::FIXED, 10.0f);
	m_layout.AddRow(topo::RowColumnType::STAR, 1.0f);

	m_layout.AddColumn(topo::RowColumnType::STAR, 1.0f);

	button = m_layout.AddControl<topo::Button>();
	SET_DEBUG_NAME_PTR(button, "Test Button");


//	topo::Layout* sublayout = m_layout.AddSubLayout(3, 0);

}