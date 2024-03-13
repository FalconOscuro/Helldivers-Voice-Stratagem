#ifndef STRATPANE_H
#define STRATPANE_H

#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>

#include <vector>

#include "stratagem.h"
#include "collapsible.h"

namespace hdvs {

class StratPane : public QGroupBox
{
    Q_OBJECT
public:
    explicit StratPane(QWidget* parent = nullptr);
    ~StratPane() override;

public slots:
    void AddStratagem(const QVariant& strat);

private:

    QScrollArea* m_scrollArea;
    QVBoxLayout* m_lay_scroll;

    QVBoxLayout* m_lay;

    std::vector<collapsible::Box*> m_stratOpts;
}; // class StratPane

} // namespace hdvs

#endif // STRATPANE_H
