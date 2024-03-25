#ifndef HDVS_H
#define HDVS_H

#include <QObject>
#include <whisper.h>
#include <vector>

#include "config.h"
#include "status.h"
#include "stratagem.h"
#include "util/common-sdl.h"

#define DATA_PATH               "./data"
#define CONFIG_PATH DATA_PATH   "/config.yml"
#define STRAT_PATH  DATA_PATH   "/stratagem.yml"

namespace hdvs {

class HDVS : public QObject
{
    Q_OBJECT
public:
    explicit HDVS(QObject* parent = nullptr);
    virtual ~HDVS() override;

signals:
    void SendLog(const QString& msg);
    void PhaseChange(hdvs::Status::Phase phase);

    void LoadStratagem(const hdvs::Stratagem& strat);

public slots:
    void UpdateStratagems(const QList<hdvs::Stratagem>& stratagems);
    void ForceExit();

private slots:
    void PostInit();

    void CheckState();
    void Listen();

private:
    bool StratKeyHeld() const;
    void SetTriggerWords();

    Config m_config;
    std::vector<Stratagem> m_stratagems;
    std::vector<std::pair<std::string, size_t>> m_triggers_id;

    util::audio_async* m_audio;
    whisper_context* m_wctx;
    std::vector<std::vector<whisper_token>> m_trigger_tokens;
    std::vector<whisper_token> m_trigger_prompt;

    bool m_running = false;
}; // class HDVS

} // namespace hdvs

#endif // HDVS_H