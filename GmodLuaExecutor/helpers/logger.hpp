#pragma once

#pragma warning(disable : 4996)

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <filesystem>
#include <stdarg.h>

class AsyncLogger
{
private:
    std::ofstream file;
    std::thread logThread;
    std::atomic<bool> running{ true };

    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable cv;

    std::string lastMessage;
    int sameMessageCount = 0;

    void logWorker() {
        using clock = std::chrono::steady_clock;
        auto lastFlush = clock::now();
        std::string lastMessage;
        int repeatCount = 0;
        const auto flushInterval = std::chrono::milliseconds(200);

        while (running.load() || !logQueue.empty()) {
            std::string message;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                if (cv.wait_for(lock, flushInterval, [this] { return !logQueue.empty() || !running.load(); })) {
                    // got a message
                    message = logQueue.front();
                    logQueue.pop();
                }
            }

            if (!message.empty()) {
                if (repeatCount == 0) {
                    lastMessage = message;
                    repeatCount = 1;
                }
                else if (message == lastMessage) {
                    repeatCount++;
                }
                else {
                    logToFile(lastMessage, repeatCount);
                    lastMessage = message;
                    repeatCount = 1;
                }
                lastFlush = clock::now();
            }

            // time-based flush
            if (repeatCount > 0 && (clock::now() - lastFlush) > flushInterval) {
                logToFile(lastMessage, repeatCount);
                repeatCount = 0;
                lastMessage.clear();
                lastFlush = clock::now();
            }
        }
        // Final flush
        if (repeatCount > 0) {
            logToFile(lastMessage, repeatCount);
        }
    }

    void logToFile(const std::string& msg, int count)
    {
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        std::ostringstream timeStamp;
        timeStamp << std::put_time(localTime, "[%Y-%m-%d %H:%M:%S] ");
        if (count > 1)
            timeStamp << msg << " [" << count << "x]";
        else
            timeStamp << msg;
        file << timeStamp.str() << std::endl;
    }

public:
    AsyncLogger(const std::string& filename) : lastMessage("") {
        std::filesystem::remove(filename);
        file.open(filename, std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filename);
        }
        logThread = std::thread(&AsyncLogger::logWorker, this);
    }

    ~AsyncLogger()
    {
        running = false;
        cv.notify_all();
        if (logThread.joinable())
            logThread.join();
        file.close();
    }

    void println(const std::string& message)
    {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            logQueue.push(message);
        }
        cv.notify_one();
    }

    void println(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        println(std::string(buffer));
    }
};