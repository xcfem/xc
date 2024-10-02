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

from setuptools import setup
from setuptools import find_packages
from setuptools.command.install import install as _install
from setuptools.dist import Distribution
# from distutils.core import setup, sys
# from distutils.sysconfig import get_python_lib
import site
import shutil

# Get installation prefix.
myPrefix = os.environ.get("XC_INSTALLATION_PREFIX")
if not myPrefix and "PREFIX" in os.environ:
    myPrefix = os.environ.get("PREFIX")
if not myPrefix or not len(myPrefix):
    myPrefix = "/usr/local"

site_packages= site.getsitepackages()
for pth in site_packages:
    if(pth.startswith(myPrefix)):
        pth_to_libs= pth
print('XC installation prefix: '+myPrefix)
print('XC path to libs: '+pth_to_libs)

pth_to_steel_shape_data= pth_to_libs+'/materials/sections/structural_shapes/aux'
print('XC path to steel shape data: '+pth_to_steel_shape_data)
steel_shape_data= ['materials/sections/structural_shapes/aux/arcelor_r_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_au_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_l_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_chs_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_rhs_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_he_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_hl_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_shs_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_ipe_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_uc_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_ub_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_ipn_profiles.json', 'materials/sections/structural_shapes/aux/arcelor_upn_profiles.json', 'materials/sections/structural_shapes/aux/bs_en_10210_hfshs_profiles.json', 'materials/sections/structural_shapes/aux/bs_en_10219_profiles.json', 'materials/sections/structural_shapes/aux/common_micropile_tubes.json']

pth_to_awc_nds_section_data= pth_to_libs+'/materials/awc_nds/aux/'
print('path to awc_nds shape data: '+pth_to_awc_nds_section_data)
awc_nds_section_data= ['materials/awc_nds/aux/pr_400_i_joists.json']

xc_python_packages= ['geom_utils', 'geotechnics', 'geotechnics.foundations', 'geotechnics.earth_retaining', 'geotechnics.earthworks', 'geotechnics.tuneles', 'rough_calculations','solution', 'model', 'model.boundary_cond','model.geometry', 'model.mesh','model.sets','postprocess','import_export','import_export.sciaXML','import_export.sciaXML.xml_basics','import_export.sciaXML.scia_loads','postprocess.gnuplot','postprocess.config','postprocess.reports','postprocess.elements','postprocess.quick_inquiry',"actions","actions.basic_loads","actions.quake","actions.earth_pressure","actions.pedestrian_traffic","actions.railway_traffic","actions.roadway_traffic","actions.snow","actions.weight","actions.wind","actions.thermal","actions.prestress","actions.imposed_strain",'actions.load_combination_utils', 'actions.utils','materials',"materials.aci","materials.aisi","materials.astm_aisc","materials.ec2", "materials.mc10","materials.ec3","materials.ec5","materials.ceb","materials.ehe","materials.en_1337","materials.nbe_ea95","materials.eae","materials.api","materials.sections","materials.sections.fiber_section","materials.sections.structural_shapes","materials.sections.structural_shapes.aux","materials.sia262","materials.sia263","materials.eota","materials.prestressing","materials.awc_nds", "materials.awc_nds.aux","materials.tm5_809_3","materials.tie_bars", "import_export", "postprocess.serviceability_limit_states", "postprocess.xcVtk","postprocess.xcVtk.fields","postprocess.xcVtk.diagrams","postprocess.xcVtk.FE_model","postprocess.xcVtk.CAD_model", "connections.steel_connections","misc","misc.latex","misc.sqlite_utils","misc.vtk_utils","misc_utils"]

cpp_libraries= ['extensions/xc_base.so','extensions/loadCombinations.so','extensions/geom.so','extensions/xcGnuGts.so','extensions/xc.so']

def post_install():
    '''Install C++ libraries and material data.'''
    # Copy the C++ libraries to pth_to_libs
    for cpp_lib in cpp_libraries:
        origin= cpp_lib
        dest= pth_to_libs+'/'+origin.split('/')[-1]
        cmd= 'cp '+origin+' '+dest
        print('copying '+origin+' to '+dest)
        os.system(cmd)
    # # Copy steel shapes data.
    # for steel_shapes in steel_shape_data:
    #     origin= steel_shapes
    #     dest= pth_to_steel_shape_data+'/'+origin.split('/')[-1]
    #     cmd= 'cp '+origin+' '+dest
    #     print('copying '+origin+' to '+dest)
    #     os.system(cmd)
    # # Copy awc section data.
    # for section_data in awc_nds_section_data:
    #     origin= section_data
    #     dest=pth_to_awc_nds_section_data+'/'+origin.split('/')[-1]
    #     cmd= 'cp '+origin+' '+dest
    #     print('copying '+origin+' to '+dest)
    #     os.system(cmd)
        

class xc_install(_install):
    # see http://stackoverflow.com/a/18159969

    def run(self):
        '''Call superclass run method, then copy the binaries'''
        _install.run(self)
        self.execute(post_install, args=[], msg=post_install.__doc__)
        
retval= setup(name='XCModules',
      version='0.14.0',
      author='Luis C. Pérez Tato',
      packages= find_packages(include= xc_python_packages),
      url= 'https://github.com/xcfem/xc',
      # If any package contains *.pickle or *.json files, include them:
      package_data={'': ['*.pickle', '*.json']},
      cmdclass= {'install': xc_install}
     )
