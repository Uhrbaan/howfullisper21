"""
This file is the entry point to the flask server.
"""

from app.app import create_app

flask_app = create_app()

if __name__ == '__name__':
    flask_app.run()