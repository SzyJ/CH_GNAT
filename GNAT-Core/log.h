#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace GNAT {
	class GNAT_Log {
	public:
		static void init();
		static void init_client();
		static void init_server();
		static void init_peer();
		static void init_connection();

		inline static std::shared_ptr<spdlog::logger>& getConnectionLogger() { return connection_logger; }
		inline static std::shared_ptr<spdlog::logger>& getServerLogger() { return server_logger; }
		inline static std::shared_ptr<spdlog::logger>& getPeerLogger() { return peer_logger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return client_logger; }

	private:
		const static int LOG_FILE_SIZE_IN_MB = 5;
		const static int ROTATING_FILE_COUNT = 3;

		static std::shared_ptr<spdlog::logger> connection_logger;
		static std::shared_ptr<spdlog::logger> server_logger;
		static std::shared_ptr<spdlog::logger> peer_logger;
		static std::shared_ptr<spdlog::logger> client_logger;
	};
}

// Loging Macros
#define CONNECT_LOG_FATAL(...) GNAT::GNAT_Log::getConnectionLogger()->fatal(__VA_ARGS__); std::cout << "  " << __VA_ARGS__ << std::endl
#define CONNECT_LOG_ERROR(...) GNAT::GNAT_Log::getConnectionLogger()->error(__VA_ARGS__); std::cout << "  " << __VA_ARGS__ << std::endl
#define CONNECT_LOG_WARN(...) GNAT::GNAT_Log::getConnectionLogger()->warn(__VA_ARGS__);	 std::cout << "  " << __VA_ARGS__ << std::endl
#define CONNECT_LOG_INFO(...) GNAT::GNAT_Log::getConnectionLogger()->info(__VA_ARGS__);	 std::cout << "  " << __VA_ARGS__ << std::endl
#define CONNECT_LOG_TRACE(...) GNAT::GNAT_Log::getConnectionLogger()->trace(__VA_ARGS__); std::cout << "  " << __VA_ARGS__ << std::endl

#define SERVER_LOG_FATAL(...) GNAT::GNAT_Log::getServerLogger()->fatal(__VA_ARGS__); std::cout << "  " << __VA_ARGS__ << std::endl
#define SERVER_LOG_ERROR(...) GNAT::GNAT_Log::getServerLogger()->error(__VA_ARGS__); std::cout << "  " << __VA_ARGS__ << std::endl
#define SERVER_LOG_WARN(...) GNAT::GNAT_Log::getServerLogger()->warn(__VA_ARGS__);	 std::cout << "  " << __VA_ARGS__ << std::endl
#define SERVER_LOG_INFO(...) GNAT::GNAT_Log::getServerLogger()->info(__VA_ARGS__);	 std::cout << "  " << __VA_ARGS__ << std::endl
#define SERVER_LOG_TRACE(...) GNAT::GNAT_Log::getServerLogger()->trace(__VA_ARGS__); std::cout << "  " << __VA_ARGS__ << std::endl

#define PEER_LOG_FATAL(...) GNAT::GNAT_Log::getPeerLogger()->fatal(__VA_ARGS__);	 std::cout << "  " << __VA_ARGS__ << std::endl
#define PEER_LOG_ERROR(...) GNAT::GNAT_Log::getPeerLogger()->error(__VA_ARGS__);	 std::cout << "  " << __VA_ARGS__ << std::endl
#define PEER_LOG_WARN(...) GNAT::GNAT_Log::getPeerLogger()->warn(__VA_ARGS__);		 std::cout << "  " << __VA_ARGS__ << std::endl
#define PEER_LOG_INFO(...) GNAT::GNAT_Log::getPeerLogger()->info(__VA_ARGS__);		 std::cout << "  " << __VA_ARGS__ << std::endl
#define PEER_LOG_TRACE(...) GNAT::GNAT_Log::getPeerLogger()->trace(__VA_ARGS__);	 std::cout << "  " << __VA_ARGS__ << std::endl

#define CLIENT_LOG_FATAL(...) GNAT::GNAT_Log::getClientLogger()->fatal(__VA_ARGS__); std::cout << "  " << __VA_ARGS__ << std::endl
#define CLIENT_LOG_ERROR(...) GNAT::GNAT_Log::getClientLogger()->error(__VA_ARGS__); std::cout << "  " << __VA_ARGS__ << std::endl
#define CLIENT_LOG_WARN(...) GNAT::GNAT_Log::getClientLogger()->warn(__VA_ARGS__);	 std::cout << "  " << __VA_ARGS__ << std::endl
#define CLIENT_LOG_INFO(...) GNAT::GNAT_Log::getClientLogger()->info(__VA_ARGS__);	 std::cout << "  " << __VA_ARGS__ << std::endl
#define CLIENT_LOG_TRACE(...) GNAT::GNAT_Log::getClientLogger()->trace(__VA_ARGS__); std::cout << "  " << __VA_ARGS__ << std::endl