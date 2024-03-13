#include "HDVS/stratpane.h"

namespace hdvs {

StratPane::StratPane(QWidget* parent):
    QGroupBox(parent)
{
    setTitle("Stratagems");

    m_stratOpts = std::vector<collapsible::Box*>();
    
    QWidget* scrollContent = new QWidget;
    scrollContent->setLayout(new QVBoxLayout);

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(scrollContent);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_scrollArea);
}

void StratPane::AddStratagem(const QVariant& var)
{
    if (!var.canConvert<Stratagem>())
        return;

    Stratagem strat = var.value<Stratagem>();
    collapsible::Box* stratOpt = new collapsible::Box(QString::fromStdString(strat.name));
    stratOpt->SetIcon(QPixmap("./data/icons/" + QString::fromStdString(strat.GetIconName())));

    m_scrollArea->widget()->layout()->addWidget(stratOpt);
    m_stratOpts.push_back(stratOpt);
}

} // namespace hdvs
