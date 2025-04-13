XC installation scripts for Linux operating system
==================================================

## Dependencies
Depending on the various distribution out there we gone provide scripts to install the needed dependency packages needed to compile XC.

### Note: Windows Subsystem for Linux
If you are running in Windows Subsystem for Linux, launch your WSL terminal and install python3 and make sure it is the default on your system. Then close all your terminals and launch it again to continue the installation process.

``` console
sudo apt-get install -y python3 python-is-python3
#
```

#### Ubuntu Noble (Ubuntu 24.04)

- packages_install_ubuntu_24.04.sh
- https://github.com/xcfem/xc/blob/master/install/packages_install_ubuntu_24.04.sh
```console
wget https://raw.githubusercontent.com/xcfem/xc/master/install/packages_install_ubuntu_24.04.sh
sudo bash packages_install_ubuntu_24.04.sh
#
```

#### Ubuntu Focal Fossa (Ubuntu 20)

- packages_install_ubuntu_focal_fossa.sh
- https://github.com/xcfem/xc/blob/master/install/packages_install_ubuntu_focal_fossa.sh
```console
wget https://raw.githubusercontent.com/xcfem/xc/master/install/packages_install_ubuntu_focal_fossa.sh
sudo bash packages_install_ubuntu_focal_fossa.sh
#
```


#### Debian Stretch 

- packages_install_debian_stretch.sh
- https://github.com/xcfem/xc/blob/master/install/old/packages_install_debian_stretch.sh
```console
wget https://raw.githubusercontent.com/xcfem/xc/master/install/old/packages_install_debian_stretch.sh
sudo bash packages_install_debian_stretch.sh
#
```

#### Ubuntu Bionic Beaver (Ubuntu 18)

- packages_install_ubuntu_bionic.sh
- https://github.com/xcfem/xc/blob/master/install/old/packages_install_ubuntu_bionic.sh
```console
wget https://raw.githubusercontent.com/xcfem/xc/master/install/old/packages_install_ubuntu_bionic.sh
sudo bash packages_install_ubuntu_bionic.sh
#
```


## Compile XC
All this is done in the following script.

- xc_code_install.sh
- https://github.com/xcfem/xc/blob/master/install/xc_code_install.sh
```console
wget https://raw.githubusercontent.com/xcfem/xc/master/install/xc_code_install.sh
bash xc_code_install.sh
#
```
