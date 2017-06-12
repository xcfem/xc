# -*- coding: utf-8 -*-

def VtkCreaStrArraySetData(preprocessor,setName,entTypeName,nmbArray,data):
  # Crea una matriz de cadenas de caracteres con datos de los puntos definidas en el conjunto.
  #Creamos el actor para las líneas.
  st= preprocessor.getSetLoader.getSet(setName)
  
    # \preprocessor{\cad{\sets{\@setName
    #   {
    #     \c{Definimos la matriz}
    #     \vtk{ \define["vtkShStrArray",@nmbArray]{} }
    #     \numera{}
    #     \c{Cargamos los valores en la matriz}
    #     \def_var["tmp","string"]{}
    #     \@entTypeName{\for_each
    #       {
    #         \set{tmp= @eval(@data)}
    #         \vtk{\@nmbArray{ \insert_value{getIdx,@tmp} }}
    #       }}
    #   }}}}
