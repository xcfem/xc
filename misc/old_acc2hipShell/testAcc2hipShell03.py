# -*- coding: utf-8 -*-
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia.

# Macros

from sqliteUtils import macros_sqlite
execfile("misc/ansysToEsfShell.lcmm")
execfile("misc/tablaEsf.lcmm")
from model import fix_node_6dof
execfile("misc/banco_pruebas_shell.xcm")
execfile("materiales/parametrosSeccionRectangular.xcm")
execfile("materiales/ehe/tablasResult.xcm")
execfile("acciones/borraAcciones.xcm")
execfile("misc/calculaCombinaciones.xcm")
execfile("misc/acc2hipShell.xcm")

os.system("cp test_verif/combinaciones/acc2hipShell/EP1.db /tmp/EP1.db")
os.system("cp test_verif/combinaciones/acc2hipShell/acc2hipShellPrb3.lst /tmp/acc2hipShellPrb3.lst")

\acc2hipShell("/tmp/EP1.db","comb_elu","comb_els","/tmp/acc2hipShellPrb3.lst","%4.1f")
os.system("rm -f /tmp/acc2hipShellPrb3.lst")
os.system("rm -f /tmp/acc2hipShellPrb3.db")
iguales= fcmp("/tmp/acc2hipShellPrb3.hip","test_verif/combinaciones/acc2hipShell/acc2hipShellPrb3.hip.verif")
os.system("rm -f /tmp/acc2hipShellPrb3.hip")
os.system("rm -f /tmp/EP1.db")


import os
fname= os.path.basename(__file__)
if iguales:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
