import logging


class LogMixin:
    __logger = None
    __log_name = None

    def logger_set_name(self, name: str):
        self.__log_name = name
        self.__logger = None

    @property
    def log(self) -> logging.Logger:
        if self.__logger is None:
            self.__logger = logging.getLogger(self.__log_name)
        return self.__logger
