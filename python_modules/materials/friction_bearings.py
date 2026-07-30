# -*- coding: utf-8 -*-
''' Convenience functions to define friction bearings.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import xc
import sys
from misc_utils import log_messages as lmsg
import uuid

def def_flat_slider_bearing_2d(modelSpace, n1, n2, frictionModel, vertResp, rotResp, kInit, x= xc.Vector([1,0,0]), y= xc.Vector([0,1,0]), shearDistI= 0.0, addRayleigh= 0, mass= 0.0, maxIter= 20, tol= 1e-8, kFactUplift= 1e-12):
    ''' Defines a FlatSliderSimple2d element.

    :param modelSpace: wrapper of the FE preprocessor.
    :param n1: node I.
    :param n2: node J.
    :param frictionModel: friction model (horizontal response of the bearing).
    :param vertResp: uniaxial material defining the vertical response of the
                     bearing.
    :param rotResp: uniaxial material defining the rotational response of the
                    bearing.
    :param kInit: initial stiffness of hysteretic component.
    :param x: x local direction vector.
    :param y: y local direction vector.
    :param shearDistI: shear distance from node I as fraction of length.
    :param addRayleigh: flag to add Rayleigh damping.
    :param mass: mass of the bearing element.
    :param maxIter: maximum number of iterations to reach convergence.
    :param tol: tolerance for convergence criterion.
    :param kFactUplift: stiffness factor when uplift is encountered.
    '''
    retval= modelSpace.newElement("FlatSliderSimple2d", nodeTags= [n1.tag,n2.tag])
    retval.setFrictionModels([frictionModel])
    retval.setMaterials([vertResp, rotResp])
    retval.setInitialStiffnessOfHystereticComponent(kInit)
    retval.setLocalXDirection(x)
    retval.setLocalYDirection(y)
    retval.setShearDistanceFromNodeIAsFractionOfLength(shearDistI)
    retval.setAddRayleighDampingFlag(addRayleigh)
    retval.setBearingElementMass(mass)
    retval.setMaxIter(maxIter)
    retval.setTol(tol)
    retval.setStiffnessFactorWhenUplift(kFactUplift)
    retval.setup() # Call after any change in the input parameters.
    return retval
