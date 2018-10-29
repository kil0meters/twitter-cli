#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "strtools.h"

char *xdg_cache_home(void) {
    struct stat st = {0};

    char *cache_home = getenv("XDG_CACHE_CONFIG");
    if (cache_home == NULL) {
        cache_home = cat_strs(getenv("HOME"), "/.cache/twitter-cli");
    }
    if (stat(cache_home, &st) == -1)
        mkdir(cache_home, 0700);

    return cache_home;
}

char *xdg_config_home(void) {
    struct stat st = {0};

    char *config_home = getenv("XDG_CONFIG_HOME");
    if (config_home == NULL) {
        config_home = cat_strs(getenv("HOME"), "/.config/twitter-cli");
    }
    if (stat(config_home, &st) == -1)
        mkdir(config_home, 0700);

    return config_home;
}
