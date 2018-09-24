include (CheckFunctionExists)
check_function_exists(epoll_create HAVE_EPOLL)
check_function_exists(select HAVE_SELECT)
