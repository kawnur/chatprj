insert into public.companions (name) values ('me');
insert into public.companions (name) values ('client_host');
insert into public.companions (name) values ('client1');
insert into public.companions (name) values ('client2');

insert into public.sockets (id, ipaddress, server_port, client_port) values (
        (select id from companions where name = 'me'), '0.0.0.0', 0, 0);
insert into public.sockets (id, ipaddress, server_port, client_port) values (
        (select id from companions where name = 'client_host'), '172.21.0.1', 5002, 5003);
insert into public.sockets (id, ipaddress, server_port, client_port) values (
        (select id from companions where name = 'client1'), '172.21.0.2', 5002, 5003);
insert into public.sockets (id, ipaddress, server_port, client_port) values (
        (select id from companions where name = 'client2'), '172.21.0.3', 5003, 5003);
