def cargaCombTabla(nmbDataBase,nmbTabla,nmbGrupo,nmbSubGrupo):
  '''Put the combinations in a MySQL table.'''
  {
    conex= 1
    \conex
      {
        \connect{nmbDataBase,localhost,guest}
        \create_query{"crea_tabla_comb"}
        \crea_tabla_comb
          {
            \execute_sql{"DROP TABLE IF EXISTS nmbTabla"}
            \execute_sql{"CREATE TABLE IF NOT EXISTS nmbTabla (idComb INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY, grupo CHAR(20), subgrupo CHAR(10), descomp CHAR(100))"}
            \for_each
              {
                \execute_sql{"INSERT INTO nmbTabla (grupo,subgrupo,descomp) VALUES ('nmbGrupo','nmbSubGrupo','getName')"}
              }
          }
      }
  }
