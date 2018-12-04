docker run --rm -it -v $(pwd)/../..:/sources_docker -p 5900:5900  gcc8_2_builder /bin/zsh

# CXXFLAGS="-std=c++17" ./TLDR_hunter.py test-build corrade
