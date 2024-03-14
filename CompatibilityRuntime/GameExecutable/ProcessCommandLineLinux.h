#ifndef PROCESS_COMMAND_LINE_LINUX_H
#define PROCESS_COMMAND_LINE_LINUX_H

#include <filesystem>
#include <vector>
#include <string>

class ProcessCommandLineLinux {
public:
    explicit ProcessCommandLineLinux(const std::filesystem::path &executablePath);
    ~ProcessCommandLineLinux();

    ProcessCommandLineLinux(const ProcessCommandLineLinux &other) = delete;
    ProcessCommandLineLinux &operator =(const ProcessCommandLineLinux &other) = delete;

    ProcessCommandLineLinux(ProcessCommandLineLinux &&other) noexcept;
    ProcessCommandLineLinux &operator =(ProcessCommandLineLinux &&other) noexcept;

    void addArgument(const std::string_view &arg);
    void addArgument(const std::filesystem::path &path);

    int32_t startProcess();

private:
    std::vector<std::string> m_args;
};

#endif

