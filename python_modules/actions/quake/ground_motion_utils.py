# -*- coding: utf-8 -*-
'''Convenience functions to define ground motion loads.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

def uniform_excitation_from_simple_record(modelSpace, name, dof, inputFileName, dt, cod_ts, factor= 1.0, vel0= 0.0):
    ''' Create a new uniform excitation from the given file.

    :param modelSpace: wrapper of the FE problem preprocessor.
    :param name: name of the new load pattern.
    :param dof: degree of freedom in which the ground motion acts.
    :param inputFileName: name of the file containing the record (acceleration
                         values in a single column).
    :param dt: time step between the recorded accelerations.
    :param cod_ts: name of the time series.
    :param factor: constant factor to apply to the given accelerations
                   (optional, default= 1.0).
    :param vel0: initial velocity ((optional, default= 0.0).
    '''
    # Read the accelerations from the file.
    accelValues= list()
    with open(inputFileName, 'r') as f:
        for line in f:
            values= line.rstrip().split()
            for v in values:
                accelValues.append(float(v))
    size= len(accelValues)
    retval= modelSpace.newUniformExcitation(name= name, dof= dof, path= accelValues, dt= dt, cod_ts= cod_ts, factor= factor, vel0= vel0)
    return retval, size
