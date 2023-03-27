#pragma once
#include <WS2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

class Socket {
private:
	std::string ClientIpAddress;
	SOCKET innerSocket;
	bool blocking = true;

public:
	Socket();
	~Socket();

	static void InitSockets();
	static void CloseSockets();
	void ListenPort(int port);
	void Connect(std::string ipAddress, int port);
	void Accept();
	std::string GetClientIp();
	void Send(char* bytes, int size);
	bool Recv(char* bytes, int size);
	void NonBlocking();
};