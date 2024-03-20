#ifndef STRATPANE_H
#define STRATPANE_H

#include <QGroupBox>
#include <QScrollArea>

#include <vector>

#include "stratagem.h"
#include "collapsible.h"

namespace hdvs {

class StratOpt : public collapsible::Box
{
    Q_OBJECT
public:
    explicit StratOpt(const Stratagem& strat, QWidget* parent = nullptr);

private:
    Stratagem m_stratagem;
}; // StratOpt

class StratPane : public QGroupBox
{
    Q_OBJECT
public:
    explicit StratPane(QWidget* parent = nullptr);

public slots:
    void AddStratagem(const QVariant& strat);

private:

    QScrollArea* m_scrollArea;

    std::vector<StratOpt*> m_stratOpts;
}; // class StratPane

} // namespace hdvs

#endif // STRATPANE_H
