#pragma once
#include "pch.h"


namespace topo
{
	std::string app_current_time_and_date() noexcept
	{
		try
		{
			auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
			return std::format("{:%X}", time);
		}
		catch (const std::runtime_error& e)
		{
			return std::format("Caught runtime error: {}", e.what());
		}
	}
	
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

// CORE Log macros
#define TOPO_CORE_TRACE(...) ::topo::LogCoreTrace(__VA_ARGS__)
#define TOPO_CORE_INFO(...) ::topo::LogCoreInfo(__VA_ARGS__)
#define TOPO_CORE_WARN(...) ::topo::LogCoreWarn(__VA_ARGS__)
#define TOPO_CORE_ERROR(...) ::topo::LogCoreError(__VA_ARGS__)

// CLIENT Log macros
#define TOPO_TRACE(...) ::topo::LogTrace(__VA_ARGS__)
#define TOPO_INFO(...) ::topo::LogInfo(__VA_ARGS__)
#define TOPO_WARN(...) ::topo::LogWarn(__VA_ARGS__)
#define TOPO_ERROR(...) ::topo::LogError(__VA_ARGS__)