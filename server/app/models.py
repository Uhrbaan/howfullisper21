"""
This file defines the SQLAlchemy models representing the database tables used in the application.
It includes models for sensor recordings, static room information, and historical room occupancy data.
"""

from app.app import db
from sqlalchemy.orm import Mapped, mapped_column
from datetime import datetime
from sqlalchemy import func

# Table representing the data recorded by the atom-lites
class Recordings(db.Model):
    """
    Represents individual data recordings from the atom-lite devices.

    Attributes:
        id (int): Primary key for the recording.
        time (datetime): Timestamp of the recording, automatically set on creation.
        room (str): Identifier of the room where the recording was taken.
        count (int): Number of Bluetooth devices detected.
    """
    id: Mapped[int] = mapped_column(primary_key=True)
    time: Mapped[datetime] = mapped_column(server_default=func.current_timestamp())
    room: Mapped[str]
    count: Mapped[float]

    def __repr__(self):
        return f"[{self.id}][{self.time}] Room '{self.room}' contains {self.count} bluetooth devices."
    
# contains static information about the room
class RoomInfo(db.Model):
    """
    Stores static information about each room.

    Attributes:
        room (str): Primary key, the name or identifier of the room.
        capacity (int): The maximum capacity of the room when empty.
    """
    room: Mapped[str] = mapped_column(primary_key=True)
    capacity: Mapped[int] # number of places in the room when empty

    def __repr__(self):
        return f"Room '{self.room}' has a capacity of {self.capacity} people."

# Table representing historical room occupancy
class Occupancy(db.Model):
    """
    Represents historical occupancy data for each room.

    Attributes:
        id (int): Primary key for the occupancy record.
        time (datetime): Timestamp of the occupancy record, automatically set on creation.
        room (str): Identifier of the room.
        occupancy (float): The occupancy level of the room as a percentage (0.0 to 1.0).
    """
    id: Mapped[int] = mapped_column(primary_key=True)
    time: Mapped[datetime] = mapped_column(server_default=func.current_timestamp())
    room: Mapped[str]
    occupancy: Mapped[float]

    def __repr__(self):
        return f"[{self.id}][{self.time}] Room '{self.room}' is occupied to {self.occupancy:.0%}."