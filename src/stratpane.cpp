#include "HDVS/stratpane.h"

namespace hdvs {

StratPane::StratPane(QWidget* parent):
    QGroupBox(parent)
{
    setTitle("Stratagems");
    
    m_lay_scroll = new QVBoxLayout();
    QWidget* scrollContent = new QWidget;
    scrollContent->setLayout(m_lay_scroll);

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(scrollContent);

    m_lay = new QVBoxLayout(this);
    m_lay->addWidget(m_scrollArea);
}

StratPane::~StratPane()
{
    delete m_scrollArea;
    delete m_lay_scroll;
    delete m_lay;

    for (size_t i = 0; i < m_stratOpts.size(); i++)
        delete m_stratOpts[i];
}

void StratPane::AddStratagem(const QVariant& var)
{
    if (!var.canConvert<Stratagem>())
        return;

    Stratagem strat = var.value<Stratagem>();
    collapsible::Box* stratOpt = new collapsible::Box(QString::fromStdString(strat.name));

    m_lay_scroll->addWidget(stratOpt);
    m_stratOpts.push_back(stratOpt);
}

} // namespace hdvs
