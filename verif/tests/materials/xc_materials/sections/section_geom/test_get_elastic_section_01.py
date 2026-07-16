# -*- coding: utf-8 -*-
''' Home made test.'''
from __future__ import print_function

import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Es= 2.1e11 # Steel elastic modulus.
n= 15
Ec= Es/n # Concrete elastic modulus.
nu= 0.2 # Poisson's ratio
G= Ec/(2*(1+nu)) # Shear modulus.
sideLength= 1.0

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define materials.
concrete= typical_materials.defElasticMaterial(preprocessor, 'concrete',Ec)

# Define section geometry.
sectionGeometryTest= preprocessor.getMaterialHandler.newSectionGeometry("sectionGeometryTest")
regions= sectionGeometryTest.getRegions
concrete= regions.newQuadRegion(concrete.name)
concrete.pMin= geom.Pos2d(0.0,0.0)
concrete.pMax= geom.Pos2d(1.0,1.0)

# Define 3D elastic section.
Iw= 0.0 # Warping constant (assumed zero).
J= 0.141*sideLength**4 # Torsional inertia (approximated value).
es3d= sectionGeometryTest.getElasticSection3d('es3d', Ec, Iw, G, J)
es3d_prop= es3d.sectionProperties

area= es3d_prop.A # Area.
Iy= es3d_prop.Iy # Second moment inertia around y axis. 
Iz= es3d_prop.Iz # Second moment inertia around z axis. 
Iyz= es3d_prop.Iyz # Product of inertia.
JJ= es3d_prop.J # Torsional inertia.

areaTeor= sideLength*sideLength
iTeor= (sideLength)**4/12.0

ratio1= ((area-areaTeor)/areaTeor)
ratio2= ((Iy-iTeor)/iTeor)
ratio3= ((Iz-iTeor)/iTeor)
ratio4= abs(Iyz)
ratio5= ((JJ-J)/J)

''' 
print("area= ",area)
print("G= ",G)
print("Iy= ",Iy)
print("Iz= ",Iz)
print("iTeor= ",iTeor)
print("Pyz= ",Pyz)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15)& (abs(ratio5)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# import matplotlib.pyplot as plt
# from materials.sections.fiber_section import plot_fiber_section as pfs
# fig = plt.figure()
# ax= fig.add_subplot(111)
# pfs.mplot_section_geometry(ax, sectionGeometry= sectionGeometryTest, rotate= True)
# plt.show()
