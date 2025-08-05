all: build enter

build: off
	@docker compose up --build -d --force-recreate

enter:
	@docker compose exec ubuntu_dev /bin/zsh -c "cd src && /bin/zsh"

off:
	@docker compose down

.PHONY: all build enter off
