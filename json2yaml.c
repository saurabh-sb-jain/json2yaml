#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>

uint32_t max_depth = 0;

#define SPACE_SZ 4

void
json_print_leaf (json_t *value)
{
    json_type value_type;

    if (!value) {
        return;
    }
    value_type = json_typeof(value);
    switch (value_type) {
    case JSON_STRING:
        printf("%s\n", json_string_value(value));
        break;
    case JSON_INTEGER:
        printf("%d\n", json_integer_value(value));
        break;
    case JSON_REAL:
        printf("%f\n", json_real_value(value));
        break;
    case JSON_TRUE:
        printf("True\n");
        break;
    case JSON_FALSE:
        printf("False\n");
        break;
    case JSON_NULL:
        printf("Null\n");
        break;
    default:
        assert(false);
        break;
    }
}

void
json_flat_dump (json_t *root, int depth)
{
    const char *key = NULL;
    void *iter = NULL;
    json_type value_type, arr_elem_value_type;
    json_t *value = NULL;
    json_t *arr_val = NULL;
    int i = 0;
    uint32_t num_spaces = depth * SPACE_SZ;
    char *ptr = calloc(num_spaces + 1, sizeof(char));
    memset(ptr, ' ', num_spaces);

    if (depth > max_depth) {
        max_depth = depth;
    }

    iter = json_object_iter(root);
    while (iter) {
        key = json_object_iter_key(iter);
        value = json_object_iter_value(iter);
        value_type = json_typeof(value);
        printf("%s%s: ", ptr, key);

        switch (value_type) {
        case JSON_OBJECT:
            printf("\n");
            json_flat_dump(value, depth + 1);
            break;
        case JSON_ARRAY:
            for (i = 0; i < json_array_size(value); i++) {
                arr_val = json_array_get(value, i);
                arr_elem_value_type = json_typeof(arr_val);
                if (arr_elem_value_type == JSON_ARRAY ||
                    arr_elem_value_type == JSON_OBJECT) {
                    printf("\n");
                    printf("%s%s", ptr, "-");
                    printf("\n");
                    json_flat_dump(arr_val, depth + 1);
                } else {
                    if (i == 0) {
                        printf("\n");
                    }
                    printf("%s%s", ptr, "- ");
                    json_print_leaf(arr_val);
                }
            }
            break;
        case JSON_STRING:
        case JSON_INTEGER:
        case JSON_REAL:
        case JSON_TRUE:
        case JSON_FALSE:
        case JSON_NULL:
        default:
            json_print_leaf(value);
            break;
        }
        iter = json_object_iter_next(root, iter);
    }
    free(ptr);
}

int main (int argc, char **argv)
{
    json_t *root;
    json_error_t err;
    FILE *in;
    size_t flags = 0;

    in = fopen(argv[1], "r");
    if (!in) {
        printf("Error: %d opening file: %s, exiting.\n", errno, argv[1]);
        return errno;
    }

    root = json_loadf(in, flags, &err);
    if (!root) {
        printf("Error: (%d, %s, %s) parsing file at: (%d, %d, %d), exiting.\n",
               json_error_code(&err), err.text, err.source,
               err.line, err.column, err.position);
        return EINVAL;
    }

    json_flat_dump(root, 0);

    return 0;
}