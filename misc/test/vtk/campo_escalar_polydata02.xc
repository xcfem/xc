\path{"/usr/local/lib/macros_xc"}
\incluye{"vtk/vtk_grafico_diagrama_esfuerzos.xcm"}

\def_prop["defDiagrama","record"]{}
\populateRecordDefDiagramaEsf("defDiagrama"){}

\def_prop["org","pos3d"]{}
\def_prop["dest","pos3d"]{}
\dest{\coo{0,10,0}}
\def_prop["valOrg","double"]{100}
\def_prop["valDest","double"]{-80}
\def_prop["indice","double"]{0}
\defDiagrama
  {
    \set{nmbDiagrama= "pData"}
    \set{nmbPoints= "points"}
    \set{nmbEscalares= "escalares"}
    \set{nmbCeldas= "celdas"}
    \set{escala= 0.02}
    \set{vDir= [0,0,1]}
    \set{valMin= @min(@valOrg,@valDest)}
    \set{valMax= @max(@valOrg,@valDest)}
  }

\vtk
  {
    \define["vtkPoints","points"]{}
    \define["vtkDoubleArray","escalares"]{}
    \define["vtkCellArray","celdas"]{}
    \set{indice= @vtkCreaTramoDiagrama(@indice,@org,@valOrg,@dest,@valDest,"defDiagrama")}

    \vtkCreaLookUpTable("defDiagrama"){}
    \vtkCreaActorDiagrama("defDiagrama"){}

\c{Dibuja}
\define["vtkRenderer","renderer"]{}
\define["vtkRenderWindow","renWin"]{ \add_renderer{"renderer"} }
\define["vtkRenderWindowInteractor","iren"]{ \set_render_window{"renWin"} }



\renderer{ \add_actor{"pDataActor"} \set_background{1,1,1}}

\renWin{ \set_size{450,450} }
     \renderer
       {
         \reset_camera{}
         \active_camera
           {
             \view_up{0,0,1}
             \position{100,0,0}
             \parallel{}
           }
         \reset_camera_clipping_range{}
       }
\iren{ \initialize{} }
\iren{ \start{} }
  }