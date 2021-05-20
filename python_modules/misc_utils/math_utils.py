# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2019 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

class LinearInterpolation(object):
    ''' Simple linear interpolation.

    :ivar x0: first point abscissa.
    :ivar y0: first point ordinate.
    :ivar x1: second point abscissa.
    :ivar y1: second point ordinate.
    '''
    def __init__(self,x0, y0, x1, y1):
        ''' Constructor.

        :param x0: first point abscissa.
        :param y0: first point ordinate.
        :param x1: second point abscissa.
        :param y1: second point ordinate.
        '''
        self.tang= (y1-y0)/(x1-x0)
        self.x0= x0; self.y0= y0
    def getY(self,x):
        ''' Return the linear interpolation
            result for x.

        :param x: abscissa.
        '''
        return self.tang*(x-self.x0)+self.y0 

