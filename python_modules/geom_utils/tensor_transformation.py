# -*- coding: utf-8 -*-
''' Tensor transformation between different coordinate systems.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.ortega.ort@gmail.com"

import math
import numpy as np
from numpy.linalg import multi_dot

def cylindrical_to_cartesian(tensorCylindrical, theta):
    ''' Get the cartesian components of the tensor argument

    :param tensorCylindrical: cylindrical components of the tensor expressed
                              as a numpy array.
    :param theta: theta coordinate of the tensor position.
    '''
    cosTheta= math.cos(theta)
    sinTheta= math.sin(theta)
    trfMatrix= np.array([[cosTheta, -sinTheta, 0.0],
                         [sinTheta, cosTheta, 0.0],
                         [0.0, 0.0, 1.0]])
    return multi_dot([trfMatrix, tensorCylindrical, trfMatrix.T])

def cartesian_to_cylindrical(tensorCartesian, pos):
    ''' Get the cylindrical components of the tensor argument

    :param tensorCartesian: cartesian components of the tensor expressed
                            as a numpy array.
    :param pos: cartesian coordinates of the tensor position.
    '''
    theta= math.atan2(pos.y, pos.x)
    cosTheta= math.cos(theta)
    sinTheta= math.sin(theta)
    trfMatrix= np.array([[cosTheta, sinTheta, 0.0],
                         [-sinTheta, cosTheta, 0.0],
                         [0.0, 0.0, 1.0]])
    return multi_dot([trfMatrix, tensorCartesian, trfMatrix.T])
    
