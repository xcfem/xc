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
from materials import wood_base

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
        specificGravity= 0.0
        woodData= None
        if(name in ec5_wood_classes):
            woodData= ec5_wood_classes[name]
            specificGravity= woodData['ro_k']
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

    def getCharacteristicBendingStrength(self):
        ''' return the value of the characteristic bending strength.'''
        return self.fm_k
    
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
    
    def getCharacteristicCompressiveStrengthPerpendicularToTheGrain(self):
        ''' return the characteristic compressive strength perpendicular 
            to the grain.
        '''
        return self.fc_90_k
    
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
    
    def get(self):
        ''' return the mean value of shear modulus.'''
        return self.Gmean 
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.'''
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
            of EC5.'''
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
    

class SolidTimber(Wood):
    '''Solid timber material according to EC5.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(SolidTimber,self).__init__(name, specificGravity)
        

class GluedLaminatedTimber(Wood):
    '''Glued laminated timeber material according to EC5.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(GluedLaminatedTimber,self).__init__(name, specificGravity)
        
class LVL(Wood):
    '''Laminated veneer lumber material according to EC5.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(LVL,self).__init__(name, specificGravity)

class Plywood(Wood):
    '''Plywood material according to EC5.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(Plywood,self).__init__(name, specificGravity)
        
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.'''
        retval= 2.5
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
        
class OSB(Wood):
    '''Oriented strand board material according to EC5.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(OSB,self).__init__(name, specificGravity)
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.'''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.'''
        retval= 2.25
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
class ParticleBoard(Wood):
    '''Particle board material according to EC5.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(ParticleBoard,self).__init__(name, specificGravity)
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.'''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.'''
        retval= 3.0
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
class FibreBoardHard(Wood):
    '''Fibre board hard material according to EC5.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(FibreBoardHard,self).__init__(name, specificGravity)
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.'''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.'''
        retval= 3.0
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
class FibreBoardMedium(Wood):
    '''Fibre board medium material according to EC5.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(FibreBoardMedium,self).__init__(name, specificGravity)
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.'''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.'''
        retval= 4.0
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval

class FibreBoardMDF(Wood):
    '''Fibre board MDF material according to EC5.
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(FibreBoardMDF,self).__init__(name, specificGravity)
        
    def getKmod(self, loadDurationClass:str, serviceClass:int):
        ''' Return the strength modification factor according to table 3.1
            of EC5.'''
        retval= 0.2
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
    def getKDef(self, serviceClass:int):
        ''' Return the deformation modification factor according to table 3.2
            of EC5.'''
        
        retval= 3.0
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; not implemented yet.')
        return retval
    
ec5_wood_classes= {'C14': {'fm_k': 14.00E+06, 'ft_0_k': 8.00E+06, 'ft_90_k': 400.00E+03, 'fc_0_k': 16.00E+06, 'fc_90_k': 2.00E+06, 'fv_k': 1.70E+06, 'E0_mean': 7.00E+09, 'E0_05': 4.70E+09, 'E90_mean': 230.00E+06, 'Gmean': 440.00E+06, 'ro_k': 290, 'ro_mean': 350}, 'C16': {'fm_k': 16.00E+06, 'ft_0_k': 10.00E+06, 'ft_90_k': 500.00E+03, 'fc_0_k': 17.00E+06, 'fc_90_k': 2.20E+06, 'fv_k': 1.80E+06, 'E0_mean': 8.00E+09, 'E0_05': 5.40E+09, 'E90_mean': 270.00E+06, 'Gmean': 500.00E+06, 'ro_k': 310, 'ro_mean': 370}, 'C18': {'fm_k': 18.00E+06, 'ft_0_k': 11.00E+06, 'ft_90_k': 500.00E+03, 'fc_0_k': 18.00E+06, 'fc_90_k': 2.20E+06, 'fv_k': 2.00E+06, 'E0_mean': 9.00E+09, 'E0_05': 6.00E+09, 'E90_mean': 300.00E+06, 'Gmean': 560.00E+06, 'ro_k': 320, 'ro_mean': 380}, 'C22': {'fm_k': 22.00E+06, 'ft_0_k': 13.00E+06, 'ft_90_k': 500.00E+03, 'fc_0_k': 20.00E+06, 'fc_90_k': 2.40E+06, 'fv_k': 2.40E+06, 'E0_mean': 10.00E+09, 'E0_05': 6.70E+09, 'E90_mean': 330.00E+06, 'Gmean': 630.00E+06, 'ro_k': 340, 'ro_mean': 410}, 'C24': {'fm_k': 24.00E+06, 'ft_0_k': 14.00E+06, 'ft_90_k': 500.00E+03, 'fc_0_k': 21.00E+06, 'fc_90_k': 2.50E+06, 'fv_k': 2.50E+06, 'E0_mean': 11.00E+09, 'E0_05': 7.40E+09, 'E90_mean': 370.00E+06, 'Gmean': 690.00E+06, 'ro_k': 350, 'ro_mean': 420}, 'C27': {'fm_k': 27.00E+06, 'ft_0_k': 16.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 22.00E+06, 'fc_90_k': 2.60E+06, 'fv_k': 2.80E+06, 'E0_mean': 11.50E+09, 'E0_05': 7.70E+09, 'E90_mean': 380.00E+06, 'Gmean': 720.00E+06, 'ro_k': 370, 'ro_mean': 450}, 'C30': {'fm_k': 30.00E+06, 'ft_0_k': 18.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 23.00E+06, 'fc_90_k': 2.70E+06, 'fv_k': 3.00E+06, 'E0_mean': 12.00E+09, 'E0_05': 8.00E+09, 'E90_mean': 400.00E+06, 'Gmean': 750.00E+06, 'ro_k': 380, 'ro_mean': 460}, 'C35': {'fm_k': 35.00E+06, 'ft_0_k': 21.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 25.00E+06, 'fc_90_k': 2.80E+06, 'fv_k': 3.40E+06, 'E0_mean': 13.00E+09, 'E0_05': 8.70E+09, 'E90_mean': 430.00E+06, 'Gmean': 810.00E+06, 'ro_k': 400, 'ro_mean': 480}, 'C40': {'fm_k': 40.00E+06, 'ft_0_k': 24.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 26.00E+06, 'fc_90_k': 2.90E+06, 'fv_k': 3.80E+06, 'E0_mean': 14.00E+09, 'E0_05': 9.40E+09, 'E90_mean': 470.00E+06, 'Gmean': 880.00E+06, 'ro_k': 420, 'ro_mean': 500}, 'D30':{'fm_k': 30.00E+06, 'ft_0_k': 18.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 23.00E+06, 'fc_90_k': 8.00E+06, 'fv_k': 3.00E+06, 'E0_mean': 10.00E+09, 'E0_05': 8.00E+09, 'E90_mean': 640.00E+06, 'Gmean': 600.00E+06, 'ro_k': 530, 'ro_mean': 640}, 'D35':{'fm_k': 35.00E+06, 'ft_0_k': 21.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 25.00E+06, 'fc_90_k': 8.40E+06, 'fv_k': 3.40E+06, 'E0_mean': 10.00E+09, 'E0_05': 8.70E+09, 'E90_mean': 690.00E+06, 'Gmean': 650.00E+06, 'ro_k': 560, 'ro_mean': 670}, 'D40':{'fm_k': 40.00E+06, 'ft_0_k': 24.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 26.00E+06, 'fc_90_k': 8.80E+06, 'fv_k': 3.80E+06, 'E0_mean': 11.00E+09, 'E0_05': 9.40E+09, 'E90_mean': 750.00E+06, 'Gmean': 700.00E+06, 'ro_k': 590, 'ro_mean': 700}, 'D50':{'fm_k': 50.00E+06, 'ft_0_k': 30.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 29.00E+06, 'fc_90_k': 9.70E+06, 'fv_k': 4.60E+06, 'E0_mean': 14.00E+09, 'E0_05': 11.80E+09, 'E90_mean': 930.00E+06, 'Gmean': 880.00E+06, 'ro_k': 650, 'ro_mean': 780}, 'D60':{'fm_k': 60.00E+06, 'ft_0_k': 36.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 32.00E+06, 'fc_90_k': 10.50E+06, 'fv_k': 5.30E+06, 'E0_mean': 17.00E+09, 'E0_05': 14.30E+09, 'E90_mean': 1.13E+09, 'Gmean': 1.06E+09, 'ro_k': 700, 'ro_mean': 840}, 'D70':{'fm_k': 70.00E+06, 'ft_0_k': 42.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 34.00E+06, 'fc_90_k': 13.50E+06, 'fv_k': 6.00E+06, 'E0_mean': 20.00E+09, 'E0_05': 16.80E+09, 'E90_mean': 1.33E+09, 'Gmean': 1.25E+09, 'ro_k': 900, 'ro_mean': 1080}, 'GL24h':{ 'fm_k': 24.00E+06, 'ft_0_k': 16.50E+06, 'ft_90_k': 400.00E+03, 'fc_0_k': 24.00E+06, 'fc_90_k': 2.70E+06, 'fv_k': 2.70E+06, 'E0_mean': 11.60E+09, 'E0_05': 9.40E+09, 'E90_mean': 390.00E+06, 'Gmean': 750.00E+06, 'ro_k': 380}, 'GL28h':{ 'fm_k': 28.00E+06, 'ft_0_k': 19.50E+06, 'ft_90_k': 450.00E+03, 'fc_0_k': 26.50E+06, 'fc_90_k': 3.00E+06, 'fv_k': 3.20E+06, 'E0_mean': 12.60E+09, 'E0_05': 10.20E+09, 'E90_mean': 420.00E+06, 'Gmean': 780.00E+06, 'ro_k': 410}, 'GL32h':{ 'fm_k': 32.00E+06, 'ft_0_k': 22.50E+06, 'ft_90_k': 500.00E+03, 'fc_0_k': 29.00E+06, 'fc_90_k': 3.30E+06, 'fv_k': 3.80E+06, 'E0_mean': 13.70E+09, 'E0_05': 11.10E+09, 'E90_mean': 460.00E+06, 'Gmean': 850.00E+06, 'ro_k': 430}, 'GL36h':{ 'fm_k': 36.00E+06, 'ft_0_k': 26.00E+06, 'ft_90_k': 600.00E+03, 'fc_0_k': 31.00E+06, 'fc_90_k': 3.60E+06, 'fv_k': 4.30E+06, 'E0_mean': 14.70E+09, 'E0_05': 11.90E+09, 'E90_mean': 490.00E+06, 'Gmean': 910.00E+06, 'ro_k': 450}, 'GL24c':{ 'fm_k': 24.00E+06, 'ft_0_k': 14.00E+06, 'ft_90_k': 350.00E+03, 'fc_0_k': 21.00E+06, 'fc_90_k': 2.40E+06, 'fv_k': 2.20E+06, 'E0_mean': 11.60E+09, 'E0_05': 9.40E+09, 'E90_mean': 320.00E+06, 'Gmean': 590.00E+06, 'ro_k': 350}, 'GL28c':{ 'fm_k': 28.00E+06, 'ft_0_k': 16.50E+06, 'ft_90_k': 400.00E+03, 'fc_0_k': 24.00E+06, 'fc_90_k': 2.70E+06, 'fv_k': 2.70E+06, 'E0_mean': 12.60E+09, 'E0_05': 10.20E+09, 'E90_mean': 390.00E+06, 'Gmean': 720.00E+06, 'ro_k': 380}, 'GL32c':{ 'fm_k': 32.00E+06, 'ft_0_k': 19.50E+06, 'ft_90_k': 450.00E+03, 'fc_0_k': 26.50E+06, 'fc_90_k': 3.00E+06, 'fv_k': 3.20E+06, 'E0_mean': 13.70E+09, 'E0_05': 11.10E+09, 'E90_mean': 420.00E+06, 'Gmean': 780.00E+06, 'ro_k': 410}, 'GL36c':{ 'fm_k': 36.00E+06, 'ft_0_k': 22.50E+06, 'ft_90_k': 500.00E+03, 'fc_0_k': 29.00E+06, 'fc_90_k': 3.30E+06, 'fv_k': 3.80E+06, 'E0_mean': 14.70E+09, 'E0_05': 11.90E+09, 'E90_mean': 460.00E+06, 'Gmean': 850.00E+06, 'ro_k': 430}}
