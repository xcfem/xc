# -*- coding: utf-8 -*-
''' Design of hinges under general loading.

   Based on the article:
   Markic T, Kaufmann W.
   Modeling and design of concrete hinges under general loading. 
   Structural Concrete. 2023;24(4):5122–49. 
   https://doi.org/10.1002/suco.202201110
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
from misc_utils import log_messages as lmsg

class ConcreteHinge(object):
    ''' Concrete hinge design according to the article:

       Markic T, Kaufmann W.
       Modeling and design of concrete hinges under general loading. 
       Structural Concrete. 2023;24(4):5122–49. 
       https://doi.org/10.1002/suco.202201110

    :ivar b: block length.
    :ivar b1: throath length.
    :ivar b2: length of the block available for load dispersion.
    :ivar d: block width.
    :ivar d1: throath width.
    :ivar d2: width of the block available for load dispersion.
    :ivar ht: height of the hinge throat.
    :ivar hm: height of the hinge "mouth" (distance between the outer edges 
              of opposing blocks).
    '''

    def __init__(self, b, d, b1= None, d1= None, ht= None, hm= None):
        ''' Constructor.

        :param b: block length.
        :param b1: throath length.
        :param d: block width.
        :param d1: throath width.
        :param ht: height of the hinge throat.
        :param hm: height of the hinge "mouth" (distance between the outer 
                   edges of opposing blocks).
        '''
        self.b= b
        self.d= d
        if(d1 is not None):
            self.d1= d1
        else:
            self.d1= self.getMaxThroathWidth()
        if(b1 is not None):
            self.b1= b1
        else:
            self.b1= self.getMaxThroathLength()
        if(ht is not None): 
            self.ht= ht
        else:
            self.ht= self.getMaxThroathHeight()
        if(hm is not None): 
            self.hm= hm
        else:
            self.hm= self.getMaxMouthOpening()

    def getMaxThroathWidth(self):
        ''' Return the maximum value of the throath width according to the 
            figure 1 (d) of the article.
        '''
        return 0.3*self.d

    def getMaxThroatHeight(self):
        ''' Return the maximum value of the hinge throat height according to
            the figure 1 (d) of the article.
        '''
        return min(0.2*self.d1, .02) # assuming dimensions in meters.
    
    def getMaxMouthOpening(self):
        ''' Return the maximum value of the distance between the outer edges 
            of opposing blocks according to the figure 1 (d) of the article.
        '''
        return 0.1*self.d # tan(beta)= tan(hm/d) < 0.1
    
    def getMaxThroathLength(self):
        ''' Return the maximum value of the hinge throat length according to
            the figure 1 (d) of the article.
        '''
        min_bn= max(0.7*self.d1, .05) # assuming dimensions in meters.
        return self.b-2*min_bn
    
    def getGeometryEfficiency(self, threshold= 1.0, silent= False):
        ''' Check the hinge geometry according to expressions in the figure 1
            (d) of the article. Return a value lesser that one if the geometry
            conditions are fullfilled.

        :param threshold: value of the efficiency that must be exceeded to 
                          be considered an error.
        :param silent: if true, don't issue warning messages.
        '''
        max_d1= self.getMaxThroathWidth()
        cf_d1= self.d1/max_d1
        if(cf_d1>threshold and not silent):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= '; throath width d1= '+str(self.d1) + ' too big (greater than '+str(max_d1)+').'
            lmsg.warning(className+'.'+methodName+warnMsg)
        max_ht= self.getMaxThroatHeight()
        cf_ht= self.ht/max_ht
        if(cf_ht>threshold and not silent):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= '; throath height ht= '+str(self.ht) + ' too big (greater than '+str(max_ht)+').'
            lmsg.warning(className+'.'+methodName+warnMsg)
        max_hm= self.getMaxMouthOpening()
        cf_hm= self.hm/max_hm
        if(cf_hm>threshold and not silent):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= '; mouth opening hm= '+str(self.hm) + ' too big (greater than '+str(max_hm)+').'
            lmsg.warning(className+'.'+methodName+warnMsg)
        max_b1= self.getMaxThroathLength()
        cf_b1= self.b1/max_b1
        if(cf_b1>threshold and not silent):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= '; mouth opening b1= '+str(self.b1) + ' too big (greater than '+str(max_b1)+').'
            lmsg.warning(className+'.'+methodName+warnMsg)
        return max(cf_d1, cf_ht, cf_hm, cf_b1)
