C++ HTTP Server
A lightweight, feature-rich HTTP server built in C++ using the cpp-httplib library. This project demonstrates REST API development, request logging, statistics tracking, and dynamic routing capabilities.
 Features

REST API Endpoints - Multiple JSON API routes for different functionalities
Request Logging - Automatic logging of all incoming requests with timestamps
Statistics Tracking - Real-time request counting and per-endpoint analytics
Dynamic Routing - Path parameters for personalized responses
POST Request Handling - Echo endpoint for testing request bodies
Uptime Monitoring - Track server runtime since startup
Custom Error Handling - Styled 404 pages
Middleware Support - Pre-routing handlers for request interception
Cross-Platform - Works on Windows with Winsock support

Prerequisites

C++ Compiler (GCC, Clang, or MSVC with C++11 support)
Windows: MinGW-w64 or Visual Studio
Winsock2 library (included in Windows)

 Installation
bash# Clone the repository
git clone https://github.com/yourusername/cpp-http-server.git
cd cpp-http-server

# Compile (Windows with MinGW)
g++ -std=c++11 main.cpp -o server.exe -lws2_32

# Compile (Linux/macOS - if adapted)
g++ -std=c++11 main.cpp -o server -lpthread
Usage
bash# Run the server
./server.exe  # Windows
./server      # Linux/macOS

# Server will start on http://localhost:8080
```

## 📡 API Endpoints

### **GET /** 
Homepage with interactive documentation
```
http://localhost:8080/
GET /api/status
Server status and uptime information
bashcurl http://localhost:8080/api/status
Response:
json{
    "status": "online",
    "uptime": "0h 5m 32s",
    "timestamp": "2025-12-28 15:30:45",
    "total_requests": 42
}
GET /api/time
Current server timestamp
bashcurl http://localhost:8080/api/time
Response:
json{
    "current_time": "2025-12-28 15:30:45"
}
GET /api/stats
Request statistics per endpoint
bashcurl http://localhost:8080/api/stats
Response:
json{
  "total_requests": 42,
  "endpoints": {
    "/": 15,
    "/api/status": 10,
    "/api/time": 8,
    "/greet/john": 5
  }
}
POST /api/echo
Echo back the request body with metadata
bashcurl -X POST http://localhost:8080/api/echo -d "Hello Server"
Response:
json{
    "message": "Hello Server",
    "length": 12,
    "timestamp": "2025-12-28 15:30:45"
}
```

### **GET /greet/:name**
Personalized greeting page
```
http://localhost:8080/greet/zayd
```
Returns a styled HTML page with a personalized greeting.

## tree
```
cpp-http-server/
├── httplib.h           # Single-header HTTP library (v0.20.0)
├── main.cpp            # Server implementation with routes
├── server.crt          # SSL certificate (optional)
├── server.key          # SSL private key (optional)
└── README.md           # This file
🛠️ Technical Details
Key Components
1. Request Middleware
cppserver.set_pre_routing_handler([](const httplib::Request& req, httplib::Response&) {
    cout << "[" << getCurrentTime() << "] " << req.method << " " << req.path << endl;
    requestCount++;
    endpointStats[req.path]++;
    return httplib::Server::HandlerResponse::Unhandled;
});
Logs every request and updates statistics before routing.
2. Statistics Tracking
cppint requestCount = 0;
map<string, int> endpointStats;
Global counters track total requests and per-endpoint hits.
3. Time Utilities
cppstring getCurrentTime() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
Formats timestamps for logging and API responses.
4. Dynamic Routing
cppserver.Get(R"(/greet/(\w+))", [](const httplib::Request& req, httplib::Response& res) {
    auto name = req.matches[1];
    // Generate personalized response
});
Regex-based path parameters for flexible routing.
 Features Breakdown
FeatureDescriptionImplementationLoggingTimestamps all requestsMiddleware + chronoStatisticsCounts requests per routeSTL map + countersUptimeTracks server runtimesteady_clockPOST EchoReturns request bodyBody parsing404 HandlerCustom error pagesset_error_handlerJSON APIMultiple REST endpointsManual JSON construction
 Troubleshooting
Port Already in Use
bash# Change port in main.cpp line 185
server.listen("0.0.0.0", 9090); // Use different port
Winsock Error (Windows)
Ensure you're linking against ws2_32.lib:
bashg++ main.cpp -lws2_32
Compilation Errors
Make sure you're using C++11 or later:
bashg++ -std=c++11 main.cpp -o server
```


```
========================================
  C++ HTTP Server Starting...
========================================
  Server:  http://localhost:8080
  Started: 2025-12-28 15:30:45
========================================
[2025-12-28 15:31:02] GET /
[2025-12-28 15:31:05] GET /api/status
[2025-12-28 15:31:08] POST /api/echo
[2025-12-28 15:31:12] GET /greet/john
Security Notes

This is a development server - not production-ready
No authentication/authorization implemented
No input sanitization (vulnerable to injection)
No rate limiting
For learning purposes only

 Future Enhancements

 Add authentication (JWT/Basic Auth)
 Database integration (SQLite)
 WebSocket support
 File upload handling
 CORS configuration
 Rate limiting
 Request validation
 Logging to file

 Dependencies

cpp-httplib v0.20.0 - Single-header C++11 HTTP/HTTPS library (MIT License)
Winsock2 (Windows only)

 License
This project uses the MIT License.

