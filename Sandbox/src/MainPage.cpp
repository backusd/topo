#include "MainPage.h"


MainPage::MainPage(float width, float height) : topo::Page(width, height)
{
	SET_DEBUG_NAME(m_layout, "MainPage Layout");

	m_layout.AddRow(topo::RowColumnType::FIXED, 10.0f);
	m_layout.AddRow(topo::RowColumnType::STAR, 1.0f);
	m_layout.AddRow(topo::RowColumnType::FIXED, 100.0f);
	m_layout.AddRow(topo::RowColumnType::STAR, 1.0f);

	m_layout.AddColumn(topo::RowColumnType::FIXED, 50.0f);
	m_layout.AddColumn(topo::RowColumnType::STAR, 1.0f);
	m_layout.AddColumn(topo::RowColumnType::FIXED, 50.0f);
	m_layout.AddColumn(topo::RowColumnType::STAR, 1.0f);

	button = m_layout.AddControl<topo::Button>(1, 1);
	SET_DEBUG_NAME_PTR(button, "Test Button");
	button->SetMargin(10.0f);

	button2 = m_layout.AddControl<topo::Button>(2, 2);
	button->SetPadding(5.0f, 10.0f);


//	topo::Layout* sublayout = m_layout.AddSubLayout(3, 0);

}