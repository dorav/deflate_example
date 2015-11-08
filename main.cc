#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>

using namespace std;

typedef int FileDescriptor;

FileDescriptor connectTo(string hostAddr, int port)
{
	sockaddr_in serv_addr = {0};

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(inet_network(hostAddr.c_str()));
	in_addr_t addr_t = (serv_addr.sin_addr.s_addr);
	in_addr* addr = reinterpret_cast<in_addr*>(&addr_t);
	cout << "trying to connect to " << inet_ntoa(*addr) << ":" << ntohs(serv_addr.sin_port) << endl;
	int tryNum = 1;
	while (connect(fd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		++tryNum;
		cout << "unable to connect because of " << strerror(errno) << ", trying again.. " << flush;
		usleep (100000);	
		cout << "now, for the #" << tryNum << " time" << endl;
	}
	
	cout << "Connected!" << endl;

}

struct Data
{
	long unsigned int length;
	char* buffer;
};

Data compress(string str)
{
	uLongf compressedSize = compressBound(str.length());
	unsigned char* destBuffer = new unsigned char[compressedSize];
	const char* source_ = str.c_str();
	const unsigned char* source = reinterpret_cast<const unsigned char*>(source_);
	auto errorCode = compress(destBuffer, &compressedSize, source, str.length());
	if (errorCode != Z_OK)
		throw errorCode;
	return Data { compressedSize, (char*) destBuffer };
}

Data uncompress(const void* input_, int inputSize_, unsigned int expectedSize)
{
	const Bytef* input = (const Bytef*) input_;
	uLongf expectedSize_ = expectedSize;
	
	unsigned char* output = new unsigned char[expectedSize];
	int errorCode = uncompress(output, &expectedSize_,
				   input, inputSize_);
	if (errorCode != Z_OK)
		throw errorCode;
	return Data { expectedSize, (char*) output };
}

int main()
{
	//int fd = connectTo("127.0.0.1", 34567);
	
	try
	{
		cout << "Compressing .. ";
		string originalString = "hello world";
		Data data = compress(originalString);
		cout << "Success" << endl << "uncompressing.. :";
		data = uncompress(data.buffer, data.length, originalString.length());
		cout << "Success" << endl << "resulted string is : " << data.buffer << endl;
	}
	catch (int errorCode)
	{
		cout << "Error - " << errorCode << endl;
	}
	
		
	return 0;
}
