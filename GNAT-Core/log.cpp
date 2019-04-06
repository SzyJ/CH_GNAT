#include "pch.h"
#include "log.h"
#include <spdlog/sinks/rotating_file_sink.h>
#include <ctime>

namespace GNAT {
	std::shared_ptr<spdlog::logger> GNAT_Log::connection_logger;
	std::shared_ptr<spdlog::logger> GNAT_Log::server_logger;
	std::shared_ptr<spdlog::logger> GNAT_Log::peer_logger;
	std::shared_ptr<spdlog::logger> GNAT_Log::client_logger;

	void GNAT_Log::init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
	
		try
		{
			peer_logger = spdlog::rotating_logger_mt("CONN", "Logs\\CONN-" + std::to_string(std::time(0)) + ".log", 1024 * 1024 * LOG_FILE_SIZE_IN_MB, ROTATING_FILE_COUNT);
			server_logger = spdlog::rotating_logger_mt("SERV", "Logs\\SERV-" + std::to_string(std::time(0)) + ".log", 1024 * 1024 * LOG_FILE_SIZE_IN_MB, ROTATING_FILE_COUNT);
			peer_logger = spdlog::rotating_logger_mt("PEER", "Logs\\PEER-" + std::to_string(std::time(0)) + ".log", 1024 * 1024 * LOG_FILE_SIZE_IN_MB, ROTATING_FILE_COUNT);
			client_logger = spdlog::rotating_logger_mt("CLNT", "Logs\\CLNT-" + std::to_string(std::time(0)) + ".log", 1024 * 1024 * LOG_FILE_SIZE_IN_MB, ROTATING_FILE_COUNT);
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cout << "Log initialization failed: " << ex.what() << std::endl;
		}
	}

	void GNAT_Log::init_client() {
		spdlog::set_pattern("%^[%T] %n: %v%$");

		try
		{
			client_logger = spdlog::rotating_logger_mt("CLNT", "Logs\\CLNT-" + std::to_string(std::time(0)) + ".log", 1024 * 1024 * LOG_FILE_SIZE_IN_MB, ROTATING_FILE_COUNT);
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cout << "Log initialization failed: " << ex.what() << std::endl;
		}
	}

	void GNAT_Log::init_server() {
		spdlog::set_pattern("%^[%T] %n: %v%$");

		try
		{
			server_logger = spdlog::rotating_logger_mt("SERV", "Logs\\SERV-" + std::to_string(std::time(0)) + ".log", 1024 * 1024 * LOG_FILE_SIZE_IN_MB, ROTATING_FILE_COUNT);
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cout << "Log initialization failed: " << ex.what() << std::endl;
		}
	}

	void GNAT_Log::init_peer() {
		spdlog::set_pattern("%^[%T] %n: %v%$");

		try
		{
			peer_logger = spdlog::rotating_logger_mt("PEER", "Logs\\PEER-" + std::to_string(std::time(0)) + ".log", 1024 * 1024 * LOG_FILE_SIZE_IN_MB, ROTATING_FILE_COUNT);
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cout << "Log initialization failed: " << ex.what() << std::endl;
		}
	}

	void GNAT_Log::init_connection() {
		spdlog::set_pattern("%^[%T] %n: %v%$");

		try
		{
			peer_logger = spdlog::rotating_logger_mt("CONN", "Logs\\CONN-" + std::to_string(std::time(0)) + ".log", 1024 * 1024 * LOG_FILE_SIZE_IN_MB, ROTATING_FILE_COUNT);
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cout << "Log initialization failed: " << ex.what() << std::endl;
		}
	}
}