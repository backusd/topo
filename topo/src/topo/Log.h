#pragma once
#include "Core.h"

namespace topo
{
	std::string app_current_time_and_date() noexcept;
	
	// CORE Logging ========================================================================================

	template <typename... T>
	void LogCoreTrace(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
		if constexpr (sizeof...(T) == 0)
			std::println("[TRACE {0}] CORE - {1}", app_current_time_and_date(), msg);
		else
		{
			std::print("[TRACE {0}] CORE - ", app_current_time_and_date());
			std::vprint_nonunicode(std::cout, msg, std::make_format_args(std::forward<T>(args)...));
			std::println("");
		}
	}

	template <typename... T>
	void LogCoreInfo(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
#endif
		if constexpr (sizeof...(T) == 0)
			std::println("[INFO {0}] CORE - {1}", app_current_time_and_date(), msg);
		else
		{
			std::print("[INFO {0}] CORE - ", app_current_time_and_date());
			std::vprint_nonunicode(std::cout, msg, std::make_format_args(std::forward<T>(args)...));
			std::println("");
		}
	}

	template <typename... T>
	void LogCoreWarn(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
#endif
		if constexpr (sizeof...(T) == 0)
			std::println("[WARNING {0}] CORE - {1}", app_current_time_and_date(), msg);
		else
		{
			std::print("[WARNING {0}] CORE - ", app_current_time_and_date());
			std::vprint_nonunicode(std::cout, msg, std::make_format_args(std::forward<T>(args)...));
			std::println("");
		}
	}

	template <typename... T>
	void LogCoreError(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
#endif
		if constexpr (sizeof...(T) == 0)
			std::println("[ERROR {0}] CORE - {1}", app_current_time_and_date(), msg);
		else
		{
			std::print("[ERROR {0}] CORE - ", app_current_time_and_date());
			std::vprint_nonunicode(std::cout, msg, std::make_format_args(std::forward<T>(args)...));
			std::println("");
		}
	}

	// CLIENT Logging ========================================================================================

	template <typename... T>
	void LogTrace(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
		if constexpr (sizeof...(T) == 0)
			std::println("[TRACE {0}] {1}", app_current_time_and_date(), msg);
		else
		{
			std::print("[TRACE {0}] ", app_current_time_and_date());
			std::vprint_nonunicode(std::cout, msg, std::make_format_args(std::forward<T>(args)...));
			std::println("");
		}
	}

	template <typename... T>
	void LogInfo(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
#endif
		if constexpr (sizeof...(T) == 0)
			std::println("[INFO {0}] {1}", app_current_time_and_date(), msg);
		else
		{
			std::print("[INFO {0}] ", app_current_time_and_date());
			std::vprint_nonunicode(std::cout, msg, std::make_format_args(std::forward<T>(args)...));
			std::println("");
		}
	}

	template <typename... T>
	void LogWarn(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
#endif
		if constexpr (sizeof...(T) == 0)
			std::println("[WARNING {0}] {1}", app_current_time_and_date(), msg);
		else
		{
			std::print("[WARNING {0}] ", app_current_time_and_date());
			std::vprint_nonunicode(std::cout, msg, std::make_format_args(std::forward<T>(args)...));
			std::println("");
		}
	}

	template <typename... T>
	void LogError(std::string_view msg, T... args) noexcept
	{
#ifdef TOPO_PLATFORM_WINDOWS
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
#endif
		if constexpr (sizeof...(T) == 0)
			std::println("[WARNING {0}] {1}", app_current_time_and_date(), msg);
		else
		{
			std::print("[WARNING {0}] ", app_current_time_and_date());
			std::vprint_nonunicode(std::cout, msg, std::make_format_args(std::forward<T>(args)...));
			std::println("");
	}
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
