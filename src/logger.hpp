void InitLogger()
{
  auto logger = spdlog::basic_logger_mt("log", "hook-log.log");
  spdlog::set_level(spdlog::level::trace);
  logger->flush_on(spdlog::level::trace);
  spdlog::set_default_logger(logger);
  spdlog::info("logger init");
}