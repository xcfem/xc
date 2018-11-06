# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 3. '''
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from miscUtils import LogMessages as lmsg
import scipy.interpolate
import numpy

# Lateral torsional buckling of steel beams.

def getLateralTorsionalBucklingCurve(shape):
  ''' Returns the lateral torsional bukling curve name (a,b,c or d) depending of the type of section (rolled, welded,...). EC3 EN 1993-1-1 Table 6.4, 6.3.2.2(2).
  :param shape: cross section shape.
  :param rypo: 'rolled' or 'welded' shape
  '''
  if(shape.typo=='rolled'):
    if((shape.h()/shape.b())<=2):
      return 'a'
    else:
      return 'b'
  elif(shape.typo=='welded'):
    if((shape.h()/shape.b())<=2):
      return 'c'
    else:
      return 'd'
  else:
    return 'd'

def shearBucklingVerificationNeeded(shape):
  '''Returns true if shear buckling verification is needed EC3-1-5
  :param shape: cross section shape.'''
  epsilon= math.sqrt(235e6/shape.steelType.fy)
  eta= 1.0 #Conservative
  f1= shape.hw()/shape.tw()
  f2= 72*epsilon/eta
  return (f1>f2)

def getBendingResistanceReductionCoefficient(shape,Vd):
  '''Returns bending resistance reduction coefficient as in
     clause 6.2.8 of EC3 EN 1993-1-1
  :param shape: cross section shape.
  '''
  VplRd= shape.getVplRdy()
  ratio= Vd/VplRd
  if(ratio<=0.5):
    return 0.0 #No reduction
  else:
    return (2*ratio-1)**2

def getMvRdz(shape,sectionClass,Vd):
  '''Returns the major bending resistance of the cross-section under a
     shear force of Vd.
    :param shape: cross section shape.
  '''
  McRdz= shape.getMcRdz(sectionClass)
  reductionCoeff= shape.getBendingResistanceReductionCoefficient(Vd)
  if(reductionCoeff<=0.0):
    return McRdz
  else:
    Aw= shape.hw()*shape.tw()
    sustr= reductionCoeff*Aw**2/4.0/shape.tw()
    return min((shape.getWz(sectionClass)-sustr)*shape.steelType.fy/shape.steelType.gammaM0(),McRdz)

def getLateralBucklingImperfectionFactor(shape):
  ''' Returns lateral torsional imperfection factor depending of the type of section (rolled, welded,...).
    :param shape: cross section shape.
  '''
  curve= getLateralTorsionalBucklingCurve(shape)
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
    ''' returns the five alpha values that are needed for C1 calculation.'''
    return [(1.0-self.k2),5*self.k1**3/self.k2**2,5*(1.0/self.k1+1.0/self.k2),5*self.k2**3/self.k1**2,(1.0-self.k1)]

def getLateralBucklingIntermediateFactor(shape,sectionClass,xi,Mi,supportCoefs= SupportCoefficients()):
  ''' Returns lateral torsional buckling intermediate factor value.

     :param shape: cross section shape.
     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
  '''
  alphaLT= shape.getLateralBucklingImperfectionFactor()
  overlineLambdaLT= shape.getLateralBucklingNonDimensionalBeamSlenderness(sectionClass,xi,Mi,supportCoefs)
  return 0.5*(1+alphaLT*(overlineLambdaLT-0.2)+overlineLambdaLT**2)

def getLateralBucklingReductionFactor(shape,sectionClass,xi,Mi,supportCoefs= SupportCoefficients()):
  ''' Returns lateral torsional buckling reduction factor value.

     :param shape: cross section shape.
     :param sectionClass: section classification (1,2,3 or 4)
     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
  '''  
  phiLT= shape.getLateralBucklingIntermediateFactor(sectionClass,xi,Mi,supportCoefs)
  overlineLambdaLT= shape.getLateralBucklingNonDimensionalBeamSlenderness(sectionClass,xi,Mi,supportCoefs)
  return 1.0/(phiLT+math.sqrt(phiLT**2-overlineLambdaLT**2))

def getLateralTorsionalBucklingResistance(shape,sectionClass,xi,Mi,supportCoefs= SupportCoefficients()):
  '''Returns lateral torsional buckling resistance of this cross-section.
     Calculation is made following the paper:

     A. López, D. J. Yong, M. A. Serna,
     Lateral-torsional buckling of steel beams: a general expression for
     the moment gradient factor.
     (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

     :param shape: cross section shape.
     :param sectionClass: section classification (1,2,3 or 4)
     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
  '''  
  chiLT= shape.getLateralBucklingReductionFactor(sectionClass,xi,Mi,supportCoefs)
  return chiLT*shape.getMcRdz(sectionClass)

def getMcr(shape,xi,Mi,supportCoefs= SupportCoefficients()):
  '''Returns elastic critical moment about minor axis: y
     Calculation is made following the paper:

     A. López, D. J. Yong, M. A. Serna,
     Lateral-torsional buckling of steel beams: a general expression for
     the moment gradient factor.
     (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

     :param shape: cross section shape.
     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
  '''
  mgf= MomentGradientFactorC1(xi,Mi)
  L= mgf.getL()
  C1= mgf.getC1(supportCoefs)
  pi2EIy= math.pi**2*shape.EIy()
  GIt= shape.GJ()
  kyL2= (supportCoefs.ky*L)**2
  Mcr0= pi2EIy/kyL2
  sum1= (supportCoefs.ky/supportCoefs.kw)**2*shape.Iw()/shape.Iy()
  sum2= GIt/Mcr0
  f2= math.sqrt(sum1+sum2)
  # print '  L= ', L
  # print '  kyL2= ', kyL2
  # print '  GJ= ', GIt/1e3
  # print '  Iw= ', shape.Iw()*100**6, ' cm^6'
  # print '  C1= ', C1
  # print '  Mcr0=', Mcr0/1e3  
  # print '  sum1= ', sum1
  # print '  sum2= ', sum2
  # print '  f2= ', f2
  return C1*Mcr0*f2

def getLateralBucklingNonDimensionalBeamSlenderness(shape,sectionClass,xi,Mi,supportCoefs= SupportCoefficients()):
  '''Returns non dimensional beam slenderness
     for lateral torsional buckling
     see parameter definition on method getMcr.

     :param shape: cross section shape.
     :param sectionClass: section classification (1,2,3 or 4)
     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
  '''
  Mcr= shape.getMcr(xi,Mi,supportCoefs)
  return math.sqrt(shape.getWz(sectionClass)*shape.steelType.fy/Mcr)

class MomentGradientFactorC1(object):
  ''' Calculation of the C1 moment gradient factor as defined
      in: A. López, D. J. Yong, M. A. Serna,
      Lateral-torsional buckling of steel beams: a general expression for
      the moment gradient factor.
      (Lisbon, Portugal: Stability and ductility of steel structures, 2006). '''
  def __init__(self,xi,Mi):
    self.xi= xi
    self.Mi= Mi
    self.momentDiagram= scipy.interpolate.interp1d(xi, Mi)
  
  def getL(self):
    ''' Returns the length of the moment diagram. '''
    return self.xi[-1]-self.xi[0]
  
  def getExtremeMoment(self):
    ''' Return the extreme of the bending moments (maximum or minimum). '''
    mMax= max(self.Mi)
    mMin= min(self.Mi)
    retval= mMax
    if(abs(retval)<abs(mMin)):
      retval= mMin
    return retval

  def getMi(self):
    ''' returns the five moment values that are needed for C1 calculation. '''
    nDiv= 4
    step= self.getL()/nDiv
    retval=list()
    xi= 0.0
    Mi= 0.0
    for i in range(0,nDiv+1):
      Mi= self.momentDiagram(xi)
      retval.append(float(Mi))
      xi+= step
    return retval

  def getA2(self):
    ''' return the value for the A2 coefficient. '''
    Mi= self.getMi()
    return (Mi[0]+2*Mi[1]+3*Mi[2]+2*Mi[3]+Mi[4])/(9*self.getExtremeMoment())

  def getA1(self,supportCoefs):
    ''' return the value for the A1 coefficient. 
       k1: warping AND lateral bending coefficient at left end
                               k1= 1.0 => free warping AND lateral bending
                               k1= 0.5 => prevented warp. AND lateral bending
       k2: warping AND lateral bending coefficient at right end
                               k2= 1.0 => free warping AND lateral bending
                               k2= 0.5 => prevented warp. AND lateral bending'''
    Mi= self.getMi()
    ai= supportCoefs.getAlphaI()
    Mmax2= self.getExtremeMoment()**2
    return (Mmax2+ai[0]*Mi[0]**2+ai[1]*Mi[1]**2+ai[2]*Mi[2]**2+ai[3]*Mi[3]**2+ai[4]*Mi[4]**2)/((1+ai[0]+ai[1]+ai[2]+ai[3]+ai[4])*Mmax2)

  def getC1(self,supportCoefs):
    ''' return the value for the C1 coefficient. 
       k1: warping AND lateral bending coefficient at left end
                               k1= 1.0 => free warping AND lateral bending
                               k1= 0.5 => prevented warp. AND lateral bending
       k2: warping AND lateral bending coefficient at right end
                               k2= 1.0 => free warping AND lateral bending
                               k2= 0.5 => prevented warp. AND lateral bending'''
    k= math.sqrt(supportCoefs.k1*supportCoefs.k2)
    rootK= math.sqrt(k)
    A1= self.getA1(supportCoefs)
    A2= self.getA2()
    B1= rootK*A1+((1-rootK)/2.0*A2)**2
    return (math.sqrt(B1)+(1-rootK)/2.0*A2)/A1

  
# Routines to install in recorder  to execute in every commit to check
# eurocode 3 criterions.

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

