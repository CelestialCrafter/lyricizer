#pragma once

#include <stdbool.h>
#include "mpd.h"

void lrcify_path(char* new, const char* path);

void save_lyrics(const char* path, const char* response, song_info song);

