#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit SortFilterProxyModel(QObject *parent = 0);

public slots:
	void updateStringFilter(int column, const QString &pattern);
	void updateStateFilter(int column, const Qt::CheckState &cs);

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
	QMap<int, QRegExp> _columnPatterns;
	QMap<int, Qt::CheckState> _columnStates;
};

#endif // SORTFILTERPROXYMODEL_H
