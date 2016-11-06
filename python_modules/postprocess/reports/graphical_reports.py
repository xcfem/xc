# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O" 
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega@ciccp.es "

from xcVtk.malla_ef import vtk_grafico_ef
from xcVtk.malla_ef import Fields

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
    grFileNameNoPath=grfileNm.split('/')[grfileNm.count('/')]
    texFile.write('\\begin{center}\n')
    texFile.write('\includegraphics[width='+grWdt+']{'+grFileNameNoPath+'}\n')
    if len(self.captTex)<>0:
      texFile.write('\caption{'+self.captTex+'}\n')
    if len(self.lablTex)<>0:
      texFile.write('\label{'+self.lablTex+'}\n')
    texFile.write('\end{center}\n')
    return

def checksReports(limitStateLabel,setsToReport,argsToReport,capTexts,pathGr,texReportFile):
    report=open(texReportFile,'w')    #report latex file
    dfDisp= vtk_grafico_ef.RecordDefDisplayEF()
    for st in setsToReport:
        for arg in argsToReport:
            attributeName= limitStateLabel + 'Dir1'
            field= Fields.getScalarFieldFromControlVar(attributeName,arg,st.elSet,None,1.0)
            capt=capTexts[limitStateLabel] + ', ' + capTexts[arg] + '. '+ st.genDescr.capitalize() + ', ' + st.sectDescr[0]
            grFileNm=pathGr+st.elSet.name+arg+'Dir1'
            field.display(defDisplay=dfDisp,caption=capt,fName=grFileNm+'.jpg')
            report.write('\\begin{figure}[h]\n')
            report.write('\\begin{center}\n')
            report.write('\\includegraphics[width=120mm]{'+grFileNm+'}\n')
            report.write('\\caption{'+capt+'}\n')
            report.write('\\end{center}\n')
            report.write('\\end{figure}\n')

            attributeName= limitStateLabel + 'Dir2'
            field= Fields.getScalarFieldFromControlVar(attributeName,arg,st.elSet,None,1.0)
            capt=capTexts[limitStateLabel] + ', ' + capTexts[arg] + '. '+ st.genDescr.capitalize() + ', ' + st.sectDescr[1]
            grFileNm=pathGr+st.elSet.name+arg+'Dir2'
            field.display(defDisplay=dfDisp,caption=capt,fName=grFileNm+'.jpg')
            report.write('\\begin{figure}[h]\n')
            report.write('\\begin{center}\n')
            report.write('\\includegraphics[width=120mm]{'+grFileNm+'}\n')
            report.write('\\caption{'+capt+'}\n')
            report.write('\\end{center}\n')
            report.write('\\end{figure}\n')
    report.close()
    return
