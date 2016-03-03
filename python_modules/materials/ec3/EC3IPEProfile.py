# -*- coding: utf-8 -*-

'''
European I beams

WARNING Axis definition different from those of Eurocode 3:

   ARCELOR          XC

                    ^ Y                    
                    |

    -----         -----
      |             | 
      | -> Y        | -> Z
      |             |
    -----         -----

      |
      v Z

So:
   Z is the cross-section MAJOR axis in XC (minor in EC3).
   Y is the cross-section MINOR axis in XC (major in EC3).

'''

from __future__ import division
import math
from miscUtils import LogMessages as lmsg
from materials.perfiles_metalicos.arcelor import perfiles_ipe_arcelor as ipe
from materials.ec3 import lateral_torsional_buckling as ltb
from materials.ec3 import EC3_callback_controls as EC3cc

class EC3IPEProfile(ipe.IPEProfile):
  """IPE profile with Eurocode 3 verification routines."""
  def __init__(self,steel,name):
    super(EC3IPEProfile,self).__init__(steel,name)
  def getBucklingCurve(self,typo= 'rolled'):
    ''' Returns the bukling curve name (a,b,c or d) depending of the type of section (rolled, welded,...).'''
    if(typo=='rolled'):
      if((self.h()/self.b())<=2):
        return 'a'
      else:
        return 'b'
    elif(typo=='welded'):
      if((self.h()/self.b())<=2):
        return 'c'
      else:
        return 'd'
    else:
      return 'd'
  def getAvy(self):
    '''Returns y direction (web direction) shear area'''
    return self.get('Avy')
  def shearBucklingVerificationNeeded(self):
    '''Returns true if shear buckling verification is needed EC3-1-5'''
    epsilon= math.sqrt(235e6/self.steelType.fy)
    eta= 1.0 #Conservative
    f1= self.hw()/self.tw()
    f2= 72*epsilon/eta
    return (f1>f2)
  def getVplRdy(self):
    '''Returns y direction (web direction) plastic shear resistance'''
    if(self.shearBucklingVerificationNeeded()):
      lmsg.warning('section needs shear buckling verification.')
    return self.getAvy()*(self.steelType.fy/math.sqrt(3))/self.steelType.gammaM0()
  def getVcRdy(self):
    '''Returns y direction (web direction) shear resistance'''
    return self.getVplRdy()
  def getBendingResistanceReductionCoefficient(self,Vd):
    '''Returns bending resistance reduction coefficient as in
       clause 6.2.8 of EC31-1'''
    VplRd= self.getVplRdy()
    ratio= Vd/VplRd
    if(ratio<=0.5):
      return 0.0 #No reduction
    else:
      return (2*ratio-1)**2
  def getNcRd(self,sectionClass):
    '''Returns the axial compression resistance of the cross-section.'''
    return self.getAeff(sectionClass)*self.steelType.fy/self.steelType.gammaM0()
  def getMcRdy(self,sectionClass):
    '''Returns the minor bending resistance of the cross-section.'''
    return self.getWy(sectionClass)*self.steelType.fy/self.steelType.gammaM0()
  def getMcRdz(self,sectionClass):
    '''Returns the major bending resistance of the cross-section.'''
    return self.getWz(sectionClass)*self.steelType.fy/self.steelType.gammaM0()
  def getMvRdz(self,sectionClass,Vd):
    '''Returns the major bending resistance of the cross-section under a
       shear force of Vd.'''
    McRdz= self.getMcRdz(sectionClass)
    reductionCoeff= self.getBendingResistanceReductionCoefficient(Vd)
    if(reductionCoeff<=0.0):
      return McRdz
    else:
      Aw= self.hw()*self.tw()
      sustr= reductionCoeff*Aw**2/4.0/self.tw()
      return min((self.getWz(sectionClass)-sustr)*self.steelType.fy/self.steelType.gammaM0(),McRdz)


  def getLateralBucklingImperfectionFactor(self,typo= 'rolled'):
    ''' Returns lateral torsional imperfection factor depending of the type of section (rolled, welded,...).'''
    curve= self.getBucklingCurve(typo= 'rolled')
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
  def getLateralBucklingIntermediateFactor(self,sectionClass,xi,Mi,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0,typo= 'rolled'):
    ''' Returns lateral torsional buckling intermediate factor value.'''
    alphaLT= self.getLateralBucklingImperfectionFactor(typo)
    overlineLambdaLT= self.getLateralBucklingNonDimensionalBeamSlenderness(sectionClass,xi,Mi,ky,kw, k1, k2)
    return 0.5*(1+alphaLT*(overlineLambdaLT-0.2)+overlineLambdaLT**2)
  def getLateralBucklingReductionFactor(self,sectionClass,xi,Mi,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0,typo= 'rolled'):
    ''' Returns lateral torsional buckling reduction factor value.'''  
    phiLT= self.getLateralBucklingIntermediateFactor(sectionClass,xi,Mi,ky,kw, k1, k2,typo)
    overlineLambdaLT= self.getLateralBucklingNonDimensionalBeamSlenderness(sectionClass,xi,Mi,ky,kw, k1, k2)
    return 1.0/(phiLT+math.sqrt(phiLT**2-overlineLambdaLT**2))
  def getLateralTorsionalBucklingResistance(self,sectionClass,xi,Mi,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0,typo= 'rolled'):
    '''Returns lateral torsional buckling resistance of this cross-section.
       Calculation is made following the paper:

       A. López, D. J. Yong, M. A. Serna,
       Lateral-torsional buckling of steel beams: a general expression for
       the moment gradient factor.
       (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

       Parameters:
       xi: abcissae for the moment diagram
       Mi: ordinate for the moment diagram
       ky: lateral bending coefficient ky= 1.0 => free lateral bending
                                       ky= 0.5 => prevented lateral bending
       kw: warping coefficient kw= 1.0 => free warping
                               ky= 0.5 => prevented warping
       k1: warping AND lateral bending coefficient at left end
                               k1= 1.0 => free warping AND lateral bending
                               k1= 0.5 => prevented warp. AND lateral bending
       k2: warping AND lateral bending coefficient at right end
                               k2= 1.0 => free warping AND lateral bending
                               k2= 0.5 => prevented warp. AND lateral bending'''

    chiLT= self.getLateralBucklingReductionFactor(sectionClass,xi,Mi,ky,kw, k1, k2,typo)
    return chiLT*self.getMcRdz(sectionClass)
  def getMcr(self,xi,Mi,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0):
    '''Returns elastic critical moment about minor axis: y
       Calculation is made following the paper:

       A. López, D. J. Yong, M. A. Serna,
       Lateral-torsional buckling of steel beams: a general expression for
       the moment gradient factor.
       (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

       Parameters:
       xi: abcissae for the moment diagram
       Mi: ordinate for the moment diagram
       ky: lateral bending coefficient ky= 1.0 => free lateral bending
                                       ky= 0.5 => prevented lateral bending
       kw: warping coefficient kw= 1.0 => free warping
                               ky= 0.5 => prevented warping
       k1: warping AND lateral bending coefficient at left end
                               k1= 1.0 => free warping AND lateral bending
                               k1= 0.5 => prevented warp. AND lateral bending
       k2: warping AND lateral bending coefficient at right end
                               k2= 1.0 => free warping AND lateral bending
                               k2= 0.5 => prevented warp. AND lateral bending'''
    mgf= ltb.MomentGradientFactorC1(xi,Mi)
    L= mgf.getL()
    C1= mgf.getC1(k1,k2)
    pi2EIy= math.pi**2*self.EIy()
    GIt= self.GJ()
    kyL2= (ky*L)**2
    Mcr0= pi2EIy/kyL2
    sum1= (ky/kw)**2*self.Iw()/self.Iy()
    sum2= GIt/Mcr0
    f2= math.sqrt(sum1+sum2)
    # print '  L= ', L
    # print '  kyL2= ', kyL2
    # print '  GJ= ', GIt/1e3
    # print '  Iw= ', self.Iw()*100**6, ' cm^6'
    # print '  C1= ', C1
    # print '  Mcr0=', Mcr0/1e3  
    # print '  sum1= ', sum1
    # print '  sum2= ', sum2
    # print '  f2= ', f2
    return C1*Mcr0*f2

  def getLateralBucklingNonDimensionalBeamSlenderness(self,sectionClass,xi,Mi,ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0):
    '''Returns non dimensional beam slenderness
       for lateral torsional buckling
       see parameter definition on method getMcr.'''
    Mcr= self.getMcr(xi,Mi,ky,kw, k1, k2)
    return math.sqrt(self.getWz(sectionClass)*self.steelType.fy/Mcr)

  def getYShearEfficiency(self,sectionClass,Vyd):
    '''Returns major axis shear efficiency'''
    return abs(Vyd/self.getVcRdy())

  def getZBendingEfficiency(self,sectionClass,Mzd,Vyd= 0.0, chiLT= 1.0):
    '''Returns major axis bending efficiency
       chiLT: lateral bucling reduction factor (default= 1.0).
    '''
    MvRdz= self.getMvRdz(sectionClass,Vyd)
    MbRdz= chiLT*MvRdz #Lateral buckling reduction.
    return abs(Mzd)/MbRdz

  def getBiaxialBendingEfficiency(self,sectionClass,Nd,Myd,Mzd,Vyd= 0.0,chiLT=1.0):
    '''Returns biaxial bending efficiency (clause 6.2.9 of EC3.1.1)
       chiLT: lateral buckling reduction factor (default= 1.0).
    '''
    NcRd= self.getNcRd(sectionClass)
    McRdy= self.getMcRdy(sectionClass)
    MvRdz= self.getMvRdz(sectionClass,Vyd)
    MbRdz= chiLT*MvRdz #Lateral buckling reduction.
    alpha= 2.0
    beta= max(1.0,Nd/NcRd)
    return (abs(Mzd)/MbRdz)**alpha+(abs(Myd)/McRdy)**beta

  def setupULSControlVars(self,elems,sectionClass= 1, chiLT=1.0):
    '''For each element creates the variables
       needed to check ultimate limit state criterion to satisfy.'''
    super(EC3IPEProfile,self).setupULSControlVars(elems)
    for e in elems:
      e.setProp('sectionClass',sectionClass) #Cross section class.
      e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.
      e.setProp('crossSection',self)

  def installULSControlRecorder(self,recorderType, elems,sectionClass= 1, chiLT=1.0):
    '''Installs recorder for verification of ULS criterion. Preprocessor obtained from the set of elements.'''
    preprocessor= elems.owner.getPreprocessor
    nodes= preprocessor.getNodeLoader
    domain= preprocessor.getDomain
    recorder= domain.newRecorder(recorderType,None);
    recorder.setElements(elems.getTags())
    for e in elems:
      e.setProp('ULSControlRecorder',recorder)
    self.setupULSControlVars(elems,sectionClass,chiLT)
    if(nodes.numGdls==3):
      recorder.callbackRecord= '''print \'ERROR in element: , self.tag, bidimensional criterion not implemented.\''''
    else:
      recorder.callbackRecord= EC3cc.controlULSCriterion()

    recorder.callbackRestart= "print \"Restart method called.\""
    return recorder
