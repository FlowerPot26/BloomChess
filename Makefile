run:
	g++ -o bloom bloom_main.cpp bloom_util.cpp bloom_board.cpp bloom_engine.cpp bloom_tests.cpp bloom_zobrist.cpp
	./bloom
	rm bloom



