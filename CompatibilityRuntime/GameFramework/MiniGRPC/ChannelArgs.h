#ifndef CHANNEL_ARGS_H
#define CHANNEL_ARGS_H

#include <cstring>
#include <string>
#include <utility>
#include <vector>

namespace minigrpc {
    using ChannelArg = std::pair<std::string, std::string>;

    class ChannelArgs {
    public:
        explicit ChannelArgs(size_t capacity);
        ~ChannelArgs();

        ChannelArgs(const ChannelArgs &other) = delete;
        ChannelArgs &operator =(const ChannelArgs &other) = delete;

        void setString(size_t index, const char *key, const char *value);
        void setInteger(size_t index, const char *key, int value);

        inline const std::vector<ChannelArg> &args() const {
            return m_args;
        }

    private:
        std::vector<ChannelArg> m_args;
    };
}

#endif
