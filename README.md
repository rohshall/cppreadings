# cppreadings

Device readings web application using C++ and Boost.

## Requirements

Boost >= 1.53.

## Build

Use following commands to configure & build the application:
```
c++ -o cppreadings ../server.cpp -lfcgi -lfcgi++ -lpq -std=c++11
spawn-fcgi -p 8000 -n ./cppreadings
```
Now, modify the server configuration to forward the requests to port 8000 via fastcgi interface.
