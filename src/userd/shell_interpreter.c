#include "shell_interpreter.h"

static int str_eq_prefix(const char *a, const char *b)
{
    int i = 0;

    while (b[i])
    {
        if (a[i] != b[i])
            return 0;
        i++;
    }

    return 1;
}

static void copy_string(char *dst, const char *src, int max)
{
    int i = 0;

    if (!dst || !src || max <= 0)
        return;

    while (src[i] && i < max - 1)
    {
        dst[i] = src[i];
        i++;
    }

    dst[i] = 0;
}

void shell_interpret_command(const char *input, char *output, int output_size)
{
    if (!output || output_size <= 0)
        return;

    output[0] = 0;

    if (!input || !input[0])
    {
        copy_string(output, "Empty command.", output_size);
        return;
    }

    if (str_eq_prefix(input, "echo"))
    {
        const char *p = input + 4;

        while (*p == ' ')
            p++;

        if (!*p)
        {
            copy_string(output, "", output_size);
            return;
        }

        copy_string(output, p, output_size);
        return;
    }

    copy_string(output, "Unknown command. Try: echo hello", output_size);
}