# -*- coding: utf-8 -*-
''' Structural steel as specified in EC3 standard.'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
import sys
from materials import steel_base
from materials import typical_materials
from materials.ec3 import EC3_limit_state_checking as EC3lsc
from misc_utils import log_messages as lmsg
from connections.steel_connections import bolts

class EC3Steel(steel_base.BasicSteel):
    '''Eurocode 3 structural steel.

      :ivar fy16: 0<t<16mm
      :ivar fy40: 16<t<40mm
      :ivar fy63: 40<t<63mm
      :ivar fy80: 63<t<80mm
      :ivar fy100: 80<t<100mm
      :ivar fy125: 100<t<125mm
      :ivar gammaM: partial safety factor for steel strength.
      :ivar gammaM1: partial factor for buckling resistance.
      :ivar gammaM2: partial factor for cross-sections in tension to fracture.
    '''
    def __init__(self, fy, fy16, fy40, fy63, fy80, fy100, fy125, fu, gammaM, gammaM1= 1.0, gammaM2= 1.1):
        ''' Constructor.
          :param fy:  yield stress.
          :param fy16: 0<t<16mm
          :param fy40: 16<t<40mm
          :param fy63: 40<t<63mm
          :param fy80: 63<t<80mm
          :param fy100: 80<t<100mm
          :param fy125: 100<t<125mm
          :param fu: ultimate stress.
          :param gammaM: partial safety factor for steel strength.
          :param gammaM1: partial factor for buckling resistance.
          :param gammaM2: partial factor for cross-sections in tension to fracture.
        '''

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
        ''' Return the value of the partial safety factor for steel strength.'''
        return self.gammaM

    def setGammaM0(self, gammaM0:float):
        ''' Assign the value of the partial safety factor for steel strength.

        :param gammaM0: Partial safety factor for steel strength.
        '''
        self.gammaM= gammaM0

    def fydV(self):
        ''' Return the design value of the shear strength.'''
        return self.fyd()/math.sqrt(3)

    def getLambda1(self):
        '''return lambda_1 value as specified in EC3 part 1 5.5.1.2
        '''
        return math.pi*math.sqrt(self.E/self.fy)

    def getDesignElasticPerfectlyPlasticMaterial(self,preprocessor,name):
        return typical_materials.defElasticPPMaterial(preprocessor, name,self.E,self.fyd(),-self.fyd())

def fyEC3(desig, t):
    '''
    Return steel yield strength from its name and the part thickness 
    (see table 3.1).

    
    :param desig: steel designation (235, 275, 355).
    :param t: part thickness.
    '''
    retval= 0.0
    if(t>0.1):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; part thickness out of range: '+str(t*1000)+' mm.')

    if(desig == 235):
        retval= ifte(t<40e-3,235e6,215e6)
    elif(desig == 275):
        retval= ifte(t<40e-3,275e6,255e6)
    elif(desig == 355):
        retval= ifte(t<40e-3,355e6,335e6)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; unknown steel designation: '+str(desig)+'.')
    return retval

def fuEC3(desig, t):
    '''
    Return steel ultimate strength from its name and the part thickness 
    (see table 3.1).
    
    :param desig: steel designation (235, 275, 355).
    :param t: part thickness.
    '''
    retval= 0.0
    if(t>0.1):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; part thickness out of range: '+str(t*1000)+' mm.')

    if(desig == 235):
        retval= ifte(t<40e-3,360e6,340e6)
    elif(desig == 275):
        retval= ifte(t<40e-3,430e6,410e6)
    elif(desig == 355):
        retval= ifte(t<40e-3,510e6,490e6)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+'; unknown steel designation: '+str(desig)+'.')
    return retval

# European norm EN 10025-2:2004
S235JR= EC3Steel(fy= 235e6, fy16= 235e6, fy40= 225e6, fy63= 215e6, fy80= 215e6, fy100= 215e6, fy125= 195e6,fu= 360e6,gammaM= 1.1)

S275JR= EC3Steel(fy= 275e6, fy16= 275e6, fy40= 265e6, fy63= 255e6, fy80= 245e6, fy100= 235e6, fy125= 225e6,fu= 360e6,gammaM= 1.1)

S355JR= EC3Steel(fy= 355e6, fy16= 355e6, fy40= 345e6, fy63= 335e6, fy80= 325e6, fy100= 315e6, fy125= 295e6, fu= 360e6, gammaM= 1.1)

S450J0= EC3Steel(fy= 450e6, fy16= 450e6, fy40= 430e6, fy63= 410e6, fy80= 390e6, fy100= 380e6, fy125= 380e6, fu= 360e6, gammaM= 1.1)

#Alpha imperfection factor.
def alphaImperfectionFactor(bucklingCurve):
    '''Return the alpha imperfection factor for buckling curves
       see tables 6.1 and 6.2 of EC3 (EN 19931-1:2005).

       :param bucklingCurve: buckling curve (a0,a,b,c or d) according 
              to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
              curve for a cross-section". Note that in table 6.2 Y and 
              Z axes are swapped with respect to those used in XC. 
              XC axes: Y->weak axis, Z->strong axis.
    '''
    retval= 0.76
    if(bucklingCurve=='a0'):
        retval= 0.13
    elif(bucklingCurve=='a'):
        retval= 0.21
    elif(bucklingCurve=='b'):
        retval= 0.34
    elif(bucklingCurve=='c'):
        retval= 0.49
    elif(bucklingCurve=='d'):
        retval= 0.76
    else:
        lmsg.error('Buckling curve: '+str(bucklingCurve)+' unknown.')
    return retval

class EC3Shape(object):
    '''Steel shape with Eurocode 3 verification routines.

    :ivar name: steel shape name.
    :ivar typo: 'rolled' or 'welded' shape
    '''
    def __init__(self,name, typo= 'rolled'):
        '''
          Constructor.

        :param name: steel shape name.
        :param typo: 'rolled' or 'welded' shape
        '''
        self.name=name
        self.typo= typo

    def getClassInternalPartInCompression(self, ratioCT=None):
        '''Return the cross-section classification of internal part 
        (web in open shapes, ...) totally subject to compression 
        (conservative). Clause 5.5 EC3-1-1

        :param ratioCT: ratio c/t width-to-thickness of the internal
                        compressed part (defaults to widthToThicknessWeb)
        
        '''
        ratioCT=ratioCT if ratioCT is not None else self.widthToThicknessWeb()
        eps=math.sqrt(235e6/self.steelType.fy)
        limits=[33*eps,38*eps,42*eps]
        classif=0
        while ratioCT>limits[classif]:
            classif+=1
        return (classif+1)
    
    def getClassInternalPartInBending(self, ratioCT=None):
        '''Return the cross-section classification of internal part 
        (web in open shapes, ...) subject to pure bending. Clause 5.5 EC3-1-1

        :param ratioCT: ratio c/t width-to-thickness of the internal
                        compressed part (defaults to widthToThicknessWeb)
        
        '''
        ratioCT=ratioCT if ratioCT is not None else self.widthToThicknessWeb()
        eps=math.sqrt(235e6/self.steelType.fy)
        limits=[72*eps,83*eps,124*eps]
        classif=0
        while ratioCT>limits[classif]:
            classif+=1
        return (classif+1)
        
        
    # def getClassInternalPartInBending(self):
    #     '''Return the cross-section classification of internal part 
    #     (web in open shapes, ...) totally subject to compression 
    #     (conservative). Clause 5.5 EC3-1-1

    #     :param ratioCT: ratio c/t width-to-thickness of the internal
    #                     compressed part (defaults to widthToThicknessWeb).
    #     '''
    #     ratioCT=self.widthToThicknessWeb()
    #     eps=math.sqrt(235e6/self.steelType.fy)
    #     limits=[72*eps,83*eps,124*eps]
    #     classif=0
    #     while ratioCT>limits[classif]:
    #         classif+=1
    #     return (classif+1)
        

    def getClassOutstandPartInCompression(self, ratioCT=None):
        '''Return the cross-section classification of outstand part 
        (flanges) totally subject to compression 
        (conservative). Clause 5.5 EC3-1-1

        :param ratioCT: ratio c/t width-to-thickness of the outstand
               compressed part (defaults to widthToThicknessFlange)
        
        '''
        ratioCT=ratioCT if ratioCT is not None else self.widthToThicknessFlange()
        eps=math.sqrt(235e6/self.steelType.fy)
        limits=[9*eps,10*eps,14*eps]
        classif=0
        while ratioCT>limits[classif]:
            classif+=1
        return (classif+1)
    
    def getCfactIntPart(self):
        '''Return the C length of internal part in compression used to 
        classify the cross-section. Table 5.2 EC3-1-1
        '''
        if self.name[0] in ['I','H']:
            C=self.h
            
    def getAdimensionalSlendernessY(self,Leq,sectionClass= 1):
        '''return adimensional slenderness relative to y-axis (weak axis) 
           as defined in EC3 part 1 6.3.1

           :param Leq: buckling length in XZ buckling plane.
           :param sectionClass: class of the section (1 to 3, 4 not yet 
                  implemented) (defaults to 1)
        '''
        betaA= self.getAeff(sectionClass)/self.A()
        lambdA= self.getSlendernessY(Leq)
        lambda1= self.steelType.getLambda1()
        return lambdA/lambda1*math.sqrt(betaA)
    
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        raise NotImplementedError()
        return 'd'
    
    def getBucklingReductionFactorY(self,Leq, sectionClass= 1):
        '''return buckling reduction factor relative to y-axis (weak axis) 
        as defined in EC3-1-1 6.3.1

        :param Leq: buckling length in XZ buckling plane.
        :param sectionClass: class of the section (1 to 3, 4 not yet 
               implemented) (defaults to 1)
        '''
        bucklingCurve= self.getBucklingCurve(majorAxis= False)
        alpha= alphaImperfectionFactor(bucklingCurve)
        lmb= self.getAdimensionalSlendernessY(Leq, sectionClass= sectionClass)
        phi= 0.5*(1+alpha*(lmb-0.2)+lmb**2)
        return min(1.0/(phi+math.sqrt(phi**2-lmb**2)),1.0)

    def getBucklingResistanceY(self,Leq, sectionClass= 1):
        '''return buckling resistance relative to y-axis (weak axis)
        according to EC3-1-1 6.3.2

        :param Leq: buckling length in XZ buckling plane.
        :param sectionClass: class of the section (1 to 3, 4 not yet implemented) (defaults to 1).
        '''
        X= self.getBucklingReductionFactorY(Leq,sectionClass)
        return X*self.getAeff(sectionClass)*self.steelType.fyd()
    
    def getAdimensionalSlendernessZ(self, Leq, sectionClass= 1):
        '''return adimensional slenderness relative to z-axis (strong axis) 
           as defined in EC3-1-1 6.3.1

           :param Leq: buckling length in XY buckling plane.
           :param sectionClass: class of the section (1 to 3, 4 not yet 
                  implemented) (defaults to 1)
        '''
        betaA= self.getAeff(sectionClass)/self.A()
        lambdA= self.getSlendernessZ(Leq)
        lambda1= self.steelType.getLambda1()
        return lambdA/lambda1*math.sqrt(betaA)
    
    def getBucklingReductionFactorZ(self, Leq, sectionClass= 1):
        '''return buckling reduction factor lative to z-axis (strong axis) 
        as defined in EC3-1-1 6.3.1

        :param Leq: buckling length in XY buckling plane.
        :param sectionClass: class of the section (1 to 3, 4 not yet 
               implemented) (defaults to 1)
        '''
        bucklingCurve= self.getBucklingCurve(majorAxis= True)
        alpha= alphaImperfectionFactor(bucklingCurve)
        lmb= self.getAdimensionalSlendernessZ(Leq, sectionClass= sectionClass)
        phi= 0.5*(1+alpha*(lmb-0.2)+lmb**2)
        return min(1.0/(phi+math.sqrt(phi**2-lmb**2)),1.0)

    def getBucklingResistanceZ(self, Leq, sectionClass= 1):
        '''return buckling resistance relative to z-axis (strong axis)
        according to EC3-1-1 6.3.2

        :param Leq: buckling length in XY buckling plane.
        :param sectionClass: class of the section (1 to 3, 4 not yet 
               implemented) (defaults to 1).
        '''
        X= self.getBucklingReductionFactorZ(Leq,sectionClass)
        return X*self.getAeff(sectionClass)*self.steelType.fyd()

    def getBucklingResistance(self, LeqY, LeqZ, sectionClass= 1):
        '''return minimum of buckling resistance in XY and XZ buckling planes
        calculated according to EC3-1-1 6.3.2

        :param LeqY: buckling length of the member in XZ buckling plane.
        :param LeqZ: buckling length of the member in XY buckling plane.
        :param sectionClass: class of the section (1 to 3, 4 not yet 
               implemented) (defaults to 1)
        '''
        rY= self.getBucklingResistanceY(LeqY,sectionClass)
        rZ= self.getBucklingResistanceZ(LeqZ,sectionClass)
        return min(rY,rZ)
    
    def getLateralTorsionalBucklingCurve(self):
        ''' Return the lateral torsional bukling curve name (a,b,c or d) depending of the type of section (rolled, welded,...). EC3 Table 6.4, 6.3.2.2(2).'''
        return EC3lsc.getLateralTorsionalBucklingCurve(self)
    
    def getAvy(self):
        '''Return y direction (web direction) shear area'''
        return self.get('Avy')
    
    def getShearArea(self, majorAxis= True):
        ''' Return area for shear strength calculation (added for compatibility
            with ASTM steel sections).

        :param majorAxis: if true major axis bending; so shear parallel to
                          minor axis.
        '''
        retval= self.getAvy()
        if(majorAxis):
            return retval
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+': for minor axis not implemented yet.')
            retval/= 1e3
        return retval
    
    def shearBucklingVerificationNeeded(self):
        '''Return true if shear buckling verification is needed EC3-1-5'''
        return EC3lsc.shearBucklingVerificationNeeded(self)
    
    def getVplRdy(self):
        '''Return y direction (web direction) plastic shear resistance
           according to clause 6.2.6 (expression 6.18) of EC3-1-1:2005.'''
        if(self.shearBucklingVerificationNeeded()):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+': section needs shear buckling verification.')
        return self.getAvy()*(self.steelType.fy/math.sqrt(3))/self.steelType.gammaM0()
    
    def getVcRdy(self):
        '''Return y direction (web direction) shear resistance
        [plastic design in absence of torsion]
        '''
        return self.getVplRdy()
    
    def getBendingResistanceReductionCoefficient(self,Vd):
        '''Return bending resistance reduction coefficient as in
           clause 6.2.8 of EC31-1'''
        return EC3lsc.getBendingResistanceReductionCoefficient(self,Vd)
    
    def getNcRd(self,sectionClass):
        '''Return the axial compression resistance of the cross-section.

        :param sectionClass: section classification (1,2,3 or 4)
        '''
        return self.getAeff(sectionClass)*self.steelType.fy/self.steelType.gammaM0()
    
    def getMcRdy(self,sectionClass):
        '''Return the minor bending resistance of the cross-section.

        :param sectionClass: section classification (1,2,3 or 4)
        '''
        return self.getWy(sectionClass)*self.steelType.fy/self.steelType.gammaM0()
    
    def getMcRdz(self,sectionClass):
        '''Return the major bending resistance of the cross-section.

        :param sectionClass: section classification (1,2,3 or 4)
        '''
        return self.getWz(sectionClass)*self.steelType.fy/self.steelType.gammaM0()
    
    def getMvRdz(self,sectionClass,Vd):
        '''Return the major bending resistance of the cross-section under a
           shear force of Vd according to clause 6.2.8 of EC3-1-1:2005.

          :param sectionClass: section classification (1,2,3 or 4)
          :param Vd: concomintant shear force.
        '''
        return EC3lsc.getMvRdz(self,sectionClass,Vd)

    def getLateralBucklingImperfectionFactor(self):
        ''' Return lateral torsional imperfection factor depending of the type of section (rolled, welded,...).

        '''
        return EC3lsc.getLateralBucklingImperfectionFactor(self)

    def getLateralBucklingIntermediateFactor(self,sectionClass,L,Mi,supportCoefs= EC3lsc.SupportCoefficients()):
        ''' Returns lateral torsional buckling intermediate factor value.

         :param sectionClass: section classification (1,2,3 or 4)
         :param Mi: ordinate for the moment diagram
         :param supportCoefs: coefficients that represent support conditions.
        '''
        return EC3lsc.getLateralBucklingIntermediateFactor(self,sectionClass,L,Mi,supportCoefs)

    def getLateralBucklingReductionFactor(self,sectionClass,L,Mi,supportCoefs= EC3lsc.SupportCoefficients()):
        ''' Returns lateral torsional buckling reduction factor value.

          :param sectionClass: section classification (1 to 3, 4 not yet implemented)
          :param Mi: ordinate for the moment diagram
          :param supportCoefs: coefficients that represent support conditions.
        '''  
        return EC3lsc.getLateralBucklingReductionFactor(self,sectionClass,L,Mi,supportCoefs)

    def getLateralTorsionalBucklingResistance(self,sectionClass,L,Mi,supportCoefs= EC3lsc.SupportCoefficients()):
        '''Return lateral torsional buckling resistance of this cross-section.
           Calculation is made following the paper:

           A. López, D. J. Yong, M. A. Serna,
           Lateral-torsional buckling of steel beams: a general expression for
           the moment gradient factor.
           (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

         :param Mi: ordinate for the moment diagram
         :param supportCoefs: coefficients that represent support conditions.
        '''
        return EC3lsc.getLateralTorsionalBucklingResistance(self,sectionClass,L,Mi,supportCoefs)

    def getMcr(self,L,Mi,supportCoefs= EC3lsc.SupportCoefficients()):
        '''Return elastic critical moment about minor axis: y
           Calculation is made following the paper:

           A. López, D. J. Yong, M. A. Serna,
           Lateral-torsional buckling of steel beams: a general expression for
           the moment gradient factor.
           (Lisbon, Portugal: Stability and ductility of steel structures, 2006).

         :param Mi: ordinate for the moment diagram
         :param supportCoefs: coefficients that represent support conditions.
        '''
        return EC3lsc.getMcr(self,L,Mi,supportCoefs)

    def getLateralBucklingNonDimensionalBeamSlenderness(self,sectionClass,L,Mi,supportCoefs= EC3lsc.SupportCoefficients()):

        '''Return non dimensional beam slenderness
         for lateral torsional buckling
         see parameter definition on method getMcr.

         :param shape: cross section shape.
         :param sectionClass: section classification (1,2,3 or 4)
         :param Mi: ordinate for the moment diagram
         :param supportCoefs: coefficients that represent support conditions.
        '''
        return EC3lsc.getLateralBucklingNonDimensionalBeamSlenderness(self,sectionClass,L,Mi,supportCoefs)

    def getYShearEfficiency(self,sectionClass,Vyd):
        '''Return major axis shear efficiency.

         :param sectionClass: section classification (1,2,3 or 4)
        '''
        return abs(Vyd/self.getVcRdy())

    def getZBendingEfficiency(self, Nd, Mzd, Vyd= 0.0, chiN=1.0, chiLT= 1.0,sectionClass= None):
        '''Return major axis bending efficiency

        :param Nd: required axial strength.
        :param Mzd: required bending strength (major axis).
        :param Vyd: required shear strength (major axis)
        :param chiN: axial strength reduction factor (default= 1.0).
        :param chiLT: lateral buckling reduction factor (default= 1.0).
        :param sectionClass: section classification (1,2,3 or 4)
        '''
        if(Nd!=0.0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+': for compressed sections not implemented yet.')
        MvRdz= self.getMvRdz(sectionClass,Vyd)
        MbRdz= chiLT*MvRdz #Lateral buckling reduction.
        return abs(Mzd)/MbRdz

    def getBiaxBendCoeffs(self,NEd,NplRd):
        '''Return (alpha,beta) constants for bi-axial bending criterion 
        (clause 6.2.9 of EC3.1.1)
        '''
        n= NEd/NplRd
        if self.name[0] in ['I','H']:
            alpha=2
            beta=max(1,5*n)
        elif self.name[:2] == 'CH':
            alpha=2
            beta=2
        elif self.name[:2] in ['RH','SH']:
            alpha=min(6,abs(1.66/(1-1.13*n**2)))
            beta=alpha
        else:  #conservative
            alpha=1
            beta=1
        return (alpha,beta)
    
    def getBiaxialBendingEfficiency(self,sectionClass,Nd,Myd,Mzd,Vyd= 0.0,chiLT=1.0):
        '''Return biaxial bending efficiency (clause 6.2.9 of EC3.1.1)
        (only class 1 and 2 cross-sections are considered currently)

        :param sectionClass: section classification (1,2,3 or 4)
        :param chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        NcRd= self.getNcRd(sectionClass)
        McRdy= self.getMcRdy(sectionClass)
        McRdz= self.getMcRdz(sectionClass)
        MvRdz= self.getMvRdz(sectionClass,Vyd)
        MbRdz= chiLT*MvRdz #Lateral buckling reduction.
        alpha,beta=self.getBiaxBendCoeffs(Nd,NcRd)
        CF=(abs(Mzd)/MbRdz)**alpha+(abs(Myd)/McRdy)**beta
        return (CF,NcRd,McRdy,McRdz,MvRdz,MbRdz)

    def setupULSControlVars(self, elems, sectionClass= 1, chiLT=1.0):
        '''For each element creates the variables
           needed to check ultimate limit state criterion to be satisfied.

        :param elems: elements to define properties on.
        :param sectionClass: section classification (1,2,3 or 4)
        :param chiLT: lateral buckling reduction factor (default= 1.0).
        '''
        super(EC3Shape,self).setupULSControlVars(elems)
        for e in elems:
            e.setProp('sectionClass',sectionClass) #Cross section class.
            e.setProp('chiLT',chiLT) #Lateral torsional buckling reduction factor.
            e.setProp('crossSection',self)

    def installULSControlRecorder(self, recorderType, elems, sectionClass= 1, chiLT=1.0, calcSet= None):
        '''Installs recorder for verification of ULS criterion. Preprocessor obtained from the set of elements.

        :param recorderType: recorder type.
        :param sectionClass: section classification (1,2,3 or 4)
        :param chiLT: lateral buckling reduction factor (default= 1.0).
        :param calcSet: set of elements to be checked (defaults to 'None' which 
                        means that this set will be created elsewhere). In not
                        'None' the member elements will be appended to this set.
        '''
        recorder= self.createRecorder(recorderType, calcSet)

        self.setupULSControlVars(elems,sectionClass,chiLT)
        nodHndlr= self.getPreprocessor().getNodeHandler        
        if(nodHndlr.numDOFs==3):
            recorder.callbackRecord= EC3lsc.controlULSCriterion2D()
        else:
            recorder.callbackRecord= EC3lsc.controlULSCriterion()

        recorder.callbackRestart= "print(\"Restart method called.\")"
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

from materials.sections.structural_shapes import arcelor_metric_shapes


def getIShapedRolledSectionBucklingCurve(shape, majorAxis):
    ''' Return the buckling curve  (a0,a,b,c or d) for I shaped rolled
       cross-sections according to table 6.2 of EN 1993-1-1:2005 "Selection 
       of buckling  curve for a cross-section". Note that in table 6.2 Y and 
       Z axes are swapped with respect to those used in XC. 
       XC axes: Y->weak axis, Z->strong axis.

    :param majorAxis: true if buckling around major axis.
    '''
    fy= shape.steelType.fy # steel fy
    h= shape.h() # section depth.
    b= shape.b() # flange width.
    tf= shape.tf() # flange thickness.
    if(h/b>1.2):
        if(tf<=0.04): # tf<=40 mm
            if(fy>=460e6):
                retval= 'a0'
            else:
                if(majorAxis):
                    retval= 'a'
                else:
                    retval= 'b'
        elif(tf<=0.1): # 40 mm < tf <= 100 mm
            if(fy>=460e6):
                retval= 'a'
            else:
                if(majorAxis):
                    retval= 'b'
                else:
                    retval= 'c'
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+': flange too thick: '+str(tf/1e3)+' mm')
    else: # h/b<=1.2
        if(tf<=0.1): # tf<=100 mm
            if(fy>=460e6):
                retval= 'a'
            else:
                if(majorAxis):
                    retval= 'b'
                else:
                    retval= 'c'
        else: # tf > 100 mm
            if(fy>=460e6):
                retval= 'c'
            else:
                if(majorAxis):
                    retval= 'd'
                else:
                    retval= 'd'
    return retval

def getHollowShapedSectionBucklingCurve(shape, hotFinished):
    ''' Return the buckling curve  (a0,a,b,c or d) for O shaped rolled
       cross-sections according to table 6.2 of EN 1993-1-1:2005 "Selection 
       of buckling  curve for a cross-section". Note that in table 6.2 Y and 
       Z axes are swapped with respect to those used in XC. 
       XC axes: Y->weak axis, Z->strong axis.

    :param hotFinished: true if hot finished steel false for cold formed.
    '''
    fy= shape.steelType.fy # steel fy
    if(hotFinished): # hot finished steel
        if(fy>=460e6):
            retval= 'a0'
        else:
            retval= 'a'
    else: # cold formed steel.
        retval= 'c'
    return retval    

class IPNShape(EC3Shape,arcelor_metric_shapes.IPNShape):
    """IPN shape with Eurocode 3 verification routines."""
    def __init__(self, steel, name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. IPN_160)
        '''
        super(IPNShape, self).__init__(name= name, typo= 'rolled')
        arcelor_metric_shapes.IPNShape.__init__(self,steel,name)

    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        return getIShapedRolledSectionBucklingCurve(self, majorAxis)


class IPEShape(EC3Shape,arcelor_metric_shapes.IPEShape):
    '''IPE shape with Eurocode 3 verification routines.'''
    def __init__(self, steel, name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. IPE_600)
        '''
        super(IPEShape, self).__init__(name= name, typo= 'rolled')
        arcelor_metric_shapes.IPEShape.__init__(self,steel,name)
        
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        return getIShapedRolledSectionBucklingCurve(self, majorAxis)

class SHSShape(EC3Shape,arcelor_metric_shapes.SHSShape):
    """SHS shape with Eurocode 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. 'SHS175x175x8')
        '''
        super(SHSShape, self).__init__(name= name, typo= 'rolled')
        arcelor_metric_shapes.SHSShape.__init__(self,steel,name)
        
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        hotFinished= (self.typo=='rolled')
        return getHollowShapedSectionBucklingCurve(self, hotFinished= hotFinished)

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

class HEShape(EC3Shape,arcelor_metric_shapes.HEShape):
    """HE shape with Eurocode 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. HE_600_A)
        '''
        super(HEShape, self).__init__(name= name, typo= 'rolled')
        arcelor_metric_shapes.HEShape.__init__(self,steel,name)
        
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        return getIShapedRolledSectionBucklingCurve(self, majorAxis)

class UPNShape(EC3Shape,arcelor_metric_shapes.UPNShape):
    """UPN shape with Eurocode 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. UPN_320)
        '''
        super(UPNShape, self).__init__(name= name, typo= 'rolled')
        arcelor_metric_shapes.UPNShape.__init__(self,steel,name)

class AUShape(EC3Shape,arcelor_metric_shapes.AUShape):
    """AU shape with Eurocode 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. AU_23)
        '''
        super(AUShape, self).__init__(name= name, typo= 'rolled')
        arcelor_metric_shapes.AUShape.__init__(self,steel,name)

class CHSShape(EC3Shape,arcelor_metric_shapes.CHSShape):
    """CHS shape with Eurocode 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. AU_23)
        '''
        super(CHSShape, self).__init__(name= name, typo='rolled')
        arcelor_metric_shapes.CHSShape.__init__(self,steel,name)

    def getClassInCompression(self):
        '''Return the cross-section classification of the section 
        subject to compression. Clause 5.5 EC3-1-1.
        '''
        return self.getClassInternalPartInCompression(ratioCT= self.getSlendernessRatio())

    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        hotFinished= (self.typo=='rolled')
        return getHollowShapedSectionBucklingCurve(self, hotFinished= hotFinished)
    
    def getNcRd(self):
        '''Return the axial compression resistance of the cross-section.'''
        return super(CHSShape, self).getNcRd(self.getClassInCompression())

        
class RHSShape(EC3Shape,arcelor_metric_shapes.RHSShape):
    """RHS shape with Eurocode 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. AU_23)
        '''
        super(RHSShape, self).__init__(name= name, typo= 'rolled')
        arcelor_metric_shapes.RHSShape.__init__(self,steel,name)
        
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        hotFinished= (self.typo=='rolled')
        return getHollowShapedSectionBucklingCurve(self, hotFinished= hotFinished)
    
class UCShape(EC3Shape,arcelor_metric_shapes.UCShape):
    """UC shape with Eurocode 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. UC_23)
        '''
        super(UCShape, self).__init__(name= name, typo= 'rolled')
        arcelor_metric_shapes.UCShape.__init__(self,steel,name)
        
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        hotFinished= (self.typo=='rolled')
        return getHollowShapedSectionBucklingCurve(self, hotFinished= hotFinished)
        
class UBShape(EC3Shape,arcelor_metric_shapes.UBShape):
    """UB shape with Eurocode 3 verification routines."""
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. UB356x127x33)
        '''
        super(UBShape, self).__init__(name= name, typo= 'rolled')
        arcelor_metric_shapes.UBShape.__init__(self,steel,name)

from materials.sections.structural_shapes import bs_en_10210_shapes

class HFSHSShape(EC3Shape, bs_en_10210_shapes.HFSHSShape):
    """BS EN 10210-2: 2006 steel shapes with Eurocode 3 
       verification routines.
    """
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. 'HFSHS300x300x10.0')
        '''
        super(HFSHSShape, self).__init__(name= name, typo= 'rolled')
        bs_en_10210_shapes.HFSHSShape.__init__(self,steel,name)
        
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        hotFinished= (self.typo=='rolled')
        return getHollowShapedSectionBucklingCurve(self, hotFinished= hotFinished)
        
from materials.sections.structural_shapes import bs_en_10219_shapes

class CFSHSShape(EC3Shape, bs_en_10219_shapes.CFSHSShape):
    """BS EN 10219-2: cold formed square hollow steel shapes 
       with Eurocode 3 verification routines.
    """
    def __init__(self, steel, name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. 'HFSHS300x300x10.0')
        '''
        super(CFSHSShape, self).__init__(name= name, typo= 'welded')
        bs_en_10219_shapes.CFSHSShape.__init__(self, steel, name)
        
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        hotFinished= (self.typo=='rolled')
        return getHollowShapedSectionBucklingCurve(self, hotFinished= hotFinished)
        
class CFRHSShape(EC3Shape, bs_en_10219_shapes.CFRHSShape):
    """BS EN 10219-2: cold formed rectangular hollow steel shapes 
       with Eurocode 3 verification routines.
    """
    def __init__(self, steel, name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. 'HFSHS300x300x10.0')
        '''
        super(CFRHSShape, self).__init__(name= name, typo= 'welded')
        bs_en_10219_shapes.CFRHSShape.__init__(self, steel= steel, name= name)
        
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        hotFinished= (self.typo=='rolled')
        return getHollowShapedSectionBucklingCurve(self, hotFinished= hotFinished)
        
class CFCHSShape(EC3Shape, bs_en_10219_shapes.CFCHSShape):
    """BS EN 10219-2: cold formed circular hollow steel shapes 
       with Eurocode 3 verification routines.
    """
    def __init__(self, steel, name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. 'HFSHS300x300x10.0')
        '''
        super(CFCHSShape, self).__init__(name= name, typo= 'welded')
        bs_en_10219_shapes.CFCHSShape.__init__(self, steel= steel, name= name)
        
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        hotFinished= (self.typo=='rolled')
        return getHollowShapedSectionBucklingCurve(self, hotFinished= hotFinished)

from materials.sections.structural_shapes import common_micropile_tubes

class MicropileTubeShape(EC3Shape, common_micropile_tubes. MicropileTubeShape):
    '''Common micropile tube shapes with Eurocode 3 verification routines.
    '''
    def __init__(self, steel, name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. 'HFSHS300x300x10.0')
        '''
        super(MicropileTubeShape, self).__init__(name= name, typo= 'rolled')
        common_micropile_tubes.MicropileTubeShape.__init__(self, steel= steel, name= name)
        
    def getClassInCompression(self):
        '''Return the cross-section classification of the section 
        subject to compression. Clause 5.5 EC3-1-1.
        '''
        return self.getClassInternalPartInCompression(ratioCT= self.getSlendernessRatio())
    
    def getBucklingCurve(self, majorAxis= False):
        ''' Return the buckling curve  (a0,a,b,c or d) for this cross-section 
            according to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        '''
        hotFinished= (self.typo=='rolled')
        return getHollowShapedSectionBucklingCurve(self, hotFinished= hotFinished)

    def getNcRd(self):
        '''Return the axial compression resistance of the cross-section.'''
        return super(MicropileTubeShape, self).getNcRd(self.getClassInCompression())

class EC3BoltSteel(steel_base.BasicSteel):
    '''Eurocode 3 structural steel for bolts according to table 3.1 of
       EC3-1-8:2005.

      :ivar gammaM2: partial safety factor for steel strength.
    '''
    def __init__(self, name= None, fy= 240e6, fu= 400e6, gammaM2= 1.25):
        ''' Constructor.
          :param fy:  yield stress.
          :param fu: ultimate stress.
          :param gammaM2: partial safety factor for steel strength.
        '''

        super(EC3BoltSteel,self).__init__(E= 210000e6,nu= 0.3,fy= fy, fu= fu, gammaM= gammaM2)
        if(name):
            self.name= name
        else:
            self.name= None

    def getAlpha_v(self, threadsExcluded= False):
        ''' Return the alpha_v coefficient of the fastener according
            to table 3.4 of EC3-1-8:2005.

        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        retval= 0.5
        if(threadsExcluded):
            retval= 0.6
        elif((self.name=='4.6') or (self.name=='5.6') or (self.name=='8.8')):
            retval= 0.6
        return retval
    
    def getNominalShearStrength(self, threadsExcluded= False):
        ''' Return the nominal shear strength of the steel according
            to table 3.4 of EC3-1-8:2005.

        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        return self.getAlpha_v(threadsExcluded= threadsExcluded)*self.fu

    def getDesignShearStrength(self, threadsExcluded= False):
        ''' Return the design shear strength of the steel according
            to table 3.4 of EC3-1-8:2005.

        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        '''
        return self.getNominalShearStrength(threadsExcluded= threadsExcluded)/self.gammaM
    
bolt4dot6Steel= EC3BoltSteel(name='4.6', fy= 240e6, fu= 400e6, gammaM2= 1.25)
bolt4dot8Steel= EC3BoltSteel(name= '4.8', fy= 320e6, fu= 400e6, gammaM2= 1.25)
bolt5dot6Steel= EC3BoltSteel(name= '5.6', fy= 300e6, fu= 500e6, gammaM2= 1.25)
bolt6dot8Steel= EC3BoltSteel(name= '6.8', fy= 480e6, fu= 600e6, gammaM2= 1.25)
bolt8dot8Steel= EC3BoltSteel(name= '8.8', fy= 640e6, fu= 800e6, gammaM2= 1.25)
bolt10dot9Steel= EC3BoltSteel(name= '10.9', fy= 900e6, fu= 1000e6, gammaM2= 1.25)
       
class BoltFastener(bolts.BoltBase):
    ''' Bolt according to chapter 3 of EC3-1-8:2005.

    :ivar steelType: type of the bolt steel. 
    '''
    def __init__(self, diameter, steelType= bolt8dot8Steel, gammaM3= 1.25, pos3d= None):
       ''' Constructor.

       :param diameter: bolt diameter.
       :param steelType: bolt steel type.
       :param gammaM3: partial safety factor for slip resistance.
       :param pos3d: bolt position.
       '''
       super(BoltFastener,self).__init__(diameter, pos3d)
       self.steelType= steelType
       self.gammaM3= gammaM3
    
    def getNominalShearStrength(self, threadsExcluded= False, numberOfShearPlanes= 1):
        ''' Return the nominal shear strength of the fastener according
            to table 3.4 of EC3-1-8:2005.

        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        :param numberOfShearPlanes: number of shear planes.
        '''
        return self.steelType.getNominalShearStrengthv(threadsExcluded= threadsExcluded)*self.getTensileStressArea(threadsExcluded= threadsExcluded)*numberOfShearPlanes

    def getDesignShearStrength(self, threadsExcluded= False, numberOfShearPlanes= 1):
        ''' Return the design shear strength of the fastener according
            to table 3.4 of EC3-1-8:2005.

        :param threadsExcluded: true if threads and transition area of 
                                shank are excluded from the shear plane.
        :param numberOfShearPlanes: number of shear planes.
        '''
        return self.steelType.getDesignShearStrength()*self.getTensileStressArea(threadsExcluded= threadsExcluded)*numberOfShearPlanes

    def getKs(self, holeType= 'normal', slotParallelToLoad= False):
        ''' Return the value of k_s according to table 3.6 of EC3-1-8:2005.

        :param holeType: 'normal' or 'shortSlot' or 'longSlot'.
        :param slotParallelToLoad: true if the load is parallel to the axis of the slot.
        '''
        retval= 0.63 # pessimistic assumption.
        if(holeType=='normal'):
            retval= 1.0
        else:
            if(slotParallelToLoad and (holeType=='shortSlot')):
                retval= 0.76
            else: # load perpendicular to the slot axis.
                if(holeType=='shortSlot'): # short slotted holes
                    retval= 0.85
                else:
                    retval= 0.7
               
        return retval

    def getSlipFactor(self, classOfFrictionSurfaces= 'D'):
        ''' Return the value of the slip factor according to table 3.7
            of EC3-1-8:2005.

        :param classOfFrictionSurfaces: class of friction surfaces
                                       (see 1.2.7 Reference standard: Group 7).
        '''
        retval= 0.2
        if(classOfFrictionSurfaces=='A'):
            retval= 0.5
        elif(classOfFrictionSurfaces=='B'):
            retval= 0.4
        elif(classOfFrictionSurfaces=='C'):
            retval= 0.3
        elif(classOfFrictionSurfaces=='D'):
            retval= 0.2
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+': unknown class of friction: '+str(classOfFrictionSurfaces))
        return retval

    def getNominalSlipStrength(self, holeType= 'normal', slotParallelToLoad= False, numberOfFrictionSurfaces= 1, classOfFrictionSurfaces= 'D'):
        ''' Return the value the design value of the  slip resistance according
            to clause 3.9.1 of EC3-1-8:2005.

        :param holeType: 'normal' or 'shortSlot' or 'longSlot'.
        :param slotParallelToLoad: true if the load is parallel to the axis of the slot.
        '''
        ks= self.getKs(holeType= holeType, slotParallelToLoad= slotParallelToLoad)
        mu= self.getSlipFactor(classOfFrictionSurfaces=classOfFrictionSurfaces)
        FpC= 0.7*self.steelType.fu*self.getTensileStressArea(threadsExcluded= False)
        return FpC*ks*numberOfFrictionSurfaces*mu
    
    def getDesignSlipStrength(self, holeType= 'normal', slotParallelToLoad= False, numberOfFrictionSurfaces= 1, classOfFrictionSurfaces= 'D'):
        ''' Return the value the design value of the  slip resistance according
            to clause 3.9.1 of EC3-1-8:2005.

        :param holeType: 'normal' or 'shortSlot' or 'longSlot'.
        :param slotParallelToLoad: true if the load is parallel to the axis of the slot.
        '''
        return self.getNominalSlipStrength(holeType= holeType, slotParallelToLoad= slotParallelToLoad, numberOfFrictionSurfaces= numberOfFrictionSurfaces, classOfFrictionSurfaces= classOfFrictionSurfaces)/self.gammaM3
