# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
from miscUtils import LogMessages as lmsg
from materials.ec3 import lateral_torsional_buckling as ltb
from materials.ec3 import EC3_callback_controls as EC3cc

class EC3Profile(object):
  """Steel profile with Eurocode 3 verification routines."""
  def __init__(self,typo= 'rolled'):
    '''
      Constructor.

      :param typo: 'rolled' or 'welded' profile
    '''
    self.typo= typo
  def getLateralTorsionalBucklingCurve(self):
    ''' Returns the lateral torsional bukling curve name (a,b,c or d) depending of the type of section (rolled, welded,...). EC3 Table 6.4, 6.3.2.2(2).'''
    return ltb.getLateralTorsionalBucklingCurve(self)
  def getAvy(self):
    '''Returns y direction (web direction) shear area'''
    return self.get('Avy')
  def shearBucklingVerificationNeeded(self):
    '''Returns true if shear buckling verification is needed EC3-1-5'''
    return ltb.shearBucklingVerificationNeeded(self)
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
    return ltb.getBendingResistanceReductionCoefficient(self,Vd)
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
       shear force of Vd.

      :param sectionClass: section classification (1,2,3 or 4)
    '''
    return ltb.getMvRdz(self,sectionClass,Vd)

  def getLateralBucklingImperfectionFactor(self):
    ''' Returns lateral torsional imperfection factor depending of the type of section (rolled, welded,...).

    '''
    return ltb.getLateralBucklingImperfectionFactor(self)

  def getLateralBucklingIntermediateFactor(self,sectionClass,xi,Mi,supportCoefs= ltb.SupportCoefficients()):
    ''' Returns lateral torsional buckling intermediate factor value.

     :param sectionClass: section classification (1,2,3 or 4)
     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
    '''
    return ltb.getLateralBucklingIntermediateFactor(self,sectionClass,xi,Mi,supportCoefs)

  def getLateralBucklingReductionFactor(self,sectionClass,xi,Mi,supportCoefs= ltb.SupportCoefficients()):
    ''' Returns lateral torsional buckling reduction factor value.

      :param sectionClass: section classification (1,2,3 or 4)
      :param xi: abcissae for the moment diagram
      :param Mi: ordinate for the moment diagram
      :param supportCoefs: coefficients that represent support conditions.
      '''  
    return ltb.getLateralBucklingReductionFactor(self,sectionClass,xi,Mi,supportCoefs)

  def getLateralTorsionalBucklingResistance(self,sectionClass,xi,Mi,supportCoefs= ltb.SupportCoefficients()):
    '''Returns lateral torsional buckling resistance of this cross-section.
       Calculation is made following the paper:

       A. López, D. J. Yong, M. A. Serna,
       Lateral-torsional buckling of steel beams: a general expression for
       the moment gradient factor.
       (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
    '''
    return ltb.getLateralTorsionalBucklingResistance(self,sectionClass,xi,Mi,supportCoefs)

  def getMcr(self,xi,Mi,supportCoefs= ltb.SupportCoefficients()):
    '''Returns elastic critical moment about minor axis: y
       Calculation is made following the paper:

       A. López, D. J. Yong, M. A. Serna,
       Lateral-torsional buckling of steel beams: a general expression for
       the moment gradient factor.
       (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
    '''
    return ltb.getMcr(self,xi,Mi,supportCoefs)

  def getLateralBucklingNonDimensionalBeamSlenderness(self,sectionClass,xi,Mi,supportCoefs= ltb.SupportCoefficients()):

    '''Returns non dimensional beam slenderness
     for lateral torsional buckling
     see parameter definition on method getMcr.

     :param profile: cross section profile.
     :param sectionClass: section classification (1,2,3 or 4)
     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
    '''
    return ltb.getLateralBucklingNonDimensionalBeamSlenderness(self,sectionClass,xi,Mi,supportCoefs)
  
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
    super(EC3Profile,self).setupULSControlVars(elems)
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
      recorder.callbackRecord= EC3cc.controlULSCriterion2D()
    else:
      recorder.callbackRecord= EC3cc.controlULSCriterion()

    recorder.callbackRestart= "print \"Restart method called.\""
    return recorder
