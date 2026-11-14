#pragma once

#include <Arduino.h>
#include <PubSubClient.h>

/**
 * @brief Publishes messages to an MQTT broker, reconnecting at a limited rate when the session drops.
 */
class MqttPublisher
{
public:
  /**
   * @brief Configures the broker and the client identity.
   *
   * @param networkClient TCP client used to reach the broker.
   * @param host Broker IP address or host name.
   * @param port Broker TCP port.
   * @param clientId MQTT client identifier, unique on the broker.
   * @param user User name, or an empty string for anonymous access.
   * @param password Password, ignored when user is empty.
   * @param retryIntervalMs Minimum time between connection attempts in milliseconds.
   */
  MqttPublisher(Client &networkClient, const char *host, uint16_t port, const char *clientId,
                const char *user, const char *password, uint32_t retryIntervalMs);

  /**
   * @brief Sets the broker address on the MQTT client.
   */
  void begin();

  /**
   * @brief Services the MQTT session and reconnects when due. Call only while the network is up.
   *
   * @return true if connected to the broker.
   */
  bool maintain();

  /**
   * @brief Publishes a message with QoS 0.
   *
   * @param topic Destination topic.
   * @param payload Message text.
   * @return true if the message was handed to the network.
   */
  bool publish(const char *topic, const char *payload);

private:
  PubSubClient client;
  const char *host;
  uint16_t port;
  const char *clientId;
  const char *user;
  const char *password;
  uint32_t retryIntervalMs;
  uint32_t lastAttemptMs = 0;
  bool hasAttempted = false;

  /**
   * @brief Tries to open the MQTT session if retryIntervalMs passed since the last attempt.
   */
  void reconnect_if_due();
};
