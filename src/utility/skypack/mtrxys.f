      SUBROUTINE MTRXYS (LN,N,NY,A,X,Y,DGPNT,BLOCK,NBLOCK,
     &                   FNAME,FUNIT,INFO)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    MTRXYS performs the product (Y)=(A)*(X) being (A) skyline        *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    LN     (sii) : leading dimension of (X) and (Y)                  *
C*    N      (sii) : dimension of the matrix (A)                       *
C*    NY     (sii) : number of vectors to be multiplied                *
C*    A      (arb) : the coefficients of the matrix (A)                *
C*    X      (ari) : input  vectors (X), LNxNY array                   *
C*    Y      (aro) : output vectors (Y), LNxNY array                   *
C*    DGPNT  (aii) : addresses of the diagonal coefficients of (A)     *
C*    BLOCK  (aii) : blocking information                              *
C*    NBLOCK (sii) : number of blocks                                  *
C*    FNAME  (ssi) : file name associated with (A)                     *
C*    FUNIT  (sii) : file unit associated with (A), which is ignored   *
C*                   if FNAME is equal to "INCORE" or "incore"         *
C*    INFO   (sio) : information on the execution of MTRXYS            *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                                                                     *
C*  - BLAS kernels :                                                   *
C*    DAXPY,DDOT,DSCAL                                                 *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          BLOCK(3,*),DGPNT(*),FUNIT,INFO,LN,N,NBLOCK,NY
      DOUBLE PRECISION A(*),X(LN,NY),Y(LN,NY)
      CHARACTER*(*)    FNAME
C
C==== local variables ==================================================
C
      INTEGER          I,IA,IX,J,JDIAG,JMAX,JMIN,K
      DOUBLE PRECISION SUM,XJK
C
C==== BLAS kernel ======================================================
C
      DOUBLE PRECISION DDOT
C
C**** executable statements ********************************************
C
      INFO = 0
C
      IF ( FNAME.NE.'INCORE' .AND. FNAME.NE.'incore' )
     &REWIND (UNIT=FUNIT,ERR=1)
C
C.... initialize (Y) ...................................................
C
      CALL DSCAL (LN*NY,0.0D0,Y,1)
C
C.... compute (Y)=(A)*(X), loop on all blocks ..........................
C
      DO 010 I = 1,NBLOCK
C
C....... define the limits of the current block ........................
C
         IA   = 1
	 JMIN = BLOCK(1,I)
	 JMAX = BLOCK(2,I)
C
         IF ( FNAME.NE.'INCORE' .AND. FNAME.NE.'incore' )
     &   READ (UNIT=FUNIT,ERR=1) (A(J),J=1,DGPNT(JMAX))
C
C....... loop on all columns of the current block ......................
C
         DO 020 J = JMIN,JMAX
C
            JDIAG = DGPNT(J)
            IX = J + IA - JDIAG
C
C.......... loop on all input vectors ..................................
C
            DO 030 K = 1,NY
               XJK = X(J,K)
               SUM = A(JDIAG)*XJK
               IF ( IA .LT. JDIAG ) THEN
                  CALL DAXPY (J-IX,XJK,A(IA),1,Y(IX,K),1)
                  SUM = SUM + DDOT(J-IX,A(IA),1,X(IX,K),1)
               END IF
               Y(J,K) = Y(J,K) + SUM 
  030       CONTINUE
C
            IA = JDIAG + 1
C
  020    CONTINUE
C
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
C**** end of MTRXYS ****************************************************
C
      END
