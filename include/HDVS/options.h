#ifndef OPTIONS_H
#define OPTIONS_H

#include <QGroupBox>
#include <QLineEdit>

#include "config.h"

namespace hdvs {

class OptionKey : public QWidget
{
    Q_OBJECT

public:
    explicit OptionKey (QString name, QWidget* parent = nullptr);

    int GetKey() const;

public slots:
    void SetKey(int key);

private:
    int m_key;
    QLineEdit* m_key_name;
}; // class OptionKey

class Options : public QGroupBox 
{
    Q_OBJECT

public:
    explicit Options(QWidget* parent = nullptr);

public slots:
    void ReceiveConfig(const hdvs::Config& cfg);

private:
    Config m_config;

    OptionKey* m_up;
    OptionKey* m_down;
    OptionKey* m_right;
    OptionKey* m_left;
    OptionKey* m_strat;
}; // class Options

} // namespace hdvs

#endif // OPTIONS_H