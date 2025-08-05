VOLUME_NAME = gitlab-runner-3dviewer
CI_NAME = ubuntu_ci_3dviewer
DOCKER_HUB = $(USER_DOCKER_HUB)
IMAGE = $(DOCKER_HUB)/$(CI_NAME)

include .env

all: build-ubuntu_ci push-ubuntu_ci build-gitlab_runner register-gitlab_runner

build-ubuntu_ci:
	cd .. && docker build -t $(IMAGE) .

push-ubuntu_ci:
	docker push $(IMAGE):latest

build-gitlab_runner:
	docker volume create $(VOLUME_NAME)
	sudo docker run -d --name $(VOLUME_NAME) --restart always -v /var/run/docker.sock:/var/run/docker.sock -v $(VOLUME_NAME):/etc/gitlab-runner gitlab/gitlab-runner:latest

register-gitlab_runner:
	sudo docker run --rm -it \
		-v $(VOLUME_NAME):/etc/gitlab-runner \
		--env-file .env \
		gitlab/gitlab-runner:latest register \
		--non-interactive \
		--url $(REGISTER_URL) \
		--registration-token $(REGISTRATION_TOKEN) \
		--name $(CI_NAME) \
		--executor docker \
		--docker-image $(IMAGE)

.PHONY: all build-ubuntu_ci push-ubuntu_ci build-gitlab_runner register-gitlab_runner
