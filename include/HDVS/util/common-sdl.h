// SOURCE: https://github.com/ggerganov/whisper.cpp/blob/master/examples/common-sdl.h
#ifndef COMMON_SDL_H
#define COMMON_SDL_H

#include <SDL.h>
#include <SDL_audio.h>

#include <atomic>
#include <cstdint>
#include <vector>
#include <mutex>

namespace hdvs::util {

//
// SDL Audio capture
//

class audio_async {
public:
    audio_async(int len_ms = 5000);
    ~audio_async();

    bool init(int sample_rate, int capture_id = -1);

    // start capturing audio via the provided SDL callback
    // keep last len_ms seconds of audio in a circular buffer
    bool resume();
    bool pause();
    bool clear();
    bool running() const;

    // callback to be called by SDL
    void callback(uint8_t * stream, int len);

    // get audio data from the circular buffer
    void get(int ms, std::vector<float> & audio);

private:
    SDL_AudioDeviceID m_dev_id_in = 0;

    int m_len_ms = 0;
    int m_sample_rate = 0;

    std::atomic_bool m_running;
    std::mutex       m_mutex;

    std::vector<float> m_audio;
    size_t             m_audio_pos = 0;
    size_t             m_audio_len = 0;
}; // class audio_async

// Return false if need to quit
bool sdl_poll_events();

} // namespace hdvs::util

#endif // COMMON_SDL_H