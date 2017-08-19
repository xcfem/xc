# -*- coding: utf-8 -*-
from postprocess import utils_display



txtFCnormalStresses= "Facteur de capacité (contraintes normales) des éléments sous charges durables (ELUT2*)"
txtMyCP1= "Efforts de flexion (contraintes normales) déterminants."
txtMyCP2= "Efforts de flexion (contraintes normales) déterminants."

#Shear
txtFCshearStresses= "Facteur de capacité vis-à-vis de l'effort tranchant (clause 4.3.3 SIA 262)"
txtVyCP1= "Valeurs de calcul de l'effort tranchant."
txtVyCP2= "Valeurs de calcul de l'effort tranchant."

#Fissuration
txtSGSQP= "Fissuration. Enveloppe de contraintes maximales sous charges quasi-permanentes"
txtSGSFreq= "Fissuration. Enveloppe de contraintes maximales sous charges fréquentes"

#Fatigue
txtIncSgS= "Vérification á fatigue de l'armature de flexion. Différence de contrainte $\\Delta \\sigma_{sd}(Q_{fat})$ dans l'armature sous actions de fatigue (SIA 262 chiffre 4.3.8.2)"
txtFCSGC= "Vérification á fatigue du béton. Valeurs de: $\\frac{|\\sigma_{cd}|_{max}}{\\sigma_{c,lim}(SIA262)}$ (voir equation \\ref{eq_sigma_c_lim})"
txtFCV= "Vérification selon 4.3.8.3.2 SIA 262. Valeurs de: $\\frac{|V_d|_{max}}{V_{d,lim}(SIA262)}$ (voir equation \\ref{eq_vd_lim})"

class FigsCollectionPlotter(object):
  fieldFilesPath= 'armatures/results/'
  graphicOutputPath= 'post_process/results/figures/'
  latexOutputPath= 'post_process/results/'
  fUnits= "[kN/m]"
  mUnits= "[kN m/m]"
  sUnits= "[MPa]"
  txtArmature1= "Armature en dir. long."
  txtArmature2= "Armature en dir. trsv."

  def plotNormalStresses(self,preprocessor,partName,elemSetName):
    figureList= []
    figureList.append(utils_display.FigureDefinition(partName,"Flexion","MyCP1",txtMyCP1,self.txtArmature1,self.mUnits))
    figureList.append(utils_display.FigureDefinition(partName,"Flexion","FCCP1",txtFCnormalStresses,self.txtArmature1))
    figureList.append(utils_display.FigureDefinition(partName,"Flexion","MyCP2",txtMyCP2,self.txtArmature2,self.mUnits))
    figureList.append(utils_display.FigureDefinition(partName,"Flexion","FCCP2",txtFCnormalStresses,self.txtArmature2))
    #Load properties to display:
    fName= self.fieldFilesPath + "verifRsl_normStrsULS.py"
    execfile(fName)

    tp= utils_display.TakePhotos(elemSetName)
    tp.pthGraphOutput= self.graphicOutputPath
    tp.plotFigures(preprocessor,figureList,self.latexOutputPath+partName+"_verifRsl_normStrsULS.tex",self.latexOutputPath+partName+"_verifRsl_normStrsULS_list_figures.tex")


  def plotShear(self,preprocessor,partName,elemSetName):
    figureList= []
    eluStr= "Tranchant"
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"VyCP1",txtVyCP1,self.txtArmature1,self.fUnits))
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"FCCP1",txtFCshearStresses,self.txtArmature1))
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"VyCP2",txtVyCP2,self.txtArmature2,self.fUnits))
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"FCCP2",txtFCshearStresses,self.txtArmature2))
    #Load properties to display:
    fName= self.fieldFilesPath + "verifRsl_shearULS.py"
    execfile(fName)

    tp= utils_display.TakePhotos(elemSetName)
    tp.pthGraphOutput= self.graphicOutputPath
    tp.plotFigures(preprocessor,figureList,self.latexOutputPath+partName+"_verifRsl_shearULS.tex",self.latexOutputPath+partName+"_verifRsl_shearULS_list_figures.tex")


  def plotFissurationFreq(self,preprocessor,partName,elemSetName):
    figureList= []
    eluStr= "FissurationFreq"
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"sg_s1",txtSGSFreq,self.txtArmature1,self.sUnits))
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"sg_s2",txtSGSFreq,self.txtArmature2,self.sUnits))
    #Load properties to display:
    fName= self.fieldFilesPath + "verifRsl_crackingSLS_freq.py"
    execfile(fName)
    elemSet= preprocessor.getSets.getSet(elemSetName).getElements
    for e in elemSet:
      sgPos1= e.getProp("sg_sPos1")
      sgNeg1= e.getProp("sg_sNeg1")
      e.setProp("sg_s1",max(sgPos1,sgNeg1))
      sgPos2= e.getProp("sg_sPos2")
      sgNeg2= e.getProp("sg_sNeg2")
      e.setProp("sg_s2",max(sgPos2,sgNeg2))

    tp= utils_display.TakePhotos(elemSetName)
    tp.pthGraphOutput= self.graphicOutputPath
    tp.plotFigures(preprocessor,figureList,self.latexOutputPath+partName+"_verifRsl_crackingSLS_freq.tex",self.latexOutputPath+partName+"_verifRsl_crackingSLS_freq_list_figures.tex")


  def plotFissurationQP(self,preprocessor,partName,elemSetName):
    figureList= []
    eluStr= "FissurationQP"

    figureList.append(utils_display.FigureDefinition(partName,eluStr,"sg_s1",txtSGSQP,self.txtArmature1,self.sUnits))
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"sg_s2",txtSGSQP,self.txtArmature2,self.sUnits))
    #Load properties to display:
    fName= self.fieldFilesPath + "verifRsl_crackingSLS_qperm.py"
    execfile(fName)
    elemSet= preprocessor.getSets.getSet(elemSetName).getElements
    for e in elemSet:
      sgPos1= e.getProp("sg_sPos1")
      sgNeg1= e.getProp("sg_sNeg1")
      e.setProp("sg_s1",max(sgPos1,sgNeg1))
      sgPos2= e.getProp("sg_sPos2")
      sgNeg2= e.getProp("sg_sNeg2")
      e.setProp("sg_s2",max(sgPos2,sgNeg2))

    tp= utils_display.TakePhotos(elemSetName)
    tp.pthGraphOutput= self.graphicOutputPath
    tp.plotFigures(preprocessor,figureList,self.latexOutputPath+partName+"_verifRsl_crackingSLS_qperm.tex",self.latexOutputPath+partName+"_verifRsl_crackingSLS_qperm_list_figures.tex")



  def plotFatigue(self,preprocessor,partName,elemSetName):
    figureList= []
    eluStr= "Fatigue"
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"inc_sg_s1",txtIncSgS,self.txtArmature1,self.sUnits))
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"fc_sg_c1",txtFCSGC,self.txtArmature1))
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"fc_v1",txtFCV,self.txtArmature1))

    figureList.append(utils_display.FigureDefinition(partName,eluStr,"inc_sg_s2",txtIncSgS,self.txtArmature2,self.sUnits))
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"fc_sg_c2",txtFCSGC,self.txtArmature2))
    figureList.append(utils_display.FigureDefinition(partName,eluStr,"fc_v2",txtFCV,self.txtArmature2))


    #Load properties to display:
    fName= self.fieldFilesPath + "verifRsl_fatigueULS.py"
    execfile(fName)

    elemSet= preprocessor.getSets.getSet(elemSetName).getElements
    for e in elemSet:
      Vy11= e.getProp("Vy11")
      Vy01= e.getProp("Vy01")
      e.setProp("incV1",Vy11-Vy01)
      Vy12= e.getProp("Vy12")
      Vy02= e.getProp("Vy02")
      e.setProp("incV2",Vy12-Vy02)


    tp= utils_display.TakePhotos(elemSetName)
    tp.pthGraphOutput= self.graphicOutputPath
    tp.plotFigures(preprocessor,figureList,self.latexOutputPath+partName+"_verifRsl_fatigueULS.tex",self.latexOutputPath+partName+"_verifRsl_fatigueULS_list_figures.tex")
