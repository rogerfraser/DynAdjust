//============================================================================
// Name         : dnaprocessfuncs.cpp
// Author       : Roger Fraser
// Contributors :
// Version      : 1.00
// Copyright    : Copyright 2017 Geoscience Australia
//
//                Licensed under the Apache License, Version 2.0 (the
//                "License"); you may not use this file except in compliance
//                with the License. You may obtain a copy of the License at
//
//                http ://www.apache.org/licenses/LICENSE-2.0
//
//                Unless required by applicable law or agreed to in writing,
//                software distributed under the License is distributed on an
//                "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
//                either express or implied. See the License for the specific
//                language governing permissions and limitations under the
//                License.
//
// Description  : Common process and fork functions
//============================================================================
#include <include/functions/dnaprocessfuncs.hpp>
#include <iostream>
#include <string>
#include <cstdlib>

// Platform detection
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    #include <sys/wait.h>
    #include <unistd.h>
    #include <fcntl.h>
#else
    #error "Unsupported platform"
#endif

bool run_command(const std::string& executable_path, bool quiet) {
    
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    STARTUPINFO startup{};
    PROCESS_INFORMATION process{};
    startup.cb = sizeof(STARTUPINFO);
    
    HANDLE devnull = INVALID_HANDLE_VALUE;
    if (quiet) {
        devnull = CreateFile("NUL", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (devnull != INVALID_HANDLE_VALUE) {
            startup.hStdOutput = devnull;
            startup.hStdError = devnull;
            startup.dwFlags |= STARTF_USESTDHANDLES;
        }
    }
    
    auto cmd_copy = executable_path;
    BOOL success = CreateProcess(nullptr, cmd_copy.data(), nullptr, nullptr, 
                               TRUE, 0, nullptr, nullptr, &startup, &process);
    
    DWORD return_code = EXIT_FAILURE;
    if (success) {
        WaitForSingleObject(process.hProcess, INFINITE);
        GetExitCodeProcess(process.hProcess, &return_code);
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
    } else {
        std::cout << "\n- Error: Cannot execute " << executable_path 
                  << "\n  Error code: " << GetLastError() << '\n';
    }
    
    if (devnull != INVALID_HANDLE_VALUE) {
        CloseHandle(devnull);
    }
    
    return (return_code == EXIT_SUCCESS);

#else
    pid_t pid = fork();
    
    if (pid == -1) {
        std::cout << "\n- Error: Failed to fork process for " << executable_path << '\n';
        return false;
    }
    
    if (pid == 0) {
        // Child process
        if (quiet) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull != -1) {
                dup2(devnull, STDOUT_FILENO);
                dup2(devnull, STDERR_FILENO);
                close(devnull);
            }
        }
        
        execl("/bin/sh", "sh", "-c", executable_path.c_str(), nullptr);
        
        // If we get here, execl failed
        std::cout << "\n- Error: Cannot execute " << executable_path << '\n';
        _exit(EXIT_FAILURE);
    }
    
    // Parent process - wait for child
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        return (WEXITSTATUS(status) == EXIT_SUCCESS);
    }
    
    std::cout << "\n- Error: Process " << executable_path << " terminated abnormally\n";
    return false;
#endif
}
