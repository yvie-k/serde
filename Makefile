CXX = g++-16

CXXFLAGS = -std=c++26 -fmodules -freflection -Wall -Wextra

.PHONY: all run

all: build-gcc

run: build-gcc
	./build-gcc

gcm.cache/std.gcm: /usr/include/x86_64-linux-gnu/c++/16/bits/stdc++.h
	$(CXX) $(CXXFLAGS) -fmodules -fsearch-include-path -c bits/std.cc -o /dev/null

gcm.cache/nlohmann.json.gcm: json.hpp json.cppm
	$(CXX) $(CXXFLAGS) -fmodules -c json.cppm -o /dev/null

gcm.cache/serde-serialize.gcm: serde/serialize.cpp gcm.cache/std.gcm
	$(CXX) $(CXXFLAGS) -c serde/serialize.cpp -o /dev/null

gcm.cache/serde-deserialize.gcm: serde/deserialize.cpp gcm.cache/std.gcm
	$(CXX) $(CXXFLAGS) -c serde/deserialize.cpp -o /dev/null

gcm.cache/serde-convert.base.gcm: serde/convert/base.cpp gcm.cache/serde-serialize.gcm gcm.cache/serde-deserialize.gcm
	$(CXX) $(CXXFLAGS) -c serde/convert/base.cpp -o /dev/null

gcm.cache/serde-convert.numeric.gcm: serde/convert/numeric.cpp gcm.cache/serde-convert.base.gcm
	$(CXX) $(CXXFLAGS) -c serde/convert/numeric.cpp -o /dev/null

gcm.cache/serde-convert.enumerations.gcm: serde/convert/enum.cpp gcm.cache/serde-convert.base.gcm
	$(CXX) $(CXXFLAGS) -c serde/convert/enum.cpp -o /dev/null

gcm.cache/serde-convert.classes.gcm: serde/convert/classes.cpp gcm.cache/serde-convert.base.gcm gcm.cache/serde-annotations.gcm
	$(CXX) $(CXXFLAGS) -c serde/convert/classes.cpp -o /dev/null

gcm.cache/serde-convert.gcm: serde/convert.cpp gcm.cache/serde-convert.base.gcm gcm.cache/serde-convert.numeric.gcm gcm.cache/serde-convert.enumerations.gcm gcm.cache/serde-convert.classes.gcm
	$(CXX) $(CXXFLAGS) -c serde/convert.cpp -o /dev/null

gcm.cache/serde-annotations.gcm: serde/annotations.cpp
	$(CXX) $(CXXFLAGS) -c serde/annotations.cpp -o /dev/null

gcm.cache/serde.gcm: serde.cpp gcm.cache/serde-serialize.gcm gcm.cache/serde-convert.gcm gcm.cache/serde-annotations.gcm
	$(CXX) $(CXXFLAGS) -c serde.cpp -o /dev/null

build-gcc: test.cpp gcm.cache/serde.gcm gcm.cache/nlohmann.json.gcm
	$(CXX) $(CXXFLAGS) test.cpp -o build-gcc
