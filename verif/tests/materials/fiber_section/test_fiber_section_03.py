# -*- coding: utf-8 -*-
# Verification test of una sección metálica rectangular de fibras de material elastoplástico.
#    elaborado a partir de «Nociones de cálculo plástico». C. Benito Hernández.
#    página 26 y siguientes.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Rectangular cross-section definition
from materials import paramRectangularSection
from misc import banco_pruebas_scc3d
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

# Rectangular cross-section definition
b= 10 # Cross section width  [cm]
h= 20 # Cross section depth [cm]
scc10x20= paramRectangularSection.RectangularSection('scc10x20',b,h)
scc10x20.nDivIJ= 32 # number of cells in IJ direction  
scc10x20.nDivJK= 32 # number of cells in JK direction

import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/macros_test_fiber_section.py")

fy= 2600 # Tensión de cedencia of the material expressed in kp/cm2.
E= 2.1e6 # Young modulus of the material en kp/cm2.

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Ignore warning messages
preprocessor=  prueba.getPreprocessor
# Materials definition
epp= typical_materials.defElasticPPMaterial(preprocessor, "epp",E,fy,-fy)
# Section geometry
#creation
geomRectang= preprocessor.getMaterialLoader.newSectionGeometry("geomRectang")
#generation of a quadrilateral region of the scc10x20 sizes and number of
#divisions made of material nmbMat
rreg= scc10x20.getRegion(geomRectang,"epp")
rectang= preprocessor.getMaterialLoader.newMaterial("fiber_section_3d","rectang")
fiberSectionRepr= rectang.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomRectang")
rectang.setupFibers()
extraeParamSccFibras(rectang,scc10x20)

banco_pruebas_scc3d.sectionModel(preprocessor, "rectang")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNode000_0F0(2)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
loadMy= 0.999*scc10x20.getPlasticMomentY(fy)
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,loadMy,0]))

#We add the load case to domain.
casos.addToDomain("0")


# Obtención de la solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(1)


nodes= preprocessor.getNodeLoader
nodes.calculateNodalReactions(True)

RM= nodes.getNode(1).getReaction[4] 

elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
esfMy= scc.getFibers().getMy(0.0)

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
ratio12= (RM+loadMy)/loadMy
ratio13= (esfMy-loadMy)/loadMy

'''
printRatios(scc10x20)
    print "RM= ",(RM)
    print "esfMy= ",(esfMy)
    print "loadMy= ",(loadMy)
'''

from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-3) & (abs(ratio5)<1e-2) & (abs(ratio6)<1e-3) & (abs(ratio7)<1e-2) & (abs(ratio8)<1e-3) & (abs(ratio9)<1e-3) & (abs(ratio10)<1e-5) & (abs(ratio11)<1e-5) & (abs(ratio12)<1e-5) & (abs(ratio13)<1e-5) & (analOk == 0.0) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
