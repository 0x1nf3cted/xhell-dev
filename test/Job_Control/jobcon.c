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

Job createJob(const char *command)
{
    // Fork a new process
    pid_t childPID = fork();

    if (childPID == -1)
    {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (childPID == 0)
    {
        // This is the child process
        execlp("/bin/sh", "sh", "-c", command, NULL);
        // If execlp() fails
        perror("Exec failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // This is the parent process
        newJob.pid = childPID;
        // Assign a job ID (you may implement this according to your needs)
        newJob.job_id = jobCounter++;
        // Copy the command to the job structure
        strncpy(newJob.command, command, MAX_INPUT - 1);
        newJob.command[MAX_INPUT - 1] = '\0'; // Ensure null-terminated
    }

    return newJob;
}

