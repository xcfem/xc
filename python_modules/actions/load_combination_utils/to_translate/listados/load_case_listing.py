# -*- coding: utf-8 -*-
# Imprime el encabezamiento para la lista de hipótesis.
def printCabeceraListaHipotesis(archivo):
  archivo.write("\\begin{center}\n")
  archivo.write("\\tablefirsthead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{2}{|c|}{Hipótesis de cálculo}\\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablehead{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{2}{|l|}{\small\sl \ldots continúa}\\\\\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{2}{|c|}{Hipótesis de cálculo}\\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tabletail{%\n")
  archivo.write("\\hline\n")
  archivo.write("\\multicolumn{2}{|r|}{\small\sl continúa\ldots}\\\\\n")
  archivo.write("\\hline}\n")
  archivo.write("\\tablelasttail{\hline}\n")
  archivo.write("\\begin{supertabular}{|l|l|}\n")

