#include "pch.h"
#include "Layout.h"



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


namespace topo
{
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

	float rowStarHeight = CalculateRowStarHeight();
	float nextTop = m_rect.Top;

	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
	{
		Row& row = m_rows[iii];

		// In all cases, the rows will span the layout
		row.Rect.Left = m_rect.Left;
		row.Rect.Right = m_rect.Right;

		// If we have already filled the available space, just make the row not visible and their rect a single horizontal line
		if (nextTop >= m_rect.Bottom) [[unlikely]]
		{
			// Set visible to false to indicate the row (and all of its contents) should not be rendered
			row.Visible = false;

			// Make the row's rectangle just a single line 
			row.Rect.Top = m_rect.Bottom;
			row.Rect.Bottom = m_rect.Bottom;
			continue;
		}

		// Row values should only ever be 0 if it is an AUTO row and nothing has been placed inside it yet
		if (row.Value <= 0.0f && row.Type != RowColumnType::AUTO) [[unlikely]]
		{
			LOG_ERROR("[Layout: {0}] Row #{1} has an invalid height ({2}). Non-AUTO row heights must be greater than 0.", m_name, iii, row.Value);

			// Set visible to false to indicate the row (and all of its contents) should not be rendered
			row.Visible = false;

			// Make the row's rectangle just a single line 
			row.Rect.Top = nextTop;
			row.Rect.Bottom = nextTop;
			continue;
		}

		// If we have made it here, the row has a valid value and will at least be partially visible
		row.Visible = true;
		row.Rect.Top = nextTop;

		switch (row.Type)
		{
		// intentially fallthrough AUTO because it holds a fixed height value
		case RowColumnType::AUTO:
		case RowColumnType::FIXED:   nextTop += row.Value; break;
		case RowColumnType::PERCENT: nextTop += ((row.Value / 100) * m_rect.Height()); break;
		case RowColumnType::STAR:	 nextTop += (row.Value * rowStarHeight); break;
		default:
			ASSERT(false, "Unrecognized row type");
		}

		// Set the height using the computed height value (Don't allow it to go beyond the bottom of the layout)
		row.Rect.Bottom = std::min(nextTop, m_rect.Bottom);
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

	float rowStarWidth = CalculateColumnStarWidth();
	float nextLeft = m_rect.Left;

	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
	{
		Column& column = m_columns[iii];

		// In all cases, the columns will span the layout
		column.Rect.Top = m_rect.Top;
		column.Rect.Bottom = m_rect.Bottom;

		// If we have already filled the available space, just make the column not visible and their rect a single vertical line
		if (nextLeft >= m_rect.Right) [[unlikely]]
		{
			// Set visible to false to indicate the column (and all of its contents) should not be rendered
			column.Visible = false;

			// Make the column's rectangle just a single line 
			column.Rect.Left = m_rect.Right;
			column.Rect.Right = m_rect.Right;
			continue;
		}

		// Column values should only ever be 0 if it is an AUTO column and nothing has been placed inside it yet
		if (column.Value <= 0.0f && column.Type == RowColumnType::AUTO) [[unlikely]]
		{
			LOG_ERROR("[Layout: {0}] Column #{1} has an invalid width ({2}). Non-AUTO row heights must be greater than 0.", m_name, iii, column.Value);

			// Set visible to false to indicate the column (and all of its contents) should not be rendered
			column.Visible = false;

			// Make the column's rectangle just a single line 
			column.Rect.Top = nextLeft;
			column.Rect.Bottom = nextLeft;
			continue;
		}

		// If we have made it here, the column has a valid value and will at least be partially visible
		column.Visible = true;
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

		// Set the width using the computed height value (Don't allow it to go beyond the right of the layout)
		column.Rect.Right = std::min(nextLeft, m_rect.Right);
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
void Layout::ResetRows(std::span<Row> rows) noexcept
{
	// If their are fewer new rows, then we need to adjust any controls that reside in
	// rows that will no longer exist
	bool controlLocationsNeedAdjusting = (rows.size() < m_rows.size());

	m_rows.assign_range(rows);

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

	m_rows = std::move(rows);

	if (controlLocationsNeedAdjusting)
		AdjustControlAndSublayoutRowPositioning();

	// Adjust the row sizing (and controls/sublayouts
	ReadjustRows(true);
}
void Layout::ResetColumns(std::span<Column> columns) noexcept
{
	// If their are fewer new columns, then we need to adjust any controls that reside in
	// columns that will no longer exist
	bool controlLocationsNeedAdjusting = (columns.size() < m_columns.size());

	m_columns.assign_range(columns);

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

	m_columns = std::move(columns);

	if (controlLocationsNeedAdjusting)
		AdjustControlAndSublayoutRowPositioning();

	// Adjust the row sizing (and controls/sublayouts)
	ReadjustColumns(true);
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

		// Once we reach here, we are done adjusting this row, so go ahead and assign its final value
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

		// Once we reach here, we are done adjusting this column, so go ahead and assign its final value
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

}