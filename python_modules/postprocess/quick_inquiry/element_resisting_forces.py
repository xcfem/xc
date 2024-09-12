# -*- coding: utf-8 -*-
''' Extract element resisting forces results from the finite element model.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2024, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import sys
import geom
import uuid
import xc
from misc_utils import log_messages as lmsg

def sort_control_positions(controlPositions):
    ''' Sort the controls positions according to its placement along the
        contour.

    :param controlPositions: dictionary containing, for each outside node,
                             the corresponding position in the contour and
                             the elements touching that node.
    '''
    tmp= list()
    for nTag in controlPositions:
        contourLambda= controlPositions[nTag]['contour_lambda']
        tmp.append((nTag, contourLambda))
    sortedTags= sorted(tmp, key=lambda tup: tup[1])
    retval= dict()
    for (nTag, contourLambda) in sortedTags:
        retval[nTag]= controlPositions[nTag]
    return retval

def compute_tributary_lengths(controlPositions, contourPerimeter):
    ''' Compute the tributary length for each control position.

    :param controlPositions: dictionary containing, for each outside node,
                             the corresponding position in the contour and
                             the elements touching that node.
    :param contourPerimeter: length of the contour perimeter.
    '''
    # Compute distances between positions.
    distances= list()
    keys= list(controlPositions.keys())
    sz= len(keys)
    if(sz>0):
        s0= controlPositions[keys[0]]['contour_lambda']
        for key in keys[1:]:
            s1= controlPositions[key]['contour_lambda']
            dist= s1-s0
            distances.append(dist)
            s0= s1
        distances.append(contourPerimeter-controlPositions[keys[-1]]['contour_lambda'])
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; no control positions to compute tributary lengths.')
    retval= dict()
    # Compute tributary lengths.
    if(distances):
        d0= distances[-1]
        for d, key in zip(distances, keys):
            retval[key]= (d0+d)/2.0
            d0= d
    return retval

class ContourControlPositions(object):
    ''' Control positions along a contour.

    :ivar controlPositions: dictionary containing, for each outside node,
                            the corresponding position in the contour and
                            the elements touching that node.
    '''


    def __init__(self, contour, elemSet, contourSetName= None, tol= 0.0):
        ''' Compute the positions on the contour perimeter that correspond to 
            the nodes, which, being part of the elements that cross the 
            contour, lie outside it.

        :param contour: geometric object subjected to the forces to be 
                        extracted.
        :param elemSet: set of finite element to extract the forces from.
        :param contourSetName: name of the new set that will store the elements
                               that cross the contour.
        :returns: dictionary that, for each of the nodes outside the contour
                  contains: 

                  - the positions on the contour perimeter that correspond to
                    the node. 
                  - the crossing elements that touch each of the nodes outside
                    the contour.
        '''
        if(contourSetName is None):
            contourSetName= uuid.uuid4().hex
        self.elementsOnContourBorder= elemSet.pickElemsCrossing(contourSetName, contour, tol)
        self.elementsOnContourBorder.fillDownwards()
        tmp= dict() # contour positions and elements for each node outside it.
        if(len(self.elementsOnContourBorder.elements)>0):
            for e in self.elementsOnContourBorder.elements:
                for i, n in enumerate(e.nodes):
                    if(n.Out(contour, 1, tol)): # node outside the critical contour.
                        nodePosition= n.getInitialPos3d
                        # Project the node position onto the contour.
                        projPosition= contour.getProjection(nodePosition)
                        # Compute the parameter of the projected position in the contour.
                        contourLambda= contour.getLambda(projPosition)
                        nTag= n.tag
                        if(nTag in tmp):
                            tmp[nTag]['neighbour_elements'].append(e) 
                        else:
                            tmp[nTag]= {'node':n, 'node_position': nodePosition, 'contour_position': projPosition, 'contour_lambda':contourLambda, 'neighbour_elements': [e]}
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; no elements crossing contour.')
        if(len(tmp)>0):
            tmp= sort_control_positions(tmp)
            # Compute tributary lengths.
            contourPerimeter= contour.getPerimeter()
            tributaryLengths= compute_tributary_lengths(controlPositions= tmp, contourPerimeter= contourPerimeter)
            for nTag in tmp:
                tmp[nTag]['tributary_length']= tributaryLengths[nTag]
        self.controlPositions= tmp

    def getNumberOfControlPositions(self):
        ''' Return the number of control positions for this object.'''
        return len(self.controlPositions)
    
    def computeElementResistingForces(self):
        ''' Extract the resisting forces that act on the given control 
            positions.
        '''
        retval= dict() # forces on the nodes outside the contour.
        for nTag in self.controlPositions:
            values= self.controlPositions[nTag]
            neighbourElements= values['neighbour_elements']
            alreadyVisited= set()
            F= geom.Vector3d(0,0,0)
            M= geom.Vector3d(0,0,0)
            for e in neighbourElements:
                eTag= e.tag
                if(eTag not in alreadyVisited):
                    alreadyVisited.add(eTag)
                    e.getResistingForce()
                    nodeResistingForce= e.getNodeResistingForceIncInertiaByTag(nTag)
                    # Get resisting force.
                    F+= geom.Vector3d(nodeResistingForce[0],nodeResistingForce[1],nodeResistingForce[2])
                    # Get resisting moment.
                    M+= geom.Vector3d(nodeResistingForce[3],nodeResistingForce[4],nodeResistingForce[5])
                else:
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    lmsg.error(className+'.'+methodName+'; element: '+str(eTag)+' appears twice for node: '+str(nTag))
            nodePosition= values['node_position']
            svs= geom.SlidingVectorsSystem3d(nodePosition, F, M)
            retval[nTag]= svs
                    
        return retval

    def computeContourForces(self):
        ''' Compute the forces acting on the contour.'''
        retval= dict()
        elementResistingForces= self.computeElementResistingForces()
        for nTag in self.controlPositions:
            svs= elementResistingForces[nTag]
            pos= svs.getOrg()
            contourPos= self.controlPositions[nTag]['contour_position']
            contourLambda= self.controlPositions[nTag]['contour_lambda']
            tributaryLength= self.controlPositions[nTag]['tributary_length']
            newSvs= svs.reduceTo(contourPos)
            retval[nTag]= {'contour_position':contourPos, 'contour_lambda': contourLambda, 'sliding_vector_system': newSvs, 'tributary_length':tributaryLength}
        return retval

    
