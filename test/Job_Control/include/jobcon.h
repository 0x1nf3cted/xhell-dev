/**
 * @file jobcon.h
 * @brief Job control prototype functions
 *
 * The Xhell program requires job control, so we must implement a
 * minimum foundation API for other devs of Xhell and ArtilleryOS
 *
 * @author Lukas R. Jackson
 * @date December 29th, 2023
 *
 * @details
 * Function prototypes, Types, possible enums
 *
 * @license
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef jobcon_h
#define jobcon_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <termios.h>

/* A process is a single process */
typedef struct process
{
  struct process *next; /* next process in pipeline */
  char **argv;          /* for exec */
  pid_t pid;            /* process ID */
  char completed;       /* true if process has completed */
  char stopped;         /* true if process has stopped */
  int status;           /* reported status value */
} process;

/* A job is a pipeline of processes.  */
typedef struct job
{
  struct job *next;          /* next active job */
  char *command;             /* command line, used for messages */
  process *first_process;    /* list of processes in this job */
  pid_t pgid;                /* process group ID */
  char notified;             /* true if user told about stopped job */
  struct termios tmodes;     /* saved terminal modes */
  int stdin, stdout, stderr; /* standard i/o channels */
} job;

/* The active jobs are linked into a list. Below is the head. (Of the linked list) */
job *first_job = NULL;

/* Functions to perform operations on job objects */

/**
 * @brief Finds a job
 *
 * @param pgid
 * @return job*
 */
job *find_job(pid_t pgid);

/**
 * @brief Returns true if all processes in the job are complete or stopped
 *
 * @param j
 * @return int
 */
int job_is_stopped(job *j);

/**
 * @brief Returns true if all processes in a job are completed
 *
 * @param j
 * @return int
 */
int job_is_completed(job *j);

/**
 * @brief Launches a process
 *
 * @param p
 * @param pgid
 * @param infile
 * @param outfile
 * @param errfile
 * @param foreground
 */
void launch_process(process *p, pid_t pgid,
                    int infile, int outfile, int errfile,
                    int foreground);

/**
 * @brief Runs a process as a foreground job.
 *
 * @param command The command to be executed.
 */
void foregroundJob(job *j, int cont);

/**
 * @brief Runs process as background job
 *
 * @param command
 */
void backgroundJob(job *j, int cont);

/**
 * @brief Store the status of the pid that was returned by waitpid
 *
 * @param pid
 * @param status
 * @return int
 */
int mark_process_status(pid_t pid, int status);

/**
 * @brief check for statuses for information without blocking
 *
 */
void update_status(void);

/**
 * @brief Waits for specific job to complete and blocks all processes
 *
 * @param j
 */
void wait_for_job(job *j);

/**
 * @brief Formats job into and displays it
 * 
 * @param j 
 * @param status 
 */
void format_job_info(job *j, const char *status);

/**
 * @brief Notifies user about stopped or temrinated jobs
 * 
 */
void job_notification(void);

/**
 * @brief Marks a job as running again
 *
 * @param j
 */
void mark_job_as_running (job *j);

/**
 * @brief Continue a job
 * 
 * @param j 
 * @param foreground 
 */
void continue_job(job *j, int foreground);

#endif /* jobcon_h */