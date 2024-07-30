#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>

#define BUFFER_SIZE 1500000
#define PORT 8080

// Function Declarations
void connectionHandler(int s_sockit);
void getFileName(std::string &request, std::string &file_name);
std::string handleFileName(std::string &file_name);
char* fileExists(std::string &file_name);
bool sendFileContents(int c_sockit, char* &file_name);
std::string send404Error();

// Function Implementations

std::string send404Error(){
  // html for 404 error
  std::string error_page = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1><p>The requested file was not found on this server.</p></body></html>";
  
  // set up a buffer and include the response header at the top
  char buff[BUFFER_SIZE];
  snprintf(buff, sizeof(buff), "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n",
        error_page.length());
  
  // add the 404 error to the body of the response
  std::string response(buff);
  response += error_page;

  return response;
}

std::string determine_mime_type(const std::string &file_extension) {

  if (file_extension == ""){
    return "application/octet-stream";
  }

  // compare each file extension with the required mime types according to the prompt and return its content type for the response header
  if (strncasecmp(file_extension.c_str(), ".html", file_extension.length()) == 0 || strncasecmp(file_extension.c_str(), ".htm", file_extension.length()) == 0) {
    return "text/html";
  } else if (strncasecmp(file_extension.c_str(), ".txt", file_extension.length()) == 0) {
    return "text/plain";
  } else if (strncasecmp(file_extension.c_str(), ".jpg", file_extension.length()) == 0 || strncasecmp(file_extension.c_str(), ".jpeg", file_extension.length()) == 0) {
    return "image/jpeg";
  } else if (strncasecmp(file_extension.c_str(), ".png", file_extension.length()) == 0) {
    return "image/png";
  }
  
  return NULL;
  
}

bool sendFileContents(int c_sockit, char* &file_name){

  // open file
  int fd = open(file_name, O_RDONLY);
  if (fd < 0) {
    perror("Error: File cannot be opened");
    return false;
  }

  char buff[BUFFER_SIZE];
  ssize_t bytes_read = 0;

  // read the bytes of the file
  while ((bytes_read = read(fd, buff, sizeof(buff))) > 0){
    if (send(c_sockit, buff, bytes_read, 0) < 0){
      perror("Error: File chunks cannot be sent to the client. ");
      close(fd);
      return false;
    }
  }
  
  close(fd);
  return true;
}

char* fileExists(std::string &file_name) {
    DIR *dir;
    struct dirent *f;
    std::string cwd = ".";

    // open current dir
    dir = opendir(cwd.c_str());
    if (dir) {
        // iteratively search through the files until no files are searcheable
        while ((f = readdir(dir)) != NULL) {
          // if length of file is equal to a file in the directory
          if (file_name.length() == strlen(f->d_name)) {
            // compare file names with consideration of case insentiveness
            if (strncasecmp(file_name.c_str(), f->d_name, file_name.length()) == 0) {
                return f->d_name;
            }
          }
        }
        closedir(dir);
    }
  return NULL;
}

std::string handleFileName(std::string &file_name){
  // new file name after spaces and %'s are handled
  std::string n_fname;

  int len = file_name.length();
  int itr_max = len - 3;

  int i = 0;
  // iterate through the file name and look for substring %20 for spaces or %
  while (i < len) {
    if (i <= itr_max && file_name[i] == '%') {
      if ((i+2) <= len && file_name[i+1] == '2') {

        // add space to string if %20 
        if (file_name[i+2] == '0')
          n_fname.push_back(' ');
        // add % to string if %25
        else if (file_name[i+2])
          n_fname.push_back('%');

        i += 3;
      } else {
        // if % not encrypted as %25, push it
        n_fname.push_back(file_name[i]);
        i++;
      }
    } else {
      // else add current character
      n_fname.push_back(file_name[i]);
      i++;
    }
  }

  return n_fname;
}

void getFileName(std::string &request, std::string &file_name){
  // get the file name using the first two spaces of the request header
  size_t start = request.find(' ') + 1;
  size_t end = request.find(' ', start);
  file_name = request.substr(start, end - start);

  // remove leading '/'
  if (file_name != "/") {
    file_name = file_name.substr(1);
  }
}

void connectionHandler(int s_sockit) {

  struct sockaddr_in c_addr;
  socklen_t c_size;
  char buff[BUFFER_SIZE];
  int c_sockit;
  ssize_t bytes_read;

  while (1) {
    // accept incoming request from server
    c_size = sizeof(c_addr);
    c_sockit = accept(s_sockit, (struct sockaddr *) &c_addr, &c_size);
    if (c_sockit < 0){
      perror("Error: Connection cannot be accepted. ");
      close(c_sockit);
      continue;
    }

    // one client connection at a time
    int opt = 1;
    if (setsockopt(c_sockit, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Error setting socket option");
        close(c_sockit);
        continue;
    }

    // read contents of packet from client
    bytes_read = recv(c_sockit, buff, BUFFER_SIZE - 1, 0);
    if (bytes_read < 0) {
      perror("Error: Data from socket cannot be read.");
      close(c_sockit);
      continue;
    }

    // create resonse string for the client to send
    std::string res;

    buff[bytes_read] = '\0';
    // create string object and initialize with contents of buff for string manipulation
    std::string request(buff);
    std::cout << "HTTP Request: " << std::endl << request << std::endl;

    // get file name
    std::string fname;
    getFileName(request, fname);

    // send 404 Error if no file name is given
    if (fname.empty() || fname.back() == '/') {
      res = send404Error();
      send(c_sockit, res.c_str(), res.size(), 0);
      close(c_sockit);
      continue;
    }

    // handle spaces and % encodings of file name
    fname = handleFileName(fname);

    std::vector<char> fcontent;
    // check if file exists in directory and return correct file name
    char* n_fname = fileExists(fname);
    
    if (n_fname != NULL) {
      // get the extension of the file to find its mime / content type
      std::string file_ext;
      if (fname.find('.') == std::string::npos) {
        file_ext = "";
      } else {
        file_ext = fname.substr(fname.find_last_of('.'));
      }
      std::string mime_type = determine_mime_type(file_ext);

      struct stat fcontent;
      stat(n_fname, &fcontent);
      size_t fsize = fcontent.st_size;

      std::cout << fsize << std::endl;
      
      // setup a buffer for the response header
      char res_buff[BUFFER_SIZE];
      snprintf(res_buff, sizeof(res_buff), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n",
        mime_type.c_str(), fsize);
      std::string res(res_buff);
      
      // send the header and the content of the file
      send(c_sockit, res.c_str(), res.size(), 0);
      sendFileContents(c_sockit, n_fname);
    } else {
      // if file is not found, send a 404 error
      res = send404Error();
      // send response header and html contents of 404 error
      send(c_sockit, res.c_str(), res.size(), 0);
    }

    close(c_sockit);
  }
}

int main(int argc, char *argv[])
{

  // create the socket instance  
  int sockit = socket(AF_INET, SOCK_STREAM, 0);
  if (sockit < 0) {
    perror("Error: Socket cannot be instantiated.");
    return 1;
  }
  printf("Socket has been initiated.\n");

  // one server connection at a time
  int opt = 1;
  if (setsockopt(sockit, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
      perror("Error setting socket option");
      return 1;
  }

  // define the server address struct
  struct sockaddr_in s_addr; 
  s_addr.sin_port = htons(PORT);
  s_addr.sin_family = AF_INET;
  s_addr.sin_addr.s_addr = INADDR_ANY;

  // bind the server with the associated IP address and port number
  socklen_t s_size = sizeof(s_addr);
  int binded = bind(sockit, (struct sockaddr *) &s_addr, s_size);
  if (binded < 0) {
    perror("Error: Server cannot be binded to the IP address and/or Port number.");
    close(sockit);
    return 1;
  }
  printf("Socket is binded with address.\n");

  // listen for any incoming requests
  if (listen(sockit, 5) < 0){
    perror("Error: Server cannot listen to any new connections. ");
    close(sockit);
    return 1;
  }
  printf("Server is listening for incoming connections on port 8080.\n");

  // handle each incoming client request by the server socket
  connectionHandler(sockit);

  // close the socket to prevent any unnecessary memory leaks
  close(sockit);

  return 0;
}
