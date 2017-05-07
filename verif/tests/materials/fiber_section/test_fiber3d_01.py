# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

# Testing fiber section model of Steel01 uniaxial material.

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

width= 1
depth= 1
nDivIJ= 2
nDivJK= 2
nFibTeor= nDivIJ*nDivJK
areaTeor= width*depth
iyTeor= 1/12.0*width*depth**3
izTeor= 1/12.0*depth*width**3
y0= 0
z0= 0
F= 1000 # Force magnitude

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Node definition

nodes.defaultTag= 1 #First node number.
nod1= nodes.newNodeXY(1,0)
nod2= nodes.newNodeXY(1,0)

# Materials definition: uniaxial bilinear steel
fy= 2600 # Yield stress.
E= 2.1e6 # Young modulus.
b=0.001    # strain-hardening ratio: ratio between post-yield tangent and initial elastic tangent
acero= typical_materials.defSteel01(preprocessor=preprocessor,name="acero",E=E,fy=fy,b=b)

# Secciones
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/geomCuadFibrasTN.py")
cuadFibrasTN= preprocessor.getMaterialLoader.newMaterial("fiber_section_3d","cuadFibrasTN")
fiberSectionRepr= cuadFibrasTN.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomCuadFibrasTN")
cuadFibrasTN.setupFibers()
fibras= cuadFibrasTN.getFibers()

nfibras= fibras.getNumFibers
Iz= fibras.getIz
Iy= fibras.getIy
zCdg= fibras.getCdgZ()
yCdg= fibras.getCdgY()
'''
             \for_each_fiber

print "fibra: ",tag, " mat. tag:", getMaterial.tag
'''



# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "cuadFibrasTN"
elementos.dimElem= 1
elementos.defaultTag= 1
zl= elementos.newElement("zero_length_section",xc.ID([1,2]))

# Constraints
constraints= preprocessor.getConstraintLoader
#
spc= constraints.newSPConstraint(1,0,0.0) # Nodo 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(1,2,0.0)
spc= constraints.newSPConstraint(2,1,0.0)
spc= constraints.newSPConstraint(2,2,0.0)

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



nodes.calculateNodalReactions(True)
nod2= nodes.getNode(2)
deltax= nod2.getDisp[0] 
nod1= nodes.getNode(1)
R= nod1.getReaction[0] 

elementos= preprocessor.getElementLoader

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
  
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3)<1e-5) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
