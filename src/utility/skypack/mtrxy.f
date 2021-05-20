      SUBROUTINE MTRXY (LN,N,NY,A,X,Y,DGPNT,BLOCK,NBLOCK,
     &                  MTYPE,FNAME,FUNIT,INFO)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    MTRXY performs the product (Y)=(A)*(X), where (A) is a real      *
C*    symmetric matrix stored in a skyline way (the upper              *
C*    triangle including the diagonal)                                 *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    LN     (sii) : leading dimension of (X) and (Y)                  *
C*    N      (sii) : dimension of the matrix (A)                       *
C*    NY     (sii) : number of vectors to be multiplied                *
C*    A      (arb) : the coefficients of the matrix (A)                *
C*                 - if MTYPE=1, (A) can be read from the binary file  *
C*                   specified by FNAME, one record with N words       *
C*                 - if MTYPE=2, (A) can be read from the binary file  *
C*                   specified by FNAME, NBLOCK records with the       *
C*                   number of words defined by DGPNT and BLOCK        *
C*    X      (ari) : input  vectors (X), LNxNY array                   *
C*    Y      (aro) : output vectors (Y), LNxNY array                   *
C*    DGPNT  (aii) : addresses of the diagonal coefficients of (A)     *
C*    BLOCK  (aii) : blocking information, which is ignored if MTYPE=1 *
C*    NBLOCK (sii) : number of blocks, which is ignored if MTYPE=1     *
C*    MTYPE  (sii) : matrix type                                       *
C*                   = 1, (A) is diagonal                              *
C*                   = 2, (A) is skyline (upper triangle)              *
C*    FNAME  (ssi) : file name associated with (A)                     *
C*    FUNIT  (sii) : file unit associated with (A), which is ignored   *
C*                   if FNAME is equal to "INCORE" or "incore"         *
C*    INFO   (sio) : information on the execution of MTRXY             *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                   = 2, the file specified by FNAME does not exist   *
C*                                                                     *
C*  - SUBPROGRAMS :                                                    *
C*    MTRXYD,MTRXYS                                                    *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          BLOCK(3,*),DGPNT(*),FUNIT,INFO,
     &                 LN,MTYPE,N,NBLOCK,NY
      DOUBLE PRECISION A(*),X(LN,NY),Y(LN,NY)
      CHARACTER*(*)    FNAME
C
C==== local variables ==================================================
C
      LOGICAL          ALIVE,JOINED
C
C**** executable statements ********************************************
C
C.... if FNAME is given check the status of the file ...................
C
      IF ( FNAME.NE.'INCORE' .AND. FNAME.NE.'incore' ) THEN
C
         INQUIRE (FILE=FNAME,EXIST=ALIVE,OPENED=JOINED,ERR=1) 
C
         IF      ( .NOT.ALIVE  ) THEN
                 INFO = 2
                 RETURN
         ELSE IF ( .NOT.JOINED ) THEN
                 OPEN (UNIT  =FUNIT,
     &                 FILE  =FNAME,
     &                 FORM  ='UNFORMATTED',
     &                 ACCESS='SEQUENTIAL',
     &                 STATUS='OLD',
     &                 ERR   =1)
         END IF
C
      END IF
C
C.... compute (Y)=(A)*(X) ..............................................
C
      IF       ( MTYPE .EQ. 1 ) THEN
C
C....... diagonal (A) ..................................................
C
         CALL MTRXYD (LN,N,NY,A,X,Y,FNAME,FUNIT,INFO)             
C
      ELSE IF  ( MTYPE .EQ. 2 ) THEN
C
C....... skyline  (A) ..................................................
C
         CALL MTRXYS (LN,N,NY,A,X,Y,DGPNT,BLOCK,NBLOCK,FNAME,FUNIT,INFO)
C
      END IF
C
      RETURN
C
C.... an IO error has occurred .........................................
C
    1 INFO = 1
C
      RETURN
C
C**** end of MTRXY *****************************************************
C
      END
