# -*- coding: utf-8 -*-

'''sectionReport.py: report describing RC cross sections mechanical properties.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import sys
import math
import os
from latex import latexUtils
from materials import materialGraphics as mg
import plotGeomSeccion as pg

fmt3_1f= '{:3.1f}'
fmt4_1f= '{:4.1f}'
fmt4_2f= '{:4.2f}'
fmt5_2f= '{:5.2f}'
fmt5_3f= '{:5.3f}'
fmt6_3f= '{:6.3f}'

class RecordFamArmaduraPrincipal(object):
  # Parámetros que se obtienen de cada familia de armaduras

  def __init__(self,reinfLayer):
    self.nmb= reinfLayer.nombre
    self.nRebars= reinfLayer.numReinfBars # Número de las barras.
    self.rebarsDiam= reinfLayer.barDiameter # Diámetro de las barras.
    self.areaRebar= reinfLayer.barArea # Area total de la familia de armaduras
    self.coverMec= reinfLayer.getRecubrimiento() # Valor mínimo del coverrimiento mecánico.
    self.cdgBarras= reinfLayer.getCdg() # Posición del centro de gravedad.

  def texWrite(self,archTex,areaHorm):
    archTex.write(self.nmb+' & '+str(self.nRebars))
    archTex.write(' & '+str(round(self.rebarsDiam*1e3)))
    archTex.write(' & '+fmt5_2f.format(self.areaRebar*1e4))
    archTex.write(' & '+fmt4_2f.format(self.areaRebar/areaHorm*1e3))
    archTex.write(' & '+fmt4_1f.format(self.coverMec*1e2))
    archTex.write(' & '+fmt5_3f.format(self.cdgBarras[0]) +' & '+fmt5_3f.format(self.cdgBarras[1]) +"\\\\\n")

def writeMainReinforcement(listaFamArmaduraPrincipal, areaHorm, archTex):
  archTex.write("\\begin{tabular}{ll}\n")
  areaArmaduraPrincipal= listaFamArmaduraPrincipal.getAreaSeccBruta()
  archTex.write("Área total $A_s="+fmt5_2f.format(areaArmaduraPrincipal*1e4) +"\\ cm^2$ & Cuantía geométrica $\\rho= "+fmt4_2f.format(areaArmaduraPrincipal/areaHorm*1e3) +"\\permil$\\\\\n")
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\hline\n")
  archTex.write("Familias de armadura principal:\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{tabular}{cccccccc}\n")
  archTex.write("Id & n. barras & $\\phi$ & área & c. geom. & cover. mec. & $y_{cdg}$ & $z_{cdg}$\\\\\n")
  archTex.write(" &  & $(mm)$ & $(cm^2)$ & $(\\permil)$ & $(cm)$ & $(m)$ & $(m)$\\\\\n")
  for f in listaFamArmaduraPrincipal:
    archTex.write("\hline\n")
    RecordFamArmaduraPrincipal(f).texWrite(archTex,areaHorm)
  archTex.write("\\end{tabular} \\\\\n")

def writeShearReinforcement(recordArmaduraCortante, archTex, width):
  archTex.write("\\hline\n")
  archTex.write(recordArmaduraCortante.familyName+' & '+str(recordArmaduraCortante.nShReinfBranches))
  areaShReinfBranchs= recordArmaduraCortante.getAs()
  diamRamas= math.sqrt(4*areaShReinfBranchs/math.pi)
  archTex.write(' & '+str(round(diamRamas*1e3)))
  archTex.write(' & '+fmt5_2f.format(areaShReinfBranchs*recordArmaduraCortante.nShReinfBranches*1e4))
  archTex.write(' & '+fmt4_1f.format(recordArmaduraCortante.shReinfSpacing*1e2))
  archTex.write(' & '+fmt5_2f.format(areaShReinfBranchs*recordArmaduraCortante.nShReinfBranches/width/recordArmaduraCortante.shReinfSpacing*1e4))
  archTex.write(' & '+fmt3_1f.format(math.degrees(recordArmaduraCortante.angAlphaShReinf)))
  archTex.write(' & '+fmt3_1f.format(math.degrees(recordArmaduraCortante.angThetaConcrStruts))+"\\\\\n")

class SectionInfo(object):
  ''' Obtains section parameters for report'''
  def __init__(self,preprocessor,section):
    self.scc= section
    self.sectionGeometryName= self.scc.nmbGeomSeccion()
    self.geomSection= preprocessor.getMaterialLoader.getSectionGeometry(self.sectionGeometryName)
    self.EHorm= self.scc.tipoHormigon.Ecm()
    self.EAcero= self.scc.tipoArmadura.Es
    self.tangHorm= self.scc.getConcreteDiagram(preprocessor).getTangent()
    self.tangSteel= self.scc.getSteelDiagram(preprocessor).getTangent()
    self.regions= self.geomSection.getRegions
    self.GB= self.geomSection.getCdgSeccBruta() # Centro de gravedad de la sección bruta.
    self.AB= self.geomSection.getAreaSeccBruta() # Área de la sección bruta.
    self.IyB= self.geomSection.getIySeccBruta() # Tensor de inercia de la sección bruta.
    self.IzB= self.geomSection.getIzSeccBruta()
    self.PyzB= self.geomSection.getPyzSeccBruta()


    self.armaduras= self.geomSection.getReinfLayers
    self.areaArmaduraPrincipal= self.armaduras.getAreaSeccBruta()
    self.coverrimiento= self.armaduras.getRecubrimiento
    self.lista_fams_armadura= []
    for f in self.armaduras:
      datosFam= RecordFamArmaduraPrincipal(f)
      self.lista_fams_armadura.append(datosFam)
 
    self.GH= self.geomSection.getCdgSeccHomogeneizada(self.tangHorm) # Centro de gravedad de la sección homogeneizada.
    self.AH= self.geomSection.getAreaSeccHomogeneizada(self.tangHorm) # Área de la sección homogeneizada.
    self.IyH= self.geomSection.getIySeccHomogeneizada(self.tangHorm) # Tensor de inercia de la sección homogeneizada.
    self.IzH=  self.geomSection.getIzSeccHomogeneizada(self.tangHorm)
    self.PyzH= self.geomSection.getPyzSeccHomogeneizada(self.tangHorm)

  def writeReport(self,archTex, pathFigura):
    if(self.geomSection):
      pg.plotGeomSeccion(self.geomSection,pathFigura)
    else:
      sys.stderr.write('error: geometry of section: '+self.sectionGeometryName+' not defined\n')
    fileHandler= open(archTex,'w')
    fileHandler.write('%% ****** Packages needed for LaTeX document: ****** \n')
    fileHandler.write('%%\usepackage{graphicx} %%\postscript includes\n')
    fileHandler.write('%%\usepackage{multirow} %%\multirow command\n')
    fileHandler.write('%%\usepackage{wasysym} %%\permil command\n')
    fileHandler.write('%%\usepackage{gensymb} %%\degree command\n')

    fileHandler.write('\\begin{table}\n')
    fileHandler.write('\\begin{center}\n')
    fileHandler.write('\\begin{tabular}{|c|}\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{large} '+latexUtils.toLaTex(self.sectionGeometryName)+' \end{large}\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write(self.scc.descSeccion+'\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{c|l}\n')
    fileHandler.write('\\begin{minipage}{85mm}\n')
    fileHandler.write('\\vspace{2mm}\n')
    fileHandler.write('\\begin{center}\n')
    #name without extension to allow pdfLatex process the file
    nameWOExt= os.path.splitext(pathFigura)[0]
    fileHandler.write('\\includegraphics[width=80mm]{'+nameWOExt+'}\n')
    fileHandler.write('\\end{center}\n')
    fileHandler.write('\\vspace{1pt}\n')
    fileHandler.write('\\end{minipage} & \n')
    fileHandler.write('\\begin{tabular}{l}\n')
    fileHandler.write('width: \\\\\n')
    fileHandler.write('$b= '+'{0:.2f}'.format(self.scc.width)+'\\ m$\\\\\n')
    fileHandler.write('depth: \\\\\n')
    fileHandler.write('$h= '+'{0:.2f}'.format(self.scc.depth)+'\\ m$\\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\textbf{Materials}:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{ll}\n')
    fileHandler.write('Concrete: '+self.scc.tipoHormigon.nmbMaterial+' & Módulo de deformación longitudinal: $E_c= '+'{0:.2f}'.format(self.EHorm/1e9)+'\\ GPa$\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('Steel: '+self.scc.tipoArmadura.nmbMaterial+' & Elastic modulus: $E_s= '+'{0:.2f}'.format(self.EAcero/1e9)+'\\ GPa$\\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\textbf{Mechanical properties}:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('Gross section:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{ll}\n')
    fileHandler.write('$A_{bruta}='+fmt6_3f.format(self.AB) +'\\ m^2$ & \\multirow{3}{*}{Tensor de inercia ($cm^4$): $ \\left( \\begin{array}{ccc}'+ fmt5_2f.format(self.scc.getJTorsion()*1e4) +' & 0.00 & 0.00 \\\\ 0.00 & '+ fmt5_2f.format(self.IyB*1e4) +' & '+fmt5_2f.format(self.PyzB) +' \\\\ 0.00 & '+fmt5_2f.format(self.PyzB) +' & '+fmt5_2f.format(self.IzB*1e4) +' \\end{array} \\right)$} \\\\\n')
    fileHandler.write('& \\\\\n')
    fileHandler.write('C.D.G.: $('+fmt5_3f.format(self.GB[0])+','+fmt5_3f.format(self.GB[1])+')\\ m$  & \\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('Homogeinized section:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{ll}\n')
    fileHandler.write('$A_{homog.}='+fmt6_3f.format(self.AH) +'\\ m^2$ & \\multirow{3}{*}{Tensor de inercia ($cm^4$): $ \\left( \\begin{array}{ccc}'+ fmt5_2f.format(self.scc.getJTorsion()*1e4) +' & 0.00 & 0.00 \\\\ 0.00 & '+ fmt5_2f.format(self.IyH*1e4) +' & '+fmt5_2f.format(self.PyzH) +' \\\\ 0.00 & '+fmt5_2f.format(self.PyzH) +' & '+fmt5_2f.format(self.IzH*1e4)+' \\end{array} \\right)$} \\\\\n')
    fileHandler.write('& \\\\\n')
    fileHandler.write('C.D.G.: $('+fmt5_3f.format(self.GH[0])+','+fmt5_3f.format(self.GH[1])+')\\ m$  & \\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\textbf{Armadura pasiva}:\\\\\n')
    fileHandler.write('\\hline\n')
    writeMainReinforcement(self.armaduras,self.AB,fileHandler)
    fileHandler.write('\\hline\n')
    fileHandler.write('Familias de armadura de cortante:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{cccccccc}\n')
    fileHandler.write('Id & n. ramas & $\\phi$ & área & sep. & area/m & $\\alpha$ & $\\beta$\\\\\n')
    fileHandler.write(' &  & $(mm)$ & $(cm^2)$ & $(cm)$ & $(cm^2/m)$ & $( \\degree)$ & $( \\degree)$\\\\\n')
    writeShearReinforcement(self.scc.armCortanteZ,fileHandler,self.scc.width)
    writeShearReinforcement(self.scc.armCortanteY,fileHandler,self.scc.depth)
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\end{tabular}\n')
    fileHandler.write('\\end{center}\n')
    fileHandler.write('\\caption{'+self.scc.descSeccion+' ('+ latexUtils.toLaTex(self.scc.nmbSeccion) +').'+'} \\label{tb_'+self.scc.nmbSeccion+'}\n')
    fileHandler.write('\\end{table}\n')
    fileHandler.close()

