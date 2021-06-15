# -*- coding: utf-8 -*-
from __future__ import print_function
'''Element selection from a node set. Home made test.'''
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0.0,0.0,0.0)
n2= nodes.newNodeXYZ(1.0,0.0,0.0)
n3= nodes.newNodeXYZ(1.0,1.0,0.0)
n4= nodes.newNodeXYZ(0.0,1.0,0.0)
n5= nodes.newNodeXYZ(0.0,0.0,1.0)
n6= nodes.newNodeXYZ(1.0,0.0,1.0)
n7= nodes.newNodeXYZ(1.0,1.0,1.0)
n8= nodes.newNodeXYZ(0.0,1.0,1.0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,1]))

# Materials
section= typical_materials.defElasticSection3d(preprocessor, "section",1,1,1,1,1,1)

elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name # Coord. transformation.
elements.defaultMaterial= section.name
elements.defaultTag= 1 #Tag for next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n2.tag,n3.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n3.tag,n4.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n4.tag,n1.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n3.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n2.tag,n4.tag]))

beam3d= elements.newElement("ElasticBeam3d",xc.ID([n5.tag,n6.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n6.tag,n7.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n7.tag,n8.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n8.tag,n5.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n5.tag,n7.tag]))
beam3d= elements.newElement("ElasticBeam3d",xc.ID([n6.tag,n8.tag]))

# Trial set definition
trialSet1= preprocessor.getSets.defSet("trialSet1")

#Color property get/set test.
colorA= trialSet1.color
ratioA= colorA[0]**2+colorA[1]**2+colorA[2]**2
trialSet1.color= xc.Vector([1.0,2.0,3.0])
colorB= trialSet1.color
ratioB= (colorB[0]-1)**2+(colorB[1]-2)**2+(colorB[2]-3)**2


nodes= preprocessor.getSets.getSet("total").getNodes
for n in nodes:
  coord= n.getCoo
  if(abs(coord[1]-0.0)<1e-2):
    trialSet1.getNodes.append(n)



'''
nodes= trialSet1.getNodes
for n in nodes:
  coord= n.getCoord
    print("  node: ",n.tag," x= ",coord[0],", y= ",coord[1],", z= ",coord[2])
'''
elements= preprocessor.getSets.getSet("total").getElements
for e in elements:
  if((abs(e.getMaxCooNod(1)-0.0)<1e-2) & (abs(e.getMinCooNod(1)-0.0)<1e-2)):
    trialSet1.getElements.append(e)

''' 
print('ratioA= ', ratioA)
print('ratioB= ', ratioB)
elements= trialSet1.getElements
for e in elements:
    print("tag= ",e.tag," node I:",e.nod[0].tag," node J:",e.nod[1].tag)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratioA==0) & (ratioB==0) & (abs(trialSet1.getNumNodes-4)<1e-15) & (abs(trialSet1.getNumElements-2)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')




