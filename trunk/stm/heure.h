
#ifndef HeureH

#define HeureH

  #include <sys/resource.h>
  #include <sys/time.h>


//renvoie une mesure de temps en seconde a l'instant donne
inline double give_time() 
{

    struct rusage ru;
    struct timeval tim;
    getrusage(RUSAGE_SELF, &ru);
    //getrusage(RUSAGE_CHILDREN, &ru);


    tim = ru.ru_utime;
    //temps systeme en secondes
    double stime = (double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
    
    return stime;

}

#endif
