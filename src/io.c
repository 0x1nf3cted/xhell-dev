#include "common.h"

void move_cursor(int x)
{
    printf("\x1b[0;%dH", x);
}

void insert_key_press(int key_press, Cmd *cmd)
{

    switch (key_press)
    {

    case UP_ARROW:
        write(STDOUT_FILENO, "UP", 2);
        printf("UP");
        // get the last command
        break;
    case DOWN_ARROW:
        write(STDOUT_FILENO, "DOWN", 4);

        // get the next command
        break;
    case RIGHT_ARROW:
        if (cmd->c_pos < cmd->length) // Ensure cursor is not at the end of the string
        {
            write(STDOUT_FILENO, "\x1b[C", 3); // Move cursor right
            cmd->c_pos += 1;
        }
        break;

    case LEFT_ARROW:
        if (cmd->c_pos > 0) // Ensure cursor is not at the beginning of the string
        {
            write(STDOUT_FILENO, "\x1b[D", 3); // Move cursor left
            cmd->c_pos -= 1;
        }
        break;
    }
}

void insertChar(char addedChar, Cmd *cmd, dir_info *dir)
{
    int l;

    if (cmd->cont == NULL)
    {
        cmd->cont = (char *)malloc(sizeof(char));
    }
    switch (addedChar)
    {

    case BACKSPACE:
        if (cmd->length > 0)
        {
            cmd->cont[cmd->length - 1] = '\0';
            cmd->length -= 1;
            cmd->c_pos -= 1;

            printf("\b \b");
            fflush(stdout);
        }
        break;

    case TAB:
        write(STDOUT_FILENO, "tap", 3);
        break;
    default:
        addCharToBuffer(addedChar, cmd, dir);
        break;
    }
}

void addCharToBuffer(const char addedChar, Cmd *cmd, dir_info *dir)
{
    if (cmd == NULL)
    {
        perror("Null command pointer");
        return;
    }

    size_t l = cmd->length;

    // Directly reallocate cmd->cont
    cmd->cont = realloc(cmd->cont, (l + 2) * sizeof(char));
    if (cmd->cont == NULL)
    {
        perror("Allocation error");
        // Handle the error, e.g., by returning, but do not exit if you want to preserve the data.
        return;
    }

    cmd->cont[l] = addedChar;
    cmd->cont[l + 1] = '\0';
    cmd->length += 1;

    write(STDOUT_FILENO, "\x1b[2K", 4);
    write(STDOUT_FILENO, "\r", 1);

    if (dir->cur_dir)
    {
        printf("%s > ", dir->cur_dir);
    }
    write(STDOUT_FILENO, cmd->cont, cmd->length);

    cmd->c_pos = cmd->length;
}
