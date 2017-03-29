#include "filterheader.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QContextMenuEvent>

static const int filterReactionTime = 200; // in millisecons

FilterHeader::FilterHeader(QWidget *parent)
	: QHeaderView(Qt::Horizontal, parent), _menu(tr("Show or hide columns"), this)
{
	setSectionsClickable(true);
	setSectionsMovable(true);
	setDefaultAlignment(Qt::AlignTop | Qt::AlignHCenter);
	setContextMenuPolicy(Qt::DefaultContextMenu);

	connect(this, SIGNAL(sectionResized(int, int, int)),
			this, SLOT(handleSectionResized(int)));
    connect(this, SIGNAL(sectionMoved(int, int, int)),
			this, SLOT(handleSectionMoved(int, int, int)));

	connect(&_actionSignalMapper, SIGNAL(mapped(int)),
			this, SLOT(showOrHideColumn(int)));
	connect(&_filterSignalMapper, SIGNAL(mapped(int)),
			this, SLOT(filterChanged(int)));

	_filterTimer.setSingleShot(true);
	connect(&_filterTimer, SIGNAL(timeout()),
			this, SLOT(triggerFilter()));
}

FilterHeader::~FilterHeader()
{
}

void FilterHeader::fixWidgetPositions()
{
	for (int i = 0; i < count(); ++i)
        setFilterGeometry(i);
}

void FilterHeader::setCheckColumns(const QList<int> &checkColumns)
{
	_checkColumns = checkColumns;
}

void FilterHeader::contextMenuEvent(QContextMenuEvent* event)
{
    _menu.popup(event->globalPos());
}

void FilterHeader::showEvent(QShowEvent *e)
{
	QFont font;
	QFontMetrics fm(font);
	_textHeight = 0;

	// Create our filters and actions and determine max height via each section name
    for (int i = 0; i < count(); ++i) {
		if (!_filters[i]) {
			createFilter(i);
			createAction(i);
		}
		QString title = model()->headerData(i, orientation()).toString();

		// Cannot use boundingRect(QString) due possible bug in Qt
		// see https://bugreports.qt.io/browse/QTBUG-15974
		QRect rect = fm.boundingRect(QRect(0,0,0,0), Qt::AlignLeft|Qt::AlignBottom, title);
		if (rect.height() > _textHeight)
			_textHeight = rect.height();
    }
	// Now add some space and set height of header
	_textHeight += 10;
	setFixedHeight(_textHeight + QLineEdit().sizeHint().height() + 17);

	// Finally show each filter
	for (int i = 0; i < count(); ++i) {
		setFilterGeometry(i);
		_filters[i]->show();
	}
    QHeaderView::showEvent(e);
}

void FilterHeader::createFilter(int column)
{
	if (_checkColumns.contains(column)) {
	    QCheckBox *box = new QCheckBox(this);
	    box->setTristate(true);
		box->setCheckState(Qt::CheckState::PartiallyChecked);

	    connect(box, SIGNAL(stateChanged(int)),
			    &_filterSignalMapper, SLOT(map()));
		_filterSignalMapper.setMapping(box, column);
	    _filters[column] = box;
    }
    else {
	    QLineEdit *edit = new QLineEdit(this);
	    edit->setPlaceholderText(tr("Filter"));
		edit->setToolTip(tr("Wildcards *, ? oder [] erlaubt. Zum Freistellen \\ vor die Wildcard setzen"));

	    connect(edit, SIGNAL(textChanged(QString)),
			    &_filterSignalMapper, SLOT(map()));
		_filterSignalMapper.setMapping(edit, column);
	    _filters[column] = edit;
    }
}

void FilterHeader::createAction(int column)
{
	QString title = model()->headerData(column, orientation()).toString();
	QAction *action = new QAction(title.simplified(), &_menu);
	action->setCheckable(true);
	action->setChecked(!isSectionHidden(column));

	connect(action, SIGNAL(triggered()),
			&_actionSignalMapper, SLOT(map()));
	_actionSignalMapper.setMapping(action, column);
	_columnActions.insert(column, action);
	_menu.addAction(action);
}

void FilterHeader::handleSectionResized(int logical)
{
    for (int i = visualIndex(logical); i < count(); ++i)
        setFilterGeometry(logicalIndex(i));
}

void FilterHeader::handleSectionMoved(int logical, int oldVisualIndex, int newVisualIndex)
{
    for (int i = qMin(oldVisualIndex, newVisualIndex); i < count(); ++i)
		setFilterGeometry(logicalIndex(i));
}

void FilterHeader::setFilterGeometry(int column)
{
	if (_filters[column]) {
		_filters[column]->setGeometry(sectionViewportPosition(column) + 5, _textHeight,
									  sectionSize(column) - 10, _filters[column]->height());
	}
}

void FilterHeader::showOrHideColumn(int column)
{
    if (!_columnActions.contains(column))
        return;

    QAction* action = _columnActions[column];
    if (action->isChecked()) {
        showSection(column);
    }
	else {
        // If the user hides every column then the table will disappear. This
        // guards against that. NB: hiddenCount reflects checked QAction's so
        // size-hiddenCount will be zero the moment they uncheck the last
        // section
        if (_columnActions.size() - hiddenCount() > 0) {
            hideSection(column);
        }
		else {
            // Otherwise, ignore the request and re-check this QAction
            action->setChecked(true);
        }
    }
}

int FilterHeader::hiddenCount()
{
    int count = 0;
	for (auto action : _columnActions) {
        if (!action->isChecked())
            ++count;
    }

    return count;
}

void FilterHeader::filterChanged(int column)
{
	if (_filterTimer.isActive() && _currentColumn != column) {
		// Let current column complete it's filtering first of all
		_filterTimer.stop();
		triggerFilter();
	}

	_currentColumn = column;
    _filterTimer.start(filterReactionTime);
}

void FilterHeader::triggerFilter()
{
	QLineEdit *edit = qobject_cast<QLineEdit*>(_filters[_currentColumn]);
	if (edit)
		emit stringFilterChanged(_currentColumn, edit->text());

	QCheckBox *box = qobject_cast<QCheckBox*>(_filters[_currentColumn]);
	if (box)
		emit stateFilterChanged(_currentColumn, box->checkState());
}
