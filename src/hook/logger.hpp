void InitLogger()
{
  auto logger = logger::basic_logger_mt("log", "./dfint_data/dfint_log.log");
  logger::set_level(static_cast<logger::level::level_enum>(Config::Setting::log_level));
  logger->flush_on(logger::level::trace);
  logger::set_default_logger(logger);
  logger::set_pattern("[%H:%M:%S.%e] [%l] %v");
  logger::info("logger init");
}