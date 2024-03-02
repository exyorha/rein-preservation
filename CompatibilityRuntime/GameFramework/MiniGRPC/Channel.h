#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>

namespace minigrpc {

    class ChannelArgs;

    class Channel {
    public:
        Channel(const char *target, const ChannelArgs *args);
        ~Channel();

        Channel(const Channel &other) = delete;
        Channel &operator =(const Channel &other) = delete;

        inline const std::string &target() const {
            return m_target;
        }

    private:
        std::string m_target;
    };

}

#endif
