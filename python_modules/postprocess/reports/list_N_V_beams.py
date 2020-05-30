# -*- coding: utf-8 -*-

import sqlite3 as sqlite

def listBarNormalStressesLaTeX(nmbDBase, nmbTabla, nmbArchivoListadoTex):
    '''Result of normal stresses in linear elements to include in a LaTeX file.'''
    archTex= open(nmbArchivoListadoTex)
    con= sqlite.connect(nmbDBase)
    con.row_factory= sqlite.Row
    idSecc= 0
    idElem= 0
    idComb= 0
    axil= 0.0
    m1= 0.0
    m2= 0.0
    tipoSol= 0
    epsCMin= 0.0
    epsCMax= 0.0
    epsSMax= 0.0
    aprovTN= 0.0
    cur= con.cursor()
    cur.execute("select * from " + nmbTabla)
    for row in cur:
      idSecc= row["idSecc"]
      idElem= row["idElem"]
      idComb= row["idComb"]
      axil= row["axil"]
      m1= row["m1"]
      m2= row["m2"]
      tipoSol= row["tipoSol"]
      epsCMin= row["epsCMin"]
      epsCMax= row["epsCMax"]
      epsSMax= row["epsSMax"]
      aprovTN= row["aprovTN"]
      archTex.write(int(idSecc)," & ",int(idElem)," & ",idComb," & ",format(axil/1e3,'{:6.3f}')," & ",format(m1/1e3,'{:6.3f}')," & ",format(m2/1e3,'{:6.3f}')," & ",format(epsCMin*100,'{:3.2f}')," & ",format(epsCMax*100,'{:3.2f}')," & ",format(epsSMax*100,'{:3.2f}'),"\\\\\n")
    archTex.close()

def listaBarShearLaTeX(nmbDBase, nmbTabla, nmbArchivoListadoTex):
    '''Shear check results in linear elements to include in a LaTeX file.'''
    archTex= open(nmbArchivoListadoTex)
    con= sqlite.connect(nmbDBase)
    con.row_factory= sqlite.Row
    idSecc= 0
    idElem= 0
    idComb= 0
    axil= 0.0
    vrd= 0.0
    vu1= 0.0
    vu2= 0.0
    vu= 0.0
    aprovV= 0.0
    cur= con.cursor("select * from " + nmbTabla)
    cur.execute()
    for fow in cur:
        idSecc= row["idSecc"]
        idElem= row["idElem"]
        idComb= row["idComb"]
        axil= row["axil"]
        vrd= row["vrd"]
        vu1= row["vu1"]
        vu2= row["vu2"]
        vu= row["vu"]
        aprovV= row["aprovV"]
        archTex.write(int(idSecc)," & ",int(idElem)," & ",idComb," & ",format(axil/1e3,'{:6.3f}')," & ",format(vrd/1e3,'{:6.3f}')," & ",format(vu1/1e3,'{:6.3f}')," & ",format(vu2/1e3,'{:6.3f}')," & ",format(vu/1e3,'{:6.3f}'),"\\\\\n")
    archTex.close()
