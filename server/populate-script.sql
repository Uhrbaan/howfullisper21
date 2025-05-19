-- partially ai-generated
-- this document can be used to generate som random filler data in the database.
-- You can use any SQLite client, I personnally recommand DBeaver community, 
-- which is a GUI that works with nearly all databases.

INSERT INTO room_info (room, capacity) VALUES
('infolab0', 30),
('infolab1', 20),
('library', 100);

INSERT INTO recordings (room, count, time) VALUES
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-55 minutes')),
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-50 minutes')),
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-45 minutes')),
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-40 minutes')),
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-35 minutes')),
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-30 minutes')),
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-25 minutes')),
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-20 minutes')),
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-15 minutes')),
('infolab0', ABS(RANDOM() % 35), strftime('%Y-%m-%d %H:%M:%S', 'now', '-10 minutes'));

INSERT INTO recordings (room, count, time) VALUES
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-55 minutes')),
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-50 minutes')),
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-45 minutes')),
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-40 minutes')),
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-35 minutes')),
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-30 minutes')),
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-25 minutes')),
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-20 minutes')),
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-15 minutes')),
('infolab1', ABS(RANDOM() % 25), strftime('%Y-%m-%d %H:%M:%S', 'now', '-10 minutes'));

INSERT INTO recordings (room, count, time) VALUES
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-55 minutes')),
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-50 minutes')),
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-45 minutes')),
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-40 minutes')),
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-35 minutes')),
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-30 minutes')),
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-25 minutes')),
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-20 minutes')),
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-15 minutes')),
('library', ABS(RANDOM() % 105), strftime('%Y-%m-%d %H:%M:%S', 'now', '-10 minutes'));

INSERT INTO occupancy (time, room, occupancy)
SELECT r.time, r.room, CAST(r.count AS REAL) / ri.capacity
FROM recordings r
JOIN room_info ri ON r.room = ri.room;