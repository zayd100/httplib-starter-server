#include "httplib.h"
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

// Request counter for stats
int requestCount = 0;
map<string, int> endpointStats;

// Helper function to get current timestamp
string getCurrentTime() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Helper function to get uptime
string getUptime(chrono::steady_clock::time_point start) {
    auto now = chrono::steady_clock::now();
    auto uptime = chrono::duration_cast<chrono::seconds>(now - start).count();
    int hours = uptime / 3600;
    int minutes = (uptime % 3600) / 60;
    int seconds = uptime % 60;
    
    stringstream ss;
    ss << hours << "h " << minutes << "m " << seconds << "s";
    return ss.str();
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock" << endl;
        return 1;
    }

    httplib::Server server;
    auto startTime = chrono::steady_clock::now();

    // Middleware: Log all requests
    server.set_pre_routing_handler([](const httplib::Request& req, httplib::Response&) {
        cout << "[" << getCurrentTime() << "] " << req.method << " " << req.path << endl;
        requestCount++;
        endpointStats[req.path]++;
        return httplib::Server::HandlerResponse::Unhandled;
    });

    // Route: Home page
    server.Get("/", [&](const httplib::Request&, httplib::Response& res) {
        string html = R"(
            <!DOCTYPE html>
            <html>
            <head>
                <title>C++ HTTP Server</title>
                <style>
                    body { 
                        font-family: Arial, sans-serif; 
                        max-width: 800px; 
                        margin: 50px auto; 
                        padding: 20px;
                        background: #f5f5f5;
                    }
                    .container { 
                        background: white; 
                        padding: 30px; 
                        border-radius: 8px; 
                        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
                    }
                    h1 { color: #333; }
                    .endpoint { 
                        background: #e8f4f8; 
                        padding: 10px; 
                        margin: 10px 0; 
                        border-left: 4px solid #2196F3;
                        border-radius: 4px;
                    }
                    code { 
                        background: #f4f4f4; 
                        padding: 2px 6px; 
                        border-radius: 3px;
                        font-family: 'Courier New', monospace;
                    }
                </style>
            </head>
            <body>
                <div class="container">
                    <h1>C++ HTTP Server</h1>
                    <p>Welcome! This server is built with cpp-httplib.</p>
                    
                    <h2>Available Endpoints:</h2>
                    <div class="endpoint">
                        <strong>GET</strong> <code>/</code> - This page
                    </div>
                    <div class="endpoint">
                        <strong>GET</strong> <code>/api/status</code> - Server status (JSON)
                    </div>
                    <div class="endpoint">
                        <strong>GET</strong> <code>/api/time</code> - Current server time
                    </div>
                    <div class="endpoint">
                        <strong>GET</strong> <code>/api/stats</code> - Request statistics
                    </div>
                    <div class="endpoint">
                        <strong>POST</strong> <code>/api/echo</code> - Echo back your message
                    </div>
                    <div class="endpoint">
                        <strong>GET</strong> <code>/greet/:name</code> - Personalized greeting
                    </div>
                </div>
            </body>
            </html>
        )";
        res.set_content(html, "text/html");
    });

    // Route: Server status
    server.Get("/api/status", [&](const httplib::Request&, httplib::Response& res) {
        string json = R"({
            "status": "online",
            "uptime": ")" + getUptime(startTime) + R"(",
            "timestamp": ")" + getCurrentTime() + R"(",
            "total_requests": )" + to_string(requestCount) + R"(
        })";
        res.set_content(json, "application/json");
    });

    // Route: Current time
    server.Get("/api/time", [](const httplib::Request&, httplib::Response& res) {
        string json = R"({"current_time": ")" + getCurrentTime() + R"("})";
        res.set_content(json, "application/json");
    });

    // Route: Request statistics
    server.Get("/api/stats", [](const httplib::Request&, httplib::Response& res) {
        stringstream json;
        json << "{\n  \"total_requests\": " << requestCount << ",\n";
        json << "  \"endpoints\": {\n";
        
        int count = 0;
        for (const auto& pair : endpointStats) {
            if (count > 0) json << ",\n";
            json << "    \"" << pair.first << "\": " << pair.second;
            count++;
        }
        
        json << "\n  }\n}";
        res.set_content(json.str(), "application/json");
    });

    // Route: Echo endpoint (POST)
    server.Post("/api/echo", [](const httplib::Request& req, httplib::Response& res) {
        if (req.body.empty()) {
            res.set_content(R"({"error": "No message provided"})", "application/json");
            res.status = 400;
            return;
        }
        
        string json = R"({
            "message": ")" + req.body + R"(",
            "length": )" + to_string(req.body.length()) + R"(,
            "timestamp": ")" + getCurrentTime() + R"("
        })";
        res.set_content(json, "application/json");
    });

    // Route: Personalized greeting with path parameter
    server.Get(R"(/greet/(\w+))", [](const httplib::Request& req, httplib::Response& res) {
        auto name = req.matches[1];
        string html = R"(
            <!DOCTYPE html>
            <html>
            <head>
                <title>Greeting</title>
                <style>
                    body { 
                        font-family: Arial, sans-serif; 
                        text-align: center; 
                        padding: 100px;
                        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
                        color: white;
                    }
                    h1 { font-size: 3em; }
                </style>
            </head>
            <body>
                <h1> Hello, )" + name + R"(!</h1>
                <p>Welcome to the C++ server</p>
            </body>
            </html>
        )";
        res.set_content(html, "text/html");
    });

    // 404 handler
    server.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        string html = R"(
            <!DOCTYPE html>
            <html>
            <head>
                <title>404 Not Found</title>
                <style>
                    body { 
                        font-family: Arial, sans-serif; 
                        text-align: center; 
                        padding: 100px;
                        background: #f5f5f5;
                    }
                    h1 { color: #e74c3c; font-size: 4em; }
                </style>
            </head>
            <body>
                <h1>404</h1>
                <p>The page you're looking for doesn't exist.</p>
                <a href="/">Go Home</a>
            </body>
            </html>
        )";
        res.set_content(html, "text/html");
    });

    cout << "========================================" << endl;
    cout << "  C++ HTTP Server Starting..." << endl;
    cout << "========================================" << endl;
    cout << "  Server:  http://localhost:8080" << endl;
    cout << "  Started: " << getCurrentTime() << endl;
    cout << "========================================" << endl;

    server.listen("0.0.0.0", 8080);

    WSACleanup();
    return 0;
}
