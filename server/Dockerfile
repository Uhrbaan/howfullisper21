FROM python:3.13-slim
WORKDIR /flask-app

# copy dependencies over (there will be too many but who cares)
# if we want to do it right, we should create a second venv that excludes the
# python libraries we need for the firmware
COPY requirements.txt requirements.txt
RUN pip3 install -r requirements.txt
RUN apt-get update && apt-get install -y sqlite3

COPY . .

WORKDIR /flask-app/app/

# Initialize migrations if the directory doesn't exist
RUN if [ ! -d migrations ]; then flask db init; fi
RUN flask db migrate
RUN flask db upgrade

WORKDIR /flask-app

ENV FLASK_APP="run:create_app"
CMD ["python3", "-m", "flask", "run", "--host=0.0.0.0", "--port=5000"]