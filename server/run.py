from app.app import create_app

flask_app = create_app()

if __name__ == '__name__':
    flask_app.run()