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
