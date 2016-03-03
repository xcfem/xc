# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
from miscUtils import LogMessages as lmsg
from xcVtk import vtk_grafico_base
from xcVtk.malla_ef import vtk_grafico_ef
from xcVtk.malla_ef import Fields
from xcVtk import vtk_grafico_diagrama_esfuerzos as gde


class FigureBase(object):
  def __init__(self,pLabel,vLabel,figDescr,reinfDescr=None,units=None,sz= "90mm"):
    self.partLabel= pLabel #Something like 'wall' or '2ndFloorDeck'
    self.verifLabel= vLabel #Something like "Fatigue" or "CrackControl"
    self.attributeName= ''
    self.figDescription= figDescr #Text to insert as caption in the LaTeX file.
    self.unitsLabel= units # Somethin like '[MPa]' or 'radians'...
    self.armatureDescription= reinfDescr #Something like "horizontal reinforcement."
    self.figSize= sz #LaTeX size for the figure.
    self.viewName= "XYZPos"
  def getCaption(self):
    retval= self.partLabel+'. '+self.figDescription
    if(self.unitsLabel!=None):
      retval+= ' '+ self.unitsLabel
    if(self.armatureDescription!=None):
      retval+= '. ' + self.armatureDescription
    return retval
  def getFileName(self):
    return self.partLabel+self.verifLabel+self.attributeName
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
  def __init__(self,pLabel,vLabel,figDescr,reinfDescr=None,units=None,sz= "90mm"):
    super(SlideDefinition,self).__init__(pLabel,vLabel,figDescr,reinfDescr,units,sz)
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
      defDisplay.displayMesh(preprocessor,elementSetName,None,self.diagrams,jpegName)
    os.system("convert "+ jpegName + " " + epsName)
  

class FigureDefinition(SlideDefinition):
  diagrams= None #List of diagrams to display (see ColoredDiagram, LinearLoadDiagram,...)

  def __init__(self,pLabel,vLabel,attrName,figDescr,reinfDescr=None,units=None,sz= "90mm"):
    super(FigureDefinition,self).__init__(pLabel,vLabel,figDescr,reinfDescr,units,sz)
    self.attributeName= attrName
    lmsg.warning('FigureDefinition DEPRECATED; use SlideDefinition.')

  def defField(self, elementSetName):
    self.field= Fields.ExtrapolatedScalarField(self.attributeName,"getProp",None,1.0,elementSetName)

  def genGraphicFile(self,preprocessor,defDisplay, elementSetName, nmbFichGraf):
    jpegName= nmbFichGraf+".jpeg"
    epsName= nmbFichGraf+".eps"
    self.defField(elementSetName)
    self.field.plot(preprocessor, defDisplay,jpegName)
    os.system("convert "+ jpegName + " " + epsName)
 

class TakePhotos(object):

  def __init__(self,elemSetName):
    self.defDisplay= None
    self.elementSetName= elemSetName
    self.defDisplay= vtk_grafico_ef.RecordDefDisplayEF()
    self.pthGraphOutput= '/tmp/'  #Directory to put the graphics in.
    self.fichLatexFigs= None #Latex file to include figures.
    self.fichLatexList= None #Latex file with figures list.

  def insertFigureLatex(self, figDef, conta, fichFig,labelText):
    #fichLatexFigs: fichero latex donde insertar el gráfico
    #fichFig: nombre del fichero que contiene el gráfico (sin extensión y con path completo)
    figDef.insertIntoLatex(self.fichLatexFigs,self.fichLatexList,fichFig,labelText)
    if((conta>0) & (conta % 10==0)):
      self.fichLatexFigs.write('\\clearpage\n' )

  def displayFigures(self,preprocessor,figDefinitionList,nmbFichLatexFigs,nmbFichLatexList):
    '''Creates graphics files from figure definition list.'''
    #nmbLstIss: nombre de la lista donde se definen los resultados a representar
    #nmbFichLatex: nombre del fichero de látex donde montar los gráficos
    conta= 0
    self.fichLatexFigs= open(nmbFichLatexFigs,'w')
    self.fichLatexList= open(nmbFichLatexList,'w')
    self.fichLatexList.write('\\begin{itemize}\n' )
    for figDef in figDefinitionList:
      nmbFichGraf= self.pthGraphOutput+figDef.getFileName()
      figDef.genGraphicFile(preprocessor,self.defDisplay, self.elementSetName,nmbFichGraf)
      conta+= 1
      self.insertFigureLatex(figDef,conta,nmbFichGraf,"fg_"+figDef.getFileName())
    self.fichLatexFigs.close()
    self.fichLatexList.write('\\end{itemize}\n' )
    self.fichLatexList.close()
    return

  def plotFigures(self,preprocessor,figDefinitionList,nmbFichLatexFigs,nmbFichLatexList):
    lmsg.warning('plotFigures DEPRECATED; use displayFigures.')
    self.displayFigures(preprocessor,figDefinitionList,nmbFichLatexFigs,nmbFichLatexList)

