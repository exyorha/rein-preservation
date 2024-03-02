#include "ChannelArgs.h"

namespace minigrpc {

    ChannelArgs::ChannelArgs(size_t size) : m_args(size) {

    }

    ChannelArgs::~ChannelArgs() = default;

    void ChannelArgs::setString(size_t index, const char *key, const char *value) {
        auto &arg = m_args.at(index);
        arg.first = key;
        arg.second = value;
    }

    void ChannelArgs::setInteger(size_t index, const char *key, int value) {
        auto &arg = m_args.at(index);
        arg.first = key;
        arg.second = std::to_string(value);
    }
}
