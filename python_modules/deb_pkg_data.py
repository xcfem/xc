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

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.ortega.ort@gmail.com"

debian_package_name= 'xc-python-modules'
control_file_contents= '''Package: @debian_package_name@
Version: xc_version
Architecture: sys_arch
Maintainer: Luis C. Pérez Tato <l.pereztato@gmail.com>
Description: Python modules of the XC finite element analysis program.
'''
control_file_contents= control_file_contents.replace('@debian_package_name@', debian_package_name)

if __name__ == '__main__':
    print(debian_package_name)
