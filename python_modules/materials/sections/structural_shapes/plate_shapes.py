# -*- coding: utf-8 -*-
''' Plate structural steel shapes (metric units).'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
import math
from materials.sections import section_properties as sp


class PlateShape(sp.RectangularSection):
    ''' Properties of steel plates.

    :ivar steelType: steel object (e.g. S275JR)
    '''
    
    def __init__(self, steel, name:str, thickness:float, width:float):
        ''' Constructor.

        :param steel: steel material object (e.g. S275JR)
        :param name: name identifying the plate.
        :param thickness: plate thickness.
        :param width: plate width.
        '''
        super(PlateShape,self).__init__(name, b= thickness, h= width)
        self.steelType= steel
    
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super().__eq__(other)
                if(retval):
                    retval= (self.steelType == other.steelType)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= super().getDict()
        steelTypeClassName= str(self.steelType.__class__)[8:-2]
        retval.update({'steelTypeClassName':steelTypeClassName, 'steelType':self.steelType.getDict()})
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.'''
        super().setFromDict(dct)
        steelTypeClassName= dct['steelTypeClassName']+'()'
        self.steelType= eval(steelTypeClassName)
        self.steelType.setFromDict(dct['steelType'])

    def getThickness(self):
        ''' Return the thickness of the plate.'''
        return self.b

    def getWidth(self):
        ''' Return the thickness of the plate.'''
        return self.h
    
    def getRho(self):
        ''' Returns mass per unit length. '''
        return self.A()*self.steelType.getRho()
    
    def getSymmetry(self):
        ''' Returns the symmetry of the plate: 
            'double', 'simple' or 'none'.'''
        return 'double'
    
    def Avy(self):
        '''return shear area along y axis'''
        return super().Ay()
    
    def Avz(self):
        '''return shear area along z axis'''
        return super().Az()

    def EA(self):
        '''return cross-sectional axial stiffness.'''
        return self.steelType.E*self.A()
    
    def GAy(self):
        '''return cross-sectional shear stiffness along y axis.'''
        return self.getShearStiffnessY(G= steelType.G())
    
    def GAz(self):
        '''return cross-sectional shear stiffness along y axis.'''
        return self.getShearStiffnessY(G= steelType.G())

    def getGyrationRadiusY(self):
        '''return radius of gyration about y-axis (weak axis)'''
        return math.sqrt(self.Iy()/self.A())

    def getSlendernessY(self,Leq):
        '''return buckling slenderness relative to y-axis (weak axis)

           :param Leq: buckling length in XZ buckling plane.       
        '''
        return Leq/self.getGyrationRadiusY()

    def getGyrationRadiusZ(self):
        '''return radius of gyration about z-axis (strong axis)'''
        return math.sqrt(self.Iz()/self.A())

    def getSlendernessZ(self,Leq):
        '''return buckling slenderness relative to z-axis (strong axis)

           :param Leq: buckling length in XY buckling plane.
        '''
        return Leq/self.getGyrationRadiusZ()

    def getSlendernessRatio(self):
        ''' Return the slenderness ratio computed as the maximum dimension of
            the plate divided by two times its thickness.
        '''
        if(self.b>self.h):
            return self.b/(2*self.h)
        else:
            return self.h/(2*self.b)
        
    def J(self):
        '''return torsional moment of inertia of the section'''
        return super().J()*self.torsionalStiffnessFactor

    def It(self):
        '''Return Saint Venant torsional constant.'''
        return self.getJTorsion()

    def Iw(self):
        '''return warping constant.'''
        return self.getWarpingConstant()

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
        return self.getPlasticSectionModulusZ()*self.steelType.fy
    
    def getPlasticMomentY(self):
        ''' Return the theoretical plastic moment of the section (without
            taking into acount the class of the section) with respect to
            y-axis (weak axis).'''
        return self.getPlasticSectionModulusY()*self.steelType.fy
    
    def getElasticMomentZ(self):
        ''' Return the theoretical elastic moment of the section (without
            taking into acount the class of the section) with respect to
            z-axis (strong axis).'''
        return self.getElasticSectionModulusZ()*self.steelType.fy
    
    def getElasticMomentY(self):
        ''' Return the theoretical elastic moment of the section (without
            taking into acount the class of the section) with respect to
            y-axis (weak axis).'''
        return self.getElasticSectionModulusY()*self.steelType.fy

    def getWz(self):
        '''return section modulus with respect to z-axis (strong axis).
        '''
        if(self.sectionClass<3):
            return self.getPlasticSectionModulusZ()
        elif(self.sectionClass==3):
            return self.getElasticSectionModulusZ()
        else:
            lmsg.warning('cross sections of class: '+ str(self.sectionClass) + ' not implemented.')

    def getWy(self):
        '''return section modulus with respect to y-axis (weak axis).
        '''
        if(self.sectionClass<3):
            return self.getPlasticSectionModulusY()
        elif(self.sectionClass==3):
            return self.getElasticSectionModulusY()
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

    def getNcrY(self, Leq):
        '''return theoretical critical axial force on y-axis (weak axis).

         :param Leq: buckling length of the member in XZ buckling plane.
        '''
        return math.pi**2*self.EIy()/(Leq**2)

    def getNcrZ(self, Leq):
        '''return theoretical critical axial force on z-axis (strong axis).

         :param Leq: buckling length  of the member in XY buckling plane.
        '''
        return math.pi**2*self.EIz()/(Leq**2)

    def getNcr(self, LeqY, LeqZ):
        '''return theoretical critical axial force  (minimum of NcrY and NcrZ).

           :param LeqY: buckling length  of the member in XZ buckling plane.
           :param LeqZ: buckling length  of the member in XY buckling plane.
        '''
        return min(self.getNcrY(LeqY),self.getNcrZ(LeqZ))

    def setupULSControlVars(self, elems):
        '''For each element creates the variables
           needed to check ultimate limit state criterion to satisfy.

        :param elems: elements to define properties on.
        '''
        vc.def_vars_envelope_internal_forces_beam_elems_3d(elems)
        for e in elems:
            e.setProp('FCTNCP',[-1.0,-1.0]) # Normal stresses efficiency.
            e.setProp('FCVCP',[-1.0,-1.0]) #S hear stresses efficiency.

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
        return super().defElasticSection3d(preprocessor, self.steelType, overrideRho= overrideRho)
    
    def defElasticShearSection3d(self, preprocessor, overrideRho= None):
        '''elastic section appropriate for 3D beam analysis, including shear 
           deformations

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super().defElasticShearSection3d(preprocessor, self.steelType, overrideRho= overrideRho)

    def defElasticSection1d(self,preprocessor, overrideRho= None):
        ''' Return an elastic section appropriate for truss analysis.

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super().defElasticSection1d(preprocessor, self.steelType, overrideRho= overrideRho)
    
    def defElasticSection2d(self,preprocessor, majorAxis= True, overrideRho= None):
        ''' Return an elastic section appropriate for 2D beam analysis

        :param preprocessor: preprocessor object.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super().defElasticSection2d(preprocessor, material= self.steelType, majorAxis= majorAxis, overrideRho= overrideRho)
    
    def defElasticShearSection2d(self,preprocessor, majorAxis= True, overrideRho= None):
        '''elastic section appropriate for 2D beam analysis, including shear deformations

        :param  preprocessor: preprocessor object.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return super().defElasticShearSection2d(preprocessor, material= self.steelType, majorAxis= majorAxis, overrideRho= overrideRho)
    
    # def getCrossSectionProperties2D(self):
    #     '''Return a CrossSectionProperties object with the 2D properties of 
    #        the section.'''
    #     return super(SteelShape,self).getCrossSectionProperties2D(material= self.steelType)
    
    # def getCrossSectionProperties3D(self):
    #     '''Return a CrossSectionProperties object with the 3D properties of 
    #        the section.'''
    #     return super(SteelShape,self).getCrossSectionProperties2D(material= self.steelType)

