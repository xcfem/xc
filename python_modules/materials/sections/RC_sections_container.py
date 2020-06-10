# -*- coding: utf-8 -*-
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import xc_base
import geom
import xc
# Macros
from misc_utils import log_messages as lmsg

# TO ENHANCE: Interactions diagrams ("d" and "k") are calculated each time we 
# call the checking routines. Maybe it's a good idea to calculate them once and
# write them in a file to use them as needed.

class SectionContainer(object):

    def __init__(self):
        ''' Container for the reinforced concrete definitions (name, concrete
        type, rebar positions,...).

        :ivar   mapInteractionDiagrams:  file containing a dictionary such that
                                         associates each element with the two 
                                         interactions diagrams of materials 
                                         to be used in the verification
        '''
        self.sections= [] # List with the section definitions.
        self.mapSections= {} # Dictionary with pairs (sectionName, reference to
                             # section definition.
        self.mapInteractionDiagrams= None

    def append(self, rcSections):
        rcSections.createSections()
        self.sections.append(rcSections)
        for i in range(len(rcSections.lstRCSects)):
            self.mapSections[rcSections.lstRCSects[i].sectionName]= rcSections.lstRCSects[i]

    def search(self,nmb):
        ''' Return section named nmb (if founded) '''
        retval= None
        for s in self.sections:
          if(s.name==nmb):
            retval= s
        return retval

    def createRCsections(self,preprocessor,matDiagType):
        '''Creates for each element in the container the fiber sections 
        (RCsimpleSections) associated with it.
        Depending on the value of attribute 'initTensStiff' of the concrete 
        class, the method generates the concrete fibers using a constitutive 
        model without tension branch (diagram ot type concrete01) or 
        uses a concrete02 model, that initializes the material in order to
        check the cracking limit state (tension stiffening models).

        :param matDiagType: type of stress-strain diagram (="k" for characteristic diagram, ="d" for design diagram)
        '''
        for s in self.sections:
            for i in range(len(s.lstRCSects)):
                s.lstRCSects[i].defRCSection(preprocessor,matDiagType)


    def calcInteractionDiagrams(self,preprocessor,matDiagType, diagramType= 'NMyMz'):
        '''Calculates 3D interaction diagrams for each section.

        :param preprocessor:    FEA problem preprocessor
        :param matDiagType:     'k' for characteristic, 'd' for design
        :param diagramType:    three dimensional diagram: NMyMz
                               bi-dimensional diagram: NMy
                               bi-dimensional diagram: NMz
        '''
        self.mapInteractionDiagrams= {}
        for s in self.sections:
            for i in range(len(s.lstRCSects)):
                diag= None
                if(diagramType=='NMyMz'):
                    diag= s.lstRCSects[i].defInteractionDiagram(preprocessor)
                elif(diagramType=='NMy'):
                    diag= s.lstRCSects[i].defInteractionDiagramNMy(preprocessor,matDiagType)
                elif(diagramType=='NMz'):
                    diag= s.lstRCSects[i].defInteractionDiagramNMz(preprocessor,matDiagType)
                else:
                    lmsg.error("calcInteractionDiagrams; interaction diagram type: " + diagramType + "' unknown.")
                self.mapInteractionDiagrams[s.lstRCSects[i].sectionName]= diag

