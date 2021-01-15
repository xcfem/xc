# -*- coding: utf-8 -*-
''' Pre-tensioned tendons.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

class StraightStrand(object):
    ''' Straight strand.

    :ivar strandSection: strand cross-section type.
    :ivar transfLength: prestressing transfer lenght.
    :ivar xInit: abscissa where the cable starts.
    :ivar xEnd: abscissa where the cable ends.
    '''
    def __init__(self, strandSection, transfLength, xInit, xEnd):
        ''' Constructor.

        :param strandSection: cable cross-section type.
        :param transfLength: prestressing transfer lenght.
        :param xInit: abscissa where the cable starts.
        :param xEnd: abscissa where the cable ends.
        '''
        self.strandSection= strandSection
        self.transfLenght= transfLength
        self.xInit= xInit
        self.xEnd= xEnd

    def getStressFactor(self, x):
        ''' Return the strand stress factor at point x.

        :param x: abscissa of the point for which the stress factor
                  will be returned.
        '''
        retval= 0.0
        xA= self.xInit+self.transfLenght
        xB= self.xEnd-self.transfLenght
        if(x>self.xInit):
            if(x<xA): # transference length
                retval= (x-self.xInit)/(xA-self.xInit)
            elif(x<xB):
                retval= 1.0
            elif(x<self.xEnd): # transference length
                retval= 1.0-(x-xB)/(self.xEnd-xB)
        return retval
    
    def getStress(self, effectiveStress, x):
        ''' Return the strand stress factor at point x.

        :param effectiveStress: stress of the strand out of the 
                                transference length.
        :param x: abscissa of the point for which the stress will be returned.
        '''
        return effectiveStress*self.getStressFactor(x)
