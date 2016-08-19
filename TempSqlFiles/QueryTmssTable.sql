
delete from timeshiftedserviceevent;
delete from timeshiftedservice;

INSERT INTO timeshiftedservice (`TmssId_TsId`, `TmssId_SvcId`, `Description`) 
    VALUES (1, 1, 'hello');
INSERT INTO timeshiftedserviceevent (`Idx`, `EventId`, `PosterId`, `StartTimePoint`, `Duration`, `Tmss_TsId`, `Tmss_SvcId`) 
    VALUES (1, 1, 1, 1, 1, 1, 1);

select * from timeshiftedservice;
select * from timeshiftedserviceevent;
