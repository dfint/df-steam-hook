void InitLogger()
{
  auto logger = spdlog::basic_logger_mt("log", "dfint_log.log");
  spdlog::set_level(static_cast<spdlog::level::level_enum>(Config::Setting::log_level));
  logger->flush_on(spdlog::level::trace);
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("[%H:%M:%S.%e] [%l] %v");
  spdlog::info("logger init");
}