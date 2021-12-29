cd arcirk/
cmake .
make clean
make
cd -
#mkdir -p ws_server/bin/data
#mkdir -p ws_server/bin/config
#mkdir -p ws_server/bin/html
cd ws_server/
make clean
cmake .
make