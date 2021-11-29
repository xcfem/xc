# -*- coding: utf-8 -*-
''' Base classes and functions for structural steel.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import math
from materials.sections import section_properties as sp
from postprocess import def_vars_control as vc
from misc_utils import log_messages as lmsg
import xc_base
import geom
import materials


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


class SteelShape(sp.SectionProperties):
    '''Properties of a section in structural steel

    :ivar steel:    steel object (e.g. S275JR)
    :ivar table:    module containing a dictionary with mechanical characteristics
                    of a series of shapes 
                    (e.g. materials.sections.structural_shapes.arcelor_metric_shapes.IPE)
    :ivar name:     name identifying the section in the table
    '''
    def __init__(self,steel,name,table):
        ''' Constructor.

        :param steel: steel object (e.g. S275JR)
        :param name: name identifying the section in the table
        :param table: module containing a dictionary with mechanical 
                      characteristics of a series of shapes 
                      (e.g. materials.sections.structural_shapes.arcelor_metric_shapes.IPE)
        '''
        self.steelType= steel
        if(name!=''):
            self.shape= table[name]
        else:
            self.shape= None
        super(SteelShape,self).__init__(name)

    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(SteelShape, self).getDict()
        steelTypeClassName= str(self.steelType.__class__)[8:-2]
        retval.update({'steelTypeClassName':steelTypeClassName, 'steelType':self.steelType.getDict(), 'shape': self.shape})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        super(SteelShape, self).setFromDict(dct)
        steelTypeClassName= dct['steelTypeClassName']+'()'
        self.steelType= eval(steelTypeClassName)
        self.steelType.setFromDict(dct['steelType'])
        self.shape= dct['shape']

    def getSymmetry(self):
        ''' Returns the symmetry of the shape: 
            'double', 'simple' or 'none'.'''
        return 'none'
    
    def get(self,code):
        return self.shape[code]

    def A(self):
        '''return cross-sectional area'''
        return self.get('A')

    def Iy(self):
        '''return second moment of area about y-axis (weak axis)'''
        return self.get('Iy')

    def getGyrationRadiusY(self):
        '''return radius of gyration about y-axis (weak axis)'''
        return math.sqrt(self.Iy()/self.A())

    def getSlendernessY(self,Leq):
        '''return buckling slenderness relative to y-axis (weak axis)

           :param Leq: buckling length in XZ buckling plane.       
        '''
        return Leq/self.getGyrationRadiusY()

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

    def getBucklingReductionFactorY(self,Leq,bucklingCurve,sectionClass= 1):
        '''return buckling reduction factor relative to y-axis (weak axis) 
        as defined in EC3-1-1 6.3.1

        :param Leq: buckling length in XZ buckling plane.
        :param bucklingCurve: buckling curve (a0,a,b,c or d) according 
           to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        :param sectionClass: class of the section (1 to 3, 4 not yet 
               implemented) (defaults to 1)
        '''
        alpha= alphaImperfectionFactor(bucklingCurve)
        lmb= self.getAdimensionalSlendernessY(Leq,sectionClass= 1)
        phi= 0.5*(1+alpha*(lmb-0.2)+lmb**2)
        return min(1.0/(phi+math.sqrt(phi**2-lmb**2)),1.0)

    def getBucklingResistanceY(self,Leq,bucklingCurve,sectionClass= 1):
        '''return buckling resistance relative to y-axis (weak axis)
        according to EC3-1-1 6.3.2

        :param Leq: buckling length in XZ buckling plane.
        :param bucklingCurve: buckling curve (a0,a,b,c or d) according 
           to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        :param sectionClass: class of the section (1 to 3, 4 not yet implemented) (defaults to 1).
        '''
        X= self.getBucklingReductionFactorY(Leq,bucklingCurve,sectionClass)
        return X*self.getAeff(sectionClass)*self.steelType.fyd()

    def Iz(self):
        '''return second moment of area about z-axis (strong axis)'''
        return self.get('Iz')

    def getGyrationRadiusZ(self):
        '''return radius of gyration about z-axis (strong axis)'''
        return math.sqrt(self.Iz()/self.A())

    def getSlendernessZ(self,Leq):
        '''return buckling slenderness relative to z-axis (strong axis)

           :param Leq: buckling length in XY buckling plane.
        '''
        return Leq/self.getGyrationRadiusZ()

    def getAdimensionalSlendernessZ(self,Leq,sectionClass= 1):
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

    def getBucklingReductionFactorZ(self,Leq,bucklingCurve,sectionClass= 1):
        '''return buckling reduction factor lative to z-axis (strong axis) 
        as defined in EC3-1-1 6.3.1

        :param Leq: buckling length in XY buckling plane.
        :param bucklingCurve: buckling curve (a0,a,b,c or d) according 
           to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        :param sectionClass: class of the section (1 to 3, 4 not yet 
               implemented) (defaults to 1)
        '''
        alpha= alphaImperfectionFactor(bucklingCurve)
        lmb= self.getAdimensionalSlendernessZ(Leq,sectionClass= 1)
        phi= 0.5*(1+alpha*(lmb-0.2)+lmb**2)
        return min(1.0/(phi+math.sqrt(phi**2-lmb**2)),1.0)

    def getBucklingResistanceZ(self,Leq,bucklingCurve,sectionClass= 1):
        '''return buckling resistance relative to z-axis (strong axis)
        according to EC3-1-1 6.3.2

        :param Leq: buckling length in XY buckling plane.
        :param bucklingCurve: buckling curve (a0,a,b,c or d) according 
           to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        :param sectionClass: class of the section (1 to 3, 4 not yet 
               implemented) (defaults to 1).
        '''
        X= self.getBucklingReductionFactorZ(Leq,bucklingCurve,sectionClass)
        return X*self.getAeff(sectionClass)*self.steelType.fyd()

    def getBucklingResistance(self,LeqY,LeqZ,bucklingCurveY,bucklingCurveZ,sectionClass= 1):
        '''return minimum of buckling resistance in XY and XZ buckling planes
        calculated according to EC3-1-1 6.3.2

        :param LeqY: buckling length of the member in XZ buckling plane.
        :param LeqZ: buckling length of the member in XY buckling plane.
        :param bucklingCurveY: buckling curve (a0,a,b,c or d) with respect 
           to y-axis (weak axis) according 
           to table 6.2 of EN 1993-1-1:2005 "Selection of buckling 
           curve for a cross-section". Note that in table 6.2 Y and 
           Z axes are swapped with respect to those used in XC. 
           XC axes: Y->weak axis, Z->strong axis.
        :param bucklingCurveY: buckling curve (a0,a,b,c or d) with respect 
           to z-axis (strong axis)
        :param sectionClass: class of the section (1 to 3, 4 not yet 
               implemented) (defaults to 1)
        '''
        rY= self.getBucklingResistanceY(LeqY,bucklingCurveY,sectionClass)
        rZ= self.getBucklingResistanceZ(LeqZ,bucklingCurveZ,sectionClass)
        return min(rY,rZ)

    def J(self):
        '''return torsional moment of inertia of the section'''
        return self.get('It')*self.torsionalStiffnessFactor

    def It(self):
        '''return Saint Venant torsional constant.'''
        return self.get('It')

    def Iw(self):
        '''return warping constant.'''
        return self.get('Iw')

    def EIy(self):
        ''' return the product E*Iy'''
        return self.steelType.E*self.Iy()

    def EIz(self):
        ''' return the product E*Iz'''
        return self.steelType.E*self.Iz()

    def GJ(self):
        ''' return the product G*J (G=shear modulus, J=St. Venant torsional constant) '''
        return self.steelType.G()*self.It()

    def getWz(self,sectionClass= 1):
        '''return section modulus with respect to z-axis (strong axis)

        :param sectionClass: 1 to 3 (4 not yet implemented) (defaults to 1)
        '''
        if(sectionClass<3):
            return self.get('Wzpl')
        elif(sectionClass==3):
            return self.get('Wzel')
        else:
            lmsg.warning('cross sections of class: '+ str(sectionClass) + ' not implemented.')

    def getWy(self,sectionClass= 1):
        '''return section modulus with respect to y-axis (weak axis)

        :param sectionClass: 1 to 3 (4 not yet implemented) (defaults to 1)
        '''
        if(sectionClass<3):
            return self.get('Wypl')
        elif(sectionClass==3):
            return self.get('Wyel')
        else:
            lmsg.warning('cross sections of class: '+ str(sectionClass) + ' not implemented.')

    def getAeff(self,sectionClass= 1):
        '''return effective area depending on the cross-section class.

        :param sectionClass: class of the section (1 to 3, 4 not yet 
               implemented) (defaults to 1)
        '''
        retval= self.A()
        if(sectionClass>=3):
            lmsg.warning('effective area for sections of class: '+ str(sectionClass) + ' not implemented.')
            retval/=100.0
        return retval

    def alphaY(self):
        '''return shear shape factor with respect to y-axis (weak axis)'''
        return self.get('Avy')/self.A()
    
    def alphaZ(self):
        '''return shear shape factor with respect to z-axis (strong axis)'''
        return 1.0-self.alphaY()

#Avz is not usually defined for steel sections
#    def alphaZ(self):
#        '''return shear shape factor with respect to z-axis (strong axis)'''
#        return self.get('Avz')/self.A()

    def getNcrY(self,Leq):
        '''return theoretical critical axial force on y-axis (weak axis).

         :param Leq: buckling length of the member in XZ buckling plane.
        '''
        return math.pi**2*self.EIy()/(Leq**2)

    def getNcrZ(self,Leq):
        '''return theoretical critical axial force on z-axis (strong axis).

         :param Leq: buckling length  of the member in XY buckling plane.
        '''
        return math.pi**2*self.EIz()/(Leq**2)

    def getNcr(self,LeqY,LeqZ):
        '''return theoretical critical axial force  (minimum of NcrY and NcrZ).

           :param LeqY: buckling length  of the member in XZ buckling plane.
           :param LeqZ: buckling length  of the member in XY buckling plane.
        '''
        return min(self.getNcrY(LeqY),self.getNcrZ(LeqZ))

    def setupULSControlVars(self,elems):
        '''For each element creates the variables
           needed to check ultimate limit state criterion to satisfy.'''
        vc.defVarsEnvelopeInternalForcesBeamElems(elems)
        for e in elems:
            e.setProp('FCTNCP',[-1.0,-1.0]) #Normal stresses efficiency.
            e.setProp('FCVCP',[-1.0,-1.0]) #Shear stresses efficiency.

    def checkUniaxialBendingForElement(self,elem,nmbComb):
        '''Called in every commit to check uniaxial bending criterion (bars in 2D problems).

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        sectionClass= elem.getProp('sectionClass')
        chiLT= elem.getProp('chiLT')
        chiN= elem.getProp('chiN')
        N1= 0.0; M1= 0.0; V1= 0.0
        N2= 0.0; M2= 0.0; V2= 0.0
        axialForces= elem.getValuesAtNodes('N', False)
        if(len(axialForces)>1): # 'N' found.
            N1= axialForces[0]
            N2= axialForces[1]
        bending= elem.getValuesAtNodes('M', False)
        if(len(bending)>1): # 'M' found.
            M1= bending[0]
            M2= bending[1]
        shear= elem.getValuesAtNodes('V', False)
        if(len(shear)>1): # 'V' found.
            V1= shear[0]
            V2= shear[1]
        FCTN1= self.getZBendingEfficiency(Nd= N1, Mzd= M1, Vyd= V1,chiN= chiN, chiLT= chiLT, sectionClass= sectionClass)[0]
        FCTN2= self.getZBendingEfficiency(Nd= N2, Mzd= M2, Vyd= V2,chiN= chiN, chiLT= chiLT, sectionClass= sectionClass)[0]
        fctn= elem.getProp("FCTNCP")
        if(FCTN1 > fctn[0]):
            fctn[0]= FCTN1
            elem.setProp("HIPCPTN1",nmbComb)
        if(FCTN2 > fctn[1]):
            fctn[1]= FCTN2
            elem.setProp("HIPCPTN2",nmbComb)
        elem.setProp("FCTNCP",fctn)
        vc.updateEnvelopeInternalForcesBeamElem2D(elem)

    def checkBiaxialBendingForElement(self,elem,nmbComb):
        '''Called in every commit to check biaxial bending criterion 
            (bars in 3D problems).

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        sectionClass= elem.getProp('sectionClass')
        chiLT= elem.getProp('chiLT')
        chiN= elem.getProp('chiN')
        N1= 0.0; My1= 0.0; Mz1= 0.0; Vy1= 0.0;
        N2= 0.0; My2= 0.0; Mz2= 0.0; Vy2= 0.0;
        axialForces= elem.getValuesAtNodes('N', False)
        if(len(axialForces)>1): # 'N' found.
            N1= axialForces[0]
            N2= axialForces[1]
        bendingY= elem.getValuesAtNodes('My', False)
        if(len(bendingY)>1): # 'My' found.
            My1= bendingY[0]
            My2= bendingY[1]
        bendingZ= elem.getValuesAtNodes('Mz', False)
        if(len(bendingZ)>1): # 'Mz' found.
            Mz1= bendingZ[0]
            Mz2= bendingZ[1]
        shearY= elem.getValuesAtNodes('Vy', False)
        if(len(shearY)>1): # 'Vy' found.
            Vy1= shearY[0]
            Vy2= shearY[1]
        FCTN1= self.getBiaxialBendingEfficiency(Nd= N1, Myd= My1, Mzd= Mz1, Vyd= Vy1, chiN= chiN, chiLT= chiLT)[0]
        FCTN2= self.getBiaxialBendingEfficiency(Nd= N2, Myd= My2, Mzd= Mz2, Vyd= Vy2, chiN= chiN, chiLT= chiLT)[0]
        fctn= elem.getProp("FCTNCP")
        if(FCTN1 > fctn[0]):
            fctn[0]= FCTN1
            elem.setProp("HIPCPTN1",nmbComb)
        if(FCTN2 > fctn[1]):
            fctn[1]= FCTN2
            elem.setProp("HIPCPTN2",nmbComb)
        elem.setProp("FCTNCP",fctn)
        vc.updateEnvelopeInternalForcesBeamElem(elem)

    def checkYShearForElement(self,elem,nmbComb):
        '''Called in every commit to y shear criterion.

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        sectionClass= elem.getProp('sectionClass')
        Vy1= 0.0
        Vy2= 0.0
        shearY= elem.getValuesAtNodes('Vy', False)
        if(len(shearY)>1): # 'Vy' found.
            Vy1= shearY[0]
            Vy2= shearY[1]
        FCV1= self.getYShearEfficiency(sectionClass,Vy1)
        FCV2= self.getYShearEfficiency(sectionClass,Vy2)
        fcv= elem.getProp("FCVCP")
        if(FCV1 > fcv[0]):
            fcv[0]= FCV1
            elem.setProp("HIPCPV1",nmbComb)
        if(FCV2 > fcv[1]):
            fcv[1]= FCV2
            elem.setProp("HIPCPV2",nmbComb)
        elem.setProp("FCVCP",fcv)
        
    def checkZShearForElement(self,elem,nmbComb):
        '''Called in every commit to z shear criterion.

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        sectionClass= elem.getProp('sectionClass')
        Vz1= 0.0
        Vz2= 0.0
        shearZ= elem.getValuesAtNodes('Vz', False)
        if(len(shearZ)>1): # 'Vz' found.
            Vz1= shearZ[0]
            Vz2= shearZ[1]
        FCV1= self.getZShearEfficiency(sectionClass,Vz1)
        FCV2= self.getZShearEfficiency(sectionClass,Vz2)
        fcv= elem.getProp("FCVCP")
        if(FCV1 > fcv[0]):
            fcv[0]= FCV1
            elem.setProp("HIPCPV1",nmbComb)
        if(FCV2 > fcv[1]):
            fcv[1]= FCV2
            elem.setProp("HIPCPV2",nmbComb)
        elem.setProp("FCVCP",fcv)
        
    def defElasticSection3d(self,preprocessor, overrideRho= None):
        ''' Return an elastic section appropiate for 3D beam analysis

        :param  preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticSection3d(preprocessor, self.steelType, overrideRho= overrideRho)
    
    def defElasticShearSection3d(self,preprocessor, overrideRho= None):
        '''elastic section appropiate for 3D beam analysis, including shear 
           deformations

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticShearSection3d(preprocessor, self.steelType, overrideRho= overrideRho)

    def defElasticSection1d(self,preprocessor, overrideRho= None):
        ''' Return an elastic section appropiate for truss analysis.

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticSection1d(preprocessor, self.steelType, overrideRho= overrideRho)
    
    def defElasticSection2d(self,preprocessor, majorAxis= True, overrideRho= None):
        ''' Return an elastic section appropiate for 2D beam analysis

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticSection2d(preprocessor, material= self.steelType, majorAxis= majorAxis, overrideRho= overrideRho)
    
    def defElasticShearSection2d(self,preprocessor, majorAxis= True, overrideRho= None):
        '''elastic section appropiate for 2D beam analysis, including shear deformations

        :param  preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticShearSection2d(preprocessor, material= self.steelType, majorAxis= majorAxis, overrideRho= overrideRho)
    
    def getCrossSectionProperties2D(self):
        '''Return a CrossSectionProperties object with the
         2D properties of the section.'''
        return super(SteelShape,self).getCrossSectionProperties2D(preprocessor, material= self.steelType)
        

class IShape(SteelShape):
    def __init__(self,steel,name,table):
        ''' Constructor.

        :param steel: steel object (e.g. S275JR)
        :param name: name identifying the section in the table
        :param table: module containing a dictionary with mechanical 
                      characteristics of a series of shapes.
        '''
        super(IShape,self).__init__(steel,name,table)
        self.tileSize= 0.01 #Size of tiles
        self.updateQuantities()

    def updateQuantities(self):
        ''' Update some derived values.'''
        if(self.shape):
            self.bHalf= self.get('b')/2.0 #Half flange width
            self.hHalf= self.get('h')/2.0 #Half section height
            self.hiHalf= self.get('hi')/2.0 #Half section interior height.
            self.twHalf= self.get('tw')/2.0 #Half web thickness
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super(IShape, self).getDict()
        retval.update({'tileSize':self.tileSize})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        super(IShape, self).setFromDict(dct)
        self.tileSize= dct['tileSize']
        self.updateQuantities()

    def getSymmetry(self):
        ''' Returns the symmetry of the shape: 
            'double', 'simple' or 'none'.'''
        return 'double'

    def b(self):
        return self.get('b')
      
    def h(self):
        '''Return shape height.'''
        return self.get('h')
      
    def tf(self):
        '''Return flange thickess'''
        return self.get('tf')
      
    def tw(self):
        '''Return web thickess'''
        return self.get('tw')
    
    def hw(self):
        '''Return web height'''
        return self.h()-2*self.tf()

    def d(self):
        '''Return internal web height'''
        return self.h()-2*self.tf()-2*self.r()

    def r(self):
        '''Return radius web-flange'''
        return self.get('r')
      
    def getRho(self):
        ''' Returns mass per unit length. '''
        return self.get('P')
      
    def getShapeRegions(self):
        ''' Returns regions valid for fiber section model creation. '''
        retval= list()
        #Lower flange
        p0= geom.Pos2d(-self.hHalf,-self.bHalf)
        p1= geom.Pos2d(-self.hiHalf,self.bHalf)
        retval.append([p0,p1])
        #Web
        p2= geom.Pos2d(-self.hiHalf,-self.twHalf)
        p3= geom.Pos2d(self.hiHalf,self.twHalf)
        retval.append([p2,p3])
        #Upper flange
        p4= geom.Pos2d(self.hiHalf,-self.bHalf,)
        p5= geom.Pos2d(self.hHalf,self.bHalf)
        retval.append([p4,p5])
        return retval

    def widthToThicknessWeb(self):
        '''return the ratio width-to-thickness for classification
        in web (table 5.2 EC3-1-1)
        '''
        c=self.d()
        t=self.tw()
        return c/t

    def widthToThicknessFlange(self):
        '''return the ratio width-to-thickness for classification
        in flange (table 5.2 EC3-1-1)
        '''
        c=(self.b()-self.tw()-2*self.r())/2.
        t=self.tf()
        return c/t
        
    def discretization(self,preprocessor,matModelName):
        self.sectionGeometryName= 'gm'+self.get('nmb')
        self.gm= preprocessor.getMaterialHandler.newSectionGeometry(self.sectionGeometryName)
        regions= self.gm.getRegions
        for r in self.getShapeRegions():
            reg= regions.newQuadRegion(matModelName)
            reg.pMin= r[0]
            reg.pMax= r[1]
            numberOfTiles= reg.setTileSize(self.tileSize,self.tileSize)
        return self.gm

    def getFiberSection3d(self,preprocessor,matModelName):
        reg= self.discretization(preprocessor,matModelName)
        self.fiberSection3dName= 'fs3d'+self.get('nmb')
        self.fiberSection3d= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d",self.fiberSection3dName)
        fiberSectionRepr= self.fiberSection3d.getFiberSectionRepr()
        fiberSectionRepr.setGeomNamed(self.sectionGeometryName)
        self.fiberSection3d.setupFibers()
        fibras= self.fiberSection3d.getFibers()
        return self.fiberSection3d


class QHShape(SteelShape):
    '''Quadrilateral hollow shape''' 
    def __init__(self,steel,name,table):
        ''' Constructor.

        :param steel: steel object (e.g. S275JR)
        :param name: name identifying the section in the table
        :param table: module containing a dictionary with mechanical 
                      characteristics of a series of shapes.
        '''
        super(QHShape,self).__init__(steel,name,table)
        self.bHalf= self.get('b')/2.0 #Half section width
        self.hHalf= self.get('h')/2.0 #Half section height

    def getSymmetry(self):
        ''' Returns the symmetry of the shape: 
            'double', 'simple' or 'none'.'''
        return 'double'

    def b(self):
        return self.get('b')
      
    def h(self):
        '''Return shape height.'''
        return self.get('h')
      
    def hw(self):
        '''Return web height'''
        return self.h()-2*self.t()
      
    def getRho(self):
        ''' Return mass per unit length. '''
        return self.get('P')

    def getOutsideRadius(self):
        ''' Return the outside radius as two times the wall thickness of 
        HS shape. (AISC Steel Construction Manual (1989 ASD) Tubing Dimensions 
        and Properties on pages 1-94 - 1-103 says: Properties are based upon a 
        nominal OUTISIDE corner radius equal to two times the wall thickness.)
        (This value is also used in example K.1 of AISC Design Examples V14.1)
        '''
        return 2*self.t()

    def widthToThicknessWeb(self):
        '''return the ratio width-to-thickness  for classification
        in web (table 5.2 EC3-1-1)
        '''
        c=self.h()-2*self.t()-2*self.t()
        t=self.t()
        return c/t

    def widthToThicknessHorzInt(self):
        '''return the internal ratio width-to-thickness  for classification
        in horizontal sup. and inf. plates (table 5.2 EC3-1-1)
        '''
        c=self.b()-2*self.t()-2*self.t()
        t=self.t()
        return c/t

class CHShape(SteelShape):
    '''Circular hollow shape''' 
    def __init__(self,steel,name,table):
        ''' Constructor.

        :param steel: steel object (e.g. S275JR)
        :param name: name identifying the section in the table
        :param table: module containing a dictionary with mechanical 
                      characteristics of a series of shapes.
        '''
        super(CHShape,self).__init__(steel,name,table)

    def getSymmetry(self):
        ''' Returns the symmetry of the shape: 
            'double', 'simple' or 'none'.'''
        return 'double'

    def getRho(self):
        ''' Returns mass per unit length. '''
        return self.get('P')
    
    def getOutsideDiameter(self):
        ''' Returns the outside diameter. '''
        return self.get('OD')

    
class UShape(SteelShape):
    def __init__(self,steel,name,table):
        ''' Constructor.

        :param steel: steel object (e.g. S275JR)
        :param name: name identifying the section in the table
        :param table: module containing a dictionary with mechanical 
                      characteristics of a series of shapes.
        '''
        super(UShape,self).__init__(steel,name,table)

    def getSymmetry(self):
        ''' Returns the symmetry of the shape: 
            'double', 'simple' or 'none'.'''
        return 'simple'

    def getRho(self):
        ''' Returns mass per unit length. '''
        return self.get('P')
      
    def h(self):
        '''Return shape height.'''
        return self.get('h')
      
    def d(self):
        '''Return internal web height.'''
        return self.get('d')
      
    def b(self):
        '''Return shape height.'''
        return self.get('b')
      
    def tf(self):
        '''Return flange thickess'''
        return self.get('tf')
      
    def tw(self):
        '''Return web thickess'''
        return self.get('tw')
      
    def hw(self):
        '''Return web height'''
        return self.h()-2*self.tf()

    def r(self):
        '''Return radius web-flange'''
        return self.get('r1')

    def widthToThicknessWeb(self):
        '''return the ratio width-to-thickness for classification
        in web (table 5.2 EC3-1-1)
        '''
        c=self.d()
        t=self.tw()
        return c/t

    def widthToThicknessFlange(self):
        '''return the ratio width-to-thickness for classification 
        in flange (table 5.2 EC3-1-1)
        '''
        c=self.b()-self.tw()-self.r()
        t=self.tf()
        return c/t

class LShape(SteelShape):
    ''' Single angle steel shape.

    '''
    def __init__(self,steel,name,table):
        ''' Constructor.

        :param steel: steel object (e.g. S275JR)
        :param name: name identifying the section in the table
        :param table: module containing a dictionary with mechanical 
                      characteristics of a series of shapes.
        '''
        super(LShape,self).__init__(steel,name,table)

    def getSymmetry(self):
        ''' Returns the symmetry of the shape: 
            'double', 'simple' or 'none'.'''
        return 'none'

