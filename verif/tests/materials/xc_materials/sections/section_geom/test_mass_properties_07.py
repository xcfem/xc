# -*- coding: utf-8 -*-
''' Test getElasticSection3d method. Irregular section.
    
Check that the returned values for mass properties by the ElasticSection3d
object are correct.
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
# Reference values.
E0= concrete.getEcm() # Reference elastic modulus.
nu= 0.2 # Poisson's ratio
G= E0/(2*(1+nu)) # Shear modulus.
psA= pierSectionGeometry.getAreaHomogenizedSection(E0)
psEA= E0*psA
Iw= 0.0 # Warping constant (assumed zero).
alpha_y= 5/6.0 # Factor for shear response along local y axis.
alpha_z= 0.5*alpha_y # Factor for shear response along local z axis
                     # (0.5 only to make it different from the previous one,
                     # so no physical meaning)).
Iy= pierSectionGeometry.getIyHomogenizedSection(E0)
Iz= pierSectionGeometry.getIzHomogenizedSection(E0)
J= 3.141592 # Just a number (no physical meaning).
majorI= max(Iy, Iz)
minorI= min(Iy, Iz)
linearRho= pierSectionGeometry.getLinearRho()
regions= pierSectionGeometry.getRegions
concreteLinearRho= regions.getLinearRho()
concreteArea= regions.getAreaGrossSection()
reinfLayers= pierSectionGeometry.getReinfLayers
reinfLinearRho= reinfLayers.getLinearRho()
reinfArea= reinfLayers.getAreaGrossSection()

ratio1= abs(concreteLinearRho/concreteArea-2500)/2500.0
ratio2= abs(reinfLinearRho/reinfArea-7850)/7850.0

'''
print('concrete rho: ', concrete.density())
print('steel rho: ', steel.rho)
print('concreteLinearRho= ', concreteLinearRho)
print('ratio1= ', ratio1)
print('reinfLinearRho= ', reinfLinearRho, reinfLinearRho/reinfArea)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-12 and abs(ratio2)<1e-12:
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
