"""
This file defines the routes and view functions for the Flask application.
It handles requests for the homepage, data collection from sensors, and displaying room-specific information.
"""
from flask import render_template, request, jsonify
from app.models import Recordings, Occupancy, RoomInfo
from markupsafe import escape
from sqlalchemy.sql import select, func

def register_routes(app, db):
    """
    Registers the routes for the Flask application.

    Args:
        app (Flask): The Flask application instance.
        db (SQLAlchemy): The SQLAlchemy database instance.
    """
    @app.route('/')
    def index():
        """
        Renders the homepage, displaying a list of available rooms.
        It queries the database for all unique room names from the RoomInfo table.
        Returns:
            flask.Response: The rendered 'index.html' template with the list of rooms.
        """
        query = db.select(RoomInfo.room)
        room_names = db.session.execute(query).scalars().all()
        
        # Add dynamic image filenames for each room
        rooms = []
        for room_name in room_names:
            # Get the latest occupancy value for this room
            subquery = db.select(func.max(Occupancy.time)).where(Occupancy.room == room_name).scalar_subquery()
            occ_query = db.select(Occupancy).where(Occupancy.room == room_name, Occupancy.time == subquery).limit(1)
            latest_occupancy = db.session.execute(occ_query).scalar_one_or_none()

            occupancy_percent = round(latest_occupancy.occupancy * 100, 2) if latest_occupancy else 0

            rooms.append({
                "name": room_name,
                "image": f"{room_name.lower()}.png",
                "occupancy_percent": occupancy_percent
            })

        return render_template(
            'index.html',
            rooms=rooms,
            text_p='hello, world!'
        )
    
    @app.route('/hello')
    def hell():
        return render_template('hello.html', cool='😎')
    
    @app.route('/justgage')
    def jauge():
        return render_template('justgage.html')
    
    # curl --request POST http://127.0.0.1:5000/collect?room=INFOLAB0&count=7
    @app.route('/collect', methods=['POST'])
    def collect():
        room_name: str = ''
        count: int = 0

        if request.method == 'POST':
            data = request.get_json()
            if data is None:
                return jsonify({'error': 'Request body is not valid JSON'}), 400
            
            room_name = data.get('room')
            room_capacity = data.get('room_capacity')
            room_empty_device_count = data.get('room_empty_device_count')
            room_device_per_person = data.get('room_device_per_person')
            device_count = data.get('device_count')
            people_count = data.get('people_count')
            occupancy = data.get('occupancy')

            if room_name and isinstance(people_count, float):
                # add the recording
                device = Recordings(
                    room=room_name, 
                    room_capacity=room_capacity,
                    room_empty_device_count=room_empty_device_count,
                    room_device_per_person=room_device_per_person,
                    device_count=device_count,
                    people_count=people_count,
                    occupancy=occupancy)
                db.session.add(device)
                db.session.commit()

                # if the RoomInfo does not exist i.e, its a new room, create it
                if room_capacity is not None:
                    existing_room = db.session.query(RoomInfo).filter_by(room=room_name).first()
                    if not existing_room:
                        new_room_info = RoomInfo(room=room_name, capacity=room_capacity)
                        db.session.add(new_room_info)
                        db.session.flush()  # Ensure the room is added before the commit
                        print(f"RoomInfo added for room: {room_name} with capacity: {room_capacity}")
                    else:
                        print(f"RoomInfo already exists for room: {room_name}")
            else:
                return jsonify({'error': 'Missing "room" or "count" in JSON data, or "count" is not an integer.'}), 400 # More specific error
                
        if room_name is not None and occupancy is not None: # Ensure room and count are set before proceeding    
            db.session.add(Occupancy(room=room_name, occupancy=occupancy))
            db.session.commit()
            return jsonify({'message': 'Data received, stored, and occupancy calculated successfully'}), 201
        else:
            # This 'else' should theoretically not be hit if the above checks are robust,
            # but it's a fallback for unexpected flow.
            return jsonify({'error': 'Internal server error: room or count not set after request processing.'}), 500

        
    @app.route('/room/<room>')
    def room(room):
        """
        Renders the page for a specific room, displaying its recording history and current occupancy.

        It retrieves all recordings and occupancy data for the given room from the database.
        It also fetches the room's capacity and calculates the estimated number of people
        based on the latest occupancy reading.

        Args:
            room (str): The name of the room to display.

        Returns:
            flask.Response: The rendered 'room.html' template with the room's data.
        """
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
        
        occupancy_percent = None
        if highest_occupancy_result and room_info_result and room_info_result.capacity > 0:
            occupancy_percent = round(highest_occupancy_result.occupancy * 100, 2)

        people = None
        if highest_occupancy_result and room_info_result:
            people = highest_occupancy_result.occupancy * room_info_result.capacity

        return render_template(
            "room.html",
            recordings=recordings,
            occupancies=occupancies,
            room=room,
            people=people,
            room_info=room_info_result,
            occupancy_percent=occupancy_percent
        )

