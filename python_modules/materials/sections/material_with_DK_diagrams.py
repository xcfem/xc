# -*- coding: utf-8 -*-

'''MaterialWithDKDiagrams.py: materials with characteristic (K) and design (D) diagrams.'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import sys
from misc_utils import log_messages as lmsg

class MaterialWithDKDiagrams(object):
    """Base class for materials with characteristic (K) and design (D) diagrams 

      :ivar materialName: name of the material.

      :ivar nmbDiagK: name of the characteristic diagram.

      :ivar nmbDiagD: name of the design diagram.

      :ivar nmbDiagE: name of the elastic diagram.
    """
    def __init__(self, matName):
        ''' Constructor.

        :param matName: material name.
        '''
        self.setupName(matName)

    def getDict(self):
        ''' Return a dictionary with the values of the object members.'''
        retval= {'material_name':self.materialName}
        return retval

    def setFromDict(self, dct):
        ''' Set the member values from those in the given dictionary.'''
        matName= dct['material_name']
        self.setupName(matName)
        
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
                retval= (self.materialName==other.materialName)
            else:
                retval= True
        else:
            retval= False
        return retval
    
    def setupName(self, matName):
        ''' Material setup.

        :param matName: material name.
        '''
        self.materialName= matName # Name identifying the material.
        if(matName):
            # Characteristic stress-strain diagram.
            self.nmbDiagK= "dgK_"+self.materialName # Name identifying the characteristic stress-strain diagram.
            # Design stress-strain diagram.
            self.nmbDiagD= "dgD_"+self.materialName # Name identifying the design stress-strain diagram.
            # TD stress-strain diagram.
            self.nmbDiagTD= "dgTD_"+self.materialName # Name identifying the TD stress-strain diagram.
            # Linear elastic stress-strain diagram.
            self.nmbDiagE= "dgE_"+self.materialName # Name identifying the linear elastic stress-strain diagram.

    def __repr__(self):
        return self.materialName

    def getDiagK(self, preprocessor):
        ''' Return the uniaxila material corresponding to the characteristic 
            stress-strain diagram of this material.

        :param preprocessor: finite element preprocessor.
        '''
        return preprocessor.getMaterialHandler.getMaterial(self.nmbDiagK)
    
    def diagKExists(self, preprocessor):
        ''' Return true if the  uniaxial material corresponding to the 
            characteristic stress-strain diagram of this material is already
            create.

        :param preprocessor: pre-processor of the FE problem at hand.
        '''
        return preprocessor.getMaterialHandler.materialExists(self.nmbDiagK)

    def getMaterialDiagramK(self):
        ''' Return the uniaxial material corresponding to the characteristic 
            stress-strain diagram of this object.'''
        retval= None
        if(hasattr(self,'preprocessor')):
            if(self.preprocessor):
                retval= self.getDiagK(self.preprocessor)
        return retval
            
    def getMatTagK(self, preprocessor= None):
        ''' Return the identifier of the uniaxial material corresponding to
            the characteristic stress-strain diagram.

        :param preprocessor: finite element preprocessor. If none assume the
                             preprocessor is always the same.
        '''
        retval= -1
        if(preprocessor):
            tmp= self.getDiagK(preprocessor)
        else:
            tmp= self.getMaterialDiagramK()
        if(tmp):
            retval= tmp.tag
        return retval
           
    def getDiagD(self, preprocessor):
        ''' Return the uniaxila material corresponding to the design 
            stress-strain diagram of this material.

        :param preprocessor: finite element preprocessor.
        '''
        return preprocessor.getMaterialHandler.getMaterial(self.nmbDiagD)

    def diagDExists(self, preprocessor):
        ''' Return true if the  uniaxial material corresponding to the design
            stress-strain diagram of this material is already create.

        :param preprocessor: pre-processor of the FE problem at hand.
        '''
        return preprocessor.getMaterialHandler.materialExists(self.nmbDiagD)

    def getMaterialDiagramD(self):
        ''' Return the uniaxial material corresponding to the design
            stress-strain diagram of this object.'''
        retval= None
        if(hasattr(self,'preprocessor')):
            if(self.preprocessor):
                retval= self.getDiagD(self.preprocessor)
        return retval
    
    def getMatTagD(self, preprocessor= None):
        ''' Return the identifier of the uniaxial material corresponding to
            the design stress-strain diagram.

        :param preprocessor: finite element preprocessor. If none assume the
                             preprocessor is always the same.
        '''
        retval= -1
        if(preprocessor):
            tmp= self.getDiagD(preprocessor)
        else:
            tmp= self.getMaterialDiagramD()
        if(tmp):
            retval= tmp.tag
        return retval
       
    def getDiagTD(self, preprocessor):
        ''' Return the uniaxila material corresponding to the TD stress-strain
            diagram of this material.

        :param preprocessor: finite element preprocessor.
        '''
        return preprocessor.getMaterialHandler.getMaterial(self.nmbDiagTD)
    
    def diagTDExists(self, preprocessor):
        ''' Return true if the  uniaxial material corresponding to the 
            characteristic stress-strain diagram of this material is already
            create.

        :param preprocessor: pre-processor of the FE problem at hand.
        '''
        return preprocessor.getMaterialHandler.materialExists(self.nmbDiagTD)
    
    def getMaterialDiagramTD(self):
        ''' Return the uniaxial material corresponding to the TD stress-strain
            diagram of this object.'''
        retval= None
        if(hasattr(self,'preprocessor')):
            if(self.preprocessor):
                retval= self.getDiagTD(self.preprocessor)
        return retval
    
    def getMatTagTD(self, preprocessor= None):
        ''' Return the identifier of the uniaxial material corresponding to
            the TD stress-strain diagram.

        :param preprocessor: finite element preprocessor. If none assume the
                             preprocessor is always the same.
        '''
        retval= -1
        if(preprocessor):
            tmp= self.getDiagTD(preprocessor)
        else:
            tmp= self.getMaterialDiagramTD()
        if(tmp):
            retval= tmp.tag
        return retval
       
    def getDiagE(self, preprocessor):
        ''' Return the uniaxila material corresponding to the linear elastic
            stress-strain diagram of this material.

        :param preprocessor: finite element preprocessor.
        '''
        return preprocessor.getMaterialHandler.getMaterial(self.nmbDiagE)
    
    def diagEExists(self, preprocessor):
        ''' Return true if the  uniaxial material corresponding to the 
            characteristic stress-strain diagram of this material is already
            create.

        :param preprocessor: pre-processor of the FE problem at hand.
        '''
        return preprocessor.getMaterialHandler.materialExists(self.nmbDiagE)
    
    def getMaterialDiagramE(self):
        ''' Return the uniaxial material corresponding to the linear elastic
            stress-strain diagram of this object.'''
        retval= None
        if(hasattr(self,'preprocessor')):
            if(self.preprocessor):
                retval= self.getDiagE(self.preprocessor)
        return retval
    
    def getMatTagE(self, preprocessor= None):
        ''' Return the identifier of the uniaxial material corresponding to
            the linear elastic stress-strain diagram.

        :param preprocessor: finite element preprocessor. If none assume the
                             preprocessor is always the same.
        '''
        retval= -1
        if(preprocessor):
            tmp= self.getDiagE(preprocessor)
        else:
            tmp= self.getMaterialDiagramE()
        if(tmp):
            retval= tmp.tag
        return retval

    def _set_preprocessor(self, preprocessor):
        ''' Store the preprocessor that has been used to create the uniaxial
            material corresponding to a stress-strain diagram of this material.

        :param preprocessor: pre-processor for the finite element problem.
        '''
        # Check if this diagram has already been created with another
        # preprocessor.
        if(hasattr(self,'preprocessor')):
            if(id(self.preprocessor)!=id(preprocessor)):
                # # Issue an warning message.
                # className= type(self).__name__
                # methodName= sys._getframe(0).f_code.co_name
                # warningMsg= "; material: '"+str(self.nmbDiagK)
                # warningMsg+= "' already exists but does not"
                # warningMsg+= ' belong to the given preprocessor.'
                # lmsg.warning(className+'.'+methodName+warningMsg)
                self.preprocessor= preprocessor
            else:
                pass # No need of doing anything.
        else:
            self.preprocessor= preprocessor
           
    def getDesignStressStrainDiagramFileName(self, path= '', withExtension= True):
        ''' Return the file name for the design stress strain diagram.

        :param path: path for the file.
        :param withExtension: if true append the file extension.
        '''
        retval= path
        if(len(retval)>0):
            if(retval[-1]!= '/'):
                retval+= '/'
        retval+= self.materialName+'_design_stress_strain_diagram'
        if(withExtension):
            retval+= '.png'
        return retval

    def getDesignStressStrainDiagramTitle(self):
        ''' Return the title for the design stress strain diagram.'''
        return self.materialName + ' design stress-strain diagram'
