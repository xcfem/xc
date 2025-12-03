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

p0= geom.Pos2d(0,-2.0)
p1= geom.Pos2d(5,-0.5)
p2= geom.Pos2d(10,-2.0)

parabola= pb.Parabola(p0,p1,p2)

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
        pA= localRefSys.getGlobalPosition(geom.Pos3d(0.0,0.0,0.0))
        kPA= modelSpace.newKPointFromPos3d(pos3d= pA)
        localZ= parabola.y(placement)
        pB= localRefSys.getGlobalPosition(geom.Pos3d(0.0,0.0,localZ))
        kPB= modelSpace.newKPointFromPos3d(pos3d= pB)
        self.points= {'A':kPA, 'B':kPB}
    
    def computeSectionComponents(self, modelSpace, axis, placement):
        ''' Return a dictionary containing the components contained inside the
            section itself (i. e. diaphragms,...).

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        kPA= self.points['A']
        kPB= self.points['B']
        ln= modelSpace.newLine(p1= kPA, p2= kPB)
        self.sectionComponents= {'lAB': ln}
        


length= 10.0
pth= geom.Polyline3d([geom.Pos3d(0,0,0), geom.Pos3d(length,0,0)])
ft= geom.FrenetTrihedron(pth)
placements= list(range(0,int(length)+1))
crossSectionsClasses= len(placements)*[TestSection]
# Line connection pattern.
lineConnections= [(length/2.0+1e-3, [['A','A'], ['B','B'], ['B', 'A']]), (length, [['A', 'A'], ['B', 'B'], ['A', 'B']])]

# Problem definition.
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
modelSpace= predefined_spaces.StructuralMechanics3D(preprocessor.getNodeHandler)

bm= bpm.BridgeModeller(axis= ft, placements= placements, crossSectionsClasses= crossSectionsClasses, lineConnections= lineConnections, surfaceConnections= [])
bm.buildModelGeometry(modelSpace= modelSpace)

# Check results.
xcTotalSet= modelSpace.getTotalSet()
totalLength= 0.0
for l in xcTotalSet.lines:
    totalLength+= l.getLength()

refTotalLength= 48.34326179077429

ratio= abs(totalLength-refTotalLength)/refTotalLength

# print(totalLength, ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayBlocks()
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
