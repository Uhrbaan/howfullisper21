from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_migrate import Migrate

visited_times = 0

db = SQLAlchemy()

def create_app():
    app = Flask(__name__, template_folder='templates')
    app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///./sqlite.db'
    db.init_app(app)

    from app.routes import register_routes
    register_routes(app, db)

    migrate = Migrate(app, db)
    return app