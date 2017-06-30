# -*- coding: utf-8 -*-
''' Verification test of la comprobación a fisuración de una sección de hormigón armado.
   Los resultados se comparan con los obtenidos del problema 9 del
   libro «El hormigón armado en problemas. Según la norma EHE». '''

import xc_base
import geom
import xc

from misc import banco_pruebas_scc3d
from solution import predefined_solutions # Procedimiento de solución


from materials.ehe import EHE_materials
from materials.ehe import fisuracionEHE
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

width= 1.70 # Section width expressed in meters.
depth= 1.10 # Section depth expressed in meters.
cover= 0.05 # Concrete cover expressed in meters.
areaFi20=3.14e-4
areaFi25=4.91e-4
areaFi32=8.04e-4

NDato= 0 # Axil para comprobar fisuración.
MyDato= -195.3*9810 # Momento para comprobar fisuración.
MzDato= 0 # Momento para comprobar fisuración.

numBarras= 16
rebarsSpacing= ((width-2*cover)/(numBarras-1))
offsetBarras= ((width-(numBarras-1)*rebarsSpacing)/2)

''' 
print "rebarsSpacing= ",rebarsSpacing
print "numBarras= ",numBarras
print "offsetBarras= ",offsetBarras
   '''

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
concrMatTag25= EHE_materials.HA25.defDiagK(preprocessor)
tagB400S= EHE_materials.B400S.defDiagK(preprocessor)

geomSecHA= preprocessor.getMaterialLoader.newSectionGeometry("geomSecHA")
regiones= geomSecHA.getRegions
concrete= regiones.newQuadRegion(EHE_materials.HA25.nmbDiagK)
concrete.nDivIJ= 10
concrete.nDivJK= 10
concrete.pMin= geom.Pos2d(-width/2.0,-depth/2.0)
concrete.pMax= geom.Pos2d(width/2.0,depth/2.0)
reinforcement= geomSecHA.getReinfLayers
reinforcementA= reinforcement.newStraightReinfLayer(EHE_materials.B400S.nmbDiagK)
reinforcementA.numReinfBars= numBarras/2
reinforcementA.barArea= areaFi20
reinforcementA.p1= geom.Pos2d(cover-width/2.0,cover-depth/2.0)
reinforcementA.p2= geom.Pos2d(width/2.0-cover-rebarsSpacing,cover-depth/2.0)
reinforcementB= reinforcement.newStraightReinfLayer(EHE_materials.B400S.nmbDiagK)
reinforcementB.numReinfBars= numBarras/2
reinforcementB.barArea= areaFi32
reinforcementB.p1= geom.Pos2d(cover+rebarsSpacing-width/2.0,cover-depth/2.0)
reinforcementB.p2= geom.Pos2d(width/2.0-cover,cover-depth/2.0)
reinforcementC= reinforcement.newStraightReinfLayer(EHE_materials.B400S.nmbDiagK)
reinforcementC.numReinfBars= 13
reinforcementC.barArea= areaFi25
reinforcementC.p1= geom.Pos2d(cover-width/2.0,depth/2.0-cover) # Armadura superior.
reinforcementC.p2= geom.Pos2d(width/2.0-cover,depth/2.0-cover)

materiales= preprocessor.getMaterialLoader
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()

banco_pruebas_scc3d.sectionModel(preprocessor, "secHA")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_0FF(2)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,0,0,MyDato,MzDato]))

#We add the load case to domain.
casos.addToDomain("0")



# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(10)


secHAParamsFis= fisuracionEHE.CrackControl('SLS_crack')



elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
secHAParamsFis.calcApertCaracFis(scc,EHE_materials.HA25.matTagK,EHE_materials.B400S.matTagK,EHE_materials.HA25.fctm())

ratio1= ((secHAParamsFis.rebarsSpacingTracc-0.105)/0.105)
ratio2= ((secHAParamsFis.Wk-0.3e-3)/0.3e-3)


'''
secHAParamsFis.printParams()
print "ratio1= ",ratio1
print "ratio2= ",ratio2
'''


import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<0.05) & (abs(ratio2)<0.1):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
