# -*- coding: utf-8 -*-
''' Wood construction materials according to
    Eurocode 5: design of timber structures.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
import math
from materials import wood_base
from materials import typical_materials
from materials.sections import section_properties
from materials.ec5 import EC5_wood_classes # Timber classes.
from misc_utils import log_messages as lmsg

ec5_wood_classes= EC5_wood_classes.ec5_wood_classes

class Wood(wood_base.Wood):
    '''Base class for wood materials according
       to EC5.

    :ivar fm_k: characteristic bending strength.
    :ivar ft_0_k: characteristic tensile strength along the grain.
    :ivar ft_90_k: characteristic tensile strength perpendicular to the grain.
    :ivar fc_0_k: characteristic compressive strength.
    :ivar fc_90_k: characteristic compressive strength perpendicular to the grain.
    :ivar fv_k: characteristic shear strength.
    :ivar E0_mean: mean value of modulus of elasticity along the grain.
    :ivar E0_05: fifth percentile value of modulus of elasticity along the grain.
    :ivar E90_mean: mean value of modulus of elasticity perpendicular to the grain.
    :ivar Gmean: mean value of shear modulus.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        specificGravity= None
        woodData= None
        if(name in ec5_wood_classes):
            woodData= ec5_wood_classes[name]
            specificGravity= woodData['ro_k']
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; unknown wood class: \''+str(name)+'\'.')
            
        super(Wood,self).__init__(name, specificGravity)
        if(woodData):
            self.fm_k= woodData['fm_k']
            self.ft_0_k= woodData['ft_0_k']
            self.ft_90_k= woodData['ft_90_k']
            self.fc_0_k= woodData['fc_0_k']
            self.fc_90_k= woodData['fc_90_k']
            self.fv_k= woodData['fv_k']
            self.E0_mean= woodData['E0_mean']
            self.E0_05= woodData['E0_05']
            self.E90_mean= woodData['E90_mean']
            self.Gmean= woodData['Gmean']

    def getPoissonRatio(self, perpendicular= False):
        ''' Return the Poisson's ratio of the wood. Its value is quite large
            due to the anisotropic material behavior.'''
        E= self.E0_mean
        if(perpendicular):
            E= self.E90_mean
        G= self.Gmean
        return E/(2.0*G)-1.0
    
    def getElasticMaterial(self, perpendicular= False):
        ''' Return an elastic material with the mechanical properties of the
            timber.

        :param perpendicular: if true use the value of modulus of elasticity perpendicular to the grain.
        '''
        nu= self.getPoissonRatio()
        E= self.E0_mean
        if(perpendicular):
            E= self.E90_mean
        return typical_materials.BasicElasticMaterial(E, nu, self.specificGravity)
    
    def getElasticMembranePlateSection(self, preprocessor, thickness:float, overridePoissonRatio= None, name:str= None, perpendicular= False):
        ''' Return an elastic membrane plate material with the mechanical
            properties of the timber.

        :param preprocessor: preprocessor of the finite element problem.
        :param thickness: overall depth of the section.
        :param overridePoissonRatio: the Poisson's ratio value is quite large
                                     due to the anisotropic material behavior.
        :param name: name for the new material.
        :param perpendicular: if true use the value of modulus of elasticity perpendicular to the grain.
        '''
        if(overridePoissonRatio):
            nu= overridePoissonRatio
        else:
            nu= self.getPoissonRatio()
        E= self.E0_mean
        if(perpendicular):
            E= self.E90_mean
        if(name==None):
            name= self.name+str(thickness)+'_elastic_membrane_plate_section'
        return typical_materials.defElasticMembranePlateSection(preprocessor, name,E,nu, rho= self.specificGravity, h= thickness)
    
    def gammaM(self):
        ''' return the partial factor for material properties and resistances
            according to table 2.3 of EC5.'''
        return 1.3

    def getCharacteristicBendingStrength(self):
        ''' return the value of the characteristic bending strength.'''
        return self.fm_k
    
    def getDesignBendingStrength(self, loadDurationClass:str, serviceClass:int):
        ''' return the value of the characteristic bending strength.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        return self.getCharacteristicBendingStrength()*self.getKmod(loadDurationClass, serviceClass)/self.gammaM()
    
    def getCharacteristicShearStrength(self):
        ''' return the value of the characteristic shear strength.'''
        return self.fv_k
    
    def getDesignShearStrength(self, loadDurationClass:str, serviceClass:int):
        ''' return the value of the characteristic shear strength.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        return self.getCharacteristicShearStrength()*self.getKmod(loadDurationClass, serviceClass)/self.gammaM()
    
    def getCharacteristicTensileStrengthAlongTheGrain(self):
        ''' return the characteristic tensile strength along the grain.'''
        return ft_0_k
    
    def getCharacteristicTensileStrengthPerpendicularToTheGrain(self):
        ''' return the characteristic tensile strength perpendicular to 
            the grain.
        '''
        return self.ft_90_k
    
    def getCharacteristicCompressiveStrength(self):
        ''' return the characteristic compressive strength.'''
        return self.fc_0_k
    
    def getDesignCompressiveStrength(self, loadDurationClass:str, serviceClass:int):
        ''' return the value of the characteristic bending strength.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        return self.getCharacteristicCompressiveStrength()*self.getKmod(loadDurationClass, serviceClass)/self.gammaM()
    
    def getCharacteristicCompressiveStrengthPerpendicularToTheGrain(self):
        ''' return the characteristic compressive strength perpendicular 
            to the grain.
        '''
        return self.fc_90_k
    
    def getDesignCompressiveStrengthPerpendicularToTheGrain(self, loadDurationClass:str, serviceClass:int):
        ''' return the value of the characteristic bending strength.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        return self.getCharacteristicCompressiveStrengthPerpendicularToTheGrain()*self.getKmod(loadDurationClass, serviceClass)/self.gammaM()
    
    def getCharacteristicShearStrength(self):
        ''' return the characteristic shear strength.'''
        return self.fv_k
    
    def getMeanValueOfModulusOfElasticityAlongTheGrain(self):
        ''' return the mean value of modulus of elasticity along the grain.'''
        return self.E0_mean
    
    def getFifthPercentileValueOfModulusOfElasticityAlongTheGrain(self):
        ''' return the fifth percentile value of modulus of elasticity 
            along the grain.
        '''
        return self.E0_05
    
    def getMeanValueOfModulusOfElasticityPerpendicularToTheGrain(self):
        ''' return the mean value of modulus of elasticity perpendicular to the grain.'''
        return self.E90_mean
    
    def getMeanValueOfShearModulus(self):
        ''' return the mean value of shear modulus.'''
        return self.Gmean 
        
    def getFifthPercentileValueOfShearModulus(self):
        ''' return the fifth percentile value of shear modulus assuming
            that G is normally distributed with a coefficient of 
            variation of about 10%.
        '''
        return 5/6.0*self.Gmean
    
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 0.5
        if(loadDurationClass=='permanent'):
            if(serviceClass==3):
                retval= 0.5
            else:
                retval= 0.6
        elif(loadDurationClass=='long_term'):
            if(serviceClass==3):
                retval= 0.55
            else:
                retval= 0.7
        elif(loadDurationClass=='medium_term'):
            if(serviceClass==3):
                retval= 0.65
            else:
                retval= 0.8
        elif(loadDurationClass=='short_term'):
            if(serviceClass==3):
                retval= 0.7
            else:
                retval= 0.9
        elif(loadDurationClass=='instantaneous'):
            if(serviceClass==3):
                retval= 0.9
            else:
                retval= 1.1
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; unknown load duration class: \''+str(loadDurationClass)+'\', values can be: permanent, long_term, medium_term, short_term or instantaneous.')
        return retval

    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.

        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 0.6
        if(serviceClass==1):
            retval= 0.6
        elif(serviceClass==2):
            retval= 0.8
        elif(serviceClass==3):
            retval= 2.0
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; unknown service class: \''+str(serviceClass)+'\', values can be: 1, 2 or 3.')
        return retval
    
    def getDepthFactor(self, h:float):
        ''' Return the depth factor according to clause 3.2 of EC5.

        :param h: depth.
        '''
        return 1.0 # Overloaded in solid timber. 

class SolidTimber(Wood):
    '''Solid timber material according to EC5.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        '''
        super(SolidTimber,self).__init__(name)
        
    def getDepthFactor(self, h:float):
        ''' Return the depth factor according to clause 3.2 of EC5.

        :param h: depth.
        '''
        retval= 1.0
        if(h<0.15 and self.specificGravity<700):
            retval= min(math.pow(0.15/h,0.2), 1.3)
        return retval

        

C14= SolidTimber('C14')
C16= SolidTimber('C16')
C18= SolidTimber('C18')
C22= SolidTimber('C22')
C24= SolidTimber('C24')
C27= SolidTimber('C27')
C30= SolidTimber('C30')
C35= SolidTimber('C35')
C40= SolidTimber('C40')

D30= SolidTimber('D30')
D35= SolidTimber('D35')
D40= SolidTimber('D40')
D50= SolidTimber('D50')
D60= SolidTimber('D60')
D70= SolidTimber('D70')

class GluedLaminatedTimber(Wood):
    '''Glued laminated timeber material according to EC5.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        '''
        super(GluedLaminatedTimber,self).__init__(name)
        
    def gammaM(self):
        ''' return the partial factor for material properties and resistances
            according to table 2.3 of EC5.'''
        return 1.25

    def getDepthFactor(self, h:float):
        ''' Return the depth factor according to clause 3.3 of EC5.

        :param h: depth.
        '''
        retval= 1.0
        if(h<0.6):
            retval= min(math.pow(0.6/h,0.1), 1.1)
        return retval

GL24h= GluedLaminatedTimber('GL24h')
GL28h= GluedLaminatedTimber('GL28h')
GL30h= GluedLaminatedTimber('GL30h')
GL32h= GluedLaminatedTimber('GL32h')
GL36h= GluedLaminatedTimber('GL36h')

GL24c= GluedLaminatedTimber('GL24c')
GL28c= GluedLaminatedTimber('GL28c')
GL32c= GluedLaminatedTimber('GL32c')
GL36c= GluedLaminatedTimber('GL36c')

        
class LVL(Wood):
    '''Laminated veneer lumber material according to EC5.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        '''
        super(LVL,self).__init__(name)
        
    def gammaM(self):
        ''' return the partial factor for material properties and resistances
            according to table 2.3 of EC5.'''
        return 1.2
    
    def getDepthFactor(self, h:float):
        ''' Return the depth factor according to clause 3.3 of EC5.

        :param h: depth.
        '''
        retval= 1.0
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval

class Plywood(Wood):
    '''Plywood material according to EC5.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        '''
        super(Plywood,self).__init__(name)
        
    def gammaM(self):
        ''' return the partial factor for material properties and resistances
            according to table 2.3 of EC5.'''
        return 1.2
        
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.

        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 2.5
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
        
class OSB(Wood):
    '''Oriented strand board material according to EC5.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        '''
        super(OSB,self).__init__(name)
        
    def gammaM(self):
        ''' return the partial factor for material properties and resistances
            according to table 2.3 of EC5.'''
        return 1.2
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.

        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 2.25
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
class ParticleBoard(Wood):
    '''Particle board material according to EC5.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        '''
        super(ParticleBoard,self).__init__(name)
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.

        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 3.0
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
class FibreBoardHard(Wood):
    '''Fibre board hard material according to EC5.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        '''
        super(FibreBoardHard,self).__init__(name)
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.

        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 3.0
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
class FibreBoardMedium(Wood):
    '''Fibre board medium material according to EC5.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        '''
        super(FibreBoardMedium,self).__init__(name)
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.

        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 4.0
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval

class FibreBoardMDF(Wood):
    '''Fibre board MDF material according to EC5.
    '''
    def __init__(self, name):
        '''Constructor.

        :param name: wood name.
        '''
        super(FibreBoardMDF,self).__init__(name)
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.

        :param loadDurationClass: duration of the load application , values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.

        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        '''      
        retval= 3.0
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
class EC5Shape(object):
    ''' Timber shape with Eurocode 5 verification routines.

    :ivar wood: wood material (i.e. C24).
    :ivar name: shape name.
    '''
    def __init__(self, wood, name= ''):
       '''Constructor.

       :param wood: wood material.
       :param name: name of the shape.
       '''
       self.name= name
       self.wood= wood
       
    def getDict(self):
        ''' Put member values in a dictionary.'''
        return {'name':self.name, 'wood':wood.getDict()}

    def EIz(self):
        ''' Return the bending stiffness around the z axis.'''
        return self.Iz()*self.wood.getMeanValueOfModulusOfElasticityAlongTheGrain()
    def EIy(self):
        ''' Return the bending stiffness around the z axis.'''
        return self.Iy()*self.wood.getMeanValueOfModulusOfElasticityAlongTheGrain()
    def getShearShapeFactor(self):
        ''' Shape factor of the section.'''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+': must be redefined in derived classes.')     
        return None
    
    def getCriticalStress(self, l_ef):
        ''' Return the critical bending stress calculated according to the
            classical theory of stability, using 5-percentile stiffness 
            values (expression 6.31 of EC5).

        :param l_ef: effective length of the beam, depending on the 
                     support conditions and the load configuration, 
                     according to Table 6.1 of EC5.
        '''
        E_05= self.wood.getFifthPercentileValueOfModulusOfElasticityAlongTheGrain()
        G_05= self.wood.getFifthPercentileValueOfShearModulus()
        Iy= self.Iy() # second moment of area about the weak axis
        Itor= self.J()  # torsional moment of inertia
        return math.pi*math.sqrt(E_05*Iy*G_05*Itor)/l_ef/self.Wzel()
    
    def getBendingRelativeSlenderness(self, l_ef):
        ''' Return the relative slenderness for bending according to the
            expresion 6.30 of the paragraph (2) of clause 6.3.3 of EC5.

        :param l_ef: effective length of the beam, depending on the 
                     support conditions and the load configuration, 
                     according to Table 6.1 of EC5.
        '''
        sg_m_crit= self.getCriticalStress(l_ef)
        fm_k= self.wood.getCharacteristicBendingStrength()
        return math.sqrt(fm_k/sg_m_crit)

    def getLateralBucklingBendingStressReductionFactor(self, l_ef):
        ''' Return the bending stress reduction factor due to lateral
            buckling according to expression 6.34 of the paragraph
            (4) of clause 6.3.3 of EC5.

        :param l_ef: effective length of the beam, depending on the 
                     support conditions and the load configuration, 
                     according to Table 6.1 of EC5.
        '''
        lambda_rel_m= self.getBendingRelativeSlenderness(l_ef)
        retval= 1
        if(lambda_rel_m>0.75):
            if(lambda_rel_m>1.4):
                retval= 1.0/lambda_rel_m**2
            else: # 0.75<lambda_rel_l<1.4
                retval= 1.56-0.75*lambda_rel_m
        return retval
        

class RectangularShape(EC5Shape, section_properties.RectangularSection):
    ''' Rectangular timber shape.'''
    
    def __init__(self,wood,name, b: float, h: float):
        ''' Constructor.

        :param wood: wood material.
        :param name: name of the shape.
        :param b: cross-section width (parallel to local z-axis)
        :param h: cross-section depth (parallel to local y-axis)
        '''
        EC5Shape.__init__(self, wood, name)
        section_properties.RectangularSection.__init__(self,name, b, h)

        
    def getDepthFactor(self):
        ''' Return the depth factor according to clause 3.2 of EC5.'''
        return self.wood.getDepthFactor(self.h)
        
    def getDesignBendingStrength(self, loadDurationClass:str, serviceClass:int, k_sys):
        ''' return the value of the characteristic bending strength.

        :param loadDurationClass: duration of the load application, values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        :param k_sys: system strength factor.
        '''
        return self.wood.getDesignBendingStrength(loadDurationClass, serviceClass)*self.getDepthFactor()*k_sys
    
    def getShearShapeFactor(self):
        ''' Shape factor of the section.'''
        return 3/2.0
    
    def getDesignShearStrength(self, loadDurationClass:str, serviceClass:int, k_v= 1.0):
        ''' return the value of the characteristic shear strength.

        :param loadDurationClass: duration of the load application, values 
               can be: permanent, long_term, medium_term, short_term 
               or instantaneous.
        :param serviceClass: service class according to clause 2.3.1.3 of EC5.
        :param k_v: notch factor.
        '''
        fv_d= self.wood.getDesignShearStrength(loadDurationClass, serviceClass)
        retval= fv_d*self.A()*k_v
        retval/=self.getShearShapeFactor()
        return retval
    
    def defElasticSection3d(self, prep):
        ''' Return an elastic section appropiate for 3D beam analysis

        :param prep: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.defElasticSection3d(self, preprocessor= prep, material= self.wood.getElasticMaterial())
   
    def defElasticShearSection3d(self, prep):
        ''' Return an elastic section appropiate for 3D beam analysis

        :param prep: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.defElasticShearSection3d(self, preprocessor= prep, material= self.wood.getElasticMaterial())
   
    def defElasticSection1d(self, prep):
        ''' Return an elastic section appropiate for 1D beam analysis

        :param prep: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.defElasticSection1d(self, preprocessor= prep, material= self.wood.getElasticMaterial())
    
    def defElasticSection2d(self, prep):
        ''' Return an elastic section appropiate for 2D beam analysis

        :param prep: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.defElasticSection2d(self, preprocessor= prep, material= self.wood.getElasticMaterial())
    
    def defElasticShearSection2d(self, prep):
        ''' Return an elastic section appropiate for 2D beam analysis

        :param prep: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.defElasticShearSection2d(self, preprocessor= prep, material= self.wood.getElasticMaterial())
