#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

int main() {
  std::cout << "Hello, World!" << std::endl;
  auto m_logger = spdlog::stdout_logger_mt("app.log");
  m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l]  %t %v");
  m_logger->info("hello world");
  return 0;
}