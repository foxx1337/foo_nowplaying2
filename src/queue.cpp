#include "queue.h"

void QueueListener::register_callback(QueueCallback* p_callback)
{
    pfc::mutexScope list_lock_scope(list_lock_);
    callbacks_.push_back(p_callback);
}

void QueueListener::unregister_callback(QueueCallback* p_callback)
{
    pfc::mutexScope list_lock_scope(list_lock_);
    callbacks_.remove(p_callback);
}

void QueueListener::on_changed(t_change_origin p_origin)
{
    pfc::mutexScope list_lock_scope(list_lock_);
    for (const auto callback : callbacks_)
    {
        callback->on_queue();
    }
}

service_factory_single_t<QueueListener> g_queue;
