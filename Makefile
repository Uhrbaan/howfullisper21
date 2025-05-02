.PHONY: firmware
firmware:
	pio run --project-dir firmware

.PHONY: server
server:
	flask --app server/run.py run --host=0.0.0.0