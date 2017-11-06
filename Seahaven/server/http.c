#include "http.h"

/*
  Creates a block of memory of MAX_HTTP_SIZE large and saves a
  part of it for header. The body gets put in the body section
  right aligned to the buffer. The header section is inserted at
  the right in front of the body. This means there will be a gap
  between the beginning of the buffer and where the head start
  which is the part that gets sent back to client
 */
void* httpHandle(http_client* http_config) {

  int content_length;
  int header_offset;
  int header_length;

  // status = callApiRoute(&request_HTTP, &response_HTTP, route, (http_t*)config);
  // Not a API route, checking for file
      
  // generates timestamp for response header
  getTime(&http_config->timestamp, 256);

  // gets contents from file to send back in response boday
  content_length = getFileContent(http_config->header->route,
				  &http_config->response_HTTP,
				  MAX_RESPONSE_SIZE);

  if (content_length < 0) {
    sprintf(http_config->response_HTTP, "HTTP/1.1 400 OK\r\nCache-Control: no-cache, private\r\nDate: %s\r\n\r\n", http_config->timestamp);
    header_length = strlen(http_config->response_HTTP);
    header_offset = 0;
    content_length = 0; // need for send() logic
  } else {
    // gets a pointer where the response_body starts
    sprintf(http_config->response_header, "HTTP/1.1 200 OK\r\nCache-Control: no-cache, private\r\nContent-Length: %i\r\nDate: %s\r\n\r\n", content_length, http_config->timestamp);
    header_length = strlen(http_config->response_header);

    if (content_length + header_length > MAX_RESPONSE_SIZE) {
      // TODO - too large of response
    }
	
    // offset where header is from start of buffer
    header_offset = MAX_RESPONSE_SIZE - content_length - header_length;

    memcpy(http_config->response_HTTP + header_offset, http_config->response_header, header_length);
  }

  // api was called
  //sprintf(response_HTTP, "HTTP/1.1 200 OK\r\nCache-Control: no-cache, private\r\nContent-Length: 11\r\nContent-Type: application/json\r\nDate: Sat, 24 Jun 2017 05:29:07\r\n\r\n{\"test\":42}\r\n");
  //header_length = strlen(response_HTTP);
  //header_offset = 0;
  //content_length = 0; // need for send() logic
  //} else {
  //  tODO error
  //}
  
  send(http_config->socket_id, http_config->response_HTTP + header_offset, header_length + content_length, 0);

  close(http_config->socket_id);

  free(http_config->header);
    
return NULL;
}

int getFileContent(char* relative_path, char** return_body, int length) {

  FILE* file_p;
  int   content_length;
  int   front_offset = 0;
  char  file_path[512];
  char*  file_ext;
  const char PERIOD = '.';

  printf("File: %s\n", relative_path);
  
  // make default site index.html if no path set
  if (1 == strlen(relative_path) && strncmp(relative_path, "/", 1) == 0) {
    strcat(relative_path, "index.html");
  }
  
  strcpy(file_path, WEBSITE_FOLDER);
  strcat(file_path, relative_path);  
  
  // need to decide if file is text or binary
  file_ext = strrchr(relative_path, PERIOD);

  if (NULL == file_ext) {
    return printError("--SERVER-- ERROR: getFileContent - no file extension\n", -1);
  }

  // Only opens as text file if one of accepted text file types
  if ( 0 == strcmp(file_ext, ".html") ||
       0 == strcmp(file_ext, ".css")  ||
       0 == strcmp(file_ext, ".js")) {
    file_p = fopen(file_path, "r");
  } else {  
    file_p = fopen(file_path, "rb");
  }
  
  if (file_p == NULL) {
    return printError("--SERVER-- ERROR: getFileContent - can't open file\n", -1);
  }
  
  // gets length of file and resets
  fseek(file_p, 0, SEEK_END);
  content_length = ftell(file_p);
  fseek(file_p, 0, SEEK_SET);
  
  if (content_length > length) {
    return printError("--SERVER-- ERROR: getFileContent - File to large\n", -1);
  } else {
    front_offset = length - content_length;
  }

  // reads in file so its right align with max length
  fread(*return_body + front_offset, content_length, 1, file_p);
  fclose(file_p);
  return content_length;
}
