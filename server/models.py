from app import db
from sqlalchemy.orm import Mapped, mapped_column
from datetime import datetime
from sqlalchemy import func

# Table representing the data collected by the atom-lites
class BLEDevices(db.Model):
    __tablename__ = 'bledevices' # name of he database table
    
    id: Mapped[int] = mapped_column(primary_key=True)
    time: Mapped[datetime] = mapped_column(server_default=func.current_timestamp())
    room: Mapped[str]
    count: Mapped[int]

    def __repr__(self):
        return f'id:{self.id}, time: {self.time}, room: {self.room}, count: {self.count}'
    
# Table representing historical room occupancy
# class Occupancy(db.Model):
#     id: Mapped[int] = mapped_column(primary_key=True)
#     time: Mapped[datetime] = mapped_column(server_default=func.current_timestamp())
#     occupancy: Mapped[float]

# NOTE: every time this scheme is updated, you have to run
#   flask db migrate
#   flask db upgrade
