# -*- coding: utf-8 -*-
''' Test de funcionamiento de una sección de hormigón armado empleando
   el material fiber_section_GJ. '''

# Macros
import math
import xc_base
import geom
import xc
from misc import banco_pruebas_scc3d

areaFi16= 2.01e-4 # Área de las barras expresado en metros cuadrados.
brazo= 0.5 # Brazo de las barras respecto al eje de la sección.


from materials.ehe import EHE_reinforcing_steel
from model import fix_node_6dof
from solution import predefined_solutions

# Coeficientes de seguridad.
gammas= 1.15 # Coeficiente de minoración de la resistencia del acero.



MzDato= 15e3
NDato= 0.0

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
# Materials definition
tag= EHE_reinforcing_steel.B500S.defDiagD(preprocessor)
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/geomSeccBarras.py")
secBarras= preprocessor.getMaterialLoader.newMaterial("fiber_section_GJ","secBarras")
fiberSectionRepr= secBarras.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecBarras")
secBarras.setupFibers()

banco_pruebas_scc3d.modeloSecc3d(preprocessor, "secBarras")

# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.fixNode6DOF(coacciones,1)
fix_node_6dof.Nodo6DOFMovXGiroZLibres(coacciones,2)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,0,MzDato]))

#We add the load case to domain.
casos.addToDomain("0")


# Procedimiento de solución
analisis= predefined_solutions.simple_newton_raphson(prueba)
analOk= analisis.analyze(1)


nodes= preprocessor.getNodeLoader
nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader

RN= nodes.getNode(1).getReaction[0] 
RM= nodes.getNode(1).getReaction[5] 
RN2= nodes.getNode(2).getReaction[0] 

elementos= preprocessor.getElementLoader
ele1= elementos.getElement(1)
scc= ele1.getSection()
esfN= scc.getFibers().getResultant()
esfMy= scc.getFibers().getMy(0.0)
esfMz= scc.getFibers().getMz(0.0)
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")

ratio1= (RM+MzDato)/MzDato
ratio2= (esfMz-MzDato)/MzDato
ratio3= (esfN-NDato)

''' 
print "RN= ",(RN/1e3)," kN \n"
print "RN2= ",(RN2/1e3)," kN \n"
print "N= ",(esfN/1e3)," kN \n"
print "My= ",(esfMy/1e3)," kN m\n"
print "Mz= ",(esfMz/1e3)," kN m\n"
print "defMz= ",(defMz)
print "defN= ",(defN)
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
  '''

fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(RN)<1e-10) & (abs(RN2)<1e-10) & (abs(esfMy)<1e-10) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
