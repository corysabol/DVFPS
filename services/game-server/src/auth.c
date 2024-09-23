#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "auth.h"

#define API_URL "http://auth-service/validate_token"

typedef struct {
    char username[50];
    char token[256];
} Player;

Player players[6]; // 3 v 3

typedef struct {
    char *data;
    size_t size;
} APIResponse;

// func to handle response from API
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    APIResponse *res = (APIResponse *)userp;

    // reallocate to handle the response
    char *ptr = realloc(res->data, res->size + totalSize + 1);
    if (ptr == NULL) {
        // out of memory
        printf("Error: out of memory while processing API response.\n");
        return 0;
    }

    res->data = ptr;
    memcpy(&(res->data[res->size]), contents, totalSize);
    res->size += totalSize;
    res->data[res->size] = 0;

    return totalSize;
}

// func to validate the player's token using the rest API
int validate_player_token(const char *token) {
    CURL *curl;
    CURLcode res;
    APIResponse response = { .data = NULL, .size = 0 };

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        printf("Error: failed to initialize curl.\n");
        return 0;
    }

    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    char postFields[300];
    sprintf(postFields, "token=%s", token);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // do http request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("Error: failed to make API request: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 0;
    }

    // api returns "valid" or "invalid"
    int isValid = 0;
    if (response.data != NULL) {
        if (strstr(response.data, "valid") != NULL) {
            isValid = 1;
        }
        free(response.data);
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return isValid;
}

int check_player_auth(int playerId) {
    if (playerId < 0 || playerId >= 6 || strlen(players[playerId].username) == 0) {
        printf("Error: invalid player id.\n");
        return 0;
    }

    return validate_player_token(players[playerId].token);
}

void add_player(char *username, char *token) {
    for (int i = 0; i < 6; i++) {
        if (strlen(players[i].username) == 0) {
            strcpy(players[i].username, username);
            strcpy(players[i].token, token);
            return;
        }
    }
    printf("Error: lobby is full.\n");
}