_start_test:
	cd tests/ && \
	mkdir -p build && \
	cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Coverage && \
	make && \
	./run_tests

_start_test_coverage:
	cd tests/build && \
	make coverage
