VALGRIND_OPTS=--leak-check=full --show-reachable=yes --track-fds=yes -v --track-origins=yes --errors-for-leak-kinds=all --show-leak-kinds=all

_start_valgrind:
	@mkdir -p report
	valgrind $(VALGRIND_OPTS) ./tests/build/run_tests > report/valgrind.log 2>&1

_start_valgrind_tail: _start_valgrind
	tail -n 7 report/valgrind.log

_start_valgrind_ci:
	if grep -q "ERROR SUMMARY: 0 errors from 0 contexts" ./report/valgrind.log; then \
		exit 0; \
	else \
		exit 1; \
	fi
