# CS118 Project 0

This is the repo for spring23 cs118 project 0.

## Academic Integrity Note

You are encouraged to host your code in private repositories on [GitHub](https://github.com/), [GitLab](https://gitlab.com), or other places.  At the same time, you are PROHIBITED to make your code for the class project public during the class or any time after the class.  If you do so, you will be violating academic honestly policy that you have signed, as well as the student code of conduct and be subject to serious sanctions.

## Provided Files

- `project` is folder to develop codes for future projects.
- `docker-compose.yaml` and `Dockerfile` are files configuring the containers.

## Bash commands

```bash
# Setup the container(s) (make setup)
docker compose up -d

# Bash into the container (make shell)
docker compose exec node1 bash

# Remove container(s) and the Docker image (make clean)
docker compose down -v --rmi all --remove-orphans
```

## Environment

- OS: ubuntu 22.04
- IP: 192.168.10.225. NOT accessible from the host machine.
- Port forwarding: container 8080 <-> host 8080
  - Use http://localhost:8080 to access the HTTP server in the container.
- Files in this repo are in the `/project` folder. That means, `server.c` is `/project/project/server.c` in the container.

## TODO

Yash Shah UID: 405 565 567

High Level Code Design:
The entire server-client interaction is setup in server.cpp. The main file instantiates the server socket and then calls a connectionHandler function to handle everything on the client side. If the server socket is running as expected without any errors, an infinite loop is created in the connectionHandler function to listen to any interactions on the client end. It first accepts any request from the client and reads the bytes of the packet to get the request header.

Once the request header has successfully been retrieved, we can get the file name the client is looking for by parsing the request header in the getFileName function. If a filename isn't given, then a 404 error is sent as a response to the client. We handle any space and/or percentage character encodings using the handleFileName function, if otherwise. 

Then utilizing the dirent library, we iterate through our directory to find the file name we are looking for in regards to case insentiveness. If the file is not found, then we send the 404 error response again. If it is found, then we extract the file extension, get the content type, and then get the contents of the file, and send an appropriatly written response header and contents of the file as a response back to the client.

Troubles and Solutions:
Setting up the socket on both the client and server end was fairly easy given the help in the slides. However, I was running into issues on my server, where there were multiple instances of the server causing the port to be busy and un-killable. Using the following site: https://hea-www.harvard.edu/~fine/Tech/addrinuse.html, I was able to add in code that forced only one connection to occur at a time.

I then ran into a lot of issues about how to get the file name. I looked online for libraries to use and stumbled on a couple like filesystem and dirent. I ended up utilizing dirent based off this example https://stackoverflow.com/questions/25070751/navigating-through-files-using-dirent-h as I found it the easiest to work with.

I had to look at a lot of documentation to understand and utilize functions like snprintf, strncasecmp, fstat, etc. But it was doable with enough time.
