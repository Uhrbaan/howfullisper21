.PHONY: build-firmware debug-server build-server-docker clean-server-db stop-server-docker run-server-docker init-server-db migrate-server-db save-server-docker upload-remote-image load-remote-docker run-remote-docker stop-remote-docker deploy-server
.ONESHELL: build-firmware debug-server build-server-docker clean-server-db stop-server-docker run-server-docker init-server-db migrate-server-db save-server-docker upload-remote-image load-remote-docker run-remote-docker stop-remote-docker deploy-server

DOCKER_TAG := 999-flask-server-docker
FLASK_PORT := 5000
DOCKER_PORT := 5000
DOCKER_IMG_TAR := $(DOCKER_TAG).tar.xz
REMOTE_HOST := diufvm30
REMOTE_LOCATION := ~

export FLASK_APP=run:create_app

# build the firmware
build-firmware:
	cd firmware
	pio run

# local debugging options for the server
debug-server:
	cd server
	python3 -m flask run --debug --port=$(FLASK_PORT)

init-server-db:
	cd server
	flask db init
	flask db migrate
	flask db upgrade

migrate-server-db: # call this when model changes
	cd server
	flask db migrate
	flask db upgrade

clean-server-db:
	cd server
	rm -rf instance app/instance app/migrations app/__pycache__ __pycache__

# local docker-related commands
build-server-docker:
	cd server
	docker build --tag $(DOCKER_TAG) .

run-server-docker:
	docker run --detach=true --publish $(FLASK_PORT):$(DOCKER_PORT) $(DOCKER_TAG)

stop-server-docker:
	if docker stop $(shell docker ps -q --filter ancestor=$(DOCKER_TAG)); then \
		echo "docker container was stopped."; \
	else \
		echo "docker container could not be stopped: it was not running. Continuing..."; \
	fi

save-server-docker:
	docker save -o $(DOCKER_IMG_TAR) $(DOCKER_TAG):latest

# remote commands. You NEED to have your ssh keys configured for this.
upload-remote-image:
	scp $(DOCKER_IMG_TAR) $(REMOTE_HOST):$(REMOTE_LOCATION)

load-remote-docker:
	ssh $(REMOTE_HOST) docker load -i $(REMOTE_LOCATION)/$(DOCKER_IMG_TAR)

run-remote-docker:
	ssh $(REMOTE_HOST) docker run --detach=true --publish $(FLASK_PORT):$(DOCKER_PORT) $(DOCKER_TAG)

stop-remote-docker:
	if ssh $(REMOTE_HOST) docker stop $$(ssh $(REMOTE_HOST) docker ps -q --filter ancestor=$(DOCKER_TAG)); then \
		echo "The remote docker container was stopped."; \
	else \
		echo "The remote docker container could not be stopped: it was not running. Coninuing..."; \
	fi

# shorthand
deploy-server: \
	stop-remote-docker \
	clean-server-db \
	build-server-docker \
	save-server-docker \
	upload-remote-image \
	load-remote-docker \
	run-remote-docker