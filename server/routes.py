from flask import render_template, request
from models import BLEDevices

def register_routes(app, db):
    @app.route('/')
    def index():
        devices = BLEDevices.query.all()
        print(devices)
        return render_template('index.html', queries=devices)
    
    # curl --request POST http://127.0.0.1:5000/collect?room=INFOLAB0&count=7
    @app.route('/collect', methods=['POST'])
    def collect():
        data = request.args
        device = BLEDevices(room=data['room'], count=data['count'])

        db.session.add(device)
        db.session.commit()

        return f'Insertion confirmed for {data}\n'