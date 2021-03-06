# boost-file-storage
File storage server (Windows service) and client written using boost::asio
### Running
0. Install [wxWidgets](https://www.wxwidgets.org/), [boost](https://www.boost.org/), [Visual Studio](https://visualstudio.microsoft.com/) and [CMake 3.12+](https://cmake.org/) (optional).
1. Compile `service` and `client` with top-level CMakeLists.
2. Compile event logger:
	1. Enter VS command line tool in `service` directory.
	2. Print next commands:
		* mc -U BoostFileStorageEventProvider.mc;
		* rc BoostFileStorageEventProvider.mc;
		* link -dll -noentry BoostFileStorageEventProvider.res.
3. Place compiled event logger into the same directory as compiled `service`.
4. Run `service`:
    * if you've compiled it in `DEBUG` mode, just run it as usual application;
    * if you've compiled it in `RELEASE` mode, run it with `-i` argument, and then you can enable it in Windows Service Manager as `BoostFileStorage`;
	* in both methods you should set config for server, with command line arguments or service start parameter in following order: `<port> <download folder path> <max size of file, in bytes> <number of threads>`.
5. Run clinet as usual GUI app.
