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
import geom
from model import model_inquiry



class SteelShape(sp.SectionProperties):
    '''Properties of a section in structural steel

    :ivar steelType: steel object (e.g. S275JR)
    :ivar shape: steel profile (i.e. IPE300)
    '''
    def __init__(self, steel, name, table):
        ''' Constructor.

        :param steel: steel material object (e.g. S275JR)
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

    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(SteelShape, self).__eq__(other)
                if(retval):
                    retval= (self.steelType == other.steelType)
                if(retval):
                    retval= (self.shape == other.shape)
            else:
                retval= True
        else:
            retval= False
        return retval
    
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
    
    def Avy(self):
        '''return shear area along y axis'''
        return self.get('Avy')
    
    def Avz(self):
        '''return shear area along z axis'''
        return self.get('Avz')

    def EA(self):
        '''return cross-sectional axial stiffness.'''
        return self.steelType.E*self.A()
    
    def GAy(self):
        '''return cross-sectional shear stiffness along y axis.'''
        return self.steelType.G()*self.Avy()
    
    def GAz(self):
        '''return cross-sectional shear stiffness along y axis.'''
        return self.steelType.G()*self.Avz()

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

    def getPlasticMomentZ(self):
        ''' Return the theoretical plastic moment of the section (without
            taking into acount the class of the section) with respect to
            z-axis (strong axis).'''
        return self.get('Wzpl')*self.steelType.fy
    
    def getPlasticMomentY(self):
        ''' Return the theoretical plastic moment of the section (without
            taking into acount the class of the section) with respect to
            y-axis (weak axis).'''
        return self.get('Wypl')*self.steelType.fy
    
    def getElasticMomentZ(self):
        ''' Return the theoretical elastic moment of the section (without
            taking into acount the class of the section) with respect to
            z-axis (strong axis).'''
        return self.get('Wzel')*self.steelType.fy
    
    def getElasticMomentY(self):
        ''' Return the theoretical elastic moment of the section (without
            taking into acount the class of the section) with respect to
            y-axis (weak axis).'''
        return self.get('Wyel')*self.steelType.fy

    def getWz(self):
        '''return section modulus with respect to z-axis (strong axis).
        '''
        if(self.sectionClass<3):
            return self.get('Wzpl')
        elif(self.sectionClass==3):
            return self.get('Wzel')
        else:
            lmsg.warning('cross sections of class: '+ str(self.sectionClass) + ' not implemented.')

    def getWy(self):
        '''return section modulus with respect to y-axis (weak axis).
        '''
        if(self.sectionClass<3):
            return self.get('Wypl')
        elif(self.sectionClass==3):
            return self.get('Wyel')
        else:
            lmsg.warning('cross sections of class: '+ str(self.sectionClass) + ' not implemented.')

    def getAeff(self):
        '''return effective area depending on the cross-section class.
        '''
        retval= self.A()
        if(self.sectionClass>=3):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+': effective area for sections of class: '+ str(self.sectionClass) + ' not implemented.')
            retval/=100.0
        return retval

    def alphaY(self):
        '''return shear shape factor with respect to y-axis (weak axis)'''
        return self.get('Avy')/self.A()
    
    def alphaZ(self):
        '''return shear shape factor with respect to z-axis (strong axis)'''
        retval= 1e-6
        if('Avz' in self.shape):
            retval= self.get('Avz')/self.A()
        return retval

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
           needed to check ultimate limit state criterion to satisfy.

        :param elems: elements to define properties on.
        '''
        vc.def_vars_envelope_internal_forces_beam_elems(elems)
        for e in elems:
            e.setProp('FCTNCP',[-1.0,-1.0]) #Normal stresses efficiency.
            e.setProp('FCVCP',[-1.0,-1.0]) #Shear stresses efficiency.

    def checkUniaxialBendingForElement(self,elem,nmbComb):
        '''Called in every commit to check uniaxial bending criterion 
           (bars in 2D problems).

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        chiLT= elem.getProp('chiLT') # Lateral torsional buckling reduction factor.
        chiN= elem.getProp('chiN') # Axial strength reduction factor.
        [[N1, M1, V1], [N2, M2, V2]]= model_inquiry.get_values_at_nodes(elem, ['N', 'M', 'V'], silent= False)
        FCTN1= self.getZBendingEfficiency(Nd= N1, Mzd= M1, Vyd= V1, chiN= chiN, chiLT= chiLT)[0]
        FCTN2= self.getZBendingEfficiency(Nd= N2, Mzd= M2, Vyd= V2, chiN= chiN, chiLT= chiLT)[0]
        fctn= elem.getProp("FCTNCP")
        if(FCTN1 > fctn[0]):
            fctn[0]= FCTN1
            elem.setProp("HIPCPTN1",nmbComb)
        if(FCTN2 > fctn[1]):
            fctn[1]= FCTN2
            elem.setProp("HIPCPTN2",nmbComb)
        elem.setProp("FCTNCP",fctn)
        vc.update_envelope_internal_forces_beam_elem_2d(elem, nmbComb)

    def checkBiaxialBendingForElement(self, elem, nmbComb):
        '''Called in every commit to check biaxial bending criterion 
            (bars in 3D problems).

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        chiLT= elem.getProp('chiLT') # Lateral torsional buckling reduction factor.
        chiN= elem.getProp('chiN') # Axial strength reduction factor.
        [[N1, My1, Mz1, Vy1], [N2, My2, Mz2, Vy2]]= model_inquiry.get_values_at_nodes(elem, ['N', 'My', 'Mz', 'Vy'], silent= False)
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
        vc.update_envelope_internal_forces_beam_elem_3d(elem, nmbComb)

    def checkYShearForElement(self,elem,nmbComb):
        '''Called in every commit to y shear criterion.

        :param elem: finite element to check.
        :param nmbComb: name of the load combination.
        '''
        elem.getResistingForce()
        [[Vy1], [Vy2]]= model_inquiry.get_values_at_nodes(elem,['Vy'], False)
        FCV1= self.getYShearEfficiency(Vyd= Vy1)
        FCV2= self.getYShearEfficiency(Vyd= Vy2)
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
        [[Vz1], [Vz2]]= model_inquiry.get_values_at_nodes(elem,['Vz'], False)
        FCV1= self.getZShearEfficiency(Vzd= Vz1)
        FCV2= self.getZShearEfficiency(Vzd= Vz2)
        fcv= elem.getProp("FCVCP")
        if(FCV1 > fcv[0]):
            fcv[0]= FCV1
            elem.setProp("HIPCPV1",nmbComb)
        if(FCV2 > fcv[1]):
            fcv[1]= FCV2
            elem.setProp("HIPCPV2",nmbComb)
        elem.setProp("FCVCP",fcv)
        
    def defElasticSection3d(self,preprocessor, overrideRho= None):
        ''' Return an elastic section appropriate for 3D beam analysis

        :param  preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticSection3d(preprocessor, self.steelType, overrideRho= overrideRho)
    
    def defElasticShearSection3d(self, preprocessor, overrideRho= None):
        '''elastic section appropriate for 3D beam analysis, including shear 
           deformations

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticShearSection3d(preprocessor, self.steelType, overrideRho= overrideRho)

    def defElasticSection1d(self,preprocessor, overrideRho= None):
        ''' Return an elastic section appropriate for truss analysis.

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticSection1d(preprocessor, self.steelType, overrideRho= overrideRho)
    
    def defElasticSection2d(self,preprocessor, majorAxis= True, overrideRho= None):
        ''' Return an elastic section appropriate for 2D beam analysis

        :param preprocessor: preprocessor object.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticSection2d(preprocessor, material= self.steelType, majorAxis= majorAxis, overrideRho= overrideRho)
    
    def defElasticShearSection2d(self,preprocessor, majorAxis= True, overrideRho= None):
        '''elastic section appropriate for 2D beam analysis, including shear deformations

        :param  preprocessor: preprocessor object.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super(SteelShape,self).defElasticShearSection2d(preprocessor, material= self.steelType, majorAxis= majorAxis, overrideRho= overrideRho)
    
    # def getCrossSectionProperties2D(self):
    #     '''Return a CrossSectionProperties object with the 2D properties of 
    #        the section.'''
    #     return super(SteelShape,self).getCrossSectionProperties2D(material= self.steelType)
    
    # def getCrossSectionProperties3D(self):
    #     '''Return a CrossSectionProperties object with the 3D properties of 
    #        the section.'''
    #     return super(SteelShape,self).getCrossSectionProperties2D(material= self.steelType)
        

class IShape(SteelShape):
    ''' I (or double tee) shaped steel profile.'''
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
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(IShape, self).__eq__(other)
                if(retval):
                    retval= (self.tileSize == other.tileSize)
            else:
                retval= True
        else:
            retval= False
        return retval

    def updateQuantities(self):
        ''' Update some derived values.'''
        if(self.shape):
            self.bHalf= self.get('b')/2.0 # Half flange width
            self.hHalf= self.get('h')/2.0 # Half section height
            self.hiHalf= self.get('hi')/2.0 # Half section interior height.
            self.twHalf= self.get('tw')/2.0 # Half web thickness
        
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

    def hCOG(self):
        '''Return distance from the bottom fiber of the inferior flange to the 
        centre of gravity of the section.
        '''
        return self.h()/2.0
      
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
        ''' Discretize the section into tiles.

        :param preprocessor: preprocessor of the finite element problem.
        :param matModelName: name for the new material.
        '''
        self.sectionGeometryName= 'gm'+self.get('nmb')
        self.gm= preprocessor.getMaterialHandler.newSectionGeometry(self.sectionGeometryName)
        regions= self.gm.getRegions
        for r in self.getShapeRegions():
            reg= regions.newQuadRegion(matModelName)
            reg.pMin= r[0]
            reg.pMax= r[1]
            numberOfTiles= reg.setTileSize(self.tileSize,self.tileSize)
            if(__debug__):
                if(not numberOfTiles):
                    AssertionError('Can\'t set tile size.')        
        return self.gm

    def getFiberSection3d(self,preprocessor,matModelName):
        ''' Return a FiberSection3d material.

        :param preprocessor: preprocessor of the finite element problem.
        :param matModelName: name for the new material.
        '''
        reg= self.discretization(preprocessor,matModelName)
        if(__debug__):
            if(reg is None):
                AssertionError('Can\'t create discretization.')        
        self.fiberSection3dName= 'fs3d'+self.get('nmb')
        self.fiberSection3d= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d",self.fiberSection3dName)
        fiberSectionRepr= self.fiberSection3d.getFiberSectionRepr()
        fiberSectionRepr.setGeomNamed(self.sectionGeometryName)
        self.fiberSection3d.setupFibers()
        fibers= self.fiberSection3d.getFibers()
        if(__debug__):
            if(fibers is None):
                AssertionError('Can\'t get the fibers.')        
        return self.fiberSection3d


class QHShape(SteelShape):
    '''Quadrilateral hollow shape

    :ivar bHalf: half section width.
    :ivar hHalf: half section height
    ''' 
    def __init__(self,steel, name, table):
        ''' Constructor.

        :param steel: steel object (e.g. S275JR)
        :param name: name identifying the section in the table
        :param table: module containing a dictionary with mechanical 
                      characteristics of a series of shapes.
        '''
        super(QHShape,self).__init__(steel,name,table)
        self.bHalf= self.get('b')/2.0 #Half section width
        self.hHalf= self.get('h')/2.0 #Half section height

    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(QHShape, self).__eq__(other)
                if(retval):
                    retval= (self.bHalf == other.bHalf)
                if(retval):
                    retval= (self.hHalf == other.hHalf)
            else:
                retval= True
        else:
            retval= False
        return retval
    
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

    def getWarpingConstant(self):
        ''' Return the value of the warping constant.'''
        b= self.b()-self.t()
        h= self.h()-self.t()
        t= self.t()
        retval= b**2*h**2/24.0
        retval*= (b*t-h*t)**2/(b*t+h*t)**2
        retval*= (b*t-h*t)
        return retval
    
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
    
    def getWarpingConstant(self):
        ''' Return the value of the section warping constant.'''
        return 2*self.J()/self.getOutsideDiameter()

    
class UShape(SteelShape):
    ''' Channel (or U) shaped structural steel section.'''
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

    def hCOG(self):
        '''Return distance from the bottom fiber of the inferior flange to the 
        centre of gravity of the section.
        '''
        return self.h()/2.0
      
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

