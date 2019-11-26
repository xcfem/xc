# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

''' Structural steel as specified in ASTM standard.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
from materials import steel_base
from miscUtils import LogMessages as lmsg
from materials import buckling_base

class ASTMSteel(steel_base.BasicSteel):
    '''ASTM structural steel.'''
    def __init__(self, fy= 250e6, fu= 400e6, gammaM= 1.0):
        super(ASTMSteel,self).__init__(200e9,0.3,fy,fu,gammaM)

A36= ASTMSteel(250e6,400e6,1.0)
A529= ASTMSteel(290e6,414e6,1.0)
A572= ASTMSteel(345e6,450e6,1.0)
A53= ASTMSteel(240e6,414e6,1.0)
A992= ASTMSteel(345e6,450e6,1.0)
A500= ASTMSteel(315e6,400e6,1.0)
A307= ASTMSteel(245e6,390e6,1.0)

class ASTMShape(object):
    """Steel shape with ASTM verification routines."""
    def __init__(self, name):
       '''
         Constructor.

       '''
       self.name=name
       
from materials.sections.structural_shapes import aisc_metric_shapes

class WShape(ASTMShape,aisc_metric_shapes.WShape):
    """W shape with ASTM verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. W40X431)
        '''
        ASTMShape.__init__(self,name)
        aisc_metric_shapes.WShape.__init__(self,steel,name)

class CShape(ASTMShape,aisc_metric_shapes.CShape):
    """C shape with ASTM 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name  (i.e. C380X74).
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.CShape.__init__(self,steel,name)

class HSSShape(ASTMShape,aisc_metric_shapes.HSSShape):
    """HSS shape with ASTM verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. HSS2X2X_250).
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.HSSShape.__init__(self,steel,name)
        
    def getLimitingWidthToThicknessRatio(self):
        ''' Return the Limiting Width-to-Thickness Ratio 
            according to table B4.1A of AISC-360-16.
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
    def getZYieldingFlexuralStrength(self):
        ''' Return the plastic moment of the
            section around the z axis.'''
        return self.steelType.fy*self.get('Wzpl')
    def getZFlangeLocalBucklingFlexuralStrength(self):
        ''' Return the maximum flexural strength
            due to flange local buckling.'''
        classif= self.getHFlexureClassification()
        Mp= self.getZYieldingFlexuralStrength()
        retval= 0.0
        if(classif=='compact'):
            retval= Mp
        elif(classif=='noncompact'): # Flange local buckling
            tmp= (Mp-self.steelType.fy*self.get('Wzel'))
            tmp*=(3.57*self.get('bSlendernessRatio')*math.sqrt(self.steelType.fy/self.steelType.E)-4.0)
            retval= min(Mp,Mp-tmp)
        else:
            b_eff= self.getReducedEffectiveB()
            Ae= b_eff*self.t
            z= (self.h-self.t)/2.0
            Se= 2.0*Ae*z
            retval= self.steelType.fy*Se
        return retval
    def getZWebLocalBucklingFlexuralStrength(self):
        ''' Return the maximum flexural strength
            due to web local buckling.'''
        Mp= self.getZYieldingFlexuralStrength()
        retval= Mp
        classif= self.getHFlexureClassification()
        if(classif=='compact'):
            retval= Mp
        elif(classif=='noncompact'):
            tmp= (Mp-self.steelType.fy*self.get('Wzel'))
            tmp*= (0.305*self.get('hSlendernessRatio')*math.sqrt(self.steelType.fy/self.steelType.E)-0.738)
            tmp= min(Mp,Mp-tmp)
            retval= min(retval,tmp)
        else: #slender
            lmsg.error("Not implemented yet.")
        return retval
    def getZLimitingLaterallyUnbracedLengthForYielding(self):
        ''' Return the limiting laterally unbraced length 
            for the limit state of yielding according
            to expression FT-12 of AISC 360-16.
        '''
        retval= 0.13*self.steelType.E*self.get('iy')
        retval*= math.sqrt(self.get('It')*self.get('A'))
        Mp= self.getZYieldingFlexuralStrength()
        retval/=Mp
        return retval
    def getZLimitingLaterallyUnbracedLengthForInelasticBuckling(self):
        ''' Return the limiting laterally unbraced length 
            for the limit state of inelastic lateral torsional
            buckling according to expression FT-13 of AISC 360-16.
        '''
        retval= 2.0*self.steelType.E*self.get('iy')
        retval*= math.sqrt(self.get('It')*self.get('A'))
        M= 0.7*self.steelType.fy*self.get('Wyel')
        retval/=M
        return retval
        
    def getZLateralTorsionalBucklingFlexuralStrength(self, Lb, Cb):
        ''' Return the maximum flexural strength
            due to web local buckling according to
            expressions F7-10 to F7-11 of AISC 360-16

        :param Lb: Length between points that are either braced 
                   against lateral displacement of compression 
                   flange or braced against twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor Cb
                   for non uniform moment diagrams when both ends of the 
                   segment are braced according to expression 
                   F1-1 of AISC 360-16.
        '''
        Mp= self.getZYieldingFlexuralStrength()
        Lp= self.getZLimitingLaterallyUnbracedLengthForYielding()
        retval= 0.0
        if(Lb<=Lp): #No buckling
            retval= Mp
        else: #Lb>Lp
            Lr= self.getZLimitingLaterallyUnbracedLengthForInelasticBuckling()
            if(Lb<Lr):
                retval= Mp-(Mp-0.7*self.steelType.fy*self.get('Wyel'))*(Lb-Lp)/(Lr-Lp)
                retval*= Cb
                retval= min(Mp,retval)
            else: #Lb>Lp
                retval= 2.0*self.steelType.E*Cb
                retval*= math.sqrt(self.get('It')*self.get('A'))
                retval/= (Lb/self.get('iy'))
        return retval
    def getZNominalflexuralStrength(self, Lb, Cb):
        ''' Return the nominal flexural strength
            around z axis.
        :param Lb: Length between points that are either braced 
                   against lateral displacement of compression 
                   flange or braced against twist of the cross section.

        :param Cb: lateral-torsional buckling modification factor Cb
                   for non uniform moment diagrams when both ends of the 
                   segment are braced according to expression 
                   F1-1 of AISC 360-16.
        '''
        Mp= self.getZYieldingFlexuralStrength()
        retval= Mp
        Mnf= self.getZFlangeLocalBucklingFlexuralStrength()
        retval= min(retval,Mnf)
        Mnw= self.getZWebLocalBucklingFlexuralStrength()
        retval= min(retval,Mnw)
        Mlt= self.getZLateralTorsionalBucklingFlexuralStrength(Lb, Cb)
        retval= min(retval,Mlt)
        return retval
    def getYYieldingFlexuralStrength(self):
        ''' Return the plastic moment of the
            section around the z axis.'''
        return self.steelType.fy*self.get('Wypl')
    def getYFlangeLocalBucklingFlexuralStrength(self):
        ''' Return the maximum flexural strength
            due to flange local buckling.'''
        classif= self.getBFlexureClassification()
        Mp= self.getYYieldingFlexuralStrength()
        retval= 0.0
        if(classif=='compact'):
            retval= Mp
        elif(classif=='noncompact'): # Flange local buckling
            tmp= (Mp-self.steelType.fy*self.get('Wyel'))
            tmp*=(3.57*self.get('hSlendernessRatio')*math.sqrt(self.steelType.fy/self.steelType.E)-4.0)
            retval= min(Mp,Mp-tmp)
        else:
            h_eff= self.getReducedEffectiveH()
            Ae= h_eff*self.t
            y= (self.b-self.t)/2.0
            Se= 2.0*Ae*y
            retval= self.steelType.fy*Se
        return retval
    def getYWebLocalBucklingFlexuralStrength(self):
        ''' Return the maximum flexural strength
            due to web local buckling.'''
        Mp= self.getYYieldingFlexuralStrength()
        retval= Mp
        classif= self.getBFlexureClassification()
        if(classif=='compact'):
            retval= Mp
        elif(classif=='noncompact'):
            tmp= (Mp-self.steelType.fy*self.get('Wyel'))
            tmp*= (0.305*self.get('bSlendernessRatio')*math.sqrt(self.steelType.fy/self.steelType.E)-0.738)
            tmp= min(Mp,Mp-tmp)
            retval= min(retval,tmp)
        else: #slender
            lmsg.error("Not implemented yet.")
        return retval
    def getYLimitingLaterallyUnbracedLengthForYielding(self):
        ''' Return the limiting laterally unbraced length 
            for the limit state of yielding according
            to expression FT-12 of AISC 360-16.
        '''
        retval= 0.13*self.steelType.E*self.get('iz')
        retval*= math.sqrt(self.get('It')*self.get('A'))
        Mp= self.getYYieldingFlexuralStrength()
        retval/=Mp
        return retval
    def getYLimitingLaterallyUnbracedLengthForInelasticBuckling(self):
        ''' Return the limiting laterally unbraced length 
            for the limit state of inelastic lateral torsional
            buckling according to expression FT-13 of AISC 360-16.
        '''
        retval= 2.0*self.steelType.E*self.get('iz')
        retval*= math.sqrt(self.get('It')*self.get('A'))
        M= 0.7*self.steelType.fy*self.get('Wyel')
        retval/=M
        return retval        
    def getYLateralTorsionalBucklingFlexuralStrength(self, Lb, Cb):
        ''' Return the maximum flexural strength
            due to web local buckling according to
            expressions F7-10 to F7-11 of AISC 360-16

        :param Lb: Length between points that are either braced 
                   against lateral displacement of compression 
                   flange or braced against twist of the cross section.
        :param Cb: lateral-torsional buckling modification factor Cb
                   for non uniform moment diagrams when both ends of the 
                   segment are braced according to expression 
                   F1-1 of AISC 360-16.
        '''
        Mp= self.getYYieldingFlexuralStrength()
        Lp= self.getYLimitingLaterallyUnbracedLengthForYielding()
        retval= 0.0
        if(Lb<=Lp): #No buckling
            retval= Mp
        else: #Lb>Lp
            Lr= self.getYLimitingLaterallyUnbracedLengthForInelasticBuckling()
            if(Lb<Lr):
                retval= Mp-(Mp-0.7*self.steelType.fy*self.get('Wyel'))*(Lb-Lp)/(Lr-Lp)
                retval*= Cb
                retval= min(Mp,retval)
            else: #Lb>Lp
                retval= 2.0*self.steelType.E*Cb
                retval*= math.sqrt(self.get('It')*self.get('A'))
                retval/= (Lb/self.get('iz'))
        return retval
    def getYNominalflexuralStrength(self, Lb, Cb):
        ''' Return the nominal flexural strength
            around z axis.
        :param Lb: Length between points that are either braced 
                   against lateral displacement of compression 
                   flange or braced against twist of the cross section.

        :param Cb: lateral-torsional buckling modification factor Cb
                   for non uniform moment diagrams when both ends of the 
                   segment are braced according to expression 
                   F1-1 of AISC 360-16.
        '''
        Mp= self.getYYieldingFlexuralStrength()
        retval= Mp
        Mnf= self.getYFlangeLocalBucklingFlexuralStrength()
        retval= min(retval,Mnf)
        Mnw= self.getYWebLocalBucklingFlexuralStrength()
        retval= min(retval,Mnw)
        Mlt= self.getYLateralTorsionalBucklingFlexuralStrength(Lb, Cb)
        retval= min(retval,Mlt)
        return retval
        
class BendingState(object):
    ''' Bending moments along the member.

        :ivar Mmax: absolute value of maximum moment in the unbraced 
                     segment.
        :ivar Ma: absolute value of moment at quarter point of the 
                   unbraced segment.
        :ivar Mb: absolute value of moment at centerline of the 
                   unbraced segment.
        :ivar Mc: absolute value of moment at three-quarter point of 
                   the unbraced segment.
    '''
    def __init__(self, Mmax, Ma, Mb, Mc):
        ''' Constructor.'''
        self.Mmax= Mmax
        self.Ma= Ma
        self.Mb= Mb
        self.Mc= Mc
    def getLateralTorsionalBucklingModificationFactor(self):
        ''' Return the lateral-torsional buckling modification factor Cb
            for non uniform moment diagrams when both ends of the segment
            are braced according to expression F1-1 of AISC 360-16.
        '''
        return 12.5*self.Mmax/(2.5*self.Mmax+3*self.Ma+4*self.Mb+3*self.Mc)

class MemberConnection(buckling_base.MemberConnection):
    '''Member length and connections

       :ivar L: member length.
       :ivar Lb: Length between points that are either braced 
                 against lateral displacement of compression 
                 flange or braced against twist of the cross section.
       :ivar rotI: fixity of the rotation at member start.
       :ivar transI: fixity of the translation at member start.
       :ivar rotJ: fixity of the rotation at member end.
       :ivar transJ: fixity of the translation at member end.
    '''
    def __init__(self,L,rotI='free',transI='fixed',rotJ= 'free',transJ= 'fixed'):
        '''Constructor.'''
        super(MemberConnection, self).__init__(rotI,transI,rotJ,transJ)
        self.L= L
        self.Lb= L
    def getLateralTorsionalBucklingModificationFactor(self,bendingState):
        ''' Return the lateral-torsional buckling modification factor Cb
            for non uniform moment diagrams when both ends of the segment
            are braced according to expression F1-1 of AISC 360-16.

            :param Mmax: absolute value of maximum moment in the unbraced 
                         segment.
            :param Ma: absolute value of moment at quarter point of the 
                       unbraced segment.
            :param Mb: absolute value of moment at centerline of the 
                       unbraced segment.
            :param Mc: absolute value of moment at three-quarter point of 
                       the unbraced segment.
        '''
        if(self.transJ=='free' and self.rotJ=='free'): # cantilever
            return 1.0
        else:
            return bendingState.getLateralTorsionalBucklingModificationFactor()

class Member(object):
    """C shape with ASTM 3 verification routines."""
    def __init__(self,shape,connection):
        ''' Constructor.

        :param shape: structural shape.
        :param connection: member length and connection information.
        '''
        self.shape= shape
        self.connection= connection
    def getEffectiveLength(self):
        '''Return the member effective length according to
           section E2 of AISC 360-16.'''
        K= self.connection.getEffectiveBucklingLengthCoefficientRecommended()
        return  K*self.connection.L #Effective length of member.
    def getSlendernessRatio(self):
        '''Return the slenderness ratio of the member.'''
        Lc= self.getEffectiveLength()
        r= min(self.shape.get('iy'),self.shape.get('iz'))
        return Lc/r
    def getElasticBucklingStress(self):
        '''Return the elastic buckling stress of the member.'''
        return math.pi**2*self.shape.steelType.E/(self.getSlendernessRatio())**2
    def getCriticalStress(self):
        '''Return the critical stress as definded in
           section E7 of AISC 360-16.
        '''
        treshold= 4.71*math.sqrt(self.shape.steelType.E/self.shape.steelType.fy)
        r= self.getSlendernessRatio()
        Fe= self.getElasticBucklingStress()
        fy_fe= self.shape.steelType.fy/Fe
        if((r<=treshold) or (fy_fe<=2.25)):
            return math.pow(0.658,fy_fe)*self.shape.steelType.fy
        else:
            return 0.877*Fe
    def getNominalCompressiveStrength(self):
        ''' Return the nominal compressive strength according to
            section E7 of AISC 360-16.
        '''
        return self.shape.getEffectiveArea()*self.getCriticalStress()
    def getZLateralTorsionalBucklingFlexuralStrength(self, bendingState):
        ''' Return the maximum flexural strength
            due to web local buckling according to
            expressions F7-10 to F7-11 of AISC 360-16

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getZLateralTorsionalBucklingFlexuralStrength(Lb= self.connection.Lb, Cb= cb)
    def getZNominalflexuralStrength(self, bendingState):
        ''' Return the nominal flexural strength
            around z axis.


            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getZNominalflexuralStrength(Lb= self.connection.Lb, Cb= cb)
    def getYLateralTorsionalBucklingFlexuralStrength(self, bendingState):
        ''' Return the maximum flexural strength
            due to web local buckling according to
            expressions F7-10 to F7-11 of AISC 360-16

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getYLateralTorsionalBucklingFlexuralStrength(Lb= self.connection.Lb, Cb= cb)
    def getYNominalflexuralStrength(self, bendingState):
        ''' Return the nominal flexural strength
            around z axis.


            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getYNominalflexuralStrength(Lb= self.connection.Lb, Cb= cb)
    def getCapacityFactor(self,Nd,Myd,Mzd,gammaC,bendingStateY,bendingStateZ):
        ''' Return the capacity factor according to section
            H1 of AISC 360-16.

        :param Lb: Length between points that are either braced 
                   against lateral displacement of compression 
                   flange or braced against twist of the cross section.

        :param Nd: design value for the axial load (negative for compression).
        :param Mzd: design value for the bending moment around z axis.
        :param Myd: design value for the bending moment around z axis.
        :param bendingStateY: y bending moments along the member.
        :param bendingStateZ: z bending moments along the member.
        '''
        if(Nd<=0.0):
            Pn= self.getNominalCompressiveStrength()/gammaC
            Mnz= self.getZNominalflexuralStrength(bendingStateZ)/gammaC
            Mny= self.getYNominalflexuralStrength(bendingStateY)/gammaC
            ratioN= abs(Nd)/Pn
            Msum= (abs(Mzd)/Mnz+abs(Myd)/Mny)
            if(ratioN>=0.2):
                return ratioN+8/9.0*Msum # (H1-1a)
            else:
                return ratioN/2.0+Msum # (H1-1b)

        else:
            lmsg.error('Capacity factor not implemented for tension.')
