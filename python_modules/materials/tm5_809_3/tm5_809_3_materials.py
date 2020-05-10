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
from misc_utils import log_messages as lmsg

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

toPascal= ACI_materials.toPascal #Conversion from lb/inch2 to Pa 
fromPascal= ACI_materials.fromPascal #Conversion from Pa to lb/inch2
inch2Meter= 0.0254 # Conversion from inch to meter
feet2Meter= 0.3048 # Conversion from feet to meter

class Mortar(object):
    ''' Mortar according to TM 5-809-3.

    :ivar fm: compressive strength.
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

    def Fm(self):
        '''Return the allowable flexural compressive stress in the masonry.'''
        return 0.33*self.fm

sMortar= Mortar(1350.0*toPascal)
nMortar= Mortar(1000.0*toPascal)

class CMUWallCellReinforcement(object):
    """ Masonry wall cell reinforcement

    :ivar steelType: steel type.
    :ivar reinfArea: area of reinforcement on each cell.
    :ivar nBars: number of bars per cell.
    """
    def __init__(self, steelType= ACI_materials.A615G60, reinfArea= 0.0, nBars= 1):
        '''
        Constructor.

        :param steelType: steel type.
        :param area: area of reinforcement on each cell.
        '''
        self.steelType= steelType
        self.area= reinfArea
        self.nBars= nBars
    def Fs(self):
        '''Return allowable working stress according to
           table 5-8 of TM 5-809-3.'''
        return 24000.0*toPascal

class CMUWallFabric(object):
    """ Masonry wall 

    :ivar thickness: nominal wall thickness.
    :ivar groutedCellsSpacing: spacing of grouted cells.
    :ivar mortar: type of mortar.
    :ivar cellReinf: reinforcement on each cell.
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
    # Cracking moment strength
    xM= xI # nominal wall thickness (inches)
    yM= yI # effective width
    zM= [[323.0, 593.0, 946.0, 1379.0],
        [554.0, 995.0, 1525.0, 2160.0],
        [791.0, 1409.0, 2126.0, 2976.0],
        [1027.0, 1822.0, 2727.0, 3792.0],
        [0.0, 2235.0, 3328.0, 4608.0],
        [0.0, 2648.0, 3929.0, 5424.0]]
    fMoment=  scipy.interpolate.interp2d(xM,yM,zM)
    # Weigth of CMU walls (pounds per square foot)
    xW= [6,8,10,12] # nominal wall thickness (inches)
    yW= [0.0,16.0,24.0,32.0,40.0,48.0,56.0,64.0,72.0,1e6] # spacing inches
    zW= [[68.0, 92.0, 116.0, 140.0],
         [58.0, 75.0, 92.0, 111.0],
         [53.0, 69.0, 85.0, 102.0],
         [51.0, 65.0, 78.0, 93.0],
         [50.0, 62.0, 75.0, 89.0],
         [49.0, 60.0, 72.0, 85.0],
         [48.0, 58.0, 70.0, 83.0],
         [47.0, 57.0, 69.0, 81.0],
         [46.0, 56.0, 68.0, 80.0],
         [43.0, 50.0, 59.0, 69.0]]
    fWeight=  scipy.interpolate.interp2d(xW,yW,zW)
    # depth of the masonry element effective in resisting the
    # out-of-plane shear (table 5.1 of TM 5-809-3).
    xD1= [6,8,10,12]
    yD1= [2.81,3.81,4.81,5.81]
    fD1= scipy.interpolate.interp1d(xD1,yD1)
    xD2= xD1
    yD2= [2.81,5.31,7.06,8.81]
    fD2= scipy.interpolate.interp1d(xD2,yD2)
    def __init__(self,thickness, spacing, mortar= sMortar, cellReinf= None):
        '''
        Constructor.

        :param thickness: wall thickness.
        :param spacing: spacing of grouted cells.
        :param mortar: type of mortar.
        :param cellReinf: reinforcement on each cell.
        '''
        self.thickness= thickness
        if(spacing!= 0.0):
            self.groutedCellsSpacing= spacing
        else:
            lmsg.warning('cell spacing cannot be zero set to 1E-6.')
            self.groutedCellsSpacing= 1e-6#thickness
        self.mortar= mortar
        if(cellReinf):
            self.cellReinf= cellReinf
        else:
            self.cellReinf= None
    
    def getEquivalentWallThickness(self):
        ''' Return the equivalent wall thickness according to
            table 5-2 of TM 5-809-3.'''
        sp_inch= self.groutedCellsSpacing/inch2Meter
        th_inches= self.thickness/inch2Meter
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
        return self.fEquivalentThickness(th_inches,sp_inch)[0]*inch2Meter
    
    def getEffectiveAreaPerUnitLength(self):
        ''' Return the effective area per unit length according to
            table 5-3 of TM 5-809-3.'''
        sp_inch= self.groutedCellsSpacing/inch2Meter
        th_inches= self.thickness/inch2Meter
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
        retval= float(self.fEffectiveArea(th_inches,sp_inch)[0]) #in2/ft
        retval/= feet2Meter #in2/m
        retval*= (inch2Meter**2) #in2/m->m2/m
        return retval
    
    def getEffectiveArea(self):
        ''' Return the effective area.'''
        return self.getEffectiveAreaPerUnitLength()*self.groutedCellsSpacing

    def getBw(self):
        '''Return the width of the masonry element effective in 
           resisting out-of-plane shear as shown in figure
           5-2c and given in table 5-1 of TM 5-809-3.'''
        return 7.5*inch2Meter
    def getEffectiveWidth(self):
        '''Return the effective width of the fabric
           according to remark 2 on table 5-4
           of TM 5-809-3.'''
        retval= self.groutedCellsSpacing
        retval= min(6*self.thickness,retval)
        return min(retval,48.0*inch2Meter)
        
    def getGrossMomentOfInertia(self):
        '''Return the gross moment of inertia
           according to table 5-4 of TM 5-809-3.
        '''
        th_inches= self.thickness/inch2Meter
        b_inch= self.getEffectiveWidth()/inch2Meter
        return self.fInertia(th_inches,b_inch)[0]*inch2Meter**4 #in**4->m**4

    def getSectionModulus(self):
        ''' Return the section modulus
            (see TM 5-809-3 page 5-4).'''
        return  2.0*self.getGrossMomentOfInertia()/self.thickness
    def getCrackingMomentStrength(self):
        '''Return the cracking moment strength of the fabric
           according to equation 5-7 of  TM 5-809-3.'''
        th_inches= self.thickness/inch2Meter
        b_inch= self.getEffectiveWidth()/inch2Meter
        return self.fMoment(th_inches,b_inch)[0]/8.85 #lbf.ft-> N.m
    
    def getMassPerSquareMeter(self):
        '''Return the mass of the fabric per square meter
           according to table 5-5 of TM 5-809-3.'''
        th_inches= self.thickness/inch2Meter
        b_inch= self.getEffectiveWidth()/inch2Meter
        return self.fWeight(th_inches,b_inch)[0]*4.88242764 #pounds/sqft-> kg/m2

    def getEffectiveDepth(self):
        ''' Return the total depth from the compression face to 
            the reinforcing steel.'''
        return self.getShearEffectiveDepth()

    def getShearEffectiveDepth(self):
        '''Return the depth of the masonry element effective in 
           resisting the out-of-plane shear according to table 5.1 
           of TM 5-809-3.'''
        retval= 0.0
        th_inches= self.thickness/inch2Meter
        if(self.cellReinf):
            if(self.cellReinf.nBars==1): # one bar per cell
                retval= self.fD1(th_inches)
            else: # two bar per cell
                retval= self.fD2(th_inches)
        return retval*inch2Meter
    
    def getSteelRatio(self):
        '''Return the steel ratio according to equation 5-8
           of TM 5-809-3.'''
        retval= 0.0
        d= self.getEffectiveDepth()
        if(self.cellReinf):
            retval= self.cellReinf.area/d/self.getEffectiveWidth()
        else:
            lmsg.warning('reinforcement not defined.')
        return retval

    def get_n(self):
        '''Return the ratio between the elasctic modulus of
           the reinforcing steel and the mortar.'''
        retval= 0.0
        if(self.cellReinf):
            retval= self.cellReinf.steelType.Es/self.mortar.Em()
        else:
            lmsg.warning('reinforcement not defined.')
        return retval

    # Flexural design rectangular section according to
    # section 5-4 of TM 5-809-3.
    def getKCoefficient(self):
        '''Return the ratio of the depth of the compressive stress 
           block to the total depth from the compression face to 
           the reinforcing steel: d, according to equation 5-9
           of TM 5-809-3.'''
        retval= 0.0
        if(self.cellReinf):
            np= self.get_n()*self.getSteelRatio()
            retval= math.sqrt(np**2+2*np)-np
        else:
            lmsg.warning('reinforcement not defined.')
        return retval

    def getBalancedSteelRatio(self):
        '''Return the balanced steel ratio defined as the reinforcing 
           ratio where the steel and the masonry reach their maximum 
           allowable stresses for the same applied moment, according
           to equation 5-11 of TM 5-809-3.
        '''
        retval= 0.0
        Fs= self.cellReinf.Fs()
        Fm= self.mortar.Fm()
        r= Fs/Fm
        if(self.cellReinf):
            retval= self.get_n()/(2*r*(n+r))
        else:
            lmsg.warning('reinforcement not defined.')
        return retval

    def getJCoefficient(self):
        '''Return the ratio of the distance between the resultant 
           compressive force and the centroid of the tensile force 
           to the distance d, according to equation 5-9
           of TM 5-809-3.'''
        retval= 0.0
        if(self.cellReinf):
            retval= 1.0 - self.getKCoefficient()/3.0
        else:
            lmsg.warning('reinforcement not defined.')
        return retval
    
    def getReinforcementResistingMoment(self):
        '''Return the resisting moment for the reinforcement
           according to equation 5-14 of TM 5-809-3.
        '''
        retval= 0.0
        if(self.cellReinf):
            j= self.getJCoefficient()
            d= self.getEffectiveDepth()
            Fs= self.cellReinf.Fs()
            retval= Fs*self.cellReinf.area*j*d #(N.m)
        else:
            lmsg.warning('reinforcement not defined.')
        return retval
    
    def getReinforcementResistingMomentPerUnitLength(self):
        '''Return the resisting moment for the reinforcement
           per unit length.
        '''
        return self.getReinforcementResistingMoment()/self.groutedCellsSpacing
    
    def getMasonryResistingMoment(self):
        '''Return the resisting moment for the masonry
           according to equation 5-15 of TM 5-809-3.

        '''
        retval= 0.0
        Fm= self.mortar.Fm()
        if(self.cellReinf):
            k= self.getKCoefficient()
            j= self.getJCoefficient()
            b= self.getEffectiveWidth()
            d= self.getEffectiveDepth()
            retval= Fm*k*j*b*d**2/2.0 #(N.m)
        else:
            lmsg.warning('reinforcement not defined.')
        return retval

    def getMasonryResistingMomentPerUnitLength(self):
        '''Return the resisting moment for the masonry
           per unit length.
        '''
        return self.getMasonryResistingMoment()/self.groutedCellsSpacing
    
    def getResistingMoment(self):
        '''Return the resisting moment of the masonry fabric.'''
        return min(self.getMasonryResistingMoment(),self.getReinforcementResistingMoment())
    def getResistingMomentPerUnitLength(self):
        '''Return the resisting moment of the masonry fabric
           per unit length.'''
        return self.getResistingMoment()/self.groutedCellsSpacing

    # Design for axial compression (section 5-4 c)
    def getCompressiveStress(self,P):
        '''Return the compressive stress in the masonry
           according to equation 5-21 of TM 5-809-3.

        :param P: axial load (N/m)
        '''
        return P/self.getEffectiveAreaPerUnitLength()
    
    # Design for shear (section 5-4 d)
    def getShearStress(self, V):
        '''Return the out-of-plane shear stress according to equation
        5-22 of TM 5-809-3.

        :param V: out-of-plane shear load
        '''
        return V/self.getBw()/self.getShearEffectiveDepth()
    
    def getInPlaneShearStress(self, V):
        '''Return the in-plane shear stress according to equation
        5-23 of TM 5-809-3.

        :param V: in-plane shear load (N/m)
        '''
        return V/self.getEffectiveAreaPerUnitLength()

    # Allowable stresses.
    def getStressReductionFactor(self,h):
        '''Return the stress reduction factor due to buckling according 
           to equation 5-25 of TM 5-809-3.

        The stress reduction factor (R) limits the axial stress on the wall so 
        that buckling  will not occur. When analyzing the top or bottom of
        a wall, where buckling is not a concern, the stress reduction factor 
        should not be used.

        :param h: clear height of the wall. None if buckling is not
                  a concern.
        '''
        return (1-(h/40.0/self.thickness)**3) # eq 5-25.
        
    def getAllowableAxialStress(self,h= None):
        '''Return the allowable axial strees according to equation 
           5-24 of TM 5-809-3.

        The stress reduction factor (R) limits the axial stress on the wall so 
        that buckling  will not occur. When analyzing the top or bottom of
        a wall, where buckling is not a concern, the stress reduction factor 
        should not be used.

        :param h: clear height of the wall. None if buckling is not
                  a concern.
        '''
        retval= 0.2*self.mortar.fm
        if(h):
            retval*= self.getStressReductionFactor(h) # eq 5-25 (stress reduction factor)
        return retval

    def getResistingAxialForce(self,h= None):
        '''Return the resisting axial force of the masonry fabric.

        :param h: clear height of the wall. None if buckling is not
                  a concern.
        '''
        return self.getAllowableAxialStress(h)*self.getEffectiveArea()
    
    def getResistingAxialForcePerUnitLength(self,h= None):
        '''Return the resisting axial force of the masonry fabric
           per unit length.

        :param h: clear height of the wall. None if buckling is not
                  a concern.
        '''
        return self.getResistingAxialForce(h)/self.groutedCellsSpacing

    def getCapacityFactor(self,axialLoad,bendingMoment, h= None):
        ''' Return the capacity factor of the masonry fabric under
            the internal forces being passed as parameter.

        :param axialLoad: axial load (N/m).
        :param bendingMoment: bending moment (N.m/m).
        :param h: clear height of the wall. None if buckling is not
                  a concern.
        '''
        Mu= self.getResistingMomentPerUnitLength()
        Fu= self.getResistingAxialForcePerUnitLength(h)
        return axialLoad/Fu+bendingMoment/Mu



