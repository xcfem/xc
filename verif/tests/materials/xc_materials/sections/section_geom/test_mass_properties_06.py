# -*- coding: utf-8 -*-
''' Build an irregular pier section and check the values of its mass properties.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import sys
import math
import geom
import xc
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import geom_fiber_sect as gfs
from misc_utils import log_messages as lmsg

concrete= EC2_materials.C40
steel= EC2_materials.S500B

# Section geometry data.
thickness= .35

p9= geom.Pos2d(215e-2, 140e-2)
p9i= p9+geom.Vector2d(0.0, -thickness)
p10= geom.Pos2d(-p9.x, p9.y)
p10i= p10+geom.Vector2d(0.0, -thickness)
p8= p9+geom.Vector2d(.1, .1)
p7= geom.Pos2d(300e-2, 75e-2)
p6= geom.Pos2d(p7.x, -p7.y)
p5i= geom.Pos2d(265e-2, 60.5e-2)
p4i= geom.Pos2d(p5i.x, -p5i.y)
p6i= geom.Pos2d(220.5e-2, 105e-2)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

tagConcreteDiag= concrete.defDiagD(preprocessor)

# Import section definition function from file.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
sys.path.append(pth+"/../../../../aux/sections/")
import pier_section_geometry as sg
pierSectionGeometry= sg.define_pier_section_geometry(preprocessor= preprocessor,
                                                     name= "P32_01",
                                                     concrete= concrete,
                                                     steel= steel,
                                                     p4i= p4i,
                                                     p5i= p5i, p6= p6,
                                                     p6i= p6i, p7= p7,
                                                     p8= p8, p9= p9, p9i= p9i,
                                                     p10= p10, p10i= p10i,
                                                     r1Reinf= (30, 25e-3),
                                                     r2Reinf= (8, 25e-3),
                                                     r3Reinf= (9, 25e-3),
                                                     r9Reinf= (31, 16e-3),
                                                     r10Reinf= (4, 16e-3),
                                                     r11Reinf= (10, 16e-3))

# Get some mass properties from the section geometry.
## Concrete area.
regions= pierSectionGeometry.getRegions
concreteArea= regions.getAreaGrossSection()
refConcreteArea= 5.261049999999997
ratio1= abs(concreteArea-refConcreteArea)/refConcreteArea

## Steel area.
num25Bars= 2*30+2*9+4*8
area25Bars= num25Bars*math.pi*(25e-3/2)**2
num16Bars= 2*31+2*10+4*4
area16Bars= num16Bars*math.pi*(16e-3/2)**2
refSteelArea= area16Bars+area25Bars
reinforcement= pierSectionGeometry.getReinfLayers
steelArea= reinforcement.getAreaGrossSection()
ratio2= abs(steelArea-refSteelArea)/refSteelArea
## Center of mass.
G= pierSectionGeometry.getCenterOfMassHomogenizedSection(concrete.getEcm())
ratio3= math.sqrt(G[0]**2+G[1]**2)
## Inertia.
refIy= 6.091166 # value computed with Fagus-9 
Iy= pierSectionGeometry.getIyGrossSection()
refIz= 20.465745 # value computed with Fagus-9 
Iz= pierSectionGeometry.getIzGrossSection()
Pyz= pierSectionGeometry.getPyzGrossSection()
ratio4= math.sqrt((Iy-refIy)**2+(Iz-refIz)**2+Pyz**2)

'''
print('Concrete area: ', concreteArea, 'm2')
print('Reference concrete area: ', refConcreteArea, 'm2')
print('ratio1: ', ratio1)
print('\nSteel area: ', steelArea*1e4, 'cm2')
print('Referenca steel area: ', refSteelArea*1e4, 'cm2')
print('ratio2: ', ratio2)
print('\nG= '+str(G))
print('ratio3: ', ratio3)
print('Iy= ', Iy, 'm4')
print('Iz= ', Iz, 'm4')
print('Pyz= ', Pyz, 'm4')
print('ratio4: ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-12 and abs(ratio4)<1e-3:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# import matplotlib.pyplot as plt
# from materials.sections.fiber_section import plot_fiber_section as pfs
# fig = plt.figure()
# ax= fig.add_subplot(111)
# pfs.mplot_section_geometry(ax, sectionGeometry= pierSectionGeometry, rotate= False)
# plt.show()
