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
    retval.setSoilType(soilType)
    retval.setUltimateCapacity(pult)
    retval.setY50(Y50)
    retval.setDragResistanceFactor(Cd)
    retval.setDashPot(c)
    retval.initialize()
    return retval

def def_tzsimple1_material(preprocessor, matName, tzType:int, tult:float, z50:float, c:float= 0.0):
    ''' Create a TzSimple1 uniaxial material object. It is normally used to 
        simulate the load-transfer behavior (t-z curves) for the skin 
        friction/axial shear resistance of deep foundation piles

        -tzType= 1 Backbone of t-z curve approximates Reese & O’Neill (1987) 
                   relation.

        -tzType= 2 Backbone of t-z curve approximates Mosher (1984) relation.

    The “tult” argument is the ultimate capacity of the t-z material. Note 
    that “t” or “tult” are shear stresses [force per unit area of pile surface]
    in common design equations, but are both loads for this uniaxialMaterial
    [i.e., shear stress times the tributary area of the pile].

    The optional argument c is the viscous damping term (dashpot) on the 
    far-field (elastic) component of the displacement rate (velocity). This 
    argument defaults to zero. Nonzero c values are used to represent 
    radiation damping effects.

    :param preprocessor: pre-processor of the finite element problem.
    :param matName: name for the new material (if None: let the preprocessor
                    assign the name).
    :param tzType: = 1 for Reese & O’Neill relation; = 2 for Mosher relation.
                   See previous notes.
    :param tult: Ultimate capacity of the t-z material.
    :param z50: Displacement at which 50% of tult is mobilized in monotonic 
                loading.
    :param c: The viscous damping term (dashpot). (optional Default = 0.0).
    '''
    materials= preprocessor.getMaterialHandler
    retval= materials.newMaterial('tz_simple1', matName)
    retval.setTzType(tzType)
    retval.setUltimateCapacity(tult)
    retval.setZ50(z50)
    retval.setDashPot(c)
    retval.initialize()
    return retval
