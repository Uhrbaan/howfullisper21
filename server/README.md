This folder contains everything linked to the server that will aggregate the
data from our chips.

## How to start flask
You can start flask privately (as in, it will not be visible on the network, 
meaning the chip won't be able to send data to it) with:

```sh
flask --app run.py run
```

To see errors and changes in live, you can enable debug mode (not safe !) with:

```sh
flask --app run.py run --debug
```

## Database
Because we have very little data, it is simplest to use SQLite.

Database commands:

```sh
flask db init # only needed when creating the database
flask db migrate # use to detect changes in the schemes (defined in `models.py`)
flask db upgrade # used to update the database to the newst scheme without data loss.
```

## Building docker
First, make sure you are part of the docker group, you will need it later.

```sh
sudo usermod -aG docker $USER
```

Once that is done, you have to log out for the change to tack effect. 
Then, let's create our docker image:

```sh
cd server
docker build --tag 999-flask-server-docker .
```

To see if your image is visible, do:

```sh
docker images
```

Once you made local docker instance, then you can run it (here it is important
to be part of the docker group). The port number is remapped with `--publish` to 
another port.

```sh
docker run --detach=true --publish 5000:5000 999-flask-server-docker
```

## Server
The server is located on the intranet ad `diufvm30`. 
Currently, due to security reasons, password authentication is disabled.
If you need access, you **must** create ssh keys `ssh-copy-id` them onto the vm.

Most steps of the hardening of the server were followed according to 
[this](https://youtu.be/F-9KWQByeU0?si=kQfYqPrAL_dBl0qv) video.

The ssh port is on 22.

The deployment will be done as suggested in [this Flask tutorial](https://youtu.be/oQ5UfJqW5Jo?si=hQU07xT6giZJRb40)