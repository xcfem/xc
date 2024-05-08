# -*- coding: utf-8 -*-

'''MaterialWithDKDiagrams.py: materials with characteristic (K) and design (D) diagrams.'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

class MaterialWithDKDiagrams(object):
    """Base class for materials with characteristic (K) and design (D) diagrams 

      :ivar materialName: name of the material.

      :ivar nmbDiagK: name of the characteristic diagram.
      :ivar matTagK:  tag of the uniaxial material in the characteristic diagram.
      :ivar materialDiagramK: characteristic stress-strain diagram.

      :ivar nmbDiagD: name of the design diagram.
      :ivar matTagD: tag of the uniaxial material in the design diagram.
      :ivar materialDiagramD: design stress-strain diagram.

      :ivar nmbDiagE: name of the elastic diagram.
      :ivar matTagE: tag of the uniaxial material in the elastic diagram.
      :ivar materialDiagramE: elastic stress-strain diagram.
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
    def newFromDict(cls, dct):
        ''' Builds a new object from the data in the given dictionary.

        :param cls: class of the object itself.
        :param dct: dictionary contaning the data.
        '''
        newObject = cls.__new__(cls) # just object.__new__
        newObject.setFromDict(dct)
        return newObject

    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(self is not other):
            retval= (self.materialName==other.materialName)
        else:
            retval= True
        return retval
    
    def setupName(self, matName):
        ''' Material setup.

        :param matName: material name.
        '''
        self.materialName= matName # Name identifying the material.
        if(matName):
            # Characteristic stress-strain diagram.
            self.nmbDiagK= "dgK"+self.materialName # Name identifying the characteristic stress-strain diagram.
            self.matTagK= -1 # Tag of the uniaxial material with the characteristic stress-strain diagram.
            self.materialDiagramK= None # Characteristic stress-strain diagram.

            # Design stress-strain diagram.
            self.nmbDiagD= "dgD"+self.materialName # Name identifying the design stress-strain diagram.
            self.matTagD= -1 # Tag of the uniaxial material with the design stress-strain diagram .
            self.materialDiagramD= None # Design stress-strain diagram.

            # Linear elastic stress-strain diagram.
            self.nmbDiagE= "dgE"+self.materialName # Name identifying the linear elastic stress-strain diagram.
            self.matTagE= -1 # Tag of the uniaxial material with the design stress-strain diagram .
            self.materialDiagramE= None # Design stress-strain diagram.

    def __repr__(self):
        return self.materialName

    def getDiagK(self,preprocessor):
        return preprocessor.getMaterialHandler.getMaterial(self.nmbDiagK)
    
    def getDiagD(self,preprocessor):
        return preprocessor.getMaterialHandler.getMaterial(self.nmbDiagD)

