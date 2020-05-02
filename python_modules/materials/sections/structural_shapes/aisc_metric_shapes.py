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

def getShapePlasticMoment(shape, majorAxis= True):
    ''' Return the plastic moment of the shape.

    :param shape: structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    Fy= shape.steelType.fy
    if(majorAxis):
        retval= Fy*shape.get('Wzpl')
    else:
        retval= Fy*shape.get('Wypl')

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
        lmsg.error('r_{ts} coefficient not implemented for minor axis.')
    Cw= shape.get('Cw') # Warping constant.
    Iy= shape.get('Iy') # Moment of inertia about minor axis.
    Sz= shape.get('Wzel') # Elastic section modulus about major axis.
    return math.sqrt(math.sqrt(Iy*Cw)/Sz)

def getUIShapeLp(shape, majorAxis= True):
    '''Return the limiting laterally unbraced length for the limit state
       of yielding according to equation F2-5 of AISC-360-16.

    :param shape: I or channel structural shape.
    :param majorAxis: true if flexure about the major axis.
    '''
    if(not majorAxis):
        lmsg.error('L_p not implemented for minor axis.')
    ry= shape.get('iy') # Radius of gyration about minor axis.
    E= self.get('E') # Elastic modulus.
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
        lmsg.error('L_r not implemented for minor axis.')
    rts= shape.getRts()
    E= self.get('E') # Elastic modulus.
    Fy= shape.steelType.fy # specified minimum yield stress
    Sz= shape.get('Wzel') # Elastic section modulus about major axis.
    h0= shape.get('ho') # Distance between the flange centroids
    J= shape.get('It') # Torsional moment of inertia
    c= shape.getCCoefficient()
    sqrt1= math.sqrt((J*c/Sx/h0)**2+6.76*(0.7*Fy/E)**2)
    sqrt2= math.sqrt((J*c/Sx/h0)+sqrt1)
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
    if(not majorAxis):
        lmsg.error('L_r not implemented for minor axis.')
    E= self.get('E') # Elastic modulus.
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
        according to equations F2-1 to F2-3 of AISC-360-16.

    :param shape: I or channel structural shape.
    :param lateralUnbracedLength: length between points that are either 
                                  braced against lateral displacement of
                                  the compression flange or braced against 
                                  twist of the cross section.
    :param Cb: lateral-torsional buckling modification factor.
    :param majorAxis: true if flexure about the major axis.
    '''
    if(not majorAxis):
        lmsg.error('Nominal flexural strength not implemented for minor axis.')
    compactRatio= shape.compactWebAndFlangeRatio(majorAxis)
    if(compactRatio>1):
        lmsg.error('Nominal flexural strength for noncompact or slender sections not implemented yet.')
    Lb= lateralUnbracedLength
    Mp= shape.getPlasticMoment(majorAxis) # plastic moment.
    Mn= 0.0
    Lp= shape.getLp(majorAxis)
    if(Lb<Lp):
        Mn= Mp
    else:
        Lr= shape.getLr(majorAxis)
        Sz= shape.get('Wzel') # Elastic section modulus about major axis.
        if(Lb<=Lr):
            Fy= shape.steelType.fy # specified minimum yield stress
            Mn= Cb*(Mp-(Mp-0.7*Fy*Sz)*((Lb-Lp)/(Lr-Lp)))
            Mn= min(Mn,Mp)
        else:
            Fcr= shape.getCriticalStress(Lb, Cb, majorAxis)
            Mn= min(Fcr*Sz, Mp)
    return Mn

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

    def compactFlangeRatio(self, majorAxis= True):
        ''' If flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_p= 0.38*math.sqrt(E/Fy) # Case 10 or case 13
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then flanges are compact.
    
    def slenderFlangeRatio(self, majorAxis= True):
        ''' If flanges are noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_r= 1.0*math.sqrt(E/Fy) # Case 10 or case 13
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # if <1 then flanges are noncompact.

    def compactWebRatio(self, majorAxis= True):
        ''' If web is compact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error('compact web ratio not implemented for minor axis.')
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
            lmsg.error('compact web ratio not implemented for minor axis.')
        lambda_p= 5.70*math.sqrt(E/Fy) # Case 15
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then web is noncompact.
    
    def compactWebAndFlangeRatio(shape, majorAxis= True):
        ''' If web and flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapeCompactWebAndFlangeRatio(self,majorAxis)

    def slendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to table B4.1a of AISC-360-16.'''
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_r= 0.56*math.sqrt(E/Fy) # Case 1
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # OK if < 1.0

    def getAw(self):
        ''' Return A_w according to AISC specification Section G2.1b'''
        return self.h()*self.get('tw')
      
    def getShearResistanceFactor(self):
        ''' Return the resistance factor for shear according to
        section G2.1a.'''
        return 1.0
      
    def getWebShearStrengthCoefficient(self):
        ''' Return the web shear stress coefficient Cv1 according
            to equations G2-2, G2-3 and G2-4 of "Specification for 
            Structural Steel Buildings, July 7, 2016 AISC"
        '''
        Cv1= 1.0
        h_tw= self.get('h')/self.get('tw')
        h_tw_threshold= 2.24*math.sqrt(self.get('E')/self.steelType.fy)
        if(h_tw<h_tw_threshold):
            Cv1= 1.0;
        else:
            lmsg.error('getWebShearStrengthCoefficient not implemented yet for this type of sections.')
            Cv1= 0.0;
        return Cv1
          
    def getNominalShearStrengthWithoutTensionFieldAction(self):
        ''' Return the nominal shear strength according to equation
            2.1 of "Specification for Structural Steel Buildings,
            July 7, 2016 AISC"
        '''
        #
        Cv1= self.getWebShearStrengthCoefficient()
        return 0.6*self.steelType.fy*self.getAw()*Cv1
    
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
        
    def compactFlangeRatio(self, majorAxis= True):
        ''' If flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_p= 0.38*math.sqrt(E/Fy) # Case 10 or case 13
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then flanges are compact.
    
    def slenderFlangeRatio(self, majorAxis= True):
        ''' If flanges are noncompact according to table 4.1b of 
            AISC-360-16 return a value less than one otherwise
            they are slender.

        :param majorAxis: true if flexure about the major axis.
        '''
        lambda_r= 1.0*math.sqrt(E/Fy) # Case 10 or case 13
        slendernessRatio= self.get('bSlendernessRatio')
        return slendernessRatio/lambda_r # if <1 then flanges are noncompact.
    
    def compactWebRatio(self, majorAxis= True):
        ''' If web is compact according to case 15 of table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        if(not majorAxis):
            lmsg.error('compact web ratio not implemented for minor axis.')
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
            lmsg.error('compact web ratio not implemented for minor axis.')
        lambda_p= 5.70*math.sqrt(E/Fy) # Case 15
        slendernessRatio= self.get('hSlendernessRatio')
        return slendernessRatio/lambda_p # if <1 then web is noncompact.
    
    def compactWebAndFlangeRatio(shape, majorAxis= True):
        ''' If web and flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapeCompactWebAndFlangeRatio(self,majorAxis)
    
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
            lmsg.error('c coefficient not implemented for minor axis.')
        h0= self.get('ho') # Distance between the flange centroids
        Cw= self.get('Cw') # Warping constant.
        Iy= self.get('Iy') # Moment of inertia about minor axis.
        return h0/2.0*math.sqrt(Iy/Cw)

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

    def isRectangular(self):
        ''' Return true if it's a rectangular HSS section.'''
        return ('h_flat' in shape)

    def isRound(self):
        ''' Return true if it's a rectangular HSS section.'''
        return not self.isRectangular()

    def compactFlangeRatio(self, majorAxis= True):
        ''' If flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        retval= 10.0
        if(isRound):
            slendernessRatio= shape['slendernessRatio']
            lambda_p= 0.07*(E/Fy) # Case 20 (no square root here)
            retval= slendernessRatio/lambda_p
        else:
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
        if(isRound):
            slendernessRatio= shape['slendernessRatio']
            lambda_r= 0.31*(E/Fy) # Case 20 (no square root here)
            retval= slendernessRatio/lambda_r
        else:
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
        if(isRound):
            slendernessRatio= shape['slendernessRatio']
            lambda_p= 0.07*(E/Fy) # Case 20 (no square root here)
            retval= slendernessRatio/lambda_p
        else:
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
        if(isRound):
            slendernessRatio= shape['slendernessRatio']
            lambda_r= 0.31*(E/Fy) # Case 20 (no square root here)
            retval= slendernessRatio/lambda_r
        else:
            lambda_r= 5.7*math.sqrt(E/Fy) # Case 19
            if(majorAxis):
                slendernessRatio= self.get('bSlendernessRatio')
            else:
                slendernessRatio= self.get('hSlendernessRatio')
            retval= slendernessRatio/lambda_r
        return retval # if <1 then webs are noncompact.
    
    def compactWebAndFlangeRatio(shape, majorAxis= True):
        ''' If web and flanges are compact according to table 4.1b of 
            AISC-360-16 return a value less than one.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapeCompactWebAndFlangeRatio(self,majorAxis)
    
    def slendernessCheck(self):
        ''' Verify that the section doesn't contains slender elements
            according to table B4.1 a of AISC-360-16.'''
        E= self.get('E')
        Fy= self.steelType.fy
        lambda_r= 1.4*math.sqrt(E/Fy)
        if(self.isRectangular()): # rectangular
            slendernessRatio= max(shape['hSlendernessRatio'],shape['bSlendernessRatio'])
        else: # round
            slendernessRatio= shape['slendernessRatio']
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
    
    def getPlasticMoment(self, majorAxis= True):
        ''' Return the plastic moment of the section.

        :param majorAxis: true if flexure about the major axis.
        '''
        return getShapePlasticMoment(self,majorAxis)
    
    def getNominalFlexuralStrength(self, lateralUnbracedLength, Cb, majorAxis= True):
        ''' Return the nominal flexural strength of the member
            according to equations XXX of AISC-360-16.

        :param lateralUnbracedLength: length between points that are either 
                                      braced against lateral displacement of
                                      the compression flange or braced against 
                                      twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor.
        :param majorAxis: true if flexure about the major axis.
        '''
        lmsg.error('Nominal flexural strength for HSS sections not implemented yet.')
        return 0.0

# Label conversion metric->US customary | US customary -> metric.
def getUSLabel(metricLabel):
    '''Return the US customary label from the metric one.'''
    return labels.USLabel[metricLabel]

def getMetricLabel(USLabel):
    '''Return the metric label from the US customary one.'''
    return labels.MetricLabel[metricLabel]




