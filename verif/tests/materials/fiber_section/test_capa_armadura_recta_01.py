# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__cppyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

# Test for checking the straight reinforcement layer
import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

width= 1
depth= 2
y0= 0
z0= 0
nRebars= 4.0
As= 1.0/nRebars
areaTeor= nRebars*As
iyTeor= nRebars*As*(width/2.0)**2
izTeor= nRebars*As*(depth/2.0)**2
F= 1000.0 # Force magnitude

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
predefined_spaces.gdls_resist_materiales2D(nodes)

nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(1,0)
nod= nodes.newNodeXY(1,0)

# Materials definition: uniaxial bilinear steel
fy= 2600   # yield strength
E= 2.1e6   # initial elastic tangent
b=0.001    # strain-hardening ratio: ratio between post-yield tangent and initial elastic tangent
acero= typical_materials.defSteel01(preprocessor=preprocessor,name="acero",E=E,fy=fy,b=b)


geomCuadFibras= preprocessor.getMaterialLoader.newSectionGeometry("geomCuadFibras")
y1= width/2.0
z1= depth/2.0

reinforcement= geomCuadFibras.getReinfLayers
reinforcementA= reinforcement.newStraightReinfLayer("acero")
reinforcementA.numReinfBars= 2
reinforcementA.barArea= As
reinforcementA.p1= geom.Pos2d(y0-depth/2.0,z0-width/2.0)
reinforcementA.p2= geom.Pos2d(y0+depth/2.0,z0-width/2.0)

reinforcementB= reinforcement.newStraightReinfLayer("acero")
reinforcementB.numReinfBars= 2
reinforcementB.barArea= As
reinforcementB.p1= geom.Pos2d(y0-depth/2.0,z0+width/2.0)
reinforcementB.p2= geom.Pos2d(y0+depth/2.0,z0+width/2.0)

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



nodes.calculateNodalReactions(True)
nod2= nodes.getNode(2)
deltax= nod2.getDisp[0] 
nod1= nodes.getNode(1)
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




ratio1= (nfib-nRebars)/nRebars
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
