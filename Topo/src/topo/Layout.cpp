#include "pch.h"
#include "Layout.h"






namespace topo
{
void Layout::Render(UIRenderer& renderer, const Timer& timer)
{
	// Update controls
	for (auto& pair : m_controls)
	{
		std::get<0>(pair)->Render(renderer, timer);
	}

	// Update sublayouts
	for (auto& pair : m_sublayouts)
	{
		std::get<0>(pair)->Render(renderer, timer);
	}


	for (const Row& row : m_rows)
	{
		renderer.DrawLine(row.Rect.Left, row.Rect.Top, row.Rect.Right, row.Rect.Top, { 0.0f, 0.0f, 0.0f, 1.0f }, 2.0f);
	}

	for (const Column& column : m_columns)
	{
		renderer.DrawLine(column.Rect.Left, column.Rect.Top, column.Rect.Left, column.Rect.Bottom, { 0.0f, 0.0f, 0.0f, 1.0f }, 2.0f);
	}

}

Layout* Layout::AddSubLayout(unsigned int rowIndex, unsigned int columnIndex, unsigned int rowSpan, unsigned int columnSpan)
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
		LOG_WARN("[Layout: {0}] Cannot add sub layout to row index {1} - max index is {2}.", m_name, rowIndex, m_rows.size() - 1);
		rowIndex = static_cast<unsigned int>(m_rows.size()) - 1;
	}
	if (columnIndex >= m_columns.size()) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add sub layout to column index {1} - max index is {2}.", m_name, columnIndex, m_columns.size() - 1);
		columnIndex = static_cast<unsigned int>(m_columns.size()) - 1;
	}
	if (rowSpan == 0) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add sub layout with a row span of 0.", m_name);
		rowSpan = 1;
	}
	if (columnSpan == 0) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add sub layout with a column span of 0.", m_name);
		columnSpan = 1;
	}
	if (rowIndex + rowSpan > m_rows.size()) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add sub layout with row index {1} and row span of {2} because it would go beyond the max row index of {3}.", m_name, rowIndex, rowSpan, m_rows.size() - 1);
		rowSpan = static_cast<unsigned int>(m_rows.size()) - rowIndex;
	}
	if (columnIndex + columnSpan > m_columns.size()) [[unlikely]]
	{
		LOG_WARN("[Layout: {0}] Cannot add sub layout with column index {1} and column span of {2} because it would go beyond the max column index of {3}.", m_name, columnIndex, columnSpan, m_columns.size() - 1);
		columnSpan = static_cast<unsigned int>(m_columns.size()) - columnIndex;
	}

	ControlPosition cp = { rowIndex, columnIndex, rowSpan, columnSpan };

	Layout* sublayout = new Layout(
		m_columns[columnIndex].Rect.Left,
		m_rows[rowIndex].Rect.Top,
		m_columns[columnIndex + columnSpan - 1].Rect.Right,
		m_rows[rowIndex + rowSpan - 1].Rect.Bottom
	);

	m_sublayouts.emplace_back(sublayout, cp);

	// If the sublayout resides (either partially or completely) within an AUTO row/column
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

	return sublayout;
}


void Layout::ReadjustRows(bool readjustControlsAndSublayouts) noexcept
{
	ASSERT(m_rect.Bottom > m_rect.Top, "Cannot have negative height");

	// Before we can compute what one star height should equal, we need to 
	// make sure all AUTO sized rows have the correct height
	UpdateAutoRowHeights();

	// Layout::Only need to calculate rowStarHeight if star rows exist, and they should only exist if vertical scrollability is false
	float rowStarHeight = 0.0f;
	if (!m_canScrollVertically)
		rowStarHeight = CalculateRowStarHeight();

	// offset the top by the scroll offset
	float nextTop = m_rect.Top - m_verticalScrollOffset;

	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
	{
		Row& row = m_rows[iii];

		// In all cases, the rows will span the layout
		row.Rect.Left = m_rect.Left;
		row.Rect.Right = m_rect.Right;

		// Set the top and then compute the height
		row.Rect.Top = nextTop;
		switch (row.Type)
		{
		// intentially fallthrough AUTO because it holds a fixed height value
		case RowColumnType::AUTO:
		case RowColumnType::FIXED:   nextTop += row.Value; break;
		case RowColumnType::PERCENT: nextTop += ((row.Value / 100) * m_rect.Height()); break;
		case RowColumnType::STAR:
		{
			if (m_canScrollVertically)
				ASSERT(false, "Something went wrong - m_canScrollVertically should be false if there is a STAR row");
			
			nextTop += (row.Value * rowStarHeight);
			break;
		}
		default:
			ASSERT(false, "Unrecognized row type");
		}
		row.Rect.Bottom = nextTop;

		// Row is only visible if the row does not lie completely above the layout rect or completely below it
		row.Visible = !(row.Rect.Bottom < m_rect.Top || row.Rect.Top > m_rect.Bottom);
	}

	if (readjustControlsAndSublayouts)
		ReadjustControlsAndSublayouts();
}
void Layout::ReadjustColumns(bool readjustControlsAndSublayouts) noexcept
{
	ASSERT(m_rect.Right > m_rect.Left, "Cannot have negative width");

	// Before we can compute what one star height should equal, we need to 
	// make sure all AUTO sized rows have the correct height
	UpdateAutoColumnWidths();

	// Layout::Only need to calculate rowStarWidth if star column exist, and they should only exist if horizontal scrollability is false
	float rowStarWidth = 0.0f;
	if (!m_canScrollHorizontally)
		rowStarWidth = CalculateColumnStarWidth();

	// offset the left by the scroll offset
	float nextLeft = m_rect.Left - m_horizontalScrollOffset;

	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
	{
		Column& column = m_columns[iii];

		// In all cases, the columns will span the layout
		column.Rect.Top = m_rect.Top;
		column.Rect.Bottom = m_rect.Bottom;

		// Set the left and then compute the width
		column.Rect.Left = nextLeft;
		switch (column.Type)
		{
			// intentially fallthrough AUTO because it holds a fixed height value
		case RowColumnType::AUTO:
		case RowColumnType::FIXED:   nextLeft += column.Value; break;
		case RowColumnType::PERCENT: nextLeft += ((column.Value / 100) * m_rect.Width()); break;
		case RowColumnType::STAR:	 nextLeft += (column.Value * rowStarWidth); break;
		default:
			ASSERT(false, "Unrecognized column type");
		}
		column.Rect.Right = nextLeft;

		// Column is only visible if the column does not lie completely left of the layout rect or completely right of it
		column.Visible = !(column.Rect.Right < m_rect.Left || column.Rect.Left > m_rect.Right);
	}

	if (readjustControlsAndSublayouts)
		ReadjustControlsAndSublayouts();
}
float Layout::CalculateRowStarHeight() const noexcept
{
	float totalStars = 0.0f;
	for (const Row& row : m_rows)
	{
		if (row.Type == RowColumnType::STAR)
			totalStars += row.Value;
	}

	if (totalStars == 0.0f)
		return 0.0f;

	float availableHeight = m_rect.Height();
	for (const Row& row : m_rows) 
	{
		switch (row.Type)
		{
		// intentially fallthrough AUTO because it holds a fixed height value
		case RowColumnType::AUTO:
		case RowColumnType::FIXED:   availableHeight -= row.Value; break;
		case RowColumnType::PERCENT: availableHeight -= ((row.Value / 100) * m_rect.Height()); break;
		}

		if (availableHeight < 0.0f)
		{
			LOG_ERROR("[Layout: {0}] Available Star Height should never be less than 0", m_name);
			return 0.0f;
		}
	}

	ASSERT(totalStars > 0.0f, "If we reached here, total stars should be > 0");
	return availableHeight / totalStars;
}
float Layout::CalculateColumnStarWidth() const noexcept
{
	float totalStars = 0.0f;
	for (const Column& column : m_columns)
	{
		if (column.Type == RowColumnType::STAR)
			totalStars += column.Value;
	}

	if (totalStars == 0.0f)
		return 0.0f;

	float availableWidth = m_rect.Width();
	for (const Column& column : m_columns)
	{
		switch (column.Type)
		{
		// intentially fallthrough AUTO because it holds a fixed height value
		case RowColumnType::AUTO:
		case RowColumnType::FIXED:   availableWidth -= column.Value; break;
		case RowColumnType::PERCENT: availableWidth -= ((column.Value / 100) * m_rect.Width()); break;
		}

		if (availableWidth < 0.0f)
		{
			LOG_WARN("[Layout: {0}] No available Star Width", m_name);
			return 0.0f;
		}
	}

	return availableWidth / totalStars;
}
void Layout::ReadjustControlsAndSublayouts() noexcept
{
	// Adjust controls
	for (std::pair<std::unique_ptr<Control>, ControlPosition>& pair : m_controls)
	{
		Control* control = std::get<0>(pair).get();
		ControlPosition& cp = std::get<1>(pair);

		control->SetPositionRect(
			m_columns[cp.ColumnIndex].Rect.Left,
			m_rows[cp.RowIndex].Rect.Top,
			m_columns[cp.ColumnIndex + cp.ColumnSpan - 1].Rect.Right,
			m_rows[cp.RowIndex + cp.RowSpan - 1].Rect.Bottom
		);
	}
	
	// Adjust sublayouts
	for (std::pair<std::unique_ptr<Layout>, ControlPosition>& pair : m_sublayouts)
	{
		Layout* layout = std::get<0>(pair).get();
		ControlPosition& cp = std::get<1>(pair);

		layout->SetPosition(
			m_columns[cp.ColumnIndex].Rect.Left,
			m_rows[cp.RowIndex].Rect.Top,
			m_columns[cp.ColumnIndex + cp.ColumnSpan - 1].Rect.Right,
			m_rows[cp.RowIndex + cp.RowSpan - 1].Rect.Bottom
		);
	}
}
void Layout::ReadjustControlsAndSublayoutsInRow(unsigned int rowIndex) noexcept
{
	// Adjust controls
	for (std::pair<std::unique_ptr<Control>, ControlPosition>& pair : m_controls)
	{
		Control* control = std::get<0>(pair).get();
		ControlPosition& cp = std::get<1>(pair);

		if (cp.RowIndex <= rowIndex && cp.RowIndex + cp.RowSpan > rowIndex)
		{
			control->SetPositionRect(
				m_columns[cp.ColumnIndex].Rect.Left,
				m_rows[cp.RowIndex].Rect.Top,
				m_columns[cp.ColumnIndex + cp.ColumnSpan - 1].Rect.Right,
				m_rows[cp.RowIndex + cp.RowSpan - 1].Rect.Bottom
			);
		}
	}

	// Adjust sublayouts
	for (std::pair<std::unique_ptr<Layout>, ControlPosition>& pair : m_sublayouts)
	{
		Layout* layout = std::get<0>(pair).get();
		ControlPosition& cp = std::get<1>(pair);

		if (cp.RowIndex <= rowIndex && cp.RowIndex + cp.RowSpan > rowIndex)
		{
			layout->SetPosition(
				m_columns[cp.ColumnIndex].Rect.Left,
				m_rows[cp.RowIndex].Rect.Top,
				m_columns[cp.ColumnIndex + cp.ColumnSpan - 1].Rect.Right,
				m_rows[cp.RowIndex + cp.RowSpan - 1].Rect.Bottom
			);
		}
	}
}
void Layout::ReadjustControlsAndSublayoutsInColumn(unsigned int columnIndex) noexcept
{
	// Adjust controls
	for (std::pair<std::unique_ptr<Control>, ControlPosition>& pair : m_controls)
	{
		Control* control = std::get<0>(pair).get();
		ControlPosition& cp = std::get<1>(pair);

		if (cp.ColumnIndex <= columnIndex && cp.ColumnIndex + cp.ColumnSpan > columnIndex)
		{
			control->SetPositionRect(
				m_columns[cp.ColumnIndex].Rect.Left,
				m_rows[cp.RowIndex].Rect.Top,
				m_columns[cp.ColumnIndex + cp.ColumnSpan - 1].Rect.Right,
				m_rows[cp.RowIndex + cp.RowSpan - 1].Rect.Bottom
			);
		}
	}

	// Adjust sublayouts
	for (std::pair<std::unique_ptr<Layout>, ControlPosition>& pair : m_sublayouts)
	{
		Layout* layout = std::get<0>(pair).get();
		ControlPosition& cp = std::get<1>(pair);

		if (cp.ColumnIndex <= columnIndex && cp.ColumnIndex + cp.ColumnSpan > columnIndex)
		{
			layout->SetPosition(
				m_columns[cp.ColumnIndex].Rect.Left,
				m_rows[cp.RowIndex].Rect.Top,
				m_columns[cp.ColumnIndex + cp.ColumnSpan - 1].Rect.Right,
				m_rows[cp.RowIndex + cp.RowSpan - 1].Rect.Bottom
			);
		}
	}
}

void Layout::ResetRows(std::span<Row> rows) noexcept
{
	// If their are fewer new rows, then we need to adjust any controls that reside in
	// rows that will no longer exist
	bool controlLocationsNeedAdjusting = (rows.size() < m_rows.size());

	m_rows.clear();
	m_rows.assign_range(rows);

	m_canScrollVertically = true;
	for (const Row& row : m_rows)
	{
		if (row.Type == RowColumnType::STAR)
		{
			m_canScrollVertically = false;
			break;
		}
	}

	if (controlLocationsNeedAdjusting)
		AdjustControlAndSublayoutRowPositioning();

	// Adjust the row sizing (and controls/sublayouts
	ReadjustRows(true);
}
void Layout::ResetRows(std::vector<Row>&& rows) noexcept
{
	// If their are fewer new rows, then we need to adjust any controls that reside in
	// rows that will no longer exist
	bool controlLocationsNeedAdjusting = (rows.size() < m_rows.size());

	m_rows.clear();
	m_rows = std::move(rows);

	m_canScrollVertically = true;
	for (const Row& row : m_rows)
	{
		if (row.Type == RowColumnType::STAR)
		{
			m_canScrollVertically = false;
			break;
		}
	}

	if (controlLocationsNeedAdjusting)
		AdjustControlAndSublayoutRowPositioning();

	// Adjust the row sizing (and controls/sublayouts
	ReadjustRows(true);
}
void Layout::RemoveRow(unsigned int rowIndex, bool deleteContainedControlsAndSublayouts, bool deleteOverlappingControlsAndSublayouts) noexcept
{
	if (rowIndex >= m_rows.size())
	{
		LOG_ERROR("[Layout: {0}] Cannot delete row at index {1} - there are only {2} rows.", m_name, rowIndex, m_rows.size());
		return;
	}

	std::vector<unsigned int> indicesToDelete;

	// Delete any controls/sublayouts that reside soley within the row
	if (deleteContainedControlsAndSublayouts)
	{
		for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
		{
			const ControlPosition& cp = std::get<1>(m_controls[iii]);
			if (cp.RowIndex == rowIndex && cp.RowSpan == 1)
				indicesToDelete.push_back(iii);
		}

		for (std::vector<unsigned int>::reverse_iterator riter = indicesToDelete.rbegin(); riter != indicesToDelete.rend(); ++riter)
			m_controls.erase(m_controls.begin() + *riter);

		indicesToDelete.clear();

		for (unsigned int iii = 0; iii < m_sublayouts.size(); ++iii)
		{
			const ControlPosition& cp = std::get<1>(m_sublayouts[iii]);
			if (cp.RowIndex == rowIndex && cp.RowSpan == 1)
				indicesToDelete.push_back(iii);
		}

		for (std::vector<unsigned int>::reverse_iterator riter = indicesToDelete.rbegin(); riter != indicesToDelete.rend(); ++riter)
			m_sublayouts.erase(m_sublayouts.begin() + *riter);
	}

	indicesToDelete.clear();

	// Delete any controls/sublayouts that have a rowspan > 2 and are partially in the row OR modify their rowspan/row index value to
	// make sure they don't bleed into other rows
	for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
	{
		ControlPosition& cp = std::get<1>(m_controls[iii]);
		if (cp.RowIndex <= rowIndex && cp.RowIndex + cp.RowSpan > rowIndex)
		{
			if (deleteOverlappingControlsAndSublayouts)
				indicesToDelete.push_back(iii);
			else if (cp.RowSpan > 1)
				--cp.RowSpan;
		}
	}
	if (deleteOverlappingControlsAndSublayouts)
	{
		for (std::vector<unsigned int>::reverse_iterator riter = indicesToDelete.rbegin(); riter != indicesToDelete.rend(); ++riter)
			m_controls.erase(m_controls.begin() + *riter);

		indicesToDelete.clear();
	}

	for (unsigned int iii = 0; iii < m_sublayouts.size(); ++iii)
	{
		ControlPosition& cp = std::get<1>(m_sublayouts[iii]);
		if (cp.RowIndex <= rowIndex && cp.RowIndex + cp.RowSpan > rowIndex)
		{
			if (deleteOverlappingControlsAndSublayouts)
				indicesToDelete.push_back(iii);
			else if (cp.RowSpan > 1)
				--cp.RowSpan;
		}
	}

	if (deleteOverlappingControlsAndSublayouts)
	{
		for (std::vector<unsigned int>::reverse_iterator riter = indicesToDelete.rbegin(); riter != indicesToDelete.rend(); ++riter)
			m_sublayouts.erase(m_sublayouts.begin() + *riter);
	}

	// For all remaining controls/sublayouts, we must decrement their RowIndex value if it was greater than the row being deleted
	for (auto& pair : m_controls)
	{
		ControlPosition& cp = std::get<1>(pair);
		if (cp.RowIndex > rowIndex)
			cp.RowIndex -= 1;
	}
	for (auto& pair : m_sublayouts)
	{
		ControlPosition& cp = std::get<1>(pair);
		if (cp.RowIndex > rowIndex)
			cp.RowIndex -= 1;
	}

	// Delete the row
	m_rows.erase(m_rows.begin() + rowIndex);
}

void Layout::ResetColumns(std::span<Column> columns) noexcept
{
	// If their are fewer new columns, then we need to adjust any controls that reside in
	// columns that will no longer exist
	bool controlLocationsNeedAdjusting = (columns.size() < m_columns.size());

	m_columns.clear();
	m_columns.assign_range(columns);

	m_canScrollHorizontally = true;
	for (const Column& column : m_columns)
	{
		if (column.Type == RowColumnType::STAR)
		{
			m_canScrollHorizontally = false;
			break;
		}
	}

	if (controlLocationsNeedAdjusting)
		AdjustControlAndSublayoutRowPositioning();

	// Adjust the row sizing (and controls/sublayouts)
	ReadjustColumns(true);
}
void Layout::ResetColumns(std::vector<Column>&& columns) noexcept
{	
	// If their are fewer new columns, then we need to adjust any controls that reside in
	// columns that will no longer exist
	bool controlLocationsNeedAdjusting = (columns.size() < m_columns.size());

	m_columns.clear();
	m_columns = std::move(columns);

	m_canScrollHorizontally = true;
	for (const Column& column : m_columns)
	{
		if (column.Type == RowColumnType::STAR)
		{
			m_canScrollHorizontally = false;
			break;
		}
	}

	if (controlLocationsNeedAdjusting)
		AdjustControlAndSublayoutRowPositioning();

	// Adjust the row sizing (and controls/sublayouts)
	ReadjustColumns(true);
}
void Layout::RemoveColumn(unsigned int columnIndex, bool deleteContainedControlsAndSublayouts, bool deleteOverlappingControlsAndSublayouts) noexcept
{
	if (columnIndex >= m_columns.size())
	{
		LOG_ERROR("[Layout: {0}] Cannot delete column at index {1} - there are only {2} columns.", m_name, columnIndex, m_columns.size());
		return;
	}

	std::vector<unsigned int> indicesToDelete;

	// Delete any controls/sublayouts that reside soley within the column
	if (deleteContainedControlsAndSublayouts)
	{
		for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
		{
			const ControlPosition& cp = std::get<1>(m_controls[iii]);
			if (cp.ColumnIndex == columnIndex && cp.ColumnSpan == 1)
				indicesToDelete.push_back(iii);
		}

		for (std::vector<unsigned int>::reverse_iterator riter = indicesToDelete.rbegin(); riter != indicesToDelete.rend(); ++riter)
			m_controls.erase(m_controls.begin() + *riter);

		indicesToDelete.clear();

		for (unsigned int iii = 0; iii < m_sublayouts.size(); ++iii)
		{
			const ControlPosition& cp = std::get<1>(m_sublayouts[iii]);
			if (cp.ColumnIndex == columnIndex && cp.ColumnSpan == 1)
				indicesToDelete.push_back(iii);
		}

		for (std::vector<unsigned int>::reverse_iterator riter = indicesToDelete.rbegin(); riter != indicesToDelete.rend(); ++riter)
			m_sublayouts.erase(m_sublayouts.begin() + *riter);
	}

	indicesToDelete.clear();

	// Delete any controls/sublayouts that have a columnspan > 2 and are partially in the column OR modify their columnspan/column index value to
	// make sure they don't bleed into other columns
	for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
	{
		ControlPosition& cp = std::get<1>(m_controls[iii]);
		if (cp.ColumnIndex <= columnIndex && cp.ColumnIndex + cp.ColumnSpan > columnIndex)
		{
			if (deleteOverlappingControlsAndSublayouts)
				indicesToDelete.push_back(iii);
			else if (cp.ColumnSpan > 1)
				--cp.ColumnSpan;
		}
	}
	if (deleteOverlappingControlsAndSublayouts)
	{
		for (std::vector<unsigned int>::reverse_iterator riter = indicesToDelete.rbegin(); riter != indicesToDelete.rend(); ++riter)
			m_controls.erase(m_controls.begin() + *riter);

		indicesToDelete.clear();
	}

	for (unsigned int iii = 0; iii < m_sublayouts.size(); ++iii)
	{
		ControlPosition& cp = std::get<1>(m_sublayouts[iii]);
		if (cp.ColumnIndex <= columnIndex && cp.ColumnIndex + cp.ColumnSpan > columnIndex)
		{
			if (deleteOverlappingControlsAndSublayouts)
				indicesToDelete.push_back(iii);
			else if (cp.ColumnSpan > 1)
				--cp.ColumnSpan;
		}
	}

	if (deleteOverlappingControlsAndSublayouts)
	{
		for (std::vector<unsigned int>::reverse_iterator riter = indicesToDelete.rbegin(); riter != indicesToDelete.rend(); ++riter)
			m_sublayouts.erase(m_sublayouts.begin() + *riter);
	}

	// For all remaining controls/sublayouts, we must decrement their ColumnIndex value if it was greater than the column being deleted
	for (auto& pair : m_controls)
	{
		ControlPosition& cp = std::get<1>(pair);
		if (cp.ColumnIndex > columnIndex)
			cp.ColumnIndex -= 1;
	}
	for (auto& pair : m_sublayouts)
	{
		ControlPosition& cp = std::get<1>(pair);
		if (cp.ColumnIndex > columnIndex)
			cp.ColumnIndex -= 1;
	}

	// Delete the row
	m_columns.erase(m_columns.begin() + columnIndex);
}


void Layout::UpdateAutoRowHeights() noexcept
{
	// Create a vector the same size as the number of total rows that will hold the required
	// height values for the auto rows (NOTE: elements of the vector that correspond to non-auto
	// rows will not get used)
	std::vector<float> autoRowHeights(m_rows.size());

	// Loop over each control/sublayout to compute required heights and cache the values
	std::vector<float> controlRequiredHeights(m_controls.size());
	std::vector<float> subLayoutRequiredHeights(m_sublayouts.size());
	for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
	{
		// Calling GetAutoHeight is expensive so only do it for the controls that reside in an auto row
		const ControlPosition& cp = std::get<1>(m_controls[iii]);
		for (unsigned int rowIndex = cp.RowIndex; iii < cp.RowIndex + cp.RowSpan; ++rowIndex)
		{
			if (m_rows[rowIndex].Type == RowColumnType::AUTO)
			{
				controlRequiredHeights[iii] = std::get<0>(m_controls[iii])->GetAutoHeight();
				break;
			}
		}
	}
	for (unsigned int iii = 0; iii < m_sublayouts.size(); ++iii)
	{
		// Calling GetAutoHeight is expensive so only do it for the sublayouts that reside in an auto row
		const ControlPosition& cp = std::get<1>(m_sublayouts[iii]);
		for (unsigned int rowIndex = cp.RowIndex; rowIndex < cp.RowIndex + cp.RowSpan; ++rowIndex)
		{
			if (m_rows[rowIndex].Type == RowColumnType::AUTO)
			{
				subLayoutRequiredHeights[iii] = std::get<0>(m_sublayouts[iii])->GetAutoHeight();
				break;
			}
		}
	}

	// First generate an auto height value for only those controls/sublayouts that reside in
	// an AUTO row and do not span into any other rows
	for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
	{
		const ControlPosition& cp = std::get<1>(m_controls[iii]);
		if (m_rows[cp.RowIndex].Type == RowColumnType::AUTO && cp.RowSpan == 1)
			autoRowHeights[cp.RowIndex] = std::max(autoRowHeights[cp.RowIndex], controlRequiredHeights[iii]);
	}
	for (unsigned int iii = 0; iii < m_sublayouts.size(); ++iii)
	{
		const ControlPosition& cp = std::get<1>(m_sublayouts[iii]);
		if (m_rows[cp.RowIndex].Type == RowColumnType::AUTO && cp.RowSpan == 1)
			autoRowHeights[cp.RowIndex] = std::max(autoRowHeights[cp.RowIndex], controlRequiredHeights[iii]);
	}

	// Now we loop over all auto rows in order and can subtract required height from the rows that come after them
	// for controls/sublayouts that have a row span >1
	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
	{
		if (m_rows[iii].Type != RowColumnType::AUTO)
			continue;

		// Now, loop over controls and sublayouts again, but this time we target controls/sublayouts that
		// start in the AUTO row we are on, but have a row span >1
		for (unsigned int controlIndex = 0; controlIndex < m_controls.size(); ++controlIndex)
		{
			const ControlPosition& cp = std::get<1>(m_controls[controlIndex]);
			if (cp.RowIndex == iii && cp.RowSpan > 1)
			{
				// We need to loop over all rows after iii that the control spans and subtract the height they contain
				float requiredHeight = controlRequiredHeights[controlIndex];
				for (unsigned int jjj = iii + 1; jjj < cp.RowIndex + cp.RowSpan; ++jjj)
				{
					// NOTE: We ignore STAR type because star rows just take up all available space after everything else has been determined
					switch (m_rows[jjj].Type)
					{
					case RowColumnType::FIXED:		requiredHeight -= m_rows[jjj].Value; break;
					case RowColumnType::PERCENT:	requiredHeight -= ((m_rows[jjj].Value / 100) * m_rect.Height()); break;
					case RowColumnType::AUTO:		requiredHeight -= autoRowHeights[jjj]; break;
					}
				}

				autoRowHeights[iii] = std::max(autoRowHeights[iii], requiredHeight);
			}
		}
		for (unsigned int sublayoutIndex = 0; sublayoutIndex < m_sublayouts.size(); ++sublayoutIndex)
		{
			const ControlPosition& cp = std::get<1>(m_sublayouts[sublayoutIndex]);
			if (cp.RowIndex == iii && cp.RowSpan > 1)
			{
				// We need to loop over all rows after iii that the sublayout spans and subtract the height they contain
				float requiredHeight = subLayoutRequiredHeights[sublayoutIndex];
				for (unsigned int jjj = iii + 1; jjj < cp.RowIndex + cp.RowSpan; ++jjj)
				{
					// NOTE: We ignore STAR type because star rows just take up all available space after everything else has been determined
					switch (m_rows[jjj].Type)
					{
					case RowColumnType::FIXED:		requiredHeight -= m_rows[jjj].Value; break;
					case RowColumnType::PERCENT:	requiredHeight -= ((m_rows[jjj].Value / 100) * m_rect.Height()); break;
					case RowColumnType::AUTO:		requiredHeight -= autoRowHeights[jjj]; break;
					}
				}

				autoRowHeights[iii] = std::max(autoRowHeights[iii], requiredHeight);
			}
		}

		// Layout::Once we reach here, we are done adjusting this row, so go ahead and assign its final value
		m_rows[iii].Value = autoRowHeights[iii];
	}
}
void Layout::UpdateAutoColumnWidths() noexcept
{
	// Create a vector the same size as the number of total columns that will hold the required
	// width values for the auto columns (NOTE: elements of the vector that correspond to non-auto
	// columns will not get used)
	std::vector<float> autoColumnWidths(m_columns.size());

	// Loop over each control/sublayout to compute required widths and cache the values
	std::vector<float> controlRequiredWidths(m_controls.size());
	std::vector<float> sublayoutRequiredWidths(m_sublayouts.size());
	for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
	{
		// Calling GetAutoWidth is expensive so only do it for the controls that reside in an auto row
		const ControlPosition& cp = std::get<1>(m_controls[iii]);
		for (unsigned int columnIndex = cp.ColumnIndex; iii < cp.ColumnIndex + cp.ColumnSpan; ++columnIndex)
		{
			if (m_columns[columnIndex].Type == RowColumnType::AUTO)
			{
				controlRequiredWidths[iii] = std::get<0>(m_controls[iii])->GetAutoWidth();
				break;
			}
		}
	}
	for (unsigned int iii = 0; iii < m_sublayouts.size(); ++iii)
	{
		// Calling GetAutoWidth is expensive so only do it for the sublayouts that reside in an auto row
		const ControlPosition& cp = std::get<1>(m_sublayouts[iii]);
		for (unsigned int columnIndex = cp.ColumnIndex; columnIndex < cp.ColumnIndex + cp.ColumnSpan; ++columnIndex)
		{
			if (m_columns[columnIndex].Type == RowColumnType::AUTO)
			{
				sublayoutRequiredWidths[iii] = std::get<0>(m_sublayouts[iii])->GetAutoWidth();
				break;
			}
		}
	}

	// First generate an auto width value for only those controls/sublayouts that reside in
	// an AUTO column and do not span into any other columns
	for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
	{
		const ControlPosition& cp = std::get<1>(m_controls[iii]);
		if (m_columns[cp.ColumnIndex].Type == RowColumnType::AUTO && cp.ColumnSpan == 1)
			autoColumnWidths[cp.ColumnIndex] = std::max(autoColumnWidths[cp.ColumnIndex], controlRequiredWidths[iii]);
	}
	for (unsigned int iii = 0; iii < m_sublayouts.size(); ++iii)
	{
		const ControlPosition& cp = std::get<1>(m_sublayouts[iii]);
		if (m_columns[cp.ColumnIndex].Type == RowColumnType::AUTO && cp.ColumnSpan == 1)
			autoColumnWidths[cp.ColumnIndex] = std::max(autoColumnWidths[cp.ColumnIndex], controlRequiredWidths[iii]);
	}

	// Now we loop over all auto columns in order and subtract required width from the columns that come after them
	// for controls/sublayouts that have a column span >1
	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
	{
		if (m_columns[iii].Type != RowColumnType::AUTO)
			continue;

		// Now, loop over controls and sublayouts again, but this time we target controls/sublayouts that
		// start in the AUTO column we are on, but have a column span >1
		for (unsigned int controlIndex = 0; controlIndex < m_controls.size(); ++controlIndex)
		{
			const ControlPosition& cp = std::get<1>(m_controls[controlIndex]);
			if (cp.ColumnIndex == iii && cp.ColumnSpan > 1)
			{
				// We need to loop over all columns after iii that the control spans and subtract the width they contain
				float requiredWidth = controlRequiredWidths[controlIndex];
				for (unsigned int jjj = iii + 1; jjj < cp.ColumnIndex + cp.ColumnSpan; ++jjj)
				{
					// NOTE: We ignore STAR type because star columns just take up all available space after everything else has been determined
					switch (m_columns[jjj].Type)
					{
					case RowColumnType::FIXED:		requiredWidth -= m_columns[jjj].Value; break;
					case RowColumnType::PERCENT:	requiredWidth -= ((m_columns[jjj].Value / 100) * m_rect.Width()); break;
					case RowColumnType::AUTO:		requiredWidth -= autoColumnWidths[jjj]; break;
					}
				}

				autoColumnWidths[iii] = std::max(autoColumnWidths[iii], requiredWidth);
			}
		}
		for (unsigned int sublayoutIndex = 0; sublayoutIndex < m_sublayouts.size(); ++sublayoutIndex)
		{
			const ControlPosition& cp = std::get<1>(m_sublayouts[sublayoutIndex]);
			if (cp.ColumnIndex == iii && cp.ColumnSpan > 1)
			{
				// We need to loop over all columns after iii that the sublayout spans and subtract the width they contain
				float requiredWidth = sublayoutRequiredWidths[sublayoutIndex];
				for (unsigned int jjj = iii + 1; jjj < cp.ColumnIndex + cp.ColumnSpan; ++jjj)
				{
					// NOTE: We ignore STAR type because star columns just take up all available space after everything else has been determined
					switch (m_columns[jjj].Type)
					{
					case RowColumnType::FIXED:		requiredWidth -= m_columns[jjj].Value; break;
					case RowColumnType::PERCENT:	requiredWidth -= ((m_columns[jjj].Value / 100) * m_rect.Width()); break;
					case RowColumnType::AUTO:		requiredWidth -= autoColumnWidths[jjj]; break;
					}
				}

				autoColumnWidths[iii] = std::max(autoColumnWidths[iii], requiredWidth);
			}
		}

		// Layout::Once we reach here, we are done adjusting this column, so go ahead and assign its final value
		m_columns[iii].Value = autoColumnWidths[iii];
	}
}

bool Layout::AdjustControlAndSublayoutRowPositioning() noexcept
{
	bool changeMade = false;
	const unsigned int maxRowIndex = static_cast<unsigned int>(m_rows.size()) - 1;
	for (std::pair<std::unique_ptr<Control>, ControlPosition>& pair : m_controls)
	{
		ControlPosition& cp = std::get<1>(pair);
		if (cp.RowIndex > maxRowIndex)
		{
			cp.RowIndex = maxRowIndex;
			changeMade = true;
		}
		
		if (cp.RowIndex + cp.RowSpan - 1 > maxRowIndex)
		{
			cp.RowSpan = maxRowIndex - cp.RowIndex + 1;
			changeMade = true;
		}
	}

	for (std::pair<std::unique_ptr<Layout>, ControlPosition>& pair : m_sublayouts)
	{
		ControlPosition& cp = std::get<1>(pair);
		if (cp.RowIndex > maxRowIndex)
		{
			cp.RowIndex = maxRowIndex;
			changeMade = true;
		}

		if (cp.RowIndex + cp.RowSpan - 1 > maxRowIndex)
		{
			cp.RowSpan = maxRowIndex - cp.RowIndex + 1;
			changeMade = true;
		}
	}

	return changeMade;
}
bool Layout::AdjustControlAndSublayoutColumnPositioning() noexcept
{
	bool changeMade = false;
	const unsigned int maxColumnIndex = static_cast<unsigned int>(m_columns.size()) - 1;
	for (std::pair<std::unique_ptr<Control>, ControlPosition>& pair : m_controls)
	{
		ControlPosition& cp = std::get<1>(pair); 
		if (cp.ColumnIndex > maxColumnIndex)
		{
			cp.ColumnIndex = maxColumnIndex;
			changeMade = true;
		}

		if (cp.ColumnIndex + cp.ColumnSpan - 1 > maxColumnIndex)
		{
			cp.ColumnSpan = maxColumnIndex - cp.ColumnIndex + 1;
			changeMade = true;
		}
	}

	for (std::pair<std::unique_ptr<Layout>, ControlPosition>& pair : m_sublayouts)
	{
		ControlPosition& cp = std::get<1>(pair);
		if (cp.ColumnIndex > maxColumnIndex)
		{
			cp.ColumnIndex = maxColumnIndex;
			changeMade = true;
		}

		if (cp.ColumnIndex + cp.ColumnSpan - 1 > maxColumnIndex)
		{
			cp.ColumnSpan = maxColumnIndex - cp.ColumnIndex + 1;
			changeMade = true;
		}
	}
	return changeMade;
}

bool Layout::CheckMouseOverDraggableRowOrColumn(float x, float y) noexcept
{
	ASSERT(m_rows.size() > 0, "Invalid to have 0 rows");
	ASSERT(m_columns.size() > 0, "Invalid to have 0 columns");

	m_rowDraggingIndex = std::nullopt; 
	m_columnDraggingIndex = std::nullopt;

	for (unsigned int iii = 0; iii < m_rows.size() - 1; ++iii)
	{
		if (m_rows[iii].Adjustable && m_rows[iii + 1].Adjustable)
		{
			Rect rect{ m_rect.Left, m_rows[iii].Rect.Bottom - 2.0f, m_rect.Right, m_rows[iii].Rect.Bottom + 2.0f };
			if (rect.ContainsPoint(x, y))
			{
				m_rowDraggingIndex = iii;
				return true;
			}
		}
	}
	for (unsigned int iii = 0; iii < m_columns.size() - 1; ++iii)
	{
		if (m_columns[iii].Adjustable && m_columns[iii + 1].Adjustable)
		{
			Rect rect{ m_columns[iii].Rect.Right - 2.0f, m_rect.Top, m_columns[iii].Rect.Right + 2.0f, m_rect.Bottom };
			if (rect.ContainsPoint(x, y))
			{
				m_columnDraggingIndex = iii;
				return true;
			}
		}
	}
	return false;
}

float Layout::GetAutoHeight() const noexcept 
{ 
	float requiredHeight = 0.0f;

	// First, loop over the rows and sum rows that have FIXED height
	for (const Row& row : m_rows)
	{
		if (row.Type == RowColumnType::FIXED)
			requiredHeight += row.Value;
	}

	// Loop over the controls and determine their required heights (but skip controls that reside in FIXED height rows)
	for (const auto& pair : m_controls)
	{
		Control* control = std::get<0>(pair).get();
		const ControlPosition& cp = std::get<1>(pair);

		// Skip the control if it only resides FIXED height rows
		bool canSkip = true;
		for (unsigned int iii = cp.RowIndex; iii < cp.RowIndex + cp.RowSpan; ++iii) 
		{
			if (m_rows[iii].Type != RowColumnType::FIXED)
			{
				canSkip = false;
				break;
			}
		}

		if (canSkip)
			continue;

		// This control resides at least partially in AUTO/STAR/PERCENT rows. Therefore we need to get its required height
		// and subtract out the amount it resides in any FIXED rows to get the remaining amount
		float controlRequiredHeight = control->GetAutoHeight();
		for (unsigned int iii = cp.RowIndex; iii < cp.RowIndex + cp.RowSpan; ++iii)
		{
			if (m_rows[iii].Type == RowColumnType::FIXED)
				controlRequiredHeight -= m_rows[iii].Value;
		}
		
		requiredHeight += std::max(0.0f, controlRequiredHeight);
	}

	// Loop over the sublayouts and determine their required heights (but skip sublayouts that reside in FIXED height rows)
	for (const auto& pair : m_sublayouts)
	{
		Layout* layout = std::get<0>(pair).get();
		const ControlPosition& cp = std::get<1>(pair);

		// Skip the sublayout if it only resides FIXED height rows
		bool canSkip = true;
		for (unsigned int iii = cp.RowIndex; iii < cp.RowIndex + cp.RowSpan; ++iii)
		{
			if (m_rows[iii].Type != RowColumnType::FIXED)
			{
				canSkip = false;
				break;
			}
		}

		if (canSkip)
			continue;

		// This sublayout resides at least partially in AUTO/STAR/PERCENT rows. Therefore we need to get its required height
		// and subtract out the amount it resides in any FIXED rows to get the remaining amount
		float sublayoutRequiredHeight = layout->GetAutoHeight();
		for (unsigned int iii = cp.RowIndex; iii < cp.RowIndex + cp.RowSpan; ++iii)
		{
			if (m_rows[iii].Type == RowColumnType::FIXED)
				sublayoutRequiredHeight -= m_rows[iii].Value;
		}

		requiredHeight += std::max(0.0f, sublayoutRequiredHeight);
	}

	return requiredHeight; 
}
float Layout::GetAutoWidth() const noexcept 
{ 
	float requiredWidth = 0.0f;

	// First, loop over the columns and sum columns that have FIXED width
	for (const Column& column : m_columns)
	{
		if (column.Type == RowColumnType::FIXED)
			requiredWidth += column.Value;
	}

	// Loop over the controls and determine their required widths (but skip controls that reside in FIXED width columns)
	for (const auto& pair : m_controls)
	{
		Control* control = std::get<0>(pair).get();
		const ControlPosition& cp = std::get<1>(pair);

		// Skip the control if it only resides FIXED width columns
		bool canSkip = true;
		for (unsigned int iii = cp.ColumnIndex; iii < cp.ColumnIndex + cp.ColumnSpan; ++iii)
		{
			if (m_columns[iii].Type != RowColumnType::FIXED)
			{
				canSkip = false;
				break;
			}
		}

		if (canSkip)
			continue;

		// This control resides at least partially in AUTO/STAR/PERCENT columns. Therefore we need to get its required width
		// and subtract out the amount it resides in any FIXED columns to get the remaining amount
		float controlRequiredWidth = control->GetAutoWidth();
		for (unsigned int iii = cp.ColumnIndex; iii < cp.ColumnIndex + cp.ColumnSpan; ++iii)
		{
			if (m_columns[iii].Type == RowColumnType::FIXED)
				controlRequiredWidth -= m_columns[iii].Value;
		}

		requiredWidth += std::max(0.0f, controlRequiredWidth);
	}

	// Loop over the sublayouts and determine their required widths (but skip sublayouts that reside in FIXED width columns)
	for (const auto& pair : m_sublayouts)
	{
		Layout* layout = std::get<0>(pair).get();
		const ControlPosition& cp = std::get<1>(pair);

		// Skip the sublayout if it only resides FIXED width columns
		bool canSkip = true;
		for (unsigned int iii = cp.ColumnIndex; iii < cp.ColumnIndex + cp.ColumnSpan; ++iii)
		{
			if (m_columns[iii].Type != RowColumnType::FIXED)
			{
				canSkip = false;
				break;
			}
		}

		if (canSkip)
			continue;

		// This sublayout resides at least partially in AUTO/STAR/PERCENT columns. Therefore we need to get its required width
		// and subtract out the amount it resides in any FIXED columns to get the remaining amount
		float sublayoutRequiredWidth = layout->GetAutoWidth();
		for (unsigned int iii = cp.ColumnIndex; iii < cp.ColumnIndex + cp.ColumnSpan; ++iii)
		{
			if (m_columns[iii].Type == RowColumnType::FIXED)
				sublayoutRequiredWidth -= m_columns[iii].Value;
		}

		requiredWidth += std::max(0.0f, sublayoutRequiredWidth);
	}

	return requiredWidth;
}


// Window Event Methods
void Layout::OnWindowClosed() 
{ 
	// Send event to all controls and sublayouts
	for (auto& pair : m_controls)
		std::get<0>(pair)->OnWindowClosed();

	for (auto& pair : m_sublayouts)
		std::get<0>(pair)->OnWindowClosed();
}
void Layout::OnKillFocus() 
{ 
	// Send event to all controls and sublayouts
	for (auto& pair : m_controls)
		std::get<0>(pair)->OnKillFocus();

	for (auto& pair : m_sublayouts)
		std::get<0>(pair)->OnKillFocus();
}

// Mouse Event Methods
IEventReceiver* Layout::OnLButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// First, check if we are hovering over a row or column that can be adjusted
	if (m_rowDraggingIndex.has_value() || m_columnDraggingIndex.has_value())
	{
		m_activelyDragging = true;
		return this;
	}

	// Don't pass event to child controls/sublayouts if the mouse is not over the layout
	if (ContainsPoint(mouseX, mouseY))
	{
		IEventReceiver* ret = nullptr;
		for (auto& pair : m_controls)
		{
			ret = std::get<0>(pair)->OnLButtonDown(mouseX, mouseY, keyStates);
			if (ret != nullptr)
				return ret;
		}
		for (auto& pair : m_sublayouts)
		{
			ret = std::get<0>(pair)->OnLButtonDown(mouseX, mouseY, keyStates);
			if (ret != nullptr)
				return ret;
		}

		return this;
	}
	return nullptr;
}
IEventReceiver* Layout::OnLButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// If we were actively dragging, then set to false and check if mouse still resides over a draggable row/column boundary
	if (m_activelyDragging)
	{
		m_activelyDragging = false;
		CheckMouseOverDraggableRowOrColumn(mouseX, mouseY);
		return this;
	}

	// Don't pass event to child controls/sublayouts if the mouse is not over the layout
	if (ContainsPoint(mouseX, mouseY))
	{
		IEventReceiver* ret = nullptr;
		for (auto& pair : m_controls)
		{
			ret = std::get<0>(pair)->OnLButtonUp(mouseX, mouseY, keyStates);
			if (ret != nullptr)
				return ret;
		}
		for (auto& pair : m_sublayouts)
		{
			ret = std::get<0>(pair)->OnLButtonUp(mouseX, mouseY, keyStates);
			if (ret != nullptr)
				return ret;
		}

		return this;
	}
	return nullptr;
}
IEventReceiver* Layout::OnLButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnMButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnMButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnMButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnRButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnRButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnRButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnX1ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnX1ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnX1ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnX2ButtonDown(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnX2ButtonUp(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnX2ButtonDoubleClick(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnMouseMoved(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	// NOTE: When rows/columns are dragged, we need a way to make sure the layout doesn't snap back to original values. 
	// For example, suppose we have two rows, each with height 100, then we drag the dividing line so that they become 
	// 110 and 90. If we then resize the window, we need to make sure we have actually set the rows' "Value" field to the
	// new values and not have just resized the "Rect" field, otherwise, the resizing of the window will use the "Value"
	// and the rows will snap back to their original sizes.
	// Also consider the scenario where we have three rows, each with a height of 100, but the first row is a Fixed row, and 
	// the other two each have a star value of 1. Now suppose we drag the dividing line between rows 1 and 2 such that the first row
	// has a new height of 110 and the second has a height of 90. We need to make sure we also update the star row's
	// "Value" field so that it has a star value of 90 / 190. This is necessary because suppose the layout was immediately 
	// readjusted - the 190 pixels available to the star rows would get divided equally to 95 each. So adjusting rows 1 & 2
	// could later have the effect of resizing any other star rows. (Note: the only time this is not necessary is if there is
	// only a single star row amoung all the rows, in which case, the star value should probably just be 1)

	if (m_activelyDragging)
	{
		if (m_rowDraggingIndex.has_value())
		{
			unsigned int rowIndex = m_rowDraggingIndex.value();
			Row& topRow = m_rows[rowIndex];
			Row& bottomRow = m_rows[rowIndex + 1];

			float currentDividingLineY = topRow.Rect.Bottom;
			float newDividingLineY = mouseY;
			
			// Drag is upward
			if (mouseY < currentDividingLineY)
			{
				// adjust the new dividing line Y value if this move would make the top row below its minimum height
				if (topRow.MinHeight.has_value())
					newDividingLineY = std::max(newDividingLineY, topRow.Rect.Top + topRow.MinHeight.value());
				
				// adjust the new dividing line Y value if this move would make the bottom row greater than its maximum height
				if (bottomRow.MaxHeight.has_value())
					newDividingLineY = std::max(newDividingLineY, bottomRow.Rect.Bottom - bottomRow.MaxHeight.value());
			}
			// Drag is downward
			else
			{
				// adjust the new dividing line Y value if this move would make the top row above its maximum height
				if (topRow.MaxHeight.has_value())
					newDividingLineY = std::min(newDividingLineY, topRow.Rect.Top + topRow.MaxHeight.value());

				// adjust the new dividing line Y value if this move would make the bottom row less than its minimum height
				if (bottomRow.MinHeight.has_value())
					newDividingLineY = std::min(newDividingLineY, bottomRow.Rect.Bottom - bottomRow.MinHeight.value());
			}

			// If the new dividing line is extremely close to the current line, then no change is necessary, so just return
			if (std::abs(newDividingLineY - currentDividingLineY) < 0.005f)
				return this;

			// Adjust the top row
			float newHeight = newDividingLineY - topRow.Rect.Top;
			switch (topRow.Type)
			{
			case RowColumnType::AUTO:
			case RowColumnType::FIXED:   topRow.Value = newHeight; break;
			case RowColumnType::PERCENT: topRow.Value = (newHeight / m_rect.Height()) * 100; break;
			case RowColumnType::STAR:  	 topRow.Value = (topRow.Value / topRow.Rect.Height()) * newHeight; break;
			}
			ReadjustControlsAndSublayoutsInRow(rowIndex);

			// Adjust the bottom row
			newHeight = bottomRow.Rect.Bottom - newDividingLineY;
			switch (bottomRow.Type)
			{
			case RowColumnType::AUTO:
			case RowColumnType::FIXED:   bottomRow.Value = newHeight; break;
			case RowColumnType::PERCENT: bottomRow.Value = (newHeight / m_rect.Height()) * 100; break;
			case RowColumnType::STAR:  	 bottomRow.Value = (bottomRow.Value / bottomRow.Rect.Height()) * newHeight; break;
			}
			ReadjustControlsAndSublayoutsInRow(rowIndex + 1);
		}
		else if (m_columnDraggingIndex.has_value())
		{
			unsigned int columnIndex = m_columnDraggingIndex.value();
			Column& leftColumn = m_columns[columnIndex];
			Column& rightColumn = m_columns[columnIndex + 1];

			float currentDividingLineX = leftColumn.Rect.Right;
			float newDividingLineX = mouseX;

			// Drag is left
			if (mouseX < currentDividingLineX)
			{
				// adjust the new dividing line X value if this move would make the left column below its minimum width
				if (leftColumn.MinWidth.has_value())
					newDividingLineX = std::max(newDividingLineX, leftColumn.Rect.Left + leftColumn.MinWidth.value()); 

				// adjust the new dividing line X value if this move would make the right column above its maximum width
				if (rightColumn.MaxWidth.has_value())
					newDividingLineX = std::max(newDividingLineX, rightColumn.Rect.Right - rightColumn.MaxWidth.value());
			}
			// Drag is right
			else
			{
				// adjust the new dividing line X value if this move would make the left column above its maximum width
				if (leftColumn.MaxWidth.has_value()) 
					newDividingLineX = std::min(newDividingLineX, leftColumn.Rect.Left + leftColumn.MaxWidth.value());

				// adjust the new dividing line X value if this move would make the right column below its minimum width
				if (rightColumn.MinWidth.has_value())
					newDividingLineX = std::min(newDividingLineX, rightColumn.Rect.Right - rightColumn.MinWidth.value());
			}

			// If the new dividing line is extremely close to the current line, then no change is necessary, so just return
			if (std::abs(newDividingLineX - currentDividingLineX) < 0.005f)
				return this;

			// Adjust the left column
			float newWidth = newDividingLineX - leftColumn.Rect.Left;
			switch (leftColumn.Type)
			{
			case RowColumnType::AUTO:
			case RowColumnType::FIXED:   leftColumn.Value = newWidth; break;
			case RowColumnType::PERCENT: leftColumn.Value = (newWidth / m_rect.Width()) * 100; break;
			case RowColumnType::STAR:  	 leftColumn.Value = (leftColumn.Value / leftColumn.Rect.Width()) * newWidth; break;
			}
			ReadjustControlsAndSublayoutsInColumn(columnIndex);

			// Adjust the bottom row
			newWidth = rightColumn.Rect.Right - newDividingLineX;
			switch (rightColumn.Type)
			{
			case RowColumnType::AUTO:
			case RowColumnType::FIXED:   rightColumn.Value = newWidth; break;
			case RowColumnType::PERCENT: rightColumn.Value = (newWidth / m_rect.Width()) * 100; break;
			case RowColumnType::STAR:  	 rightColumn.Value = (rightColumn.Value / rightColumn.Rect.Width()) * newWidth; break;
			}
			ReadjustControlsAndSublayoutsInColumn(columnIndex + 1);
		}
		else
		{
			LOG_ERROR("[Layout: {0}] Something went wrong. m_activelyDragging is true, but there is no row/column dragging index value", m_name);
		}

		return this;
	}

	if (CheckMouseOverDraggableRowOrColumn(mouseX, mouseY))
		return this;

	// Don't pass event to child controls/sublayouts if the mouse is not over the layout
	if (ContainsPoint(mouseX, mouseY))
	{
		IEventReceiver* ret = nullptr;
		for (auto& pair : m_controls)
		{
			ret = std::get<0>(pair)->OnMouseMoved(mouseX, mouseY, keyStates);
			if (ret != nullptr)
				return ret;
		}
		for (auto& pair : m_sublayouts)
		{
			ret = std::get<0>(pair)->OnMouseMoved(mouseX, mouseY, keyStates);
			if (ret != nullptr)
				return ret;
		}

		return this;
	}

	return nullptr;
}
IEventReceiver* Layout::OnMouseEntered(float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnMouseLeave()
{
	return nullptr;
}
IEventReceiver* Layout::OnMouseWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}
IEventReceiver* Layout::OnMouseHWheel(float wheelDelta, float mouseX, float mouseY, MouseButtonEventKeyStates keyStates)
{
	return nullptr;
}

// Keyboard Event Methods
IEventReceiver* Layout::OnChar(unsigned int character, unsigned int repeatCount)
{
	return nullptr;
}
IEventReceiver* Layout::OnKeyDown(KeyCode keyCode, unsigned int repeatCount)
{
	return nullptr;
}
IEventReceiver* Layout::OnKeyUp(KeyCode keyCode, unsigned int repeatCount)
{
	return nullptr;
}
IEventReceiver* Layout::OnSysKeyDown(KeyCode keyCode, unsigned int repeatCount)
{
	return nullptr;
}
IEventReceiver* Layout::OnSysKeyUp(KeyCode keyCode, unsigned int repeatCount)
{
	return nullptr;
}




}