# -*- coding: utf-8 -*-
from __future__ import print_function
'''Selection of entities inside a geometric object. Home made test.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from misc_utils import log_messages as lmsg

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod0= nodes.newNodeXYZ(0.0,0.0,0.0)
nod1= nodes.newNodeXYZ(0.5,0.5,0.5)
nod2= nodes.newNodeXYZ(2.0,2.0,2.0)
nod3= nodes.newNodeXYZ(3.0,3.0,3.0)

xcSetA= modelSpace.defSet('A')
xcSetA.nodes.append(nod0)
xcSetA.nodes.append(nod1)
xcSetA.nodes.append(nod2)

xcSetB= modelSpace.defSet('B')
xcSetB.nodes.append(nod1)
xcSetB.nodes.append(nod2)
xcSetB.nodes.append(nod3)

nodeSets= dict()
for n in [nod0, nod1, nod2, nod3]:
    setsC= preprocessor.getSets.getSetsContaining(n)
    setNames= list()
    for s in setsC:
        setNames.append(s.name)
    nodeSets[n.tag]= setNames

nodeSetsRef= {nod0.tag: ['A', 'total'], nod1.tag: ['A', 'B', 'total'], nod2.tag: ['A', 'B', 'total'], nod3.tag: ['B', 'total']}

ok= (nodeSets==nodeSetsRef)

# print(nodeSets)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ok):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')




