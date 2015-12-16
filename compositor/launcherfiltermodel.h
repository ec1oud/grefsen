#ifndef LAUNCHERFILTERMODEL_H
#define LAUNCHERFILTERMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class LauncherFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterSubstring READ filterSubstring WRITE setFilterSubstring NOTIFY filterSubstringChanged)

public:
    LauncherFilterModel();

    QString filterSubstring() const { return m_filterSubstring; }
    void setFilterSubstring(QString filterSubstring);

//    bool LauncherFilterModel::lessThan(const QModelIndex &left,
//                                       const QModelIndex &right) const
//    {
//        QObject *leftObj = sourceModel()->data(left).value<QObject*>();
//        QObject *rightObj = sourceModel()->data(right).value<QObject*>();
//        return QString::localeAwareCompare(leftString, rightString) < 0;
//    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

signals:
    void filterSubstringChanged();

protected:
    QString m_filterSubstring;
};

#endif // LAUNCHERFILTERMODEL_H
