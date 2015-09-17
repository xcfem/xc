# -*- coding: utf-8 -*-

import plotGeomSeccion as pg
import sys
from latex import latexUtils

class RecordFamArmaduraPrincipal(object):
  # Parámetros que se obtienen de cada familia de armaduras
  nmb=  None # Código que identifica a la familia de armaduras.
  nBarras=  0 # Número de las barras.
  diamBarras=  0.0 # Diámetro de las barras.
  areaBarras=  0.0 # Area total de la familia de armaduras
  recubMec=  0.0 # Valor mínimo del recubrimiento mecánico.
  cdgBarras=  None # Posición del centro de gravedad.

  def __init__(self,reinfLayer):
    self.nmb= reinfLayer.nombre
    self.nBarras= reinfLayer.numReinfBars # Número de las barras.
    self.diamBarras= reinfLayer.barDiameter # Diámetro de las barras.
    self.areaBarras= reinfLayer.barArea # Area total de la familia de armaduras
    self.recubMec= reinfLayer.getRecubrimiento() # Valor mínimo del recubrimiento mecánico.
    self.cdgBarras= reinfLayer.getCdg # Posición del centro de gravedad.

  def texWrite(self,archTex):
    archTex.write(self.nmb," & ",self.nBarras)
    archTex.write(" & ",'%u'.format(round(self.diamBarras*1e3)))
    archTex.write(" & ",'%5.2f'.format(self.areaBarras*1e4))
    archTex.write(" & ",'%4.2f'.format(self.areaBarras/areaHorm*1e3))
    archTex.write(" & ",'%4.1f'.format(self.recubMec*1e2))
    archTex.write(" & ","%5.3f".format(self.cdgBarras.x) ," & ","%5.3f".format(self.cdgBarras.y) ,"\\\\\n")

def writeMainReinforcement(listaFamArmaduraPrincipal, areaHorm, archTex):
  archTex.write("\\begin{tabular}{ll}\n")
  archTex.write("Área total $A_s=","%5.2f".format(areaArmaduraPrincipal*1e4) ,"\\ cm^2$ & Cuantía geométrica $\\rho= ","%4.2f".format(areaArmaduraPrincipal/areaHorm*1e3) ,"\\permil$\\\\\n")
  archTex.write("\\end{tabular} \\\\\n")
  archTex.write("\\hline\n")
  archTex.write("Familias de armadura principal:\\\\\n")
  archTex.write("\\hline\n")
  archTex.write("\\begin{tabular}{cccccccc}\n")
  archTex.write("Id & n. barras & $\\phi$ & área & c. geom. & recub. mec. & $y_{cdg}$ & $z_{cdg}$\\\\\n")
  archTex.write(" &  & $(mm)$ & $(cm^2)$ & $(\\permil)$ & $(cm)$ & $(m)$ & $(m)$\\\\\n")
  for f in listaFamArmaduraPrincipal:
    archTex.write("\hline\n")
    f.write(archTex)
  archTex.write("\\end{tabular} \\\\\n")

def writeShearReinforcement(recordArmaduraCortante, archTex):
  archTex.write("\\hline\n")
  archTex.write(nmbFamilia," & ",recordArmaduraCortante.nRamas)
  diamRamas= sqrt(4*areaRamas/PI)
  archTex.write(" & ",'%u'.format(round(recordArmaduraCortante.diamRamas*1e3)))
  archTex.write(" & ",'%5.2f'.format(recordArmaduraCortante.areaRamas*recordArmaduraCortante.nRamas*1e4))
  archTex.write(" & ",'%4.1f'.format(recordArmaduraCortante.espaciamientoRamas*1e2))
  archTex.write(" & ",'%5.2f'.format(recordArmaduraCortante.areaRamas*nRamas/ancho/recordArmaduraCortante.espaciamientoRamas*1e4))
  archTex.write(" & ",'%3.1f'.format(rad2deg(recordArmaduraCortante.angAlphaRamas)))
  archTex.write(" & ",'%3.1f'.format(rad2deg(recordArmaduraCortante.angThetaBielas)),"\\\\\n")

class SectionInfo(object):
  ''' Obtains section parameters for report'''
  def __init__(self,mdlr,section):
    self.scc= section
    self.sectionGeometryName= self.scc.nmbGeomSeccion()
    self.geomSection= mdlr.getMaterialLoader.getSectionGeometry(self.sectionGeometryName)
    self.EHorm= self.scc.tipoHormigon.Ecm
    self.EAcero= self.scc.tipoArmadura.Es
    self.tangHorm= self.scc.getConcreteDiagram(mdlr).getTangent()
    self.regions= self.geomSection.getRegions
    self.GB= self.regions.getCdgSeccBruta() # Centro de gravedad de la sección bruta.
    self.AB= self.regions.getAreaSeccBruta() # Área de la sección bruta.
    self.IyB= self.regions.getIySeccBruta() # Tensor de inercia de la sección bruta.
    self.IzB= self.regions.getIzSeccBruta()
    self.PyzB= self.regions.getPyzSeccBruta()


    self.armaduras= self.geomSection.getReinfLayers
    self.areaArmaduraPrincipal= self.armaduras.getAreaSeccBruta
    self.recubrimiento= self.armaduras.getRecubrimiento
    self.lista_fams_armadura= []
    for f in self.armaduras:
      datosFam= RecordFamArmaduraPrincipal(f)
      self.lista_fams_armadura.append(datosFam)
 
    self.GH= self.regions.getCdgSeccHomogeneizada(self.tangHorm) # Centro de gravedad de la sección homogeneizada.
    self.AH= self.regions.getAreaSeccHomogeneizada(self.tangHorm) # Área de la sección homogeneizada.
    self.IyH= self.regions.getIySeccHomogeneizada(self.tangHorm) # Tensor de inercia de la sección homogeneizada.
    self.IzH=  self.regions.getIzSeccHomogeneizada(self.tangHorm)
    self.PyzH= self.regions.getPyzSeccHomogeneizada(self.tangHorm)

  def writeReport(self,archTex, pathFigura):
    if(self.geomSection):
      pg.plotGeomSeccion(self.geomSection,pathFigura)
    else:
      sys.stderr.write('error: geometry of section: '+self.sectionGeometryName+' not defined\n')
    fileHandler= open(archTex,'w')
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
    fileHandler.write('\\includegraphics[width=80mm]{'+pathFigura+'}\n')
    fileHandler.write('\\end{center}\n')
    fileHandler.write('\\vspace{1pt}\n')
    fileHandler.write('\\end{minipage} & \n')
    fileHandler.write('\\begin{tabular}{l}\n')
    fileHandler.write('ancho: \\\\\n')
    fileHandler.write('$b= '+'{0:.2f}'.format(self.scc.ancho)+'\\ m$\\\\\n')
    fileHandler.write('canto: \\\\\n')
    fileHandler.write('$h= '+'{0:.2f}'.format(self.scc.canto)+'\\ m$\\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\textbf{Materiales}:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{ll}\n')
    fileHandler.write('Hormigón: '+self.scc.tipoHormigon.nmbMaterial+' & Módulo de deformación longitudinal: $E_c= '+'{0:.2f}'.format(self.scc.tipoHormigon.Ecm()/1e9)+'\\ GPa$\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('Acero: '+self.scc.tipoArmadura.nmbMaterial+' & Módulo elástico: $E_s= '+'{0:.2f}'.format(self.scc.tipoArmadura.Es/1e9)+'\\ GPa$\\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\textbf{Valores estáticos}:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('Sección bruta:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{ll}\n')
    fileHandler.write('$A_{bruta}='+'%6.3f'.format(self.AB) +'\\ m^2$ & \\multirow{3}{*}{Tensor de inercia ($cm^4$): $ \\left( \\begin{array}{ccc}'+ '%5.2f'.format(self.JTorsion*1e4) +' & 0.00 & 0.00 \\\\ 0.00 & '+ '%5.2f'.format(self.IyB*1e4) +' & '+'%5.2f'.format(self.PyzB) +' \\\\ 0.00 & '+'%5.2f'.format(self.PyzB) +' & ','%5.2f'.format(self.IzB*1e4) +' \\end{array} \\right)$} \\\\\n')
    fileHandler.write('& \\\\\n')
    fileHandler.write('C.D.G.: $(','%5.2f'.format(self.GB[0]) ,',','%5.2f'.format(self.GB[1]) ,')\\ m$  & \\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('Sección homogeneizada:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{ll}\n')
    fileHandler.write('$A_{homog.}=','%6.3f'.format(self.AH) ,'\\ m^2$ & \\multirow{3}{*}{Tensor de inercia ($cm^4$): $ \\left( \\begin{array}{ccc}', '%5.2f'.format(self.JTorsion*1e4) ,' & 0.00 & 0.00 \\\\ 0.00 & ', '%5.2f'.format(self.IyH*1e4) ,' & ','%5.2f'.format(self.PyzH) ,' \\\\ 0.00 & ','%5.2f'.format(self.PyzH) ,' & ','%5.2f'.format(self.IzH*1e4),' \\end{array} \\right)$} \\\\\n')
    fileHandler.write('& \\\\\n')
    fileHandler.write('C.D.G.: $(','%5.2f'.format(self.GH[0]),',','%5.2f'.format(self.GH[1]),')\\ m$  & \\\\\n')
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\textbf{Armadura pasiva}:\\\\\n')
    fileHandler.write('\\hline\n')
    imprimeArmaduraPrincipal('lista_fams_armadura',self.AB,fileHandler)
    fileHandler.write('\\hline\n')
    fileHandler.write('Familias de armadura de cortante:\\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\begin{tabular}{cccccccc}\n')
    fileHandler.write('Id & n. ramas & $\\phi$ & área & sep. & area/m & $\\alpha$ & $\\beta$\\\\\n')
    fileHandler.write(' &  & $(mm)$ & $(cm^2)$ & $(cm)$ & $(cm^2/m)$ & $( \\degree)$ & $( \\degree)$\\\\\n')
    imprimeArmaduraCortante('armCortanteZ',fileHandler)
    imprimeArmaduraCortante('armCortanteY',fileHandler)
    fileHandler.write('\\end{tabular} \\\\\n')
    fileHandler.write('\\hline\n')
    fileHandler.write('\\end{tabular}\n')
    fileHandler.write('\\end{center}\n')
    fileHandler.write('\\caption{'+self.scc.descSeccion+' ('+ latexUtils.toLaTex(self.scc.nmbSeccion) +').'+'} \\label{tb_'+self.scc.nmbSeccion+'}\n')
    fileHandler.write('\\end{table}\n')
    fileHandler.close()

