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

class WallLegsSection(bpm.AbstractCrossSection):

    def computePoints(self, modelSpace, axis, placement):
        ''' Return a dictionary containing the points that define the 
            section geometry.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        localRefSys= axis.getRefSys(placement)
        p0= localRefSys.getGlobalPosition(geom.Pos3d(0.0,-9.5/2.0,0.0))
        p1= localRefSys.getGlobalPosition(geom.Pos3d(0.0,-8.4/2.0,0.0))
        p2= localRefSys.getGlobalPosition(geom.Pos3d(0.0, -6.6/2.0, -1.5))
        p3= localRefSys.getGlobalPosition(geom.Pos3d(0.0, p1.y+1.4, 0.0))
        p4= localRefSys.getGlobalPosition(geom.Pos3d(0.0, p1.y+1.4, -1.5))
        p5= localRefSys.getGlobalPosition(geom.Pos3d(0.0, 0.0, 0.0))
        p6= localRefSys.getGlobalPosition(geom.Pos3d(0.0, 0.0, -1.5))
        self.points= dict()
        for i, p in enumerate([p0, p1, p2, p3, p4, p5, p6]):
            key= i
            if(p.y!=0.0):
                if(key==0):
                    self.points[-100]= modelSpace.newKPointFromPos3d(pos3d= p)
                    self.points[100]= modelSpace.newKPoint(x= p.x, y= -p.y, z= p.z)
                    pass
                else:
                    self.points[-key]= modelSpace.newKPointFromPos3d(pos3d= p)
                    self.points[key]= modelSpace.newKPoint(x= p.x, y= -p.y, z= p.z)
            else:
                self.points[key]= modelSpace.newKPointFromPos3d(pos3d= p)
    
    def computeSectionComponents(self, modelSpace, axis, placement):
        ''' Return a dictionary containing the components contained inside the
            section itself (i. e. diaphragms,...).

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        topPoints= [1, 3, 5, -3, -1]
        bottomPoints= [2, 4, 6, -4, -2]
        # Vertical lines.
        verticalLines= list()
        for pA, pB in zip(topPoints, bottomPoints):
            verticalLines.append((pA, pB, 'trsv_vertical'))
        # Diagonal lines.
        diagonalLines= list()
        tp0= topPoints[0]
        bp0= bottomPoints[0]
        for tp1, bp1 in zip(topPoints[1:], bottomPoints[1:]):
            diagonalLines.append((tp0, bp1, 'trsv_diagonal'))
            diagonalLines.append((bp0, tp1, 'trsv_diagonal'))
            tp0= tp1
            bp0= bp1
        # Top and bottom chords.
        horizontalLines= list()
        pA= topPoints[0]
        for pB in topPoints[1:]:
            horizontalLines.append((pA, pB, 'trsv_top_chord'))
            pA= pB
        pA= bottomPoints[0]
        for pB in bottomPoints[1:]:
            horizontalLines.append((pA, pB, 'trsv_bottom_chord'))
            pA= pB
        
        self.sectionComponents= dict()
        for (pA, pB, label) in verticalLines+diagonalLines+horizontalLines:
            kP1= self.points[pA]
            kP2= self.points[pB]
            ln= modelSpace.newLine(p1= kP1, p2= kP2)
            ln.setProp('label', label)
            key= str(pA)+'_'+str(pB)
            self.sectionComponents[key]= ln
        
class PilesSection(WallLegsSection):

    longPilesLength= 556.55-546.7
    shortPilesLength= 556.55-548.0

    def computePoints(self, modelSpace, axis, placement):
        ''' Return a dictionary containing the points that define the 
            section geometry.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        super().computePoints(modelSpace= modelSpace, axis= axis, placement= placement)
        localRefSys= axis.getRefSys(placement)
        if(placement in shortPilePlacements):
            p2b= localRefSys.getGlobalPosition(geom.Pos3d(0.0, -6.6/2.0, -1.5-self.shortPilesLength))
            if(placement != shortPilePlacements[1]): # if not in the middle.
                p6b= localRefSys.getGlobalPosition(geom.Pos3d(0.0, 0.0, -1.5-self.shortPilesLength))
                self.points[600]= modelSpace.newKPoint(x= p6b.x, y= p6b.y, z= p6b.z)
        else:
            p2b= localRefSys.getGlobalPosition(geom.Pos3d(0.0, -6.6/2.0, -1.5-self.longPilesLength))
        self.points[-200]= modelSpace.newKPoint(x= p2b.x, y= p2b.y, z= p2b.z)
        self.points[200]= modelSpace.newKPoint(x= p2b.x, y= -p2b.y, z= p2b.z)
 
    
    def computeSectionComponents(self, modelSpace, axis, placement):
        ''' Return a dictionary containing the components contained inside the
            section itself (i. e. diaphragms,...).

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        super().computeSectionComponents(modelSpace= modelSpace, axis= axis, placement= placement)
        if(placement in shortPilePlacements):
            label= 'short_pile'
        else:
            label= 'long_pile'
        kP1= self.points[2]
        kP2= self.points[200]
        ln= modelSpace.newLine(p1= kP1, p2= kP2)
        ln.setProp('label', label)
        key= 'pile2'
        self.sectionComponents[key]= ln
        kP1= self.points[-2]
        kP2= self.points[-200]
        ln= modelSpace.newLine(p1= kP1, p2= kP2)
        ln.setProp('label', label)
        key= '-pile2'
        self.sectionComponents[key]= ln
        if(600 in self.points):
            kP1= self.points[6]
            kP2= self.points[600]
            ln= modelSpace.newLine(p1= kP1, p2= kP2)
            ln.setProp('label', label)
            key= 'pile6'
            self.sectionComponents[key]= ln

length= 25.2 # 26.4
pth= geom.Polyline3d([geom.Pos3d(0,0,0), geom.Pos3d(length,0,0)])
ft= geom.FrenetTrihedron(pth)

# Placements of the wall legs.
wallLegPlacements= [0.60]
n= int(length/1.2)-1
wallLegPlacements+= n*[1.2]
wallLegPlacements= list(np.cumsum(wallLegPlacements))

# Placements of the piles.
pilePlacements= [1.2, 4.8, 3.6, 3.0, 3.0, 3.6, 4.8]
pilePlacements= list(np.cumsum(pilePlacements))
# Placement of the short piles.
shortPilePlacements= pilePlacements[2:5]
placements= wallLegPlacements+pilePlacements
placements.sort()
## simplify placements.
placements= bpm.simplify_placements(placements, tol= 1e-3)
## Search placements corresponding to short piles.
shortPilePlacements= bpm.get_closest_placements(input_placements= placements, positions= shortPilePlacements, tol= 1e-3)

pilePlacements= bpm.get_closest_placements(input_placements= placements, positions= pilePlacements, tol= 1e-3)

crossSectionsClasses= list()
for p in placements:
    if(p in pilePlacements):
        crossSectionsClasses.append(PilesSection)
    elif(p in wallLegPlacements):
        crossSectionsClasses.append(WallLegsSection)
    else:
        print('error: ', p)

# Line connection pattern.
## Diagonals along the pile cap.
diagonalsAlongPileCap= [[1, 2, 'lng_diagonal'], [2, 1, 'lng_diagonal'], [-1, -2, 'lng_diagonal'], [-2, -1, 'lng_diagonal'], [5, 6, 'lng_diagonal'], [6, 5, 'lng_diagonal'], [3, 4, 'lng_diagonal'], [4, 3, 'lng_diagonal'], [-3, -4, 'lng_diagonal'], [-4, -3, 'lng_diagonal']]
topChordsAlongPileCap= [[1, 1, 'lng_top_chord'], [-1, -1, 'lng_top_chord'], [3, 3, 'lng_top_chord'], [-3, -3, 'lng_top_chord'], [5, 5, 'lng_top_chord']]
lineConnections= [(length, diagonalsAlongPileCap+topChordsAlongPileCap)]
bottomChordsAlongPileCap= [[2, 2, 'lng_bottom_chord'], [-2, -2, 'lng_bottom_chord'], [4, 4, 'lng_bottom_chord'], [-4, -4, 'lng_bottom_chord'], [6, 6, 'lng_bottom_chord']]
lineConnections= [(length, diagonalsAlongPileCap+topChordsAlongPileCap+bottomChordsAlongPileCap)]
surfaceConnections= [(length, [([(0,100), (0,1), (1,1), (1,100)], 'pile_cap'), ([(0,-100), (0,-1), (1,-1), (1,-100)], 'pile_cap'), ([(0,1), (0,3), (1,3), (1,1)], 'pile_cap'), ([(0,-1), (0,-3), (1,-3), (1,-1)], 'pile_cap'), ([(0,3), (0,5), (1,5), (1,3)], 'pile_cap'), ([(0,-3), (0,5), (1,5), (1,-3)], 'pile_cap') ]) ]

# Problem definition.
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
modelSpace= predefined_spaces.StructuralMechanics3D(preprocessor.getNodeHandler)

bm= bpm.BridgeModeller(axis= ft, placements= placements, crossSectionsClasses= crossSectionsClasses, lineConnections= lineConnections, surfaceConnections= surfaceConnections)
bm.buildModelGeometry(modelSpace= modelSpace)

xcTotalSet= modelSpace.getTotalSet()
xcSets= bpm.create_sets_from_labels(modelSpace, xcTotalSet)

checkDict= dict()
for label in xcSets:
    xcSet= xcSets[label]
    xcSet.fillDownwards()
    checkDict[label]= (len(xcSet.lines), len(xcSet.surfaces))
refDict= {'trsv_vertical': (135, 0), 'trsv_diagonal': (216, 0), 'trsv_top_chord': (108, 0), 'trsv_bottom_chord': (108, 0), 'long_pile': (8, 0), 'short_pile': (8, 0), 'lng_diagonal': (260, 0), 'lng_top_chord': (130, 0), 'lng_bottom_chord': (130, 0), 'pile_cap': (344, 156)}
testOK= (checkDict==refDict)

l_max= 0.0
l_min= 6.023e23
for ln in xcTotalSet.lines:
    l= ln.getLength()
    if(l>l_max):
        l_max= l
    if(l<l_min):
        l_min= l

ratio0= abs(l_min-0.5)/0.5
ratio1= abs(l_max-9.85)/9.85
testOK= testOK and (ratio0<1e-12) and (ratio1<1e-12)

'''
print('lines without label: ', len(linesWithoutLabel))
print('surfaces without label: ', len(surfacesWithoutLabel))
print(checkDict)
print(testOK)
print(l_min, ratio0, l_max, ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayBlocks(displayLocalAxes= False)
# for label in xcSets:
#     setToDisplay= xcSets[label]
#     oh.displayBlocks(setToDisplay= setToDisplay, displayLocalAxes= False)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
