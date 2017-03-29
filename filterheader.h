#ifndef FILTERHEADER_H
#define FILTERHEADER_H

#include <QHeaderView>
#include <QSignalMapper>
#include <QMenu>
#include <QTimer>

class FilterHeader : public QHeaderView
{
	Q_OBJECT
public:
	FilterHeader(QWidget *parent = 0);
	~FilterHeader();

	void fixWidgetPositions();
	void setCheckColumns(const QList<int> &checkColumns);

signals:
	void stringFilterChanged(int,QString);
	void stateFilterChanged(int,Qt::CheckState);

private slots:
	void handleSectionResized(int logical);
	void handleSectionMoved(int logical, int oldVisualIndex, int newVisualIndex);
    void showOrHideColumn(int);
	void filterChanged(int);
	void triggerFilter();

protected:
	virtual void showEvent(QShowEvent *e) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;

private:
	void setFilterGeometry(int column);
	void createFilter(int column);
	void createAction(int column);
	int hiddenCount();

	QList<int> _checkColumns;
	QMap<int, QAction*> _columnActions;
	QMap<int, QWidget*> _filters;
	QSignalMapper _actionSignalMapper;
	QSignalMapper _filterSignalMapper;
	QMenu _menu;
	QTimer _filterTimer;
	int _currentColumn;
	int _textHeight;
};

#endif // FILTERHEADER_H
