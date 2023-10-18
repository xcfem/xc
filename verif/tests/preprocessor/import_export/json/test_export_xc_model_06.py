# -*- coding: utf-8 -*-
''' Test that the conciliaNDivs method works correctly when the block topology is readed from a dictionary.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import json
import geom
import xc
import os
import sys
from model import predefined_spaces
from materials import typical_materials

CooMaxX= 2
CooMaxY= 1
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density
old_stderr= sys.stderr

numNodes= 0
numElem= 0
# Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodeHandler)

# Problem geometry.
## K-points.
points= preprocessor.getMultiBlockTopology.getPoints
pointDict= dict()
pointDict[187]= [23.29, 80.77, 11.3991]
pointDict[184]= [22.2, 80.58, 11.3999]
pointDict[186]= [18.52, 97.55, 11.3991]
pointDict[185]= [17.46, 97.25, 11.3999]

quads= [[184, 187, 186, 185]]

kPointDict= dict()
for key in pointDict:
    [x, y, z]= pointDict[key]
    kPointDict[key]= points.newPoint(geom.Pos3d(x,y,x))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
for quad in quads:
    tags= list()
    for key in quad:
        tag= kPointDict[key].tag
        tags.append(tag)
    s= surfaces.newQuadSurfacePts(tags[0], tags[1], tags[2], tags[3])
    
xcTotalSet= modelSpace.getTotalSet()
surfaces= xcTotalSet.surfaces
for s in surfaces:
    s.setElemSize(0.5, False)

okBefore= modelSpace.conciliaNDivs()

# Write model to JSON file.
outputData= preprocessor.getDict()

# Create new problem.
fePrbB= xc.FEProblem()
prepB= fePrbB.getPreprocessor

# Populate from the previous dictionary.
prepB.setFromDict(outputData)

xcTotalSetB= prepB.getSets.getSet('total')
surfaces= xcTotalSetB.surfaces
for s in surfaces:
    s.setElemSize(0.5, False)

okAfter= prepB.getMultiBlockTopology.getSurfaces.conciliaNDivs()

'''
print(okBefore)
print(okAfter)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(okBefore and okAfter):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
