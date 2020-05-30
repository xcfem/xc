# -*- coding: utf-8 -*-

def listaKPtsSet(preprocessor,setName, fileName, fmt):
    ''' Writes a list of the points from the set

       :param setName: name of the set.
       :param fileName: file name.
       :param fmt: format for coordinates.
    '''
    caption= "Points of the set: "+setName
    defCampos= "|r|r|r|r|"
    idsCampos= "Id & x & y & z"
    cabeceraSupertabular(fileName,4,defCampos,idsCampos,caption)
    pointSet= preprocessor.getSets.getSet(setName)
    points= pointSet.getPoints()
    for p in points:
        fileName.write(p.tag," & ",fmt.format(p.pos.x)," & ",fmt.format(p.pos.y)," & ",fmt.format(p.pos.z),"\\\\\n")
    cierraSupertabular(fileName) 

def listaLineasSet(setName, fileName):
    ''' Writes a list of the lines from the set

       :param setName: name of the set.
       :param fileName: file name.
    '''
    caption= "Lineas del conjunto: "+setName
    defCampos= "|r|r|c|"
    idsCampos= "Id & nDiv & Points"
    cabeceraSupertabular(fileName,3,defCampos,idsCampos,caption) 
    setLineas= preprocessor.getSets.getSet(setName)
    lineas= setLineas.getLines()
    for l in lineas:
        fileName.write(l.tag," & ",l.ndiv," & ",l.kPts,"\\\\\n")
    cierraSupertabular(fileName)

def listSetNodes(preprocessor, setName, fileName, fmt):
    ''' Writes a list of the nodes from the set

       :param setName: name of the set.
       :param fileName: file name.
       :param fmt: format for coordinates.
    '''
    caption= "Nodes from set: "+setName
    defCampos= "|r|r|r|r|"
    idsCampos= "Id & x & y & z"
    cabeceraSupertabular(fileName,4,defCampos,idsCampos,caption) 
    s= preprocessor.getSets.getSet(setName)
    nodes= s.nodes
    for n in nodes:
        pos= n.getPos()
        fileName.write(n.tag," & ",fmt.format(pos.x)," & ",fmt.format(pos.y)," & ",fmt.format(pos.z),"\\\\\n")
    cierraSupertabular(fileName) 

def listaElementosSet(preprocessor, setName, fileName):
    ''' Writes a list of the nodes from the set

       :param setName: name of the set.
       :param fileName: file name.
    '''
    fileName.write("Elementos del conjunto: ",setName,"\n")
    s= preprocessor.getSets.getSet(setName)
    elems= s.elements
    for e in elems:
        fileName.write(e.tag," & ",e.nod(0).tag," & ",e.nod(1).tag,"\\\\\n")

def listaElementosTrussSet(preprocessor, setName, fileName, fmt):
    ''' Writes a list of the truss elements from the set

       :param setName: name of the set.
       :param fileName: file name.
       :param fmt: format for coordinates.
    '''
    caption= "Elementos del conjunto: "+setName
    defCampos= "|r|r|r|r|"
    idsCampos= "Id & nI & nJ & Mater."
    cabeceraSupertabular(fileName,4,defCampos,idsCampos,caption) 
    s= preprocessor.getSets.getSet(setName)
    elems= s.elements
    for e in elems:
        fileName.write(e.tag," & ",e.nod(0).tag," & ",e.nod(1).tag," & ",e,getMaterial().tag,"\\\\\n")
    cierraSupertabular(fileName) 

def listaBarElementSet(preprocessor, setName, fName, fmt):
    ''' Writes a list of the bar elements from the set

       :param setName: name of the set.
       :param fileName: file name.
       :param fmt: format for coordinates.
    '''
    caption= "Elemento from set: "+setName
    defCampos= "|r|r|r|r|r|r|r|r|r|r|r|"
    idsCampos= " Id & nI  & nJ  &Sc.&  E     &  G     & Area   & alpha &    J     &   Iy    &    Iz  \\\\\n -  &     &  -  & - & GPa    & GPa    &  cm2   &   -   &   cm4    &  cm4    &   cm4"
    cabeceraSupertabular(fileName,11,defCampos,idsCampos,caption) 
    s= preprocessor.getSets.getSet(setName)
    elems= s.elements
    for e in elems:
        str= str(e.tag)+" & "+str(e.nod(0).tag)+" & "+str(e.nod(1).tag)+" & "
        k= 0
        sections= e.getSections()
        for s in sections:
            fName.write(str,k," & ")
            fName.write(fmt.format(s.getE()/1e9)," & ",fmt.format(s.getG()/1e9)," & ",fmt.format(s.getA()*1e4)," & ",fmt.format(s.getAlpha())," & ")
            fName.write(fmt.format(s.getJ()*1e6)," & ",fmt.format(s.getIy()*1e6)," & ",fmt.format(s.getIz()*1e6),"\\\\\n")
            k+=1

    cierraSupertabular(fileName) 


def listaEntidadesSet(preprocessor, setName, fileName, nmbProcLstElementos, fmt, encab, tit):
    ''' Writes a list of the entities from the set

       :param setName: name of the set.
       :param fileName: file name.
       :param fmt: format for coordinates.
    '''
    fileName.write("\\twocolumn\n")
    fileName.write("\\",encab,"{",tit,"}\n")
    listaKPtsSet(preprocessor, setName,fileName,fmt) 
    listaLineasSet(preprocessor, setName,fileName) 
    listSetNodes(preprocessor, setName,fileName,fmt) 
    fileName.write("\\onecolumn\n")
    nmbProcLstElementos(preprocessor, setName,fileName,fmt) 


def listaEntidadesLinea(preprocessor, lineName, fileName, nmbProcLstElementos, fmt, encab, tit):
    ''' Writes a list of the entities from the line

       :param lineName: name of the line.
       :param fileName: file name.
       :param fmt: format for coordinates.
    '''
    fileName.write("\\",encab,"{",tit,"}\n")
    listSetNodes(preprocessor, lineName,fileName,fmt)
    nmbProcLstElementos(lineName,fileName,fmt) 

