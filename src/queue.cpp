#include "queue.h"

void QueueListener::on_changed(t_change_origin p_origin)
{
    if (callback_ != nullptr)
    {
        callback_->on_queue();
    }
}

service_factory_single_t<QueueListener> g_queue;
