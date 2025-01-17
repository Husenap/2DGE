#include "Logger.h"

// Simply used to customize console colours a bit
#ifdef _WIN32
#include <cwchar>

#include <Windows.h>
#include <wchar.h>

// library has the 4242 warning which would trigger Warnings As Errors
#pragma warning(disable : 4242)
#include <color.hpp>
#pragma warning(default : 4242)
#endif

namespace fs = ::std::filesystem;

namespace Wraith
{
    Logger::Logger(bool multiThreaded)
        : m_Queue()
        , m_Level((char)Level::All)
        , m_ShouldLogToFile(true)
        , m_MultiThreaded(multiThreaded)
    {
        if (m_MultiThreaded)
            m_Thread = std::jthread(Update, this, std::chrono::milliseconds(16));

        VerifyLogPath();

#ifdef _WIN32
        // Customize Console a little bit
        HWND hwnd = GetConsoleWindow();
        SetConsoleTitleW(L"Engine Console");
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hwnd, 0, 245, LWA_ALPHA);

        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        cfi.nFont = 0;
        cfi.dwFontSize.X = 0;   // Width of each character in the font
        cfi.dwFontSize.Y = 18;  // Height
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;
        wcscpy_s(cfi.FaceName, _countof(cfi.FaceName), L"Consolas");
        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
#endif
    }

    void Logger::LogInternal(Level level, const char* file, u32 line, const char* function, std::string text)
    {
        if (!Get())
            return;

        if ((char)level < (char)m_Level)
            return;

        if (level == Level::All || level == Level::Count)
            return;

        std::string filename = fs::path(file).filename().string();
        std::string func(function);
        if (func.find("lambda") != std::string::npos)
            func = func.substr(0, func.find("lambda") + 6) + ">";

        if (u64 pos = func.find_last_of(":"); pos != std::string::npos)
        {
            func = func.substr(pos + 1);
        }

        if (m_MultiThreaded)
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_Queue.push(LogEntry{ filename, func, text, line, level });
        }
        else
        {
            Print(Get(), LogEntry{ filename, func, text, line, level });
        }

        // Force log thread to finish
        if (level == Level::Fatal && m_MultiThreaded)
            m_Thread.request_stop();
    }

#ifdef _WIN32
    auto Logger::GetLevelString(Level level)
    {
        switch (level)
        {
        case Logger::Level::Verbose:
            return dye::white("VERBOSE");
            break;
        case Logger::Level::Info:
            return dye::light_green("INFO");
            break;
        case Logger::Level::Warning:
            return dye::light_yellow("WARNING");
            break;
        case Logger::Level::Error:
            return dye::light_red("ERROR");
            break;
        case Logger::Level::Fatal:
            return dye::light_purple("FATAL");
            break;
        }
        return dye::black("");
    }
#else
    std::string Logger::GetLevelString(Level level)
    {
        switch (level)
        {
        case Logger::Level::Verbose:
            return "VERBOSE";
            break;
        case Logger::Level::Info:
            return "INFO";
            break;
        case Logger::Level::Warning:
            return "WARNING";
            break;
        case Logger::Level::Error:
            return "ERROR";
            break;
        case Logger::Level::Fatal:
            return "FATAL";
            break;
        }
        return "";
    }
#endif

    void Logger::VerifyLogPath()
    {
        // Make sure the saved directory exists so that we can create a log file
        if (!fs::is_directory("saved") || !fs::exists("saved"))
            fs::create_directory("saved");

        // Remove the file if it already exists a log for this date
        m_LogPath = std::format("saved/log_{:%F}.txt", std::chrono::utc_clock::now());
        if (fs::exists(m_LogPath))
            fs::remove(m_LogPath);

        // Create the file
        std::ofstream ofs(m_LogPath.c_str(), std::ios_base::out);
    }

    void Logger::LogToFile(const std::string& msg)
    {
        std::ofstream ofs(m_LogPath.c_str(), std::ios_base::out | std::ios_base::app);
        ofs << std::format("[{:%F %H:%M:%OS}]\t", std::chrono::utc_clock::now()) << msg;
        ofs.close();
    }

    void Logger::Update(Logger* instance, std::chrono::duration<double, std::milli> interval)
    {
        while (!instance->m_Thread.get_stop_token().stop_requested() || !instance->m_Queue.empty())
        {
            auto t1 = std::chrono::steady_clock::now();
            {
                std::lock_guard<std::mutex> lock(instance->m_QueueMutex);
                while (!instance->m_Queue.empty())
                {
                    auto& entry = instance->m_Queue.front();

                    Print(instance, entry);
                    instance->m_Queue.pop();
                }
            }
            auto t2 = std::chrono::steady_clock::now();
            std::chrono::duration<double, std::milli> time_took = t2 - t1;

            if (time_took < interval)
                std::this_thread::sleep_for(interval - time_took);
        }
    }

    void Logger::Print(Logger* instance, const LogEntry& entry)
    {
#ifdef _WIN32
        auto dyed = dye::grey("[") + GetLevelString(entry.level) + dye::grey("]") + " " +
                    dye::grey(entry.filename + ":" + std::to_string(entry.line) + ":" + entry.function + ": ") +
                    dye::bright_white(entry.msg) + "\n";
        std::cout << dyed;

        std::stringstream sstr;
        sstr << dyed;

        if (instance->m_ShouldLogToFile)
            instance->LogToFile(sstr.str());
#else
        auto log = "[" + GetLevelString(entry.level) + "]" + " " + entry.filename + ":" + std::to_string(entry.line) +
                   ":" + entry.function + ": " + entry.msg;
        puts(log.c_str());

        if (instance->m_ShouldLogToFile)
            instance->LogToFile(log);
#endif
    }

    void Logger::SetLevel(Level level) { Get()->m_Level = (char)level; }

    void Logger::SetShouldLogToFile(bool shouldLogToFile) { Get()->m_ShouldLogToFile = shouldLogToFile; }
}  // namespace Wraith