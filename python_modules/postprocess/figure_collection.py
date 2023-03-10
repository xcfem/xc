# -*- coding: utf-8 -*-

import sys
from postprocess import utils_display
from postprocess import control_vars
from misc_utils import log_messages as lmsg



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
txtIncSgS= "Bending reinforcement fatigue checking. Stress difference $\\Delta \\sigma_{sd}(Q_{fat})$ on rebars under fatigue loads (SIA 262 clause 4.3.8.2)"
txtFCSGC= "Vérification á fatigue du béton. Valeurs de: $\\frac{|\\sigma_{cd}|_{max}}{\\sigma_{c,lim}(SIA262)}$ (voir equation \\ref{eq_sigma_c_lim})"
txtFCV= "Vérification selon 4.3.8.3.2 SIA 262. Valeurs de: $\\frac{|V_d|_{max}}{V_{d,lim}(SIA262)}$ (voir equation \\ref{eq_vd_lim})"

class FigsCollectionPlotter(object):
  fieldFilesPath= 'reinforcement/results/'
  graphicOutputPath= 'post_process/results/figures/'
  latexOutputPath= 'post_process/results/'
  fUnits= "[kN/m]"
  mUnits= "[kN m/m]"
  sUnits= "[MPa]"
  reinforcementText1= "Longitudinal reinforcement"
  reinforcementText2= "Transverse reinforcement"


  def plotCommonControlVars(self, preprocessor, partName, elemSetName, figureList, outputFilesSuffix):
      ''' Plot figures showing the values of common control vars.

      :param preprocessor: preprocessor of the finite element model.
      :param partName: name of the model part.
      :param elemSetName: name of the set to display.
      :param figureList: list of figures to plot.
      :param outputFilesSuffix: suffix for the output files.
      '''
      # Load properties to display:
      fName= self.fieldFilesPath + outputFilesSuffix + ".json" # Control vars file name.
      control_vars.readControlVars(preprocessor= preprocessor, inputFileName= fName) # Read values of control vars.
      
      # Create images.
      tp= utils_display.TakePhotos(elemSetName)
      tp.pthGraphOutput= self.graphicOutputPath
      latexOuputFileName= self.latexOutputPath + partName +"_"+outputFilesSuffix+".tex"
      latexFiguresFileName= self.latexOutputPath + partName +"_"+outputFilesSuffix+"_list_figures.tex"
      tp.plotFigures(preprocessor, figureList, latexOuputFileName,latexFiguresFileName)
      
  def plotNormalStresses(self, preprocessor,partName,elemSetName):
      ''' Plot the bending strength efficiencies and associated bending moments.

      :param preprocessor: preprocessor of the finite element model.
      :param partName: name of the model part.
      :param elemSetName: name of the set to display.
      '''
      figureList= []
      figureList.append(utils_display.FigureDefinition(partName,"Flexion","MyCP1",txtMyCP1,self.reinforcementText1,self.mUnits))
      figureList.append(utils_display.FigureDefinition(partName,"Flexion","FCCP1",txtFCnormalStresses,self.reinforcementText1))
      figureList.append(utils_display.FigureDefinition(partName,"Flexion","MyCP2",txtMyCP2,self.reinforcementText2,self.mUnits))
      figureList.append(utils_display.FigureDefinition(partName,"Flexion","FCCP2",txtFCnormalStresses,self.reinforcementText2))

      # Plot control vars.
      outputFilesSuffix= "verifRsl_normStrsULS"
      self.plotCommonControlVars(preprocessor= preprocessor, partName= partName, elemSetName= elemSetName, figureList= figureList, outputFilesSuffix= outputFilesSuffix)
      
  def plotShear(self,preprocessor,partName,elemSetName):
      ''' Plot the shear strength efficiencies and associated shear forces.

      :param preprocessor: preprocessor of the finite element model.
      :param partName: name of the model part.
      :param elemSetName: name of the set to display.
      '''
      figureList= []
      eluStr= "Tranchant"
      figureList.append(utils_display.FigureDefinition(partName,eluStr,"VyCP1",txtVyCP1,self.reinforcementText1,self.fUnits))
      figureList.append(utils_display.FigureDefinition(partName,eluStr,"FCCP1",txtFCshearStresses,self.reinforcementText1))
      figureList.append(utils_display.FigureDefinition(partName,eluStr,"VyCP2",txtVyCP2,self.reinforcementText2,self.fUnits))
      figureList.append(utils_display.FigureDefinition(partName,eluStr,"FCCP2",txtFCshearStresses,self.reinforcementText2))
      # Plot control vars.
      outputFilesSuffix= "verifRsl_shearULS"
      self.plotCommonControlVars(preprocessor= preprocessor, partName= partName, elemSetName= elemSetName, figureList= figureList, outputFilesSuffix= outputFilesSuffix)
      
  def plotCrackingControlVars(self, preprocessor, partName, elemSetName, slsStr):
      ''' Plot the cracking efficiencies and associated rebar stresses.

      :param preprocessor: preprocessor of the finite element model.
      :param partName: name of the model part.
      :param elemSetName: name of the set to display.
      :param slsStr: 'characteristic' or 'frequent' or 'quasi-permanent'
      '''
      figureList= []
      if(slsStr=='characteristic'):
          slsId= "FissurationRare"
          slsPrefix= 'verifRsl_crackingSLS_rare'
      elif(slsStr=='frequent'):
          slsId= "FissurationFreq"
          slsPrefix= 'verifRsl_crackingSLS_freq'
      elif(slsStr=='quasi-permanent'):
          slsId= "FissurationQP"
          slsPrefix= 'verifRsl_crackingSLS_qp'
      else:
          className= type(self).__name__
          methodName= sys._getframe(0).f_code.co_name
          lmsg.error(className+'.'+methodName+'; slsStr '+str(slsStr) + "unknown. Must be 'characteristic' or 'frequent' or 'quasi-permanent'")
      figureList.append(utils_display.FigureDefinition(partName, slsId,"sg_s1",txtSGSFreq,self.reinforcementText1,self.sUnits))
      figureList.append(utils_display.FigureDefinition(partName, slsId,"sg_s2",txtSGSFreq,self.reinforcementText2,self.sUnits))
      #Load properties to display:
      fName= self.fieldFilesPath + slsPrefix + ".json"
      control_vars.readControlVars(preprocessor= preprocessor, inputFileName= fName)
      elemSet= preprocessor.getSets.getSet(elemSetName).elements
      for e in elemSet:
          sgPos1= e.getProp("sg_sPos1")
          sgNeg1= e.getProp("sg_sNeg1")
          e.setProp("sg_s1",max(sgPos1,sgNeg1))
          sgPos2= e.getProp("sg_sPos2")
          sgNeg2= e.getProp("sg_sNeg2")
          e.setProp("sg_s2",max(sgPos2,sgNeg2))

      tp= utils_display.TakePhotos(elemSetName)
      tp.pthGraphOutput= self.graphicOutputPath
      latexOutputFileName= self.latexOutputPath + partName +"_"+slsPrefix+".tex"
      latexFiguresFileName= self.latexOutputPath + partName +"_"+slsPrefix+"_list_figures.tex"
      tp.plotFigures(preprocessor, figureList, latexOutputFileName, latexFiguresFileName)
      
  def plotFissurationFreq(self,preprocessor,partName,elemSetName):
      ''' Plot the cracking efficiencies and associated rebar stresses.

      :param preprocessor: preprocessor of the finite element model.
      :param partName: name of the model part.
      :param elemSetName: name of the set to display.
      '''
      self.plotCrackingControlVars(preprocessor= preprocessor, partName= partName, elemSetName= elemSetName, slsStr= 'characteristic')

  def plotFissurationQP(self,preprocessor,partName,elemSetName):
      ''' Plot the cracking efficiencies and associated rebar stresses.

      :param preprocessor: preprocessor of the finite element model.
      :param partName: name of the model part.
      :param elemSetName: name of the set to display.
      '''
      self.plotCrackingControlVars(preprocessor= preprocessor, partName= partName, elemSetName= elemSetName, slsStr= 'quasi-permanent')

  def plotFatigue(self,preprocessor,partName,elemSetName):
      figureList= []
      eluStr= "Fatigue"
      figureList.append(utils_display.FigureDefinition(partName,eluStr,"inc_sg_s1",txtIncSgS,self.reinforcementText1,self.sUnits))
      figureList.append(utils_display.FigureDefinition(partName,eluStr,"fc_sg_c1",txtFCSGC,self.reinforcementText1))
      figureList.append(utils_display.FigureDefinition(partName,eluStr,"fc_v1",txtFCV,self.reinforcementText1))

      figureList.append(utils_display.FigureDefinition(partName,eluStr,"inc_sg_s2",txtIncSgS,self.reinforcementText2,self.sUnits))
      figureList.append(utils_display.FigureDefinition(partName,eluStr,"fc_sg_c2",txtFCSGC,self.reinforcementText2))
      figureList.append(utils_display.FigureDefinition(partName,eluStr,"fc_v2",txtFCV,self.reinforcementText2))


      #Load properties to display:
      fName= self.fieldFilesPath + "verifRsl_fatigueULS.json"
      control_vars.readControlVars(preprocessor= preprocessor, inputFileName= fName)

      elemSet= preprocessor.getSets.getSet(elemSetName).elements
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
