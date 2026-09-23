#!/bin/bash

source ./.env

# create volumes
#docker volume create chatapp_volume1
#docker volume create chatapp_volume2
#docker volume create chatapp_volume3

# create network
docker network create my-bridge-network --driver "bridge" --subnet $CHATAPP_BRIDGE_NETWORK_SUBNET

