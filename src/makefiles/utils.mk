# Функция для показа справки

define show_help
	@awk '/^#/ {comment=substr($$0,3)} /^[a-zA-Z][a-zA-Z_-]*:/ && comment && $$1 !~ /^_/ {printf "  $(YELLOW)%-$(HELP_WIDTH)s$(NC) %s\n", substr($$1,1,length($$1)-1), comment; comment=""}' $(MAKEFILE_LIST)
endef
