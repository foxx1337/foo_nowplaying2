#pragma once

#include <SDK/foobar2000.h>
#include <list>

class QueueCallback
{
public:
    virtual void on_queue() = 0;
};


class QueueListener : public playback_queue_callback
{
public:
    void register_callback(QueueCallback* callback);

    void unregister_callback(QueueCallback* callback);

private:
    // Playback queue callback methods.
    void on_changed(t_change_origin p_origin) override;

    pfc::mutex list_lock_;
    std::list<QueueCallback*> callbacks_;
};

extern service_factory_single_t<QueueListener> g_queue;
