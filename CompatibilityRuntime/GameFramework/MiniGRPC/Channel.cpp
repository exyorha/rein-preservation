#include "Channel.h"
#include "ChannelArgs.h"


namespace minigrpc {
    Channel::Channel(const char *target, const ChannelArgs *args) : m_target(target) {
        (void)args;
    }

    Channel::~Channel() = default;
}

