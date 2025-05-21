.PHONY: build-firmware debug-server build-server-docker clean-server-db stop-server-docker run-server-docker init-server-db migrate-server-db save-server-docker upload-remote-image load-remote-docker run-remote-docker stop-remote-docker deploy-server
.ONESHELL: build-firmware debug-server build-server-docker clean-server-db stop-server-docker run-server-docker init-server-db migrate-server-db save-server-docker upload-remote-image load-remote-docker run-remote-docker stop-remote-docker deploy-server

DOCKER_TAG := 999-flask-server-docker
FLASK_PORT := 5000
DOCKER_PORT := 80# default for the http protocol → the user won't have to enter it
DOCKER_IMG_TAR := $(DOCKER_TAG).tar.xz
REMOTE_HOST := diufvm30
REMOTE_LOCATION := ~
POPULATE_SCRIPT := 'server/populate-script.sql'

export FLASK_APP=run:create_app

# build the documentation file. Can be found in server/static/docs
build-docs:
	doxygen Doxyfile

# build and upload the firmware
build-firmware:
	cd firmware
	pio run

# local debugging options for the server
debug-server:
	cd server
	python3 -m flask run --debug --port=$(FLASK_PORT)

# remove the database to clean it
clean-server-db:
	cd server
	rm -rf instance app/instance app/migrations app/__pycache__ __pycache__

# initialize the database, if it was deleted for example, or you just installed the project
init-server-db:
	cd server
	flask db init
	flask db migrate
	flask db upgrade

# Run if you do any changes to the models.py file
migrate-server-db: # call this when model changes
	cd server
	flask db migrate
	flask db upgrade

# build the docker image (Dockerfile in server)
build-server-docker:
	cd server
	docker build --tag $(DOCKER_TAG) .

# run the docker server with port redirection
run-server-docker:
	docker run --detach=true --publish $(FLASK_PORT):$(DOCKER_PORT) $(DOCKER_TAG)

# stop the docker container. Note: this might fail if you have multiple instances 
# of docker running.
stop-server-docker:
	if docker stop $(shell docker ps -q --filter ancestor=$(DOCKER_TAG)); then \
		echo "docker container was stopped."; \
	else \
		echo "docker container could not be stopped: it was not running. Continuing..."; \
	fi

# compress the docker image into a tar file.
save-server-docker:
	docker save -o $(DOCKER_IMG_TAR) $(DOCKER_TAG):latest

# remote commands. You NEED to have your ssh keys configured for this.
# upload the docker tar to the distant location. You HAVE to have you ssh key configured.
upload-remote-image:
	scp $(DOCKER_IMG_TAR) $(REMOTE_HOST):$(REMOTE_LOCATION)

# distant: decompress the docker tar into a real docker image
load-remote-docker:
	ssh $(REMOTE_HOST) docker load -i $(REMOTE_LOCATION)/$(DOCKER_IMG_TAR)

# distant: run the docker image on the server
run-remote-docker:
	ssh $(REMOTE_HOST) docker run --detach=true --publish $(DOCKER_PORT):$(FLASK_PORT) $(DOCKER_TAG)

# distant: stop the docker image on the serve. Note: might fail if you have multiple
# instances of docker running
stop-remote-docker:
	if ssh $(REMOTE_HOST) docker stop $$(ssh $(REMOTE_HOST) docker ps -q --filter ancestor=$(DOCKER_TAG)); then \
		echo "The remote docker container was stopped."; \
	else \
		echo "The remote docker container could not be stopped: it was not running. Coninuing..."; \
	fi

# populate the distant server with random data.
populate-remote-db:
	scp $(POPULATE_SCRIPT) $(REMOTE_HOST):$(REMOTE_LOCATION)/$(POPULATE_SCRIPT)
	ssh $(REMOTE_HOST) "docker cp $(POPULATE_SCRIPT) $$(ssh $(REMOTE_HOST) docker ps -q --filter ancestor=$(DOCKER_TAG)):$(POPULATE_SCRIPT)"
	ssh $(REMOTE_HOST) "docker exec -i $(shell ssh $(REMOTE_HOST) docker ps -q) sqlite3 instance/sqlite.db < $(POPULATE_SCRIPT)"

# runs multiple commands to deploy the servor on the distant location. Might take
# a while depending on your internet speed.
deploy-server: \
	build-docs \
	stop-remote-docker \
	clean-server-db \
	build-server-docker \
	save-server-docker \
	upload-remote-image \
	load-remote-docker \
	run-remote-docker