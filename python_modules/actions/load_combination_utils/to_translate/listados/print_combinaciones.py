# -*- coding: utf-8 -*-
def printComb(pathArchSalida):
  {
    arch_salida_ltx.open(pathArchSalida,"w")
    arch_salida_ltx.write("\\begin{center}\n")
    arch_salida_ltx.write("\\begin{small}\n")
    arch_salida_ltx.write("\\begin{longtable}{|c|l|}\n")
    arch_salida_ltx.write("\\hline\n")
    arch_salida_ltx.write("Combination & Decomposition \\\\\n")
    arch_salida_ltx.write("\\hline\n")
    arch_salida_ltx.write("\\endfirsthead\n")
    arch_salida_ltx.write("\\hline\n")
    arch_salida_ltx.write("\\multicolumn{2}{|l|}{../..}\\\\\n")
    arch_salida_ltx.write("\\hline\n")
    arch_salida_ltx.write("Combination & Decomposition \\\\\n")
    arch_salida_ltx.write("\\hline\n")
    arch_salida_ltx.write("\\endhead\n")
    arch_salida_ltx.write("\\hline\n")
    arch_salida_ltx.write("\\multicolumn{2}{|r|}{../..}\\\\\n")
    arch_salida_ltx.write("\\hline\n")
    arch_salida_ltx.write("\\endfoot\n")
    arch_salida_ltx.write("\\hline \\hline\n")
    arch_salida_ltx.write("\\endlastfoot\n")
    \expr{cont= 1}
    \for_each
      {
        arch_salida_ltx.write(cont," & ",ascii2ltx(getExpandedName),"\\\\\n")
        \expr{cont= tonum(cont+1)}
      }
    arch_salida_ltx.write("\\end{longtable}\n")
    arch_salida_ltx.write("\\end{small}\n")
    arch_salida_ltx.write("\\end{center}\n")
    arch_salida_ltx.close()
  }

def printCombEluSitPersistentes(pathArchSalida):
  '''Write the combinations that correspond to ultimate limit states in
   persistent or transient situations.'''
  {
    \combinations{ \comb_elu_persistentes{ \printComb(pathArchSalida){} } }
  }

def printCombEluSitAccidentales(pathArchSalida):
  '''Write the combinations that correspond to ultimate limit states in
   accidental situations.'''
  {
    \combinations{ \comb_elu_accidentales{ \printComb(pathArchSalida){} } }
  }

def printCombEluSitSismicas(pathArchSalida):
  '''Write the combinations that correspond to ultimate limit states in
   seismic situations.'''
  {
    \combinations{ \comb_elu_sismicas{ \printComb(pathArchSalida){} } }
  }

def printCombElsSitPocoFrecuentes(pathArchSalida):
  '''Write the combinations that correspond to serviceability limit states in
   rare situations.'''
  {
    \combinations{ \comb_els_poco_frecuentes{ \printComb(pathArchSalida){} } }
  }

def printCombElsSitFrecuentes(pathArchSalida):
  '''Write the combinations that correspond to serviceability limit states in
   frequent situations.'''
  {
    \combinations{ \comb_els_frecuentes{ \printComb(pathArchSalida){} } }
  }

def printCombElsSitCuasiPermanentes(pathArchSalida):
  '''Write the combinations that correspond to serviceability limit states in
   quasi-permanent situations.'''
  {
    \combinations{ \comb_els_cuasi_permanentes{ \printComb(pathArchSalida){} } }
  }
