      SUBROUTINE MTRXYD (LN,N,NY,A,X,Y,FNAME,FUNIT,INFO)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    MTRXYD performs the product (Y)=(A)*(X) being (A) diagonal       *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    LN     (sii) : leading dimension of (X) and (Y)                  *
C*    N      (sii) : dimension of the matrix (A)                       *
C*    NY     (sii) : number of vectors to be multiplied                *
C*    A      (ari) : the diagonal of (A)                               *
C*    X      (ari) : input  vectors (X), LNxNY array                   *
C*    Y      (aro) : output vectors (Y), LNxNY array                   *
C*    FNAME  (ssi) : file name associated with (A)                     *
C*    FUNIT  (sii) : file unit associated with (A), which is ignored   *
C*                   if FNAME is equal to "INCORE" or "incore"         *
C*    INFO   (sio) : information on the execution of MTRXYD            *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          FUNIT,INFO,LN,N,NY 
      DOUBLE PRECISION A(N),X(LN,NY),Y(LN,NY)
      CHARACTER*(*)    FNAME
C
C==== local variables ==================================================
C
      INTEGER          I,J
C
C**** executable statements ********************************************
C
C.... if required read (A) from the file ...............................
C
      IF ( FNAME.NE.'INCORE' .AND. FNAME.NE.'incore' ) THEN
         REWIND (UNIT=FUNIT,ERR=1)
         READ   (UNIT=FUNIT,ERR=1) (A(I),I=1,N)
      END IF
C
C.... compute (Y)=(A)*(X) ..............................................
C
      DO 010 I = 1,NY
         DO 020 J = 1,N 
            Y(J,I) = A(J)*X(J,I)
  020    CONTINUE
  010 CONTINUE
C
      RETURN
C
C.... an IO error has occurred .........................................
C
    1 INFO = 1
C
      RETURN
C
C**** end of MTRXYD ****************************************************
C
      END
