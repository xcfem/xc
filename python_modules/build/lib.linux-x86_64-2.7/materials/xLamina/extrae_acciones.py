# -*- coding: utf-8 -*-
def extraeAccionesLST(nmbArchLST, nmbArchAcc):
  # Extrae las acciones de un archivo de salida generado en Ansys
  listado= open(nmbArchLST,"r")
  acciones= open(nmbArchAcc,"w")
  for line in listado:
    str1= line
    str1SB= str1.strip()
    if(strlen(str1)>0):
      if(re.match(".*Accion.*",str1)):
        nmbAccion= copiaDesde(" ",copiaHasta(":",str1SB))
        acciones.write("\casos{\load_pattern")
        acciones.write("[\"",nmbAccion,"\"]")
        acciones.write("{}}\n")
  listado.close()
  acciones.close()

def extraeIdsElemLST(nmbArchLST, nmbArchElemScc1, nmbArchElemScc2):
  # Extrae los identificadores de elementos de un archivo de salida generado en Ansys
  listado= open(nmbArchLST,"r")
  elementos_scc1= open(nmbArchElemScc1,"w")
  elementos_scc2= open(nmbArchElemScc2,"w")

  fieldwidths = (0,8,9,20,21,32,33,44,45,56,57,68,69,80,81,92,93,106)
  fmtstring = ''.join('%ds' % f for f in fieldwidths)
  fieldstruct = struct.Struct(fmtstring)
  parse = fieldstruct.unpack_from

  for line in listado:
    str1= line
    str1SB= str1.strip()
    if(strlen(str1)>0):
      if(re.match("^ +ELEM .*",str1)):
        if(leerIds==0):
          leerIds= 1
        else: 
          leerIds= 2
      if(re.match("[0-9]+.*",str1SB) & (leerIds==1)):
        lst=csv2list(fixedWidth2Csv(str1))
        elementos_scc1.write("\\nodos")
        elementos_scc1.write("{ \\nod[",lst[0]*10+1,"]{\\coo{",lst[0]*10+1,",0,0}} \\nod[",lst[0]*10+3,".write(\\coo{",lst[0]*10+1,",0,0}} }\n")
        elementos_scc1.write("\\constraints{\\fijaNodo6GDL(",lst[0]*10+1,"){} }\n")
        elementos_scc1.write("\\elementos")
        elementos_scc1.write("{ \\elem_zero_length_section")
        elementos_scc1.write("[",lst[0]*10+1,".write(\\nodes{",lst[0]*10+1,",",lst[0]*10+3,"} }}\n")
        elementos_scc2.write("\\nodos")
        elementos_scc2.write("{ \\nod[",lst[0]*10+2,"]{\\coo{",lst[0]*10+2,",0,0}} \\nod[",lst[0]*10+4,".write(\\coo{",lst[0]*10+2,",0,0}} }\n")
        elementos_scc2.write("\\constraints{\\fijaNodo6GDL(",lst[0]*10+2,"){} }\n")
        elementos_scc2.write("\\elementos")
        elementos_scc2.write("{ \\elem_zero_length_section")
        elementos_scc2.write("[",lst[0]*10+2,".write(\\nodes{",lst[0]*10+2,",",lst[0]*10+4,"} }}\n")

  listado.close()
  elementos_scc1.close()
  elementos_scc2.close()

def extraeCargasLaminaLST(nmbArchLST, nmbArchCargas):
  '''
  Define las cargas en el extremo libre de dada elemento a partir
     de las acciones de un archivo de salida generado en Ansys
  '''
  listado= open(nmbArchLST,"r")
  cargas= open(nmbArchCargas,"w")

  fieldwidths = (0,8,9,20,21,32,33,44,45,56,57,68,69,80,81,92,93,106)
  fmtstring = ''.join('%ds' % f for f in fieldwidths)
  fieldstruct = struct.Struct(fmtstring)
  parse = fieldstruct.unpack_from

  for line in listado:
    str1= line
    str1SB= str1.strip()
    if(strlen(str1)>0):
      if(re.match(".*Accion.*",str1)):
        nmbAccion= copiaDesde(" ",copiaHasta(":",str1SB))
        cargas.write("\\casos{\\set_current_load_pattern")
        cargas.write("{\"",nmbAccion,"\"}}")
        cargas.write("\n")
      else:
        if(re.match("[0-9]+.*",str1SB)):
          lst=csv2list(fixedWidth2Csv(str1))
          '''Adoptamos el método de Wood para evaluar los
             momentos para dimensionar a flexión'''
          # AXIL_X,Q_X,RASANTE,TORSOR,MOM_X,0.0
          #      1,  7,      3,     6,    4,0.0
          cargas.write("\\casos{\\nodal_load{ \\nod{",lst[0]*10+3,"} \val{",lst[1]*1e3,",",lst[7]*1e3,",",lst[3]*1e3,",",0.0,",",(lst[4]+sign(lst[4])*abs(lst[6]))*1e3,",",0.0,"} }}\n")
          # AXIL_Y,Q_Y,RASANTE,TORSOR,MOM_Y,0.0
          #      2,  8,      3,     6,    5,0.0
          cargas.write("\\casos{\\nodal_load{ \\nod{",lst[0]*10+4,"} \val{",lst[2]*1e3,",",lst[8]*1e3,",",lst[3]*1e3,",",0.0,",",(lst[5]+sign(lst[5])*abs(lst[6]))*1e3,",",0.0,"} }}\n")

  listado.close()
  cargas.close()

def extraeDatosLST(nmbArchLST):
  '''
  Defina las cargas en el extremo libre de dada elemento a partir
     de las acciones de un archivo de salida generado en Ansys
  '''
  extraeAccionesLST(nmbArchLST,"/tmp/acciones.xci")
  extraeIdsElemLST(nmbArchLST,"/tmp/elementos_scc1.xci","/tmp/elementos_scc2.xci")
  extraeCargasLaminaLST(nmbArchLST,"/tmp/cargas.xci")
