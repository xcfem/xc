# -*- coding: utf-8 -*-
''' Test de funcionamiento de la comprobación a fisuración de una sección de hormigón armado.'''

import xc_base
import geom
import xc
# Macros
from materials.ehe import auxEHE
from misc import banco_pruebas_scc3d
from solution import predefined_solutions # Procedimiento de solución


from materials.ehe import hormigonesEHE
from materials.ehe import EHE_reinforcing_steel
from materials.ehe import fisuracionEHE
from model import fix_node_6dof
import math

ancho= 1.0 # Ancho de la sección expresado en metros.
canto= 1.0 # Canto de la sección expresado en metros.
recub= 0.03 # Recubrimiento de la sección expresado en metros.
areaFi10=0.79e-4
areaFi25=4.91e-4

NDato= 0 # Axil para comprobar fisuración.
MyDato= -67*9810*ancho # Momento para comprobar fisuración.
MzDato= 0 # Momento para comprobar fisuración.

sepBarras= 0.15
numBarras= int(math.floor(ancho/sepBarras))
offsetBarras= ((ancho-(numBarras-1)*sepBarras)/2.0)

''' 
print "sepBarras= ",sepBarras
print "numBarras= ",numBarras
print "offsetBarras= ",offsetBarras
   '''

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
tagHA25= hormigonesEHE.HA25.defDiagK(preprocessor)
tagB400S= EHE_reinforcing_steel.B400S.defDiagK(preprocessor)

geomSecHA= preprocessor.getMaterialLoader.newSectionGeometry("geomSecHA")
regiones= geomSecHA.getRegions
hormigon= regiones.newQuadRegion(hormigonesEHE.HA25.nmbDiagK)
hormigon.nDivIJ= 10
hormigon.nDivJK= 10
hormigon.pMin= geom.Pos2d(-ancho/2.0,-canto/2.0)
hormigon.pMax= geom.Pos2d(ancho/2.0,canto/2.0)
armaduras= geomSecHA.getReinfLayers
armaduraInf= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B400S.nmbDiagK)
armaduraInf.numReinfBars= numBarras
armaduraInf.barArea= areaFi25
armaduraInf.p1= geom.Pos2d(offsetBarras-ancho/2.0,recub-canto/2.0)
armaduraInf.p2= geom.Pos2d(ancho/2.0-offsetBarras,recub-canto/2.0)
armaduraSup= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B400S.nmbDiagK)
armaduraSup.numReinfBars= numBarras
armaduraSup.barArea= areaFi10
armaduraSup.p1= geom.Pos2d(offsetBarras-ancho/2.0,canto/2.0-recub)
armaduraSup.p2= geom.Pos2d(ancho/2.0-offsetBarras,canto/2.0-recub)

materiales= preprocessor.getMaterialLoader
secHA= materiales.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()

banco_pruebas_scc3d.modeloSecc3d(preprocessor, "secHA")

# Constraints
coacciones= preprocessor.getConstraintLoader
fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.Nodo6DOFMovXGirosYZLibres(coacciones,2)

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

secHAParamsFis= fisuracionEHE.ParamsFisuracionEHE()



elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
secHAParamsFis.calcApertCaracFis(scc,hormigonesEHE.HA25.tagDiagK,EHE_reinforcing_steel.B400S.tagDiagK,hormigonesEHE.HA25.fctm())


ratio1= ((secHAParamsFis.sepBarrasTracc-0.15)/0.15)
WkTeor= 0.26e-3
ratio2= ((secHAParamsFis.Wk-WkTeor)/WkTeor)


'''
secHAParamsFis.printParams()
print "ratio1= ",ratio1
print "wk= ", secHAParamsFis.Wk
print "ratio2= ",ratio2
'''


import os
fname= os.path.basename(__file__)
if (abs(ratio1)<0.05) & (abs(ratio2)<0.05):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
