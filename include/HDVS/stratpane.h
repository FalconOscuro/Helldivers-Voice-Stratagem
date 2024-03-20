#ifndef STRATPANE_H
#define STRATPANE_H

#include <QGroupBox>
#include <QScrollArea>
#include <QStringListModel>
#include <QCheckBox>

#include <vector>

#include "stratagem.h"
#include "collapsible.h"

namespace hdvs {

class StratOpt : public collapsible::Box
{
    Q_OBJECT
public:
    explicit StratOpt(const Stratagem& strat, QWidget* parent = nullptr);

    Stratagem GetState();

private:
    Stratagem m_stratagem;

    QStringListModel* m_triggerModelList;
    QCheckBox* m_enabled;
}; // StratOpt

class StratPane : public QGroupBox
{
    Q_OBJECT
public:
    explicit StratPane(QWidget* parent = nullptr);

signals:
    void UpdateStratagems(const QList<QVariant>& stratagems);

public slots:
    void AddStratagem(const QVariant& strat);

private slots:
    void OnUpdateStratagems();

private:

    QScrollArea* m_scrollArea;

    std::vector<StratOpt*> m_stratOpts;
}; // class StratPane

} // namespace hdvs

#endif // STRATPANE_H
