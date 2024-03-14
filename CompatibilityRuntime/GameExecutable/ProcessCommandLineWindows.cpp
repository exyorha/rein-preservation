#include "ProcessCommandLineWindows.h"

#include "Il2CppUtilities.h"

#include <windows.h>

ProcessCommandLineWindows::ProcessCommandLineWindows(const std::filesystem::path &executablePath) : m_executablePath(executablePath.wstring()) {

    addArgument(std::wstring_view(m_executablePath));

}

ProcessCommandLineWindows::~ProcessCommandLineWindows() = default;

ProcessCommandLineWindows::ProcessCommandLineWindows(ProcessCommandLineWindows &&other) noexcept = default;

ProcessCommandLineWindows &ProcessCommandLineWindows::operator =(ProcessCommandLineWindows &&other) noexcept = default;

void ProcessCommandLineWindows::addArgument(const std::string_view &arg) {
    auto wide = utf8ToUtf16(arg);

    addArgument(std::wstring_view(reinterpret_cast<const wchar_t *>(wide.data()), wide.size()));
}

void ProcessCommandLineWindows::addArgument(const std::filesystem::path &path) {
   addArgument(std::wstring_view(path.wstring()));
}

void ProcessCommandLineWindows::addArgument(const std::wstring_view &arg) {
    if(!m_commandLine.empty())
        m_commandLine.push_back(L' ');

    m_commandLine.push_back(L'"');

    for(wchar_t ch: arg) {
        if(ch == L'\\' || ch == L'"')
            m_commandLine.push_back(L'\\');

        m_commandLine.push_back(ch);
    }

    m_commandLine.push_back(L'"');
}

uint32_t ProcessCommandLineWindows::startProcess() {
    printf("Starting a process: executable: '%ls', command line: '%ls'\n", m_executablePath.c_str(), m_commandLine.c_str());

    STARTUPINFO si;
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));

    if(!CreateProcess(m_executablePath.c_str(), m_commandLine.data(), nullptr, nullptr, TRUE,
        CREATE_BREAKAWAY_FROM_JOB | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP, nullptr, nullptr, &si, &pi))
        throw std::runtime_error("CreateProcess has failed");

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return pi.dwProcessId;
}
