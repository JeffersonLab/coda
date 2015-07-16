
/* coda_et2et.c - CODA wrapper for et_2_et */

#if defined(Linux_armv7l)

void
coda_et2et()
{
  printf("coda_et2et is dummy for ARM etc\n");
}

#else

/* INCLUDES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#ifdef Linux
#include <sys/prctl.h>
#endif
#if defined __sun||LINUX
#include <dlfcn.h>
#endif
#ifdef Linux
#include <unistd.h> /* for usleep() */
#endif

#include "rc.h"
#include "da.h"
#include "libdb.h"
#include "et_private.h"


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


typedef int (*IFUNCPTR) ();

typedef struct ETpriv
{
  objClass object;

  char *session;

  /* params */
  int ngroups;
  int nevents;
  int event_size;
  int serverPort;
  int udpPort;
  int deleteFile;
  int noDelay;
  int maxNumStations;
  int sendBufSize;
  int recvBufSize;
  int et_verbose;


  /* variables */
  char          host[ET_FILENAME_LENGTH];      /* our host */
  char          host_from[ET_FILENAME_LENGTH];
  char          et_from[ET_FILENAME_LENGTH];
  char          host_to[ET_FILENAME_LENGTH];
  char          et_to[ET_FILENAME_LENGTH];
  char          station[ET_FILENAME_LENGTH];
  char          option[ET_FILENAME_LENGTH];

  et_sysconfig  config;

  et_statconfig   sconfig;
  et_openconfig   openconfig;
  et_bridgeconfig bconfig;
  et_sys_id       id_from;
  et_sys_id       id_to;
  et_att_id       att_from;
  et_att_id       att_to;
  et_stat_id      stat_from;
  et_stat_id      stat_to;

  pthread_t       id;
  int             exit;

} ET_priv;

static ET_priv ETP;





/*static*/extern objClass localobject;
extern char configname[128]; /* coda_component.c */
extern char *session; /* coda_component.c */
#define ET_ERROR 1
#define ET_OK 0

/*
int listSplit1(char *list, int flag,
           int *argc, char argv[LISTARGV1][LISTARGV2]);
*/


/* Define time limit for staying in write loop (seconds) */
#define ET_WRITE_LOOP_TIMEOUT   10



#define MAXBUF 100000
static unsigned int hpsbuf[MAXBUF];

/****************************************************************************/
/***************************** tcpServer functions **************************/

static int tcpState = DA_UNKNOWN;

void
rocStatus()
{
  /*
  printf("%d \n",tcpState);
  */
  switch(tcpState)
  {
    case DA_UNKNOWN:
      printf("unknown\n");
      break;
    case DA_BOOTING:
      printf("booting\n");
      break;
    case DA_BOOTED:
      printf("booted\n");
      break;
    case DA_CONFIGURING:
      printf("initing\n");
      break;
    case DA_CONFIGURED:
      printf("initied\n");
      break;
    case DA_DOWNLOADING:
      printf("loading\n");
      break;
    case DA_DOWNLOADED:
      printf("loaded\n");
      break;
    case DA_PRESTARTING:
      printf("prestarting\n");
      break; 
    case DA_PAUSED:
      printf("paused\n");
      break;
    case DA_PAUSING:
      printf("pausing\n");
      break;
    case DA_ACTIVATING:
      printf("activating\n");
      break;
    case DA_ACTIVE:
      printf("active\n");
      break;
    case DA_ENDING:
      printf("ending\n");
      break;
    case  DA_VERIFYING:
      printf("verifying\n");
      break;
    case DA_VERIFIED:
      printf("verified\n");
      break;
    case DA_TERMINATING:
      printf("terminating\n");
      break;
    case DA_PRESTARTED:
      printf("prestarted\n");
      break;
    case DA_RESUMING:
      printf("resuming\n");
      break;
    case DA_STATES:
      printf("states\n");
      break;
    default:
      printf("unknown\n");
  }
}


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#define NUMEVENTS 400


/* our version of 'et_start' */
int
et2etStart()
{
  objClass object = localobject;
  ET_priv *etp = (ET_priv *) object->privated;

  size_t hostlen = ET_FILENAME_LENGTH - 1;
  char *ch;

  struct timespec btime = {1, 0}; /* 1 sec */

  int status, ntransferred = 0;
  MYSQL *dbsock;
  char tmp[1000], tmpp[1000];

  printf("et2etStart reached\n");

  printf("\n0 etp->exit = %d\n\n",etp->exit);

  /************************************/
  /* default configuration parameters */
  /************************************/
  etp->nevents = 4000;      /* total number of events */
  etp->event_size = 500000; /* size of event in bytes */

  etp->serverPort = 11111;  /* server port number, unique for every ET system on the same machine */
                            /* will NOT use that value, will get free port and put it into database */

  etp->udpPort = 0;
  etp->deleteFile = 1;
  etp->ngroups = 1;    
  etp->noDelay = 0;
  etp->maxNumStations = 0;
  etp->sendBufSize = 0;
  etp->recvBufSize = 0;
  etp->et_verbose = ET_DEBUG_NONE;




  /***************************************************/
  /* extract all necessary information from database */

  /* connect to database */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));


  /* get 'from' info */
  sprintf(tmp,"SELECT inputs FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmp, tmpp)==ET_ERROR)
  {
    printf("coda_et2et: ERROR in mysql query >%s<\n",tmp);
  }
  else
  {
    printf("inputs >%s<\n",tmpp);
  }

  ch = strchr(tmpp,':'); /* get pointer to the location of ':' */
  if(ch==NULL)
  {
    printf("wrong arguments in 'from' - exit\n");
    exit(0);
  }
  ch[0] = '\0'; /* replace ':' by the end of string */
  strcpy(etp->host_from,tmpp);
  strcpy(etp->et_from,(ch+1));
  printf("host_from >%s<, et_from >%s<\n",etp->host_from,etp->et_from);


  /* get 'to' info */
  sprintf(tmp,"SELECT outputs FROM %s WHERE name='%s'",configname,object->name);
  if(dbGetStr(dbsock, tmp, tmpp)==ET_ERROR)
  {
    printf("cannot get 'outputs' from table>%s< for the name>%s<\n",configname,object->name);
    return(ET_ERROR);
  }
  else
  {
    printf("outputs >%s<\n",tmpp);
  }
  ch = strchr(tmpp,':'); /* get pointer to the location of ':' */
  if(ch==NULL)
  {
    printf("wrong arguments in 'to' - exit\n");
    exit(0);
  }
  ch[0] = '\0'; /* replace ':' by the end of string */
  strcpy(etp->host_to,tmpp);
  strcpy(etp->et_to,(ch+1));
  printf("host_to >%s<, et_to >%s<\n",etp->host_to,etp->et_to);


  /* disconnect from database */
  dbDisconnect(dbsock);


  /* station 'from' */
  strcpy(etp->station,"ET2ET");




  /* init ET configuration will be used for both ET systems */
  et_open_config_init(&etp->openconfig);


  /*****************/
  /* open 'from' ET */
  /*****************/

  et_open_config_sethost(etp->openconfig, etp->host_from);
  et_open_config_gethost(etp->openconfig, ch);
  printf("host_from >%s<\n",ch);
  /*et_open_config_setport(openconfig, 12345);*/


  /*sergey: increase tcp buffersize 
  {
	int rBufSize;
    int sBufSize;
    int noDelay;

	et_open_config_gettcp(openconfig, &rBufSize, &sBufSize, &noDelay);
    printf("default rBufSize=%d, sBufSize=%d, noDelay=%d\n", rBufSize, sBufSize, noDelay);

	rBufSize = 65000;
    sBufSize = 65000;
    noDelay = 0;
	et_open_config_settcp(openconfig, rBufSize, sBufSize, noDelay);

	et_open_config_gettcp(openconfig, &rBufSize, &sBufSize, &noDelay);
    printf("et2et: set rBufSize=%d, sBufSize=%d, noDelay=%d\n", rBufSize, sBufSize, noDelay);
  }
*/

  if(et_open(&etp->id_from, etp->et_from, etp->openconfig) != ET_OK)
  {
    printf("et_open 'from' problems\n");
    exit(1);
  }


  /******************/
  /* open 'to' ET */
  /******************/

  /*!!!!!!!!!!!!!!!! ET_HOST_ANYWHERE works, to_node does not !!!!!!!!*/
  et_open_config_sethost(etp->openconfig, etp->host_to/*ET_HOST_ANYWHERE*/);
  et_open_config_gethost(etp->openconfig, ch);
  printf("host_to >%s<\n",ch);

  /* Always use 'direct' connection: you can connect to any subnet,
     but only one ET system allowed on remote machine */
  et_open_config_setcast(etp->openconfig, ET_DIRECT);






  /*sergey: increase tcp buffersize 
  {
	int rBufSize;
    int sBufSize;
    int noDelay;

	et_open_config_gettcp(openconfig, &rBufSize, &sBufSize, &noDelay);
    printf("default rBufSize=%d, sBufSize=%d, noDelay=%d\n", rBufSize, sBufSize, noDelay);

	rBufSize = 65000;
    sBufSize = 65000;
    noDelay = 0;
	et_open_config_settcp(openconfig, rBufSize, sBufSize, noDelay);

	et_open_config_gettcp(openconfig, &rBufSize, &sBufSize, &noDelay);
    printf("et2et: set rBufSize=%d, sBufSize=%d, noDelay=%d\n", rBufSize, sBufSize, noDelay);
  }
*/


  /*
  {
    et_open_config *config = (et_open_config *) openconfig;
    printf("befor et_open: rBufSize=%d, sBufSize=%d, noDelay=%d\n",
      config->tcpSendBufSize,config->tcpRecvBufSize,config->tcpNoDelay);
  }
  */


  if(et_open(&etp->id_to, etp->et_to, etp->openconfig) != ET_OK)
  {
    printf("et_open 'to' problems\n");
    exit(1);
  }

  {
    et_id *idto = (et_id *) etp->id_to;
    printf("11111: idto->endian=0x%08x idto->systemendian=0x%08x\n",idto->endian,idto->systemendian);
  }

printf("11\n");fflush(stdout);
  /* destroy configuration */
  et_open_config_destroy(etp->openconfig);

  /* init station configuration */
  et_station_config_init(&etp->sconfig);
  et_station_config_setuser(etp->sconfig, ET_STATION_USER_MULTI);
  et_station_config_setrestore(etp->sconfig, ET_STATION_RESTORE_OUT);
  et_station_config_setprescale(etp->sconfig, 1);
  et_station_config_setcue(etp->sconfig, 150);
 
printf("12\n");fflush(stdout);

  /* ET system "all" mode */
  et_station_config_setselect(etp->sconfig, ET_STATION_SELECT_ALL);
  et_station_config_setblock(etp->sconfig, ET_STATION_BLOCKING);

  /* ET system "on req" mode 
  et_station_config_setselect(etp->sconfig, ET_STATION_SELECT_ALL);
  et_station_config_setblock(etp->sconfig, ET_STATION_NONBLOCKING);
  */
  /* ET system "condition" mode 
  et_station_config_setselect(etp->sconfig, ET_STATION_SELECT_MATCH);
  et_station_config_setblock(setp->config, ET_STATION_BLOCKING);
  et_station_config_setselectwords(etp->sconfig, selections);
  */
  /* new non-blocking "condition" mode 
  et_station_config_setselect(etp->sconfig, ET_STATION_SELECT_MATCH);
  et_station_config_setblock(etp->sconfig, ET_STATION_NONBLOCKING);
  et_station_config_setselectwords(etp->sconfig, selections);
  */
  /* user's condition, blocking  mode 
  et_station_config_setselect(etp->sconfig, ET_STATION_SELECT_USER);
  et_station_config_setblock(etp->sconfig, ET_STATION_BLOCKING);
  et_station_config_setselectwords(etp->sconfig, selections);
  if (et_station_config_setfunction(etp->sconfig, "et_carls_function") == ET_ERROR) {
	printf("%s: cannot set function\n", argv[0]);
	exit(1);
  }
  if (et_station_config_setlib(etp->sconfig, "/home/timmer/cvs/coda/source/et/src/libet_user.so") == ET_ERROR) {
    printf("%s: cannot set library\n", argv[0]);
	exit(1);
  }
  */
  /* user's condition, nonblocking mode 
  et_station_config_setselect(etp->sconfig, ET_STATION_SELECT_USER);
  et_station_config_setblock(etp->sconfig, ET_STATION_NONBLOCKING);
  et_station_config_setselectwords(etp->sconfig, selections);
  et_station_config_setfunction(etp->sconfig, "et_carls_function");
  et_station_config_setlib(etp->sconfig, "/home/timmer/cvs/coda/source/et/src/libet_user.so");
  */
  
  /* set debug level */
  et_system_setdebug(etp->id_from, ET_DEBUG_INFO);
  et_system_setdebug(etp->id_to,   ET_DEBUG_INFO);

  if ((status = et_station_create(etp->id_from, &etp->stat_from, etp->station, etp->sconfig)) < ET_OK)
  {
    if (status == ET_ERROR_EXISTS)
    {
      /* my_stat contains pointer to existing station */;
      printf("station already exists, will attach to it\n");

      /* get id and attach to existing station (must be created by 'et_start' */
      if((status = et_station_name_to_id(etp->id_from, &etp->stat_from, etp->station)) < ET_OK)
      {
        printf("error in station_name_to_id\n");
        exit(0);
      }
    }
    else
    {
      printf("error in station creation\n");
      exit(0);
    }
  }

  et_station_config_destroy(etp->sconfig);
printf("13\n");fflush(stdout);





  /* */
  if (et_station_attach(etp->id_from, etp->stat_from, &etp->att_from) < 0)
  {
    printf("error in station attach\n");
    exit(0);
  }
printf("14\n");fflush(stdout);

  if (et_station_attach(etp->id_to, ET_GRANDCENTRAL, &etp->att_to) < 0)
  {
    printf("error in station attach\n");
    exit(0);
  }
printf("15\n");fflush(stdout);

  et_bridge_config_init(&etp->bconfig);

  /*et_bridge_config_setfunc(etp->bconfig, et_bridge_CODAswap);*/


  /* set timeouts - does not do anything ???
  if (et_bridge_config_settimeoutfrom(etp->bconfig, btime) < 0)
  {
    printf("error et_bridge_config_settimeoutfrom\n");
    exit(0);
  }

  if (et_bridge_config_settimeoutto(etp->bconfig, btime) < 0)
  {
    printf("error et_bridge_config_settimeoutto\n");
    exit(0);
  }
  */

  printf("1 etp->exit = %d\n",etp->exit);
  printf("1 etp->exit = %d\n",etp->exit);
  printf("1 etp->exit = %d\n",etp->exit);

  /* infinite loop */
  status = ET_OK;
  while((status == ET_OK) && (etp->exit == 0))
  {
	printf("-> status=%d etp->exit=%d\n",status,etp->exit);
    status = et_events_bridge(etp->id_from, etp->id_to, etp->att_from, etp->att_to,
			      etp->bconfig, NUMEVENTS, &ntransferred);
  }

  printf("2 etp->exit = %d\n",etp->exit);
  printf("2 etp->exit = %d\n",etp->exit);
  printf("2 etp->exit = %d\n",etp->exit);

  /* cleanup */
  et_bridge_config_destroy(etp->bconfig);
  et_forcedclose(etp->id_from);
  et_forcedclose(etp->id_to);

  etp->exit = 0;

  return(ET_OK);
}


int
et2etConstructor()
{
  localobject->privated = (void *) &ETP;
  bzero ((char *) &ETP,sizeof(ETP));

  objClass object = localobject;
  ET_priv *etp = (ET_priv *) object->privated;

  int id, res;
  pthread_attr_t attr;

  printf("et2etConstructor reached\n");

  etp->id = 0;
  etp->exit = 0;

  tcpState = DA_BOOTED;
  if(codaUpdateStatus("booted") != ET_OK) return(ET_ERROR);

  tcpServer(localobject->name); /*start server to process non-coda commands sent by tcpClient*/

  return(ET_OK);
}


int
et2etDestructor()
{
  objClass object = localobject;
  ET_priv *etp = (ET_priv *) object->privated;
}


/*********************************************************/
/*********************************************************/
/*********************************************************/


int
codaInit(char *conf)
{
  if(codaUpdateStatus("configuring") != CODA_OK)
  {
    return(CODA_ERROR);
  }

  UDP_start();

  if(codaUpdateStatus("configured") != CODA_OK)
  {
    return(CODA_ERROR);
  }

  return(CODA_OK);
}


int
codaDownload(char *conf)
{
  objClass object = localobject;
  ET_priv *etp = (ET_priv *) object->privated;

  static char tmp[1000];
  static char tmp2[1000];
  int  ix, status, ntransferred;  
  char *ch;
  int  listArgc;
  char listArgv[LISTARGV1][LISTARGV2];

  MYSQL *dbsock;
  char tmpp[1000];

  int res;
  pthread_attr_t attr;

  
  etp->object = object;

  printf("111\n");fflush(stdout);

  /*****************************/
  /*****************************/
  /* FROM CLASS (former conf1) */

  strcpy(configname,conf); /* Sergey: save CODA configuration name */
  printf("coda_et2et: configname = >%s<\n",configname);fflush(stdout);


  UDP_start();

  tcpState = DA_DOWNLOADING;
  if(codaUpdateStatus("downloading") != ET_OK) return(ET_ERROR);



  /*****************************/
  /*****************************/
  /*****************************/

  printf("INFO: Downloading configuration '%s'\n", configname);


  printf("etp->id = 0x%08x\n",etp->id);
  printf("etp->id = 0x%08x\n",etp->id);
  printf("etp->id = 0x%08x\n",etp->id);

  printf("etp->exit = %d\n",etp->exit);
  printf("etp->exit = %d\n",etp->exit);
  printf("etp->exit = %d\n",etp->exit);

  /* if not the first entry, exit et2etStart thread 
  if(etp->id != 0)
  {
    etp->exit = 1;
    while(etp->exit == 1)
    {
      printf("Waiting for et2etStart thread to exit ...\n");
      sleep(1);
    }
    printf("et2etStart thread to exited !\n");
    sleep(10);
  }
  */

  /*
  if(etp->id != 0)
  {
    printf("Sending kill request to et2etStart thread ...\n");
    pthread_kill(etp->id, 0);
  }
  sleep(10);
  */

  /* if the first entry, start et2etStart thread */
  if(etp->id == 0)
  {
    pthread_attr_init(&attr); /* initialize attr with default attributes */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    res = pthread_create(&etp->id, &attr, et2etStart, NULL);
    if(res!=0)
    {
      printf("ERROR: pthread_create(et2etStart) returned %d - exit\n",res);
      exit(-1);
    }
    else
    {
      printf("et2etStart thread started, etp->id = 0x%08x\n",etp->id);
    }
  }

  printf("coda_et2et: downloaded !!!\n");

  tcpState = DA_DOWNLOADED;
  if(codaUpdateStatus("downloaded") != ET_OK) return(ET_ERROR);

  return(ET_OK);
}


int
codaPrestart()
{
  objClass object = localobject;
  ET_priv *etp = (ET_priv *) object->privated;

  MYSQL *dbsock;
  char tmpp[1000];

  printf("INFO: Prestarting\n");

  /*
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));

  sprintf(tmpp,"SELECT runNumber FROM sessions WHERE name='%s'",session);
  if(dbGetInt(dbsock,tmpp,&(object->runNumber))==ET_ERROR) return(ET_ERROR);

  dbDisconnect(dbsock);
  */

  printf("INFO: prestarting,run %d, type %d\n",
    object->runNumber, object->runType);

  object->nevents = 0;
  object->nlongs = 0;

  tcpState = DA_PAUSED;
  codaUpdateStatus("paused");

  return(ET_OK);
}

int
codaGo()
{
  tcpState = DA_ACTIVE;
  codaUpdateStatus("active");
  return(ET_OK);
}

int
codaEnd()
{
  tcpState = DA_DOWNLOADED;
  codaUpdateStatus("downloaded");
  return(ET_OK);  
}

int
codaPause()
{
  tcpState = DA_PAUSED;
  codaUpdateStatus("paused");
  return(ET_OK);
}

int
codaExit()
{
  tcpState = DA_CONFIGURED;
  codaUpdateStatus("configured");
  /*
  UDP_reset();
  */
  return(ET_OK);
}


/****************/
/* main program */
/****************/

void
main (int argc, char **argv)
{
  CODA_Init(argc, argv);

  et2etConstructor();

  printf("\n\n\n et2etConstructor done, running CODA_Execute\n\n");

  /* CODA_Service ("ROC"); */
  CODA_Execute ();

  et2etDestructor(); /* never called ... */
}

#endif
