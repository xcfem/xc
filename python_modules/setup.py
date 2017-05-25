# -*- coding: utf-8 -*-
#!/usr/bin/env python

# Copyright (C) 2009-2012  Luis C. Pérez Tato
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

from distutils.core import setup, sys
from distutils.sysconfig import get_python_lib

myPrefix = sys.prefix
if len (sys.argv) > 2:
    i = 0
    for o in sys.argv:
        if o.startswith ("--prefix"):
            if o == "--prefix":
                if len (sys.argv) >= i:
                    myPrefix = sys.argv[i + 1]
                sys.argv.remove (prefix)
            elif o.startswith ("--prefix=") and len (o[9:]):
                myPrefix = o[9:]
            sys.argv.remove (o)
        i += 1
if not myPrefix and "PREFIX" in os.environ:
    myPrefix = os.environ["PREFIX"]
if not myPrefix or not len (myPrefix):
    myPrefix = "/usr/local"

pth_to_libs= get_python_lib(1,0,myPrefix)
print pth_to_libs

setup(name='XCModules',
      version='0.12.0',
      author='Luis C. Pérez Tato',
      packages=['rough_calculations','solution', 'model', 'model/boundary_cond','model/geometry', 'model/grid_based', 'model/mesh','postprocess','import_export','import_export/sciaXML','import_export/sciaXML/xml_basics','import_export/sciaXML/scia_loads','postprocess/reports','postprocess/elastic_beam_3d',"actions","actions/quake","actions/earth_pressure","actions/railway_trafic","actions/snow","actions/weight","actions/wind",'materials',"materials/ec2","materials/ec3","materials/ec5","materials/ehe","materials/eae","materials/fiber_section","materials/perfiles_metalicos","materials/perfiles_metalicos/aisc","materials/perfiles_metalicos/arcelor","materials/sia262","materials/sia263","materials/anchor_bolts","import_export","misc","xcVtk","xcVtk/malla_ef","xcVtk/malla_cad"],
      data_files=[(pth_to_libs, ['extensions/xc.so'])]
     )
