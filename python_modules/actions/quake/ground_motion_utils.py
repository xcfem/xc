# -*- coding: utf-8 -*-
'''Convenience functions to define ground motion loads.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

def is_float(element: any) -> bool:
    ''' Return true if the given object can be converted to float.

    :param element: element to convert to float.
    '''
    #If you expect None to be passed:
    if element is None: 
        return False
    try:
        float(element)
        return True
    except ValueError:
        return False

def get_accelerations_from_csv_file(inputFileName, accelColumnNumber):
    ''' Get the acceleration history from a CSV file.

    :param inputFileName: name of the file containing the record (acceleration
                         values in a single column).
    :param accelColumnNumber: number of the column that contain the acceleration
                              data.
    '''
    # Read the accelerations from the file.
    retval= list()
    with open(inputFileName, 'r') as f:
        for line in f:
            values= line.rstrip().split(',')
            accel= values[accelColumnNumber]
            if(is_float(accel)):
                accel= float(values[accelColumnNumber])
                retval.append(accel)
    return retval

def get_accelerations_from_simple_record(inputFileName):
    ''' Get the accleration history from the given file.

    :param inputFileName: name of the file containing the record (acceleration
                         values in a single column).
    '''
    # Read the accelerations from the file.
    retval= list()
    with open(inputFileName, 'r') as f:
        for line in f:
            values= line.rstrip().split()
            for v in values:
                retval.append(float(v))
    return retval

def get_uniform_excitation_from_accel_values(modelSpace, name, dof, accelValues, dt, cod_ts, factor= 1.0, vel0= 0.0):
    ''' Create a new uniform excitation from the given file.

    :param modelSpace: wrapper of the FE problem preprocessor.
    :param name: name of the new load pattern.
    :param dof: degree of freedom in which the ground motion acts.
    :param accelValues: values of the acceleration during the motion.
    :param dt: time step between the recorded accelerations.
    :param cod_ts: name of the time series.
    :param factor: constant factor to apply to the given accelerations
                   (optional, default= 1.0).
    :param vel0: initial velocity ((optional, default= 0.0).
    '''
    size= len(accelValues)
    retval= modelSpace.newUniformExcitation(name= name, dof= dof, path= accelValues, dt= dt, cod_ts= cod_ts, factor= factor, vel0= vel0)
    return retval, size

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
    accelValues= get_accelerations_from_simple_record(inputFileName)
    size= len(accelValues)
    return get_uniform_excitation_from_accel_values(modelSpace= modelSpace, name= name, dof= dof, accelValues= accelValues, dt= dt, cod_ts= cod_ts, factor= factor, vel0= vel0)

def uniform_excitation_from_csv_file(modelSpace, name, dof, inputFileName, accelColumnNumber, dt, cod_ts, factor= 1.0, vel0= 0.0):
    ''' Create a new uniform excitation from the given file.

    :param modelSpace: wrapper of the FE problem preprocessor.
    :param name: name of the new load pattern.
    :param dof: degree of freedom in which the ground motion acts.
    :param inputFileName: name of the file containing the record (acceleration
                         values in a single column).
    :param accelColumnNumber: number of the column that contain the acceleration
                              data.
    :param dt: time step between the recorded accelerations.
    :param cod_ts: name of the time series.
    :param factor: constant factor to apply to the given accelerations
                   (optional, default= 1.0).
    :param vel0: initial velocity ((optional, default= 0.0).
    '''
    # Read the accelerations from the file.
    accelValues= get_accelerations_from_csv_file(inputFileName, accelColumnNumber)
    return get_uniform_excitation_from_accel_values(modelSpace= modelSpace, name= name, dof= dof, accelValues= accelValues, dt= dt, cod_ts= cod_ts, factor= factor, vel0= vel0)
