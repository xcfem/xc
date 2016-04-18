# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
from miscUtils import LogMessages as lmsg
from miscUtils import string_utils as su
from xcVtk import vtk_grafico_base
from xcVtk.malla_ef import vtk_grafico_ef
from xcVtk.malla_ef import Fields
from xcVtk import vtk_grafico_diagrama_esfuerzos as gde


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
    #fichFig: nombre del fichero que contiene el gráfico (sin extensión y con path completo)
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
      d.agregaDiagrama()

  def genGraphicFile(self,preprocessor,defDisplay, elementSetName, nmbFichGraf):
    jpegName= nmbFichGraf+".jpeg"
    epsName= nmbFichGraf+".eps"
    self.setupDiagrams()
    defDisplay.viewName= self.viewName
    if(self.field):
      self.field.plot(preprocessor, defDisplay,fName)
    else:
      defDisplay.displayMesh(preprocessor,elementSetName,None,self.diagrams,jpegName,self.getCaption())
    os.system("convert "+ jpegName + " " + epsName)
  

class FigureDefinition(SlideDefinition):
  diagrams= None #List of diagrams to display (see ColoredDiagram, LinearLoadDiagram,...)

  def __init__(self,pLabel,limitStateLabel,attrName,figDescr,reinfDescr=None,units=None,sz= "90mm"):
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
    #lmsg.warning('FigureDefinition DEPRECATED; use SlideDefinition.')

  def defField(self, elementSetName):
    print '********** Enters FigureDefinition::defField; limit state: ', self.limitStateLabel, ' attributeName= ', self.attributeName, ' elementSetName= ', elementSetName 
    self.field= Fields.ExtrapolatedScalarField(self.attributeName,"getProp",None,1.0,elementSetName)
    print '********** Exits FigureDefinition::defField; limit state: ', self.limitStateLabel, ' attributeName= ', self.attributeName, ' elementSetName= ', elementSetName 

  def genGraphicFile(self,preprocessor,defDisplay, elementSetName, nmbFichGraf):
    jpegName= nmbFichGraf+".jpeg"
    epsName= nmbFichGraf+".eps"
    self.defField(elementSetName)
    self.field.plot(preprocessor, defDisplay,jpegName,self.getCaption())
    os.system("convert "+ jpegName + " " + epsName)
 
class TakePhotos(object):
  '''Generation of bitmaps with analysis and design results.'''
  def __init__(self,elemSetName):
    self.defDisplay= None
    self.elementSetName= elemSetName
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

  def displayFigures(self,preprocessor,figDefinitionList,LatexFigsFilename,LatexListFilename):
    '''Creates graphics files from figure definition list.'''
    #nmbLstIss: nombre de la lista donde se definen los resultados a representar
    #LatexFilename: nombre del fichero de látex donde montar los gráficos
    conta= 0
    self.fichLatexFigs= open(self.pthTextOutput+LatexFigsFilename,'w')
    self.fichLatexList= open(self.pthTextOutput+LatexListFilename,'w')
    self.fichLatexList.write('\\begin{itemize}\n' )
    for figDef in figDefinitionList:
      bitmapFilename= self.pthGraphOutput+figDef.getFileName()
      figDef.genGraphicFile(preprocessor,self.defDisplay, self.elementSetName,bitmapFilename)
      conta+= 1
      self.insertFigureLatex(figDef,conta,bitmapFilename,"fg_"+figDef.getFileName())
    self.fichLatexFigs.close()
    self.fichLatexList.write('\\end{itemize}\n' )
    self.fichLatexList.close()
    return

  def plotFigures(self,preprocessor,figDefinitionList,LatexFigsFilename,LatexListFilename):
    lmsg.warning('plotFigures DEPRECATED; use displayFigures.')
    self.displayFigures(preprocessor,figDefinitionList,LatexFigsFilename,LatexListFilename)

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
    self.elementSetName= self.getShortName()+'_elementSet'
    elems= self.getElements()
    # Definimos el conjunto
    st= preprocessor.getSets.defSet(self.elementSetName)
    for e in elems:
      st.getElements.append(e)
    st.fillDownwards()
    return st
  def display(self,preprocessor,tp,resultsToDisplay):
    '''Generate an image for every result to display
       resultToDisplay: collection of results to be displayed.
    '''
    elementSet= self.getElementSet(preprocessor)
    tp.elementSetName= self.elementSetName
    resultsToDisplay.display(preprocessor,tp,self)
       

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
    execfile(fName)
    for k in self.keys():
      part= self[k]
      part.display(preprocessor,tp,resultsToDisplay)

