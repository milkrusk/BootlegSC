#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <bcm2835.h>
#include "epd2in13b.h"S

#define SPOTIFY_API_ENDPOINT "https://api.spotify.com/v1/me/player/currently-playing"
#define ACCESS_TOKEN "your_access_token_here"

static char song_name[256];
static char artist_name[256];
static char album_art_url[256];
static uint8_t album_art[4736];

void get_current_song_info() {
    CURL *curl;
    CURLcode res;

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set the Spotify API endpoint
        curl_easy_setopt(curl, CURLOPT_URL, SPOTIFY_API_ENDPOINT);

        // Set the HTTP headers (Authorization and Content-Type)
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Authorization: Bearer " ACCESS_TOKEN);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Make the HTTP request
        res = curl_easy_perform(curl);

        // If the request was successful, parse the JSON response
        if (res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if (response_code == 200) {
                char *response_data;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_data);

                cJSON *response_json = cJSON_Parse(response_data);

                cJSON *item = cJSON_GetObjectItemCaseSensitive(response_json, "item");
                cJSON *name = cJSON_GetObjectItemCaseSensitive(item, "name");
                cJSON *artists = cJSON_GetObjectItemCaseSensitive(item, "artists");
                cJSON *artist = cJSON_GetArrayItem(artists, 0);
                cJSON *images = cJSON_GetObjectItemCaseSensitive(item, "album");
                cJSON *image = cJSON_GetObjectItemCaseSensitive(images, "images");
                cJSON *url = cJSON_GetArrayItem(image, 0);

                if (cJSON_IsString(name)) {
                    strncpy(song_name, name->valuestring, 255);
                }
                if (cJSON_IsString(artist)) {
                    strncpy(artist_name, artist->valuestring, 255);
                }
                if (cJSON_IsString(url)) {
                    strncpy(album_art_url, url->valuestring, 255);
                }

                // Clean up JSON object
                cJSON_Delete(response_json);
            }
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // Clean up CURL resources
    curl_global_cleanup();
}

void get_album_art() {
    CURL *curl;
    CURLcode res;

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set the album art URL
        curl_easy_setopt(curl, CURLOPT_URL, album_art_url);

        // Make the HTTP request
        res = curl_easy_perform(curl);

        // If the request was successful, read the image data
        if (res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if (response_code == 200) {
                char *response_data;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_data);

                //
            // Get the image data
            size_t response_size;
            curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &response_size);
            uint8_t *image_data = malloc(response_size);
            memcpy(image_data, response_data, response_size);

            // Resize and convert the image data to match the display resolution
            resize_image(image_data, response_size, album_art, sizeof(album_art));

            // Free the image data
            free(image_data);
        }
    }

    // Clean up
    curl_easy_cleanup(curl);
}

// Clean up CURL resources
curl_global_cleanup();
