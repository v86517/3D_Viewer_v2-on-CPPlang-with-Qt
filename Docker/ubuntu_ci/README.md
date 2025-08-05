# Установка gitlab-runner в Docker

**Запуск контейнера GitLab Runner с использованием тома:**
```sh
make build-gitlab_runner
```

Эта команда запускает новый контейнер Docker с несколькими параметрами:
- `-d`: Запускает контейнер в фоновом режиме (detached mode).
- `--name`: Присваивает контейнеру имя `gitlab-runner-3dviewer`.
- `--restart always`: Гарантирует, что контейнер будет автоматически перезапускаться, если он остановится по какой-либо причине.
- `-v /var/run/docker.sock:/var/run/docker.sock`: Монтирует сокет Docker с хост-системы в контейнер. Это позволяет GitLab Runner управлять другими контейнерами Docker.
- `-v gitlab-runner-3dviewer:/etc/gitlab-runner`: Монтирует ранее созданный том `gitlab-runner-3dviewer` в папку `/etc/gitlab-runner` внутри контейнера. Это позволяет сохранять конфигурацию GitLab Runner даже после перезапуска или удаления контейнера.
- `gitlab/gitlab-runner:latest`: Имя образа, который будет использован для создания контейнера. В данном случае используется последняя версия образа GitLab Runner.
