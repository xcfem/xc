# -*- coding: utf-8 -*-
# Predimensionamiento de zapata aislada. Números gordos, 2ª ed., pag.16

import math

print "\input{cabecera.tex}"

# Datos necesarios
Nk=10 # Axil característico (kN)
Sadm=1 # Tensión admisible del terreno (kN/m2)
FiArmPilar=1.2 # Diámetro de la armadura longitudinal del pilar (cm)
Lpilar=0.5 # Escuadría del pilar (m)
print "\\textsc{Predimensionamiento de zapata aislada} \\\\ \\\\"
print "Datos"
print "\\begin{itemize}"
print "\\item Axil característico del pilar N$_k$ = ", "%5.2f".format(Nk), " kN" 
print "\\item Tensión admisible del terreno $\\sigma_{adm}$ = ", Sadm, " kN/m$^2$" 
print "\\item Diámetro de la armadura longitudinal del pilar $\\Phi$ = ", "%2.1f".format(FiArmPilar), " cm" 
print "\item Escuadría del pilar $l$ = ",  "%3.2f".format(Lpilar), " m"
print "\\end{itemize}"

# Predimensionamiento
Lzapata=(math.sqrt(Nk/Sadm))
print "Área de la zapata"
print "$$A=a^2=\\cfrac{N_k}{\\sigma_{adm}}=",(Nk/Sadm),"\\ m^ 2 \\rightarrow a =", "%3.2f".format(Lzapata),"\\ m$$"
print "Canto de la zapata"
vuelo=((math.sqrt(Nk/Sadm)-Lpilar)/2)
print "$$ vuelo = v = \\frac{a-l}{2} = ", "%3.2f".format(vuelo),"\\ m $$"
aux=((15*FiArmPilar*FiArmPilar+10)/100)
depth=max((max(vuelo/2,0.5)),aux)
print "$$h = max[v/2; (15\\Phi^2 +10)/100; 0.5] = ", "%3.2f".format(depth), "\\ m $$"
print "Armadura de la zapata"
Md=(1.5*Sadm*(Lzapata)**2/8)
As=(Md/0.8/depth/500*1.15*10)
print "$$M_d = 1.5 \\sigma_{adm} \\cfrac{a^2}{8}= ","%3.2f".format(Md),"\\ mkN/m $$"
print "$$A_s = \\cfrac{M_d}{0.8hf_{yd}} \\times 10 =", "%3.2f".format(As),"\\ cm^2/m $$"
print "\\begin{itemize}"
print "\\item La armadura se dispondrá en el paramento inferior en ambas direcciones"
print "\\item No es necesario disponer armadura en el paramento superior"
print "\\item Se recomienda disponer patilla de, al menos, la mitad del depth de la zapata"
print "\\item Comprobar la armadura mínima"
print "\\end{itemize}"

# # Dibujo
# \psplot
# {
#  \pageSize{"letter"}
#  \open{"zapais.ps"} # fichero postscript
#  \fspace{0,0,1.5*Lzapata,3*depth} # área ocupada por el dibujo: xmin, ymin, xmax, ymax
#  \lineWidth{0.01}   # grosor de la línea en las unidades de trabajo
#  \penColorName{"blue"} # color de la pluma a utilizar
#  \erase           # Borra todo
#  xini=(0.25*Lzapata)
#  yini=(1*depth)
#  \moveTo{xini,yini}       # mueve la pluma levantada al punto especificado (x,y)
#  \cont{xini+Lzapata,yini} # traza con la pluma hasta el punto especificado (x,y)
#  \cont{xini+Lzapata,yini+depth}
#  \cont{xini+Lzapata-vuelo,yini+depth}
#  \cont{xini+Lzapata-vuelo,yini+depth+depth/2}
#  \moveTo{xini+vuelo,yini+depth+depth/2}
#  \cont{xini+vuelo,yini+depth}
#  \cont{xini,yini+depth}
#  \cont{xini,yini}
#  \flush           # volcado de todo
#  \close           # cierra el fichero
# }
print "\\begin{figure}[h] \n" 
print "\\centering \n"
print "\\includegraphics[width=60mm]{zapais.ps} \n" 
print "\\caption{Predimensionamiento zapata aislada}\\label{zapais} \n"
print "\\end{figure} \n"

print "\\end{document}"



