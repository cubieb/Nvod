
select * from referenceservice;
select * from referenceserviceevent;
select * from movie;
select * from movies_refevents;

delete from movies_refevents;
delete from movie;
delete from referenceserviceevent;
delete from referenceservice;

/*
INSERT INTO referenceservice (RefsId_TsId, RefsId_SvcId, Description) 
    VALUES (1, 1, 'hello reference service 1.');
    
INSERT INTO referenceserviceevent 
    (Idx, EventId, StartTimePoint, Duration, Refs_TsId, Refs_SvcId) 
    VALUES (1, 1, 1, 1, 1, 1);
INSERT INTO referenceserviceevent 
    (Idx, EventId, StartTimePoint, Duration, Refs_TsId, Refs_SvcId) 
    VALUES (2, 2, 1, 1, 1, 1);
    
INSERT INTO movie (MovieId, Description) VALUES (100, 'hello movie-100');
INSERT INTO movie (MovieId, Description) VALUES (101, 'hello movie-101');
INSERT INTO movies_refevents (MovieId, RefsEventIdx) VALUES (100, 1);
INSERT INTO movies_refevents (MovieId, RefsEventIdx) VALUES (100, 2);
INSERT INTO movies_refevents (MovieId, RefsEventIdx) VALUES (101, 1);
*/

