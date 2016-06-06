# -*- coding: utf-8 -*-

__author__= "Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, A_OO" 
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com "


class RecordLoadCaseDisp(object):
  '''Generation of graphic files and adding to report-tex files for a load case
  
  :ivar loadCaseName:   name of the load case to be depicted
  :ivar setToDisplay:   set of elements to be displayed
                   (defaults to 'total')
  :ivar unitsScale:     factor to apply to the results if we want to change
                   the units (defaults to 1).
  :ivar vectorScale:    factor to apply to the vectors length in the 
                   representation (defaults to 1).
  :ivar multByElemArea: boolean value that must be True if we want to 
                   represent the total load on each element 
                   (=load multiplied by element area) and False if we 
                   are going to depict the value of the uniform load 
                   per unit area (defaults to False)
  :ivar viewName:       name of the view  that contains the renderer (possible
                   options: "XYZPos", "XPos", "XNeg","YPos", "YNeg",
                   "ZPos", "ZNeg") (defaults to "XYZPos")
  :ivar descGrph:       description text to be displayed in the graphic
  :ivar captTex:        caption to describe the graphic content in the tex file
  :ivar lablTex:        label to be associated to the graphic in the tex file
  '''

  def __init__(self,loadCaseName):
    self.loadCaseName=loadCaseName
    self.setToDisplay=None
    self.unitsScale=1.0
    self.vectorScale=1.0
    self.multByElemArea=False
    self.viewName="XYZPos"
    self.descGrph=''
    self.captTex=''
    self.lablTex=''

  def createGraphicFile(self,gridmodl,fileName):
    '''Creates the graphic file
    
    :param gridmodl:   object of type GridModel
    :param fileName:   full name of the graphic file to be created
    '''
    gridmodl.displayLoad(self.setToDisplay,self.loadCaseName,self.unitsScale,self.vectorScale, self.multByElemArea,self.viewName,self.descGrph,fileName)
    return

  def insertGraphic(self,grfileNm,grWdt,texFile):
    '''Inserts the graphic file in the report tex file
    
    :param grfileNm:   name of the graphics file
    :param grWdt:      width of the grafic 
    :param texFile:    opened tex file in which the graphic is being inserted
    '''
    texFile.write('\\begin{center}\n')
    texFile.write('\includegraphics[width='+grWdt+']{./'+grfileNm+'}\n')
    if len(self.captTex)<>0:
      texFile.write('\caption{'+self.captTex+'}\n')
    if len(self.lablTex)<>0:
      texFile.write('\label{'+self.lablTex+'}\n')
    texFile.write('\end{center}\n')
    return
