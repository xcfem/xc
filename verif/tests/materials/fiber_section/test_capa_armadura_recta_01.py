# -*- coding: utf-8 -*-
# Test de funcionamiento del comando capa_armadura_recta
import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

ancho= 1
canto= 2
y0= 0
z0= 0
nBarras= 4.0
As= 1.0/nBarras
areaTeor= nBarras*As
iyTeor= nBarras*As*(ancho/2.0)**2
izTeor= nBarras*As*(canto/2.0)**2
F= 1000.0 # Magnitud de la fuerza

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodos= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodos)

nodos.defaultTag= 1 #First node number.
nod= nodos.newNodeXY(1,0)
nod= nodos.newNodeXY(1,0)

# Materials definition
fy= 2600 # Tensión de cedencia del acero.
E= 2.1e6 # Módulo de Young del acero.
acero= typical_materials.defSteel01(preprocessor, "acero",E,fy,0.001)

geomCuadFibras= preprocessor.getMaterialLoader.newSectionGeometry("geomCuadFibras")
y1= ancho/2.0
z1= canto/2.0

armaduras= geomCuadFibras.getReinfLayers
armaduraA= armaduras.newStraightReinfLayer("acero")
armaduraA.numReinfBars= 2
armaduraA.barArea= As
armaduraA.p1= geom.Pos2d(y0-canto/2.0,z0-ancho/2.0)
armaduraA.p2= geom.Pos2d(y0+canto/2.0,z0-ancho/2.0)

armaduraB= armaduras.newStraightReinfLayer("acero")
armaduraB.numReinfBars= 2
armaduraB.barArea= As
armaduraB.p1= geom.Pos2d(y0-canto/2.0,z0+ancho/2.0)
armaduraB.p2= geom.Pos2d(y0+canto/2.0,z0+ancho/2.0)

materiales= preprocessor.getMaterialLoader
# Secciones
cuadFibras= materiales.newMaterial("fiber_section_3d","cuadFibras")
fiberSectionRepr= cuadFibras.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomCuadFibras")
cuadFibras.setupFibers()
fibras= cuadFibras.getFibers()

# nfibras= fibras.getNumFibers()
# sumAreas= fibras.getSumaAreas(1)
# Iz= fibras.getIz(1.0,y0)
# Iy= fibras.getIy(1.0,z0)
# zCdg= fibras.getCdgZ()
# yCdg= fibras.getCdgY()

'''
for_each_fiber
  print "fibra: ",tag, " mat. tag:", getMaterial.tag
'''



# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "cuadFibras"
elementos.dimElem= 1
elementos.defaultTag= 1
elem= elementos.newElement("zero_length_section",xc.ID([1,2]))

# Constraints
coacciones= preprocessor.getConstraintLoader
#
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(1,2,0.0)
spc= coacciones.newSPConstraint(2,1,0.0)
spc= coacciones.newSPConstraint(2,2,0.0)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0.0,0.0]))

#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)



nodos.calculateNodalReactions(True)
nod2= nodos.getNode(2)
deltax= nod2.getDisp[0] 
nod1= nodos.getNode(1)
R= nod1.getReaction[0] 


elem1= elementos.getElement(1)
elem1.getResistingForce()
scc= elem1.getSection()
nfib= scc.getFibers().getNumFibers()
avgStrain= 0.0
fibras= scc.getFibers()
sumAreas= fibras.getSumaAreas(1)
Iz= fibras.getIz(1.0,y0)
Iy= fibras.getIy(1.0,z0)
zCdg= fibras.getCdgZ()
yCdg= fibras.getCdgY()

avgStrain= 0.0
for f in fibras:
# print "fibra: ",tag, " deformación: ", getMaterial.strain
   avgStrain+= f.getMaterial().getStrain()
avgStrain/= nfib




ratio1= (nfib-nBarras)/nBarras
ratio2= (sumAreas-areaTeor)/areaTeor
ratio3= yCdg-y0
ratio4= zCdg-z0
ratio5= (Iz-izTeor)/izTeor
ratio6= (Iy-iyTeor)/iyTeor
ratio7= (-R-F)/F
ratio8= ((E*deltax)-F)/F
ratio9= (avgStrain-deltax)/deltax

''' 
print "R= ",R
print "dx= ",deltax
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
print "Iz= ",Iz
print "izTeor= ",izTeor
print "ratio5= ",(ratio5)
print "Iy= ",Iy
print "iyTeor= ",iyTeor
print "ratio6= ",(ratio6)
print "ratio7= ",(ratio7)
print "ratio8= ",(ratio8)
print "ratio9= ",(ratio9)
   '''
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5) & (abs(ratio5)<1e-5) & (abs(ratio6)<1e-5) & (abs(ratio7)<1e-5) & (abs(ratio8)<1e-5) & (abs(ratio9)<1e-5) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
