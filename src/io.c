#include "common.h"

// void move_cursor(int x)
// {
//     printf("\x1b[0;%dH", x);
// }

void insert_key_press(int key_press, Cmd *cmd, dir_info *d)
{

    switch (key_press)
    {

    case UP_ARROW:

        int l = get_file_len();
        if (d->index != l - 1)
        {
            d->index += 1;
            get_command_history(cmd, d);
        }
        break;
    case DOWN_ARROW:
        if (d->index > 0)
        {
            d->index -= 1;
            get_command_history(cmd, d);
        }
        break;
    case RIGHT_ARROW:
        if (cmd->c_pos < cmd->length)
        {
            write(STDOUT_FILENO, "\x1b[C", 3);
            cmd->c_pos += 1;
        }
        break;

    case LEFT_ARROW:
        if (cmd->c_pos > 0)
        {
            write(STDOUT_FILENO, "\x1b[D", 3);
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


void remove_char(Cmd *cmd, dir_info *dir)
{
    if (cmd->length > 0 && cmd->c_pos > 0)
    {
        cmd->length--;
        cmd->c_pos--;

        memmove(cmd->cont + cmd->c_pos, cmd->cont + cmd->c_pos + 1, cmd->length - cmd->c_pos);

        char *new_cont = realloc(cmd->cont, (cmd->length + 1) * sizeof(char));
        if (new_cont == NULL)
        {
            perror("Failed to allocate memory for cmd->cont");
            exit(EXIT_FAILURE);
        }
        cmd->cont = new_cont;

        cmd->cont[cmd->length] = '\0';

        // write(STDOUT_FILENO, "\x1b[2K", 4);
        write(STDOUT_FILENO, "\r", 1);

        if (dir && dir->cur_dir)
        {
            printf("%s > ", dir->cur_dir);
        }

        write(STDOUT_FILENO, cmd->cont, cmd->length);

        printf("\x1b[%dG", cmd->c_pos + (dir && dir->cur_dir ? strlen(dir->cur_dir) + 3 : 0));

        fflush(stdout);
    }
}

void addCharToBuffer(const char addedChar, Cmd *cmd, dir_info *dir)
{

    if (cmd == NULL)
    {
        perror("Null command pointer");
        return;
    }

    if (dir == NULL)
    {
        perror("Null directory info pointer");
        return;
    }

    int l = cmd->c_pos;

    char *new_cont = realloc(cmd->cont, (cmd->length + 2) * sizeof(char));
    if (new_cont == NULL)
    {
        perror("Allocation error");
        return;
    }

    cmd->cont = new_cont;

    memmove(cmd->cont + l + 1, cmd->cont + l, cmd->length - l + 1);

    cmd->cont[l] = addedChar;
    cmd->c_pos += 1;
    cmd->length += 1;
    cmd->cont[cmd->length] = '\0';
    write(STDOUT_FILENO, "\x1b[2K", 4);
    write(STDOUT_FILENO, "\r", 1);

    if (dir->cur_dir)
    {
        printf("%s > ", dir->cur_dir);
    }

    write(STDOUT_FILENO, cmd->cont, cmd->length);

    printf("\x1b[%dG", cmd->c_pos + (dir->cur_dir ? strlen(dir->cur_dir) + 4 : 0));

}