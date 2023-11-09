# -*- coding: utf-8 -*-
''' Two pile cap design according to clause 58.4.1.2.1 of EHE-08.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math

#
#                  N/2   N/2
#                   |    |
#                   v    v
#                   +----+
#                  /     \
#                /        \
#       Strut  /           \  Strut
#            /              \
#          /      Tie        \
#        +--------------------+
#     Pile                   Pile
#

class TwoPileCap(object):
    ''' Cap for a group of two piles.

    :ivar pileDiameter: Diameter of the pile.
    :ivar mainReinfDiameter: diameter of the main reinforcement.
    :ivar h: depth of the pile cap.
    :ivar l: length of the pile cap.
    :ivar b: width of the pile cap.
    :ivar d: distance between piles.
    :ivar a: width of the supported column.
    :ivar corbelLength: lenght of the corbel (minimum distance from the 
                        pile contour to the pile cap contour).
    '''

    def __init__(self, pileDiameter, mainReinfDiameter, reinfSteel, h= None, b= None, l= None, d= None, a= 0.0, corbelLength= 0.25):
        ''' Constructor.

        :param pileDiameter: diameter of the pile.
        :param mainReinfDiameter: diameter of the main reinforcement.
        :param reinfSteel: reinforcing steel material.
        :param h: depth of the pile cap.
        :param b: width of the pile cap.
        :param l: length of the pile cap.
        :param d: distance between piles.
        :param a: width of the supported column.
        :param corbelLength: lenght of the corbel (minimum distance from the 
                             pile contour to the pile cap contour).
        '''
        self.pileDiameter= pileDiameter
        self.mainReinfDiameter= mainReinfDiameter
        self.reinfSteel= reinfSteel
        if(h is None):
            self.h= self.getSuitableDepth()
        else:
            self.h= h
        if(b is None):
            self.b= self.getSuitableWidth()
        else:
            self.b= b
        if(l is None):
            self.l= self.getSuitableLength()
        else:
            self.l= l
        if(d is None):
            self.d= self.getSuitableDistanceBetweenPiles()
        else:
            self.d= d
        self.a= a
        self.corbelLength= corbelLength

    def getSuitableDistanceBetweenPiles(self):
        ''' Return a suitable value for the distance between piles.'''
        if(self.pileDiameter<0.5):
            return 2.0*self.pileDiameter
        else:
            return 3.0*self.pileDiameter

    def getSuitableDepth(self):
        ''' Return a suitable value for the depth of the pile cap.'''
        retval= 15*self.mainReinfDiameter**2+0.2
        retval= max(retval, self.pileDiameter)
        retval= max(retval, 0.4)
        return retval

    def getSuitableLength(self):
        ''' Return a suitable value for the length of the pile cap.'''
        return self.d+2*self.pileDiameter+2*self.corbelLength
    
    def getSuitableWidth(self):
        ''' Return a suitable value for the width of the pile cap.'''
        return self.pileDiameter+2*self.corbelLength

    def getEffectiveDepth(self):
        ''' Return the effective depth of the pile cap.'''
        return self.h-0.25

    def getAlpha(self):
        ''' Return the angle between the concrete compressed struts and 
            the horizontal according to figure 58.4.1.2.1.1.a of EHE-08.'''
        v= (self.d-self.a)/2.0
        x= v+0.25*self.a
        y= 0.85*self.getEffectiveDepth()
        return math.atan2(y, x)

    def getTensionOnBottomReinforcement(self, Nd):
        ''' Returns the tension in the inferior reinforcement of the pile cap.

        :param Nd: design value of the axial load.
        '''
        return Nd/math.tan(self.getAlpha())/2.0
  

    def getBottomReinforcementReqArea(self, Nd):
        ''' Return the required area for the main reinforcement.

        :param Nd: design value of the axial load.
        '''
        return self.getTensionOnBottomReinforcement(Nd)/self.reinfSteel.fyd()
    
    def getTopReinforcementReqArea(self, Nd):
        ''' Return the required area for the main reinforcement according
            to clause 58.4.1.2.1.2 of EHE-08.

        :param Nd: design value of the axial load.
        '''
        return 0.1*self.getBottomReinforcementReqArea(Nd)
  

    def getShearReinforcementReqArea(self):
        '''
        Devuelve el área de reinforcement necesaria para los cercos
        verticales de un encepado de DOS pilotes (ver números gordos
        HC.9 page 33).
        '''
        return 4*min(self.b,self.h/2.0)*self.l/1000.0
  
    def getHorizontalStirrupsReqArea(self):
        ''' Return the area of the required horizontal reinforcement.
        (ver números gordos HC.9 page 33).'''
        return 4*self.h*min(self.b,self.h/2.0)/1000.0
  
