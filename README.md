# hostfile updater
Downloads the most recent hostfile from http://winhelp2002.mvps.org/ and glues it together with the original host file

## Build project
g++.exe -c main.cpp -o main.o 
g++.exe main.o -o "hostfileupdater.exe" -mwindows -lWininet -g3 

## Usage
Copy the .exe file to your C:\Windows\System32\drivers\etc folder. 

The application will generate the following files:
* hosts.backup		A backup of your hostfile
* hosts.original	This is the base file, the remote file will be combined with this file
* hosts.remote		The file that was downloaded from mvps.org

If you want to make changes to your hostfile, you can do so in hosts.original. Just update that file and run the .exe again.

## Contributors

* [Thomas Crepain](http://www.thomascrepain.be/)