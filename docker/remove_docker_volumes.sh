#!/bin/bash

# remove containers
echo "##### Remove containers..."
docker container rm $(docker ps -aq)

# create volumes
echo "##### Create volumes..."
docker volume rm chatapp_volume1
docker volume rm chatapp_volume2
docker volume rm chatapp_volume3
docker volume rm docker_chatapp_volume1
docker volume rm docker_chatapp_volume2
docker volume rm docker_chatapp_volume3
