from app import db
from sqlalchemy.orm import Mapped, mapped_column
from datetime import datetime
from sqlalchemy import func

# Table representing the data recorded by the atom-lites
class Recordings(db.Model):    
    id: Mapped[int] = mapped_column(primary_key=True)
    time: Mapped[datetime] = mapped_column(server_default=func.current_timestamp())
    room: Mapped[str]
    count: Mapped[int]

    def __repr__(self):
        return f"[{self.id}][{self.time}] Room '{self.room}' contains {self.count} bluetooth devices."
    
# contains static information about the room
class RoomInfo(db.Model):
    room: Mapped[str] = mapped_column(primary_key=True)
    capacity: Mapped[int] # number of places in the room when empty

    def __repr__(self):
        return f"Room '{self.room}' has a capacity of {self.capacity} people."

# Table representing historical room occupancy
class Occupancy(db.Model):
    id: Mapped[int] = mapped_column(primary_key=True)
    time: Mapped[datetime]
    room: Mapped[str]
    occupancy: Mapped[float]

    def __repr__(self):
        return f"[{self.id}][{self.time}] Room '{self.room}' is occupied to {self.occupancy:.0%}."


# NOTE: every time this scheme is updated, you have to run
#   flask db migrate
#   flask db upgrade

# NOTE: you can fill some random data with the following command:

# # Create some rooms
# INSERT INTO room_info (room, capacity) VALUES
# ('infolab 0', 20),
# ('infolab 1', 20),
# ('library', 100); # ...

# # Create some occupancy information
# INSERT INTO recordings (room, count) VALUES
# ('infolab 0', 22),
# ('infolab 1', 18); # ...

# # Calculate the occupancy in the databse
# INSERT INTO occupancy (time, room, occupancy) 
# SELECT r.time, r.room, CAST(r.count AS REAL) / ri.capacity
# FROM recordings r
# JOIN room_info ri ON r.room == ri.room