# -*- coding: utf-8 -*-
''' RCFiberSectionParameters class.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import xc
from misc_utils import log_messages as lmsg
from materials.sections.fiber_section import geom_fiber_sect as gfs

class RCFiberSectionParameters(object):
    '''
    Parameters needed to create a reinforced concrete fiber section.

    :ivar concrType:       type of concrete (e.g. EHE_materials.HA25)     
    :ivar concrDiagName:   name identifying the characteristic stress-strain 
                           diagram of the concrete material
    :ivar reinfSteelType:  type of reinforcement steel
    :ivar reinfDiagName:   name identifying the characteristic stress-strain 
                           diagram of the reinforcing steel material
    :ivar nDivIJ:          number of cells in IJ (width or radial) direction
    :ivar nDivJK:          number of cells in JK (height or tangential)
                           direction
    '''
    def __init__(self, concrType, reinfSteelType, nDivIJ= 10, nDivJK= 10):
        self.concrType= concrType
        self.concrDiagName= None
        self.reinfSteelType= reinfSteelType
        self.reinfDiagName= None # Name of the uniaxial material
        self.nDivIJ= nDivIJ
        self.nDivJK= nDivJK
        
    def getDict(self):
        ''' Put member values in a dictionary.'''
        retval= dict()
        retval['concrete_type']= self.concrType
        retval['concrete_diagram_name']= self.concrDiagName
        retval['reinforcing_steel_type']= self.reinfSteelType
        retval['reinforcing_steel_diagram_name']= self.reinfDiagName
        retval['n_div_ij']= self.nDivIJ
        retval['n_div_jk']= self.nDivJK
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        self.concrType= dct['concrete_type']
        self.concrDiagName= dct['concrete_diagram_name']
        self.reinfSteelType= dct['reinforcing_steel_type']
        self.reinfDiagName= dct['reinforcing_steel_diagram_name']
        self.nDivIJ= dct['n_div_ij']
        self.nDivJK= dct['n_div_jk']

    def clear(self):
        ''' Clears all the members of this object.'''
        self.clearDiagrams()
        self.concrType= None
        self.concrDiagName= None
        self.reinfSteelType= None
        self.reinfDiagName= None
        self.nDivIJ= None
        self.nDivJK= None
            
    @classmethod
    def newFromDict(cls, dct= None):
        ''' Builds a new object from the data in the given dictionary.

        :param cls: class of the object itself.
        :param dct: dictionary contaning the data.
        '''
        newObject = cls.__new__(cls) # just object.__new__
        if(dct):
            newObject.setFromDict(dct)
        return newObject
    
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (self.concrType == other.concrType)
                if(retval):
                    retval= (self.concrDiagName == other.concrDiagName)
                if(retval):
                    retval= (self.reinfSteelType == other.reinfSteelType)
                if(retval):
                    retval= (self.reinfDiagName == other.reinfDiagName)
                if(retval):
                    retval= (self.nDivIJ == other.nDivIJ)
                if(retval):
                    retval= (self.nDivJK == other.nDivJK)
            else:
                retval= True
        else:
            retval= False
        return retval

    def nDivCirc(self):
        '''Alias for nDivIJ when defining circular sections.'''
        return self.nDivIJ
    
    def nDivRad(self):
        '''Alias for nDivJK when defining circular sections.'''
        return self.nDivJK

    def getConcreteDiagram(self, preprocessor):
        ''' Return the concrete strain-stress diagram.

        :param preprocessor: preprocessor of the finite element problem.
        '''

        return preprocessor.getMaterialHandler.getMaterial(self.concrDiagName)
      
    def getSteelDiagram(self, preprocessor):
        ''' Return the steel strain-stress diagram.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return preprocessor.getMaterialHandler.getMaterial(self.reinfDiagName)
      
    def getSteelEquivalenceCoefficient(self, preprocessor):
        ''' Return the equivalence coefficiente for the steel (Es/Ec).

        :param preprocessor: preprocessor of the finite element problem.
        '''
        tangHorm= self.getConcreteDiagram(preprocessor).getTangent()
        tangSteel= self.getSteelDiagram(preprocessor).getTangent()
        return tangSteel/tangHorm

    def defDiagrams(self, preprocessor, matDiagType):
        '''Stress-strain diagrams definition.

        :param preprocessor: preprocessor of the finite element problem.
        :param matDiagType: type of stress-strain diagram ("k" for 
                            characteristic diagram, "d" for design diagram).
        '''
        self.diagType= matDiagType
        if(self.diagType=="d"): # design diagram.
            if(self.concrType.matTagD<0):
                concreteMatTag= self.concrType.defDiagD(preprocessor)
                if(__debug__):
                    if(concreteMatTag is None):
                        AssertionError('Can\'t get concrete stress-strain diagram.')                    
            if(self.reinfSteelType.matTagD<0):
                reinfSteelMaterialTag= self.reinfSteelType.defDiagD(preprocessor)
                if(__debug__):
                    if(reinfSteelMaterialTag is None):
                        AssertionError('Can\'t get steel stress-strain diagram.')                    
            self.concrDiagName= self.concrType.nmbDiagD
            self.reinfDiagName= self.reinfSteelType.nmbDiagD
        elif(self.diagType=="k"): # characteristic diagram
            if(self.concrType.matTagK<0):
                concreteMatTag= self.concrType.defDiagK(preprocessor)
                if(__debug__):
                    if(concreteMatTag is None):
                        AssertionError('Can\'t get concrete stress-strain diagram.')                    
            if(self.reinfSteelType.matTagK<0):
                reinfSteelMaterialTag= self.reinfSteelType.defDiagK(preprocessor)
                if(__debug__):
                    if(reinfSteelMaterialTag is None):
                        AssertionError('Can\'t get steel stress-strain diagram.')                    
            self.concrDiagName= self.concrType.nmbDiagK
            self.reinfDiagName= self.reinfSteelType.nmbDiagK
        elif(self.diagType=='td'): # creep diagram.
            if(self.concrType.matTagTD<0):
                concreteMatTag= self.concrType.defDiagTD(preprocessor)
                if(__debug__):
                    if(concreteMatTag is None):
                        AssertionError('Can\'t get concrete stress-strain diagram.')                    
            if(self.reinfSteelType.matTagE<0): # linear elastic steel.
                reinfSteelMaterialTag= self.reinfSteelType.defDiagE(preprocessor)
                if(__debug__):
                    if(reinfSteelMaterialTag is None):
                        AssertionError('Can\'t get steel stress-strain diagram.')                    
            self.concrDiagName= self.concrType.nmbDiagTD
            self.reinfDiagName= self.reinfSteelType.nmbDiagE # linear elastic steel.
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; diagram type: '"+self.diagType+"' unknown.\n")
            exit(1)
            
    def clearDiagrams(self):
        '''Clear the previously defined stress-strain diagrams for the section
           materials.
        '''
        self.concrType.clearDiagrams()
        self.reinfSteelType.clearDiagrams()
        self.diagType= None
        self.concrDiagName= None
        self.reinfDiagName= None
            
    def defInteractionDiagramParameters(self, preprocessor):
        ''' Defines the parameters for interaction diagrams.

         :param preprocessor: preprocessor of the finite element problem.
        '''
        if(hasattr(self,'fiberSection')):
           if(self.fiberSection):
               self.idParams= gfs.get_interaction_diagram_parameters(self.fiberSection)
        else:
            self.idParams= xc.InteractionDiagramParameters()
            if(self.diagType=="d"):
                self.idParams.concreteTag= self.concrType.matTagD
                self.idParams.reinforcementTag= self.reinfSteelType.matTagD
            elif(self.diagType=="k"):
                self.idParams.concreteTag= self.concrType.matTagK
                self.idParams.reinforcementTag= self.reinfSteelType.matTagK
        return self.idParams
    
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        os.write(indentation+'concrete type: '+str(self.concrType.materialName)+'\n')
        os.write(indentation+'concrete stress-strain diagram: '+str(self.concrDiagName)+'\n')
        os.write(indentation+'steel type: '+str(self.reinfSteelType.materialName)+'\n')
        os.write(indentation+'steel stress-strain diagram: '+str(self.reinfDiagName)+'\n')
        os.write(indentation+'number of IJ divisions nDivIJ= '+str(self.nDivIJ)+'\n')
        os.write(indentation+'number of JK divisions nDivJK= '+str(self.nDivJK)+'\n')
