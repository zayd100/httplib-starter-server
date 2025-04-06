#include "httplib.h"
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

int main() {
    // Initialize Winsock (necessary for Windows)
    WSADATA wsaData;
    int wsInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsInit != 0) {
        cerr << "Winsock initialization failed with error code: " << wsInit << endl;
        return 1;
    }

    // Use HTTP Server
    httplib::Server server;

    // Route: /
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        string html = R"(
            <html>
                <head><title>Welcome</title></head>
                <body>
                    <h1>🔒C++ Server</h1>
                    <p>HTTP C++ server is running.</p>
                </body>
            </html>
        )";
        res.set_content(html, "text/html");
    });

    // Route: /json
    server.Get("/json", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"({"status": "online", "message": "Welcome, Zayd"})", "application/json");
    });

    cout << "🚀 HTTP Server running at http://localhost:8080" << endl;
    server.listen("0.0.0.0", 8080);

    // Clean up Winsock before exiting
    WSACleanup();

    return 0;
}