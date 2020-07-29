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
import scipy.interpolate
from materials import steel_base
from misc_utils import log_messages as lmsg
from materials import buckling_base
from materials.astm_aisc import AISC_limit_state_checking as aisc

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

class AnchorBolt(object):
    """ASTM anchor bolt accordint to table 2.2 from the document
    Base Plate and Anchor Rod Design Second Edition
    American Institute of Steel Construction, Inc."""

    # See table 3.2 of the design guide:
    diams= [0.015875, 0.01905, 0.022225, 0.0254, 0.028575, 0.03175, 0.0381, 0.04445, 0.0508, 0.05715, 0.0635, 0.06985, 0.0762, 0.08255, 0.0889, 0.09525, 0.1016]
    bearingArea=[0.00044451524, 0.00058451496, 0.0007870952, 0.00096774, 0.0011677396, 0.0014451584, 0.0020193508, 0.0026903172, 0.003451606, 0.0043161204, 0.0052709572, 0.006322568, 0.007354824, 0.008580628, 0.009870948, 0.0112903, 0.012838684]
    fBearingArea= scipy.interpolate.interp1d(diams,bearingArea)

    def __init__(self, name, steel, diameter):
       '''
         Constructor.
       '''
       self.name=name
       self.steelType= steel
       self.diameter= diameter

    def getArea(self):
        ''' Return the area of the anchor rod.
        '''
        return math.pi*(self.diameter/2.0)**2

    # Tension
    def getTensileStrength(self):
        ''' Return the tensile strength of the anchor rod.
        '''
        Ag= self.getArea()
        return self.steelType.fu*Ag
    
    def getNominalTensileStrength(self):
        ''' Return the tensile strength of the anchor rod.
        '''
        return 0.75*self.getTensileStrength()
    
    def getNominalShearStrength(self, type= 'N'):
        ''' Return the shear strength of the anchor rod.
        '''
        factor= 0.4
        if(type=='X'):
            factor= 0.5
        return factor*self.getTensileStrength()
    
    def getDesignTensileStrength(self, phi= 0.75):
        ''' Return the tensile strength of the anchor rod.

        :param phi: resistance factor for anchor steel  
                in tension (defaults to 0.75).
        '''
        return phi*self.getNominalTensileStrength()

    def getBearingArea(self):
        ''' Return the bearing area of the anchor according
            to the table 3.2 of the design guide.'''
        return self.fBearingArea(self.diameter)
    
    def getNominalPulloutStrength(self, fc, psi4= 1.0):
        ''' Return the nominal pullout strength of the anchor 
        rod based on the ACI Appendix D provisions (Section D5.3)

        :param fc: concrete strength.
        :param psi4: 1.4 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking (f t – f r ) at service levels, 
                     otherwise 1.0
        '''
        Abrg= self.getBearingArea() # the bearing area of the anchor
                                    # rod head or nut.
        return psi4*Abrg*8*fc
    
    def getDesignPulloutStrength(self, fc, psi4= 1.0, phi= 0.7):
        ''' Return the design pullout strength of the anchor 
        rod based on the ACI Appendix D provisions (Section D5.3)

        :param fc: concrete strength.
        :param psi4: 1.4 if the anchor is located in a region of a
                     concrete member where analysis indicates no
                     cracking (f t – f r ) at service levels, 
                     otherwise 1.0
        '''
        return phi*self.getNominalPulloutStrength(fc,psi4)

    
        


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
        E= self.get('E')
        return math.pi**2*E/sr**2
    
    def getCriticalStressE(self, effectiveLengthY, effectiveLengthZ, sectionClassif, Fe):
        ''' Return the critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLegnthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        :param Fe: flexural or torsional elastic buckling stress.
        '''
        retval= 0.0
        if(sectionClassif<aisc.SectionClassif.slender):
            sr= self.getFlexuralSlendernessRatio(effectiveLengthY, effectiveLengthZ)
            E= self.get('E')
            Fy= self.steelType.fy
            Fratio= Fy/Fe
            thresholdA= 4.71*math.sqrt(E/Fy)
            if((sr<=thresholdA) or (Fratio<=2.25)):
                retval= math.pow(0.658,Fratio)*Fy # (E3-2)
            else:
                retval= 0.877*Fe
        else:
            lmsg.error('Critical stress of slender members not implemented yet.')
        return retval
    
    def getFlexuralCriticalStress(self, effectiveLengthY, effectiveLengthZ, sectionClassif):
        ''' Return the flexural critical stress of the member according
            to equations E3-2 and E3-3 of AISC-360-16.

        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLegnthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        Fe= self.getFlexuralElasticBucklingStress(effectiveLengthY, effectiveLengthZ)
        return self.getCriticalStressE(effectiveLengthY, effectiveLengthZ, sectionClassif,Fe)
    
    def getTorsionalElasticBucklingStress(self, effectiveLengthX):
        ''' Return the torsional or flexural-torsional elastic buckling stress
            of the member according to equations E4-2, E4-3 and E4-4 of 
            AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        '''
        return getShapeTorsionalElasticBucklingStress(self, effectiveLengthX)
    
    def getTorsionalCriticalStress(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif):
        ''' Return the torsional critical stress of the member according
            to equations E4-2, E4-3 and E4-4 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLegnthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        Fe= self.getTorsionalElasticBucklingStress(effectiveLengthX)
        return self.getCriticalStressE(effectiveLengthY= effectiveLengthY, effectiveLengthZ= effectiveLengthZ, sectionClassif= sectionClassif, Fe= Fe)

    def getNominalCompressiveStrength(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif):
        ''' Return the nominal compressive strength of the member
            according to equation E3-1 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLegnthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        Ag= self.shape.get('A') # Gross area of member
        retval= 0.0
        if(effectiveLengthX<= max(effectiveLengthY,effectiveLengthZ)):
            retval= self.getFlexuralCriticalStress(effectiveLengthY, effectiveLengthZ, sectionClassif)*Ag
        else:
            retval= self.getTorsionalCriticalStress(effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif)*Ag
        return retval
    
    def getDesignCompressiveStrength(self, effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif):
        ''' Return the design compressive strength of the member
            according to section E1 of AISC-360-16.

        :param effectiveLengthX: effective length of member (torsion).
        :param effectiveLengthY: effective length of member (minor axis).
        :param effectiveLegnthZ: effective length of member (major axis).
        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        return 0.9*self.getNominalCompressiveStrength(effectiveLengthX, effectiveLengthY, effectiveLengthZ, sectionClassif)
    
    def getReferenceCompressiveStrength(self, sectionClassif):
        ''' Return the flexural strength of the section without
            take in to account the lateral buckling effect.

        :param sectionClassif: section classification compact, noncompact, slender or too slender.
        '''
        return self.getDesignCompressiveStrength(effectiveLengthX= 0.1, effectiveLengthY= 0.1, effectiveLengthZ= 0.1,sectionClassif= sectionClassif)
    # Bending 
    def getReferenceFlexuralStrength(self):
        ''' Return the flexural strength of the section without
            take in to account the lateral buckling effect.'''
        return self.getDesignFlexuralStrength(lateralUnbracedLength= 0.1, Cb= 1.0, majorAxis= True)

    # Shear
    def getYShearEfficiency(self, sectionClass, Vy):
        '''Return major axis shear efficiency according to AISC-360-16.

           :param sectionClass: dummy argument used for compatibility.
        '''
        return abs(Vy)/self.getDesignShearStrength(majorAxis= True)
    
    def getZShearEfficiency(self, sectionClass, Vz):
        '''Return major axis shear efficiency according to AISC-360-16.

           :param sectionClass: dummy argument used for compatibility.
        '''
        return Vz/self.getDesignShearStrength(majorAxis= False)
        

    # Combined internal forces
    def getBiaxialBendingEfficiency(self,sectionClassif,Nd,Myd,Mzd,Vyd= 0.0, chiN=1.0, chiLT=1.0):
        '''Return biaxial bending efficiency according to section H1
           of AISC-360-16.

           :param sectionClassif: section classification compact, noncompact, slender or too slender.
           :param chiN: axial load reduction reduction factor (default= 1.0).
           :param chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        ratioN= 0.0
        if(Nd<0): # compression
            NcRd= chiN*self.getReferenceCompressiveStrength(sectionClassif) # available axial strength.
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

    def setupULSControlVars(self,elems,sectionClass= 1, chiN=1.0, chiLT=1.0):
        '''For each element creates the variables
           needed to check ultimate limit state criterion to be satisfied.'''
        super(ASTMShape,self).setupULSControlVars(elems)
        for e in elems:
            e.setProp('sectionClass',sectionClass) #Cross section class.
            e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.
            e.setProp('chiN',chiN) # Axial strength reduction factor.
            e.setProp('crossSection',self)
            
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

class ConnectedMember(object):
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
