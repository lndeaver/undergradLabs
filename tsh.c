/*
 * tsh - A tiny shell program
 * Lauren Deaver
 * 
 * Handling SIGINT/SIGTSTP
 *      checking for a running foreground process and as long as there
 *      is one, sending the apprpriate signal to the running process
 * 
 * Handling SIGCHLD
 *      using waitpid with arguements -1 (to wait for any child process),
 *      &result (to get the status), and WNOHANG (returns immediately if
 *      no child exited so not waiting around)/ WUNTRACED (returns if a 
 *      child stopped). The last two arguements are used to check for stopped
 *      and terminated children.
 * 
 *      within the while loops, we check for the child's exit status as one
 *      of the following:
 *          WIFSTOPPED (for ctrl+Z) - as long as the running foreground
 *          process is the pid being checked, we update the suspended pid
 *          and the running pid global variables. Print the message, WUNTRACED/
 *          WSTOPSIG to know which signal caused the stop.
 *          
 *          WIFSIGNALED (for ctrl+C) - as long as running foregorund process
 *          is the pid being checked, we update the running pid global. Print
 *          message, WTERMSIG to know which signal caused the termination.
 * 
 *          WIFEXITED (for child exiting normally) - as long as the running
 *          process is the pid being checked, update the running pid global.
 * 
 *      when all is saidand done, we update the pid for the while loop using
 *      waitpid again.
 * 
 * Waiting for a foregorund job
 *      in the eval function we track whether a forked process is to be run in
 *      the background or not (with bg variable)
 * 
 *      use sigprocmask to set the signals to be blocked (which only updates the
 *      block list and does not create a new blocked list each time)
 * 
 *      if the job is not a background job, we call waitfg so we can properly
 *      wait for the foreground job to finish
 *          waitfg: uses sigsuspend which, given a mask, will suspend until a 
 *                  signal is delivered & causes a handler to run or terminate
 *                  the process.
 *                  
 *                  sigsuspend is used in a while loop that checks that the pid
 *                  that was passedis the same as the running pid (aka the 
 *                  foreground process)
 * 
 * builtin_cmd function
 *      checks for certain strings to be entered as the first arguement:
 *          "quit" - exits the shell
 *          "fg" - moves the saved suspended pid to be the current running process
 *          "&" - just checking to make sure that user isn't trying to send 
 *                something to be a background job when no command was given
 */

/*
 *******************************************************************************
 * INCLUDE DIRECTIVES
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 *******************************************************************************
 * TYPE DEFINITIONS
 *******************************************************************************
 */

typedef void handler_t (int);

/*
 *******************************************************************************
 * PREPROCESSOR DEFINITIONS
 *******************************************************************************
 */

// max line size 
#define MAXLINE 1024 
// max args on a command line 
#define MAXARGS 128

/*
 *******************************************************************************
 * GLOBAL VARIABLES
 *******************************************************************************
 */

// defined in libc
extern char** environ;   

// command line prompt 
char prompt[] = "tsh> ";

// for composings sprintf messages
char sbuf[MAXLINE];

// PID of the foreground process
volatile pid_t g_runningPid = 0;
// PID of the suspended process
volatile pid_t g_suspendedPid = 0; 

/*
 *******************************************************************************
 * FUNCTION PROTOTYPES
 *******************************************************************************
 */

int
parseline (const char* cmdline, char**argv);

void
sigint_handler (int sig);

void
sigtstp_handler (int sig);

void
sigchld_handler (int sig);

void
sigquit_handler (int sig);

void
unix_error (char* msg);

void
app_error (char* msg);

handler_t*
Signal (int signum, handler_t* handler);

void
eval (char *cmdline);

int
builtin_cmd (char**argv);

void
waitfg (pid_t pid);

/*
 *******************************************************************************
 * MAIN
 *******************************************************************************
 */

int
main (int argc, char** argv)
{
  /* Redirect stderr to stdout */
  dup2 (1, 2);

  /* Install signal handlers */
  Signal (SIGINT, sigint_handler);   /* ctrl-c */
  Signal (SIGTSTP, sigtstp_handler); /* ctrl-z */
  Signal (SIGCHLD, sigchld_handler); /* Terminated or stopped child */
  Signal (SIGQUIT, sigquit_handler); /* quit */

  char cmdline [MAXLINE];

  /* TODO -- shell goes here*/
  while(1)
  {
    printf("%s", prompt);
    fflush(stdout);

    fgets (cmdline, MAXLINE, stdin);
    
    if (feof(stdin))
    {
      exit(0);
    }
    //g_runningPid = 0;
    eval(cmdline);
  }
  /* Quit */
  exit (0);
}

/*
*  parseline - Parse the command line and build the argv array.
*
*  Characters enclosed in single quotes are treated as a single
*  argument.
*
*  Returns true if the user has requested a BG job, false if
*  the user has requested a FG job.
*/
int
parseline (const char* cmdline, char** argv)
{
  static char array[MAXLINE]; /* holds local copy of command line*/
  char* buf = array;          /* ptr that traverses command line*/
  char* delim;                /* points to first space delimiter*/
  int argc;                   /* number of args*/
  int bg;                     /* background job?*/

  strcpy (buf, cmdline);
  buf[strlen (buf) - 1] = ' ';  /* replace trailing '\n' with space*/
  while (*buf && (*buf == ' ')) /* ignore leading spaces*/
    buf++;

  /* Build the argv list*/
  argc = 0;
  if (*buf == '\'')
  {
    buf++;
    delim = strchr (buf, '\'');
  }
  else
  {
    delim = strchr (buf, ' ');
  }

  while (delim)
  {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' ')) /* ignore spaces*/
      buf++;

    if (*buf == '\'')
    {
      buf++;
      delim = strchr (buf, '\'');
    }
    else
    {
      delim = strchr (buf, ' ');
    }
  }
  argv[argc] = NULL;

  if (argc == 0) /* ignore blank line*/
    return 1;

  /* should the job run in the background?*/
  if ((bg = (*argv[argc - 1] == '&')) != 0)
  {
    argv[--argc] = NULL;
  }
  return bg;
}

/*
 *******************************************************************************
 * SIGNAL HANDLERS
 *******************************************************************************
 */

/*
*  sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
*      a child job terminates (becomes a zombie), or stops because it
*      received a SIGSTOP or SIGTSTP signal. The handler reaps all
*      available zombie children, but doesn't wait for any other
*      currently running children to terminate.
*/
void
sigchld_handler (int sig)
{
  int result;
  //get pid for any child process
  pid_t pid = waitpid (-1, &result, WNOHANG | WUNTRACED);

  while (pid > 0)
  {
    if (WIFSTOPPED(result)) 
    {
      if (g_runningPid == pid)
      {
       g_suspendedPid = pid;
       g_runningPid = 0;
      }
      printf("Job (%d) stopped by signal %d\n", pid, WSTOPSIG(result));
      fflush(stdout);
    }
    else if (WIFSIGNALED(result)) 
    {
      if (g_runningPid == pid)
      {
        g_runningPid = 0;
      }
      printf("Job (%d) terminated by signal %d\n", pid, WTERMSIG(result));
      fflush(stdout);
    } 
    else if (WIFEXITED(result))
    {
      if (g_runningPid == pid)
      {
        g_runningPid = 0;
      }
    }
    pid = waitpid (-1, &result, WNOHANG | WUNTRACED);
  }
 }

/*
 *  sigint_handler - The kernel sends a SIGINT to the shell whenever the
 *     user types ctrl-c at the keyboard.  Catch it and send it along
 *     to the foreground job.
 */
void
sigint_handler (int sig)
{
  if (g_runningPid)
  {
    kill(-(g_runningPid), SIGINT);
  }
}

/*
 *  sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *      the user types ctrl-z at the keyboard. Catch it and suspend the
 *      foreground job by sending it a SIGTSTP.
 */
void
sigtstp_handler (int sig)
{
  if (g_runningPid)
  {
    kill(-(g_runningPid), SIGTSTP);
  }
}

/*
 *******************************************************************************
 * HELPER ROUTINES
 *******************************************************************************
 */


/*
 * unix_error - unix-style error routine
 */
void
unix_error (char* msg)
{
  fprintf (stdout, "%s: %s\n", msg, strerror (errno));
  exit (1);
}

/*
*  app_error - application-style error routine
*/
void
app_error (char* msg)
{
  fprintf (stdout, "%s\n", msg);
  exit (1);
}

/*
*  Signal - wrapper for the sigaction function
*/
handler_t*
Signal (int signum, handler_t* handler)
{
  struct sigaction action, old_action;

  action.sa_handler = handler;
  sigemptyset (&action.sa_mask); /* block sigs of type being handled*/
  action.sa_flags = SA_RESTART;  /* restart syscalls if possible*/

  if (sigaction (signum, &action, &old_action) < 0)
    unix_error ("Signal error");
  return (old_action.sa_handler);
}

/*
*  sigquit_handler - The driver program can gracefully terminate the
*     child shell by sending it a SIGQUIT signal.
*/
void
sigquit_handler (int sig)
{
  printf ("Terminating after receipt of SIGQUIT signal\n");
  exit (1);
}
/***********************************************************************************************************/
/* 
*   eval - main routine that parses and interprets the command line
*/
void
eval (char *cmdline)
{
  int bg;
  char *argv[MAXARGS];
  char buff[MAXLINE];

  //Signal Blocks
  sigset_t mask, prev;
  sigemptyset(&mask);
  sigaddset(&mask, SIGCHLD);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGTSTP);

  //copy cmdline into argv
  strcpy(buff,cmdline);
  bg = parseline(buff, argv);
  //just hit enter key
  if (argv[0] == NULL)
  {
    return;
  } 
  if (!builtin_cmd(argv))
  //checkin built in commands
  {
	  //block here so not blocking where shouldnt
	  sigprocmask(SIG_BLOCK, &mask, &prev);
    //make a child
    pid_t pid = fork ();

    //child error check
    if (pid < 0)
    {
      fprintf(stderr, "fork error (%s)\n",
              strerror(errno));
      exit(-1);
    }
        
    //child forked off success
    if (pid == 0)
    {
      //set mask & group pid
      //how, set, oldset
      setpgid (0,0);
      sigprocmask(SIG_SETMASK, &prev, NULL);

      int command = execve (argv[0], &argv[0], environ);
      //command not ofund/exec errored out
      if (command < 0)
      {
        fprintf (stderr, "%s: Command not found\n", argv[0]);
        fflush(stdout);
        exit(-1);
      }
    } 

    if (!bg)
    {
    	g_runningPid = pid;
	}
    sigprocmask(SIG_SETMASK, &prev, NULL);
    //parent wait foreground
    if (!bg)
    {
      waitfg(g_runningPid);
    }
    //it's a bg job
    else
    {
      printf("(%d) %s", pid, cmdline);
      fflush(stdout);
    }
  }

}

/*
*   recognize and interprets the built-in commands
*/
int
builtin_cmd(char**argv)
{
  if (!strcmp(argv[0], "&"))
  {
    return 1;
  }
  if (!strcmp(argv[0], "quit"))
  {
    exit(0);
  }

  if (!strcmp(argv[0], "fg"))
  {
    if (g_suspendedPid != 0)
    {
      g_runningPid = g_suspendedPid;
      kill(-(g_runningPid), SIGCONT);
      g_suspendedPid = 0;
      waitfg(g_runningPid);
    }
    return 1;
  }
  //only if not built in command
  return 0;
}

void
waitfg(pid_t pid)
{
  
  sigset_t mask;
  sigemptyset(&mask);
  // keep going until pid is not runningPid
  while (g_runningPid == pid)
  {
    sigsuspend(&mask);
  }
}
