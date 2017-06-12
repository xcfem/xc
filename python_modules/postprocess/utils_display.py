# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import os
from miscUtils import LogMessages as lmsg
from miscUtils import string_utils as su
from postprocess.xcVtk import vtk_grafico_base
from postprocess.xcVtk.malla_ef import vtk_grafico_ef
from postprocess.xcVtk.malla_ef import Fields
#from postprocess.xcVtk import vtkInternalForceDiagram as ifd
from postprocess.control_vars import *

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import MultipleLocator
from mpl_toolkits.axes_grid1 import make_axes_locatable


class setToDisplay(object):
  '''Defines the description of a set of elements to be used in the graphics
  and reports associated with the calculation

  :ivar elSet:    set of elements
  :ivar genDescr: general description
  :ivar sectDescr:ordered list with the descriptions that apply to each of the 
                  sections that configures the element.
  '''
  def __init__(self,elSet,genDescr='',sectDescr=[]):
    self.elSet=elSet
    self.elSet.fillDownwards()
    self.genDescr=genDescr
    self.sectDescr=sectDescr

class FigureBase(object):
  def __init__(self,pLabel,limitStateLabel,figDescr,reinfDescr=None,units=None,sz= "90mm"):
    ''' Figure base constructor.
    pLabel: part label; something like 'wall' or '2ndFloorDeck'
    limitStateLabel; limit state check label; Something like "Fatigue" or "CrackControl"
    figDescr: figure description; text to insert as caption in the figure file and int the LaTeX file.
    units: units displayed; something like '[MPa]' or 'radians'...
    reinfDescr: reinforcement description; sSomething like "horizontal reinforcement."
    sz: LaTeX size for the figure.
    '''
    self.partLabel= pLabel #Something like 'wall' or '2ndFloorDeck'
    self.limitStateLabel= limitStateLabel #Something like "Fatigue" or "CrackControl"
    self.attributeName= ''
    self.figDescription= figDescr #Text to insert as caption in the LaTeX file.
    self.unitsLabel= units # Somethin like '[MPa]' or 'radians'...
    self.armatureDescription= reinfDescr #Something like "horizontal reinforcement."
    self.figSize= sz #LaTeX size for the figure.
    self.viewName= "XYZPos"
  def getCaption(self):
    retval= self.partLabel+'. '+self.figDescription
    if(self.unitsLabel!=None):
      retval+= ' ['+ self.unitsLabel +']'
    if(self.armatureDescription!=None):
      retval+= '. ' + self.armatureDescription
    return retval
  def getFileName(self):
    return su.slugify(self.partLabel+self.limitStateLabel+self.attributeName)
  def insertIntoLatex(self, fichLatexFigs, fichLatexList, fichFig, labelText):
    #fichLatexFigs: fichero latex donde insertar el gráfico
    #fichFig: nombre del fichero que contiene el gráfico (sin extensión y sin path)
    fichLatexFigs.write('\\begin{figure}\n' )
    fichLatexFigs.write('\\centering\n' )
    fichLatexFigs.write('\\includegraphics[width=' + self.figSize + ']{' + fichFig + '}\n' )
    fichLatexFigs.write('\\caption{'+ self.getCaption() + '} ' )
    fichLatexFigs.write('\\label{'+ labelText + '}\n' )
    fichLatexFigs.write('\\end{figure}\n \n')
    fichLatexList.write('\\item Figure \\ref{' )
    fichLatexList.write(labelText)
    fichLatexList.write('}: ' + self.getCaption() + '\n' )

class SlideDefinition(FigureBase):
  def __init__(self,pLabel,limitStateLabel,figDescr,reinfDescr=None,units=None,sz= "90mm"):
    ''' Slide constructor.
    pLabel: part label; something like 'wall' or '2ndFloorDeck'
    limitStateLabel; limit state check label; Something like "Fatigue" or "CrackControl"
    figDescr: figure description; text to insert as caption in the figure file and int the LaTeX file.
    units: units displayed; something like '[MPa]' or 'radians'...
    reinfDescr: reinforcement description; sSomething like "horizontal reinforcement."
    sz: LaTeX size for the figure.
    '''
    super(SlideDefinition,self).__init__(pLabel,limitStateLabel,figDescr,reinfDescr,units,sz)
    self.field= None
    self.diagrams= list()

  def setupDiagrams(self):
    for d in self.diagrams:
      d.addDiagram()

  def genGraphicFile(self,preprocessor,defDisplay, xcSet, nmbFichGraf):
    jpegName= nmbFichGraf+".jpeg"
    epsName= nmbFichGraf+".eps"
    self.setupDiagrams()
    defDisplay.viewName= self.viewName
    if(self.field):
      self.field.plot(preprocessor, defDisplay,fName)
    else:
      defDisplay.displayMesh(xcSet,None,self.diagrams,jpegName,self.getCaption())
    os.system("convert "+ jpegName + " " + epsName)
  

class FigureDefinition(SlideDefinition):
  diagrams= None #List of diagrams to display (see ColoredDiagram, LinearLoadDiagram,...)

  def __init__(self,pLabel,limitStateLabel,attrName,argument,figDescr,reinfDescr=None,units=None,sz= "90mm"):
    ''' Figure constructor.
    pLabel: part label as defined in model; something like 'wall' or '2ndFloorDeck'
    limitStateLabel; limit state check label; Something like "Fatigue" or "CrackControl"
    figDescr: figure description; text to insert as caption in the figure file and int the LaTeX file.
    units: units displayed; something like '[MPa]' or 'radians'...
    reinfDescr: reinforcement description; sSomething like "horizontal reinforcement."
    sz: LaTeX size for the figure.
    '''
    super(FigureDefinition,self).__init__(pLabel,limitStateLabel,figDescr,reinfDescr,units,sz)
    self.attributeName= attrName
    self.argument= argument

  def defField(self, xcSet):
    print '********** Enters FigureDefinition::defField; limit state: ', self.limitStateLabel, ' attributeName= ', self.attributeName, ' xcSet.name= ', xcSet.name
    #self.field= Fields.ExtrapolatedScalarField(self.attributeName,"getProp",None,1.0,xcSet)
    self.field= Fields.getScalarFieldFromControlVar(self.attributeName,self.argument,xcSet,None,1.0)
    print '********** Exits FigureDefinition::defField; limit state: ', self.limitStateLabel, ' attributeName= ', self.attributeName, ' xcSet.name= ', xcSet.name

  def genGraphicFile(self,defDisplay, xcSet, nmbFichGraf):
    jpegName= nmbFichGraf+".jpeg"
    epsName= nmbFichGraf+".eps"
    self.defField(xcSet)
    self.field.plot(defDisplay,jpegName,self.getCaption())
    os.system("convert "+ jpegName + " " + epsName)
 
class TakePhotos(object):
  '''Generation of bitmaps with analysis and design results.'''
  def __init__(self,xcSet):
    self.defDisplay= None
    self.xcSet= xcSet
    self.defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
    self.pthGraphOutput= '/tmp/'  #Directory to put the graphics in.
    self.pthTextOutput= '/tmp/'  #Directory to put the texts in.
    self.fichLatexFigs= None #Latex file to include figures.
    self.fichLatexList= None #Latex file with figures list.

  def insertFigureLatex(self, figDef, conta, fichFig,labelText):
    #fichLatexFigs: fichero latex donde insertar el gráfico
    #fichFig: nombre del fichero que contiene el gráfico (sin extensión y con path completo)
    figDef.insertIntoLatex(self.fichLatexFigs,self.fichLatexList,fichFig,labelText)
    if((conta>0) & (conta % 10==0)):
      self.fichLatexFigs.write('\\clearpage\n' )

  def displayFigures(self,figDefinitionList,LatexFigsFilename,LatexListFilename):
    '''Creates graphics files from figure definition list.'''
    #nmbLstIss: nombre de la lista donde se definen los resultados a representar
    #LatexFilename: nombre del fichero de látex donde montar los gráficos
    conta= 0
    self.fichLatexFigs= open(self.pthTextOutput+LatexFigsFilename,'w')
    self.fichLatexList= open(self.pthTextOutput+LatexListFilename,'w')
    self.fichLatexList.write('\\begin{itemize}\n' )
    for figDef in figDefinitionList:
      bitmapFilename= self.pthGraphOutput+figDef.getFileName()
      figDef.genGraphicFile(self.defDisplay, self.xcSet,bitmapFilename)
      conta+= 1
      self.insertFigureLatex(figDef,conta,bitmapFilename,"fg_"+figDef.getFileName())
    self.fichLatexFigs.close()
    self.fichLatexList.write('\\end{itemize}\n' )
    self.fichLatexList.close()
    return

  def plotFigures(self,preprocessor,figDefinitionList,LatexFigsFilename,LatexListFilename):
    lmsg.warning('plotFigures DEPRECATED; use displayFigures.')
    self.displayFigures(figDefinitionList,LatexFigsFilename,LatexListFilename)

class PartToDisplay(object):
  def __init__(self,partName, surfaceList,reinforcementLabels):
    ''' PartsToDisplay constructor
    partName: name assigned to the part to show.
    surfaceList: list of surfaces to show (with the names used in the model).
    reinforcementLabels: labels to identify the rebars (longitudinal reinf, vertical reinf,...).    
    '''
    self.partName= partName
    self.surfaceList= surfaceList
    self.reinforcementLabels= reinforcementLabels
  def getShortName(self):
    return su.slugify(self.partName)
  def getElements(self):
    '''Returns a list of the elements of this part.'''
    retval= list()
    for sup in self.surfaceList:
      elSup= sup.getElements()
      for elem in elSup:
        retval.append(elem)
    return retval
  def getElementSet(self,preprocessor):
    elementSetName= self.getShortName()+'_elementSet'
    elems= self.getElements()
    # Definimos el conjunto
    self.xcSet= preprocessor.getSets.defSet(elementSetName)
    for e in elems:
      self.xcSet.getElements.append(e)
    self.xcSet.fillDownwards()
    return self.xcSet
  def display(self,preprocessor,tp,resultsToDisplay):
    '''Generate an image for every result to display
       resultToDisplay: collection of results to be displayed.
    '''
    elementSet= self.getElementSet(preprocessor)
    tp.elementSetName= elementSet.name
    resultsToDisplay.display(tp,self)
       

class PartToDisplayContainer(dict):
  ''' Parts to display in figures... '''
  def __init__(self,lst):
    for l in lst:
      self.add(l)
  def add(self,part):
    self[part.getShortName()]= part
  def display(self,preprocessor,tp,resultsToDisplay):
    '''Display results for each part.
       resultToDisplay: collection of results to be displayed.
    '''
    #Load properties to display:
    fName= resultsToDisplay.limitStateData.getOutputDataFileName()
    print '******* calling: ', fName
    execfile(fName) #Load data to display.
    for k in self.keys():
      part= self[k]
      part.display(preprocessor,tp,resultsToDisplay)

def plotStressStrainFibSet(fiberSet,title,fileName=None,nContours=100):
  '''Represents graphically the cross-section current stresses and strains.
  The graphics are generated by a triangulation from the x,y coordinates of
  the fibers.

  :param fiberSet: set of fibers to be represented
  :param title:    general title for the graphic
  :param fileName: name of the graphic file (defaults to None: no file generated)
  :param nContours: number of contours to be generated (defaults to 100)
  '''
  lsYcoo=list()
  lsZcoo=list()
  lsStrain=list()
  lsStress=list()
  for f in fiberSet:
    fpos=f.getPos()
    lsYcoo.append(fpos.x)
    lsZcoo.append(fpos.y)
    lsStrain.append(f.getStrain())
    lsStress.append(f.getForce()/f.getArea()/1e6)
    
  fig,(ax1,ax2) = plt.subplots(1,2, sharex=True, sharey=True)
  fig.suptitle(title, fontsize=20)
  ax1.set_title('Strain')
  im1=ax1.tricontourf(lsYcoo,lsZcoo,lsStrain, nContours)
  divider1 = make_axes_locatable(ax1)
  cax1 = divider1.append_axes("right", size="20%", pad=0.05)
  cbar1 = plt.colorbar(im1,cax=cax1)
  ax2.set_title('Stress')
  im2=ax2.tricontourf(lsYcoo,lsZcoo,lsStress, nContours)
  divider2 = make_axes_locatable(ax2)
  cax2 = divider2.append_axes("right", size="20%", pad=0.05)
  cbar2 = plt.colorbar(im2,cax=cax2)
  if fileName<>None:
    plt.savefig(fileName)
  plt.show()
  return
