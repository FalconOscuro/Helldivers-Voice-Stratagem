#ifndef COLLAPSIBLE_H
#define COLLAPSIBLE_H

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QScrollArea>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include <QHBoxLayout>
#include <QVBoxLayout>

namespace hdvs::collapsible {

class Box : public QWidget
{
    Q_OBJECT
public:
    explicit Box(QString title = "", QWidget* parent = nullptr);
    ~Box() override;

    void SetContentLayout(QLayout* layout);

protected slots:
    void OnToggled(bool checked);

protected:
    void SetIcon(const QPixmap& pm);

private:

    QHBoxLayout* m_lay_button;
    QWidget* m_buttonArea;
    QLabel* m_icon;
    QToolButton* m_button;

    QVBoxLayout* m_lay;
    QScrollArea* m_contentArea;
    QParallelAnimationGroup* m_toggleAnimation;
}; // class Box

} // namespace hdvs::collapsible

#endif // COLLAPSIBLE_H
