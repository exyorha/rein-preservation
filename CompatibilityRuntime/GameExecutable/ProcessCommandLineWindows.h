#ifndef PROCESS_COMMAND_LINE_WINDOWS_H
#define PROCESS_COMMAND_LINE_WINDOWS_H

#include <filesystem>

class ProcessCommandLineWindows {
public:
    explicit ProcessCommandLineWindows(const std::filesystem::path &executablePath);
    ~ProcessCommandLineWindows();

    ProcessCommandLineWindows(const ProcessCommandLineWindows &other) = delete;
    ProcessCommandLineWindows &operator =(const ProcessCommandLineWindows &other) = delete;

    ProcessCommandLineWindows(ProcessCommandLineWindows &&other) noexcept;
    ProcessCommandLineWindows &operator =(ProcessCommandLineWindows &&other) noexcept;

    void addArgument(const std::string_view &arg);
    void addArgument(const std::filesystem::path &path);

    uint32_t startProcess();

private:
    void addArgument(const std::wstring_view &arg);

    std::wstring m_executablePath;
    std::wstring m_commandLine;
};

#endif
