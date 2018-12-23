#include "pch.h"
#include "log.h"
#include <spdlog/sinks/rotating_file_sink.h>
#include <ctime>

namespace GNAT {
	std::shared_ptr<spdlog::logger> GNAT_Log::s_logger;

	void GNAT_Log::init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
	
		try
		{
			s_logger = spdlog::rotating_logger_mt("CH-GNAT", "Logs\\CH-GNAT-" + std::to_string(std::time(0)) + ".log", 1024 * 1024 * LOG_FILE_SIZE_IN_MB, ROTATING_FILE_COUNT);
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cout << "Log initialization failed: " << ex.what() << std::endl;
		}
	}
}