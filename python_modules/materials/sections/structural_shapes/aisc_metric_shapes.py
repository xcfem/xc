# -*- coding: utf-8 -*-
''' AISC's structural steel shapes (metric units).'''

from __future__ import print_function
from __future__ import division

# Section axis:

#    AISC            XC

#       ^ Y           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> X        | -> Z
#       |             |
#     -----         -----

# The axis used in AISC catalogue are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

import math
import aisc_shapes_dictionaries as shapes
import aisc_shapes_labels as labels
from materials.sections import structural_steel
from misc_utils import log_messages as lmsg

# Shear areas.

# In y axis: Ay = depth of the section* web thickness
# In z axis: Az = 2/3 * combined area of the flanges. 

# *************************************************************************
# AISC W profiles.
# *************************************************************************
for item in shapes.W:
    shape= shapes.W[item]
    shape['Avy']= shape['h']*shape['tw'] # depth of the section* web thickness
    shape['Avz']= 2/3.0*(2*shape['b']*shape['tf']) # 2/3 * combined area of the flanges. 
    shape['alpha']= shape['Avy']/shape['A']
    shape['G']= shape['E']/(2*(1+shape['nu']))
    shape['hi']= shape['h']-2*shape['tf']
    shape['AreaQz']= 2*shape['b']*shape['tf'] 
    shape['AreaQy']= shape['A']-shape['AreaQz']
W= shapes.W

def getShapePlasticAxialNoad(shape):
    ''' Return the plastic axial load of the shape.

    :param shape: structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    return shape.steelType.fy*shape.get('A')

def getShapePlasticMoment(shape, majorAxis= True):
    ''' Return the plastic moment of the shape.

    :param shape: structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    Fy= shape.steelType.fy
    if(majorAxis):
        retval= Fy*shape.get('Wzpl')
    else:
        retval= min(Fy*shape.get('Wypl'), 1.6*Fy*shape.get('Wyel')) # equation F6-1
    return retval

def getShapeCompactWebAndFlangeRatio(shape, majorAxis= True):
    ''' If web and flanges are compact according to table 4.1b of 
        AISC-360-16 return a value less than one.

    :param majorAxis: true if flexure about the major axis.
    '''
    return max(shape.compactWebRatio(majorAxis),shape.compactFlangeRatio(majorAxis))
    
def getUIShapeRts(shape, majorAxis= True):
    ''' Return the value of rts according to equation F2-7
        of AISC-360-16.

    :param shape: I or channel structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    if(not majorAxis):
        lmsg.error(__name__+': r_{ts} coefficient not implemented for minor axis.')
    Cw= shape.get('Cw') # Warping constant.
    Iy= shape.get('Iy') # Moment of inertia about minor axis.
    Sz= shape.get('Wzel') # Elastic section modulus about major axis.
    return math.sqrt(math.sqrt(Iy*Cw)/Sz)

def getUIShapeLambdaPFlange(shape):
    '''Return he limiting slenderness for a compact flange, 
       defined in Table B4.1b of AISC-360-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    return 0.38*math.sqrt(E/Fy) # Case 10 or case 13

def getUIShapeLambdaRFlange(shape):
    '''Return he limiting slenderness for a noncompact flange, 
       defined in Table B4.1b of AISC-360-16.'''
    E= shape.get('E')
    Fy= shape.steelType.fy
    return 1.0*math.sqrt(E/Fy) # Case 10 or case 13


def getUIShapeLp(shape, majorAxis= True):
    '''Return the limiting laterally unbraced length for the limit state
       of yielding according to equation F2-5 of AISC-360-16.

    :param shape: I or channel structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    if(not majorAxis):
        lmsg.error(__name__+': L_p not implemented for minor axis.')
    ry= shape.get('iy') # Radius of gyration about minor axis.
    E= shape.get('E') # Elastic modulus.
    Fy= shape.steelType.fy # specified minimum yield stress
    return 1.76*ry*math.sqrt(E/Fy)

def getUIShapeLr(shape, majorAxis= True):
    '''Return the limiting unbraced length for the limit state 
       of inelastic lateral-torsional buckling according to
       equation F2-6 of  AISC-360-16.

    :param shape: I or channel structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    if(not majorAxis):
        lmsg.error(__name__+': L_r not implemented for minor axis.')
    rts= shape.getRts()
    E= shape.get('E') # Elastic modulus.
    Fy= shape.steelType.fy # specified minimum yield stress
    Sz= shape.get('Wzel') # Elastic section modulus about major axis.
    h0= shape.get('ho') # Distance between the flange centroids
    J= shape.get('It') # Torsional moment of inertia
    c= shape.getCCoefficient()
    sqrt1= math.sqrt((J*c/Sz/h0)**2+6.76*(0.7*Fy/E)**2)
    sqrt2= math.sqrt((J*c/Sz/h0)+sqrt1)
    return 1.95*rts*E/0.7/Fy*sqrt2

def getUIShapeCriticalStress(shape, lateralUnbracedLength, Cb, majorAxis= True):
    '''Return the critical stress for the limit state 
       of lateral-torsional buckling according to
       equation F2-4 of  AISC-360-16.

    :param shape: I or channel structural shape.
    :param lateralUnbracedLength: length between points that are either 
                                  braced against lateral displacement of
                                  the compression flange or braced against 
                                  twist of the cross section.
    :param Cb: lateral-torsional buckling modification factor.
    :param majorAxis: true if flexure about the major axis.
    '''
    E= shape.get('E') # Elastic modulus.
    if(not majorAxis):
        b_tf= 2.0*shape.get('bSlendernessRatio') # see lambda expression in F6. 
        return 0.69*E/b_tf**2 # equation F6-4
    else:
        J= shape.get('It') # Torsional moment of inertia
        Sz= shape.get('Wzel') # Elastic section modulus about major axis.
        h0= shape.get('ho') # Distance between the flange centroids
        rts= shape.getRts()
        Lb_rts2= (lateralUnbracedLength/rts)**2
        J= shape.get('It') # Torsional moment of inertia
        c= shape.getCCoefficient()
        sqroot= math.sqrt(1+0.078*J*c/Sz/h0*Lb_rts2)
        return Cb*math.pi**2*E/Lb_rts2*sqroot

def getUIShapeNominalFlexuralStrength(shape, lateralUnbracedLength, Cb, majorAxis= True):
    ''' Return the nominal flexural strength of the member
        according to equations F2-1 to F2-3 or F3-1 to F3-2 of AISC-360-16.

    :param shape: I or channel structural shape.
    :param lateralUnbracedLength: length between points that are either 
                                  braced against lateral displacement of
                                  the compression flange or braced against 
                                  twist of the cross section.
    :param Cb: lateral-torsional buckling modification factor.
    :param majorAxis: true if flexure about the major axis.
    '''
    Mn= 0.0
    Mp= shape.getPlasticMoment(majorAxis) # plastic moment.
    if(not majorAxis): # section F6
        compactFlanges= shape.compactFlangeRatio(majorAxis)
        if(compactFlanges<=1.0): # flanges are compact.
            Mn= Mp # equation F6-1
        else: 
            slenderFlanges= shape.slenderFlangeRatio(majorAxis)
            if(slenderFlanges<=1.0): # flanges are noncompact -> equation F6-2 applies.
                lmbd= 2.0*shape.get('bSlendernessRatio') # see lambda expression in F6. 
                lmbd_pf= shape.getLambdaPFlange()
                lmbd_rf= shape.getLambdaRFlange()
                Mn= Mp-(Mp-0.7*Fy*Sy)*((lmbd-lmbd_pf)/(lmbd_rf-lmbd_pf)) # equation F6-2
            else: # slender flanges.
                Sy= shape.get('Wyel') # Elastic section modulus about minor axis.
                Fcr= shape.getCriticalStress(None, None, majorAxis)
                Mn= Fcr*Sy # equation F6-3
    else:
        compactRatio= shape.compactWebAndFlangeRatio(majorAxis)
        Fy= shape.steelType.fy # specified minimum yield stress
        Sz= shape.get('Wzel') # Elastic section modulus about major axis.
        if(compactRatio<=1.0): # flange and web are compact.
            Lb= lateralUnbracedLength
            Lp= shape.getLp(majorAxis)
            if(Lb<Lp): # equation F2-1 applies
                Mn= Mp
            else:
                Lr= shape.getLr(majorAxis)
                if(Lb<=Lr): # equation F2-2 applies
                    Mn= Cb*(Mp-(Mp-0.7*Fy*Sz)*((Lb-Lp)/(Lr-Lp)))
                    Mn= min(Mn,Mp)
                else: # equation F2-3 applies
                    Fcr= shape.getCriticalStress(Lb, Cb, majorAxis)
                    Mn= min(Fcr*Sz, Mp)
        else: # flange or web or both are not compact.
            compactWeb= shape.compactWebRatio(majorAxis)
            if(compactWeb<=1.0): # web is compact.
                slenderFlanges= shape.slenderFlangeRatio(majorAxis)
                lmbd= shape.get('bSlendernessRatio')
                if(slenderFlanges<=1.0): # flanges are noncompact -> equation F3-1 applies.
                    lmbd_pf= shape.getLambdaPFlange()
                    lmbd_rf= shape.getLambdaRFlange()
                    Mn= Mp-(Mp-0.7*Fy*Sz)*((lmbd-lmbd_pf)/(lmbd_rf-lmbd_pf)) # equation F3-1
                else: # flanges are slender -> equation F3-2 applies.
                    kc= max(min(4.0/math.sqrt(shape.get('hSlendernessRatio')),0.76),0.35)
                    Mn= 0.9*shape.get('E')*kc*Sz/lmbd**2 # equation F3-2
            else: # web is not compact. To implement from sections F4 and F5
                lmsg.error(__name__+': nominal flexural strength for noncompact web sections not implemented yet.')
    return Mn

def getUIAw(shape, majorAxis= True):
    ''' Return area for shear strength calculation.'''
    if(majorAxis):
        return shape.h()*shape.get('tw')
    else:
        return 2.0*shape.get('tf')*shape.get('b') # two resisting elements.

def getUIWebPlateShearBucklingCoefficient(shape, a= 1e6):
    '''Return the web plate shear buckling coefficient
       according to expression G2-5 of AISC-360-16.

    :param a: clear distance between transverse stiffeners
    '''
    h= shape.h()
    if(a/h>3.0):
        return 5.34
    else:
        denom= (a/h)**2
        return 5.0(1.0+1.0/denom)
    
def getG22WebShearBucklingStrengthCoefficient(shape, kv, majorAxis= True):
    ''' Return the web shear stress coefficient Cv2 according
        to section G2.2 of AISC-360-16.

    :param kv: web plate shear buckling coefficient.
    :param majorAxis: true if flexure about the major axis.
    '''
    Cv2= 1.0
    h_t= shape.get('hSlendernessRatio')
    if(not majorAxis):
        h_t= shape.get('bSlendernessRatio')
    E= shape.get('E')
    Fy= shape.steelType.fy
    sqrtkvE_Fy= math.sqrt(kv*E/Fy)
    h_t_threshold= 1.10*sqrtkvE_Fy
    if(h_t<=h_t_threshold):
        Cv2= 1.0 # equation G2-9
    else:
        h_t_threshold2= 1.37*sqrtkvE_Fy
        if(h_t<=h_t_threshold2):
            Cv2= h_t_threshold2/h_t # equation G2-10
        else:
            Cv2= 1.51*kv*E/h_t**2/Fy  # equation G2-11
    return Cv2
    
def getUINominalShearStrengthWithoutTensionFieldAction(shape, a= 1e6, majorAxis= True):
    ''' Return the nominal shear strength according to equation
        G2-1 of AISC-360-16.

    :param a: clear distance between transverse stiffeners.
    '''
    if(majorAxis):
        Cv1= shape.getWebShearStrengthCoefficient(a)
        return 0.6*shape.steelType.fy*shape.getAw()*Cv1 # equation G2-1
    else:
        Cv2= shape.getWebShearBucklingStrengthCoefficient(kv= 1.2, majorAxis= majorAxis)
        Fy= shape.steelType.fy
        return 0.6*Fy*shape.getAw(majorAxis)*Cv2 # eauation G6-1

class WShape(structural_steel.IShape):
    '''W shape

    :ivar steel: steel material.
    :ivar name: shape name (i.e. W40X431).
    '''
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        super(WShape,self).__init__(steel,name,W)

    def h(self):
        ''' Return overall depth of member (d in AISC tables).'''
        return self.get('h')
      
    def d(self):
        ''' Return internal web height: clear distance between flanges
        less the fillet at each flange (h in AISC tables).'''
        return self.get('d')

    def getLambdaPFlange(self):
        '''Return he limiting slenderness for a compact flange, 
           defined in Table B4.1b of AISC-360-16.'''
        return getUIShapeLambdaPFlange(self)

    def getLambdaRFlange(self):
        '''Return he limiting slenderness for a noncompact flange, 
           defined in Table B4.1b of AISC-360-16.'''
        return getUIShapeLambdaRFlange(self)

    def compactFlangeRatio(self, majorAxis= True):
        ''' If flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_p= self.getLambdaPFlange()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then flanges are compact.
    
    def slenderFlangeRatio(self, majorAxis= True):
        ''' If flanges are noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_r= self.getLambdaRFlange()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # if <1 then flanges are noncompact.

    def compactWebRatio(self, majorAxis= True):
        ''' If web is compact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error(__name__+': compact web ratio not implemented for minor axis.')
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_p= 3.76*math.sqrt(E/Fy) # Case 15
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then web is compact.

    def slenderWebRatio(self, majorAxis= True):
        ''' If web is noncompact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one, otherwise the web
            is slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error(__name__+': compact web ratio not implemented for minor axis.')
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_p= 5.70*math.sqrt(E/Fy) # Case 15
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then web is noncompact.
    
    def compactWebAndFlangeRatio(self, shape, majorAxis= True):
        ''' If web and flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapeCompactWebAndFlangeRatio(self, majorAxis)

    def slendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to table B4.1a of AISC-360-16.'''
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_r= 0.56*math.sqrt(E/Fy) # Case 1
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # OK if < 1.0

    def getAw(self, majorAxis= True):
        ''' Return area for shear strength calculation.'''
        return getUIAw(self, majorAxis)
            
    def getWebPlateShearBucklingCoefficient(shape, a= 1e6):
        '''Return the web plate shear buckling coefficient
           according to expression G2-5 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return getUIWebPlateShearBucklingCoefficient(self,a)
    
    def getWebShearStrengthCoefficient(self, a= 1e6):
        ''' Return the web shear stress coefficient Cv1 according
            to equations G2-2, G2-3 and G2-4 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        Cv1= 1.0
        h_tw= self.get('hSlendernessRatio')
        E_Fy= self.get('E')/self.steelType.fy
        h_tw_threshold= 2.24*math.sqrt(E_Fy)
        if(h_tw<=h_tw_threshold):
            Cv1= 1.0 # equation G2-2
        else:
            kv= self.getWebPlateShearBucklingCoefficient(a)
            h_tw_threshold2= 1.10*math.sqrt(kv*E_Fy)
            if(h_tw<=h_tw_threshold2):
                Cv1= 1.0 # equation G2-3
            else:
                Cv1= h_tw_threshold2/h_tw  # equation G2-4
        return Cv1
    
    def getWebShearBucklingStrengthCoefficient(self, kv, majorAxis= True):
        ''' Return the web shear stress coefficient Cv2 according
            to section G2.2 of AISC-360-16.

        :param kv: web plate shear buckling coefficient.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getG22WebShearBucklingStrengthCoefficient(self,kv,majorAxis)
    
    def getNominalShearStrengthWithoutTensionFieldAction(self, a= 1e6, majorAxis= True):
        ''' Return the nominal shear strength according to equation
            G2-1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return getUINominalShearStrengthWithoutTensionFieldAction(self,a, majorAxis)

    def getShearResistanceFactor(self):
        ''' Return the resistance factor for shear according to
            sections G1 and G2.1(a) of AISC-360-16.
        '''
        retval= 0.9 # see section G1.
        h_tw= self.get('hSlendernessRatio')
        E_Fy= self.get('E')/self.steelType.fy
        h_tw_threshold= 2.24*math.sqrt(E_Fy)
        if(h_tw<=h_tw_threshold):
            retval= 1.0 # see section G2.1(a)
        return retval

    def getDesignShearStrengthWithoutTensionFieldAction(self, a= 1e6, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''

        rf= 0.9
        if(majorAxis):
            rf= self.getShearResistanceFactor()
        return rf*self.getNominalShearStrengthWithoutTensionFieldAction(a, majorAxis)

    def getDesignShearStrength(self, a= 1e6, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return self.getDesignShearStrengthWithoutTensionFieldAction(a,majorAxis)

    def getTorsionalElasticBucklingStress(self, Lc):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2 of AISC-360-16.

        :param Lc: effective length of member for torsional buckling 
                   about longitudinal axis.
        '''
        E= self.get('E')
        G= self.get('G')
        Cw= self.get('Cw')
        J= self.get('It')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        return (math.pi**2*E*Cw/Lc**2+G*J)/(Iy+Iz)
    
    def getPlasticAxialLoad(self):
        ''' Return the plastic axial load of the section.'''
        return getShapePlasticAxialLoad(self)
    
    def getPlasticMoment(self, majorAxis= True):
        ''' Return the plastic moment of the section.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapePlasticMoment(self,majorAxis)

    def getCCoefficient(self, majorAxis= True):
        ''' Return the c coefficient according to equation
            F2-8a of AISC-360-16.'''
        return 1.0

    def getRts(self, majorAxis= True):
        ''' Return the value of rts according to equation F2-7
            of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeRts(self, majorAxis)

    def getLp(self, majorAxis= True):
        ''' Return the limiting laterally unbraced length for the 
            limit state of yielding according to
            equation F2-5 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeLp(self, majorAxis)
    
    def getLr(self, majorAxis= True):
        ''' Return the limiting unbraced length for the limit state 
            of inelastic lateral-torsional buckling according to
            equation F2-6 of  AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeLr(self, majorAxis)
    
    def getCriticalStress(self, lateralUnbracedLength, Cb, majorAxis= True):
        '''Return the critical stress for the limit state 
           of lateral-torsional buckling according to
           equation F2-4 of  AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        '''
        return getUIShapeCriticalStress(self, lateralUnbracedLength, Cb, majorAxis)
    def getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the nominal flexural strength of the member
            according to equations F2-1 to F2-3 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis)
    
    def getDesignFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the design flexural strength of the section
            according to section F1 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return 0.9*self.getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis)

 
# *************************************************************************
# AISC C profiles.
# *************************************************************************

for item in shapes.C:
    shape= shapes.C[item]
    shape['G']= shape['E']/(2*(1+shape['nu']))
    shape['hi']= shape['h']-2*shape['tf']
    shape['Avy']= shape['h']*shape['tw'] # depth of the section* web thickness
    shape['Avz']= 2/3.0*(2*shape['b']*shape['tf']) # 2/3 * combined area of the flanges. 
    shape['AreaQz']= 2*shape['b']*shape['tf']
    shape['AreaQy']= shape['A']-shape['AreaQz']
C= shapes.C

class CShape(structural_steel.UShape):
    '''C shape.

    :ivar steel: steel material.
    :ivar name: shape name (i.e. C15X50).
    '''
    def __init__(self,steel,name):
        ''' Constructor.

        '''
        super(CShape,self).__init__(steel,name,C)
        
    def getLambdaPFlange(self):
        '''Return he limiting slenderness for a compact flange, 
           defined in Table B4.1b of AISC-360-16.'''
        return getUIShapeLambdaPFlange(self)

    def getLambdaRFlange(self):
        '''Return he limiting slenderness for a noncompact flange, 
           defined in Table B4.1b of AISC-360-16.'''
        return getUIShapeLambdaRFlange(self)

    def compactFlangeRatio(self, majorAxis= True):
        ''' If flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_p= self.getLambdaPFlange()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then flanges are compact.
    
    def slenderFlangeRatio(self, majorAxis= True):
        ''' If flanges are noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_r= self.getLambdaRFlange()
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # if <1 then flanges are noncompact.
    
    def compactWebRatio(self, majorAxis= True):
        ''' If web is compact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error(__name__+': compact web ratio not implemented for minor axis.')
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_p= 3.76*math.sqrt(E/Fy) # Case 15
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then web is compact.

    def slenderWebRatio(self, majorAxis= True):
        ''' If web is noncompact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one, otherwise the web
            is slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error(__name__+': compact web ratio not implemented for minor axis.')
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_p= 5.70*math.sqrt(E/Fy) # Case 15
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then web is noncompact.
    
    def compactWebAndFlangeRatio(self, majorAxis= True):
        ''' If web and flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapeCompactWebAndFlangeRatio(self,majorAxis)
    
    def getAw(self, majorAxis= True):
        ''' Return area for shear strength calculation.'''
        return getUIAw(self, majorAxis)
    
    def getWebPlateShearBucklingCoefficient(shape, a= 1e6):
        '''Return the web plate shear buckling coefficient
           according to expression G2-5 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return getUIWebPlateShearBucklingCoefficient(shape,a)
    
    def getWebShearStrengthCoefficient(self, a= 1e6):
        ''' Return the web shear stress coefficient Cv1 according
            to equations G2-3 and G2-4 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        Cv1= 1.0
        h_tw= self.get('hSlendernessRatio')
        E_Fy= self.get('E')/self.steelType.fy
        kv= self.getWebPlateShearBucklingCoefficient(a)
        h_tw_threshold2= 1.10*math.sqrt(kv*E_Fy)
        if(h_tw<=h_tw_threshold2):
            Cv1= 1.0 # equation G2-3
        else:
            Cv1= h_tw_threshold2/h_tw  # equation G2-4
        return Cv1
    
    def getWebShearBucklingStrengthCoefficient(self, kv, majorAxis= True):
        ''' Return the web shear stress coefficient Cv2 according
            to section G2.2 of AISC-360-16.

        :param kv: web plate shear buckling coefficient.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getG22WebShearBucklingStrengthCoefficient(self,kv,majorAxis)

    def getNominalShearStrengthWithoutTensionFieldAction(self, a= 1e6 , majorAxis= True):
        ''' Return the nominal shear strength according to equation
            G2-1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getUINominalShearStrengthWithoutTensionFieldAction(self,a, majorAxis)
    
    def getDesignShearStrengthWithoutTensionFieldAction(self, a= 1e6, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        :param majorAxis: true if flexure about the major axis.
        '''

        return 0.9*self.getNominalShearStrengthWithoutTensionFieldAction(a, majorAxis)
    
    def getDesignShearStrength(self, a= 1e6, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''
        return self.getDesignShearStrengthWithoutTensionFieldAction(a,majorAxis)

    def slendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to table B4.1 a of AISC-360-16.'''
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_r= 0.56*math.sqrt(E/Fy) # Case 1
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # OK if < 1.0
    
    def getTorsionalElasticBucklingStress(self, Lc):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-7 of AISC-360-16.

        :param Lc: effective length of member for torsional buckling 
                   about longitudinal axis.
        '''
        Ag= self.get('A')
        E= self.get('E')
        G= self.get('G')
        Cw= self.get('Cw')
        J= self.get('It')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        z0= self.get('x')-self.get('e0')
        # Polar radius of gyration about the shear center.
        r02= z0**2+(Iy+Iz)/Ag # E4-9
        return (math.pi**2*E*Cw/Lc**2+G*J)/(Ag*r02) # E4-7
    
    def getFlexuralConstant(self):
        ''' Return the flexural constant of the section according
            to equation E4-8 of AISC-360-16..'''
        Ag= self.get('A')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        z0= self.get('x')-self.get('e0')
        # Polar radius of gyration about the shear center.
        r02= z0**2+(Iy+Iz)/Ag # E4-9
        return 1-z0**2/r02
    
    def getCCoefficient(self, majorAxis= True):
        ''' Return the c coefficient according to equation
            F2-8b of AISC-360-16.'''
        if(not majorAxis):
            lmsg.error(__name__+': c coefficient not implemented for minor axis.')
        h0= self.get('ho') # Distance between the flange centroids
        Cw= self.get('Cw') # Warping constant.
        Iy= self.get('Iy') # Moment of inertia about minor axis.
        return h0/2.0*math.sqrt(Iy/Cw)

    def getPlasticAxialLoad(self):
        ''' Return the plastic axial load of the section.'''
        return getShapePlasticAxialLoad(self)
    
    def getPlasticMoment(self, majorAxis= True):
        ''' Return the plastic moment of the section.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapePlasticMoment(self,majorAxis)

    def getRts(self, majorAxis= True):
        ''' Return the squared value of rts according to equation F2-7
            of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeRts(self, majorAxis)

    def getLp(self, majorAxis= True):
        ''' Return the limiting laterally unbraced length for the 
            limit state of yielding according to
            equation F2-5 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeLp(self, majorAxis)
    
    def getLr(self, majorAxis= True):
        ''' Return the limiting unbraced length for the limit state 
            of inelastic lateral-torsional buckling according to
            equation F2-6 of  AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeLr(self, majorAxis)

    def getCriticalStress(self, lateralUnbracedLength, Cb, majorAxis= True):
        '''Return the critical stress for the limit state 
           of lateral-torsional buckling according to
           equation F2-4 of  AISC-360-16.

        :param majorAxis: true if flexure about the major axis.
        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        '''
        return getUIShapeCriticalStress(self, lateralUnbracedLength, Cb, majorAxis)

    def getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the nominal compressive strength of the member
            according to equations F2-1 to F2-3 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getUIShapeNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis)
    
    def getDesignFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the design flexural strength of the section
            according to section F1 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return 0.9*self.getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis)
    

# *************************************************************************
# AISC Hollow Structural Sections.
# *************************************************************************

for item in shapes.HSS:
    shape= shapes.HSS[item]
    shape['alpha']= 5/12.0
    shape['G']= shape['E']/(2*(1+shape['nu']))
    if('h_flat' in shape): # rectangular
        shape['Avy']= 2*0.7*shape['h_flat']*shape['t']
        shape['Avz']= 2*0.7*shape['b_flat']*shape['t']
    else: # circular
        tmp= math.pi*(shape['OD']-shape['t'])/2.0*shape['t']
        shape['Avy']= tmp
        shape['Avz']= tmp
    shape['AreaQz']= shape['Avz']
    shape['AreaQy']= shape['Avy']
    
HSS= shapes.HSS

class HSSShape(structural_steel.QHShape):
    ''' Hollow structural section.

    :ivar steel: steel material.
    :ivar name: shape name (i.e. HSS2X2X_250).
    '''
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        super(HSSShape,self).__init__(steel,name,HSS)

    def getLimitingWidthToThicknessRatio(self):
        ''' Return the Limiting Width-to-Thickness Ratio 
        according to table B4.1a of AISC-360-16.
        '''
        return 1.4*math.sqrt(self.steelType.E/self.steelType.fy)

    def getBClassification(self):
        ''' Return the classification for local buckling of the
            "b" wall of the section according to table B4.1a
            of AISC 360-16.
        '''
        retval= 'nonslender'
        bSlendernessRatio= self.get('bSlendernessRatio')
        lambda_r= self.getLimitingWidthToThicknessRatio()
        if(bSlendernessRatio>lambda_r):
            retval= 'slender'
        return retval
    
    def getHClassification(self):
        ''' Return the classification for local buckling of the
            "h" wall of the section according to table B4.1a
            of AISC 360-16.
        '''
        retval= 'nonslender'
        hSlendernessRatio= self.get('hSlendernessRatio')
        lambda_r= self.getLimitingWidthToThicknessRatio()
        if(hSlendernessRatio>lambda_r):
            retval= 'slender'
        return retval
    
    def getClassification(self):
        ''' Return the classification for local buckling of the
            section according to table B4.1a of AISC 360-16.
        '''
        retval= 'nonslender'
        if((self.getHClassification()=='slender') or (self.getBClassification()=='slender')):
            retval= 'slender'
        return retval
    def getLambdaP(self):
        ''' Return the limiting Width-to-Thickness Ratio (compact/noncompact)
            according to table B4.1b of AISC 360-16.
        '''
        return 1.12*math.sqrt(self.steelType.E/self.steelType.fy)
    
    def getLambdaR(self):
        ''' Return the limiting Width-to-Thickness Ratio (compact/noncompact)
            according to table B4.1b of AISC 360-16.
        '''
        return 1.40*math.sqrt(self.steelType.E/self.steelType.fy)
    
    def getBFlexureClassification(self):
        ''' Return the classification for local buckling of the
            "b" wall of the section according to table B4.1b
            of AISC 360-16.
        '''
        retval= 'compact'
        bSlendernessRatio= self.get('bSlendernessRatio')
        lambda_P= self.getLambdaP()
        lambda_R= self.getLambdaR()
        if(bSlendernessRatio>lambda_P):
            retval= 'noncompact'
        elif(bSlendernessRatio>lambda_R):
            retval= 'slender'
        return retval
    
    def getHFlexureClassification(self):
        ''' Return the classification for local buckling of the
            "h" wall of the section according to table B4.1b
            of AISC 360-16.
        '''
        retval= 'compact'
        hSlendernessRatio= self.get('hSlendernessRatio')
        lambda_P= self.getLambdaP()
        lambda_R= self.getLambdaR()
        if(hSlendernessRatio>lambda_P):
            retval= 'noncompact'
        elif(hSlendernessRatio>lambda_R):
            retval= 'slender'
        return retval

    # Bending
    def compactFlangeRatio(self, majorAxis= True):
        ''' If flanges are compact according to case
            17 (HSS sections subject to flexure) of
            table 4.1b of AISC-360-16 return a value 
            less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        retval= 10.0
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_p= 1.12*math.sqrt(E/Fy) # Case 17
        if(majorAxis):
            slendernessRatio= self.get('bSlendernessRatio')
        else:
            slendernessRatio= self.get('hSlendernessRatio')
        retval= slendernessRatio/lambda_p
        return retval # if <1 then flanges are compact.
    
    def slenderFlangeRatio(self, majorAxis= True):
        ''' If flanges are noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        retval= 10.0
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_r= 1.40*math.sqrt(E/Fy) # Case 17
        if(majorAxis):
            slendernessRatio= self.get('bSlendernessRatio')
        else:
            slendernessRatio= self.get('hSlendernessRatio')
        retval= slendernessRatio/lambda_r
        return retval # if <1 then flanges are compact.
    
    def compactWebRatio(self, majorAxis= True):
        ''' If webs are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        retval= 10.0
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_p= 2.42*math.sqrt(E/Fy) # Case 19
        if(majorAxis):
            slendernessRatio= self.get('bSlendernessRatio')
        else:
            slendernessRatio= self.get('hSlendernessRatio')
        retval= slendernessRatio/lambda_p
        return retval # if <1 then webs are compact.
    
    def slenderWebRatio(self, majorAxis= True):
        ''' If web is noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            it's slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        retval= 10.0
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_r= 5.7*math.sqrt(E/Fy) # Case 19
        if(majorAxis):
            slendernessRatio= self.get('bSlendernessRatio')
        else:
            slendernessRatio= self.get('hSlendernessRatio')
        retval= slendernessRatio/lambda_r
        return retval # if <1 then webs are noncompact.
    
    def compactWebAndFlangeRatio(self, majorAxis= True):
        ''' If web and flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapeCompactWebAndFlangeRatio(self,majorAxis)
    
    def getAw(self, majorAxis= True):
        ''' Return area for shear strength calculation.'''
        t= self.get('t')
        if(majorAxis): # see equation G4-1
            h= self.get('h_flat')
            return 2.0*h*t
        else: # see equation G6-1
            b= self.get('b_flat')
            return b*t

    
    def getWebShearBucklingStrengthCoefficient(self, kv, majorAxis= True):
        ''' Return the web shear stress coefficient Cv2 according
            to section G2.2 of AISC-360-16.

        :param kv: web plate shear buckling coefficient.
        :param majorAxis: true if flexure about the major axis.
        '''
        return getG22WebShearBucklingStrengthCoefficient(self,kv,majorAxis)
    
    def getNominalShearStrength(self, Lv= 1e6, majorAxis= True):
        ''' Return the nominal shear strength according to equations
            G4-1 and G6-1 of AISC-360-16.

        :param Lv: distance from maximum to zero shear force.
        :param majorAxis: true if flexure about the major axis.
        '''
        Fy= self.steelType.fy
        if(majorAxis):
            Cv2= self.getWebShearBucklingStrengthCoefficient(kv= 5, majorAxis= majorAxis)
            return 0.6*Fy*self.getAw(majorAxis)*Cv2 # equation G4-1
        else:
            Cv2= self.getWebShearBucklingStrengthCoefficient(kv= 1.2, majorAxis= majorAxis)
            return 0.6*Fy*self.getAw(majorAxis)*Cv2 # eauation G6-1


    def getDesignShearStrength(self, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param a: clear distance between transverse stiffeners.
        '''

        return 0.9*self.getNominalShearStrength(majorAxis)
    
    def slendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to table B4.1 a of AISC-360-16.'''
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_r= 1.4*math.sqrt(E/Fy)
        slendernessRatio= max(self.get('hSlendernessRatio'),self.get('bSlendernessRatio'))
        return slendernessRatio/lambda_r # OK if < 1.0
    
    def getTorsionalElasticBucklingStress(self, Lc):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2 of AISC-360-16.

        :param Lc: effective length of member for torsional buckling 
                   about longitudinal axis.
        '''
        E= self.get('E')
        G= self.get('G')
        Cw= self.get('C') #HSS torsional constant.
        J= self.get('It')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        return (math.pi**2*E*Cw/Lc**2+G*J)/(Iy+Iz)
    
    def getPlasticAxialLoad(self):
        ''' Return the plastic axial load of the section.'''
        return getShapePlasticAxialLoad(self)
    
    def getPlasticMoment(self, majorAxis= True):
        ''' Return the plastic moment of the section.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapePlasticMoment(self,majorAxis)

    def getReducedEffectiveH(self):
        '''Return the reduced effective width corresponding
           to the "h" walls of the shape.'''
        rt= math.sqrt(self.steelType.E/self.steelType.fy) #Simplification
        h_t= self.get('hSlendernessRatio')
        t= self.get('t')
        retval= 1.92*t*rt*(1.0-0.38/h_t*rt)
        retval= min(retval,self.get('h'))
        return retval
    
    def getReducedEffectiveB(self):
        '''Return the reduced effective width corresponding
           to the "b" walls of the shape.'''
        rt= math.sqrt(self.steelType.E/self.steelType.fy) #Simplification
        b_t= self.get('bSlendernessRatio')
        t= self.get('t')
        retval= 1.92*t*rt*(1.0-0.38/b_t*rt)
        retval= min(retval,self.get('b'))
        return retval
    
    def getEffectiveArea(self):
        '''Return the effective area.'''
        retval= self.get('A')
        clasif= self.getClassification()
        if(clasif == 'slender'):
            t= self.get('t')
            h_ineff= self.get('h_flat')-self.getReducedEffectiveH()
            retval-= 2.0*h_ineff*t
            b_ineff= self.get('b_flat')-self.getReducedEffectiveB()
            retval-= 2.0*b_ineff*t
        return retval

    def getEffectiveWidth(self, majorAxis= True):
        ''' Return the effective width of the flange for
            rectangular HSS sections according 
            to equation F7-4 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.'''
        E= self.get('E')
        Fy= self.steelType.fy
        root= math.sqrt(E/Fy)
        tf= self.get('t')
        b= self.get('b')
        if(not majorAxis):
            b= self.get('h')
        return min(1.92*tf*root*(1.0-0.38*root/(b/tf)),b)

    def getEffectiveInertia(self, majorAxis= True):
        ''' Return the effective moment of inertia for
            rectangular HSS sections due to 
            the "removed" area from the flange.

        :param majorAxis: true if flexure about the major axis.'''
        self.beff= self.getEffectiveWidth(majorAxis) # effective width
        b= self.get('b')
        if(not majorAxis):
            b= self.get('h')
        b_removed= b-self.beff # ineffective width of compression flange
        tf= self.get('t')
        A_removed= b_removed*tf # ineffective area of compression flange
        A= self.get('A')
        self.y_eff= (A*b/2.0-A_removed*(b-tf/2.0))/(A-A_removed) # new neutral axis measured from bottom of section.
        dna= (b/2.0)-self.y_eff # difference between old an new neutral axis.
        I_neg= 1/12.0*b_removed*tf**3 # moment of inertia of removed piece about ist own neutral axis.
        d_neg= b-self.y_eff-tf/2.0 # distance for new neutral axis to centroid of removed piece.
        I= self.get('Iz')
        if(not majorAxis):
            I= self.get('Iy')        
        return I+A*dna**2-I_neg-A_removed*d_neg**2

    
    def getEffectiveSectionModulus(self, majorAxis= True):
        ''' Return the effective section modulus determined 
            with the effective width of the compression angle
            calculated with equation F7-4 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.'''
        self.Ieff= self.getEffectiveInertia(majorAxis)
        b= self.get('b')
        if(not majorAxis):
            b= self.get('h')
        return self.Ieff/(b-self.y_eff)
                   
    def getFlangeLocalBucklingLimit(self, majorAxis= True):
        ''' Return the limit for nominal flexural strenght for square of
            rectangular HSS sections due to flange buckling according 
            to section F7.2 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.'''
        Mp= self.getPlasticMoment(majorAxis)
        Mn= 0.0 
        compactFlanges= self.compactFlangeRatio(majorAxis)
        if(compactFlanges<=1.0):
            Mn= Mp # equation F7-1
        else:
            Fy= self.steelType.fy
            slenderFlanges= self.slenderFlangeRatio(majorAxis)
            if(slenderFlanges<1.0): # flanges are noncompact.
                E= self.get('E')
                S= self.get('Wzel') # Elastic section modulus about major axis.
                lmbd= self.get('bSlendernessRatio') # major axis slenderness ratio.
                if(not majorAxis):
                    S= self.get('Wyel') # Elastic section modulus about minor axis.
                    lmbd= self.get('hSlendernessRatio') # minor axis slenderness ratio.
                Mn= min(Mp-(Mp-Fy*S)*(3.57*lmbd*math.sqrt(Fy/E)-4.0),Mp) # equation F7-2
            else:
                Se= self.getEffectiveSectionModulus(majorAxis)
                Mn= Se*Fy # equation F7-3
        return Mn
    
    def getWebLocalBucklingLimit(self, majorAxis= True):
        ''' Return the limit for nominal flexural strenght for square of
            rectangular HSS sections due to web buckling according 
            to section F7.3 of AISC-360-16.

        :param majorAxis: true if flexure about the major axis.'''
        Mp= self.getPlasticMoment(majorAxis)
        Mn= 0.0
        compactWebs= self.compactWebRatio(majorAxis)
        if(compactWebs<=1.0):
            Mn= Mp # equation F7-1
        else:
            Fy= self.steelType.fy
            slenderWebs= self.slenderWebRatio(majorAxis)
            if(slenderWebs<1.0): # webs are noncompact.
                E= self.get('E')
                S= self.get('Wzel') # Elastic section modulus about major axis.
                lmbd= self.get('bSlendernessRatio') # major axis slenderness ratio.
                if(not majorAxis):
                    S= self.get('Wyel') # Elastic section modulus about minor axis.
                    lmbd= self.get('hSlendernessRatio') # minor axis slenderness ratio.
                Mn= min(Mp-(Mp-Fy*S)*(0.305*lmbd*math.sqrt(Fy/E)-0.738),Mp) # equation F7-6
            else:
                lmsg.error(__name__+": something went wrong: there are no HSS with slender webs.")
                Mn= 0.0
        return Mn
    
    def getLimitingLaterallyUnbracedLengthForYielding(self, majorAxis= True):
        ''' Return the limiting laterally unbraced length 
            for the limit state of yielding according
            to expression F7-12 of AISC 360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        ry= self.get('iy')
        if(not majorAxis):
            ry= self.get('iz')
        retval= 0.13*self.steelType.E*ry
        retval*= math.sqrt(self.get('It')*self.get('A'))
        Mp= self.getPlasticMoment(majorAxis)
        retval/=Mp
        return retval
    
    def getLimitingLaterallyUnbracedLengthForInelasticBuckling(self, majorAxis= True):
        ''' Return the limiting laterally unbraced length 
            for the limit state of inelastic lateral torsional
            buckling according to expression F7-13 of AISC 360-16.

        :param majorAxis: true if flexure about the major axis.
        '''
        ry= self.get('iy')
        if(not majorAxis):
            ry= self.get('iz')
        retval= 2.0*self.steelType.E*ry
        retval*= math.sqrt(self.get('It')*self.get('A'))
        M= 0.7*self.steelType.fy*self.get('Wyel')
        retval/=M
        return retval
    
    def getLateralTorsionalBucklingLimit(self, Lb, Cb, majorAxis= True):
        ''' Return the maximum flexural strength
        due to lateral-torsional buckling according to
        expressions F7-10 to F7-11 of AISC 360-16

        :param Lb: Length between points that are either braced 
                   against lateral displacement of compression 
                   flange or braced against twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor Cb
                   for non uniform moment diagrams when both ends of the 
                   segment are braced according to expression 
                   F1-1 of AISC 360-16.
        '''
        Mp= self.getPlasticMoment(majorAxis)
        Lp= self.getLimitingLaterallyUnbracedLengthForYielding(majorAxis)
        retval= 0.0
        if(Lb<=Lp): #No lateral-torsional buckling.
            retval= Mp
        else: #Lb>Lp
            Lr= self.getLimitingLaterallyUnbracedLengthForInelasticBuckling(majorAxis)
            if(Lb<Lr): # equation F7-10
                Sx= self.get('Wyel')
                if(not majorAxis):
                    Sx= self.get('Wzel')
                retval= Mp-(Mp-0.7*self.steelType.fy*Sx)*(Lb-Lp)/(Lr-Lp) 
                retval*= Cb
                retval= min(Mp,retval)
            else: #Lb>Lp  equation F7-11
                retval= 2.0*self.steelType.E*Cb
                retval*= math.sqrt(self.get('It')*self.get('A'))
                ry= self.get('iy')
                if(not majorAxis):
                    ry= self.get('iz')
                retval/= (Lb/ry)
        return retval

    def getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the nominal flexural strength of the member
            according to equations F7-1 to F7-13 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        Mp= self.getPlasticMoment(majorAxis)
        Mn= 0.0

        compactSection= self.compactWebAndFlangeRatio(majorAxis)
        if(compactSection<=1.0):
            Mn= Mp # equation F7-1
        else:
            Mn= Mp
            Mnf= self.getFlangeLocalBucklingLimit(majorAxis)
            Mn= min(Mn,Mnf)
            Mnw= self.getWebLocalBucklingLimit(majorAxis)                
            Mn= min(Mn,Mnw)
            Mlt= self.getLateralTorsionalBucklingLimit(lateralUnbracedLength, Cb, majorAxis)
            Mn= min(Mn,Mlt)
        return Mn
    
    def getDesignFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the design flexural strength of the section
            according to section F1 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return 0.9*self.getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis)

for item in shapes.CHSS:
    shape= shapes.CHSS[item]
    shape['alpha']= 5/12.0
    shape['G']= shape['E']/(2*(1+shape['nu']))
    tmp= math.pi*(shape['OD']-shape['t'])/2.0*shape['t']
    shape['Avy']= tmp
    shape['Avz']= tmp
    shape['AreaQz']= shape['Avz']
    shape['AreaQy']= shape['Avy']

CHSS= shapes.CHSS

class CHSSShape(structural_steel.CHShape):
    ''' Circular hollow structural section.

    :ivar steel: steel material.
    :ivar name: shape name (i.e. HSS16.000X0.375).
    '''
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        super(CHSSShape,self).__init__(steel,name,CHSS)

    def getLimitingWidthToThicknessRatio(self):
        ''' Return the Limiting Width-to-Thickness Ratio 
        according to table B4.1A of AISC-360-16.
        '''
        return 1.4*math.sqrt(self.steelType.E/self.steelType.fy)

    def getLambdaP(self):
        ''' Return the limiting Width-to-Thickness Ratio (compact/noncompact)
            according to table B4.1b of AISC 360-16.
        '''
        return 1.12*math.sqrt(self.steelType.E/self.steelType.fy)
    
    def getLambdaR(self):
        ''' Return the limiting Width-to-Thickness Ratio (compact/noncompact)
            according to table B4.1b of AISC 360-16.
        '''
        return 1.40*math.sqrt(self.steelType.E/self.steelType.fy)
    

    # Bending
    def compactRatio(self):
        ''' If section is compact according to case
            20 (round HSS sections subject to flexure) of
            table B4.1b of AISC-360-16 return a value 
            less than one.

        '''
        retval= 10.0
        E= self.get('E')
        Fy= self.steelType.fy
        slendernessRatio= self.get('slendernessRatio')
        lambda_p= 0.07*(E/Fy) # Case 20 (no square root here)
        retval= slendernessRatio/lambda_p
        return retval # if <1 then section is compact.
    
    def slenderRatio(self):
        ''' If section is noncompact according to table B4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        '''
        retval= 10.0
        E= self.get('E')
        Fy= self.steelType.fy
        slendernessRatio= self.get('slendernessRatio')
        lambda_r= 0.31*(E/Fy) # Case 20 (no square root here)
        retval= slendernessRatio/lambda_r
        return retval # if <1 then flanges are compact.
    
    def getAw(self):
        ''' Return area for shear strenght calculation.'''
        return self.get('A')
    
    def getNominalShearStrength(self, Lv= None):
        ''' Return the nominal shear strength according to equations
            G4-1 and G5-1, G5-2a and G5-2b of AISC-360-16.

        :param Lv: distance from maximum to zero shear force.
        :param majorAxis: true if flexure about the major axis.
        '''
        Fy= self.steelType.fy

        E= self.get('E')
        D= self.get('OD')
        slendernessRatio= self.get('slendernessRatio')
        Fcr= 0.0
        if(Lv):
           Fcr= 1.6*E/math.sqrt(Lv/D)/math.pow(slendernessRatio,5.0/4.0) # equation G5-2a
        Fcr= max(Fcr, 0.78*E/math.pow(slendernessRatio,3.0/2.0)) # equation G-2b
        Fcr= min(Fcr,0.6*Fy)
        return Fcr*self.getAw()/2.0

    def getDesignShearStrength(self, majorAxis= True):
        ''' Return the design shear strength according to equation
            section G1 of AISC-360-16.

        :param majorAxis: dummy argument needed for compatibility with
                          the other shapes.
        '''
        return 0.9*self.getNominalShearStrength()
    
    def slendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to case 9 of table B4.1a of AISC-360-16.'''
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_r= 0.11*(E/Fy)
        slendernessRatio= self.get('slendernessRatio')
        return slendernessRatio/lambda_r # nonslender if < 1.0
    
    def getTorsionalElasticBucklingStress(self, Lc):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2 of AISC-360-16.

        :param Lc: effective length of member for torsional buckling 
                   about longitudinal axis.
        '''
        E= self.get('E')
        G= self.get('G')
        Cw= self.get('C') #HSS torsional constant.
        J= self.get('It')
        Iy= self.get('Iy')
        Iz= self.get('Iz')
        return (math.pi**2*E*Cw/Lc**2+G*J)/(Iy+Iz)
    
    def getPlasticAxialLoad(self):
        ''' Return the plastic axial load of the section.'''
        return getShapePlasticAxialLoad(self)
    
    def getPlasticMoment(self):
        ''' Return the plastic moment of the section.

        '''
        return getShapePlasticMoment(self)

    def getNominalFlexuralStrength(self):
        ''' Return the nominal flexural strength of the member
            according to equations F7-1 to F7-13 and F8-1 to F8-4 
            of AISC-360-16.
        '''
        Mn= 0.0
        lmsg.error(__name__+'; nominal flexural strength for circular HSS sections not implemented yet.')
        return Mn
    
    def getDesignFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the design flexural strength of the section
            according to section F1 of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        return 0.9*self.getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis)

    
# Label conversion metric->US customary | US customary -> metric.
def getUSLabel(metricLabel):
    '''Return the US customary label from the metric one.'''
    return labels.USLabel[metricLabel]

def getMetricLabel(USLabel):
    '''Return the metric label from the US customary one.'''
    return labels.MetricLabel[metricLabel]




