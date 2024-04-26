#pragma once
#include "Core.h"
#include "controls/Control.h"
#include "topo/Log.h"
#include "topo/utils/Concepts.h"
#include "topo/utils/Rect.h"

namespace topo
{
enum class RowColumnType
{
	FIXED, STAR, PERCENT, AUTO
};
struct Row
{
	RowColumnType		 Type = RowColumnType::STAR;
	float				 Value = 1.0f;
	Rect				 Rect = {};
	bool				 Visible = true;
	bool				 Adjustable = true;
	std::optional<float> MinHeight = std::nullopt;
	std::optional<float> MaxHeight = std::nullopt;
};
struct Column
{
	RowColumnType		 Type = RowColumnType::STAR;
	float				 Value = 1.0f;
	Rect				 Rect = {};
	bool				 Visible = true;
	bool				 Adjustable = true;
	std::optional<float> MinWidth = std::nullopt;
	std::optional<float> MaxWidth = std::nullopt;
};
struct ControlPosition
{
	unsigned int RowIndex = 0;
	unsigned int ColumnIndex = 0;
	unsigned int RowSpan = 1;
	unsigned int ColumnSpan = 1;
};


class Layout : public IEventReceiver
{
public:
	Layout(float left, float top, float right, float bottom) : 
		m_rect{ left, top, right, bottom }
	{}
	Layout(Layout&&) = default;
	Layout& operator=(Layout&&) = default;

	template<typename T> requires std::derived_from<T, ::topo::Control>
	T* AddControl(unsigned int rowIndex = 0, unsigned int columnIndex = 0, unsigned int rowSpan = 1, unsigned int columnSpan = 1);
	Layout* AddSubLayout(unsigned int rowIndex = 0, unsigned int columnIndex = 0, unsigned int rowSpan = 1, unsigned int columnSpan = 1);

	inline void SetPosition(float left, float top, float right, float bottom) noexcept { m_rect = { left, top, right, bottom }; ReadjustRowsAndColumns(); }

	// Rows
	inline void AddRow(RowColumnType type, float value, bool adjustable = false, std::optional<float> minHeight = std::nullopt, std::optional<float> maxHeight = std::nullopt) noexcept
	{
		m_rows.emplace_back(type, value, Rect(), true, adjustable, minHeight, maxHeight);
		m_canScrollVertically = !(!m_canScrollVertically || (type == RowColumnType::STAR));
		ReadjustRows(m_columns.size() > 0);
	}
	inline void AddRow(const Row& row) noexcept 
	{ 
		m_rows.push_back(row); 
		m_canScrollVertically = !(!m_canScrollVertically || (row.Type == RowColumnType::STAR));
		ReadjustRows(m_columns.size() > 0); 
	}
	inline void AddRow(std::span<Row> rows) noexcept 
	{ 
		m_rows.append_range(rows); ReadjustRows(m_columns.size() > 0);
		if (m_canScrollVertically)
		{
			for (const Row& row : rows)
			{
				if (row.Type == RowColumnType::STAR)
				{
					m_canScrollVertically = false;
					break;
				}
			}
		}
	}	
	void ResetRows(std::span<Row> rows) noexcept;
	void ResetRows(std::vector<Row>&& rows) noexcept;
	void RemoveRow(unsigned int rowIndex, bool deleteContainedControlsAndSublayouts = true, bool deleteOverlappingControlsAndSublayouts = false) noexcept;

	// Columns
	inline void AddColumn(RowColumnType type, float value, bool adjustable = false, std::optional<float> minWidth = std::nullopt, std::optional<float> maxWidth = std::nullopt) noexcept
	{
		m_columns.emplace_back(type, value, Rect(), true, adjustable, minWidth, maxWidth);
		m_canScrollHorizontally = !(!m_canScrollHorizontally || (type == RowColumnType::STAR));
		ReadjustColumns(m_rows.size() > 0);
	}
	inline void AddColumn(const Column& column) noexcept 
	{ 
		m_columns.push_back(column); 
		m_canScrollHorizontally = !(!m_canScrollHorizontally || (column.Type == RowColumnType::STAR));
		ReadjustColumns(m_rows.size() > 0); 
	}
	inline void AddColumn(std::span<Column> columns) noexcept 
	{ 
		m_columns.append_range(columns); ReadjustColumns(m_rows.size() > 0); 
		if (m_canScrollHorizontally)
		{
			for (const Column& column : columns)
			{
				if (column.Type == RowColumnType::STAR)
				{
					m_canScrollHorizontally = false;
					break;
				}
			}
		}
	}
	void ResetColumns(std::span<Column> columns) noexcept;
	void ResetColumns(std::vector<Column>&& columns) noexcept;
	void RemoveColumn(unsigned int columnIndex, bool deleteContainedControlsAndSublayouts = true, bool deleteOverlappingControlsAndSublayouts = false) noexcept;


	ND float GetAutoHeight() const noexcept;
	ND float GetAutoWidth() const noexcept;

	ND constexpr bool ContainsPoint(float x, float y) const noexcept { return m_rect.ContainsPoint(x, y); }


	// Window Event Methods
	virtual void OnWindowClosed() override;
	virtual void OnKillFocus() override;

	// Mouse Event Methods
	virtual IEventReceiver* OnLButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnLButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnLButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnMButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnMButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnMButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnRButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnRButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnRButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnX1ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnX1ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnX1ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnX2ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnX2ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnX2ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnMouseMoved(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnMouseEntered(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnMouseLeave() override;
	virtual IEventReceiver* OnMouseWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;
	virtual IEventReceiver* OnMouseHWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates) override;

	// Keyboard Event Methods
	virtual IEventReceiver* OnChar(unsigned int character, unsigned int repeatCount) override;
	virtual IEventReceiver* OnKeyDown(KeyCode keyCode, unsigned int repeatCount) override;
	virtual IEventReceiver* OnKeyUp(KeyCode keyCode, unsigned int repeatCount) override;
	virtual IEventReceiver* OnSysKeyDown(KeyCode keyCode, unsigned int repeatCount) override;
	virtual IEventReceiver* OnSysKeyUp(KeyCode keyCode, unsigned int repeatCount) override;

private:
	Layout(const Layout&) = delete;
	Layout& operator=(const Layout&) = delete;

	inline void ReadjustRowsAndColumns() noexcept
	{
		ReadjustRows(false);
		ReadjustColumns(false);
		ReadjustControlsAndSublayouts();
	}
	void ReadjustRows(bool readjustControlsAndSublayouts = true) noexcept;
	void ReadjustColumns(bool readjustControlsAndSublayouts = true) noexcept;
	void ReadjustControlsAndSublayouts() noexcept;
	void ReadjustControlsAndSublayoutsInRow(unsigned int rowIndex) noexcept;
	void ReadjustControlsAndSublayoutsInColumn(unsigned int columnIndex) noexcept;
	ND float CalculateRowStarHeight() const noexcept;
	ND float CalculateColumnStarWidth() const noexcept;
	void UpdateAutoRowHeights() noexcept;
	void UpdateAutoColumnWidths() noexcept;

	bool AdjustControlAndSublayoutRowPositioning() noexcept;
	bool AdjustControlAndSublayoutColumnPositioning() noexcept;

	bool CheckMouseOverDraggableRowOrColumn(float x, float y) noexcept;

	Rect m_rect;
	std::vector<std::pair<std::unique_ptr<Control>, ControlPosition>> m_controls;
	std::vector<std::pair<std::unique_ptr<Layout>, ControlPosition>> m_sublayouts;
	std::vector<Row> m_rows;
	std::vector<Column> m_columns;
	bool m_canScrollVertically = true;
	bool m_canScrollHorizontally = true;
	float m_verticalScrollOffset = 0.0f;
	float m_horizontalScrollOffset = 0.0f;

	bool m_activelyDragging = false;
	std::optional<unsigned int> m_rowDraggingIndex = std::nullopt;
	std::optional<unsigned int> m_columnDraggingIndex = std::nullopt;


// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept { m_name = name; }
	ND const std::string& GetDebugName() const noexcept { return m_name; }
private:
	std::string m_name = "Unnamed Layout";
#endif
};

template<typename T> requires std::derived_from<T, ::topo::Control>
T* Layout::AddControl(unsigned int rowIndex, unsigned int columnIndex, unsigned int rowSpan, unsigned int columnSpan)
{
	// First, make sure we have at least one row and column
	if (m_rows.size() == 0) [[unlikely]]
	{
		LOG_ERROR("[Layout: {0}] Attempting to add a sublayout, but no rows have been established.", m_name);
		LOG_ERROR("[Layout: {0}] Adding a default STAR row to avoid crashing", m_name);
		AddRow(RowColumnType::STAR, 1.0f);
	}
	if (m_columns.size() == 0) [[unlikely]]
	{
		LOG_ERROR("[Layout: {0}] Attempting to add a sublayout, but no columns have been established.", m_name);
		LOG_ERROR("[Layout: {0}] Adding a default STAR column to avoid crashing", m_name);
		AddColumn(RowColumnType::STAR, 1.0f);
	}

	// Make sure row/column locations are valid
	if (rowIndex >= m_rows.size()) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add control to row index {1} - max index is {2}.", m_name, rowIndex, m_rows.size() - 1);
		rowIndex = static_cast<unsigned int>(m_rows.size()) - 1;
	}
	if (columnIndex >= m_columns.size()) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add control to column index {1} - max index is {2}.", m_name, columnIndex, m_columns.size() - 1);
		columnIndex = static_cast<unsigned int>(m_columns.size()) - 1;
	}
	if (rowSpan == 0) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add control with a row span of 0.", m_name);
		rowSpan = 1;
	}
	if (columnSpan == 0) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add control with a column span of 0.", m_name);
		columnSpan = 1;
	}
	if (rowIndex + rowSpan > m_rows.size()) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add control with row index {1} and row span of {2} because it would go beyond the max row index of {3}.", m_name, rowIndex, rowSpan, m_rows.size() - 1);
		rowSpan = static_cast<unsigned int>(m_rows.size()) - rowIndex;
	}
	if (columnIndex + columnSpan > m_columns.size()) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add control with column index {1} and column span of {2} because it would go beyond the max column index of {3}.", m_name, columnIndex, columnSpan, m_columns.size() - 1);
		columnSpan = static_cast<unsigned int>(m_columns.size()) - columnIndex;
	}

	ControlPosition cp = { rowIndex, columnIndex, rowSpan, columnSpan };

	Control* control = new T(
		m_columns[columnIndex].Rect.Left,
		m_rows[rowIndex].Rect.Top,
		m_columns[columnIndex + columnSpan - 1].Rect.Right,
		m_rows[rowIndex + rowSpan - 1].Rect.Bottom
	);

	m_controls.emplace_back(control, cp);

	// If the control resides (either partially or completely) within an AUTO row/column
	// the layout needs updating
	for (unsigned int iii = rowIndex; iii < rowIndex + rowSpan; ++iii)
	{
		if (m_rows[iii].Type == RowColumnType::AUTO)
		{
			ReadjustRows();
			break;
		}
	}
	for (unsigned int iii = columnIndex; iii < columnIndex + columnSpan; ++iii)
	{
		if (m_columns[iii].Type == RowColumnType::AUTO)
		{
			ReadjustColumns();
			break;
		}
	}

	return static_cast<T*>(control);
}



}