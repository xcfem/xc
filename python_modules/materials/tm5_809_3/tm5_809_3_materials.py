# -*- coding: utf-8 -*-
''' Masonry structural design according to TM 5-809-3.'''
from __future__ import division
from __future__ import print_function

import math
import collections
import bisect
import scipy.interpolate
from materials import concrete_base
from materials.aci import ACI_materials
from miscUtils import LogMessages as lmsg
from materials.sections.fiber_section import defSimpleRCSection

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

toPascal= ACI_materials.toPascal #Conversion from Pa to lb/inch2
fromPascal= ACI_materials.fromPascal #Conversion from lb/inch2 to Pa

class Mortar(object):
    ''' Mortar according to TM 5-809-3.

    :ivar fm: compressive strenght.
    '''
    def __init__(self,fm):
        '''
        Constructor.

        :param fm: compressive strength.
        '''
        self.fm= fm
    def Em(self):
        '''Modulus of elasticity.'''
        return 1000.0*self.fm
    def Ev(self):
        '''Elastic modulus.'''
        return 400.0*self.fm
    def fr(self):
        '''Modulus of rupture (see TM 5-809-3 page 5-4).
        '''
        fmlb_inch2= abs(self.fm*fromPascal) #Pa -> lb/inch2
        return 2.5*(math.sqrt(fmlb_inch2))*toPascal #lb/inch2 -> Pa

sMortar= Mortar(1350.0*toPascal)

class CMUWall(object):
    """ Masonry wall 

    :ivar thickness: nominal wall thickness.
    :ivar groutedCellsSpacing: spacing of grouted cells.
    """
    # Interpolation of the equivalent thickness
    xT= [6,8,10,12] # nominal wall thickness (inches)
    yT= [0.0,16.0,24.0,32.0,40.0,48.0,56.0,64.0,72.0,1e6] # spacing inches
    zT= [[5.62, 7.62, 9.62, 11.62],
        [3.7, 4.9, 5.98, 7.04],
        [3.13, 4.1, 4.91, 5.7],
        [2.85, 3.7, 4.37, 5.02],
        [2.68, 3.46, 4.05, 4.62],
        [2.57, 3.3, 3.83, 4.35],
        [2.49, 3.19, 3.67, 4.16],
        [2.42, 3.1, 3.56, 4.01],
        [2.38, 3.03, 3.47, 3.90],
        [2.00, 2.50, 2.75, 3.00]]
    fEquivalentThickness= scipy.interpolate.interp2d(xT,yT,zT)
    # Interpolation of the effective area
    xA= [6,8,10,12] # nominal wall thickness (inches)
    yA= [0.0,16.0,24.0,32.0,40.0,48.0,56.0,64.0,72.0,1e6] # spacing inches
    zA= [[68.0, 92.0, 116.0, 140.0],
       [44.0, 59.0, 72.0, 85.0],
       [38.0, 49.0, 59.0, 68.0],
       [34.0, 44.0, 52.0, 60.0],
       [32.0, 42.0, 48.0, 55.0],
       [31.0, 40.0, 46.0, 52.0],
       [30.0, 38.0, 44.0, 50.0],
       [29.0, 37.0, 43.0, 48.0],
       [28.0, 36.0, 42.0, 47.0],
       [24.0, 30.0, 33.0, 36.0]]
    fEffectiveArea= scipy.interpolate.interp2d(xA,yA,zA)
    # Interpolation of the gross moment of inertia
    xI= [6,8,10,12] # nominal wall thickness (inches)
    yI= [8.0, 16.0, 24.0, 32.0, 40.0, 48.0] # effective width
    zI= [[119.0, 296.0, 594.0, 1047.0],
        [204.0, 496.0, 959.0, 1640.0],
        [290.0, 702.0, 1337.0, 2260.0],
        [377.0, 907.0, 1714.0, 2879.0],
        [0.0, 1113.0, 2092.0, 3499.0],
        [0.0, 1319.0, 2470.0, 4119.0]] # gross moment of inertia in**4
    fInertia= scipy.interpolate.interp2d(xI,yI,zI)
    xM= xI # nominal wall thickness (inches)
    yM= yI # effective width
    zM= [[323.0, 593.0, 946.0, 1379.0],
        [554.0, 995.0, 1525.0, 2160.0],
        [791.0, 1409.0, 2126.0, 2976.0],
        [1027.0, 1822.0, 2727.0, 3792.0],
        [0.0, 2235.0, 3328.0, 4608.0],
        [0.0, 2648.0, 3929.0, 5424.0]]
    fMoment=  scipy.interpolate.interp2d(xM,yM,zM)
    def __init__(self,thickness, spacing, mortar= sMortar):
        '''
        Constructor.

        :param thickness: wall thickness.
        :param spacing: spacing of grouted cells.
        '''
        self.thickness= thickness
        self.groutedCellsSpacing= spacing
        self.mortar= mortar
    
    def getEquivalentWallThickness(self):
        ''' Return the equivalent wall thickness according to
            table 5-2 of TM 5-809-3.'''
        sp_inch= self.groutedCellsSpacing/0.0254
        th_inches= self.thickness/0.0254
        tol= 1e-3
        if(th_inches<=6.0):
            if(th_inches<6.0-1e3):
                lmsg.error('thickness: '+str(self.thickness)+' out of range (too thin).')
            else:
                th_inches= 6.0
        elif(th_inches>=12.0):
            if(th_inches>12.0+tol):
                lmsg.error('thickness: '+str(self.thickness)+' out of range (too thick).')
            else:
                th_inches= 12.0
        return self.fEquivalentThickness(th_inches,sp_inch)[0]*0.0254
    def getEffectiveArea(self):
        ''' Return the effective area according to
            table 5-3 of TM 5-809-3.'''
        sp_inch= self.groutedCellsSpacing/0.0254
        th_inches= self.thickness/0.0254
        tol= 1e-3
        if(th_inches<=6.0):
            if(th_inches<6.0-1e3):
                lmsg.error('thickness: '+str(self.thickness)+' out of range (too thin).')
            else:
                th_inches= 6.0
        elif(th_inches>=12.0):
            if(th_inches>12.0+tol):
                lmsg.error('thickness: '+str(self.thickness)+' out of range (too thick).')
            else:
                th_inches= 12.0
        return self.fEffectiveArea(th_inches,sp_inch)[0]*0.0254**2/0.3048 #in/ft->m2/m
    def getEffectiveWidth(self):
        '''Return the effective width of the fabric
           according to remark 2 on table 5-4
           of TM 5-809-3.'''
        retval= self.groutedCellsSpacing
        retval= min(6*self.thickness,retval)
        return min(retval,48.0*0.0254)
        
    def getGrossMomentOfInertia(self):
        '''Return the gross moment of inertia
           according to table 5-4 of TM 5-809-3.
        '''
        th_inches= self.thickness/0.0254
        b_inch= self.getEffectiveWidth()/0.0254
        return self.fInertia(th_inches,b_inch)[0]*0.0254**4 #in**4->m**4

    def getSectionModulus(self):
        ''' Return the section modulus
            (see TM 5-809-3 page 5-4).'''
        return  2.0*self.getGrossMomentOfInertia()/self.thickness
    def getCrackingMoment(self):
        '''Return the cracking moment of the fabric
           according to equation 5-7 of  TM 5-809-3.'''
        th_inches= self.thickness/0.0254
        b_inch= self.getEffectiveWidth()/0.0254
        return self.fMoment(th_inches,b_inch)[0]/8.85 #lbf.ft-> N.m
        

