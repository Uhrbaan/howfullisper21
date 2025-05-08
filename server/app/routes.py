from flask import render_template, request, jsonify
from app.models import Recordings, Occupancy, RoomInfo
from markupsafe import escape
from sqlalchemy import text
from sqlalchemy.sql import func

def register_routes(app, db):
    @app.route('/')
    def index():
        query = db.select(RoomInfo.room)
        rooms = db.session.execute(query).scalars().all()
        print(f"Something connected: {request.headers}")
        return render_template('index.html', rooms=rooms)
    
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
        # data sent in format 
        elif request.method == 'GET':
            data = request.args
            room_ = data['room']
            count_ = int(data['count'])
            if room_ and count_:
                room = room
                count = int(count_)
            else:
                return jsonify({'error': 'Invalid URL.'}), 400

        # calculate occupancy, commit it to database
        query = db.select(RoomInfo.capacity).where(RoomInfo.room == room)
        capacity = db.session.execute(query)
        occupancy = capacity / (count/3)
        db.session.add(Occupancy(room=room, occupancy=occupancy))

        
    @app.route('/room/<room>')
    def room(room):
        room = escape(room)
        query = db.select(Recordings).where(Recordings.room == room)
        recordings = db.session.execute(query).scalars().all()

        query = db.select(Occupancy).where(Occupancy.room == room)
        occupancies = db.session.execute(query).scalars().all()

        query_room_info = db.select(RoomInfo).where(RoomInfo.room == room)
        room_info_result = db.session.execute(query_room_info).scalar_one_or_none()

        subquery = db.select(func.max(Occupancy.time)).scalar_subquery()
        query_highest_occupancy = db.select(Occupancy).where(Occupancy.time == subquery).limit(1)
        highest_occupancy_result = db.session.execute(query_highest_occupancy).scalar_one_or_none()
                
        people = None
        if highest_occupancy_result and room_info_result:
            people = highest_occupancy_result.occupancy * room_info_result.capacity

        return render_template('room.html', 
                               recordings=recordings,
                               occupancies=occupancies,
                                room=room,
                                people=people,
                                room_info=room_info_result
                               )

