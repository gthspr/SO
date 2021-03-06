#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

int semafor;

static void semafor_p(void);
static void semafor_v(void);

//  arg[0] - nazwa programu - program
//  arg[1] - semafor
//  arg[2] - ilosc sekcji krytycznych
// ./program int int
int main(int argc, char* argv[]) 
{
  
  int i;
  int sections;

  if(argc!=3)
  {
    fprintf(stdout,"nieprawidlowa ilosc argumentow programu program! Powinno byc 2\n");
    exit(3);
  }

  semafor = atoi(argv[1]);
  sections = atoi(argv[2]);

  fprintf(stdout,"Pid: %d   Przed sekcjami krytycznymi.\n", getpid());



  for(i=0;i<sections;i++)
  {
    //sekcja krytyczna
    semafor_p();
    
    
    fprintf(stdout,"Pid: %d   W sekcji krytycznej nr: %d \n", getpid(), i);
    fprintf(stdout,"wartosc semafora: %d\n",semctl(semafor,0,GETVAL));

    sleep(0.1);

    
    //koniec sekcji krytycznej
    semafor_v();
  }

  fprintf(stdout,"Pid: %d   Po sekcjach krytycznych.\n", getpid());

  return 0;
}

static void semafor_p(void)
{
  int zmien_sem;
  struct sembuf bufor_sem;
  bufor_sem.sem_num=0;
  bufor_sem.sem_op=-1;
  bufor_sem.sem_flg=SEM_UNDO;
  zmien_sem=semop(semafor,&bufor_sem,1);
  if (zmien_sem==-1) 
  {
    if(errno==EINTR)
    {
      printf("Pid: %d Wznowienie procesu\n",getpid());
      semafor_p();
    }
    else
    {
      printf("Pid: %d   Nie moglem odblokowac sekcji krytycznej.\n",getpid());
      exit(EXIT_FAILURE);
    }
    
  }
  else
  {
    printf("Pid: %d   Sekcja krytyczna zablokowana.\n",getpid());
  }
}

static void semafor_v(void)
{
  int zmien_sem;
  struct sembuf bufor_sem;
  bufor_sem.sem_num=0;
  bufor_sem.sem_op=1;
  bufor_sem.sem_flg=SEM_UNDO;
  if (semop(semafor,&bufor_sem,1)==-1) 
  {
    if(errno==EINTR)
    {
      printf("Pid: %d Wznowienie procesu\n",getpid());
      semafor_v();
    }
      
    printf("Pid: %d   Nie moglem zablokowac sekcji krytycznej.\n",getpid());
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("Pid: %d   Sekcja krytyczna odblokowana.\n\n",getpid());
  }
}
