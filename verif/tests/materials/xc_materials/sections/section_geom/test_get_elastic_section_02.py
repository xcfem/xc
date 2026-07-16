# -*- coding: utf-8 -*-
''' Test getElasticSection3d method. Section with reinforcement.
    
Check that the returned values for mass properties by the ElasticSection3d
object are correct.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"


import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

Es= 2.1e11
n= 15.0
Ec= Es/n
nu= 0.2 # Poisson's ratio
G= Ec/(2*(1+nu)) # Shear modulus.
barDiameter= 16e-3
barArea= math.pi*(barDiameter/2.0)**2

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define materials.
steel= typical_materials.defElasticMaterial(preprocessor, "steel",Es)

sectionGeometryTest= preprocessor.getMaterialHandler.newSectionGeometry("sectionGeometryTest")
reinforcement= sectionGeometryTest.getReinfLayers
reinforcementA= reinforcement.newStraightReinfLayer(steel.name)
reinforcementA.numReinfBars= 2
reinforcementA.barArea= barArea
reinforcementA.setP1P2(geom.Pos2d(0.05,0.95), geom.Pos2d(0.05,0.05)) # bottom layer.
reinforcementB= reinforcement.newStraightReinfLayer(steel.name)
reinforcementB.numReinfBars= 2
reinforcementB.barArea= barArea
reinforcementB.setP1P2(geom.Pos2d(0.95,0.95), geom.Pos2d(0.95,0.05)) # bottom layer.
nRebars= reinforcement.getNumReinfBars

# Define 3D elastic section.
Iw= 0.0 # Warping constant (assumed zero).
J= 0.0 # Torsional inertia (neglected).
es3d= sectionGeometryTest.getElasticSection3d('es3d', Ec, Iw, G, J)
es3d_prop= es3d.sectionProperties

area= es3d_prop.A # Area.
Iy= es3d_prop.Iy # Second moment inertia around y axis. 
Iz= es3d_prop.Iz # Second moment inertia around z axis. 
Iyz= es3d_prop.Iyz # Product of inertia.
JJ= es3d_prop.J # Torsional inertia.

areaTeor= (n*nRebars*barArea)
iTeor= (n*nRebars*math.pi/4*(barDiameter/2.0)**4+(0.45)**2*areaTeor)

ratio1= ((area-areaTeor)/areaTeor)
ratio2= ((Iy-iTeor)/iTeor)
ratio3= ((Iz-iTeor)/iTeor)
ratio4= abs(Iyz)
ratio5= abs(JJ)
ratio6= ((nRebars-4)/4)

'''
print("area= ",area)
print("areaTeor= ",areaTeor)
print("Iy= ",Iy)
print("Iz= ",Iz)
print("iTeor= ",iTeor)
print("Iyz= ",Iyz)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
print("ratio6= ",ratio6)
''' 

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-7) & (abs(ratio5)<1e-7) & (abs(ratio6)<1e-17) :
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
