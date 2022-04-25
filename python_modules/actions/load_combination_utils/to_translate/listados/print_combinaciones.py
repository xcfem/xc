# -*- coding: utf-8 -*-
def printComb(combs, outputPath):
    arch_salida_ltx= open(outputPath,"w")
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
    for cont, comb in enumerate(combs):
        arch_salida_ltx.write(cont," & ",ascii2ltx(comb.getExpandedName),"\\\\\n")
    arch_salida_ltx.write("\\end{longtable}\n")
    arch_salida_ltx.write("\\end{small}\n")
    arch_salida_ltx.write("\\end{center}\n")
    arch_salida_ltx.close()

def printCombEluSitPersistentes(outputPath):
    '''Write the combinations that correspond to ultimate limit states in
       persistent or transient situations.'''
    printComb(combinations.comb_elu_persistentes, outputPath)

def printCombEluSitAccidentales(outputPath):
    '''Write the combinations that correspond to ultimate limit states in
       accidental situations.'''
    printComb(combinations.comb_elu_accidentales, outputPath)

def printCombEluSitSismicas(outputPath):
    '''Write the combinations that correspond to ultimate limit states in
       seismic situations.'''
    printComb(combinations.comb_elu_sismicas, outputPath)

def printCombElsSitPocoFrecuentes(outputPath):
    '''Write the combinations that correspond to serviceability limit states in
     rare situations.'''
    printComb(combinations.comb_els_poco_frecuentes, outputPath)

def printCombElsSitFrecuentes(outputPath):
    '''Write the combinations that correspond to serviceability limit states in
       frequent situations.'''
    printComb(combinations.comb_els_frecuentes, outputPath)

def printCombElsSitCuasiPermanentes(outputPath):
    '''Write the combinations that correspond to serviceability limit states in
       quasi-permanent situations.'''
    printComb(combinations.comb_els_cuasi_permanentes, outputPath)
