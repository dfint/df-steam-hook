void InitLogger()
{
  auto logger = spdlog::basic_logger_mt("log", "hook-log.log");
  spdlog::set_level(spdlog::level::trace);
  logger->flush_on(spdlog::level::trace);
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("[%H:%M:%S.%e] [%l] %v");
  spdlog::info("logger init");
}