# -*- coding: utf-8 -*-
''' Non-linear spring used to study the track-rail interaction (ballast/connection).'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from materials import typical_materials

def def_ballast_friction_material(preprocessor, u0, k, tributaryLength, name= None, smallSlopeFactor=1.01):
    ''' Create a ballast friction material.

    :param preprocessor: pre-processor of the finite element problem.
    :param u0: displacement between elastic and plastic zones.
    :param k: resistance of sleeper in ballast.
    :param tributaryLength: length of the rail that the spring represents.
    :param matName: name for the new material (if None: let the preprocessor assign the name).
    :param smallSlopeFactor: small factor to get a slope sligthly greater than zero in plastic range.
    '''
    # list of tuples defining the (strain, stress) or (displacement, force) points.)
    frictionStrainStressCurve=[(u0,k), (1,smallSlopeFactor*k)]
    retval= typical_materials.defMultiLinearMaterial(
        preprocessor=preprocessor,
        name= name,
        points=frictionStrainStressCurve)
    return retval
