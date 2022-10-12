# -*- coding: utf-8 -*-
from __future__ import print_function

def printCombEHE(eheCombinations, outputFileName:str, msg:str):
    number_of_combinations= len(eheCombinations)
    print("Computed: "+str(number_of_combinations)+" combinations in "+msg)
    arch_salida_ltx= open(outputFileName,"w")
    exec(open("print_combinations.acc").read())
    arch_salida_ltx.close()

printCombEHE(comb_elu_persistentes, outputFileName= "./examples/output/comb_ehe_elu_pt.tex", msg= "persistent or transient situations.\n")

printCombEHE(comb_elu_accidentales, outputFileName= "./examples/output/comb_ehe_elu_acc.tex", msg= "accidental situations.\n")

printCombEHE(comb_elu_sismicas, outputFileName= "./examples/output/comb_ehe_elu_sism.tex", msg= "seismic situations.\n")

printCombEHE(comb_els_poco_frecuentes, outputFileName= "./examples/output/comb_ehe_els_pf.tex", msg= "rare situations.\n")

printCombEHE(comb_els_frecuentes, outputFileName= "./examples/output/comb_ehe_els_f.tex", msg= "frequent situations.\n")

printCombEHE(comb_els_quasi_permanentes, outputFileName= "./examples/output/comb_ehe_els_cp.tex", msg= "quasi-permanent situations.\n")
