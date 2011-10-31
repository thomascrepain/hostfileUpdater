/*
 * "THE BEER-WARE LICENSE" (Revision 42): 
 * <info@thomascrepain.be> wrote this file. As long as you retain this notice you can do whatever you want with this stuff. 
 * If we meet some day, and you think this stuff is worth it, you can buy me a beer in return. 
 * Thomas Crepain <info@thomascrepain.be>
 */
 
#include <fstream>
#include <string>

#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTTP_USER_AGENT "HTTPRIP"
#define BUFFER_SIZE 512

using namespace std;

/**
 * Copies a file
 * 
 * @return bool true if successfull
 * @param string originalFilename
 * @param string copyFilename
 */
bool copyFile(string originalFilename, string copyFilename){
	bool isSuccessfull = false;
	ifstream originalStream;
	ofstream copyStream;
	
	// open streams
	originalStream.open(originalFilename.c_str());
	copyStream.open(copyFilename.c_str());

	if (originalStream.is_open() && copyStream.is_open()){
		string line;
		
		getline(originalStream, line);
		
		while (!originalStream.fail() && !copyStream.fail()){
			copyStream << line << endl;
			getline(originalStream, line);
		}
		if (originalStream.eof()){
			isSuccessfull = true;
		}
		
		// close streams
		originalStream.close();
		originalStream.clear();
		copyStream.close();
		copyStream.clear();
	}
	
	return isSuccessfull;
}

/**
 * Checks if a file already exists
 * 
 * @return bool true if the specified file exists
 * @param string filename
 */
bool existsFile(string filename){
	bool isSuccessfull = false;
	ifstream fileStream;
	
	fileStream.open(filename.c_str());

	isSuccessfull = fileStream.is_open();

	fileStream.close();
	fileStream.clear();
	
	return isSuccessfull;
}

/**
 * Combines both source files to the specified target file
 * 
 * @return void
 * @param string targetFile
 * @param string sourceFile1
 * @param string sourcefile2
 */
void glueFiles(string targetFile, string sourceFile1, string sourcefile2){
	ifstream inputStream1;
	ifstream inputStream2;
	ofstream outputStream;
	
	// open streams
	inputStream1.open(sourceFile1.c_str());
	inputStream2.open(sourcefile2.c_str());
	outputStream.open(targetFile.c_str());

	// test if every file was succesfully opened
	if (inputStream1.is_open() && inputStream2.is_open() && outputStream.is_open()){
		string line;
		
		getline(inputStream1, line);
		
		while (!inputStream1.fail() && !outputStream.fail()){
			outputStream << line << endl;
			
			getline(inputStream1, line);
		}
		
		if (inputStream1.eof()){
			inputStream1.close();
			inputStream1.clear();
			
			outputStream << endl << endl;
			outputStream << "# redirects for ads" << endl;
			
			getline(inputStream2, line);
			while (!inputStream2.fail() && !outputStream.fail()){
				if (line.substr(0, 1) != "#") outputStream << line << endl;
				getline(inputStream2, line);
			}
		
		}
		
		// close streams
		inputStream2.close();
		inputStream2.clear();
		outputStream.close();
		outputStream.clear();
	}
}

/**
 * Retrieves a file from the internet
 * 
 * @return int returns 1 if successfull
 * @param string requesturlPar	The file url on the net
 * @param string outputfilePar	Location of retrieved file
 */
int InternetSaveFile(string requesturlPar, string outputfilePar)
{
	const char *requesturl = requesturlPar.c_str();
	const char *outputfile = outputfilePar.c_str();
	const char http[] = "HTTP";
	HANDLE file;
	SECURITY_ATTRIBUTES sa;
	HINTERNET internet;
	HINTERNET url;
	char *buf = NULL;
	DWORD bytesread;
	DWORD byteswritten;
	int k;
	
	// validate protocol -- first 4 alphas should be http, case insensitive
	for (k = 0; k < 4; k++)
		if (requesturl[k] != http[k] &&	requesturl[k] != (char)(http[k] + 32))
		return 0; // need error handling
	
	file = CreateFile(outputfile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) return 0; // need error handling
	
	internet = InternetOpen(HTTP_USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);	
	if (internet == NULL) return 0; // need error handling
	
	url = InternetOpenUrl(internet, requesturl, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE, 0);
	if (url == NULL) return 0; // need error handling
	
	buf = (char *) malloc(BUFFER_SIZE + 1);
	if (buf == NULL) return 0; // need error handling
	
	while (InternetReadFile(url, buf, BUFFER_SIZE, &bytesread)) {
		if (bytesread == 0) break;
		
		WriteFile(file, buf, bytesread, &byteswritten, NULL);
	}
	
	free(buf);
	InternetCloseHandle(url);
	InternetCloseHandle(internet);
	CloseHandle(file);
	
	return 1;
} 

/**
 * Suppresses all noise and windows
 *
 * @return void
 */
void stealth()
{
     HWND stealth;
     AllocConsole();
     stealth = FindWindowA("ConsoleWindowClass", NULL);
     ShowWindow(stealth,0);
}

/**
 * The main application. 
 * Creates a backup of the hostfile, downloads the remote file and combines it with the original file
 *
 * @return int 0 if successfull
 */
int main(){
    stealth();
    
	const string HOSTS_FILENAME = "hosts";
	const string BACKUP_EXTENSION = "backup";
	const string ORIGINAL_EXTENSION = "original";
	const string REMOTE_EXTENSION = "remote";
	const string REMOTE_HOSTSFILE = "http://www.mvps.org/winhelp2002/hosts.txt";

	// create backup hostsfile
	if(copyFile(HOSTS_FILENAME, HOSTS_FILENAME + "." + BACKUP_EXTENSION)){
	
		// test if hosts.original exists
		if(!existsFile(HOSTS_FILENAME + "." + ORIGINAL_EXTENSION)){
			// copy hosts -> hosts.original
			copyFile(HOSTS_FILENAME, HOSTS_FILENAME + "." + ORIGINAL_EXTENSION);
		}

		if (InternetSaveFile(REMOTE_HOSTSFILE, HOSTS_FILENAME + "." + REMOTE_EXTENSION))
		{
			glueFiles(HOSTS_FILENAME, HOSTS_FILENAME + "." + ORIGINAL_EXTENSION, HOSTS_FILENAME + "." + REMOTE_EXTENSION);
		}	
	}
	
	return 0;
}
