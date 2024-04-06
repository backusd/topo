#pragma once
#include "Core.h"

namespace topo
{
	TOPO_API std::string app_current_time_and_date() noexcept;
	
	// CORE Logging ========================================================================================

	template <typename... T>
	void LogCoreTrace(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
		std::cout << "[TRACE " << app_current_time_and_date() << "] CORE - ";
		if constexpr (sizeof...(T) == 0)
			std::cout << msg;
		else
			std::cout << std::vformat(msg, std::make_format_args(std::forward<T>(args)...));
		std::cout << '\n';
	}

	template <typename... T>
	void LogCoreInfo(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
#endif
		std::cout << "[INFO " << app_current_time_and_date() << "] CORE - ";
		if constexpr (sizeof...(T) == 0)
			std::cout << msg;
		else
			std::cout << std::vformat(msg, std::make_format_args(std::forward<T>(args)...));
		std::cout << '\n';
	}

	template <typename... T>
	void LogCoreWarn(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
#endif
		std::cout << "[WARNING " << app_current_time_and_date() << "] CORE - ";
		if constexpr (sizeof...(T) == 0)
			std::cout << msg;
		else
			std::cout << std::vformat(msg, std::make_format_args(std::forward<T>(args)...));
		std::cout << '\n';
	}

	template <typename... T>
	void LogCoreError(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
#endif
		std::cout << "[ERROR " << app_current_time_and_date() << "] CORE - ";
		if constexpr (sizeof...(T) == 0)
			std::cout << msg;
		else
			std::cout << std::vformat(msg, std::make_format_args(std::forward<T>(args)...));
		std::cout << '\n';
	}

	// CLIENT Logging ========================================================================================

	template <typename... T>
	void LogTrace(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
		std::cout << "[TRACE " << app_current_time_and_date() << "] ";
		if constexpr (sizeof...(T) == 0)
			std::cout << msg;
		else
			std::cout << std::vformat(msg, std::make_format_args(std::forward<T>(args)...));
		std::cout << '\n';
	}

	template <typename... T>
	void LogInfo(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
#endif
		std::cout << "[INFO " << app_current_time_and_date() << "] ";
		if constexpr (sizeof...(T) == 0)
			std::cout << msg;
		else
			std::cout << std::vformat(msg, std::make_format_args(std::forward<T>(args)...));
		std::cout << '\n';
	}

	template <typename... T>
	void LogWarn(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
#endif
		std::cout << "[WARNING " << app_current_time_and_date() << "] ";
		if constexpr (sizeof...(T) == 0)
			std::cout << msg;
		else
			std::cout << std::vformat(msg, std::make_format_args(std::forward<T>(args)...));
		std::cout << '\n';
	}

	template <typename... T>
	void LogError(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
#endif
		std::cout << "[ERROR " << app_current_time_and_date() << "] ";
		if constexpr (sizeof...(T) == 0)
			std::cout << msg;
		else
			std::cout << std::vformat(msg, std::make_format_args(std::forward<T>(args)...));
		std::cout << '\n';
	}
}

// Disable logging for distribution builds
#ifdef TOPO_DIST 

#define LOG_TRACE(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)

#elif TOPO_BUILD_DLL // If building the DLL, use CORE logging

#define LOG_TRACE(...) ::topo::LogCoreTrace(__VA_ARGS__)
#define LOG_INFO(...) ::topo::LogCoreInfo(__VA_ARGS__)
#define LOG_WARN(...) ::topo::LogCoreWarn(__VA_ARGS__)
#define LOG_ERROR(...) ::topo::LogCoreError(__VA_ARGS__)

#else // If building the client application, use basic logging

#define LOG_TRACE(...) ::topo::LogTrace(__VA_ARGS__)
#define LOG_INFO(...) ::topo::LogInfo(__VA_ARGS__)
#define LOG_WARN(...) ::topo::LogWarn(__VA_ARGS__)
#define LOG_ERROR(...) ::topo::LogError(__VA_ARGS__)

#endif
