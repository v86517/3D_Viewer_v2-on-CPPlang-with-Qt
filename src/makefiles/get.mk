URL_MODELS = https://ed843466-7e84-4cb5-b2dc-83c48c730cd2.selstorage.ru/lothostr/shared/obj.zip
MODELS = obj
DIR_MODELS = $(MODELS)
ZIP_MODELS = $(MODELS).zip

_get-models:
	@mkdir -p $(DIR_MODELS)
	@cd $(DIR_MODELS) && curl -o $(ZIP_MODELS) $(URL_MODELS)
	@cd $(DIR_MODELS) && unzip $(ZIP_MODELS)
	@cd $(DIR_MODELS) && rm $(ZIP_MODELS)
