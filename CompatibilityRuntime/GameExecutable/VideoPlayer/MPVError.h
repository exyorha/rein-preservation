#ifndef VIDEO_PLAYER_MPV_ERROR_H
#define VIDEO_PLAYER_MPV_ERROR_H

#include <exception>

class MPVError final : public std::exception {
public:
    explicit MPVError(int error);
    ~MPVError() override;

    MPVError(const MPVError &other) noexcept;
    MPVError &operator =(const MPVError &other) noexcept;

    const char* what() const noexcept override;

private:
    int m_error;
};

#endif
