#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace GNAT {
	static class GNAT_Log {
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& getLogger() { return s_logger; }

	private:
		const static int LOG_FILE_SIZE_IN_MB = 5;
		const static int ROTATING_FILE_COUNT = 3;


		static std::shared_ptr<spdlog::logger> s_logger;
	};
}

// Loging Macros
#define LOG_FATAL(...) GNAT::GNAT_Log::getLogger()->fatal(__VA_ARGS__)
#define LOG_ERROR(...) GNAT::GNAT_Log::getLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) GNAT::GNAT_Log::getLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) GNAT::GNAT_Log::getLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) GNAT::GNAT_Log::getLogger()->trace(__VA_ARGS__)