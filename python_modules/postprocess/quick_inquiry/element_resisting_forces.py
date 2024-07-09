# -*- coding: utf-8 -*-
''' Extract element resisting forces results from the finite element model.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2024, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import geom
import xc

def compute_element_resisting_forces(contour, elemSet, tol= 0.0):
    ''' Extract the resisting forces that act on the given contour. To do that
    the elements that cross the contour border (i. e. the elements which have
    nodes inside and outside the given contour) are selected. For those 
    elements, the resisting forces at the nodes outside the contour are
    computed.

    :param contour: geometric object subjected to the forces to be extracted.
    :param elemSet: set of finite element to extract the forces from.
    :returns: dictionary containing the forces in each of the nodes external
              to the contour. The key of the dictionary is the node identifier.
    '''
    contourSetName= elemSet.name+'_contour_elements'
    elementsOnContourBorder= elemSet.pickElemsCrossing(contourSetName, contour, tol)
    elementsOnContourBorder.fillDownwards()
    retval= dict() # forces on the nodes outside the contour.
    for e in elementsOnContourBorder.elements:
        for i, n in enumerate(e.nodes):
            if(n.Out(contour, 1, tol)): # node outside the critical contour.
                nodePosition= n.getInitialPos3d
                nTag= n.tag
                e.getResistingForce()
                nodeResistingForce= e.getNodeResistingForceIncInertia(i)
                F= geom.Vector3d(nodeResistingForce[0],nodeResistingForce[1],nodeResistingForce[2])
                M= geom.Vector3d(nodeResistingForce[3],nodeResistingForce[4],nodeResistingForce[5])
                svs= geom.SlidingVectorsSystem3d(nodePosition, F, M)
                if(nTag in retval):
                    retval[nTag]+= svs
                else:
                    retval[nTag]= svs
    return retval
