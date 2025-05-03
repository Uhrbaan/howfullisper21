from flask import render_template, request, jsonify
from models import BLEDevices
from markupsafe import escape

def register_routes(app, db):
    @app.route('/')
    def index():
        devices = BLEDevices.query.all()
        print(f"Something connected: {request.headers}")
        return render_template('index.html', queries=devices)
    
    # curl --request POST http://127.0.0.1:5000/collect?room=INFOLAB0&count=7
    @app.route('/collect', methods=['POST'])
    def collect():
        data = request.get_json()
        room = data.get('room')
        count = data.get('count')
        if room and isinstance(count, int):
            device = BLEDevices(room=room, count=count)
            db.session.add(device)
            db.session.commit()
            return jsonify({'message': 'Data received and stored successfully'}), 201
        else:
            return jsonify({'error': 'Invalid JSON data.'}), 400
        
    @app.route('/room/<room>')
    def room(room):
        room = escape(room)
        query = db.select(BLEDevices).where(BLEDevices.room == room)
        result = db.session.execute(query)
        return render_template('room.html', queries=result, room=room)
