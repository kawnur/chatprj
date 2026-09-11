#ifndef DB_CONSTANTS_HPP
#define DB_CONSTANTS_HPP

#include <map>
#include <string>
#include <vector>

enum class DBRequestType
{
    GET_COMPANIONS,
    GET_COMPANION_BY_NAME,
    GET_COMPANION_AND_SOCKET,
    GET_SOCKET_INFO,
    GET_SOCKET_BY_IP_ADDRESS_AND_PORT,
    GET_MESSAGES,
    GET_ALL_MESSAGES_BY_COMPANION_ID,
    GET_EARLY_MESSAGES_BY_MESSAGE_ID,
    GET_MESSAGE_BY_COMPANION_ID_AND_TIMESTAMP,
    GET_UNSENT_MESSAGES_BY_COMPANION_NAME,
    GET_PASSWORD,
    SET_MESSAGE_IS_SENT_AND_RETURN,
    SET_MESSAGE_IS_RECEIVED_AND_RETURN,
    PUSH_COMPANION_AND_RETURN,
    UPDATE_COMPANION_AND_RETURN,
    UPDATE_COMPANION_AND_SOCKET_AND_RETURN,
    PUSH_SOCKET_AND_RETURN,
    PUSH_MESSAGE_AND_RETURN,
    PUSH_MESSAGE_WITH_AUTHOR_ID_AND_RETURN,
    PUSH_PASSWORD_AND_RETURN,
    DELETE_MESSAGES_AND_RETURN,
    DELETE_COMPANION_AND_SOCKET_AND_RETURN
};

std::map<DBRequestType, std::vector<std::string>> dbRequestDataMap {
    {
        DBRequestType::GET_COMPANIONS,
        {
            "getCompanionsDBResult",            // log mark
            "SELECT {} FROM companions",  // request
            "id", "name"                        // reply data keys
        }
    },
    {
        DBRequestType::GET_COMPANION_BY_NAME,
        {
            "getCompanionByName",
            "SELECT {0} FROM companions WHERE name = '{1}'",
            "id"
        }
    },
    {
        DBRequestType::GET_COMPANION_AND_SOCKET,
        {
            "getCompanionAndSocket",
            "SELECT {0} FROM companions JOIN sockets ON companions.id = sockets.id "
            "WHERE companions.id = {1}",
            "companions.name", "sockets.ipaddress", "sockets.client_port"
        }
    },
    {
        DBRequestType::GET_SOCKET_INFO,
        {
            "getSocketInfo",
            "SELECT {0} FROM sockets WHERE id = {1}",
            "ipaddress", "server_port", "client_port"
        }
    },
    {
        DBRequestType::GET_SOCKET_BY_IP_ADDRESS_AND_PORT,
        {
            "getSocketByIpAddressAndPort",
            "SELECT {0} FROM sockets WHERE ipaddress = '{1}' AND client_port = '{2}'",
            "id"
        }
    },
    {
        DBRequestType::GET_MESSAGES,
        {
            "getMessages",
            "WITH select_id AS "
            "(SELECT id FROM companion_messages WHERE companion_id = {1} "
            "ORDER BY timestamp_tz DESC LIMIT {2}) "
            "SELECT {0} FROM companion_messages WHERE id IN (SELECT id FROM select_id) "
            "ORDER BY timestamp_tz ASC",
            "id", "companion_id", "author_id", "timestamp_tz", "message", "is_sent", "is_received"
        }
    },
    {
        DBRequestType::GET_ALL_MESSAGES_BY_COMPANION_ID,
        {
            "getMessages",
            "SELECT {0} "
            "FROM companion_messages WHERE companion_id = {1} "
            "ORDER BY timestamp_tz ASC",
            "author_id", "timestamp_tz", "message"
        }
    },
    {
        DBRequestType::GET_EARLY_MESSAGES_BY_MESSAGE_ID,
        {
            "getEarlyMessagesByMessageId",
            "WITH select_id AS "
            "(SELECT id FROM companion_messages WHERE companion_id = {1} AND id < {2} "
            "ORDER BY timestamp_tz DESC LIMIT {3}) "
            "SELECT {0} FROM companion_messages WHERE id IN (SELECT id FROM select_id) "
            "ORDER BY timestamp_tz ASC",
            "id", "companion_id", "author_id", "timestamp_tz", "message", "is_sent", "is_received"
        }
    },
    {
        DBRequestType::GET_MESSAGE_BY_COMPANION_ID_AND_TIMESTAMP,
        {
            "getEarlyMessagesByMessageId",
            "SELECT {0} FROM companion_messages WHERE companion_id = {1} AND timestamp_tz = '{2}'",
            "id"
        }
    },
    {
        DBRequestType::GET_UNSENT_MESSAGES_BY_COMPANION_NAME,
        {
            "getUnsentMessagesByCompanionName",
            "SELECT {0} "
            "FROM companion_messages WHERE companion_id = (SELECT id FROM companions WHERE name = '{1}') "
            "AND author_id = (SELECT id FROM companions WHERE name = 'me') "
            "AND is_sent IS false",
            "id", "author_id", "companion_id", "timestamp_tz", "message", "is_received"
        }
    },
    {
        DBRequestType::GET_PASSWORD,
        {
            "getPassword",
            "SELECT {0} FROM passwords",  // ???
            "id", "password"
        }
    },
    {
        DBRequestType::SET_MESSAGE_IS_SENT_AND_RETURN,
        {
            "setMessageIsSentAndReturn",
            "UPDATE messages SET is_sent = 'true' WHERE id = {0} RETURNING id",
            "id"
        }
    },
    {
        DBRequestType::SET_MESSAGE_IS_RECEIVED_AND_RETURN,
        {
            "setMessageIsReceivedAndReturn",
            "UPDATE messages SET is_received = 'true' WHERE id = {0} RETURNING id",
            "id"
        }
    },
    {
        DBRequestType::PUSH_COMPANION_AND_RETURN,
        {
            "pushCompanionAndReturn",
            "INSERT INTO companions (name) VALUES ('{1}') RETURNING id",  // !!!
            "id"
        }
    },
    // {
    //     DBRequestType::UPDATE_COMPANION_AND_RETURN,
    //     {
    //         "updateCompanionAndReturn",
    //         "INSERT INTO companions (name) VALUES ('{}') RETURNING id",  // ???
    //         "id"
    //     }
    // },
    {
        DBRequestType::UPDATE_COMPANION_AND_SOCKET_AND_RETURN,
        {
            "updateCompanionAndSocketAndReturn",
            "WITH update_name AS (UPDATE companions SET name = '{1}' WHERE id = {0} "  // ???
            "RETURNING id) UPDATE sockets SET ipaddress = '{3}', client_port = '{4}' "
            "WHERE id IN (SELECT id FROM update_name) RETURNING id",
            "id"
        }
    },
    {
        DBRequestType::PUSH_SOCKET_AND_RETURN,
        {
            "pushSocketAndReturn",
            "INSERT INTO sockets ({0}, ipaddress, server_port, client_port) "
            "VALUES ((SELECT id FROM companions WHERE name = '{1}'), '{2}', {3}, {4}) RETURNING id",
            "id"
        }
    },
    {
        DBRequestType::PUSH_MESSAGE_AND_RETURN,
        {
            "pushMessageAndReturn",
            "INSERT INTO messages "
            "(companion_id, author_id, timestamp_tz, message, is_sent, is_received) "
            "VALUES ((SELECT id FROM companions WHERE name = '{1}'), "
            "(SELECT id FROM companions WHERE name = '{2}'), '{3}', '{4}', {5}, {6}) "
            "RETURNING id, %7, timestamp_tz",  // ???
            "id", "companion_id", "timestamp_tz"
        }
    },
    {
        DBRequestType::PUSH_MESSAGE_WITH_AUTHOR_ID_AND_RETURN,
        {
            "pushMessageWithAuthorIdAndReturn",
            "INSERT INTO messages "
            "(companion_id, author_id, timestamp_tz, message, is_sent, is_received) "
            "VALUES ((SELECT id FROM companions WHERE name = '{1}'), {2}, '{3}', '{4}', {5}, {6}) "
            "RETURNING id, %7, timestamp_tz",
            "id", "companion_id", "timestamp_tz"
        }
    },
    {
        DBRequestType::PUSH_PASSWORD_AND_RETURN,
        {
            "pushPasswordAndReturn",
            "INSERT INTO passwords (password) VALUES ('{1}') RETURNING id",
            "id"
        }
    },
    {
        DBRequestType::DELETE_MESSAGES_AND_RETURN,
        {
            "deleteMessagesAndReturn",
            "DELETE FROM companion_messages WHERE companion_id = {0} RETURNING companion_id",
            "companion_id"
        }
    },
    {
        DBRequestType::DELETE_COMPANION_AND_SOCKET_AND_RETURN,
        {
            "deleteCompanionAndSocketAndReturn",
            "WITH delete_socket AS (DELETE FROM sockets WHERE id = {0} RETURNING id) "
            "DELETE FROM companions WHERE id IN (SELECT id FROM delete_socket) RETURNING id",
            "id"
        }
    }
};

#endif // DB_CONSTANTS_HPP
