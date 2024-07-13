# -*- coding: utf-8 -*-
''' Extract element resisting forces results from the finite element model.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2024, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import geom
import xc

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
    s0= controlPositions[keys[0]]['contour_lambda']
    for key in keys[1:]:
        s1= controlPositions[key]['contour_lambda']
        dist= s1-s0
        distances.append(dist)
        s0= s1
    distances.append(contourPerimeter-controlPositions[keys[-1]]['contour_lambda'])
    # Compute tributary lengths.
    retval= dict()
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


    def __init__(self, contour, elemSet, tol= 0.0):
        ''' Compute the positions on the contour perimeter that correspond to 
            the nodes, which, being part of the elements that cross the 
            contour, lie outside it.

        :param contour: geometric object subjected to the forces to be 
                        extracted.
        :param elemSet: set of finite element to extract the forces from.
        :returns: dictionary that, for each of the nodes outside the contour
                  contains: 

                  - the positions on the contour perimeter that correspond to
                    the node. 
                  - the crossing elements that touch each of the nodes outside
                    the contour.
        '''
        contourSetName= elemSet.name+'_contour_elements'
        elementsOnContourBorder= elemSet.pickElemsCrossing(contourSetName, contour, tol)
        elementsOnContourBorder.fillDownwards()
        tmp= dict() # contour positions and elements for each node outside it.
        for e in elementsOnContourBorder.elements:
            for i, n in enumerate(e.nodes):
                if(n.Out(contour, 1, tol)): # node outside the critical contour.
                    nodePosition= n.getInitialPos3d
                    # Project the node position onto the contour.
                    projPosition= contour.getProjection(nodePosition)
                    # Compute the parameter of the projected position in the contour.
                    contourLambda= contour.getLambda(projPosition)
                    nTag= n.tag
                    if(nTag in tmp):
                        tmp[nTag]['neighbour_elements'].append((e, i)) 
                    else:
                        tmp[nTag]= {'node_position': nodePosition, 'contour_position': projPosition, 'contour_lambda':contourLambda, 'neighbour_elements': [(e,i)]}
        tmp= sort_control_positions(tmp)
        # Compute tributary lengths.
        contourPerimeter= contour.getPerimeter()
        tributaryLengths= compute_tributary_lengths(controlPositions= tmp, contourPerimeter= contourPerimeter)
        for nTag in tmp:
            tmp[nTag]['tributary_length']= tributaryLengths[nTag]
        self.controlPositions= tmp
    
    def computeElementResistingForces(self):
        ''' Extract the resisting forces that act on the given control 
            positions.
        '''
        retval= dict() # forces on the nodes outside the contour.
        for nTag in self.controlPositions:
            values= self.controlPositions[nTag]
            neighbourElements= values['neighbour_elements']
            for (e, i) in neighbourElements:
                e.getResistingForce()
                nodeResistingForce= e.getNodeResistingForceIncInertia(i)
                # Get resisting force.
                F= geom.Vector3d(nodeResistingForce[0],nodeResistingForce[1],nodeResistingForce[2])
                # Get resisting moment.
                M= geom.Vector3d(nodeResistingForce[3],nodeResistingForce[4],nodeResistingForce[5])
                nodePosition= values['node_position']
                svs= geom.SlidingVectorsSystem3d(nodePosition, F, M)
                if(nTag in retval):
                    retval[nTag]+= svs
                else:
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

    
