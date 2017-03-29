#include "sortfilterproxymodel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
	setSortCaseSensitivity(Qt::CaseInsensitive);
	setSortLocaleAware(true);
}

void SortFilterProxyModel::updateStringFilter(int column, const QString &pattern)
{
	if (!_columnPatterns.contains(column))
		_columnPatterns.insert(column, QRegExp(pattern, Qt::CaseInsensitive, QRegExp::WildcardUnix));
	else if (pattern.isEmpty())
		_columnPatterns.remove(column);
	else
		_columnPatterns[column].setPattern(pattern);

	// Prefer over invalidateFilter() due https://bugreports.qt.io/browse/QTBUG-14355
	invalidate();
}

void SortFilterProxyModel::updateStateFilter(int column, const Qt::CheckState &cs)
{
	if (!_columnStates.contains(column))
		_columnStates.insert(column, cs);
	else if (cs == Qt::CheckState::PartiallyChecked)
		_columnStates.remove(column);
	else
		_columnStates[column] = cs;

	// Prefer over invalidateFilter() due https://bugreports.qt.io/browse/QTBUG-14355
	invalidate();
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	for (QMap<int, QRegExp>::const_iterator it = _columnPatterns.constBegin();
		it != _columnPatterns.constEnd(); ++it) {

		QModelIndex index = sourceModel()->index(sourceRow, it.key(), sourceParent);
		if (!index.data().toString().contains(it.value()))
			return false;
	}

	for (QMap<int, Qt::CheckState>::const_iterator it = _columnStates.constBegin();
		it != _columnStates.constEnd(); ++it) {

		QModelIndex index = sourceModel()->index(sourceRow, it.key(), sourceParent);
		if (index.data(Qt::CheckStateRole) != it.value())
			return false;
	}

	return true;
}
