#!/usr/bin/env python
''' Convenience functions to definde friction models.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import uuid

def def_coulomb_friction_model(preprocessor, name, mu):
    '''Constructs a Coulomb friction model.

    :param preprocessor: preprocessor of the finite element problem.
    :param name: name identifying the friction model (if None compute a 
                 suitable name)
    :param mu: friction coefficient.
    '''
    frictionModelHandler= preprocessor.getFrictionModelHandler
    fmodelName= name
    if(not fmodelName):
        fmodelName= uuid.uuid1().hex
    alreadyDefined= False
    if(frictionModelHandler.exists(fmodelName)):
        existingFmodel= frictionModelHandler.getFrictionModel(fmodelName)
        if(existingFmodel.tipo()=='XC::Coulomb'): # Same type.
            alreadyDefined= (existingFmodel.mu== mu)
    if(alreadyDefined):
        retval= existingFmodel
    else:
        retval= frictionModelHandler.newFrictionModel("Coulomb", fmodelName)
        retval.setMu(mu)
    return retval

def def_velocity_dependent_friction_model(preprocessor, name, muSlow, muFast, transRate):
    '''Constructs a VelDependent friction model.

    :param preprocessor: preprocessor of the finite element problem.
    :param name: name identifying the friction model (if None compute a 
                 suitable name)
    :param muSlow: friction coefficient at low velocity.
    :param muFast: friction coefficient at fast velocity.
    :param transRate: transition rate from low to high velocity.
    '''
    frictionModelHandler= preprocessor.getFrictionModelHandler
    fmodelName= name
    if(not fmodelName):
        fmodelName= uuid.uuid1().hex
    alreadyDefined= False
    if(frictionModelHandler.exists(fmodelName)):
        existingFmodel= frictionModelHandler.getFrictionModel(fmodelName)
        if(existingFmodel.tipo()=='XC::VelDependent'): # Same type.
            alreadyDefined= (existingFmodel.muSlow==muSlow) and (existingFmodel.muSlow==muFast) and (existingFmodel.transRate==transRate)
    if(alreadyDefined):
        retval= existingFmodel
    else:
        retval= frictionModelHandler.newFrictionModel("VelDependent", fmodelName)
        retval.setMuSlow(muSlow)
        retval.setMuFast(muFast)
        retval.setTransRate(transRate)
    return retval

def def_velocity_pressure_dependent_friction_model(preprocessor, name, muSlow, muFast, transRate, nominalContactArea, deltaMu, alpha):
    '''Constructs a VelDependent friction model.

    :param preprocessor: preprocessor of the finite element problem.
    :param name: name identifying the friction model (if None compute a 
                 suitable name)
    :param muSlow: friction coefficient at low velocity.
    :param muFast: friction coefficient at fast velocity.
    :param transRate: transition rate from low to high velocity.
    :param nominalContactArea: nominal contact area.
    :param deltaMu: deltaMu presure parameter.
    :param alpha: transition rate from low to high velocity.
    '''
    frictionModelHandler= preprocessor.getFrictionModelHandler
    fmodelName= name
    if(not fmodelName):
        fmodelName= uuid.uuid1().hex
    alreadyDefined= False
    if(frictionModelHandler.exists(fmodelName)):
        existingFmodel= frictionModelHandler.getFrictionModel(fmodelName)
        if(existingFmodel.tipo()=='XC::VelPressureDep'): # Same type.
            alreadyDefined= (existingFmodel.muSlow==muSlow) \
            and (existingFmodel.muSlow==muFast) \
            and (existingFmodel.transRate==transRate) \
            and (existingFmodel.A==nominalContactArea) \
            and (existingFmodel.deltaMu==deltaMu) \
            and (existingFmodel.alpha==alpha)
    if(alreadyDefined):
        retval= existingFmodel
    else:
        retval= frictionModelHandler.newFrictionModel("VelPressureDep", fmodelName)
        retval.setMuSlow(muSlow)
        retval.setMuFast(muFast)
        retval.setTransRate(transRate)
        retval.setNominalContactArea(nominalContactArea)
        retval.setDeltaMuPressureParameter(deltaMu)
        retval.setAlphaPressureParameter(alpha)
    return retval

def def_normal_force_dependent_friction_model(preprocessor, name, aSlow, nSlow, aFast, nFast, alpha0, alpha1, alpha2, maxMuFact):
    '''Constructs a Coulomb friction model.

    :param preprocessor: preprocessor of the finite element problem.
    :param name: name identifying the friction model (if None compute a 
                 suitable name)

    :param aSlow: constant for coefficient of friction at low velocity 
    :param nSlow: exponent for coefficient of friction at low velocity.
    :param aFast: constant for coefficient of friction at high velocity.
    :param nFast: exponent for coefficient of friction at high velocity. 
    :param alpha0: constant rate parameter coefficient.
    :param alpha1: linear rate parameter coefficient.
    :param alpha2: quadratic rate parameter coefficient. 
    :param maxMuFact: factor for determining the maximum coefficient of 
                      friction. This value prevents the friction coefficient
                      from exceeding an unrealistic maximum value when the 
                      normal force becomes very small. The maximum friction 
                      coefficient is determined from μFast, for 
                      example μ ≤ $maxMuFac*μFast. 
    '''
    frictionModelHandler= preprocessor.getFrictionModelHandler
    fmodelName= name
    if(not fmodelName):
        fmodelName= uuid.uuid1().hex
    alreadyDefined= False
    if(frictionModelHandler.exists(fmodelName)):
        existingFmodel= frictionModelHandler.getFrictionModel(fmodelName)
        if(existingFmodel.tipo()=='XC::VelNormalFrcDep'): # Same type.
            alreadyDefined= (existingFmodel.mu== mu) \
            and (existingFmodel.aSlow == aSlow) and (existingFmodel.nSlow == nSlow) \
            and (existingFmodel.aFast == aFast) and (existingFmodel.nFast == nFast) \
            and (existingFmodel.alpha0 == alpha0) and (existingFmodel.alpha1 == alpha1) \
            and (existingFmodel.alpha2 == alpha2) and (existingFmodel.maxMuFact == maxMuFact)
    if(alreadyDefined):
        retval= existingFmodel
    else:
        retval= frictionModelHandler.newFrictionModel("VelNormalFrcDep", fmodelName)
        retval.setMu(1e-6)
        retval.setASlow(aSlow)
        retval.setNSlow(nSlow)
        retval.setAFast(aFast)
        retval.setNFast(nFast)
        retval.setAlpha0(alpha0)
        retval.setAlpha1(alpha1)
        retval.setAlpha2(alpha2)
        retval.setMaxMuFact(maxMuFact)
        
    return retval

def def_velocity_dependent_multi_linear_friction_model(preprocessor, name, vf_points):
    '''Constructs a Coulomb friction model.

    :param preprocessor: preprocessor of the finite element problem.
    :param name: name identifying the friction model (if None compute a 
                 suitable name)
    :param vf_points: list of tuples (velocity, COF) that define the bearing 
                      behavior.
    '''
    frictionModelHandler= preprocessor.getFrictionModelHandler
    fmodelName= name
    if(not fmodelName):
        fmodelName= uuid.uuid1().hex
    alreadyDefined= False
    if(frictionModelHandler.exists(fmodelName)):
        existingFmodel= frictionModelHandler.get(fmodelName)
        if(existingFmodel.tipo()=='XC::VelDepMultiLinear'): # Same type.
            existing_vf_points= existingFmodel.getVelocityFrictionPoints()
            alreadyDefined= (existing_vf_points==vf_points)
    if(alreadyDefined):
        retval= existingFmodel
    else:
        retval= frictionModelHandler.newFrictionModel("VelDepMultiLinear", fmodelName)
        retval.setMu(1e-6)
        retval.setVelocityFrictionPoints(vf_points)
    return retval
