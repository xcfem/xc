# -*- coding: utf-8 -*-
# home made test

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties
from materials.sections import defSeccAggregation

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 2 # Bar length.
P= 10e3 # Carga uniforme transversal.
n= 1e6 # Carga uniforme axial.

prb= xc.ProblemaEF()
preprocessor=  prb.getPreprocessor   
nodes= preprocessor.getNodeLoader

seccPrueba= section_properties.RectangularSection("prueba",b=.20,h=.30)
matSeccPrueba= typical_materials.MaterialData("matprueba",E=7E9,nu=0.3,rho=2500)


# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
# Definimos el material
defSeccAggregation.defSeccAggregation2d(preprocessor, seccPrueba,matSeccPrueba)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(L,0.0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
    
# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",seccPrueba.A(),matSeccPrueba.E,seccPrueba.Iz())

# Elements definition
elements= preprocessor.getElementLoader
elements.defaultTransformation= "lin"
elements.defaultMaterial= "scc" 
elements.defaultMaterial= seccPrueba.sectionName
elements.numSections= 3 # Number of sections along the element.
elements.defaultTag= 1
beam2d= elements.newElement("ForceBeamColumn2d",xc.ID([1,2]))
    
# Constraints
constraints= preprocessor.getConstraintLoader
modelSpace.fixNode000(1)


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("beam2d_uniform_load")
eleLoad.elementTags= xc.ID([1]) 
eleLoad.transComponent= -P 
eleLoad.axialComponent= n
#We add the load case to domain.
casos.addToDomain("0")

# Solution procedure
analisis= predefined_solutions.simple_static_modified_newton(prb)
result= analisis.analyze(1)


nodes.calculateNodalReactions(True)
nod2= nodes.getNode(2)
delta0= nod2.getDisp[0] 
delta1= nod2.getDisp[1] 
nod1= nodes.getNode(1)
RN= nod1.getReaction[0] 
nod2= nodes.getNode(2)
RN2= nod2.getReaction[0] 

elem1= elements.getElement(1)
elem1.getResistingForce()
scc0= elem1.getSections()[0]

N0= scc0.getStressResultantComponent("N")


F= (n*L)
delta0Teor= (n*L**2/2.0/matSeccPrueba.E/seccPrueba.A())
ratio0= ((delta0-delta0Teor)/delta0Teor)
Q= P*L
delta1Teor= (-Q*L**3/8/matSeccPrueba.E/seccPrueba.Iz())
ratio1= ((delta1-delta1Teor)/delta1Teor)
ratio2= (abs((N0-F)/F))
ratio3= (abs((RN+F)/F))


''' 
print "delta0= ",delta0
print "delta0Teor= ",delta0Teor
print "ratio0= ",ratio0
print "delta1= ",delta1
print "delta1Teor= ",delta1Teor
print "ratio1= ",ratio1
print "F= ",F
print "N0= ",N0
print "ratio2= ",ratio2
print "RN= ",RN
print "RN2= ",RN2
print "ratio3= ",ratio3
 '''
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-4) & (abs(ratio1)<0.03) & (abs(ratio3)<1e-14):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
