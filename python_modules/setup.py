# -*- coding: utf-8 -*-
#!/usr/bin/env python

from __future__ import print_function

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
import site

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

site_packages= site.getsitepackages()
for pth in site_packages:
    if(pth.startswith(myPrefix)):
        pth_to_libs= pth
print('path to libs: '+pth_to_libs)

setup(name='XCModules',
      version='0.12.0',
      author='Luis C. Pérez Tato',
      packages=['geotechnics', 'geotechnics.foundations', 'geotechnics.tuneles', 'rough_calculations','solution', 'model', 'model/boundary_cond','model/geometry', 'model/mesh','model/sets','postprocess','import_export','import_export/sciaXML','import_export/sciaXML/xml_basics','import_export/sciaXML/scia_loads','postprocess/gnuplot','postprocess/config','postprocess/reports','postprocess/elements','postprocess/quick_inquiry',"actions","actions/basic_loads","actions/quake","actions/earth_pressure","actions/railway_trafic","actions/roadway_trafic","actions/snow","actions/weight","actions/wind","actions/thermal","actions/prestress","actions/imposed_strain",'actions/utils','materials',"materials/aci","materials/aisi","materials/astm_aisc","materials/ec2","materials/ec3","materials/ec5","materials/ceb","materials/ehe","materials/eae","materials/sections","materials/sections/fiber_section","materials/sections/structural_shapes","materials/sia262","materials/sia263","materials/eota","materials/prestressing","materials/awc_nds","materials/tm5_809_3","import_export","misc","postprocess/xcVtk","postprocess/xcVtk/fields","postprocess/xcVtk/diagrams","postprocess/xcVtk/FE_model","postprocess/xcVtk/CAD_model", "connections/steel_connections"],
      package_data={'': ['*.pickle']},
      data_files=[(pth_to_libs, ['extensions/xc.so'])]
     )
