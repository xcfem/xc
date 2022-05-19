# -*- coding: utf-8 -*-
# Base for beam formulas

from __future__ import division

import numpy as np
import scipy.integrate


class Beam(object):
    ''' Base class for beam formulas.

    :ivar E: elastic modulus.
    :ivar I: moment of inertia of the beam section.
    :ivar l: beam length.
    '''
    n= 10 # number of divisions for integration
    def __init__(self,E:float= 1.0, I:float = 1.0, l:float= 1.0):
        ''' Constructor.

        :param E: elastic modulus.
        :param I: moment of inertia of the beam section.
        :param l: beam length.
        '''
        self.E= E
        self.I= I
        self.l= l

    def EI(self):
        ''' Return the tangent stiffness.'''
        return self.E*self.I

    def samplePoints(self, x0, x1):
        ''' Return a list of sample points.

        :param x0: interval beginning.
        :param x1: interval end.
        :param n: number of divisions.
        '''
        xi= x0; step= (x1-x0)/self.n
        retval= list()
        for i in range(0, self.n+1):
            retval.append(xi)
            xi+= step
        return np.array(retval)
    
    def computeCurvature(self, xi, M):
        ''' Compute curvature values.

        :param xi: abcissae.
        :param M: bending moment function.
        '''
        retval= list()
        ei= self.EI()
        for x in xi:
            retval.append(-M(x)/ei)
        return retval
    
    def computeIncTheta(self, x0, x1, M):
        ''' Compute increment of rotation between x0 and x1.

        :param x0: interval beginning.
        :param x1: interval end.
        :param M: bending moment function.
        '''
        x= self.samplePoints(x0= x0, x1= x1)
        y= self.computeCurvature(xi= x, M=M)
        return scipy.integrate.trapz(y, x)
    
    def computeIncThetaValues(self, xi, M):
        ''' Compute the increment of rotation values in sample points.

        :param xi: sample points.
        :param M: bending moment function.
        '''
        retval= [0.0]
        xA= xi[0]
        for x in xi[1:]:
            incTheta= self.computeIncTheta(x0= xA, x1= x, M= M)
            retval.append(incTheta)
            xA= x
        return retval
    
    def computeThetaValues(self, xi, M):
        ''' Compute the increment of rotation values in sample points.

        :param xi: sample points.
        :param M: bending moment function.
        '''
        tmp= self.computeIncThetaValues(xi, M)
        return (np.add.accumulate(tmp))
    
    def computeIncZeta(self, x0, x1, M):
        ''' Compute increment of deflection between x0 and x1.

        :param x0: interval beginning.
        :param x1: interval end.
        :param M: bending moment function.
        '''
        xi= self.samplePoints(x0, x1)    
        first_integral= [0.0]
        xA= xi[0]
        total= 0.0
        for x in xi[1:]:
            total-= self.computeIncTheta(xA, x, M= M)
            first_integral.append(total)
            xA= x
        return scipy.integrate.trapz(first_integral, xi)
    
    def computeIncZetaValues(self, xi, M):
        ''' Compute increment of deflection between x0 and x1.

        :param xi: sample points.
        :param M: bending moment function.
        :param EI: bending stiffness.
        '''
        incZeta= [0.0]
        xA= xi[0]
        for x in xi[1:]:
            incZeta.append(self.computeIncZeta(x0= xA, x1= x, M= M))
        return incZeta

    def computeZetaValues(self, xi, M):
        ''' Compute the increment of rotation values in sample points.

        :param xi: sample points.
        :param M: bending moment function.
        '''
        tmp= self.computeIncZetaValues(xi, M)
        return (np.add.accumulate(tmp))
