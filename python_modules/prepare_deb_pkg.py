# -*- coding: utf-8 -*-
#!/usr/bin/env python

# Copyright (C) 2024  Luis C. Pérez Tato
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.xcingenieria.com/ or email : l.perez@xcingenieria.com
#

import os
import logging
import platform;

xc_deb_pkg_folder= None
xc_installation_target= None
usr_local_pth= None
with open('./xc_installation_target.txt','r') as f:
    xc_version= f.readline().strip()
    sys_arch= f.readline().strip()
    xc_deb_pkg_folder= f.readline().strip()
    xc_installation_target= f.readline().strip()
    usr_local_pth= f.readline().strip()
    pth_to_libs= f.readline().strip()
if (xc_version is None):
    logging.error('XC_VERSION not set.')
    exit(1)
if (sys_arch is None):
    logging.error('SYS_ARCH not set.')
    exit(1)
if (xc_deb_pkg_folder is None):
    logging.error('XC_DEB_PKG_FOLDER not set.')
    exit(1)
if (xc_installation_target is None):
    logging.error('XC_INSTALLATION_TARGET not set.')
    exit(1)
if (usr_local_pth is None):
    logging.error('USR_LOCAL not set.')
    exit(1)
if (pth_to_libs is None):
    logging.error('PTH_TO_LIBS not set.')
    exit(1)

# Prepare Debian package folder.
## Create config.ini
cmd= 'touch '+xc_deb_pkg_folder+'/config.ini'
os.system(cmd)
## Create DEBIAN directory
cmd= 'mkdir '+xc_deb_pkg_folder+'/DEBIAN/'
os.system(cmd)
### Create control file.
control_file_contents= '''Package: xc-python-modules
Version: xc_version
Architecture: sys_arch
Maintainer: Luis C. Pérez Tato <l.pereztato@gmail.com>
Description: Python modules of the XC finite element analysis program.
'''
#### Set version.
control_file_contents= control_file_contents.replace('xc_version', xc_version)
#### Get architecture.
machine= platform.machine()
control_file_contents= control_file_contents.replace('sys_arch', sys_arch)

control_file_path= xc_deb_pkg_folder+'/DEBIAN/control'
with open(control_file_path, 'w') as f:
    f.write(control_file_contents)
postinst_file_name= xc_deb_pkg_folder+'/DEBIAN/postinst'
cmd= 'touch '+postinst_file_name
os.system(cmd)
os.chmod(postinst_file_name, 0o755)
# Move Python modules to the corresponding Python packages folder
cmd= 'mkdir -p '+pth_to_libs
os.system(cmd)
origin= xc_installation_target+'/*'
dest= pth_to_libs
cmd= 'mv '+origin+' '+dest
os.system(cmd)
print('cmd= ', cmd)
cmd= 'rmdir '+ xc_installation_target
os.system(cmd)
print('cmd= ', cmd)

## Copy the C++ libraries to the corresponding Python packages folder
cpp_libraries= ['extensions/xc_base.so','extensions/loadCombinations.so','extensions/geom.so','extensions/xcGnuGts.so','extensions/xc.so']
for cpp_lib in cpp_libraries:
    origin= cpp_lib
    dest= pth_to_libs+'/'+origin.split('/')[-1]
    cmd= 'cp '+origin+' '+dest
    print('copying '+origin+' to '+dest)
    os.system(cmd)