# -*- coding: utf-8 -*-

import math

# Nº de pilotes necesario y predimensionamiento de los mismos. Números gordos, 2ª ed., pag.30
print "\input{cabecera.tex}"

# Datos necesarios
Nk= 1000.0 # Axil característico del pilar (kN)

Apil= (math.pi*(0.5)**2/4.0) # Área del pilote (m2)
Spil= 4000 # Tensión de servicio del pilote (kN/m2)
           #                      Pilotes prefabricados: 7000 - 10000
           #                       Pilotes in situ:       3000 -  5000
fck= 30    # Concrete characteristic strength (N/mm2)
fyk= 500   # Characteristic value of reinforcing steel yield strength (N/mm2)
prefabricado= "si" # Si los pilotes son prefabricados "si"
print "\\textsc{Predimensionamiento de pilotes} \\\\ \\\\"
print "Datos"
print "\\begin{itemize}"
print "\\item Axil característico del pilar N$_k$ = ", "%5.2f".format(Nk), " kN" 
print "\\item Área del pilote A$_c$ = ", "%5.2f".format(Apil)," m$^2$"
print "\\item Tensión de servicio del pilote $\\sigma$ = ", "%10.0f".format(Spil), " kN/m$^2$" 
print "\\item Concrete characteristic strength of the pile f$_{ck}$ =", "%3.0f".format(fck), " N/mm$^2$"
print "\\item Characteristic value of pile steel yield strength f$_{ck}$ =", "%4.0f".format(fyk), " N/mm$^2$"
print "\\end{itemize}"


# Predimensionamiento
print "Número de pilotes \\\\ \n "
Rpil=(Apil*Spil)
npil=math.ceil(Nk/Rpil)
print "$$Resistencia\ de\ un\ pilote\ R=A\\cdot\\sigma = ", "%5.2f".format(Rpil),"\ kN$$"
print "$$N^o\ pilotes\ n=\\cfrac{N_k}{R} = ", "%3.0f".format(npil),"\ pilotes$$"
print "Armadura \\\\ \n "
print "\\begin{itemize}"
print "\\item{Axil de cálculo}"
Nd=(1.5*Nk/npil)
print "$$N_d = 1.5 \\cfrac{N_K}{n} = ", "%5.2f".format(Nd),"\ kN$$"
print "\\item{Armadura}"
As=((Nd-fck/1.5*Apil/1000)/fyk*1.15*10)
print "$$N_d = f_{cd} A_c + f_{yd} A_s \\rightarrow $$"
print "$$A_s = \\cfrac{N_d - f_{cd} A_c \\left[\\times \\cfrac{1}{1000}\\right]}{f_{yd}}[\\times 10] =", "%5.2f".format(As),"\ cm^2  $$"
print "\\item{Armadura mínima}"
print "$$Min.\ geom.\ A_s \\ge 4\\permil A_c =", "%5.2f".format((Apil*40)),"\ cm^2 $$"
Asmmec=(0.1*Apil*fck/1.5/fyk*1.15*10000)
print "$$Min.\ mec.\ A_s \\ge\\cfrac{ 10\\% A_c f_{cd}}{f_{yd}}=", "%5.2f".format(Asmmec),"\ cm^2 $$"
print "\\item{Armadura máxima}"
if(prefabricado=="si"):
  Asmmec=(0.6*Apil*fck/1.5/fyk*1.15*10000)
  print "$$Max.\ mec.\ A_s \\le\\cfrac{ 60\\% A_c f_{cd}}{f_{yd}}=", "%5.2f".format(Asmmec),"\ cm^2 $$"
else:
  Asmmec=(Apil*fck/1.5/fyk*1.15*10000)
  print "$$Max.\ mec.\ A_s \\le\\cfrac{ 100\\% A_c f_{cd}}{f_{yd}}=", "%5.2f".format(Asmmec),"\ cm^2 $$"

print "\\item{Reinforcement tips} \\\\"
print "\\begin{tabular}{lr}"
print "Minimal diameter for the longitudinal reinforcement & $\\Phi$12 mm     \\\\"
print "Minimal number of rebars & 6   \\\\"
print "Minimal spacement between rebars in longitudinal reinforcement. & 20 cm  \\\\"
print "Minimal diameter of shear reinforcement rebars & $\\Phi_{transv.min} = 0,25\\Phi_{long}$   \\\\"
print "Stirrups spacement & s<15$\\Phi_{long}$  \\\\"
if(prefabricado=="si"):
  print "Cover of the reinforcement & 4-5 cm  \\\\"
else:
  print "Cover of the reinforcement & 7 cm  \\\\"
print "   \\\\"
print "\\end{tabular}"
print "\\end{itemize}"
print "\\end{document}"
