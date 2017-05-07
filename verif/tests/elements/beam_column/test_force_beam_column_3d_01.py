# -*- coding: utf-8 -*-
# home made test
# Ménsula horizontal sometida a carga de tracción en su extremo.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model import fix_node_6dof

# Geometry
width= .001
depth= .01
nDivIJ= 2
nDivJK= 2
y0= 0
z0= 0
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude en N

prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Materials definition
fy= 275e6 # Tensión de cedencia of the steel
E= 210e9 # Young modulus of the steel.
acero= typical_materials.defSteel01(preprocessor, "acero",E,fy,0.001)

import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/geomCuadFibrasTN.py") #Definition of section geometry (regions and rebars)
# Definition of a new empty fiber section named 'cuadFibrasTN' and stored in a
# Python variable of the same name (surprisingly enough).
cuadFibrasTN= preprocessor.getMaterialLoader.newMaterial("fiber_section_3d","cuadFibrasTN")
fiberSectionRepr= cuadFibrasTN.getFiberSectionRepr() #Fiber section representation
                                                     # of 'cuadFibrasTN'
fiberSectionRepr.setGeomNamed("geomCuadFibrasTN") #We assign the geometry (regions and rebars)
                                                  #to the fiber section representation
                                                  #of 'cuadFibrasTN'
cuadFibrasTN.setupFibers() #Create the fibers from the information contained in th
                           #geometry.
fibras= cuadFibrasTN.getFibers() #Get the fiber container from the object.
A= fibras.getSumaAreas(1.0) #Get the sum of the fiber areas.



# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultMaterial= "cuadFibrasTN" #Material name for the element (the fiber section).
beam3d= elementos.newElement("force_beam_column_3d",xc.ID([1,2]));

# Constraints
constraints= preprocessor.getConstraintLoader
fix_node_6dof.fixNode6DOF(constraints,1)

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0,0,0,0,0]))
#We add the load case to domain.
casos.addToDomain("0")
# Procedimiento de solución
analisis= predefined_solutions.simple_static_modified_newton(prueba)
result= analisis.analyze(10)

nodes= preprocessor.getNodeLoader 
nod2= nodes.getNode(2)
delta= nod2.getDisp[0]  # Node 2 xAxis displacement

elementos= preprocessor.getElementLoader

elem1= elementos.getElement(0)
elem1.getResistingForce()
scc= elem1.getSections()[0]

N0= scc.getStressResultantComponent("N")

deltateor= (F*L/(E*A))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs((N0-F)/F))

''' 
print "delta: ",delta
print "deltaTeor: ",deltateor
print "ratio1= ",ratio1
print N0
print "ratio2= ",ratio2
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-10:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

