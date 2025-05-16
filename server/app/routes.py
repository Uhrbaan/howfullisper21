"""
This file defines the routes and view functions for the Flask application.
It handles requests for the homepage, data collection from sensors, and displaying room-specific information.
"""
from flask import render_template, request, jsonify
from app.models import Recordings, Occupancy, RoomInfo
from markupsafe import escape
from sqlalchemy.sql import func

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
            if data is None:
                return jsonify({'error': 'Request body is not valid JSON'}), 400 # Added this check
            
            room = data.get('room')
            count = data.get('count')
            if room and isinstance(count, int):
                device = Recordings(room=room, count=count)
                db.session.add(device)
                db.session.commit()
                # Removed the occupancy calculation here, will move it to the end
            else:
                return jsonify({'error': 'Missing "room" or "count" in JSON data, or "count" is not an integer.'}), 400 # More specific error
                
        elif request.method == 'GET':
            data = request.args
            room = data.get('room') # Use .get() for safety
            try:
                count = int(data.get('count')) # Use .get() and try-except for safety
            except (TypeError, ValueError):
                return jsonify({'error': 'Invalid or missing "count" parameter.'}), 400
            
            if room: # Check if room is present, count is already handled by try-except
                # Record the raw data for GET requests too
                device = Recordings(room=room, count=count)
                db.session.add(device)
                db.session.commit()
                # Removed occupancy calculation here, will move it to the end
            else:
                return jsonify({'error': 'Invalid or missing "room" parameter.'}), 400

        # --- IMPORTANT: Occupancy calculation logic ---
        # This part will execute for both POST and GET after initial data handling
        # It needs to be conditional based on whether room/count were successfully extracted
        if room and isinstance(count, int): # Ensure room and count are set before proceeding
            query = db.select(RoomInfo.capacity).where(RoomInfo.room == room)
            capacity_result = db.session.execute(query).scalar_one_or_none() # Use scalar_one_or_none() for single value

            if capacity_result is None:
                return jsonify({'error': f'Room "{room}" not found in RoomInfo.'}), 404

            if count == 0: # Avoid division by zero
                occupancy_percentage = 0.0
            else:
                # You have 'occupancy = capacity / (count/3)'. This seems reversed.
                # Occupancy should be (current_people / capacity).
                # If 'count' is a sensor reading (e.g., people detected),
                # and 'capacity' is max people, you need a mapping.
                # Assuming 'count' roughly relates to people * 3 (as in your original formula):
                estimated_people = count / 3.0 # Ensure float division
                occupancy_percentage = estimated_people / capacity_result
                
            db.session.add(Occupancy(room=room, occupancy=occupancy_percentage))
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

