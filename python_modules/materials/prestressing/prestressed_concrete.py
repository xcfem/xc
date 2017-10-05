# -*- coding: utf-8 -*-

'''Classes related to prestressed concrete 
'''

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import math
import numpy as np
from scipy import interpolate
from scipy.spatial import distance
import matplotlib.pyplot as plt

class PrestressTendon(object):
    '''Geometry and prestressing losses of a prestressing tendon

    :ivar lstRoughPnt: list of 3D-coordinate tuples defining the points from 
                       which to interpolate the cubic spline that will delineate
                       the tendon geometry. e.g. [(x1,y1,z1),(x2,y2,z2),...]
    :ivar roughCoordMtr: matrix 3*n_rough_ptos [[x1,x2, ..],[y1,y2,..],[z1,z2,..]] 

    '''
    def __init__(self,lstRoughPnts):
        self.lstRoughPnts=lstRoughPnts
        self.roughCoordMtr=np.array(lstRoughPnts).transpose()

    def pntsInterpTendon(self,nPntsFine,smoothness,kgrade=3):
        '''Generates a cubic spline (default) or a spline of grade kgrade interpolation from the rough points and 
        calculates its value and the value of its derivative in nPntsFine
        equispaced.
        '''
        tck, u = interpolate.splprep(self.roughCoordMtr, k=kgrade,s=smoothness)
        x_knots, y_knots,z_knots = interpolate.splev(np.linspace(0, 1, nPntsFine), tck,der=0)
        self.fineCoordMtr=np.array([x_knots, y_knots,z_knots])
        x_der, y_der,z_der = interpolate.splev(np.linspace(0, 1,nPntsFine), tck,der=1)
        self.fineDerivMtr=np.array([x_der, y_der,z_der])
        return

    def getLengthSequence(self):
        '''Return for each point in fineCoordMtr the distance to the preceding point
        '''
        lseq=[0]+[distance.euclidean((self.fineCoordMtr[0][i],self.fineCoordMtr[1][i],self.fineCoordMtr[2][i]),(self.fineCoordMtr[0][i+1],self.fineCoordMtr[1][i+1],self.fineCoordMtr[2][i+1])) for i in range(len(self.fineCoordMtr[0])-1)]
        return lseq

    def getCumLength(self):
        '''Return for each point in fineCoordMtr the cumulative lenght of tendon from its 
        starting point
        '''
        length_sequence=getLengthSequence()
        return np.cumsum(length_sequence)


    def getReverseCumLength(self):
        '''Return for each point in fineCoordMtr the cumulative lenght of tendon from its 
        ending point
        '''
        lseq_forw=getLengthSequence().pop(0)+[0]
        lcum_forw=np.cumsum(lseq_forw.reverse).reverse
        return lcum_forw


