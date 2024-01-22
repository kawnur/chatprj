insert into public.companions (name) values ('me');
insert into public.companions (name) values ('client1');
insert into public.companions (name) values ('client3');

insert into public.sockets (id, ipaddress, port) values (
        (select id from companions where name = 'client1'), '172.21.0.2', 5002);
insert into public.sockets (id, ipaddress, port) values (
        (select id from companions where name = 'client3'), '172.21.0.4', 5002);
