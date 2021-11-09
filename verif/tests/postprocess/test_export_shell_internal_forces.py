# -*- coding: utf-8 -*-
from __future__ import print_function
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
unifLoad= 0.0001 # Uniform load (lb/in2).
ptLoad= 100 # Punctual load (lb).

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.sections import internal_forces
import json

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Define materials
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)



# Define materials
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,0.0,thickness)



seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= memb1.name
seedElemHandler.defaultTag= 1
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))



points= preprocessor.getMultiBlockTopology.getPoints
pt= points.newPoint(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPoint(2,geom.Pos3d(CooMaxX,0.0,0.0))
pt= points.newPoint(3,geom.Pos3d(CooMaxX,CooMaxY,0.0))
pt= points.newPoint(4,geom.Pos3d(0.0,CooMaxY,0.0))
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
surfaces.defaultTag= 1
s= surfaces.newQuadSurfacePts(1,2,3,4)
s.nDivI= NumDivI
s.nDivJ= NumDivJ

f1= preprocessor.getSets.getSet("f1")
f1.genMesh(xc.meshDir.I)
sides= s.getSides
# Edge iterator
for l in sides:
  for i in l.getEdge.getNodeTags():
    modelSpace.fixNode000_FFF(i)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')



f1= preprocessor.getSets.getSet("f1")
nNodes= f1.getNumNodes
 
node= f1.getNodeIJK(1, int(NumDivI/2+1), int(NumDivJ/2+1))
# print("Central node: ", node.tag)
# print("Central node coordinates: ", node.getCoo)
lp0.newNodalLoad(node.tag,xc.Vector([0,0,-ptLoad,0,0,0])) # Concentrated load


nElems= f1.getNumElements
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.simple_static_linear(feProblem)
analOk= analysis.analyze(1)

f1= preprocessor.getSets.getSet("f1")

from postprocess.reports import export_internal_forces
setTotal= preprocessor.getSets["total"]
fName= "/tmp/test_export_shell_internal_forces.txt"
internalForcesDict=  export_internal_forces.getInternalForcesDict("test",setTotal.getElements)
with open(fName, 'w') as outfile:
    json.dump(internalForcesDict, outfile)

average= [internal_forces.CrossSectionInternalForces(),internal_forces.CrossSectionInternalForces()]
nCols= len(average)

with open(fName) as json_file:
    combInternalForcesDict= json.load(json_file)
json_file.close()

nRows= 0
for key in combInternalForcesDict.keys():
    elements= combInternalForcesDict[key]
    for elemId in elements.keys():
        elementData= elements[elemId]
        internalForces= elementData['internalForces']
        for k in internalForces.keys():
            nRows+= 1
            sectionIndex= eval(k)
            crossSectionInternalForces= internal_forces.CrossSectionInternalForces()
            forces= internalForces[k]
            crossSectionInternalForces.setFromDict(forces)
            average[sectionIndex]+= crossSectionInternalForces

for m in average:
  m*= 1.0/nRows

averageRef= [internal_forces.CrossSectionInternalForces(0.0, -1.4141789118e-08, 0.0, 0.0, -0.377847769601, 0.0),internal_forces.CrossSectionInternalForces(0.0, 3.746624204e-08, 0.0, 0.0, -1.6862614343, 0.0)]

ratio1= 0.0
for i in range(0,2):
  ratio1+= (averageRef[i]-average[i]).getModulus()

'''
print("average[0]= ", average[0])
print("averageRef[0]= ", averageRef[0])
print("diff[0]= ", average[0]-averageRef[0])
print("average[1]= ", average[1])
print("averageRef[1]= ", averageRef[1])
print("diff[1]= ", average[1]-averageRef[1])
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
