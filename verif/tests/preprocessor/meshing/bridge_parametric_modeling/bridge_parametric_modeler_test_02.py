# -*- coding: utf-8 -*-
''' First test.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model.mesh import bridge_parametric_modeling as bpm
from model import predefined_spaces
from geom_utils import parabola as pb
import numpy as np

class TestSection(bpm.AbstractCrossSection):

    def computePoints(self, modelSpace, axis, placement):
        ''' Return a dictionary containing the points that define the 
            section geometry.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        localRefSys= axis.getRefSys(placement)
        p0= localRefSys.getGlobalPosition(geom.Pos3d(0.0,-1.0,0.0))
        p1= localRefSys.getGlobalPosition(geom.Pos3d(0.0,1.0,0.0))
        p2= localRefSys.getGlobalPosition(geom.Pos3d(0.0,0.0,-1.0))
        self.points= dict()
        for i, p in enumerate([p0, p1, p2]):
            self.points[i]= modelSpace.newKPoint(x= p.x, y= p.y, z= p.z)
    
    def computeSectionComponents(self, modelSpace, axis, placement):
        ''' Return a dictionary containing the components contained inside the
            section itself (i. e. diaphragms,...).

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        kp0= self.points[0]
        kp1= self.points[1]
        self.sectionComponents= dict()
        ln= modelSpace.newLine(p1= kp0, p2= kp1)
        self.sectionComponents[0]= ln

length= 10
pth= geom.Polyline3d([geom.Pos3d(0,0,0), geom.Pos3d(length,0,0)])
ft= geom.FrenetTrihedron(pth)

# Placements of the cross-sections.
placements= list(range(0,11))
crossSectionsClasses= len(placements)*[TestSection]
surfaceConnections= [(length, [([(0,0), (0,2), (1,2), (1,0)], None), ([(0,1), (0,2), (1,2), (1,1)], None)])]
lineConnections= [(length, [[0, 1], [1, 0]])]
# Problem definition.
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
modelSpace= predefined_spaces.StructuralMechanics3D(preprocessor.getNodeHandler)

bm= bpm.BridgeModeller(axis= ft, placements= placements, crossSectionsClasses= crossSectionsClasses, lineConnections= lineConnections, surfaceConnections= surfaceConnections)
bm.buildModelGeometry(modelSpace= modelSpace)

# Check results.
xcTotalSet= modelSpace.getTotalSet()
totalLength= 0.0
for l in xcTotalSet.lines:
    totalLength+= l.getLength()
totalArea= 0.0
for s in xcTotalSet.surfaces:
    totalArea+= s.getArea()

refTotalLength= 11*2+20*math.sqrt(5)+30+22*math.sqrt(2)
refTotalArea= 2*length*math.sqrt(2)

ratio0= abs(totalLength-refTotalLength)/refTotalLength
ratio1= abs(totalArea-refTotalArea)/refTotalArea

'''
print(totalLength, refTotalLength, ratio0)
print(totalArea, refTotalArea, ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-6) and (abs(ratio1)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayBlocks(displayLocalAxes= False)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
