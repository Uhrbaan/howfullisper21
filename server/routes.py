from flask import render_template, request, jsonify
from models import Recordings, Occupancy, RoomInfo
from markupsafe import escape

def register_routes(app, db):
    @app.route('/')
    def index():
        devices = Recordings.query.all()
        print(f"Something connected: {request.headers}")
        return render_template('index.html', queries=devices)
    
    # curl --request POST http://127.0.0.1:5000/collect?room=INFOLAB0&count=7
    @app.route('/collect', methods=['POST', 'GET'])
    def collect():
        room: str = ''
        count: int = 0

        if request.method == 'POST':
            data = request.get_json()
            room = data.get('room')
            count = data.get('count')
            if room and isinstance(count, int):
                device = Recordings(room=room, count=count)
                db.session.add(device)
                db.session.commit()
                return jsonify({'message': 'Data received and stored successfully'}), 201
            else:
                return jsonify({'error': 'Invalid JSON data.'}), 400
        elif request.method == 'GET':
            data = request.args
            room = data['room']
            count = int(data['count'])
            # check for errors
        # calculate occupancy, commit it to database

        
    @app.route('/room/<room>')
    def room(room):
        room = escape(room)
        query = db.select(Recordings).where(Recordings.room == room)
        recordings = db.session.execute(query)

        query = db.select(Occupancy).where(Occupancy.room == room)
        occupancies = db.session.execute(query)
        
        return render_template('room.html', queries=recordings, room=room, occupancies=occupancies)
