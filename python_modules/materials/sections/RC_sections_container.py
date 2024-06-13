# -*- coding: utf-8 -*-
''' Container of reinforced concrete section definitions.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
# Macros
from misc_utils import log_messages as lmsg

# TO ENHANCE: Interactions diagrams ("d" and "k") are calculated each time we 
# call the checking routines. Maybe it's a good idea to calculate them once and
# write them in a file to use them as needed.

class SectionContainer(object):
    ''' Section container.

    :ivar sections: List with the section definitions.
    :ivar mapSections: Dictionary with pairs (sectionName, reference to
                       section definition.
    :ivar mapInteractionDiagrams:  file containing a dictionary such that
                                   associates each element with the two 
                                   interactions diagrams of materials 
                                   to be used in the verification.
    '''
    def __init__(self):
        ''' Container for the reinforced concrete definitions (name, concrete
        type, rebar positions,...).

        '''
        self.sections= list() # List with the section definitions.
        self.mapSections= dict() # Dictionary with pairs (sectionName, reference to
                             # section definition.
        self.mapInteractionDiagrams= None

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= {'section_list': self.sections, 'map_sections': self.mapSections}
        if(self.mapInteractionDiagrams):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; not implemented yet for interaction diagrams.')
        return retval
    
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.sections= dct['section_list']
        for key in dct['map_sections']:
            value= dct['map_sections'][key]
            self.mapSections[key]= value
        if('mapInteractionDiagrams' in dct):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; not implemented yet for interaction diagrams.')
            
    @classmethod
    def newFromDict(cls, dct= None):
        ''' Builds a new object from the data in the given dictionary.

        :param cls: class of the object itself.
        :param dct: dictionary contaning the data.
        '''
        newObject = cls.__new__(cls) # just object.__new__
        newObject.mapSections= dict()
        if(dct):
            newObject.setFromDict(dct)
        return newObject
    
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= (self.sections == other.sections)
                if(retval):
                    retval= (self.mapSections == other.mapSections)
                if(retval):
                    if(self.mapInteractionDiagrams):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.warning(className+'.'+methodName+'; not implemented yet for interaction diagrams.')

            else:
                retval= True
        else:
            retval= False
        return retval
    
    def append(self, rcSections):
        ''' Append the argument to the container.

        :param rcSections: 
        '''
        rcSections.createSections()
        self.sections.append(rcSections)
        # Update references to individual sections.
        for ss in rcSections.lstRCSects:
            self.mapSections[ss.name]= ss

    def extend(self, other):
        ''' Add all the elements of the container argument to the calling one.

        :param other: SectionContainer object.
        '''
        self.sections.extend(other.sections)
        self.mapSections.update(other.mapSections)

    def search(self,nmb):
        ''' Return section named nmb (if founded) '''
        retval= None
        for s in self.sections:
            if(s.name==nmb):
                retval= s
        return retval

    def createRCsections(self, preprocessor, matDiagType):
        '''Creates for each element in the container the fiber sections 
        (RCsimpleSections) associated with it.
        Depending on the value of attribute 'initTensStiff' of the concrete 
        class, the method generates the concrete fibers using a constitutive 
        model without tension branch (diagram ot type concrete01) or 
        uses a concrete02 model, that initializes the material in order to
        check the cracking limit state (tension stiffening models).

        :param preprocessor: XC preprocessor for the finite element model.
        :param matDiagType: type of stress-strain diagram (="k" for characteristic diagram, ="d" for design diagram)
        '''
        for s in self.sections:
            for rcs in s.lstRCSects:
                rcs.defRCSection(preprocessor= preprocessor, matDiagType= matDiagType)


    def calcInteractionDiagrams(self, preprocessor, diagramType= 'NMyMz'):
        '''Calculates 3D interaction diagrams for each section.

        :param preprocessor:   XC preprocessor for the finite element model.
        :param diagramType:    three dimensional diagram: NMyMz
                               bi-dimensional diagram: NMy
                               bi-dimensional diagram: NMz
        '''
        self.mapInteractionDiagrams= {}
        for s in self.sections:
            for rcs in s.lstRCSects:
                diag= None
                if(diagramType=='NMyMz'):
                    diag= rcs.defInteractionDiagram(preprocessor)
                elif(diagramType=='NMy'):
                    diag= rcs.defInteractionDiagramNMy(preprocessor)
                elif(diagramType=='NMz'):
                    diag= rcs.defInteractionDiagramNMz(preprocessor)
                else:
                    lmsg.error("calcInteractionDiagrams; interaction diagram type: " + diagramType + "' unknown.")
                self.mapInteractionDiagrams[rcs.name]= diag

    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        for s in self.sections:
            s.report(os= os, indentation= indentation)
