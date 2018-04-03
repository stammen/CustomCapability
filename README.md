# CustomCapability
UWP HSA example demonstrating how to send audio buffers from an NT-Service to a UWP app via RPC


## Requirements
* Visual Studio 2017 with Windows Universal App Development package installed
* Windows SDK 16299 (installed with Visual Studio 2017) or SDK 15063

## How to Build

* Open the CustomCapability.sln with Visual Studio 2017

* Build the solution

* Open a Command Prompt with Administator access

* In the Command Prompt, cd to the RpcServer.exe directory (for example x64\Debug\ or x64\Release

* Start the RPCServer with RpcServer.exe -console

* In Visual Studio start the CustomCapability app

* Press the Open button to open a connection with the RPC server

* Press the Start button to start streaming audio from the RPC server to the app

* Note: Audio will be recorded from the mic and played on the speakers. Use a headset or reduce volume to avoid feedback. This is just a demo.



