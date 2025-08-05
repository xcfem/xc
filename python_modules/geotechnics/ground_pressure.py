# -*- coding: utf-8 -*-
'''ground_pressure.py: force exerted per unit area by a foundation (like a footing or a foundation slabe) onto the ground's surface.'''

from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
import geom
from misc_utils import log_messages as lmsg

def compute_ground_forces(soilSprings):
    ''' Compute the forces exerted by the ground on the foundation.

    :param soilSprings: springs used to simulate the soil-structure interaction
                        (like in a Winkler model).
    :returns: A dictionary contaning the ground force and the reference to each 
              spring supported node.
    '''
    retval= dict()
    for nTag in soilSprings:
        e= soilSprings[nTag]
        n= e.getNodes[1]
        rf= xc.Vector(e.getResistingForce()) # Store a copy not a reference.
        retval[nTag]= (rf, n)
    return retval

def compute_ground_pressure(groundForces, tributaryAreas, foundationCentroid):
    ''' Compute the foundation pressures over the soil. Calculates pressures 
        in the free nodes of the springs (those that belong to both
        the spring and the foundation) and stores these values as properties of         those nodes:
        property 'soilPressure:' [xStress,yStress,zStress]
        property 'soilReaction:' [xForce,yForce,zForce]

    :param groundForces: dictionary contaning the ground force and the 
                         position of each spring supported node.
    :param tributaryAreas: dictionary containing the tributary areas 
                           corresponding to the free nodes of the springs in the
                           elements of the foundation.
    :param foundationCentroid: centroid of the foundation.
    :returns: A sliding vector system equivalent to the forces exerted by the 
              ground on the foundation and referred to the centroid of the
              foundation.
    '''
    retval= geom.SlidingVectorsSystem3d()
    for nTag in groundForces:
        (rf, n)= groundForces[nTag]
        a= tributaryAreas[nTag]
        if(len(rf)==6):
            f3d= geom.Vector3d(rf[0],rf[1],0.0)
            m3d= geom.Vector3d(0.0,0.0,rf[2])
        else: #len(rf)==12
            f3d= geom.Vector3d(rf[0],rf[1],rf[2])
            m3d= geom.Vector3d(rf[3],rf[4],rf[5])
        pos= n.getInitialPos3d
        retval+= geom.SlidingVectorsSystem3d(pos,f3d,m3d)
        n.setProp('soilPressure',[f3d.x/a,f3d.y/a,f3d.z/a])
        n.setProp('soilReaction',[f3d.x,f3d.y,f3d.z])

    return retval.reduceTo(foundationCentroid)

def compute_ground_pressure_from_springs(soilSprings, tributaryAreas, foundationCentroid):
    ''' Compute the foundation pressures over the soil. Calculates pressures 
        and forces in the free nodes of the springs (those that belong to both
        the spring and the foundation) and stores these values as properties of         those nodes:
        property 'soilPressure:' [xStress,yStress,zStress]
        property 'soilReaction:' [xForce,yForce,zForce]

    :param soilSprings: springs used to simulate the soil-structure interaction
                        (like in a Winkler model).
    :param tributaryAreas: dictionary containing the tributary areas 
                           corresponding to the free nodes of the springs in the
                           elements of the foundation.
    :param foundationCentroid: centroid of the foundation.
    :returns: A sliding vector system equivalent to the forces exerted by the 
              ground on the foundation and referred to the centroid of the
              foundation.
    '''
    retval= geom.SlidingVectorsSystem3d()
    groundForces= compute_ground_forces(soilSprings)
    return compute_ground_pressure(groundForces= groundForces, tributaryAreas= tributaryAreas, foundationCentroid= foundationCentroid)

def compute_elastic_foundations_ground_pressure(elasticFoundations):
    ''' Compute the foundation pressures over the soil. Calculates pressures 
        and forces in the free nodes of the springs (those that belong to both
        the spring and the foundation) and stores these values as properties of         those nodes:
        property 'soilPressure:' [xStress,yStress,zStress]
        property 'soilReaction:' [xForce,yForce,zForce]

    :param elasticFoundations: list of elastic foundatins to compute the
                               pressures on the ground.
    '''
    groundForces= dict()
    tributaryAreas= dict()
    totalArea= 0.0
    org= geom.Pos3d(0,0,0)
    vectorPos= geom.Vector3d(0,0,0)
    retval= geom.SlidingVectorsSystem3d()
    for ef in elasticFoundations:
        ef_id= id(ef)
        efGroundForces= compute_ground_forces(ef.springs)
        for nTag in efGroundForces:
            ef_gf= efGroundForces[nTag]
            if not (nTag in groundForces):
                groundForces[nTag]= ef_gf
            else:
                prev_rf= groundForces[nTag][0]
                new_rf= ef_gf[0]
                node= ef_gf[1]
                groundForces[nTag]= (prev_rf+new_rf, node)
        efArea= 0.0
        for nTag in ef.tributaryAreas:
            efTributaryArea= ef.tributaryAreas[nTag]
            efArea+= efTributaryArea
            if not nTag in tributaryAreas:
                tributaryAreas[nTag]= efTributaryArea
            else:
                tributaryAreas[nTag]+= efTributaryArea
                
        totalArea+= efArea
        vectorPos+= efArea*(ef.getCentroid()-org)
    vectorPos*=(1.0/totalArea)
    foundationCentroid= org+vectorPos
    return compute_ground_pressure(groundForces= groundForces, tributaryAreas= tributaryAreas, foundationCentroid= foundationCentroid)
    
    
