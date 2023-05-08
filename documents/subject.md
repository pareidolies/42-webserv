### Parts

- [X] Socket
- [ ] Parsing
- [ ] Event Manager / Client
- [ ] Response / Request (header, body)
- [ ] cgi

- [ ] Default Page
- [X] Error pages
- [ ] Website


---

### Requirements

- [ ] Your program has to take a configuration file as argument, or use a default path.
- [ ] You can't execve another web server.
- [ ] Your server must never block and the client can be bounced properly if necessary.
- [ ] It must be non-blocking and use only 1 poll() (or equivalent) for all the I/O operations between the client and the server (listen included).
- [ ] poll() (or equivalent) must check read and write at the same time.
- [ ] You must never do a read or a write operation without going through poll() (or equivalent).
- [ ] Checking the value of errno is strictly forbidden after a read or a write operation.
- [ ] You don't need to use poll() (or equivalent) before reading your configuration file.
- [ ] You can use every macro and define like FD_SET, FD_CLR, FD_ISSET, FD_ZERO (understanding what and how they do it is very useful).
- [ ] **Timeout handling** - A request to your server should never hang forever.
- [ ] Your server must be compatible with the web browser of your choice.
- [ ] We will consider that NGINX is HTTP 1.1 compliant and may be used to compare headers and answer behaviors.
- [ ] Your HTTP response status codes must be accurate.

  - [ ] define 200, 400, 404, 403, 500, 504
- [ ] You server must have default error pages if none are provided.
- [ ] You can't use fork for something else than CGI (like PHP, or Python, and so forth).
- [ ] You must be able to serve a fully static website.
- [ ] Clients must be able to upload files.
- [ ] You need at least GET, POST, and DELETE methods.
- [ ] Stress tests your server. It must stay available at all cost.
- [ ] Your server must be able to listen to multiple ports (see Configuration file).

### Configuration file

Choose the port and host of each ’server’.

- [ ] Setup the server_names or not.
- [ ] The first server for a host:port will be the default for this host:port (that means it will answer to all the requests that don’t belong to an other server).
- [ ] Setup default error pages.
- [ ] Limit client body size.
- [ ] Setup routes with one or multiple of the following rules/configuration (routes wont be using regexp):

  - [ ] Define a list of accepted HTTP methods for the route.
  - [ ] Define a HTTP redirection.
  - [ ] Define a directory or a file from where the file should be searched
  - [ ] Turn on or off directory listing.
  - [ ] Set a default file to answer if the request is a directory.
  - [ ] Execute CGI based on certain file extension (for example .php).
  - [ ] Make it work with POST and GET methods.
  - [ ] Make the route able to accept uploaded files and configure where they shouldbe saved.
- [ ] You must provide some configuration files and default basic files to test and demonstrate every feature works during evaluation.
