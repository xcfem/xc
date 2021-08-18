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
    '''
    def __init__(self, name, specificGravity= None):
        '''Constructor.

        :param name: wood name.
        :param specificGravity: specific gravity of the wood material.   
        '''
        super(Wood,self).__init__(name, specificGravity)
        
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
    '''Solid timeber material according to EC5.
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
