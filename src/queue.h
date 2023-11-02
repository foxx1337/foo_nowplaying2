#pragma once

#include <SDK/foobar2000.h>

class QueueCallback
{
public:
    virtual void on_queue() = 0;
};


class QueueListener : public playback_queue_callback
{
public:
    void register_callback(QueueCallback* callback) { callback_ = callback; }

    void unregister_callback() { callback_ = nullptr; }

private:
    // Playback queue callback methods.
    void on_changed(t_change_origin p_origin) override;

    QueueCallback* callback_;
};

extern service_factory_single_t<QueueListener> g_queue;
