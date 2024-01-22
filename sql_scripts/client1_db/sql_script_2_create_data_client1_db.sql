insert into public.companions (name) values ('me');
insert into public.companions (name) values ('client2');
insert into public.companions (name) values ('client3');

insert into public.sockets (id, ipaddress, port) values (
	(select id from companions where name = 'client2'), '172.21.0.3', 5002);
insert into public.sockets (id, ipaddress, port) values (
	(select id from companions where name = 'client3'), '172.21.0.4', 5002);
