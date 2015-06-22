# -*- coding: utf-8 -*-
# Test de funcionamiento del comando reg_cuad

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

ancho= 1
canto= 1
ndivIJ= 2
ndivJK= 2
nFibTeor= ndivIJ*ndivJK
areaTeor= ancho*canto
iyTeor= 1/12.0*ancho*canto**3
izTeor= 1/12.0*canto*ancho**3
y0= 0
z0= 0
F= 1000 # Magnitud de la fuerza

# Problem type
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodos)

# Definimos nodos

nodos.defaultTag= 1 #First node number.
nod1= nodos.newNodeXY(1,0)
nod2= nodos.newNodeXY(1,0)

# Materials definition
fy= 2600 # Tensión de cedencia del acero.
E= 2.1e6 # Módulo de Young del acero.
acero= typical_materials.defSteel01(mdlr,"acero",E,fy,0.001)

# Secciones
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/geomCuadFibrasTN.py")
cuadFibrasTN= mdlr.getMaterialLoader.newMaterial("fiber_section_3d","cuadFibrasTN")
fiberSectionRepr= cuadFibrasTN.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomCuadFibrasTN")
cuadFibrasTN.setupFibers()
fibras= cuadFibrasTN.getFibers()

nfibras= fibras.getNumFibers
sumAreas= fibras.getSumaAreas
Iz= fibras.getIz
Iy= fibras.getIy
zCdg= fibras.getCdgZ()
yCdg= fibras.getCdgY()
'''
             \for_each_fiber

print "fibra: ",tag, " mat. tag:", getMaterial.tag
'''



# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultMaterial= "cuadFibrasTN"
elementos.dimElem= 1
elementos.defaultTag= 1
zl= elementos.newElement("zero_length_section",xc.ID([1,2]))

# Constraints
coacciones= mdlr.getConstraintLoader
#
spc= coacciones.newSPConstraint(1,0,0.0) # Nodo 1
spc= coacciones.newSPConstraint(1,1,0.0)
spc= coacciones.newSPConstraint(1,2,0.0)
spc= coacciones.newSPConstraint(2,1,0.0)
spc= coacciones.newSPConstraint(2,2,0.0)

# Loads definition
cargas= mdlr.getLoadLoader
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

elementos= mdlr.getElementLoader

elem1= elementos.getElement(1)
elem1.getResistingForce()
scc= elem1.getSection()

nfib= scc.getFibers().getNumFibers()
avgStrain= 0.0
fibras= scc.getFibers()
for f in fibras:
# print "fibra: ",tag, " deformación: ", getMaterial.strain
   avgStrain+= f.getMaterial().getStrain()
avgStrain/= nfib

ratio1= -R/F
ratio2= (E*deltax)/F
ratio3= (avgStrain-deltax)/deltax

''' 
print "R= ",R
print "dx= ",deltax
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
   '''
  
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3)<1e-5) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
