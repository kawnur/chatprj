insert into public.companions (name) values ('me');
insert into public.companions (name) values ('client1');
insert into public.companions (name) values ('client2');

insert into public.sockets (id, ipaddress, port) values (
        (select id from companions where name = 'me'), '0.0.0.0', 5002);
insert into public.sockets (id, ipaddress, port) values (
        (select id from companions where name = 'client1'), '172.21.0.2', 5002);
insert into public.sockets (id, ipaddress, port) values (
        (select id from companions where name = 'client2'), '172.21.0.3', 5002);
