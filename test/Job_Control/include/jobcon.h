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

#define MAX_JOBS 10
#define MAX_INPUT 100

static int jobCounter;

typedef struct
{
    pid_t pid;
    int job_id;
    char command[MAX_INPUT];
} Job;

/**
 * @brief Creates a new job.
 *
 * @param command The command to be executed.
 * @return Job containing PID, Job ID, and the command.
 */
Job createJob(const char* command);

/**
 * @brief Runs a process as a foreground job.
 *
 * @param command The command to be executed.
 * @return Job representing the foreground job.
 */
Job foregroundJob(const char* command);

/**
 * @brief Runs process as background job
 *
 * @param command
 * @return Job
 */
Job backgroundJob(const char *command);

/**
 * @brief Handles signals related to child processes (jobs).
 *
 * @param receivedSignal The signal received.
 * @param signal_info Information about the signal.
 * @param unused_data Unused data parameter.
 */
void handleSignal(int receivedSignal, siginfo_t *signal_info, void *unused_data);

/**
 * @brief Handles INTERRUPT signals (CTRL+C)
 *
 * @param signal
 */
void handleSIGINT(int signal);

/**
 * @brief Handles STOP signals (CTRL+Z)
 *
 * @param signal
 */
void handleSIGTSTP(int signal);

/**
 * @brief Handles unstoppable STOP signals (CTRL+Z)
 *
 * @param signal
 */
void handleSIGSTOP(int signal);

/**
 * @brief Handles TERMINATE signals
 *
 * @param command
 */
void handleSIGTERM(const char *command);

/**
 * @brief Handle CONTINUE signals
 *
 * @param command
 */
void handleSIGCONT(const char *command);

/**
 * @brief Handle immediate KILL signals
 *
 * @param command
 */
void handleSIGKILL(const char *command);

/**
 * @brief Handles HANGUP signals
 *
 * @param command
 */
void handleSIGHUP(const char *command);

#endif /* jobcon_h */