#pragma once

#include <curl/curl.h>

#include "mpd.h"

char* fetch_lyrics(CURL* curl, const char* instance, song_info song);

