# -*- coding: utf-8 -*-

# Imprime el encabezamiento para el listado de aperturas de fisura.
def printCabeceraListadoFisuracion(archivo, nmbSecc):
  archivo.write("\\begin{center}\n")
  archivo.write("\\begin{footnotesize}\n")
  archivo.write("\\tablefirsthead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|c|}{\\textbf{",nmbArch,"}}\\\\\n")
  archivo.write("\\multicolumn{6}{|c|}{Comprobación de la sección ",toLaTex(nmbSecc),"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $N$  & $M_x$  & $M_y$  & $w_k$ \\\\\n")
  archivo.write("      &      & (kN) & (kN m) & (kN m) & (mm)  \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablehead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|l|}{\small\sl \ldots continúa}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|c|}{\\textbf{",nmbArch,"}}\\\\\n")
  archivo.write("\\multicolumn{6}{|c|}{Comprobación de la sección ",toLaTex(nmbSecc),"}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("Elem. & Hip. & $N$  & $M_x$  & $M_y$  & $w_k$ \\\\\n")
  archivo.write("      &      & (kN) & (kN m) & (kN m) & (mm)  \\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tabletail{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{6}{|r|}{\small\sl continúa\ldots}\\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablelasttail{\hline}\n")
  archivo.write("\\begin{supertabular}{|l|r|r|r|r|r|}\n")

# Imprime el cierre del listado de aperturas de fisura.
def printCierreListadoFisuracion(archivo):
  archivo.write("\\end{supertabular}\n")
  archivo.write("\\end{footnotesize}\n")
  archivo.write("\\end{center}\n")

