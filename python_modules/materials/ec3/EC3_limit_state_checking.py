# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 3. '''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import sys
import math
# from misc_utils import log_messages as lmsg
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from misc_utils import log_messages as lmsg
from materials import steel_member_base

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


def getBendingResistanceReductionCoefficient(steelShape, Vd):
    '''Returns bending resistance reduction coefficient as in
    clause 6.2.8 of EC3 EN 1993-1-1

    :param steelShape: cross section shape.
    :param Vd: design value of the shear load.
    '''
    VplRd= steelShape.getVplRdy()
    ratio= Vd/VplRd
    if(ratio<=0.5):
        return 0.0 #No reduction
    else:
        return (2*ratio-1)**2

def getMvRdz(steelShape, Vd):
    '''Returns the major bending resistance of the cross-section under a
    shear force of Vd.

    param steelShape: cross section shape.
    '''
    McRdz= steelShape.getMcRdz()
    reductionCoeff= steelShape.getBendingResistanceReductionCoefficient(Vd)
    if(reductionCoeff<=0.0):
        retval= McRdz
    else:
        Aw= steelShape.hw()*steelShape.tw()
        sustr= reductionCoeff*Aw**2/4.0/steelShape.tw()
        remainingWz= steelShape.getWz()-sustr
        if(remainingWz>0.0):
            retval= min(remainingWz*steelShape.steelType.fy/steelShape.steelType.gammaM0(),McRdz)
        else:
            retval= McRdz/1e6 # avoid division by zero
    return retval

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

def get_buckling_parameters(element, bucklingLoadFactors, steelShape):
    ''' Return the effective length, mechanical slenderness, reduction factor and buckling resistance for the given buckling load factors.

    :param element: element to compute the buckling parameters for.
    :param bucklingLoadFactors: list containing the buckling load factors obtained from the linear buckling analysis.
    :param steelShape: steel section of the element.
    '''
    N= element.getN() # Element axial load.
    Ncri= [x * N for x in bucklingLoadFactors]
    nDOF= element.getNumDOF() # Number of degrees of freedom.
    section= element.physicalProperties.getVectorMaterials[0]
    Leffi= list() # Effective lengths for each mode.
    mechLambdai= list() # Mechanical slenderness for each mode.
    Xi= list() # Buckling reduction factors.
    NbRdi= list() # buckling resistance for each mode.
    if(nDOF==6): # 2D element.
        EI= section.sectionProperties.EI()
        iz= section.sectionProperties.i # Radius of gyration.
        for mode, Ncr in enumerate(Ncri):
            Leff= math.sqrt((EI*math.pi**2)/abs(Ncr)) # Effective length.
            if(Ncr>0):
                Leff= -Leff
            Leffi.append(Leff)
            mechLambda= Leff/iz # Compute mechanical slenderness
            mechLambdai.append(mechLambda)
            Xi.append(steelShape.getBucklingReductionFactorY(Leff))
            NbRdi.append(steelShape.getBucklingResistance(Leff,Leff))
    elif(nDOF==12):
        EIz= section.sectionProperties.EIz()
        EIy= section.sectionProperties.EIy()
        iz= section.sectionProperties.iz # Radius of gyration about z axis.
        iy= section.sectionProperties.iy # Radius of gyration about y axis.
        for mode, Ncr in enumerate(Ncri):
            Leffz= math.sqrt((EIz*math.pi**2)/abs(Ncr)) # Effective length.
            Leffy= math.sqrt((EIy*math.pi**2)/abs(Ncr)) # Effective length.
            if(Ncr>0):
                Leffz= -Leffz
                Leffy= -Leffy
            Leffi.append((Leffz, Leffy))
            mechLambdaZ= Leffz/iz # Compute mechanical slenderness
            mechLambdaY= Leffy/iy # Compute mechanical slenderness
            mechLambdai.append((mechLambdaZ, mechLambdaY))
            Xz= steelShape.getBucklingReductionFactorZ(Leffz)
            Xy= steelShape.getBucklingReductionFactorY(Leffy)
            Xi.append((Xz,Xy))
            NbRdi.append(steelShape.getBucklingResistance(Leffz,Leffy))
    else:
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        errMsg= className+'.'+methodName+"; not implemented for elements with. " + str(nDOF) + " degrees of freedom."
        lmsg.error(errMsg)
    return Leffi, mechLambdai, Xi, NbRdi

class BeamSupportCoefficients(object):

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

def getLateralBucklingIntermediateFactor(steelShape, L, Mi, beamSupportCoefs= BeamSupportCoefficients()):
    ''' Returns lateral torsional buckling intermediate factor value.

    :param steelShape: cross section shape.
    :param L: member length.
    :param Mi: ordinate for the moment diagram
    :param beamSupportCoefs: coefficients that represent support conditions.
    '''
    alphaLT= steelShape.getLateralBucklingImperfectionFactor()
    overlineLambdaLT= steelShape.getLateralBucklingNonDimensionalBeamSlenderness(L= L, Mi= Mi, beamSupportCoefs= beamSupportCoefs)
    return 0.5*(1+alphaLT*(overlineLambdaLT-0.2)+overlineLambdaLT**2)

def getLateralBucklingReductionFactor(steelShape, L, Mi, beamSupportCoefs= BeamSupportCoefficients()):
    ''' Returns lateral torsional buckling reduction factor value.

    :param steelShape: cross section shape.
    :param L: member length.
    :param Mi: ordinate for the moment diagram
    :param beamSupportCoefs: coefficients that represent support conditions.
    '''  
    phiLT= steelShape.getLateralBucklingIntermediateFactor(L= L, Mi= Mi, beamSupportCoefs= beamSupportCoefs)
    overlineLambdaLT= steelShape.getLateralBucklingNonDimensionalBeamSlenderness(L= L, Mi= Mi, beamSupportCoefs= beamSupportCoefs)
    return min(1.0,1.0/(phiLT+math.sqrt(phiLT**2-overlineLambdaLT**2)))

def getLateralTorsionalBucklingResistance(steelShape, L, Mi, beamSupportCoefs= BeamSupportCoefficients()):
    '''Returns lateral torsional buckling resistance of this cross-section.
    Calculation is made following the paper:

    A. López, D. J. Yong, M. A. Serna,
    Lateral-torsional buckling of steel beams: a general expression for
    the moment gradient factor.
    (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

    :param steelShape: cross section shape.
    :param Mi: ordinate for the moment diagram
    :param beamSupportCoefs: coefficients that represent support conditions.
    '''  
    chiLT= steelShape.getLateralBucklingReductionFactor(L= L, Mi= Mi, beamSupportCoefs= beamSupportCoefs)
    return chiLT*steelShape.getMcRdz()

def getMcr(steelShape, L, Mi, beamSupportCoefs= BeamSupportCoefficients()):
    '''Returns elastic critical moment about minor axis: y
    Calculation is made following the paper:

    A. López, D. J. Yong, M. A. Serna,
    Lateral-torsional buckling of steel beams: a general expression for
    the moment gradient factor.
    (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

    :param steelShape: cross section shape.
    :param L: member length.
    :param Mi: ordinate for the moment diagram
    :param beamSupportCoefs: coefficients that represent support conditions.
    '''
    mgf= MomentGradientFactorC1(Mi)
    C1= mgf.getC1(beamSupportCoefs= beamSupportCoefs)
    pi2EIy= math.pi**2*steelShape.EIy()
    GIt= steelShape.GJ()
    kyL2= (beamSupportCoefs.ky*L)**2
    Mcr0= pi2EIy/kyL2
    sum1= (beamSupportCoefs.ky/beamSupportCoefs.kw)**2*steelShape.Iw()/steelShape.Iy()
    sum2= GIt/Mcr0
    f2= math.sqrt(sum1+sum2)
    return C1*Mcr0*f2

def getLateralBucklingNonDimensionalBeamSlenderness(steelShape, L, Mi, beamSupportCoefs= BeamSupportCoefficients()):
    '''Returns non dimensional beam slenderness
    for lateral torsional buckling
    see parameter definition on method getMcr.

    :param steelShape: cross section shape.
    :param Mi: ordinate for the moment diagram
    :param beamSupportCoefs: coefficients that represent support conditions.
    '''
    Mcr= steelShape.getMcr(L= L, Mi= Mi, beamSupportCoefs= beamSupportCoefs)
    return math.sqrt(steelShape.getWz()*steelShape.steelType.fy/Mcr)

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

    def getA1(self,beamSupportCoefs):
        '''Return the value for the A1 coefficient according to equation 10
           of the reference [1].
           
        :param k1: warping AND lateral bending coefficient at left end
                                   k1= 1.0 => free warping AND lateral bending
                                   k1= 0.5 => prevented warp. AND lateral bending
        :param k2: warping AND lateral bending coefficient at right end
                                   k2= 1.0 => free warping AND lateral bending
                                   k2= 0.5 => prevented warp. AND lateral bending'''
        ai= beamSupportCoefs.getAlphaI()
        Mmax2= self.getExtremeMoment()**2
        return (Mmax2+ai[0]*self.Mi[0]**2+ai[1]*self.Mi[1]**2+ai[2]*self.Mi[2]**2+ai[3]*self.Mi[3]**2+ai[4]*self.Mi[4]**2)/((1+ai[0]+ai[1]+ai[2]+ai[3]+ai[4])*Mmax2)

    def getC1(self, beamSupportCoefs):
        '''Return the value for the C1 coefficient according to equation 8
           of the reference [1]. 

        :param beamSupportCoefs: instance of EC3_limit_state_checking.BeamSupportCoefficients
                             that wraps the support coefficients: ky, kw, k1 
                             and k2; where ky is the lateral bending 
                             coefficient, kw the warping coefficient,  k1 and 
                             the warping AND lateral bending coefficients at first
                             and last ends respectively (1.0 => free, 0.5 => prevented). 
                             (Defaults to ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0)
        '''


        k= math.sqrt(beamSupportCoefs.k1*beamSupportCoefs.k2) # equation 9
        A1= self.getA1(beamSupportCoefs)
        A2= self.getA2()
        rootK= math.sqrt(k) 
        B1= rootK*A1+((1-rootK)/2.0*A2)**2
        return (math.sqrt(B1)+(1-rootK)/2.0*A2)/A1 # equation 8

class Member(steel_member_base.BucklingMember):
    '''Steel beam defined by an arbitrary name, a cross-section shape, 
    its section class, the coefficients of supports and the type  
    
    :ivar supportCoefs: instance of EC3_limit_state_checking.BeamSupportCoefficients
          that wraps the support coefficients: ky, kw, k1 and k2. where ky is 
          the lateral bending coefficient, kw the warping coefficient,  k1 and
          the warping AND lateral bending coefficients at first and last ends 
          respectively (1.0 => free,  0.5 => prevented). 
          (Defaults to ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0)
    :ivar typo: 'rolled' or 'welded' (defaults to rolled)
    '''
    def __init__(self, name, ec3Shape, beamSupportCoefs= BeamSupportCoefficients(ky=1.0,kw=1.0,k1=1.0,k2=1.0), typo= 'rolled', lstLines=None, lstPoints=None):
        '''Constructor.

        :param name: object name.
        :param ec3Shape: cross-section shape (e.g. IPNShape, IPEShape, ...)
        :param beamSupportCoefs: instance of EC3_limit_state_checking.BeamSupportCoefficients
                             that wraps the support coefficients: ky, kw, k1 
                             and k2; where ky is the lateral bending 
                             coefficient, kw the warping coefficient,  k1 and 
                             the warping AND lateral bending coefficients at first
                             and last ends respectively (1.0 => free, 0.5 => prevented). 
                             (Defaults to ky= 1.0, kw= 1.0, k1= 1.0, k2= 1.0)
        :param typo: 'rolled' or 'welded' (defaults to rolled)
        :param lstLines: ordered list of lines that make up the beam 
                        (defaults to None).
        :param lstPoints: ordered list of points that make up the beam. 
                          Ignored if lstLines is given (defaults to None)
        '''
        super(Member,self).__init__(name, ec3Shape, lstLines= lstLines, lstPoints= lstPoints)
        self.beamSupportCoefs= beamSupportCoefs
        self.typo=typo

    def getLateralBucklingReductionFactor(self):
        ''' Return lateral torsional buckling reduction factor value
        for the elements of the beam.'''
        Mi= self.getBendingMomentsAtControlPoints()
        return self.shape.getLateralBucklingReductionFactor(L= self.getLength(), Mi= Mi, beamSupportCoefs= self.beamSupportCoefs)

    def updateLateralBucklingReductionFactor(self):
        '''Update the value of the lateral buckling reduction factor.'''
        chiLT= self.getLateralBucklingReductionFactor()
        for e in self.elemSet:
             e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.
    def updateReductionFactors(self):
        '''Update the value of the appropriate reduction factors.'''
        self.updateLateralBucklingReductionFactor()

    def getBiaxialBendingEfficiency(self, Nd, Myd, Mzd, Vyd= 0.0):
        '''Return biaxial bending efficiency according to section H1
           of AISC-360-16.

           :param Nd: axial design load (required axial strength).
           :param Myd: bending moment about weak axis (required flexural strength).
           :param Mzd: bending moment about strong axis (required flexural strength).
        '''
        # Compute axial load reduction factor.
        lrfN= self.getCompressiveStrengthReductionFactor()

        # Compute lateral buckling reduction factor.
        lrfLT= self.getFlexuralStrengthReductionFactor()

        return self.shape.getBiaxialBendingEfficiency(Nd= Nd, Myd= Myd, Mzd= Mzd, Vyd= Vyd, chiN= lrfN, chiLT= lrfLT)

    def installULSControlRecorder(self,recorderType, chiLT=1.0, calcSet= None):
        '''Install recorder for verification of ULS criterion.

        :param recorderType: type of the recorder to install.
        :param calcSet: set of elements to be checked (defaults to 'None' which 
                        means that this set will be created elsewhere). In not
                        'None' the member elements will be appended to this set.
        '''
        recorder= self.createRecorder(recorderType, calcSet)
        self.shape.setupULSControlVars(self.elemSet, chiLT= chiLT)
        nodHndlr= self.getPreprocessor().getNodeHandler        
        if(nodHndlr.numDOFs==3):
            recorder.callbackRecord= controlULSCriterion2D()
        else:
            recorder.callbackRecord= controlULSCriterion()
#        recorder.callbackRestart= "print(\"Restart method called.\")" #20181121
        return recorder
   
class BiaxialBendingNormalStressController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls normal stresses limit state.'''

    ControlVars= cv.SteelShapeBiaxialBendingControlVars
    
    def __init__(self, limitStateLabel, solutionProcedureType= lsc.defaultStaticLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel, solutionProcedureType= solutionProcedureType)
    
    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the element steel shape
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get element section properties.
        steelShape= elem.getProp('crossSection')
        if(steelShape):
            # Check each element section.
            for lf in elementInternalForces: 
                # Compute efficiency.
                CFtmp,NcRdtmp,McRdytmp,McRdztmp,MvRdztmp,MbRdztmp= steelShape.getBiaxialBendingEfficiency(Nd= lf.N, Myd= lf.My, Mzd= lf.Mz, Vyd= lf.Vy, chiLT= lf.chiLT, chiN= lf.chiN)
                sectionLabel= self.getSectionLabel(lf.idSection)
                label= self.limitStateLabel+sectionLabel
                # Update efficiency.
                if(CFtmp>elem.getProp(label).CF):
                    elem.setProp(label, self.ControlVars(sectionLabel+'s',lf.idComb,CFtmp,lf.N,lf.My,lf.Mz,NcRdtmp,McRdytmp,McRdztmp,MvRdztmp,MbRdztmp, chiN= lf.chiN, chiLT= lf.chiLT))
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= className+'.'+methodName+"; cross section not defined for element: " + str(elem.tag) + "\n"
            lmsg.error(errMsg)
        

class ShearController(lsc.LimitStateControllerBase2Sections):
    '''Object that controls shear limit state.'''

    ControlVars= cv.ShearYControlVars
    def __init__(self,limitStateLabel, solutionProcedureType= lsc.defaultStaticLinearSolutionProcedure):
        ''' Constructor.
        
        :param limitStateLabel: label that identifies the limit state.
        :param solutionProcedureType: type of the solution procedure to use
                                      when computing load combination results.
        '''
        super(ShearController,self).__init__(limitStateLabel= limitStateLabel, solutionProcedureType= solutionProcedureType)

    def updateEfficiency(self, elem, elementInternalForces):
        ''' Compute the efficiency of the element steel shape
            subjected to the internal forces argument and update
            its value if its bigger than the previous one.

        :param elem: finite element whose section will be checked.
        :param elementInternalForces: internal forces acting on the steel shape.
        '''
        # Get element section properties.
        steelShape= elem.getProp('crossSection')
        # Check each element section.
        for lf in elementInternalForces: 
            # Compute efficiency.
            CFtmp= steelShape.getYShearEfficiency(Vyd= lf.Vy)
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

