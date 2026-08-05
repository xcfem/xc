# -*- coding: utf-8 -*-
''' Convenience functions to define damper materials.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import xc
import sys
from misc_utils import log_messages as lmsg
import uuid

def set_basic_attributes(damperMaterial, k, c, lgap= 0.0, nm= 1, relTol= 1e-6, absTol= 1e-10, maxHalf= 15):
    ''' Set the basic parameters of the given damper material.

    :param damperMaterial: material that will receive the properties.
    :param k: Elastic stiffness of linear spring to model the axial flexibility of a viscous damper (e.g. combined stiffness of the supporting brace and internal damper portion). 
    :param c: Damping coefficient.
    :param lgap: Gap length to simulate the gap length due to the pin tolerance.
    :param nm: Employed adaptive numerical algorithm (default value NM = 1; 1 = Dormand-Prince54, 2=6th order Adams-Bashforth-Moulton, 3=modified Rosenbrock Triple).
    :param relTol: Tolerance for absolute relative error control of the adaptive iterative algorithm (default value 10^-6).
    :param absTol: Tolerance for absolute error control of adaptive iterative algorithm (default value 10^-10).
    :param maxHalf: Maximum number of sub-step iterations within an integration step (default value 15).
    
    '''
    damperMaterial.setElasticStiffness(k)
    damperMaterial.setDampingCoeff(c)
    damperMaterial.setGapLength(lgap)
    damperMaterial.setNumericalAlgorithm(nm)
    damperMaterial.setRelativeTolerance(relTol)
    damperMaterial.setAbsoluteTolerance(absTol)
    damperMaterial.setMaxIter(maxHalf)

def def_bilinear_oil_damper(modelSpace, name, k, c, fr= 1.0, p= 1.0, lgap= 0.0, nm= 1, relTol= 1e-6, absTol= 1e-10, maxHalf= 15):
    ''' Defines a FlatSliderSimple2d element.

    :param modelSpace: wrapper of the FE preprocessor.
    :param name: name of the new material (if None compute a suitable name).
    :param k: Elastic stiffness of linear spring to model the axial flexibility of a viscous damper (e.g. combined stiffness of the supporting brace and internal damper portion). 
    :param c: Damping coefficient.
    :param fr: Damper relief load (default=1.0, Damper property).
    :param p: Post-relief viscous damping coefficient ratio (default=1.0, linear oil damper).
    :param lgap: Gap length to simulate the gap length due to the pin tolerance.
    :param nm: Employed adaptive numerical algorithm (default value NM = 1; 1 = Dormand-Prince54, 2=6th order Adams-Bashforth-Moulton, 3=modified Rosenbrock Triple).
    :param relTol: Tolerance for absolute relative error control of the adaptive iterative algorithm (default value 10^-6).
    :param absTol: Tolerance for absolute error control of adaptive iterative algorithm (default value 10^-10).
    :param maxHalf: Maximum number of sub-step iterations within an integration step (default value 15).
    '''
    materialHandler= modelSpace.getMaterialHandler()
    matName= name
    if(not matName):
        matName= uuid.uuid1().hex
    retval= materialHandler.newMaterial("BilinearOilDamper", matName)
    set_basic_attributes(retval, k= k, c= c, lgap= lgap, nm= nm, relTol= relTol, absTol= absTol, maxHalf= maxHalf)
    retval.setDamperReliefLoad(fr)
    retval.setPostReliefViscousDamping(p)
    retval.setup() # Call after any change in the input parameters.
    return retval

def def_viscous_damper(modelSpace, name, k, c, alpha, lgap= 0.0, nm= 1, relTol= 1e-6, absTol= 1e-10, maxHalf= 15):
    ''' Defines a FlatSliderSimple2d element.

    :param modelSpace: wrapper of the FE preprocessor.
    :param name: name of the new material (if None compute a suitable name).
    :param k: Elastic stiffness of linear spring to model the axial flexibility of a viscous damper (e.g. combined stiffness of the supporting brace and internal damper portion). 
    :param c: Damping coefficient.
    :param alpha: Velocity exponent.
    :param lgap: Gap length to simulate the gap length due to the pin tolerance.
    :param nm: Employed adaptive numerical algorithm (default value NM = 1; 1 = Dormand-Prince54, 2=6th order Adams-Bashforth-Moulton, 3=modified Rosenbrock Triple).
    :param relTol: Tolerance for absolute relative error control of the adaptive iterative algorithm (default value 10^-6).
    :param absTol: Tolerance for absolute error control of adaptive iterative algorithm (default value 10^-10).
    :param maxHalf: Maximum number of sub-step iterations within an integration step (default value 15).
    '''
    materialHandler= modelSpace.getMaterialHandler()
    matName= name
    if(not matName):
        matName= uuid.uuid1().hex
    retval= materialHandler.newMaterial("ViscousDamper", matName)
    set_basic_attributes(retval, k= k, c= c, lgap= lgap, nm= nm, relTol= relTol, absTol= absTol, maxHalf= maxHalf)
    retval.setVelocityExponent(alpha)
    retval.setup() # Call after any change in the input parameters.
    return retval

