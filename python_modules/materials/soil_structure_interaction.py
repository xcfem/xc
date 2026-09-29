# -*- coding: utf-8 -*-
''' Soil-structure interaction materials.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

def def_pysimple1_material(preprocessor, matName, soilType:int, pult:float, Y50:float, Cd:float, c:float= 0.0):
    ''' Create a PySimple1 uniaxial material object.

        -soilType= 1 Backbone of p-y curve approximates Matlock (1970) soft 
                   clay relation.

        -soilType= 2 Backbone of p-y curve approximates API (1993) 
                   sand relation.

    The “p” or “pult” are distributed loads (force per length of pile) in 
    common design equations, but are both loads for this uniaxialMaterial 
    (i.e., distributed load times the tributary length of the pile).

    The viscous damping term (dashpot) on the far-field (elastic) component 
    of the displacement rate (velocity). Nonzero c values are used to 
    represent radiation damping effects. See theory below.

    In general the Hilber-Hughes-Taylor Method algorithm is preferred over a 
    Newmark Method algorithm when using this material. This is due to the 
    numerical oscillations that can develop with viscous damping forces 
    under transient loading with certain solution algorithms and damping ratios.

    :param preprocessor: pre-processor of the finite element problem.
    :param matName: name for the new material (if None: let the preprocessor
                    assign the name).
    :param soilType: = 1 for clay; = 2 for sand. see previous notes.
    :param pult: Ultimate capacity of the p-y material.
    :param Y50: Displacement at which 50% of pult is mobilized in monotonic 
                loading.
    :param Cd: To set the drag resistance within a fully-mobilized gap 
               as Cd*pult.
    :param c: The viscous damping term (dashpot). (optional Default = 0.0).
    '''
    materials= preprocessor.getMaterialHandler
    retval= materials.newMaterial('py_simple1', matName)
    retval.soilType= soilType
    retval.ultimateCapacity= pult
    retval.y50= Y50
    retval.dragResistanceFactor= Cd
    retval.dashPot= c
    retval.initialize()
    return retval
