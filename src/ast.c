#include "common.h"

 

Node *create_node(const char *content, enum NodeType type)
{
    Node *node = malloc(sizeof(Node));
    if (!node)
    {
        return NULL;  
    }
    node->node_content = strdup(content);
    node->node_type = type;
    node->next = NULL;
    node->before = NULL;
     return node;
}

enum NodeType determine_node_type(const char *token)
{
    if (strcmp(token, "|") == 0)
    {
        return PIPE_NODE;
    }
    else if (token[0] == '-')
    {
        return ARG_NODE;
    }
    else if (strchr(token, '/') != NULL)
    {
        return PATH_NODE;
    }
    else if (isalpha(token[0]))
    {
        return COMMAND_NODE;
    }
    else
    {
        return SYMBOL_NODE;  
    }
}
Node *parse_command_node(Cmd *cmd, int *nb_token)
{
    if (cmd == NULL || cmd->cont == NULL || nb_token == NULL)
    {
        return NULL;
    }

    Node *head = NULL, *current = NULL;

     char *cmd_copy = strdup(cmd->cont);
    if (cmd_copy == NULL)
    {
        return NULL; 
    }

    char *token = strtok(cmd_copy, " ");  
    while (token != NULL)
    {
        enum NodeType type = determine_node_type(token);
        Node *new_node = create_node(token, type);
        if (!new_node)
        {
            free(cmd_copy); 
 
            return NULL;
        }

        if (head == NULL)
        {
            head = new_node;
        }
        else
        {
            current->next = new_node;
            new_node->before = current;
        }

        current = new_node;
        (*nb_token)++;
        token = strtok(NULL, " ");
    }

    free(cmd_copy); 
    return head;
}

void print_ast(Node *current)
{
    while (current != NULL)
    {
        printf("Content: %s\n", current->node_content);
        current = current->next;
    }
}