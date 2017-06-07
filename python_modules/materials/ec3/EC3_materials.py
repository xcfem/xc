# -*- coding: utf-8 -*-
from __future__ import division
''' Structural steel as specified in EC3 standard.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
from materials import structural_steel
from materials import typical_materials
from materials.ec3 import EC3_limit_state_checking as EC3lsc
from miscUtils import LogMessages as lmsg

class EC3Steel(structural_steel.BasicSteel):
  '''Eurocode 3 structural steel.

    :ivar fy16: 0<t<16mm
    :ivar fy40: 16<t<40mm
    :ivar fy63: 40<t<63mm
    :ivar fy80: 63<t<80mm
    :ivar fy100: 80<t<100mm
    :ivar fy125: 80<t<125mm
    :ivar gammaM1: Partial factor for buckling resistance.
    :ivar gammaM2: Partial factor for cross-sections in tension to fracture.
  '''

  def __init__(self, fy, fy16, fy40, fy63, fy80, fy100, fy125, fu, gammaM, gammaM1= 1.0, gammaM2= 1.1):
    super(EC3Steel,self).__init__(210000e6,0.3,fy,fu,gammaM)
    self.fy16= fy16
    self.fy40= fy40
    self.fy63= fy63
    self.fy80= fy80
    self.fy100= fy100
    self.fy125= fy125
    self.gammaM1= gammaM1 #Partial factor for buckling resistance.
    self.gammaM2= gammaM2 #Partial factor for cross-sections in tension to fracture.

  def gammaM0(self):
    return self.gammaM

  def fyd(self):
    return self.fy/self.gammaM
  def fydV(self):
    return self.fyd()/math.sqrt(3)
  def getLambda1(self):
    '''
       :returns: lambda_1 value as specified in EC3 part 1 5.5.1.2
    '''
    return math.pi*math.sqrt(self.E/self.fy)
  def getDesignElasticPerfectlyPlasticMaterial(self,preprocessor,name):
    return typical_materials.defElasticPPMaterial(preprocessor, name,self.E,self.fyd(),-self.fyd())

def fyEC3(desig, t):
  '''
  Devuelve el límite elástico del acero en función de su designación
   y del espesor de la pieza (ver tabla 3.1).
  '''
  retval= 0.0
  if(t>0.1):
    lmsg.error("Espesor de la pieza fuera de rango: ",t*1000," mm\n")

  if(desig == 235):
    retval= ifte(t<40e-3,235e6,215e6)
  elif(desig == 275):
    retval= ifte(t<40e-3,275e6,255e6)
  elif(desig == 355):
    retval= ifte(t<40e-3,355e6,335e6)
  else:
    lmsg.error("No se conoce la designación del acero: ",desig,"\n")
  return retval

def fuEC3(desig, t):
  '''
  Devuelve la resistencia última a tracción del acero en función de 
  su designación y del espesor de la pieza (ver tabla 3.1).
  '''
  retval= 0.0
  if(t>0.1):
    lmsg.error("Espesor de la pieza fuera de rango: ",t*1000," mm\n")

  if(desig == 235):
    retval= ifte(t<40e-3,360e6,340e6)
  elif(desig == 275):
    retval= ifte(t<40e-3,430e6,410e6)
  elif(desig == 355):
    retval= ifte(t<40e-3,510e6,490e6)
  else:
    lmsg.error("No se conoce la designación del acero: ",desig,"\n")
  return retval

# European norm EN 10025-2:2004
S235JR= EC3Steel(fy= 235e6, fy16= 235e6, fy40= 225e6, fy63= 215e6, fy80= 215e6, fy100= 215e6, fy125= 195e6,fu= 360e6,gammaM= 1.1)

S275JR= EC3Steel(fy= 275e6, fy16= 275e6, fy40= 265e6, fy63= 255e6, fy80= 245e6, fy100= 235e6, fy125= 225e6,fu= 360e6,gammaM= 1.1)

S355JR= EC3Steel(fy= 355e6, fy16= 355e6, fy40= 345e6, fy63= 335e6, fy80= 325e6, fy100= 315e6, fy125= 295e6, fu= 360e6, gammaM= 1.1)

S450J0= EC3Steel(fy= 450e6, fy16= 450e6, fy40= 430e6, fy63= 410e6, fy80= 390e6, fy100= 380e6, fy125= 380e6, fu= 360e6, gammaM= 1.1)

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
    return EC3lsc.getLateralTorsionalBucklingCurve(self)
  def getAvy(self):
    '''Returns y direction (web direction) shear area'''
    return self.get('Avy')
  def shearBucklingVerificationNeeded(self):
    '''Returns true if shear buckling verification is needed EC3-1-5'''
    return EC3lsc.shearBucklingVerificationNeeded(self)
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
    return EC3lsc.getBendingResistanceReductionCoefficient(self,Vd)
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
    return EC3lsc.getMvRdz(self,sectionClass,Vd)

  def getLateralBucklingImperfectionFactor(self):
    ''' Returns lateral torsional imperfection factor depending of the type of section (rolled, welded,...).

    '''
    return EC3lsc.getLateralBucklingImperfectionFactor(self)

  def getLateralBucklingIntermediateFactor(self,sectionClass,xi,Mi,supportCoefs= EC3lsc.SupportCoefficients()):
    ''' Returns lateral torsional buckling intermediate factor value.

     :param sectionClass: section classification (1,2,3 or 4)
     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
    '''
    return EC3lsc.getLateralBucklingIntermediateFactor(self,sectionClass,xi,Mi,supportCoefs)

  def getLateralBucklingReductionFactor(self,sectionClass,xi,Mi,supportCoefs= EC3lsc.SupportCoefficients()):
    ''' Returns lateral torsional buckling reduction factor value.

      :param sectionClass: section classification (1,2,3 or 4)
      :param xi: abcissae for the moment diagram
      :param Mi: ordinate for the moment diagram
      :param supportCoefs: coefficients that represent support conditions.
      '''  
    return EC3lsc.getLateralBucklingReductionFactor(self,sectionClass,xi,Mi,supportCoefs)

  def getLateralTorsionalBucklingResistance(self,sectionClass,xi,Mi,supportCoefs= EC3lsc.SupportCoefficients()):
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
    return EC3lsc.getLateralTorsionalBucklingResistance(self,sectionClass,xi,Mi,supportCoefs)

  def getMcr(self,xi,Mi,supportCoefs= EC3lsc.SupportCoefficients()):
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
    return EC3lsc.getMcr(self,xi,Mi,supportCoefs)

  def getLateralBucklingNonDimensionalBeamSlenderness(self,sectionClass,xi,Mi,supportCoefs= EC3lsc.SupportCoefficients()):

    '''Returns non dimensional beam slenderness
     for lateral torsional buckling
     see parameter definition on method getMcr.

     :param profile: cross section profile.
     :param sectionClass: section classification (1,2,3 or 4)
     :param xi: abcissae for the moment diagram
     :param Mi: ordinate for the moment diagram
     :param supportCoefs: coefficients that represent support conditions.
    '''
    return EC3lsc.getLateralBucklingNonDimensionalBeamSlenderness(self,sectionClass,xi,Mi,supportCoefs)
  
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
      recorder.callbackRecord= EC3lsc.controlULSCriterion2D()
    else:
      recorder.callbackRecord= EC3lsc.controlULSCriterion()

    recorder.callbackRestart= "print \"Restart method called.\""
    return recorder

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

from materials.structural_shapes.arcelor import perfiles_ipe_arcelor as ipe

class IPEProfile(EC3Profile,ipe.IPEProfile):
  """IPE profile with Eurocode 3 verification routines."""
  def __init__(self,steel,name):
    ''' Constructor.

    :param steel: steel material.
    :param name: shape name (i.e. IPE_600)
    '''
    EC3Profile.__init__(self,'rolled')
    ipe.IPEProfile.__init__(self,steel,name)

'''
European H beams

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

from materials.structural_shapes.arcelor import perfiles_he_arcelor as he

class HEProfile(EC3Profile,he.HEProfile):
  """HE profile with Eurocode 3 verification routines."""
  def __init__(self,steel,name):
    ''' Constructor.

    :param steel: steel material.
    :param name: shape name (i.e. HE_600_A)
    '''
    EC3Profile.__init__(self,'rolled')
    he.HEProfile.__init__(self,steel,name)
