# -*- coding: utf-8 -*-
''' Base plate design according to document
    Base Plate and Anchor Rod Design Second Edition
    American Institute of Steel Construction, Inc.
'''
from __future__ import division
from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from postprocess.reports import common_formats as fmt
from misc_utils import log_messages as lmsg
from materials.astm_aisc import ASTM_materials

# Base plates under concentric axial compressive loads.

def getConcreteStrength(A, fc, phi= 0.65):
    ''' Return the bearing strength of the concrete
        surface according to expression in section 
        3.1 of the guide for A1==A2.

    :param A: concrete bearing area.
    :param fc: minimum concrete compressive strength.
    :param phi: resistance factor for bearing on 
                concrete (defaults to 0.65).
    '''
    return phi*0.85*fc*A

def computeBasePlateArea(Pu, fc, phi= 0.65):
    ''' Return the required base plate area according
        to expression in section 3.1 of the guide
        for A1==A2.

    :param Pu: axial load (LRFD).
    :param fc: minimum concrete compressive strength.
    :param phi: resistance factor for bearing on 
                concrete (defaults to 0.65).
    '''
    return Pu/(phi*0.85*fc)

def getDelta(steelShape):
    ''' Return the Deltadimension of the base plate according
        to expression in section 3.1 of the guide.

    :param steelShape: steel shape of the shaft.
    '''
    d= steelShape.get('h') # Overall depth of member.
    bf= steelShape.get('b') # Flange width.
    return (0.95*d-0.8*bf)/2.0
  
def getNDimension(steelShape, Areq):
    ''' Return the N dimension of the base plate according
        to expression in section 3.1 of the guide.

    :param steelShape: steel shape of the shaft.
    :param Areq: base plate required area
    '''
    d= steelShape.get('h') # Overall depth of member.
    bf= steelShape.get('b') # Flange width.
    delta= getDelta(steelShape)
    Nmin= max(math.sqrt(Areq),d)
    return Nmin+delta

def getBDimension(steelShape, N, Areq):
    ''' Return the B dimension of the base plate.

    :param steelShape: steel shape of the shaft.
    :param Areq: base plate required area
    '''
    B=Areq/N
    bf= steelShape.get('b') # Flange width.
    delta= getDelta(steelShape)
    B= max(B,bf+delta)
    return B
  
def computePlateThickness(steelShape, N, B, Pu, Pp, Fy, phi= 0.9):
    ''' Return the required thickness of the base plate 
        according to expression in section 3.1 of the guide.

    :param steelShape: steel shape of the shaft.
    :param N: dimension parallel to the web of the shaft.
    :param B: dimension parallel to the flange of the shaft.
    :param Pu: axial load (LRFD).
    :param Pp: bearing strength of the concrete area (LRFD).
    :param Fy: base plate steel yield stress
    :param phi: resistance factor for base plate steel (defaults to 0.9).
    '''
    d= steelShape.get('h') # Overall depth of member.
    bf= steelShape.get('b') # Flange width.
    m= (N-0.95*d)/2.0
    n= (B-0.8*bf)/2.0
    X= ((4.0*d*bf)/(d+bf)**2)*Pu/Pp
    lmbd= 2.0*math.sqrt(X)/(1+math.sqrt(1-X))
    lmbd= min(lmbd,1.0)
    lmbd_np= lmbd*math.sqrt(d*bf)/4.0
    l= max(m,n,lmbd_np)
    retval= l*math.sqrt(2.0*Pu/(phi*Fy*B*N))
    return retval

# Design for shear


  
class RectangularBasePlate(object):
    ''' Rectangular base plate.'''
    def __init__(self, N, B, t, steelShape, anchorGroup, fc, steel= ASTM_materials.A36):
        ''' Constructor.

        :param N: dimension parallel to the web of the shaft.
        :param B: dimension parallel to the flange of the shaft.
        :param t: thickness.
        :param steelShape: shape of the steel column supported
                           by the plate.
        :parma anchorGroup: anchor group.
        :param fc: minimum concrete compressive strength.
        :param steel: steel material.
        '''
        self.N= N
        self.B= B
        self.t= t
        self.steelShape= steelShape
        self.anchorGroup= anchorGroup
        self.steel= steel
        self.fc= fc

    def getArea(self):
        ''' Return the area of the base plate.'''
        return self.B*self.N

    def getConcreteStrength(self):
        ''' Return the strenght of the concrete surface.'''
        return getConcreteStrength(self.getArea(), self.fc)

    def getConcreteStrengthEfficiency(self, Pu):
        ''' Return the efficiency of the concrete strength.'''
        if(Pu>0): # concrete compressed.
            return Pu/self.getConcreteStrength()
        else:
            return 0.0

    def getAnchorLeverArm(self):
        ''' Return the lever arm of the anchors with
            respect to the shaft web.'''
        retval= 0.0
        for anchor in self.anchorGroup.anchors:
            retval+= abs(anchor.pos3d.x)
        retval/= len(self.anchorGroup.anchors)
        return retval

    def getRequiredThickness(self, Pu):
        ''' Return the required thickness.'''
        if(Pu>0): # base plate compressed
            Pp= self.getConcreteStrength()
            return computePlateThickness(self.steelShape, self.N, self.B, Pu, Pp, self.steel.fy)
        else: # rods in tension
            T_rod= -Pu/4.0 # Tensile load per anchor
            tw= self.steelShape.get('tw') # Web thickness
            anchorLeverArm= self.getAnchorLeverArm()
            M= T_rod*anchorLeverArm-tw/2.0
            b_eff= (anchorLeverArm-tw/2.0)*2
            return math.sqrt(M*4.0/(b_eff*0.9*self.steel.fy))

    def getThicknessEfficiency(self, Pu):
        ''' Return the thickness efficiency.'''
        return self.getRequiredThickness(Pu)/self.t

    def computeFrictionStrength(self,Pu, frictionCoef= 0.55, phi= 0.75):
        ''' Return the shear resistance of the base plate.

        :param fc: minimum concrete compressive strength.
        :param Pu: axial load (LRFD).
        :param frictionCoef: friction coefficient μ is 0.55 for steel on 
                             grout, and 0.7 for steel on concrete.
        :param phi: resistance factor for concrete on shear (defaults to 0.75).
        '''
        retval= phi*frictionCoef*Pu
        retval= min(retval, 0.2*self.fc*self.getArea())
        return retval
    
    def getShearEfficiency(self, Pu, Vu, n= 2):
        ''' Return the shear efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        :param Vu: shear load (LRFD).
        :param n: number of anchors that work for shear.
        '''
        anchorShearStrength= self.anchorGroup.anchors[0].getDesignShearStrength()
        shearStrength= n*anchorShearStrength
        if(Pu>0): # base plate compressed
            shearStrength+= self.computeFrictionStrength(Pu)
        return Vu/shearStrength
    
    def getPullOutEfficiency(self, Pu):
        ''' Return the pullout efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu<0): # tensile load
            anchorConcretePullOutStrength= self.anchorGroup.anchors[0].getDesignPulloutStrength(self.fc)
            T_rod= -Pu/4.0 # Tensile load per anchor
            retval= T_rod/anchorConcretePullOutStrength
        return retval
        
    def getBreakOutEfficiency(self, h_ef, Pu):
        ''' Return the breakout efficiency of the base
            plate.

        :param h_ef: depth of embedment.
        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu<0): # tensile load
            anchorConcreteBreakOutStrength= self.anchorGroup.getConcreteBreakOutStrength(h_ef, self.fc)
            retval= -Pu/anchorConcreteBreakOutStrength
        return retval
        
    def getWebStress(self, Pu):
        ''' Return the tension stress in the shaft web.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu<0): # tensile load
            T_rod= -Pu/4.0 # Tensile load per anchor
            tw= self.steelShape.get('tw') # Web thickness
            anchorLeverArm= self.getAnchorLeverArm()
            M= T_rod*anchorLeverArm-tw/2.0
            b_eff= (anchorLeverArm-tw/2.0)*2
            forcePerInch= T_rod/b_eff
            retval= 2.0*forcePerInch/tw
        return retval
    
    def getWebStressEfficiency(self, Pu):
        ''' Return the breakout efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu<0): # tensile load
            webStress= self.getWebStress(Pu)
            retval= webStress/(0.9*self.steelShape.steelType.fy)
        return retval

    def getRodTensileStrengthEfficiency(self,Pu):
        ''' Return the breakout efficiency of the base
            plate.

        :param Pu: axial load (LRFD).
        '''
        retval= 0.0
        if(Pu<0): # tensile load
            Rn= self.anchorGroup.anchors[0].getDesignTensileStrength()
            T_rod= -Pu/4.0 # Tensile load per anchor
            retval= T_rod/Rn
        return retval
    
