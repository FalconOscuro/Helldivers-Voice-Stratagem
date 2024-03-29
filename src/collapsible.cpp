#include "HDVS/collapsible.h"

namespace hdvs::collapsible {

Box::Box(QString title, QWidget* parent):
    QWidget(parent)
{
    m_button = new QToolButton;
    m_button->setText(title);
    m_button->setCheckable(true);
    m_button->setChecked(false);
    m_button->setStyleSheet("QToolButton { border: none; }");
    m_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_button->setArrowType(Qt::RightArrow);

    connect(m_button, &QToolButton::toggled, this, &Box::OnToggled);

    m_icon = new QLabel();
    m_icon->setAlignment(Qt::AlignmentFlag::AlignRight);

    m_lay_button = new QHBoxLayout;
    m_lay_button->setSpacing(0);
    m_lay_button->setContentsMargins(0, 0, 0, 0);
    m_lay_button->addWidget(m_button);
    m_lay_button->addWidget(m_icon);
    m_buttonArea = new QWidget;
    m_buttonArea->setLayout(m_lay_button);

    m_toggleAnimation = new QParallelAnimationGroup;

    m_contentArea = new QScrollArea;
    m_contentArea->setMaximumHeight(0);
    m_contentArea->setMinimumHeight(0);
    m_contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_contentArea->setFrameShape(QFrame::NoFrame);

    m_lay = new QVBoxLayout(this);
    m_lay->setSpacing(0);
    m_lay->setContentsMargins(0, 0, 0, 0);
    m_lay->addWidget(m_buttonArea);
    m_lay->addWidget(m_contentArea);

    const int ms_ANIM_LEN = 500;
    QPropertyAnimation* anim;

    anim = new QPropertyAnimation(this, "minimumHeight");
    anim->setDuration(ms_ANIM_LEN);
    anim->setStartValue(0);
    anim->setEndValue(0);
    m_toggleAnimation->addAnimation(anim);

    anim = new QPropertyAnimation(this, "maximumHeight");
    anim->setDuration(ms_ANIM_LEN);
    anim->setStartValue(sizeHint().height());
    anim->setEndValue(sizeHint().height());
    m_toggleAnimation->addAnimation(anim);

    anim = new QPropertyAnimation(m_contentArea, "maximumHeight");
    anim->setDuration(ms_ANIM_LEN);
    anim->setStartValue(0);
    anim->setEndValue(sizeHint().height());
    m_toggleAnimation->addAnimation(anim);
}

Box::~Box()
{
    delete m_lay_button;
    delete m_button;
    delete m_icon;
    delete m_buttonArea;

    delete m_toggleAnimation;
    delete m_contentArea;
    delete m_lay;
}

void Box::SetContentLayout(QLayout* layout)
{
    delete m_contentArea->layout();
    m_contentArea->setLayout(layout);
    
    int collapsedHeight = sizeHint().height();
    int contentHeight = m_contentArea->sizeHint().height();

    for (int i = 0; i < m_toggleAnimation->animationCount() - 1; i++)
    {
        QVariantAnimation* anim = dynamic_cast<QVariantAnimation*>(m_toggleAnimation->animationAt(i));

        if (anim == nullptr)
            continue;

        anim->setStartValue(collapsedHeight);
        anim->setEndValue(collapsedHeight + contentHeight);
    }

    // Special clase content area animation
    QVariantAnimation* anim = dynamic_cast<QVariantAnimation*>(m_toggleAnimation->animationAt(2));

    if (anim == nullptr)
        return;

    anim->setEndValue(contentHeight);
}

void Box::OnToggled(bool checked)
{
    m_button->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);

    m_toggleAnimation->setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    m_toggleAnimation->start();
}

void Box::SetIcon(const QPixmap& pm) {
    const float ICON_SCALE = 0.9f;
    m_icon->setPixmap(pm.scaledToHeight(static_cast<int>(sizeHint().height() * ICON_SCALE)));
}

QSize Box::sizeHint() const {
    return m_buttonArea->sizeHint();
}

} // namespace hdvs::collapsible
