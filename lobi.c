//
// Created by haniokasai on 2017/02/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>


//char csrf_token = '<input type="hidden" name="csrf_token" value="';
//char authenticity_token = '<input name="authenticity_token" type="hidden" value="';
//char redirect_after_login = '<input name="redirect_after_login" type="hidden" value="';
//char oauth_token = '<input id="oauth_token" name="oauth_token" type="hidden" value="';
//char twitter_redirect_to_lobi = '<a class="maintain-context" href="';

struct Buffer {
    char *data;
    int data_size;
};

struct curl_slist *headers = NULL;

size_t buffer_writer(char *ptr, size_t size, size_t nmemb, void *stream) {
    struct Buffer *buf = (struct Buffer *)stream;
    int block = size * nmemb;


    if (!buf) {
        return block;
    }

    if (!buf->data) {
        buf->data = (char *)malloc(block);
    }
    else {
        buf->data = (char *)realloc(buf->data, buf->data_size + block);
    }

    if (buf->data) {
        memcpy(buf->data + buf->data_size, ptr, block);
        buf->data_size += block;
    }

    if(buf->data == NULL){
        //i can get,but it is not correct way.
        //printf("%s",ptr);
        buf->data = ptr;
    }
    buf->data = ptr;
    return block;
}

char *http_get(char *url);
char *http_post(char *url,char *data);
char *get_csrf(char *src);


int main(void) {
    printf("%s","LobiNoticer are Loading....　\n");
    FILE *fp;
    fp = fopen("cookie.txt", "w+");
    if(fp==NULL){
        printf("cookie.txt writing error\n");
        return -1;
    }else{
        printf("cookie.txt wrote.\n");
    }
    fclose(fp);

    char *source;
    source=http_get("https://lobi.co/signin");
    printf("%s",source);
    char *csrf_token;
    //printf("a");
    csrf_token = get_csrf(source);
    //printf("b");
    printf("%s",csrf_token);
    //printf("c");
    char post_data[1000] = {'\0'};;
    char *mail = "";
    char *password="";
    //printf("d");
    char bar[512];
    strcat(bar, "csrf_token=" );
    strcat(bar,csrf_token);
    strcat(bar,"&email=");
    strcat(bar,mail);
    strcat(bar,"&password=");
    strcat(bar,password);
    //sprintf(post_data,bar,csrf_token,mail,password);
    //printf("%s",bar);
    char *posted = http_post("https://lobi.co/signin",bar);
    printf("\n\nc");
    printf("%s",posted);

    char *me = http_get("https://web.lobi.co/api/me?fields=premium");
    printf("%s",me);

    json_error_t error;
    json_t *result = json_loads(me, 0, &error);
    if (result == NULL) {
        fputs(error.text, stderr);
        goto leave;
    }

    leave:
    free(me);

    return 0;


}

char *http_get(char *url){
    CURL *curl;
    struct Buffer *buf;
    buf = (struct Buffer *)malloc(sizeof(struct Buffer));
    buf->data = NULL;
    buf->data_size = 0;

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

    headers = curl_slist_append(headers, "Connection: keep-alive");
    headers = curl_slist_append(headers, "Accept: application/json, text/plain, */*");
    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.110 Safari/537.36");
    headers = curl_slist_append(headers, "Accept-Language: ja,en-US;q=0.8,en;q=0.6");
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

    curl_easy_setopt(curl,CURLOPT_COOKIEFILE,"cookie.txt");
    curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"cookie.txt");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, buffer_writer);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return buf->data;
}

char *http_post(char *url,char *data){
//char *post_data = "name=edo&age=20";
    CURL *curl;
    struct Buffer *buf;
    buf = (struct Buffer *)malloc(sizeof(struct Buffer));
    buf->data = NULL;
    buf->data_size = 0;

    // curlのセットアップ
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

    headers = curl_slist_append(headers, "Connection: keep-alive");
    headers = curl_slist_append(headers, "Accept: application/json, text/plain, */*");
    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.110 Safari/537.36");
    headers = curl_slist_append(headers, "Accept-Language: ja,en-US;q=0.8,en;q=0.6");
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);

    curl_easy_setopt(curl,CURLOPT_COOKIEFILE,"cookie.txt");
    curl_easy_setopt(curl,CURLOPT_COOKIEJAR,"cookie.txt");


    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, buffer_writer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);

    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(data));



    char res = curl_easy_perform(curl);
    printf("%c",res);
    curl_easy_cleanup(curl);


    return buf->data;
}

//http://stackoverflow.com/questions/2091825/strpos-in-c-how-does-it-work
int strpos(char *haystack, char *needle, int pos){
    int i,j,check,result = -1;
    int len_needle=strlen(needle);
    int len_haystack=strlen(haystack);
    i = pos;

    if (len_needle > len_haystack || i > (len_haystack - 1)) return result;


    for(;i<len_haystack;i++){
        check = 0;
        for(j=0;j<len_needle;j++){
            if(haystack[i+j]==needle[j]){
                check++;
            }
        }
        if(check==len_needle){
            result = i;
            break;
        }

    }
    return result;
}

char *get_csrf(char *src) {
    char *csrf_token = "<input type=\"hidden\" name=\"csrf_token\" value=\"";
    int start = strpos(src, csrf_token, 0) + strlen(csrf_token);
    printf("\n");
    printf("%d",start);
    printf("\n");
    char *ec = ">";
    int nextint = (start+1);
    int end = strpos(src,ec,nextint)-4;
    printf("%d",end);
    int i = 0;
    int j = 0;
    char *result;
    for (i = start; i <= end; ++i) {
        if (src[i] == '\0') break;
        result[j] = src[i];
        //printf("%c",src[i]);
        //printf("\n");
        ++j;

    }

    /*FILE *fp;
    fp = fopen("test.txt", "w+");
    fprintf(fp,"%s", result);
    fclose(fp);*/

    return result;
}
