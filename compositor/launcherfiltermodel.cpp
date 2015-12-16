#include "launcherfiltermodel.h"
#include <QDebug>

LauncherFilterModel::LauncherFilterModel()
{
    qDebug();
}

void LauncherFilterModel::setFilterSubstring(QString filterSubstring)
{
    if (m_filterSubstring == filterSubstring)
        return;

    m_filterSubstring = filterSubstring;
    emit filterSubstringChanged();
    invalidateFilter();
}

bool LauncherFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filterSubstring.isEmpty())
        return true;
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QObject *o = sourceModel()->data(index0, Qt::UserRole + 1).value<QObject*>();
    if (!o)
        return false;
    QString title = o->property("title").toString();
    QString exec = o->property("exec").toString();
    return title.contains(m_filterSubstring, Qt::CaseInsensitive) ||
            exec.contains(m_filterSubstring, Qt::CaseInsensitive);
}
