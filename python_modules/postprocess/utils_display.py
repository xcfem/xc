# -*- coding: utf-8 -*-

import os
from xcVtk import vtk_grafico_base
from xcVtk.malla_ef import vtk_grafico_ef
from postprocess import extrapolate_elem_attr

class FigureDefinition(object):
  partLabel= None #Something like 'wall' or '2ndFloorDeck'
  verifLabel= None #Something like "Fatigue" or "CrackControl"
  attributeName= None #Magnitude to display
  fieldDescription= None #Text to insert as caption in the LaTeX file.
  unitsLabel= None # Somethin like '[MPa]' or 'radians'...
  armatureDescription= None #Something like "horizontal reinforcement."
  figSize= "90mm" #LaTeX size for the figure.

  def __init__(self,pLabel,vLabel,attrName,fDesc,aDesc=None,units=None,sz= "90mm"):
    self.partLabel= pLabel
    self.verifLabel= vLabel
    self.attributeName= attrName
    self.fieldDescription= fDesc 
    self.unitsLabel= units
    self.armatureDescription= aDesc
    self.figSize= sz


  def getCaption(self):
    retval= self.partLabel+'. '+self.fieldDescription
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

 

class TakePhotos(object):
  defDisplay= None
  elementSetName= None
  pthGraphOutput= '/tmp/'  #directorio donde colocar los gráficos
  fichLatexFigs= None #Latex file to include figures.
  fichLatexList= None #Latex file with figures list.

  def __init__(self,elemSetName):
    self.elementSetName= elemSetName
    self.elementSet= 'None'
    self.defDisplay= vtk_grafico_ef.RecordDefDisplayEF()

  def plotField(self, preprocessor, fName, attributeName):
    extrapolate_elem_attr.extrapolate_elem_function_attr(self.elementSet,attributeName,"getProp", attributeName)
    field= vtk_grafico_ef.ScalarField(attributeName,"getProp",None,1.0)
    self.defDisplay.plotScalarField(preprocessor,self.elementSetName,field,fName)

  def genGraphicFile(self,preprocessor,resRepr,nmbFichGraf):
    jpegName= nmbFichGraf+".jpeg"
    epsName= nmbFichGraf+".eps"
    self.plotField(preprocessor, jpegName, resRepr)
    os.system("convert "+ jpegName + " " + epsName)

  def insertFigureLatex(self, figDef, conta, fichFig,labelText):
    #fichLatexFigs: fichero latex donde insertar el gráfico
    #fichFig: nombre del fichero que contiene el gráfico (sin extensión y con path completo)
    figDef.insertIntoLatex(self.fichLatexFigs,self.fichLatexList,fichFig,labelText)
    if((conta>0) & (conta % 10==0)):
      self.fichLatexFigs.write('\\clearpage\n' )

  def plotFigures(self,preprocessor,figDefinitionList,nmbFichLatexFigs,nmbFichLatexList):
    #nmbLstIss: nombre de la lista donde se definen los resultados a representar
    #nmbFichLatex: nombre del fichero de látex donde montar los gráficos
    self.elementSet= preprocessor.getSets.getSet(self.elementSetName).getElements
    conta= 0
    self.fichLatexFigs= open(nmbFichLatexFigs,'w')
    self.fichLatexList= open(nmbFichLatexList,'w')
    self.fichLatexList.write('\\begin{itemize}\n' )
    for figDef in figDefinitionList:
      resRepr= figDef.attributeName  #result to display
      nmbFichGraf= self.pthGraphOutput+figDef.getFileName()
      self.genGraphicFile(preprocessor,resRepr,nmbFichGraf)
      conta+= 1
      self.insertFigureLatex(figDef,conta,nmbFichGraf,"fg_"+figDef.getFileName())
    self.fichLatexFigs.close()
    self.fichLatexList.write('\\end{itemize}\n' )
    self.fichLatexList.close()
    return
