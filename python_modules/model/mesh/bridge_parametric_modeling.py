# -*- coding: utf-8 -*-
''' Bridge parametric modeller inspired on the article:

Sousa, Í. M. R., Vidal, C. A. and Cavalcante-Neto, J. B. (2024) ‘Automated Generation of Geometric Models of Box Girder Bridges’, Structural Engineering International, 34(2), pp. 293–303. doi: 10.1080/10168664.2024.2302480.
'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
from abc import ABC, abstractmethod
from misc_utils import log_messages as lmsg

class AbstractCrossSection(ABC):
    ''' Abstract base class for the bridge cross-sections.

    :ivar points: points that define the section geometry.
    :ivar sectionComponents: components contained inside the section itself 
                             (i. e. diaphragms,...).
    :ivar connectedComponents: components that connect this section with the 
                               following and/or precedent ones.
    '''
    def __init__(self, modelSpace, axis, placement):
        ''' Compute the positions of the points that define the section 
            geometry.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        self.computePoints(modelSpace= modelSpace, axis= axis, placement= placement)
        self.computeSectionComponents(modelSpace= modelSpace, axis= axis, placement= placement)
        self.connectedComponents= {'prev':dict(), 'next':dict()}
        self.placement= placement
        
    @abstractmethod
    def computePoints(self, modelSpace, axis, placement):
        ''' Compute the positions of the points that define the section 
            geometry.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        self.points= dict()
        
    @abstractmethod
    def computeSectionComponents(self, modelSpace, axis, placement):
        ''' Return a dictionary containing the components contained inside the
            section itself (i. e. diaphragms,...).

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placement: placement of the cross-section in the bridge defined
                          by the corresponding length along the bridge.
        '''
        self.sectionComponents= dict()
        
class BridgeModeller(object):
    ''' Parametric bridge modeller.

    :ivar axis: Frenet trihedron object representing the bridge axis.
    :ivar placements: locations of relevance to the structure represented
                      as lengths along the axis.
    :ivar crossSectionsClasses: list of cross-section classes corresponding
                                to each placement.
    '''
    def __init__(self, axis, placements, crossSectionsClasses, lineConnections, surfaceConnections):
        ''' Constructor.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param axis: Frenet trihedron object representing the bridge axis.
        :param placements: locations of relevance to the structure represented
                           as lengths along the axis.
        :param crossSectionsClasses: list of cross-section classes corresponding
                                    to each placement.
        :param lineConnections: Line connections between sections expressed as 
                                a list of (placement, connection pattern), 
                                tuples indicating the connection pattern to be 
                                used until the given placement.
        :param surfaceConnections: Surface connections between sections
                                   expressed as a list of (placement, 
                                   connection pattern), tuples indicating the 
                                   connection pattern to be used until the 
                                   given placement.
        '''
        self.axis= axis
        self.placements= placements
        self.crossSectionsClasses= crossSectionsClasses
        self.lineConnections= lineConnections
        self.surfaceConnections= surfaceConnections

    def getLineConnections(self, placement):
        ''' Return the indexes of the line connections applicable to the given
            placement.

        :param placement: placement of the last connected section.
        '''
        retval= list()
        if(len(self.lineConnections)>0):
            for i, lc in enumerate(self.lineConnections):
                if(placement>lc[0]):
                    break;
            for lc in self.lineConnections[i-1][1]:
                retval.append(lc)
        return retval
            
    def computeLineConnections(self, modelSpace):
        ''' Compute the line connections between cross-sections.
        
        :param modelSpace: PredefinedSpace object used to create the FE model.
        '''
        s0= self.crossSections[0]
        for s1 in self.crossSections[1:]:
            points= list()
            indexes= self.getLineConnections(placement= s1.placement)
            for lst in indexes:
                points.append((s0.points[lst[0]], s1.points[lst[1]]))
            for index, points in zip(indexes, points):
                (idx0, idx1)= (index[0], index[1])
                (p0, p1)= (points[0], points[1])
                if(len(index)==2):
                    label= None
                else:
                    label= index[2]
                ln= modelSpace.newLine(p1= p0, p2= p1)
                key= idx0+idx1
                s0.connectedComponents['next'][key]= ln
                s1.connectedComponents['prev'][key]= ln
                if(label):
                    ln.setProp('label', label)
            s0= s1
            
    def getSurfaceConnections(self, placement):
        ''' Return the indexes of the surface connections applicable to the 
            given placement.

        :param placement: placement of the last connected section.
        '''
        retval= list()
        if(len(self.surfaceConnections)>0):
            for i, sc in enumerate(self.surfaceConnections):
                if(placement>sc[0]):
                    break;
            for (sc, label) in self.surfaceConnections[i-1][1]:
                retval.append(((sc[0], sc[1], sc[2], sc[3]), label))
        return retval
            
    def computeSurfaceConnections(self, modelSpace):
        ''' Compute the surface connections between cross-sections.
        
        :param modelSpace: PredefinedSpace object used to create the FE model.
        '''
        s0= self.crossSections[0]
        for s1 in self.crossSections[1:]:
            points= list()
            indexes= self.getSurfaceConnections(placement= s1.placement)
            for (idx, label) in indexes:
                points= list()
                for tpl in idx:
                    surfaceIndex= tpl[0]
                    pointKey= tpl[1]
                    s= None
                    if(surfaceIndex==0):
                        s= s0
                    elif(surfaceIndex==1):
                        s= s1
                    else:
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        errMsg= '; surface index must be 0 or 1; was: '+str(surfaceIndex)
                        lmsg.error(className+'.'+methodName+errMsg)
                    points.append(s.points[pointKey])
                if(len(points)==4):
                    newSurface= modelSpace.newQuadSurface(*points)
                else:
                    newSurface= modelSpace.newPolygonalSurface(*points)
                newSurface.setProp('label', label)
            s0= s1
    
    def buildModelGeometry(self, modelSpace):
        ''' Create the block topology structure that defines the model 
            geometry.
        
        :param modelSpace: PredefinedSpace object used to create the FE model.
        '''
        self.crossSections= list()
        for placement, csClass in zip(self.placements, self.crossSectionsClasses):
            self.crossSections.append(csClass(modelSpace= modelSpace, axis= self.axis, placement= placement))
        self.computeLineConnections(modelSpace)
        self.computeSurfaceConnections(modelSpace)


def create_sets_from_labels(modelSpace, xcInputSet):
    ''' Create sets from the label properties of the given set members.

    :param modelSpace: PredefinedSpace object used to create the FE model.
    :param xcInputSet: set containing the entities to group in sets.
    '''
    retval= dict()
    for pt in xcInputSet.points:
        if(pt.hasProp('label')):
            label= pt.getProp('label')
            if(label in retval):
                xcSet= retval[label]
            else:
                xcSet= modelSpace.defSet(label)
                retval[label]= xcSet
            xcSet.points.append(pt)

    for ln in xcInputSet.lines:
        if(ln.hasProp('label')):
            label= ln.getProp('label')
            if(label in retval):
                xcSet= retval[label]
            else:
                xcSet= modelSpace.defSet(label)
                retval[label]= xcSet
            xcSet.lines.append(ln)

    for face in xcInputSet.surfaces:
        if(face.hasProp('label')):
            label= face.getProp('label')
            if(label in retval):
                xcSet= retval[label]
            else:
                xcSet= modelSpace.defSet(label)
                retval[label]= xcSet
            xcSet.surfaces.append(face)
            
    for body in xcInputSet.bodies:
        if(body.hasProp('label')):
            label= body.getProp('label')
            if(label in retval):
                xcSet= retval[label]
            else:
                xcSet= modelSpace.defSet(label)
                retval[label]= xcSet
            xcSet.bodies.append(body)

    return retval
