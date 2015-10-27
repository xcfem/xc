# -*- coding: utf-8 -*-

def VtkCreaStrArraySetData(preprocessor,nmbSet,nmbTipoEnt,nmbArray,data):
  # Crea una matriz de cadenas de caracteres con datos de los puntos definidas en el conjunto.
  #Creamos el actor para las líneas.
  st= preprocessor.getSetLoader.getSet(nmbSet)
  
    # \preprocessor{\cad{\sets{\@nmbSet
    #   {
    #     \c{Definimos la matriz}
    #     \vtk{ \define["vtkShStrArray",@nmbArray]{} }
    #     \numera{}
    #     \c{Cargamos los valores en la matriz}
    #     \def_var["tmp","string"]{}
    #     \@nmbTipoEnt{\for_each
    #       {
    #         \set{tmp= @eval(@data)}
    #         \vtk{\@nmbArray{ \insert_value{getIdx,@tmp} }}
    #       }}
    #   }}}}
