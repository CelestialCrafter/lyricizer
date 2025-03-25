#pragma once

#include <curl/curl.h>

#include "mpd.h"

void fetch_lyrics(CURL* curl, const char* instance, song_info song);

