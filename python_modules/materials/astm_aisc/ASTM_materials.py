# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

''' Structural steel as specified in ASTM standard.

Predefined ASTM steel types: A36, A529, A572, A53, A992, A500, A307
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
from materials import steel_base
from misc_utils import log_messages as lmsg
from materials import buckling_base

class ASTMSteel(steel_base.BasicSteel):
    '''ASTM structural steel.

    :ivar fy: yield stress (defaults to 250e6 Pa)
    :ivar fu: ultimate tensile strength (defaults to 400e6 Pa)
    :ivar gammaM: partial factor (defaults to 1.0)
    '''
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
    """Steel shape with ASTM/AISC verification routines."""
    def __init__(self, name):
       '''
         Constructor.
       '''
       self.name=name

    # Tension
    def getDesignTensileStrength(self, Ae= None):
        ''' Return the tensile strength of the section according
            to section D2 fo AISC-360-16.

        :param Ae: effective net area according to section D3.
        '''
        Ag= self.get('A')
        retval= 0.9*self.steelType.fy*Ag
        if(not Ae):
            Ae= Ag
        retval= min(retval,0.75*self.steelType.fu*Ae)
        return retval

    # Compression
    def getFlexuralSlendernessRatio(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the flexural buckling slenderness ratio of the member.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLegnthZ: effective length of member (major axis).
        '''
        sc= self.slendernessCheck()
        if(sc>1.01):
            lmsg.warning('Member section has slender members. Results are not valid.')
        retval= effectiveLengthZ/self.get('iz')
        return max(retval,effectiveLengthY/self.get('iy'))
    
    def getFlexuralCriticalSlendernessRatio(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the critical value of the flexural buckling 
            slenderness ratio of the member.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLegnthZ: effective length of member (major axis).
         '''
        sr= self.getFlexuralSlendernessRatio(effectiveLengthY, effectiveLengthZ)
        E= self.get('E')
        Fy= self.steelType.fy
        return sr/math.pi*math.sqrt(Fy/E)
    
    def getFlexuralElasticBucklingStress(self, effectiveLengthY, effectiveLengthZ):
        ''' Return the flexural elastic buckling stress of the member according
            to equation E3-4 of AISC-360-16.
 
        :param effectiveLengthY: effective length of member (minor axis).
        :parem effectiveLegnthZ: effective length of member (major axis).
        '''
        sr= self.getFlexuralSlendernessRatio(effectiveLengthY, effectiveLengthZ)
        E= self.get('E')
        return math.pi**2*E/sr**2
    
    def getFlexuralElasticBucklingStressOnAxis(self, effectiveLengthY, effectiveLengthZ, majorAxis= True):
        ''' Return the flexural elastic buckling stress of the member according
            to equations E4-5 or E4-6 of AISC-360-16.
  
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLegnthZ: effective length of member (major axis).
        :param majorAxis: true if flexure about the major axis.
       '''
        if(majorAxis):
            sr= effectiveLengthZ/self.shape.get('iz') # Slenderness ratio for eq. E4-5
        else:
            sr= effectiveLengthY/self.shape.get('iy') # Slenderness ratio for eq. E4-6
        E= self.shape.get('E')
        return math.pi**2*E/sr**2

    # Bending 
    def getReferenceFlexuralStrength(self):
        ''' Return the flexural strength of the section without
            take in to account the lateral buckling effect.'''
        return self.getDesignFlexuralStrength(lateralUnbracedLength= 1.0, Cb= 1.0, majorAxis= True)

    # Combined internal forces
    def getBiaxialBendingEfficiency(self,sectionClass,Nd,Myd,Mzd,Vyd= 0.0, chiN=1.0, chiLT=1.0):
        '''Return biaxial bending efficiency according to section H1
           of AISC-360-16.

           sectionClass: dummy argument needed for compatibility with other codes.
           chiN: axial load reduction reduction factor (default= 1.0).
           chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        ratioN= 0.0
        if(Nd<0): # compression
            NcRd= chiN*self.getPlasticAxialLoad() # available axial strength.
            ratioN=  abs(Nd)/NcRd
        else:
            NcRd= self.getDesignTensileStrength() # available axial strength.
            ratioN= Nd/NcRd
        McRdy= self.getDesignFlexuralStrength(None, None, majorAxis= False) # available flexural strength minor axis.
        McRdz= self.getReferenceFlexuralStrength() # reference flexural strength major axis.
        # MvRdz= self.getMvRdz(sectionClass,Vyd)
        MvRdz= McRdz
        MbRdz= chiLT*MvRdz # available flexural strength major axis.
        ratioMz= abs(Mzd)/MbRdz
        ratioMy= abs(Myd)/McRdy
        if(ratioN>=0.2):
            CF= ratioN+8.0/9.0*(ratioMz+ratioMy) # equation H1-1a
        else:
            CF= ratioN/2.0+(ratioMz+ratioMy) # equation H1-1b
        return (CF,NcRd,McRdy,McRdz,MvRdz,MbRdz)
       
from materials.sections.structural_shapes import aisc_metric_shapes

class WShape(ASTMShape,aisc_metric_shapes.WShape):
    """W shape with ASTM/AISC verification routines.

    :ivar steel: steel material (i.e. A36).
    :ivar name: shape name (i.e. W40X431)

    """
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        ASTMShape.__init__(self,name)
        aisc_metric_shapes.WShape.__init__(self,steel,name)

class CShape(ASTMShape,aisc_metric_shapes.CShape):
    """C shape with ASTM 3 verification routines.

    :ivar steel: steel material (i.e. A36).
    :ivar name: shape name  (i.e. C380X74).
    """
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.CShape.__init__(self,steel,name)

class HSSShape(ASTMShape,aisc_metric_shapes.HSSShape):
    """Rectangular HSS shape with ASTM/AISC verification routines.

    :ivar steel: steel material (i.e. A36).
    :ivar name: shape name (i.e. HSS2X2X_250).
    """
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.HSSShape.__init__(self,steel,name)

class CHSSShape(ASTMShape,aisc_metric_shapes.CHSSShape):
    """Circular HSS shape with ASTM/AISC verification routines.

    :ivar steel: steel material (i.e. A36).
    :ivar name: shape name (i.e. HSS16.000X0.375).
    """
    def __init__(self,steel,name):
        ''' Constructor.
        '''
        ASTMShape.__init__(self, name)
        aisc_metric_shapes.CHSSShape.__init__(self,steel,name)

 
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

        :ivar shape: structural shape.
        :ivar connection: member length and connection information.
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
        return self.shape.getLateralTorsionalBucklingLimit(Lb= self.connection.Lb, Cb= cb, majorAxis= True)
    
    def getZNominalflexuralStrength(self, bendingState):
        ''' Return the nominal flexural strength
            around z axis.


            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getNominalFlexuralStrength(lateralUnbracedLength= self.connection.Lb, Cb= cb, majorAxis= True)
    def getYLateralTorsionalBucklingFlexuralStrength(self, bendingState):
        ''' Return the maximum flexural strength
            due to web local buckling according to
            expressions F7-10 to F7-11 of AISC 360-16

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getLateralTorsionalBucklingLimit(Lb= self.connection.Lb, Cb= cb, majorAxis= False)
    
    def getYNominalflexuralStrength(self, bendingState):
        ''' Return the nominal flexural strength
            around z axis.

            :param bendingState: bending moments along the member.
        '''
        cb= self.connection.getLateralTorsionalBucklingModificationFactor(bendingState)
        return self.shape.getNominalFlexuralStrength(lateralUnbracedLength= self.connection.Lb, Cb= cb, majorAxis= False)
    
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
