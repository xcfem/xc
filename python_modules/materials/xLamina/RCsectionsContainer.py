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
from solution import predefined_solutions
from postprocess import PhantomModel as phm

# TO ENHANCE: Interactions diagrams ("d" and "k") are calculated each time we call
# the check routines. Maybe it's a good idea to calculate them once and write them
# in a file to use them as needed.

class SectionContainer(object):

  def __init__(self):
    ''' Container for the reinforced concrete definitions (name, concrete
        type, rebar positions,...).'''
    self.sections= [] # List with the section definitions.
    self.mapSections= {} # Dictionary with pairs (sectionName, reference to
                         # section definition.

  def append(self, RCSections):
    self.sections.append(RCSections)
    for i in range(len(RCSections.lstRCSects)):
      self.mapSections[RCSections.lstRCSects[i].sectionName]= RCSections.lstRCSects[i]
    return
  
  def search(self,nmb):
    ''' Returnrs section named nmb (if founded) '''
    retval= None
    for s in self.sections:
      if(s.name==nmb):
        retval= s
    return retval


  def getInteractionDiagrams(self,preprocessor,matDiagType):
    '''Returns map of 3D interaction diagrams.

    :param preprocessor:    FEA problem preprocessor
    :param matDiagType:     'k' for characteristic, 'd' for design
    '''
    mapInteractionDiagrams= {}
    for s in self.sections:
      for i in range(len(s.lstRCSects)):
        s.lstRCSects[i].defRCSimpleSection(preprocessor,matDiagType)
        diag= s.lstRCSects[i].defInteractionDiagram(preprocessor)
        mapInteractionDiagrams[s.lstRCSects[i].sectionName]= diag
    return mapInteractionDiagrams

  def getInteractionDiagramsNMy(self,preprocessor,matDiagType):
    '''Returns map of 2D interaction diagrams in N-My plane.

    :param preprocessor:    FEA problem preprocessor
    :param matDiagType:     'k' for characteristic, 'd' for design
    '''
    mapInteractionDiagrams= {}
    for s in self.sections:
      for i in range(len(s.lstRCSects)):
        diag= s.lstRCSects[i].defInteractionDiagramNMy(preprocessor,matDiagType)
        diag.simplify() #Hasta corregir la obtención de diagramas NMy
        print "area diag= ", diag.getArea()
        mapInteractionDiagrams[s.lstRCSects[i].sectionName]= diag
    return mapInteractionDiagrams




