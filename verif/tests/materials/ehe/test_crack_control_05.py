# -*- coding: utf-8 -*-
''' Reinforced concrete section. Crack control verification test.'''

import xc_base
import geom
import xc

from misc import scc3d_testing_bench
from solution import predefined_solutions # Solution procedure


from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from model import predefined_spaces
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

width= 1.0 # Section width expressed in meters.
depth= 1.0 # Section depth expressed in meters.
cover= 0.03 # Concrete cover expressed in meters.
areaFi10=0.79e-4
areaFi25=4.91e-4

NDato= 0 # Axial force for crack control checking.
MyDato= -67*9810*width # Bending moment force for crack control checking.
MzDato= 0 # Bending moment force for crack control checking.

rebarsSpacing= 0.15
numBarras= int(math.floor(width/rebarsSpacing))
offsetBarras= ((width-(numBarras-1)*rebarsSpacing)/2.0)

''' 
print "rebarsSpacing= ",rebarsSpacing
print "numBarras= ",numBarras
print "offsetBarras= ",offsetBarras
   '''

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
concrMatTag25= EHE_materials.HA25.defDiagK(preprocessor)
tagB400S= EHE_materials.B400S.defDiagK(preprocessor)

geomSecHA= preprocessor.getMaterialHandler.newSectionGeometry("geomSecHA")
regions= geomSecHA.getRegions
concrete= regions.newQuadRegion(EHE_materials.HA25.nmbDiagK)
concrete.nDivIJ= 10
concrete.nDivJK= 10
concrete.pMin= geom.Pos2d(-width/2.0,-depth/2.0)
concrete.pMax= geom.Pos2d(width/2.0,depth/2.0)
reinforcement= geomSecHA.getReinfLayers
reinforcementInf= reinforcement.newStraightReinfLayer(EHE_materials.B400S.nmbDiagK)
reinforcementInf.numReinfBars= numBarras
reinforcementInf.barArea= areaFi25
reinforcementInf.p1= geom.Pos2d(offsetBarras-width/2.0,cover-depth/2.0)
reinforcementInf.p2= geom.Pos2d(width/2.0-offsetBarras,cover-depth/2.0)
reinforcementSup= reinforcement.newStraightReinfLayer(EHE_materials.B400S.nmbDiagK)
reinforcementSup.numReinfBars= numBarras
reinforcementSup.barArea= areaFi10
reinforcementSup.p1= geom.Pos2d(offsetBarras-width/2.0,depth/2.0-cover)
reinforcementSup.p2= geom.Pos2d(width/2.0-offsetBarras,depth/2.0-cover)

materiales= preprocessor.getMaterialHandler
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()

scc3d_testing_bench.sectionModel(preprocessor, "secHA")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_0FF(2)

# Loads definition
cargas= preprocessor.getLoadHandler

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,0,0,MyDato,MzDato]))

#We add the load case to domain.
casos.addToDomain("0")



# Solution procedure
analisis= predefined_solutions.simple_newton_raphson(feProblem)
analOk= analisis.analyze(10)

secHAParamsFis= EHE_limit_state_checking.CrackControl('SLS_crack')



elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
scc= ele1.getSection()
secHAParamsFis.computeWk(scc,EHE_materials.HA25.matTagK,EHE_materials.B400S.matTagK,EHE_materials.HA25.fctm())


ratio1= ((secHAParamsFis.tensionedRebars.spacing-0.15)/0.15)
WkTeor= 0.26e-3
ratio2= ((secHAParamsFis.Wk-WkTeor)/WkTeor)


'''
secHAParamsFis.printParams()
print "ratio1= ",ratio1
print "wk= ", secHAParamsFis.Wk
print "ratio2= ",ratio2
'''


import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.05) & (abs(ratio2)<0.05):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
