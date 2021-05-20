# -*- coding: utf-8 -*-
from __future__ import print_function

\combinations
  {
    \comb_elu_persistentes
      {
        print("Computed: ",number_of_combinations," combinations in persistent or transient situations\n")
        arch_salida_ltx.open("./examples/output/comb_ehe_elu_pt.tex","w")
        execfile("print_combinations.acc")
        arch_salida_ltx.close()
      }
    \comb_elu_accidentales
      {
        print("Computed: ",number_of_combinations," combinations in accidental situations.\n")
        arch_salida_ltx.open("./examples/output/comb_ehe_elu_acc.tex","w")
        execfile("print_combinations.acc")
        arch_salida_ltx.close()
      }
    \comb_elu_sismicas
      {
        print("Computed: ",number_of_combinations," combinations in seismic situations.\n")
        arch_salida_ltx.open("./examples/output/comb_ehe_elu_sism.tex","w")
        execfile("print_combinations.acc")
        arch_salida_ltx.close()
      }
    \comb_els_poco_frecuentes
      {
        print("Computed: ",number_of_combinations," combinations in rare situations.\n")
        arch_salida_ltx.open("./examples/output/comb_ehe_els_pf.tex","w")
        execfile("print_combinations.acc")
        arch_salida_ltx.close()
      }
    \comb_els_frecuentes
      {
        print("Computed: ",number_of_combinations," combinations in frequent situations.\n")
        arch_salida_ltx.open("./examples/output/comb_ehe_els_f.tex","w")
        execfile("print_combinations.acc")
        arch_salida_ltx.close()
      }
    \comb_els_cuasi_permanentes
      {
        print("Computed: ",number_of_combinations," combinations in quasi-permanent situations.\n")
        arch_salida_ltx.open("./examples/output/comb_ehe_els_cp.tex","w")
        execfile("print_combinations.acc")
        arch_salida_ltx.close()
      }
  }
