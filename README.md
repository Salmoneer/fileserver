# Simple C File Server

- Written from scratch in C using linux sockets
- Serves static HTML websites
- Supports HTTP adequately

## How to use
Please note this only works on linux (and potentially mac?), as it makes exclusive use of unix sockets for networking.

1. Clone this repo
```bash
git clone https://github.com/Salmoneer/fileserver.git
```

2. Compile and run
```bash
make
make run
```

3. Connect to the server
Navigate to http://127.0.0.1:8069 in your browser (while the server is running).
You should see served to you whatever is in the `content` folder (default to `index.html`).
Currently, the server only reports files as html or css, based on file extension.
