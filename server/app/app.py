"""
This file defines the core Flask application factory and initializes the database and migration tools.
It sets up the application with its database connection and registers the defined routes.
"""

from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_migrate import Migrate

visited_times = 0

db = SQLAlchemy()

def create_app():
    """
    Creates and configures the Flask application.

    This function initializes the Flask application, sets up the SQLAlchemy database connection,
    registers the application's routes, and configures Flask-Migrate for database migrations.

    :return: The configured Flask application instance.
    :rtype: flask.Flask
    """
    app = Flask(__name__, template_folder='templates')
    app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///./sqlite.db'
    db.init_app(app)

    from app.routes import register_routes
    register_routes(app, db)

    migrate = Migrate(app, db)
    return app