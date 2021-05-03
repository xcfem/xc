# XC installation.

## Linux
For linux see file https://github.com/xcfem/xc/blob/master/install/install.linux.md


## Windows 10

- Install Ubuntu on Windows 10: the [Windows Subsystem for Linux (WSL)](https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux) is a compatibility layer for running Linux binary executables natively on Windows 10 (take a look [here](https://ubuntu.com/tutorials/ubuntu-on-windows#1-overview) and [here](https://www.microsoft.com/en-us/p/ubuntu/9nblggh4msv6?activetab=pivot:overviewtab)).

- Install a XWindow server: You'll also need an [X Window System](https://en.wikipedia.org/wiki/X_Window_System) server for Windows 10 (apparently Microsoft has plans to integrate an XWindow server inside Windows, but I think there is nothing usable yet). Anyway, you can use [Xming]( http://www.straightrunning.com/XmingNotes/) which works just fine.

- Install XC on your Ubuntu subsystem using the [regular installation procedure](https://github.com/xcfem/xc/blob/master/install/install.linux.md).

## OSX

- Install Ubuntu on Mac OS using VirtualBox: You can install Ubuntu on Mac OS using [VirtualBox](https://www.virtualbox.org/). There is a tutorial on how to do this [here](https://codingwithmanny.medium.com/installing-ubuntu-18-04-on-mac-os-with-virtualbox-ac3b39678602).

- Install a XWindow server: [XQuartz](https://www.xquartz.org/) is an open source project to develop a version of the  [X Window System](https://en.wikipedia.org/wiki/X_Window_System) that runs on MacOS. 

- Install XC on your Ubuntu virtual machine using the [regular installation procedure](https://github.com/xcfem/xc/blob/master/install/install.linux.md).





