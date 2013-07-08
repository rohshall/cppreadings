# cppreadings

Device readings web application using C++ and Boost.

## Requirements

Boost >= 1.53.

## Build

Use following commands to configure & build the application:
```
c++ -o cppreadings ../server.cpp -lfcgi -lfcgi++ -lpq -std=c++11
```
Run the fastcgi handler:
```
spawn-fcgi -p 8000 -n build/cppreadings
```

Modify the nginx config:
```
        # pass the C++ scripts to FastCGI server listening on 127.0.0.1:8000
        #
        location ~ \.cpp$ {
            fastcgi_pass   127.0.0.1:8000;
            fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;
            include        fastcgi_params;
            fastcgi_intercept_errors on;
        }
```

Reload nginx config:
```
nginx -s reload
```
