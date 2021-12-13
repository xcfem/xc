# -*- coding: utf-8 -*-
'''Earthquake loads according to NCSP-07 (Spain).'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2021, A_OO   LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import math
import sys
import numpy
from matplotlib import pyplot

class HorizontalElasticResponseSpectrum(object):
    ''' Definition of the horlzontal cOlnponents of the seismic action
        according to clause 3.5.1.1 of NSSP-07.

    :ivar nu: damping correction factor.
    :ivar ac: design ground acceleration.
    :ivar Ta: lower limit of the period of the constant spectral acceleration branch.
    :ivar Tb: upper linlit of the period of the constant spectral acceleration branch.
    :ivar Tc: value defining the beginning of the constant displacement response range of the spectrum.
    :ivar uls: if true compute the values corresponding to the design 
                values of the seismic action, otherwise compute the values 
                corresponding to the analysis of the serviceabilty limit 
                states. 
    '''
    def __init__(self, zeta: float, ac: float, K:float, C:float, uls:bool= True):
        ''' Constructor.

        :param zeta: damping index according to clause 4.2.3.3 of NCSP-07.
        :param ac: design ground acceleration.
        :param uls: if true compute the values corresponding to the design 
                    values of the seismic action, otherwise compute the values 
                    corresponding to the analysis of the serviceabilty limit 
                    states. 
        '''
        if((zeta<.01) or (zeta>1.0)):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; damping index: '"+str(zeta)+"' out of range: (0.01,1.0).\n")
        self.zeta= zeta
        self.ac= ac
        self.uls= uls
        self.computeDampingCorrectionFactor()
        self.computePeriodValues(K, C)

    def computeDampingCorrectionFactor(self):
        ''' Compute the value of the damping correction factor according to
            expression 3.6 o NCSP-07.
        '''
        self.nu= math.pow(.05/self.zeta,0.4)

    def computePeriodValues(self, K:float, C:float):
        ''' Compute the values of the periods Ta, Tb, Tc which define the
            response spectrum according to table 3.2 of NCSP-07.

        :param C: soil factor depending on the soil characteristics (clause 3.2 
              of NCSP-07.)
        :param K: contribution factior accordint to figure 3.1 and annex 1 of
                  NCSP-07.
        '''
        if(self.uls):
            self.Ta= K*C/10.0
            self.Tb= K*C/2.5
            self.Tc= K*(2+C)
        else:
            self.Ta= K*C/20.0
            self.Tb= K*C/5.0
            self.Tc= K*(1+0.5*C)

    def eval(self, T: float):
        ''' Return the value of the elastic response spectrum.

        :param T: vibration period of a linear single-degree-of-freedom system.
        '''
        retval= self.ac
        if(T<=self.Ta):
            retval*= (1+T/self.Ta*(2.5*self.nu-1)) # expr. 3.5a
        elif(T<=self.Tb):
            retval*= 2.5*self.nu # expr. 3.5b
        elif(T<=self.Tc):
            retval*= 2.5*self.nu*self.Tb/T # expr. 
        else:
            retval*= 2.5*self.nu*self.Tb*self.Tc/T**2
        return retval

    def plot(self):
        ''' Get a graphic representation of the response spectrum.
        '''
        # assigning period values
        finalT= 2.0*self.Tc
        step= finalT/50.0
        Ti= [0.0, self.Ta, self.Tb]
        for ti in numpy.arange(self.Tb, self.Tc, step):
            Ti.append(float(ti))
        for ti in numpy.arange(self.Tc, finalT, step):
            Ti.append(float(ti))

        # getting the spectrum
        sa= []
        for ti in Ti:
            s= self.eval(ti)
            sa.append(s)

        # plotting the signal 
        pyplot.plot(Ti, sa, color ='blue')

        pyplot.xlabel('$T\ (s)$')
        pyplot.ylabel('$S_a(T)\ (m/s^2)$')
        pyplot.title("Elastic response spectrum")

        pyplot.show()        


        
      
        

