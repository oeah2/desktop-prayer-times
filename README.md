# desktop-prayer-times

Welcome to the open source, cross plattform desktop prayer times app!

This app aims to provide an open source and distraction free solution to display the muslim prayer times on the desktop. It is lightweight and provides calculation methods according to different schools of thought. 
To use the app you need to install the GTK runtime environment and OpenSSL. To run, simply download the repository and either build the application using make or use the precompiled binaries.


Currently known Issues:
- Socket implementation can be buggy on windows.
- On Linux the socket is implemented in a blocking way; if you don't have internet connection the app might crash.
- moving and editing cities not implemented.
- Update checking needs to be reworked
- Language files are maintained on their own.


Todo:
- Fix bug in windows socket implementation
- Make sockets also not blocking in linux
- Rework socket so the internet connection is handled in a second thread.
- Rework update checking
- Create distribution files
- Rework makefile
- Create a helper tool to create language files out of an database.


If you want to contribute, don't be shocked about the core of the application. It evolved over an considerable amount of time. Some parts are pretty messy but need reworking in future.