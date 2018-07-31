# -*- coding: utf-8 -*-

'''Routines to display limit state results over the finite element mesh.'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es    l.pereztato@ciccp.es"

from postprocess.xcVtk.FE_model import vtk_FE_graphic
from postprocess.xcVtk.FE_model import Fields

def displayFieldDirs1and2Base(limitStateLabel,argument,elementSet,genDescr,sectDescr,component,fUnitConv,fileName,captionTexts,defFScale=0.0,viewName='XYZPos',rgMinMax=None):
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
  :param viewName: name of the view that contains the renderer (possible
                       options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                       "ZPos", "ZNeg") (defaults to "XYZPos")
  :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
              of the scalar field (if any) to be represented. All the values 
              less than vmin are displayed in blue and those greater than vmax 
              in red (defaults to None)
  '''
  defDisplay= vtk_FE_graphic.RecordDefDisplayEF()
  defDisplay.viewName=viewName
  attributeName= limitStateLabel + 'Sect1'   #Normal stresses limit state direction 1.
  field= Fields.getScalarFieldFromControlVar(attributeName,argument,elementSet,component,fUnitConv,rgMinMax)
  captionBaseText= captionTexts[limitStateLabel] + ', ' + captionTexts[argument] + '. '+ genDescr.capitalize()
  field.display(defDisplay,fName= fileName,caption=  captionBaseText + ', ' + sectDescr[0],defFScale=defFScale)

  attributeName= limitStateLabel + 'Sect2'   #Normal stresses limit state direction 2
  field= Fields.getScalarFieldFromControlVar(attributeName,argument,elementSet,component,fUnitConv,rgMinMax)
  field.display(defDisplay,caption= captionBaseText + ', ' + sectDescr[1],defFScale=defFScale)

def displayFieldDirs1and2(limitStateLabel,argument,elementSet,component,fUnitConv,fileName,captionTexts,defFScale=0.0,viewName='XYZPos',rgMinMax=None):
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
  :param viewName: name of the view that contains the renderer (possible
                       options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                       "ZPos", "ZNeg") (defaults to "XYZPos")
  :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
              of the scalar field (if any) to be represented. All the values 
              less than vmin are displayed in blue and those greater than vmax 
              in red (defaults to None)
  
  '''
#  displayFieldDirs1and2Base(limitStateLabel,argument,elementSet,elementSet.description,elementSet.sectDescr,component,fUnitConv,fileName,captionTexts,defFScale=defFScale )  #21/07/2017 needs revision
  displayFieldDirs1and2Base(limitStateLabel,argument,elementSet,elementSet.description,['dir. 1', 'dir. 2'],component,fUnitConv,fileName,captionTexts,defFScale=defFScale,viewName= viewName,rgMinMax=rgMinMax)
