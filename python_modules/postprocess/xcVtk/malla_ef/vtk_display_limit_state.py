# -*- coding: utf-8 -*-

'''Routines to display limit state results over the finite element mesh.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es    l.pereztato@ciccp.es"

from postprocess.xcVtk.malla_ef import vtk_grafico_ef
from postprocess.xcVtk.malla_ef import Fields

def displayFieldDirs1and2Base(limitStateLabel,argument,elementSet,genDescr,sectDescr,component,fUnitConv,fileName,captionTexts,defFScale=0.0):
  '''Display a field defined over bi-dimensional elements in its two directions.

  :param limitStateLabel: label that identifies the limit state.
  :param argument: name of the control var to represent.
  :param xcSet: represent the field over those elements.
  :param genDescr: text to identify the set (i.e 'deck' or 'west wall').
  :param sectDescr: text list to identify each direction (as ['vertical reinforcement', 'horizontal reinforcement']).
  :param component: component of the control var to represent.
  :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
  :param fileName: file name to store the image. If none -> window on screen.
  :param captionTexts: dictionary of caption texts. 
  :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)
  '''
  defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
  attributeName= limitStateLabel + 'Sect1'   #Normal stresses limit state direction 1.
  field= Fields.getScalarFieldFromControlVar(attributeName,argument,elementSet,component,fUnitConv)
  captionBaseText= captionTexts[limitStateLabel] + ', ' + captionTexts[argument] + '. '+ genDescr.capitalize()
  field.display(defDisplay,fName= fileName,caption=  captionBaseText + ', ' + sectDescr[0] )

  attributeName= limitStateLabel + 'Sect2'   #Normal stresses limit state direction 2
  field= Fields.getScalarFieldFromControlVar(attributeName,argument,elementSet,component,fUnitConv)
  field.display(defDisplay,caption= captionBaseText + ', ' + sectDescr[1],defFScale=defFScale )

def displayFieldDirs1and2(limitStateLabel,argument,elementSet,component,fUnitConv,fileName,captionTexts,defFScale=0.0):
  '''Display a field defined over bi-dimensional elements in its two directions.

  :param limitStateLabel: label that identifies the limit state.
  :param argument: name of the control var to represent.
  :param xcSet: represent the field over those elements with "genDescr" and "sectDescr" attributes.
  :param component: component of the control var to represent.
  :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
  :param fileName: file name to store the image. If none -> window on screen.
  :param captionTexts: dictionary of caption texts. 
  :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor. (Defaults to 0.0, i.e. display of 
              initial/undeformed shape)

  '''
  displayFieldDirs1and2Base(limitStateLabel,argument,elementSet,elementSet.genDescr,elementSet.sectDescr,component,fUnitConv,fileName,captionTexts,defFScale=defFScale )
