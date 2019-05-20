# -*- coding: utf-8 -*-
'''Verification test taken from example 2-005 of 
   the SAP 2000 verification manual.'''

# feProblem.setVerbosityLevel(0)

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

NumDivI= 8
NumDivJ= 8
CooMaxX= 10
CooMaxY= 2
E= 17472000 # Elastic modulus en lb/in2
nu= 0.3 # Poisson's ratio
G= 6720000
thickness= 0.0001 # Cross section depth expressed in inches.
unifLoad= 0.0001 # Carga uniforme en lb/in2.
ptLoad= 100 # Punctual load en lb.

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.sections import internal_forces

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

nodes.newSeedNode()

# Define materials
nmb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,thickness)



seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "memb1"
seedElemHandler.defaultTag= 1
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))



points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(CooMaxX,0.0,0.0))
pt= points.newPntIDPos3d(3,geom.Pos3d(CooMaxX,CooMaxY,0.0))
pt= points.newPntIDPos3d(4,geom.Pos3d(0.0,CooMaxY,0.0))
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
surfaces.defaultTag= 1
s= surfaces.newQuadSurfacePts(1,2,3,4)
s.nDivI= NumDivI
s.nDivJ= NumDivJ

f1= preprocessor.getSets.getSet("f1")
f1.genMesh(xc.meshDir.I)
sides= s.getEdges
#Edge iterator
for l in sides:
  for i in l.getEdge.getNodeTags():
    modelSpace.fixNode000_FFF(i)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
#lPatterns.currentLoadPattern= "0"


f1= preprocessor.getSets.getSet("f1")
nNodes= f1.getNumNodes
 
node= f1.getNodeIJK(1,NumDivI/2+1,NumDivJ/2+1)
# print "Central node: ", node.tag
# print "Central node coordinates: ", node.getCoo
lp0.newNodalLoad(node.tag,xc.Vector([0,0,-ptLoad,0,0,0])) # Concentrated load


nElems= f1.getNumElements
#We add the load case to domain.
lPatterns.addToDomain(lp0.name)


# Solution procedure
analisis= predefined_solutions.simple_static_linear(feProblem)
analOk= analisis.analyze(1)

f1= preprocessor.getSets.getSet("f1")

from postprocess.reports import export_internal_forces
setTotal= preprocessor.getSets["total"]
fName= "/tmp/test_export_shell_internal_forces.txt"
f= open(fName,"w")
export_internal_forces.exportInternalForces("test",setTotal.getElements,f)
f.close()


mean= [internal_forces.CrossSectionInternalForces(),internal_forces.CrossSectionInternalForces()]
nCols= len(mean)
nRows= 0
import csv
cr = csv.reader(open(fName,"rb"))
for row in cr:
  nRows+= 1
  sectionIndex= eval(row[2])
  mean[sectionIndex]+= internal_forces.CrossSectionInternalForces(eval(row[3]),eval(row[4]),eval(row[5]),eval(row[6]),eval(row[7]),eval(row[8]))

for m in mean:
  m*= 1.0/nRows

meanRef= [internal_forces.CrossSectionInternalForces(0.0, -1.4141789118e-08, 0.0, 0.0, -0.377847769601, 0.0),internal_forces.CrossSectionInternalForces(0.0, 3.746624204e-08, 0.0, 0.0, -1.6862614343, 0.0)]
#internal_forces.ShellMaterialInternalForces(0.0,0.0,0.0,-0.755991356310675,-2.4972837939920614,-1.4220260169048315e-13, -3.033006243120112e-08).getWoodArmer()

ratio1= 0.0
for i in range(0,2):
  ratio1+= (meanRef[i]-mean[i]).getModulus()


# print "mean[0]= ", mean[0]
# print "meanRef[0]= ", meanRef[0]
# print "diff[0]= ", mean[0]-meanRef[0]
# print "mean[1]= ", mean[1]
# print "meanRef[1]= ", meanRef[1]
# print "diff[1]= ", mean[1]-meanRef[1]
# print "ratio1= ",ratio1

import os
from miscUtils import LogMessages as lmsg
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
