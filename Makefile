.PHONY: firmware server-debug server-build-docker server-remove-db server-stop-docker server-run-docker
.ONESHELL: firmware server-debug server-build-docker server-remove-db server-stop-docker server-run-docker

DOCKER_TAG := 999-flask-server-docker
FLASK_PORT := 5000
DOCKER_PORT := 5000
DOCKER_IMG_TAR := $(DOCKER_TAG).tar.xz
REMOTE_HOST := diufvm30
REMOTE_LOCATION := ~/

# build the firmware
firmware:
	cd firmware
	pio run

# local debugging options for the server
server-debug:
	cd server
	python3 -m flask run --host=0.0.0.0 --debug --port=$(FLASK_PORT)

server-remove-db:
	cd server
	rm -rf instance app/instance app/migrations app/__pycache__ __pycache__

# local docker-related commands
server-build-docker:
	cd server
	docker build --tag $(DOCKER_TAG) .

server-run-docker:
	docker run --detach=true --publish $(FLASK_PORT):$(DOCKER_PORT) $(DOCKER_TAG)

server-stop-docker:
	if docker stop $(shell docker ps -q --filter ancestor=$(DOCKER_TAG)); then \
		echo "docker container was stopped."; \
	else \
		echo "docker container could not be stopped: it was not running. Continuing..."; \
	fi

server-save-docker:
	docker save -o $(DOCKER_IMG_TAR) $(DOCKER_TAG)

# remote commands. You NEED to have your ssh keys configured for this.
remote-upload-image:
	scp $(DOCKER_IMG_TAR) $(REMOTE_HOST):$(REMOTE_LOCATION)

remote-docker-load:
	ssh $(REMOTE_HOST) cd $(REMOTE_LOCATION) && docker load -i $(DOCKER_IMG_TAR)

remote-docker-run:
	ssh $(REMOTE_HOST) docker run --detach=true --publish $(FLASK_PORT):$(DOCKER_PORT) $(DOCKER_TAG)

remote-docker-stop:
	if ssh $(REMOTE_HOST) docker stop $$(ssh $(REMOTE_HOST) docker ps -q --filter ancestor=$(DOCKER_TAG)); then \
		echo "The remote docker container was stopped."; \
	else \
		echo "The remote docker container could not be stopped: it was not running. Coninuing..."; \
	fi

# shorthand
server-deploy: remote-docker-stop server-remove-db server-build-docker server-save-docker remote-upload-image remote-docker-load remote-docker-run