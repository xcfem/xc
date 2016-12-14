# -*- coding: utf-8 -*-
# Predimensionamiento de zapata aislada. Números gordos, 2ª ed., pag.16

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from postprocess.reports import common_formats as fmt

class IsolatedFooting(object):
  def __init__(self,B,Nk=10e3,Sadm=150e3,FiArmPilar=12e-3,Lpilar=0.5):
    ''' Constructor.

    :ivar B: Footing side dimension (m)
    :ivar Nk: Characteristic value of axial load in pillar (N).
    :ivar FiArmPilar: Pillar main rebars diameter (m)
    :ivar Lpilar: Pillar depth (m)
    '''
    self.Lzapata= B 
    self.Nk= Nk/1e3 # Axil característico (kN)
    self.Sadm= Sadm/1e3 # Tensión admisible del terreno (kN/m2)
    self.FiArmPilar= FiArmPilar*100 # Diameter of main support reinforcement in centimeters.
    self.Lpilar= Lpilar #Pillar depth (m)

  def getLaTeXHeader(self):
    retval= "\\textsc{Predimensionamiento de zapata aislada} \\\\ \\\\\n"
    retval+= "Datos\n"
    retval+= "\\begin{itemize}\n"
    retval+= "\\item Axil característico del pilar N$_k$ = " + fmt.Esf.format(self.Nk) + " kN\n"
    retval+= "\\item Tensión admisible del terreno $\\sigma_{adm}$ = " + fmt.Esf.format(self.Sadm) + " kN/m$^2$\n"
    retval+= "\\item Diámetro de la reinforcement longitudinal del pilar $\\Phi$ = " + fmt.Diam.format(self.FiArmPilar) + " cm\n"
    retval+= "\item Escuadría del pilar $l$ = " +  fmt.Longs.format(self.Lpilar) + " m\n"
    retval+= "\\end{itemize}\n"
    return retval

  def getLaTeXResults(self):
    # Predimensionamiento
    self.Lzapata=(math.sqrt(self.Nk/self.Sadm))
    retval= "Área de la zapata\n"
    area= self.Nk/self.Sadm
    retval+= "$$A=a^2=\\cfrac{N_k}{\\sigma_{adm}}=" + fmt.Esf.format(area) + "\\ m^2 \\rightarrow a =" + fmt.Longs.format(self.Lzapata) + "\\ m$$\n"
    retval+= "Canto de la zapata\n"
    vuelo=((math.sqrt(self.Nk/self.Sadm)-self.Lpilar)/2)
    retval+= "$$ vuelo = v = \\frac{a-l}{2} = " + fmt.Longs.format(vuelo) + "\\ m $$\n"
    aux=((15*self.FiArmPilar*self.FiArmPilar+10)/100)
    depth=max((max(vuelo/2,0.5)),aux)
    retval+= "$$h = max[v/2; (15\\Phi^2 +10)/100; 0.5] = " + fmt.Longs.format(depth) + "\\ m $$\n"
    retval+= "Armadura de la zapata\n"
    Md=(1.5*self.Sadm*(self.Lzapata)**2/8)
    As=(Md/0.8/depth/500*1.15*10)
    retval+= "$$M_d = 1.5 \\sigma_{adm} \\cfrac{a^2}{8}= " + fmt.Esf.format(Md) + "\\ kN \cdot m/m $$\n"
    retval+= "$$A_s = \\cfrac{M_d}{0.8hf_{yd}} \\times 10 =" + fmt.Areas.format(As) + "\\ cm^2/m $$\n"
    retval+= "\\begin{itemize}\n"
    retval+= "\\item Lower reinforcement must be placed in both directions\n"
    retval+= "\\item Top reinforcement not needed\n"
    retval+= "\\item Se recomienda disponer patilla de, al menos, la mitad del depth de la zapata\n"
    retval+= "\\item Check minimal reinforcement\n"
    retval+= "\\end{itemize}\n"
    return retval

# # Dibujo
# \psplot
# {
#  \pageSize{"letter"}
#  \open{"zapais.ps"} # fichero postscript
#  \fspace{0,0,1.5*self.Lzapata,3*depth} # área ocupada por el dibujo: xmin, ymin, xmax, ymax
#  \lineWidth{0.01}   # grosor de la línea en las unidades de trabajo
#  \penColorName{"blue"} # color de la pluma a utilizar
#  \erase           # Borra todo
#  xini=(0.25*self.Lzapata)
#  yini=(1*depth)
#  \moveTo{xini,yini}       # mueve la pluma levantada al punto especificado (x,y)
#  \cont{xini+self.Lzapata,yini} # traza con la pluma hasta el punto especificado (x,y)
#  \cont{xini+self.Lzapata,yini+depth}
#  \cont{xini+self.Lzapata-vuelo,yini+depth}
#  \cont{xini+self.Lzapata-vuelo,yini+depth+depth/2}
#  \moveTo{xini+vuelo,yini+depth+depth/2}
#  \cont{xini+vuelo,yini+depth}
#  \cont{xini,yini+depth}
#  \cont{xini,yini}
#  \flush           # volcado de todo
#  \close           # cierra el fichero
# }
# print "\\begin{figure}[h] \n"
# print "\\centering \n"
# print "\\includegraphics[width=60mm]{zapais.ps} \n"
# print "\\caption{Predimensionamiento zapata aislada}\\label{zapais} \n"
# print "\\end{figure} \n"
  def getReport(self):
    retval= self.getLaTeXHeader()
    retval+= self.getLaTeXResults()
    return retval

  def getNearSlopeReductionFactor(self,beta,d):
    ''' Coefficient of reduction of bearing capacity for pinned supported
        footing near a slope. Expression proposed by Corté and Garnier (1994)

       :param beta: slope angle
       :param d: horizontal distance between the slope and the nearest 
       foundation edge.
    '''
    tanB= math.tan(beta)
    return 1 - 0.9*tanB*(2-tanB)*(max((1-d/8.0/self.Lzapata),0.0))**2

