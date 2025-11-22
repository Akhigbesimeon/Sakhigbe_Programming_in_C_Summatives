#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <curl/curl.h>

// Data Structure
typedef struct {
    char *url;
    char *filename;
} ThreadData;

// Callback Function 
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Thread Function: Web Fetcher
void *fetch_webpage(void *threadarg) {
    ThreadData *data = (ThreadData *)threadarg;
    CURL *curl_handle;
    CURLcode res;
    FILE *fp = NULL;

    printf("Thread started for: %s\n", data->url);

    // Initialize libcurl
    curl_handle = curl_easy_init();
    if (curl_handle == NULL) {
        fprintf(stderr, "ERROR: Could not initialize cURL for %s\n", data->url);
        pthread_exit(NULL);
    }

    // Open output file for writing
    fp = fopen(data->filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Could not open file %s for writing.\n", data->filename);
        curl_easy_cleanup(curl_handle);
        pthread_exit(NULL);
    }

    // Set cURL options
    curl_easy_setopt(curl_handle, CURLOPT_URL, data->url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 10L); 

    // Perform the request
    res = curl_easy_perform(curl_handle);

    // Error Handling
    if (res != CURLE_OK) {
        fprintf(stderr, "ERROR: Failed to fetch %s: %s\n", 
                data->url, curl_easy_strerror(res));
    } else {
        printf("SUCCESS: Fetched %s and saved to %s\n", data->url, data->filename);
    }

    // Cleanup
    fclose(fp);
    curl_easy_cleanup(curl_handle);

    pthread_exit(NULL);
}

// Main Program 
int main(int argc, char *argv[]) {
    char *urls[] = {
        "http://example.com",
        "https://www.google.com/robots.txt",
        "https://httpbin.org/delay/5",
        "http://nonexistent-domain-123.com" 
    };
    int num_urls = sizeof(urls) / sizeof(urls[0]);

    // Array to hold thread IDs and their corresponding data
    pthread_t threads[num_urls];
    ThreadData thread_data[num_urls];
    int rc;

    printf("--- Multi-threaded Web Scraper Starting (%d threads) ---\n", num_urls);
    printf("All URLs will be fetched concurrently.\n\n");

    // Initialize cURL environment globally
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // 1. Create all threads
    for (int i = 0; i < num_urls; i++) {
        thread_data[i].url = urls[i];
        
        thread_data[i].filename = (char *)malloc(32 * sizeof(char));
        sprintf(thread_data[i].filename, "content_%d.txt", i);

        printf("Main: Creating thread %d for URL: %s\n", i, thread_data[i].url);

        rc = pthread_create(&threads[i], NULL, fetch_webpage, (void *)&thread_data[i]);
        
        if (rc) {
            fprintf(stderr, "ERROR: pthread_create() failed; return code: %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to complete (Join)
    for (int i = 0; i < num_urls; i++) {
        pthread_join(threads[i], NULL);
        free(thread_data[i].filename);
    }

    // Cleanup cURL environment
    curl_global_cleanup();

    printf("\n--- All threads completed. Main program exiting. ---\n");
    return 0;
}
