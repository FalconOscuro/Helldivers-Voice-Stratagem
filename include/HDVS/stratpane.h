#ifndef STRATPANE_H
#define STRATPANE_H

#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>

#include <vector>

#include "stratagem.h"
#include "collapsible.h"

namespace hdvs {

class StratOpt : public collapsible::Box
{
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

    std::vector<collapsible::Box*> m_stratOpts;
}; // class StratPane

} // namespace hdvs

#endif // STRATPANE_H
