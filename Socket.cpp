#include "Socket.h"

Socket::Socket() {
	//Create the socket
	this->innerSocket = socket(AF_INET, SOCK_STREAM, 0);
}

Socket::~Socket(){
	//Close the socket
	closesocket(this->innerSocket);
}

void Socket::InitSockets() {
	//Initialize WinSock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &wsData);
}

void Socket::CloseSockets(){
	WSACleanup();
}

void Socket::ListenPort(int port) {
	//Bind the ip address and port on the socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(this->innerSocket, (sockaddr*)&hint, sizeof(hint));

	//Tell WinSock that the socket is for listening
	listen(this->innerSocket, SOMAXCONN);
}

void Socket::Accept() {
	sockaddr_in client_info;
	int clientSize = sizeof(client_info);
	//SOCKET client = this->innerSocket;
	this->innerSocket = accept(this->innerSocket, (sockaddr*)&client_info, &clientSize);
	char clientName[NI_MAXHOST];//also known as host name(client remote name)
	ZeroMemory(clientName, NI_MAXHOST);
	inet_ntop(AF_INET, &client_info.sin_addr, clientName, NI_MAXHOST);
	this->ClientIpAddress = clientName;
}

void Socket::Connect(std::string ipAddress, int port) {
	//fill in a hint struct
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//connect to server
	connect(this->innerSocket, (sockaddr*)&hint, sizeof(hint));
}

std::string Socket::GetClientIp() {
	return ClientIpAddress;
}

void Socket::Send(char* bytes, int size) {
	send(this->innerSocket, bytes, size, 0);
}

bool Socket::Recv(char* bytes, int size) {
	if (this->blocking) {
		int bytesReceived = recv(this->innerSocket, bytes, size, 0);
		if (bytesReceived <= 0) {
			// Drop the client
			//closesocket(sock.innerSocket);
		}
	}
	else {
		//try to get some bytes and if it wouldn't block then get the actual bytes
		recv(this->innerSocket, bytes, size, MSG_PEEK);
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK) {
			int bytesReceived = recv(this->innerSocket, bytes, size, 0);
			return true;
		}
	}
	return false;
}

void Socket::NonBlocking() {
	unsigned long ul = 1;
	ioctlsocket(this->innerSocket, FIONBIO, (unsigned long*)&ul);
	this->blocking = false;
}