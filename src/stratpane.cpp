#include "HDVS/stratpane.h"

#include <QCheckBox>
#include <QStringListModel>
#include <QListView>
#include <QVBoxLayout>
#include <QPushButton>

#define LIST_BOX_HEIGHT 100

namespace hdvs {

StratOpt::StratOpt(const Stratagem& strat, QWidget* parent):
    collapsible::Box(QString::fromStdString(strat.name), parent)
{
    SetIcon(QPixmap("./data/icons/" + QString::fromStdString(strat.GetIconName())));
    m_stratagem = strat;

    m_enabled = new QCheckBox("Enabled");
    m_enabled->setChecked(strat.enabled);

    QStringList list;
    for (auto t = strat.trigger.begin(); t != strat.trigger.end(); t++)
        list.push_back(QString::fromStdString(*t));
    
    m_triggerModelList = new QStringListModel(list);
    QListView* listView = new QListView;
    listView->setModel(m_triggerModelList);
    listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    // Add
    QPushButton* add = new QPushButton("Add");
    connect(add, &QPushButton::pressed, m_triggerModelList, [=]() {
        m_triggerModelList->insertRows(0, 1);

        auto index = m_triggerModelList->index(0);
        listView->scrollTo(index);
        listView->edit(index);
    });
    // Remove
    QPushButton* remove = new QPushButton("Remove Selected");
    connect(remove, &QPushButton::pressed, m_triggerModelList, [=]() {
        auto indexes = listView->selectionModel()->selectedIndexes();
        for (auto index : indexes)
            m_triggerModelList->removeRow(index.row());
    });

    QWidget* buttons = new QWidget;
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttons);
    buttonLayout->setAlignment(Qt::AlignLeft);
    buttonLayout->addWidget(add);
    buttonLayout->addWidget(remove);

    QLabel* trigLabel = new QLabel("Triggers:");
    QWidget* trigArea = new QWidget;
    QVBoxLayout* trigLayout = new QVBoxLayout(trigArea);
    trigLayout->setSpacing(0);
    trigLayout->setContentsMargins(0, 0, 0, 0);
    trigLayout->addWidget(listView);
    trigLayout->addWidget(buttons);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(trigLabel);
    layout->addWidget(trigArea);
    layout->addWidget(m_enabled);
    SetContentLayout(layout);
}

Stratagem StratOpt::GetState()
{
    m_stratagem.enabled = m_enabled->isChecked();

    // TODO: format triggers

    m_stratagem.trigger.clear();
    QStringList triggers = m_triggerModelList->stringList();
    for(auto trigger = triggers.begin(); trigger != triggers.end(); trigger++)
        m_stratagem.trigger.push_back(trigger->toStdString());

    return m_stratagem;
}

StratPane::StratPane(QWidget* parent):
    QGroupBox(parent)
{
    setTitle("Stratagems");

    m_stratOpts = std::vector<StratOpt*>();
    
    QWidget* scrollContent = new QWidget;
    scrollContent->setLayout(new QVBoxLayout);

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(scrollContent);

    QPushButton* saveButton = new QPushButton("Apply Changes");
    connect(saveButton, &QPushButton::pressed, this, &StratPane::OnUpdateStratagems);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_scrollArea);
    layout->addWidget(saveButton);
}

void StratPane::AddStratagem(const QVariant& var)
{
    if (!var.canConvert<Stratagem>())
        return;

    Stratagem strat = var.value<Stratagem>();
    StratOpt* stratOpt = new StratOpt(strat);

    m_scrollArea->widget()->layout()->addWidget(stratOpt);
    m_stratOpts.push_back(stratOpt);
}

void StratPane::OnUpdateStratagems()
{
    QList<QVariant> stratagems;
    for(size_t i = 0; i < m_stratOpts.size(); i++)
    {
        QVariant varStrat;
        varStrat.setValue(m_stratOpts[i]->GetState());

        stratagems.push_back(varStrat);
    }

    emit UpdateStratagems(stratagems);
}

} // namespace hdvs
