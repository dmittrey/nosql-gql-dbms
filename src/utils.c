#include "../include/utils.h"

int compare_string_t(struct string_t str_1, struct string_t str_2) {
    if (str_1.count > str_2.count) {
        return 1;
    } else if (str_1.count < str_2.count) {
        return -2;
    } else {
        for (size_t i = 0; i < str_1.count; i++) {
            if (str_1.val[i] != str_2.val[i]) return -1;
        }
        return 0;
    }
};

