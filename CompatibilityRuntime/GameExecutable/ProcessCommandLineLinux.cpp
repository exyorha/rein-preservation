#include "ProcessCommandLineLinux.h"

#include <spawn.h>
#include <unistd.h>

#include <system_error>
#include <cerrno>

ProcessCommandLineLinux::ProcessCommandLineLinux(const std::filesystem::path &executablePath) {
    m_args.emplace_back(executablePath.string());
}

ProcessCommandLineLinux::~ProcessCommandLineLinux() = default;

ProcessCommandLineLinux::ProcessCommandLineLinux(ProcessCommandLineLinux &&other) noexcept = default;

ProcessCommandLineLinux &ProcessCommandLineLinux::operator =(ProcessCommandLineLinux &&other) noexcept = default;

void ProcessCommandLineLinux::addArgument(const std::string_view &arg) {
    m_args.emplace_back(arg);
}

void ProcessCommandLineLinux::addArgument(const std::filesystem::path &path) {
    m_args.emplace_back(path.string());
}

int32_t ProcessCommandLineLinux::startProcess() {

    std::vector<char *> argPointers;
    argPointers.reserve(m_args.size() + 1);

    for(auto &arg: m_args) {
        argPointers.emplace_back(arg.data());
    }

    argPointers.push_back(nullptr);

    pid_t pid;
    auto result = posix_spawn(&pid, argPointers.front(), nullptr, nullptr, argPointers.data(), environ);
    if(result != 0) {
        throw std::system_error(result, std::generic_category());
    }

    return pid;
}
