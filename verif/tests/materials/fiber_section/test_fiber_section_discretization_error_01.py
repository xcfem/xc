# -*- coding: utf-8 -*-
# Fiber section model: testing error due to section discretization.

from __future__ import division
import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

width= 1
depth= 2
areaTeor= width*depth
iyTeor= 1/12.0*width*depth**3
izTeor= 1/12.0*depth*width**3
y0= 0
z0= 0
F= 1000 # Force magnitude

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Materials definition
E= 2.1e6 # Elastic modulus
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

# Section geometry
# setting up
geomSCC= preprocessor.getMaterialLoader.newSectionGeometry("geomSCC")
y1= width/2.0
z1= depth/2.0
#filling with regions
regiones= geomSCC.getRegions
#generation of a quadrilateral region of the specified sizes and number of
#divisions for the cells (fibers) generation
rg= regiones.newQuadRegion("elast")
rg.nDivIJ= 1
rg.nDivJK= 23
nFibTeor= rg.nDivIJ*rg.nDivJK
rg.pMin= geom.Pos2d(y0-y1,z0-z1)
rg.pMax= geom.Pos2d(y0+y1,z0+z1)


import os
fiberModel= preprocessor.getMaterialLoader.newMaterial("fiber_section_3d","fiberModel")
fiberSectionRepr= fiberModel.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSCC")
fiberModel.setupFibers()
fibras= fiberModel.getFibers()

zinf= z0-z1
zsup= z0+z1
epsilon= 3.5e-3
p1= geom.Pos3d(-epsilon,width/2.0,zsup)
p2= geom.Pos3d(-epsilon,-width/2.0,zsup)
p3= geom.Pos3d(epsilon,width/2.0,zinf)
fiberModel.setTrialDeformationPlane(xc.DeformationPlane(p1,p2,p3))

N= fiberModel.getStressResultantComponent("N")
Mz= fiberModel.getStressResultantComponent("Mz")
My= fiberModel.getStressResultantComponent("My")

nfiber= fibras.getNumFibers()
hFiber= depth/rg.nDivJK
IyFiber= 1/12.0*width*hFiber**3
Iy= 1/12.0*width*depth**3

MyTeor= -2*E*Iy*epsilon/depth
MFiber= -2*E*IyFiber*epsilon/depth
expectedError= rg.nDivJK*MFiber
MyCorr= My+expectedError
error= (1/rg.nDivJK**2)

ratio1= (MyCorr-MyTeor)/MyTeor
ratio2= (MyCorr-My)/MyCorr-error

''' 
print "rg.nDivJK= ", rg.nDivJK
print " My= ", My
print " MyTeor= ", MyTeor
print " MyTeor-My= ", MyTeor-My
print " expectedError= ", expectedError
print " MyCorr= ", MyCorr
print "ratio1= ", ratio1
print "ratio2= ", ratio2
  '''
  
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
