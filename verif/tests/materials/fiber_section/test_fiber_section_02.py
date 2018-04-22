# -*- coding: utf-8 -*-
'''Verification test of a rectangular fiber section with an
 elasto plastic material model.'''
#    written from "Nociones de cálculo plástico." Carlos Benito Hernández.
#    page 26 and following.
#  url={https://books.google.es/books?id=v7bbMwEACAAJ},

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Rectangular cross-section definition
from materials.sections import section_properties
from misc import scc3d_testing_bench
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from misc import scc3d_testing_bench

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
feProblem.logFileName= "/tmp/borrar.log" # Ignore warning messages

# Rectangular cross-section definition
b= 10 # Cross section width  [cm]
h= 20 # Cross section depth [cm]
scc10x20= section_properties.RectangularSection('scc10x20',b,h)
scc10x20.nDivIJ= 32 # number of cells in IJ direction  
scc10x20.nDivJK= 32 # number of cells in JK direction

import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/macros_test_fiber_section.py")


fy= 2600 # Yield stress of the material expressed in kp/cm2.
E= 2.1e6 # Young modulus of the material en kp/cm2.

# Materials definition
epp= typical_materials.defElasticPPMaterial(preprocessor, "epp",E,fy,-fy)

# Section geometry
# creation
geomRectang= preprocessor.getMaterialHandler.newSectionGeometry("geomRectang")
#generation of a quadrilateral region of the scc10x20 sizes and number of
#divisions made of material nmbMat
reg= scc10x20.getRegion(gm=geomRectang,nmbMat="epp")
rectang= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","rectang")
fiberSectionRepr= rectang.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomRectang")
rectang.setupFibers()
extractFiberSectionProperties(rectang,scc10x20)

scc3d_testing_bench.sectionModel(preprocessor, "rectang")
# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_00F(2)

# Loads definition
cargas= preprocessor.getLoadHandler

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
loadMz= 0.999*scc10x20.getPlasticMomentZ(fy)
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,0,loadMz]))

#We add the load case to domain.
casos.addToDomain("0")


# Solve
analisis= predefined_solutions.simple_newton_raphson(feProblem)
analOk= analisis.analyze(1)


nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)

RM= nodes.getNode(1).getReaction[5] 

elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
scc= ele1.getSection()
esfMz= scc.getFibers().getMz(0.0)



yCdgTeor= 0.0
zCdgTeor= 0.0
ratio1= (sumAreas-scc10x20.A())/scc10x20.A()
ratio2= yCdg-yCdgTeor
ratio3= zCdg-zCdgTeor
ratio4= (I1-scc10x20.Iz())/scc10x20.Iz()
ratio5= (I2-scc10x20.Iy())/scc10x20.Iy()
ratio6= (i1-scc10x20.iz())/scc10x20.iz()
ratio7= (i2-scc10x20.iy())/scc10x20.iy()
ratio8= (Me1-scc10x20.getYieldMomentZ(fy))/scc10x20.getYieldMomentZ(fy)
ratio9= (Me2-scc10x20.getYieldMomentY(fy))/scc10x20.getYieldMomentY(fy)
ratio10= (SzPosG-scc10x20.getPlasticSectionModulusZ())/scc10x20.getPlasticSectionModulusZ()
ratio11= (SyPosG-scc10x20.getPlasticSectionModulusY())/scc10x20.getPlasticSectionModulusY()
ratio12= (RM+loadMz)/loadMz
ratio13= (esfMz-loadMz)/loadMz

'''
printRatios(scc10x20)
print "analOk= ",analOk
print "numFibers= ", nFibers
print "numFibersTeor= ", scc10x20.nDivIJ*scc10x20.nDivJK
print "RM= ",(RM)
print "esfMz= ",(esfMz)
print "loadMz= ",(loadMz)
'''

from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-3) & (abs(ratio5)<1e-2)  & (abs(ratio6)<1e-3) & (abs(ratio7)<1e-2) & (abs(ratio8)<1e-3) & (abs(ratio9)<1e-3) & (abs(ratio10)<1e-5) & (abs(ratio11)<1e-5)  & (abs(ratio12)<1e-5) & (abs(ratio13)<1e-5) & (analOk==0):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
