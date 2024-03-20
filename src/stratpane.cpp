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

    QCheckBox* enabled = new QCheckBox("Enabled");
    enabled->setChecked(strat.enabled);

    QStringList list;
    for (auto t = strat.trigger.begin(); t != strat.trigger.end(); t++)
        list.push_back(QString::fromStdString(*t));
    
    QStringListModel* trigList = new QStringListModel(list);
    QListView* listView = new QListView;
    listView->setModel(trigList);
    listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    // Add
    QPushButton* add = new QPushButton("Add");
    connect(add, &QPushButton::pressed, trigList, [=]() {
        trigList->insertRows(0, 1);

        auto index = trigList->index(0);
        listView->scrollTo(index);
        listView->edit(index);
    });
    // Remove
    QPushButton* remove = new QPushButton("Remove Selected");
    connect(remove, &QPushButton::pressed, trigList, [=]() {
        auto indexes = listView->selectionModel()->selectedIndexes();
        for (auto index : indexes)
            trigList->removeRow(index.row());
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
    layout->addWidget(enabled);
    SetContentLayout(layout);
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

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_scrollArea);
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

} // namespace hdvs
