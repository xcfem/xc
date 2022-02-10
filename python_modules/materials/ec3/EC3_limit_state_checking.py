# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 3. '''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import math
from misc_utils import log_messages as lmsg
import numpy
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from postprocess import limit_state_data as lsd

# Lateral torsional buckling of steel beams.
# References:
# [1]: LATERAL-TORSIONAL BUCKLING OF STEEL BEAMS: A GENERAL EXPRESSION FOR THE MOMENT GRADIENT FACTOR
#      Aitziber López, Danny J. Yong and Miguel A. Serna. STABILITY AND DUCTILITY OF STEEL STRUCTURES
#      Lisbon, Portugal, September 6-8, 2006

def getLateralTorsionalBucklingCurve(steelShape):
    ''' Returns the lateral torsional bukling curve name (a,b,c or d) depending of the type of section (rolled, welded,...). EC3 EN 1993-1-1 Table 6.4, 6.3.2.2(2).
    :param steelShape: cross section shape.
    :param rypo: 'rolled' or 'welded' shape
    '''
    if(steelShape.typo=='rolled'):
        if((steelShape.h()/steelShape.b())<=2):
            return 'a'
        else:
            return 'b'
    elif(steelShape.typo=='welded'):
        if((steelShape.h()/steelShape.b())<=2):
            return 'c'
        else:
            return 'd'
    else:
        return 'd'

def shearBucklingVerificationNeeded(steelShape):
    '''Returns true if shear buckling verification is needed EC3-1-5

    :param steelShape: cross section shape.'''
    epsilon= math.sqrt(235e6/steelShape.steelType.fy)
    eta= 1.0 #Conservative
    f1= steelShape.hw()/steelShape.tw()
    f2= 72*epsilon/eta
    return (f1>f2)

def getBendingResistanceReductionCoefficient(steelShape,Vd):
    '''Returns bending resistance reduction coefficient as in
    clause 6.2.8 of EC3 EN 1993-1-1

    :param steelShape: cross section shape.
    '''
    VplRd= steelShape.getVplRdy()
    ratio= Vd/VplRd
    if(ratio<=0.5):
        return 0.0 #No reduction
    else:
        return (2*ratio-1)**2

def getMvRdz(steelShape,sectionClass,Vd):
    '''Returns the major bending resistance of the cross-section under a
    shear force of Vd.

    param steelShape: cross section shape.
    '''
    McRdz= steelShape.getMcRdz(sectionClass)
    reductionCoeff= steelShape.getBendingResistanceReductionCoefficient(Vd)
    if(reductionCoeff<=0.0):
        return McRdz
    else:
        Aw= steelShape.hw()*steelShape.tw()
        sustr= reductionCoeff*Aw**2/4.0/steelShape.tw()
        return min((steelShape.getWz(sectionClass)-sustr)*steelShape.steelType.fy/steelShape.steelType.gammaM0(),McRdz)

def getLateralBucklingImperfectionFactor(steelShape):
    ''' Returns lateral torsional imperfection factor depending of the type of section 
    (rolled, welded,...).

    :param steelShape: cross section shape.
    '''
    curve= getLateralTorsionalBucklingCurve(steelShape)
    if(curve=='A' or curve=='a'):
        return 0.21
    elif(curve=='B' or curve=='b'):
        return 0.34
    elif(curve=='C' or curve=='c'):
        return 0.49
    elif(curve=='D' or curve=='d'):
        return 0.76
    else:
        return 0.76

class SupportCoefficients(object):

  def __init__(self,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0):
    ''' Constructor

     :param ky: lateral bending coefficient ky= 1.0 => free lateral bending
                                            ky= 0.5 => prevented lateral bending
     :param kw: warping coefficient kw= 1.0 => free warping
                                    ky= 0.5 => prevented warping
     :param k1: warping AND lateral bending coefficient at left end
                                    k1= 1.0 => free warping AND lateral bending
                                    k1= 0.5 => prevented warp. AND lateral bending
     :param k2: warping AND lateral bending coefficient at right end
                                    k2= 1.0 => free warping AND lateral bending
                                    k2= 0.5 => prevented warp. AND lateral bending.
    '''

    self.ky= ky
    self.kw= kw
    self.k1= k1
    self.k2= k2
    
  def getAlphaI(self):
    ''' returns the five alpha values that are needed for C1 calculation according
        to equation 12 of the reference [1].'''
    return [(1.0-self.k2),5*self.k1**3/self.k2**2,5*(1.0/self.k1+1.0/self.k2),5*self.k2**3/self.k1**2,(1.0-self.k1)]

def getLateralBucklingIntermediateFactor(steelShape,sectionClass,L,Mi,supportCoefs= SupportCoefficients()):
    ''' Returns lateral torsional buckling intermediate factor value.

    :param steelShape: cross section shape.
    :param Mi: ordinate for the moment diagram
    :param supportCoefs: coefficients that represent support conditions.
    '''
    alphaLT= steelShape.getLateralBucklingImperfectionFactor()
    overlineLambdaLT= steelShape.getLateralBucklingNonDimensionalBeamSlenderness(sectionClass,L,Mi,supportCoefs)
    return 0.5*(1+alphaLT*(overlineLambdaLT-0.2)+overlineLambdaLT**2)

def getLateralBucklingReductionFactor(steelShape,sectionClass,L,Mi,supportCoefs= SupportCoefficients()):
    ''' Returns lateral torsional buckling reduction factor value.

    :param steelShape: cross section shape.
    :param sectionClass: section classification (1 to 3, 4 not yet implemented)
    :param Mi: ordinate for the moment diagram
    :param supportCoefs: coefficients that represent support conditions.
    '''  
    phiLT= steelShape.getLateralBucklingIntermediateFactor(sectionClass,L,Mi,supportCoefs)
    overlineLambdaLT= steelShape.getLateralBucklingNonDimensionalBeamSlenderness(sectionClass,L,Mi,supportCoefs)
    return min(1.0,1.0/(phiLT+math.sqrt(phiLT**2-overlineLambdaLT**2)))

def getLateralTorsionalBucklingResistance(steelShape,sectionClass,L,Mi,supportCoefs= SupportCoefficients()):
    '''Returns lateral torsional buckling resistance of this cross-section.
    Calculation is made following the paper:

    A. López, D. J. Yong, M. A. Serna,
    Lateral-torsional buckling of steel beams: a general expression for
    the moment gradient factor.
    (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

    :param steelShape: cross section shape.
    :param sectionClass: section classification (1,2,3 or 4)
    :param Mi: ordinate for the moment diagram
    :param supportCoefs: coefficients that represent support conditions.
    '''  
    chiLT= steelShape.getLateralBucklingReductionFactor(sectionClass,L,Mi,supportCoefs)
    return chiLT*steelShape.getMcRdz(sectionClass)

def getMcr(steelShape,L,Mi,supportCoefs= SupportCoefficients()):
    '''Returns elastic critical moment about minor axis: y
    Calculation is made following the paper:

    A. López, D. J. Yong, M. A. Serna,
    Lateral-torsional buckling of steel beams: a general expression for
    the moment gradient factor.
    (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

    :param steelShape: cross section shape.
    :param Mi: ordinate for the moment diagram
    :param supportCoefs: coefficients that represent support conditions.
    '''
    mgf= MomentGradientFactorC1(Mi)
    C1= mgf.getC1(supportCoefs)
    pi2EIy= math.pi**2*steelShape.EIy()
    GIt= steelShape.GJ()
    kyL2= (supportCoefs.ky*L)**2
    Mcr0= pi2EIy/kyL2
    sum1= (supportCoefs.ky/supportCoefs.kw)**2*steelShape.Iw()/steelShape.Iy()
    sum2= GIt/Mcr0
    f2= math.sqrt(sum1+sum2)
    # print('  L= ', L)
    # print('  kyL2= ', kyL2)
    # print('  GJ= ', GIt/1e3)
    # print('  Iw= ', steelShape.Iw()*100**6, ' cm^6')
    # print('  C1= ', C1)
    # print('  Mcr0=', Mcr0/1e3  )
    # print('  sum1= ', sum1)
    # print('  sum2= ', sum2)
    # print('  f2= ', f2)
    return C1*Mcr0*f2

def getLateralBucklingNonDimensionalBeamSlenderness(steelShape,sectionClass,L,Mi,supportCoefs= SupportCoefficients()):
    '''Returns non dimensional beam slenderness
    for lateral torsional buckling
    see parameter definition on method getMcr.

    :param steelShape: cross section shape.
    :param sectionClass: section classification (1,2,3 or 4)
    :param Mi: ordinate for the moment diagram
    :param supportCoefs: coefficients that represent support conditions.
    '''
    Mcr= steelShape.getMcr(L,Mi,supportCoefs)
    return math.sqrt(steelShape.getWz(sectionClass)*steelShape.steelType.fy/Mcr)

class MomentGradientFactorC1(object):
    ''' Calculation of the C1 moment gradient factor as defined
    in: A. López, D. J. Yong, M. A. Serna,
    Lateral-torsional buckling of steel beams: a general expression for
    the moment gradient factor.
    (Lisbon, Portugal: Stability and ductility of steel structures, 2006). '''
    def __init__(self,Mi):
        self.Mi= Mi
  
    def getExtremeMoment(self):
        ''' Return the extreme of the bending moments (maximum or minimum). '''
        mMax= max(self.Mi)
        mMin= min(self.Mi)
        retval= mMax
        if(abs(retval)<abs(mMin)):
            retval= mMin
        return retval

    def getA2(self):
        '''Return the value for the A2 coefficient according to equation 11
           of the reference [1].'''
        return (self.Mi[0]+2*self.Mi[1]+3*self.Mi[2]+2*self.Mi[3]+self.Mi[4])/(9*self.getExtremeMoment())

    def getA1(self,supportCoefs):
        '''Return the value for the A1 coefficient according to equation 10
           of the reference [1].
           
        :param k1: warping AND lateral bending coefficient at left end
                                   k1= 1.0 => free warping AND lateral bending
                                   k1= 0.5 => prevented warp. AND lateral bending
        :param k2: warping AND lateral bending coefficient at right end
                                   k2= 1.0 => free warping AND lateral bending
                                   k2= 0.5 => prevented warp. AND lateral bending'''
        ai= supportCoefs.getAlphaI()
        Mmax2= self.getExtremeMoment()**2
        return (Mmax2+ai[0]*self.Mi[0]**2+ai[1]*self.Mi[1]**2+ai[2]*self.Mi[2]**2+ai[3]*self.Mi[3]**2+ai[4]*self.Mi[4]**2)/((1+ai[0]+ai[1]+ai[2]+ai[3]+ai[4])*Mmax2)

    def getC1(self,supportCoefs):
        '''Return the value for the C1 coefficient according to equation 8
           of the reference [1]. 

        :param k1: warping AND lateral bending coefficient at left end
                                   k1= 1.0 => free warping AND lateral bending
                                   k1= 0.5 => prevented warp. AND lateral bending
        :param k2: warping AND lateral bending coefficient at right end
                                   k2= 1.0 => free warping AND lateral bending
                                   k2= 0.5 => prevented warp. AND lateral bending'''
        k= math.sqrt(supportCoefs.k1*supportCoefs.k2) # equation 9
        A1= self.getA1(supportCoefs)
        A2= self.getA2()
        rootK= math.sqrt(k) 
        B1= rootK*A1+((1-rootK)/2.0*A2)**2
        return (math.sqrt(B1)+(1-rootK)/2.0*A2)/A1 # equation 8

class BiaxialBendingNormalStressController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls normal stresses limit state.'''

    ControlVars= cv.SteelShapeBiaxialBendingControlVars
    def __init__(self,limitStateLabel):
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)
    
    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the steel shape argument
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get section properties.
        steelShape= elem.getProp('crossSection')
        sectionClass= elem.getProp('sectionClass')
        # Check each element section.
        for lf in elementInternalForces: 
            # Compute efficiency.
            CFtmp,NcRdtmp,McRdytmp,McRdztmp,MvRdztmp,MbRdztmp= steelShape.getBiaxialBendingEfficiency(sectionClass,lf.N,lf.My,lf.Mz,lf.Vy,lf.chiLT)
            sectionLabel= self.getSectionLabel(lf.idSection)
            label= self.limitStateLabel+sectionLabel
            # Update efficiency.
            if(CFtmp>elem.getProp(label).CF):
                elem.setProp(label, self.ControlVars(sectionLabel+'s',lf.idComb,CFtmp,lf.N,lf.My,lf.Mz,NcRdtmp,McRdytmp,McRdztmp,MvRdztmp,MbRdztmp,lf.chiLT))
        

class ShearController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls shear limit state.'''

    ControlVars= cv.ShearYControlVars
    def __init__(self,limitStateLabel):
        super(ShearController,self).__init__(limitStateLabel)

    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the steel shape argument
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get section properties.
        steelShape= elem.getProp('crossSection')
        sectionClass= elem.getProp('sectionClass')
        # Check each element section.
        for lf in elementInternalForces: 
            # Compute efficiency.
            CFtmp= steelShape.getYShearEfficiency(sectionClass,lf.Vy)
            sectionLabel= self.getSectionLabel(lf.idSection)
            label= self.limitStateLabel+sectionLabel
            # Update efficiency.
            if (CFtmp>elem.getProp(label).CF):
                elem.setProp(label, self.ControlVars(sectionLabel+'s',lf.idComb,CFtmp,lf.Vy))

# Routines to install in recorder  to execute in every commit to check
# eurocode-3 criterions.

def controlBiaxialBendingEfficiency():
  '''Code to execute in every commit to check stress criterion (bars in 3D problems).'''
  return '''nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkBiaxialBendingForElement(self,nmbComb)'''

def controlYShearEfficiency():
  return '''nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkYShearForElement(self,nmbComb)'''

def controlULSCriterion():
  return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkBiaxialBendingForElement(self,nmbComb)
crossSection.checkYShearForElement(self,nmbComb)'''

def controlULSCriterion2D():
  return '''recorder= self.getProp('ULSControlRecorder')
nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
crossSection= self.getProp('crossSection')
crossSection.checkUniaxialBendingForElement(self,nmbComb)
crossSection.checkYShearForElement(self,nmbComb)'''

