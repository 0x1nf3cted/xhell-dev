/**
 * @file jobcon.c
 * @brief Job control function definitions
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
#include "include/jobcon.h"

/**
 * @brief Find the active job with the indicated process group ID (pgid).
 *
 * This function searches for an active job with the specified process group ID.
 *
 * @param pgid The process group ID to search for.
 * @return A pointer to the found job or NULL if no matching job is found.
 */
job *find_job(pid_t pgid)
{
    job *j;

    for (j = first_job; j; j = j->next)
        if (j->pgid == pgid)
            return j;
    return NULL;
}

/**
 * @brief Check if all processes in the job have stopped or completed.
 *
 * This function returns true if all processes in the given job have either stopped or completed.
 *
 * @param j The job to check.
 * @return 1 if all processes are stopped or completed, 0 otherwise.
 */
int job_is_stopped(job *j)
{
    process *p;

    for (p = j->first_process; p; p = p->next)
        if (!p->completed && !p->stopped)
            return 0;
    return 1;
}

/**
 * @brief Check if all processes in the job have completed.
 *
 * This function returns true if all processes in the given job have completed.
 *
 * @param j The job to check.
 * @return 1 if all processes are completed, 0 otherwise.
 */
int job_is_completed(job *j)
{
    process *p;

    for (p = j->first_process; p; p = p->next)
        if (!p->completed)
            return 0;
    return 1;
}

/**
 *
 *
 * @brief Launching Jobs with Job Control in a Shell.
 * @link<https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html>
 *
 * Once the shell assumes responsibility for job control on its controlling terminal,
 * it can initiate jobs in response to user commands.
 *
 * @par Process Creation with fork and exec
 * Processes in a process group are created using fork and exec functions. Care must be taken
 * due to multiple child processes, ensuring the correct order to avoid race conditions.
 *
 * @par Parent-Child Relationship Structure
 * Two options exist for structuring the parent-child relationships among processes:
 * all processes in the group as children of the shell or one process as the ancestor of others.
 * The former is simpler for bookkeeping.
 *
 * @par setpgid Function
 * Each process, when forked, should place itself in the new process group using the setpgid function.
 * The first process becomes the group leader, and its ID becomes the group ID.
 *
 * @par Timing Considerations
 * To address potential timing issues, the shell and each child process should call setpgid.
 * This ensures that all child processes are in the group before execution begins,
 * regardless of which process calls setpgid first.
 *
 * @par Foreground Job Handling
 * For foreground jobs, the new process group needs to be set as the foreground on the controlling terminal
 * using tcsetpgrp. Both the shell and its child processes should perform this to avoid race conditions.
 *
 * @par Signal Actions Resetting
 * Child processes should reset their signal actions. The shell, during initialization,
 * sets itself to ignore job control signals. Child processes should explicitly set signal actions back to SIG_DFL
 * after forking.
 *
 * @par Handling Stop Signals
 * Applications inheriting signal handling from the parent (shell) should not interfere with stop signals.
 * If applications disable normal interpretation of certain characters, they should provide an alternative mechanism
 * for the user to stop the job.
 *
 * @par exec Function Call
 * Each child process should call exec in the standard way. This is also the point for handling redirection
 * of standard input and output channels.
 *
 * @par Sample Shell Function
 * The provided shell program includes a function responsible for launching a program. This function is executed
 * by each child process immediately after being forked by the shell and does not return.
 */
void launch_process(process *p, pid_t pgid,
                    int infile, int outfile, int errfile,
                    int foreground)
{
    pid_t pid;

    if (shell_is_interactive)
    {
        /* Put the process into the process group and give the process group
           the terminal, if appropriate.
           This has to be done both by the shell and in the individual
           child processes because of potential race conditions.  */
        pid = getpid();
        if (pgid == 0)
            pgid = pid;
        setpgid(pid, pgid);
        if (foreground)
            tcsetpgrp(shell_terminal, pgid);

        /* Set the handling for job control signals back to the default.  */
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
    }

    /* Set the standard input/output channels of the new process.  */
    if (infile != STDIN_FILENO)
    {
        dup2(infile, STDIN_FILENO);
        close(infile);
    }
    if (outfile != STDOUT_FILENO)
    {
        dup2(outfile, STDOUT_FILENO);
        close(outfile);
    }
    if (errfile != STDERR_FILENO)
    {
        dup2(errfile, STDERR_FILENO);
        close(errfile);
    }

    /* Exec the new process.  Make sure we exit.  */
    execvp(p->argv[0], p->argv);
    perror("execvp");
    exit(1);
}

/**
 * @brief Handling Foreground Job Launch in the Shell.
 *
 * This section discusses the actions taken by the shell when launching a job into the foreground
 * and highlights the differences compared to launching a background job.
 *
 * @par Foreground Job Launch Steps:
 * - The shell grants access to the controlling terminal for the foreground job by calling tcsetpgrp.
 * - The shell then waits for processes in the newly created process group to terminate or stop.
 *   See Stopped and Terminated Jobs for more details.
 * - Once all processes in the group have completed or stopped, the shell regains control of the
 *   terminal for its own process group by calling tcsetpgrp again.
 *
 * @par Stop Signal Handling:
 * - Stop signals caused by I/O from a background process or a SUSP character typed by the user
 *   are sent to the process group. Normally, all processes in the job stop together.
 *
 * @par Terminal State Restoration:
 * - The foreground job may leave the terminal in an altered state, so the shell restores its own
 *   saved terminal modes before proceeding.
 * - In the event the job is merely stopped, the shell first saves the current terminal modes to
 *   facilitate restoration if the job is later continued.
 * - Terminal mode functions tcgetattr and tcsetattr are used for dealing with terminal modes;
 *   refer to Terminal Modes for more information.
 */
void foregroundJob(job *j, int cont)
{
    /* Put the job into the foreground.  */
    tcsetpgrp(shell_terminal, j->pgid);

    /* Send the job a continue signal, if necessary.  */
    if (cont)
    {
        tcsetattr(shell_terminal, TCSADRAIN, &j->tmodes);
        if (kill(-j->pgid, SIGCONT) < 0)
            perror("kill (SIGCONT)");
    }

    /* Wait for it to report.  */
    wait_for_job(j);

    /* Put the shell back in the foreground.  */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Restore the shell’s terminal modes.  */
    tcgetattr(shell_terminal, &j->tmodes);
    tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);
}

/**
 * @brief Launching a Process Group as a Background Job in the Shell.
 *
 * If the process group is launched as a background job, the shell should remain in the foreground itself
 * and continue to read commands from the terminal.
 *
 * @par Background Job Handling:
 * - In the sample shell, transitioning a job to the background requires minimal actions.
 *
 * @par Background Job Function:
 * - The shell provides a specific function to accomplish putting a job into the background.
 *   This function is responsible for handling the necessary steps to transition a job to the background.
 *   For detailed implementation, refer to the specific function in the sample shell.
 */
void backgroundJob(job *j, int cont)
{
    /* Send the job a continue signal, if necessary.  */
    if (cont)
        if (kill(-j->pgid, SIGCONT) < 0)
            perror("kill (SIGCONT)");
}

/**
 * @brief Handling Process Status in a Shell.
 *
 * When a foreground process is launched, the shell blocks until all processes in that job
 * have either terminated or stopped. The waitpid function with the WUNTRACED option is used
 * to achieve this, reporting status for both stopping and terminating processes.
 *
 * The shell also checks the status of background jobs to report terminated and stopped jobs
 * to the user. This is done by calling waitpid with the WNOHANG option, often placed just
 * before prompting for a new command.
 *
 * Asynchronous notification of available status information for a child process can be received
 * by establishing a handler for SIGCHLD signals, as detailed in Signal Handling.
 *
 * In the sample shell program, the SIGCHLD signal is typically ignored to avoid reentrancy issues
 * with global data structures. However, during specific times, such as when waiting for input
 * on the terminal, it makes sense to enable a handler for SIGCHLD. The same function
 * (do_job_notification) used for synchronous status checks can also be called from within this handler.
 *
 * Here are the relevant parts of the sample shell program dealing with checking the status of jobs
 * and reporting the information to the user.
 */
int mark_process_status(pid_t pid, int status)
{
    job *j;
    process *p;

    if (pid > 0)
    {
        /* Update the record for the process.  */
        for (j = first_job; j; j = j->next)
            for (p = j->first_process; p; p = p->next)
                if (p->pid == pid)
                {
                    p->status = status;
                    if (WIFSTOPPED(status))
                        p->stopped = 1;
                    else
                    {
                        p->completed = 1;
                        if (WIFSIGNALED(status))
                            fprintf(stderr, "%d: Terminated by signal %d.\n",
                                    (int)pid, WTERMSIG(p->status));
                    }
                    return 0;
                }
        fprintf(stderr, "No child process %d.\n", pid);
        return -1;
    }

    else if (pid == 0 || errno == ECHILD)
        /* No processes ready to report.  */
        return -1;
    else
    {
        /* Other weird errors.  */
        perror("waitpid");
        return -1;
    }
}

/**
 * @brief Checks for processes with status information without blocking.
 *
 * This function uses `waitpid` with the `WUNTRACED|WNOHANG` options to
 * check for processes with status information without blocking. It then
 * calls `mark_process_status` to update the status of the processes.
 */
void update_status(void)
{
    int status;
    pid_t pid;

    do
        pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
    while (!mark_process_status(pid, status));
}

/**
 * @brief Checks for processes with status information, blocking until all processes in the given job have reported.
 *
 * This function uses `waitpid` with the `WUNTRACED` option to wait for
 * processes to complete or stop. It calls `mark_process_status` to update
 * the status of the processes and waits until all processes in the given
 * job have reported.
 *
 * @param j The job for which to wait.
 */
void wait_for_job(job *j)
{
    int status;
    pid_t pid;

    do
        pid = waitpid(WAIT_ANY, &status, WUNTRACED);
    while (!mark_process_status(pid, status) && !job_is_stopped(j) && !job_is_completed(j));
}

/**
 * @brief Formats information about job status for user display.
 *
 * This function prints detailed information about the job, including the
 * process group ID, status (such as "completed" or "stopped"), and the
 * command that initiated the job.
 *
 * @param j The job for which to format information.
 * @param status The status of the job (e.g., "completed" or "stopped").
 */
void format_job_info(job *j, const char *status)
{
    fprintf(stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

/**
 * @brief Notifies the user about stopped or terminated jobs.
 * Deletes terminated jobs from the active job list.
 *
 * This function updates status information for child processes, notifies
 * the user about completed or stopped jobs, marks stopped jobs to avoid
 * duplicate notifications, and deletes completed jobs from the active job list.
 */
void job_notification(void)
{
    job *j, *jlast, *jnext;

    /* Update status information for child processes.  */
    update_status();

    jlast = NULL;
    for (j = first_job; j; j = jnext)
    {
        jnext = j->next;

        /* If all processes have completed, tell the user the job has
           completed and delete it from the list of active jobs.  */
        if (job_is_completed(j))
        {
            format_job_info(j, "completed");
            if (jlast)
                jlast->next = jnext;
            else
                first_job = jnext;
            free_job(j);
        }

        /* Notify the user about stopped jobs,
           marking them so that we won’t do this more than once.  */
        else if (job_is_stopped(j) && !j->notified)
        {
            format_job_info(j, "stopped");
            j->notified = 1;
            jlast = j;
        }

        /* Don’t say anything about jobs that are still running.  */
        else
            jlast = j;
    }
}

/**
 * @brief Marks a stopped job as being running again.
 *
 * This function takes a job structure and iterates through its processes,
 * setting the 'stopped' flag to 0 for each process. It also resets the
 * 'notified' flag for the job, indicating that it is no longer in a stopped state.
 *
 * @param j The job structure to be marked as running.
 */
void mark_job_as_running(job *j)
{
    Process *p;

    for (p = j->first_process; p; p = p->next)
        p->stopped = 0;
    j->notified = 0;
}

/**
 * @brief Continues a stopped job by sending a SIGCONT signal to its process group.
 *
 * This function marks the job as running using `mark_job_as_running` and then
 * either puts the job in the foreground or background based on the value of
 * the 'foreground' parameter. If the job is continued in the foreground, the
 * shell invokes `put_job_in_foreground` to give the job access to the terminal
 * and restore the saved terminal settings. If continued in the background,
 * `put_job_in_background` is used.
 *
 * @param j The job structure to be continued.
 * @param foreground A flag indicating whether the job should be continued in the foreground (1) or background (0).
 */
void continue_job(job *j, int foreground)
{
    mark_job_as_running(j);
    if (foreground)
        put_job_in_foreground(j, 1);
    else
        put_job_in_background(j, 1);
}