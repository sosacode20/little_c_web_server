# little_c_web_server

Basic C Webserver for Operative System subject.

-Serve local files
-Allow specify server port and filesystem root
-Serve directories in HTML page where is possible view files detail info and sort them
-Allow multiple clients
-Allow multiple downloads for the same client

# Build

```[bash]
git clone https://github.com/leismael/little_c_web_server.git
cd little_c_web_server
cmake .
make
```

# Usage

`./webserver <port> <server-root>`

# Important
  
  In order to serve directories the template.html file should be in the same directory of the executable
